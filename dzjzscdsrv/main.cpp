#include <semaphore.h>

#include <QDir>
#include <QTextCodec>

#include "dflogger.h"
#include "dzjzactionstatis.h"
#include "dzjzcalctool.h"
#include "dzjzcommon.h"
#include "dzjzevent.h"
#include "dzjzinspect.h"
#include "dzjzrounditem.h"
#include "dzjzsrv.h"
#include "netdef.h"
#include "procthrd.h"
#include "pthread.h"
#include "rdbscd.h"
#include "sybase.h"
#include "unistd.h"
#include "dzjzupload.h"

sem_t semCalc; // 控制proc_cut_load_calc_thread线程是否开启
extern DZJZ_Event dzjzEnt;

// 定义全局列集
static std::vector<char *> dzjzrounditemcol = {
    "id", "strapreal", "strapjudge", "realvalue", "realtime", "valuejudge", "pvalue", "activejudge", "planjudge",
    "funcjudge", "judgepower", "breakstate", "linkedbreak", "devalarm", "standbypower", "planstandbypwr"};

static std::vector<char *> dzjzroundcol = {
    "id", "judgepower", "planpower", "requirepower", "issuepower", "standbypower", "planstandbypwr", "judgerequire",
    "lastalarm"};

static std::vector<char *> dzjzroundstacol = {
    "id", "judgepower", "planpower", "realfeedernum", "planfeedernum", "realfeederrate", "planfeederrate", "requirepower",
    "standbypower", "planstandbypwr", "realcomprate", "plancomprate", "realpowerrate", "planpowerrate", "realstandfn",
    "planstandfn"};

static std::vector<char *> dzjzareacol = {
    "id", "judgepower", "planpower", "realfeedernum", "planfeedernum", "realfeederrate", "planfeederrate", "requirepower",
    "standbypower", "planstandbypwr", "realcomprate", "plancomprate", "realpowerrate", "planpowerrate", "realstandfn",
    "planstandfn"};

static std::vector<char *> dzjzptaskcol = {
    "id", "lastinspect"};

/**
 * @brief RDB数据同步函数
 *
 * @param tableName 需要同步的表名
 * @param cols 需要同步的列集
 */
void rdbdatasync(char *tableName, std::vector<char *> cols)
{
    // 定义各种所需变量
    int i, rcdnum, ret;
    int id;
    Vartype_value vtvalue;

    // 创建RDB表和查询对象
    Rsqlclient rsqlcli;
    RdbSpeedyQuery spmemtbl(MyUserName, MyPassWord);
    RdbTable rounditemtable(MyUserName, MyPassWord);

    // 尝试打开RDB表
    if (rounditemtable.OpenTable(tableName) != RDB_OK)
        return;

    // 选择要查询的表和列
    rsqlcli.selectfrom(tableName);
    for (const auto &col : cols)
        rsqlcli.selectattr(col);

    // 执行查询
    ret = rsqlcli.select(MyUserName, MyPassWord);

    // 如果查询成功，执行同步操作
    if (ret == RDB_OK)
    {
        rcdnum = rsqlcli.getrcdcnt();
        for (i = 0; i < rcdnum; i++)
        {
            id = rsqlcli.get_intval(i, "id");
            for (const auto &col : cols)
            {
                ret = rsqlcli.GetColumnValue(i, col, &vtvalue);
                spmemtbl.PutColumnValue(tableName, &id, col, &vtvalue);
            }
        }
    }
}

/**
 * @brief 初始化
 *
 */
void init_dzjzproc(void)
{
    DFLOG_INFO("dzjz_server: init_dzjzproc start");

    // 如果不是主服务器，执行同步操作
    if (!ismainserver())
    {
        rdbdatasync(DZJZ_ROUND_ITEM_TBLNAME, dzjzrounditemcol);
        rdbdatasync(DZJZ_ROUND_TBLNAME, dzjzroundcol);
        rdbdatasync(DZJZ_AREA_TBLNAME, dzjzareacol);
        rdbdatasync(DZJZ_ROUND_STATIC_TBLNAME, dzjzroundstacol);
        rdbdatasync(DZJZ_PERIOD_INSPECT_TBLNAME, dzjzptaskcol);
    }

    DFLOG_INFO("dzjz_server: init_dzjzproc end");
}

/**
 * @brief Get the Devic Info By Action I D object
 *
 * @param actionID
 * @param deviceID
 * @param deviceName
 */
int getDevicInfoByActionID(char *actionID, int &deviceID, std::string &deviceName)
{
    RdbSQL sql;
    sql.selectfrom(DZJZ_ROUND_ITEM_TBLNAME);
    sql.selectattr("deviceid");
    sql.selectattr("devicename");
    sql.where("actionid", DATATYPE_STRING, actionID);
    int nRet = sql.select(MyUserName, MyPassWord);
    if (nRet != OK)
    {
        DFLOG_ERROR("failed to execute sql, nRet=%d actionid=%s", nRet, actionID);
        return -1;
    }

    int nRcdCnt = sql.getrcdcnt();
    if (nRcdCnt <= 0)
    {
        DFLOG_ERROR("no data actionid=%s", actionID);
        return -2;
    }

    deviceID = sql.get_intval(0, "deviceid");
    deviceName = sql.get_string(0, "devicename");

    return 0;
}

/**
 * @brief 存盘
 *
 * @param clockp
 */
void saveData(SYS_CLOCK *pClock)
{
    pClock->second = 0;
    intertime time = 0;
    convert_clock(pClock, &time);

    DFLOG_INFO("定时存盘: %04d-%02d-%02d %02d:%02d:%02d", pClock->year, pClock->month, pClock->day, pClock->hour, pClock->minute, pClock->second);

    // 轮次项历史值表
    std::vector<TDZJZ_ROUNDITEM_HISTORY> roundItemHisList;
    readTable<TDZJZ_ROUNDITEM, TDZJZ_ROUNDITEM_HISTORY>(DZJZ_ROUND_ITEM_TBLNAME, time, roundItemHisList);
    saveTable<TDZJZ_ROUNDITEM_HISTORY>(roundItemHisList, "xopenshdb.dbo.低周减载轮次项历史值表");

    // 轮次历史值表
    std::vector<TDZJZ_ROUND_HISTORY> roundHisList;
    readTable<TDZJZ_ROUND, TDZJZ_ROUND_HISTORY>(DZJZ_ROUND_TBLNAME, time, roundHisList);
    saveTable<TDZJZ_ROUND_HISTORY>(roundHisList, "xopenshdb.dbo.低周减载轮次历史值表");

    // 轮次类型历史值表
    std::vector<TDZJZ_ROUNDSTA_HISTORY> roundStaHisList;
    readTable<TDZJZ_ROUNDSTA, TDZJZ_ROUNDSTA_HISTORY>(DZJZ_ROUND_STATIC_TBLNAME, time, roundStaHisList);
    saveTable<TDZJZ_ROUNDSTA_HISTORY>(roundStaHisList, "xopenshdb.dbo.低周减载轮次类型历史值表");

    // 区域历史值表
    std::vector<TDZJZ_AREA_HISTORY> areaHisList;
    readTable<TDZJZ_AREA, TDZJZ_AREA_HISTORY>(DZJZ_AREA_TBLNAME, time, areaHisList);
    saveTable<TDZJZ_AREA_HISTORY>(areaHisList, "xopenshdb.dbo.低周减载区域历史值表");

    DFLOG_INFO("定时存盘结束");
}

/**
 * @brief 正常变位回调
 *
 * @param apentcode 事件类型
 * @param apentclock 时间
 * @param apentmsg 事件结构
 * @param argp 传入参数指针
 * @return int
 */
int analyse_stachg(int apentcode, SYS_CLOCK *apentclock, void *apentmsg, void *argp)
{
    APENTMSG_STACHG *stachgmsg;
    stachgmsg = (APENTMSG_STACHG *)apentmsg;
    DZJZ_ActionStatis *actionStatis = (DZJZ_ActionStatis *)argp;

    if (apentcode == APENTCODE_STACHG)
    {
        if (stachgmsg->staval == STAVAL_ON)
        {
            DFLOG_INFO("收到合位信息: objectname=%s maketype=%d", stachgmsg->objectname, stachgmsg->maketype);

            if (stachgmsg->maketype < STACHG_MAKETYPE_NOTMAKEENT)
                return OK;

            // 根据动作信号获取轮次项
            std::vector<TDZJZ_ROUNDITEM> roundItemVec;
            if (actionStatis->getRoundItemVec(stachgmsg->objectname, roundItemVec) < 0)
            {
                DFLOG_ERROR("获取轮次项失败: objectname=%s", stachgmsg->objectname);
                return -1;
            }

            // 将轮次项添加到缓存
            for (const auto &item : roundItemVec)
            {
                actionStatis->addRoundItem(item);
            }

            int deviceID = 0;
            std::string deviceName = "";
            if (getDevicInfoByActionID(stachgmsg->objectname, deviceID, deviceName) < 0)
            {
                DFLOG_ERROR("failed to get device info, actionid=%s", stachgmsg->objectname);
                return -2;
            }

            dzjzEnt.make_action_event(stachgmsg->chgclock, deviceID, deviceName, roundItemVec, STAVAL_ON);
        }

        return OK;
    }
}

/**
 * @brief 自动研判分析线程
 *
 */
#ifdef __unix
void *proc_auto_judge_thread(void *p)
#else
UINT proc_auto_judge_thread(LPVOID pParam)
#endif
{
    DFLOG_INFO("dzjz_server: proc_auto_judge_thread start");

    int loop_count = 0;
    APEvent apevent;
    // 低周减载自动研判;
    DZJZ_RoundItem roundItem;
    DZJZ_ActionStatis actionStatis;
    actionStatis.startActionTimer();

    apevent.addapent_callback(APENTCODE_STACHG, analyse_stachg, &actionStatis, NULL); // 监视变位事项
    for (;;)
    {
        // DFLOG_DEBUG("dzjz_server: proc_auto_judge_thread loop start");
        apevent.apentloop(1);
        if (loop_count % 10 == 0) // 1秒
        {
            roundItem.freshData(); // 自动研判分析
        }
        sem_post(&semCalc);
        loop_count++;
        sleepms(200);
        // DFLOG_DEBUG("dzjz_server: proc_auto_judge_thread loop end");
    }

    DFLOG_INFO("dzjz_server: proc_auto_judge_thread end");
    actionStatis.stopActionTimer();

    return NULL;
}

/**
 * @brief 投运切荷量计算线程
 *
 */
#ifdef __unix
void *proc_cut_load_calc_thread(void *p)
#else
UINT proc_cut_load_calc_thread(LPVOID pParam)
#endif
{
    DFLOG_INFO("dzjz_server: proc_cut_load_calc_thread start");

    int loop_count = 0;

    DZJZ_CalcTool calcTool;

    for (;;)
    {
        sem_wait(&semCalc);
        // DFLOG_DEBUG("dzjz_server: proc_cut_load_calc_thread loop start");
        loop_count++;
        if (loop_count % 5 == 0) // 1秒
        {
            calcTool.Calc();
        }
        // DFLOG_DEBUG("dzjz_server: proc_cut_load_calc_thread loop end");
        sleepms(200);
    }

    DFLOG_INFO("dzjz_server: proc_cut_load_calc_thread end");

    return NULL;
}

/**
 * @brief 投运切荷量计算线程
 *
 */
#ifdef __unix
void *proc_period_inspect_thread(void *p)
#else
UINT proc_period_inspect_thread(LPVOID pParam)
#endif
{
    DFLOG_INFO("dzjz_server: proc_period_inspect_thread start");

    DZJZ_Inspect periodInspect;
    periodInspect.start();

    DFLOG_INFO("dzjz_server: proc_period_inspect_thread end");

    return NULL;
}

/**
 * @brief 周期上送线程
 *
 */
#ifdef __unix
void *proc_period_upload_thread(void *p)
#else
UINT proc_period_upload_thread(LPVOID pParam)
#endif
{
    DFLOG_INFO("dzjz_server: proc_period_upload_thread start");

    DZJZ_Upload periodUpload;
    periodUpload.start();

    DFLOG_INFO("dzjz_server: proc_period_upload_thread end");

    return NULL;
}

/**
 * @brief 父进程检测
 *
 */
#ifdef __unix
void *proc_checkparent_thread(void *p)
#else
UINT proc_checkparent_thread(LPVOID pParam)
#endif
{
    DFLOG_INFO("dzjz_server: proc_checkparent_thread start");

    proc_man procman;

    for (;;)
    {
        if (!procman.CheckParentProc())
        {
            DFLOG_INFO("dzjz_server: procman.CheckParentProc returning 0");
            exit(1);
        }
        sleepms(1, 0);
    }

    DFLOG_INFO("dzjz_server: proc_checkparent_thread end");

    return NULL;
}

/**
 * @brief 存盘线程
 *
 */
#ifdef __unix
void *proc_save_proc(void *p)
#else
UINT proc_save_proc(LPVOID pParam)
#endif
{
    DFLOG_INFO("dzjz_server: proc_save_proc start");

    constexpr auto interval = 60;

    SYS_CLOCK curclock;
    while (true)
    {
        get_curclock(&curclock);
        if (curclock.minute % 5 == 0)
        {
            if (ismainserver())
            {
                // 执行存盘
                saveData(&curclock);
            }
        }

        sleepms(interval, 0);
    }

    DFLOG_INFO("dzjz_server: proc_save_proc end");

    return NULL;
}

int main(int argc, char **argv)
{
    // 中文编码
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312")); // 设置缺省解码字体类型
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));   // 设置缺省解码字体类型
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));       //

    // 日志
    DFLogger::instance()->setOStreamAppender();
    QString strLogPath = QString("%1/log/dzjzsrv").arg(qgetenv(RUN_HOME).data());
    QDir logDir;
    if (!logDir.exists(strLogPath))
    {
        bool res = logDir.mkdir(strLogPath);
        if (!res)
        {
            DFLOG_WARN("Failed to create directory %s",
                       strLogPath.toLocal8Bit().data());
            return 0;
        }
    }
    strLogPath += "/DZJZ_Server.log";
    DFLOG_INFO("日志路径: %s", strLogPath.toLocal8Bit().data());

    DFLogger::instance()->setRollingFileAppender(strLogPath.toStdString());

    DFLOG_INFO("#######################DZJZ_Server start##########################");

    proc_man procman;

    if (procman.is_proc_exist("DZJZ_Server"))
    {
        DFLOG_INFO("DZJZ_Server have already exist!");
        return FALSE;
    }
    procman.start_proc("DZJZ_Server", "电网第三道防线服务", "", NULL);

    if (!procman.CheckParentProc())
    {
        DFLOG_INFO("DZJZ_Server: procman.CheckParentProc returning 0");
        exit(1);
    }

    // 启动历史数据库
    int m;

    for (m = 0; m < 4; m++)
    {
        if (startdb(1, 1) == CS_SUCCEED)
            break;
        usleep(2000 * 1000);
    }
    if (m >= 4)
        return 1;

    sem_init(&semCalc, 0, 0);

    init_dzjzproc();

    // scd_pthread_create(proc_mail_thread, NULL, 100);
    scd_pthread_create(proc_auto_judge_thread, NULL, 100);
    scd_pthread_create(proc_cut_load_calc_thread, NULL, 100);
    scd_pthread_create(proc_period_inspect_thread, NULL, 100);
    scd_pthread_create(proc_period_upload_thread, NULL, 100);

    // 启动检测父进程线程
    scd_pthread_create(proc_checkparent_thread, NULL, 100);

    // 启动存盘线程
    proc_save_proc(NULL);

    closedb();
    sem_destroy(&semCalc);

    DFLOG_INFO("###################DZJZ_Server end#########################");

    return 0;
}