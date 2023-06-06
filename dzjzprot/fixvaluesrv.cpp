/**
 * @file fixvaluesrv.cpp
 * @author 巩拓
 * @brief 本文件为定值召唤服务类的实现文件
 * @version 0.1
 * @date 2023-04-21
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "fixvaluesrv.h"

#include <signal.h>

#include <QDateTime>

#include "dzjzinspect.h"
#include "dzjzsrv.h"
#include "eventdef.h"

/**
 * @brief 构造函数
 *
 */
CFixValueService::CFixValueService()
{
    m_pApp = new CProtApp;
    m_poCallProtocol = NULL;
    m_nRtuNo = 0;

#ifdef __unix
    sigset(SIGUSR2, SIG_IGN);
#endif
}

/**
 * @brief 析构函数
 *
 */
CFixValueService::~CFixValueService()
{
    if (m_pApp != NULL)
    {
        delete m_pApp;
        m_pApp = NULL;
    }
}

/**
 * @brief 批量召唤
 *
 * @return int
 */
int CFixValueService::executeBatch()
{
    std::vector<DZJZ_DEVICE> listDevice = GetDZJZDeviceList();

    int nRet = FIX_CALL_SUCCESS;

    DFLOG_INFO("[定值批量召唤]: ------------start---------------");

    for (int i = 0; i < listDevice.size(); i++)
    {
        executeSingle(listDevice[i].rtuno);
    }

    reloadRTDB();

    DFLOG_INFO("[定值批量召唤]: ----------------end-----------------");

    return nRet;
}

/**
 * @brief 按条件召唤
 *
 * @param strPara
 * @param progressCallback
 * @return std::string
 */
std::string CFixValueService::executeInspectionByCond(const std::string &strPara, ProgressCallback progressCallback)
{
    DFLOG_INFO("[装置巡检]: ------------start---------------");

    dfJson::Value jProgress;

    // 解析参数
    dfJson::Value jPara;
    dfJson::CharReaderBuilder readerBuilder;
    std::istringstream iss(strPara);
    dfJson::parseFromStream(readerBuilder, iss, &jPara, nullptr);

    QString station = jPara["station"].asString().c_str();
    int areaId = jPara["area"].asInt();
    int roundType = jPara["round"].asInt();

    // 获取RTU
    std::vector<DZJZ_DEVICE> listDevice = GetDZJZDeviceList(station, areaId, roundType);
    DFLOG_INFO("[装置巡检]:获取到%d个召唤设备", listDevice.size());

    // 依次召唤定值
    for (int i = 0; i < listDevice.size(); i++)
    {
        jProgress["optype"] = "callDZ";
        jProgress["progress"] = (i + 1) * 60 / listDevice.size();
        jProgress["desc"] = QString("召唤装置[%1]%2定值").arg(listDevice[i].rtuno).arg(listDevice[i].desc).toAscii().data();
        progressCallback(jsonToString(jProgress));

        executeSingle(listDevice[i].rtuno);
        jProgress["optype"] = "callDZ";
        jProgress["progress"] = (i + 1) * 60 / listDevice.size();
        jProgress["desc"] = QString("召唤装置[%1]%2定值 成功").arg(listDevice[i].rtuno).arg(listDevice[i].desc).toAscii().data();
        progressCallback(jsonToString(jProgress));
    }

    if (listDevice.size() > 0)
    {
        jProgress["optype"] = "callDZ";
        jProgress["progress"] = 70;
        jProgress["desc"] = "加载实时库...";
        progressCallback(jsonToString(jProgress));
        reloadRTDB();
        usleep(5000 * 1000);
    }

    // 执行巡检
    jProgress["optype"] = "callDZ";
    jProgress["progress"] = 80;
    jProgress["desc"] = "巡检装置...";
    progressCallback(jsonToString(jProgress));

    DZJZ_Inspect inspect;
    dfJson::Value jRes;
    inspect.doInspect(station.toAscii().data(), areaId, roundType, jRes);

    jProgress["optype"] = "callDZ";
    jProgress["progress"] = 100;
    jProgress["desc"] = "巡检完成";
    progressCallback(jsonToString(jProgress));

    progressCallback(jsonToString(jRes));

    DFLOG_INFO("[装置巡检]: ----------------end-----------------");

    // 返回结果
    return jsonToString(jRes);
}

/**
 * @brief 等待回复邮件
 *
 * @param timeout
 * @return int
 */
int CFixValueService::waitForReply(int timeout)
{
    time_t startTime = time(NULL);
    time_t currentTime;

    while (true)
    {
        m_poCallProtocol->OnTimer(1);

        if (m_poCallProtocol->isRecvOver())
        {
            return FIX_CALL_SUCCESS;
        }

        currentTime = time(NULL);

        if ((currentTime - startTime) >= timeout)
        {
            DFLOG_INFO("[定值召唤]：等待超时");
            return FIX_CALL_OVERTIME;
        }

        usleep(200000);
    }

    return FIX_CALL_OVERTIME;
}

/**
 * @brief 单次执行
 *
 * @param nRtuNo
 * @return int
 */
int CFixValueService::executeSingle(int nRtuNo)
{
    int nRet = FIX_CALL_SUCCESS;
    QString logMsg;

    DFLOG_INFO("[定值召唤]: RTU[%d]", nRtuNo);

    m_nRtuNo = nRtuNo;

    // 刷新RTU保护参数
    m_pApp->refreshProtList(nRtuNo);

    // 召唤定值区
    nRet = CallAreaNo();

    if (nRet != FIX_CALL_SUCCESS)
    {
        logMsg = PrintFixCallErr(nRet);
        CreateEvent(EVENT_SCADA, ENTTYPE_DZJZ, 1, logMsg, QString::number(m_nRtuNo));
        DFLOG_INFO("%s", logMsg.toAscii().constData());

        return nRet;
    }

    usleep(500000);

    // 召唤定值
    nRet = CallDZ(m_pApp->m_nRunAreaNo);
    if (nRet != FIX_CALL_SUCCESS)
    {
        logMsg = PrintFixCallErr(nRet);
        CreateEvent(EVENT_SCADA, ENTTYPE_DZJZ, 1, logMsg, QString::number(m_nRtuNo));
        DFLOG_INFO("%s", logMsg.toAscii().constData());
        return nRet;
    }
    usleep(500000);

    // 召唤软压板
    nRet = CallRYB();

    logMsg = PrintFixCallErr(nRet);
    CreateEvent(EVENT_SCADA, ENTTYPE_DZJZ, 1, logMsg, QString::number(m_nRtuNo));
    DFLOG_INFO("%s", logMsg.toAscii().constData());

    usleep(500000);

    return nRet;
}

/**
 * @brief 根据nRet的值生成相应的日志消息
 *
 * @param nRet
 * @return std::string
 */
QString CFixValueService::PrintFixCallErr(int nRet)
{
    QString logMessage;

    switch (nRet)
    {
    case FIX_CALL_SUCCESS:
        logMessage = QString("[定值召唤 RTU:%1]:成功").arg(m_nRtuNo);
        break;
    case FIX_CALL_ERROR:
        logMessage = QString("[定值召唤 RTU:%1]:失败").arg(m_nRtuNo);
        break;
    case FIX_CALL_OVERTIME:
        logMessage = QString("[定值召唤 RTU:%1]:超时").arg(m_nRtuNo);
        break;
    case FIX_CALL_SEND_ERROR:
        logMessage = QString("[定值召唤 RTU:%1]:邮件发送失败").arg(m_nRtuNo);
        break;
    default:
        logMessage = QString("[定值召唤 RTU:%1]:未知错误").arg(m_nRtuNo);
        break;
    }

    return logMessage;
}

/**
 * @brief 召唤定值区
 *
 * @return int
 */
int CFixValueService::CallAreaNo()
{
    DFLOG_INFO("[定值召唤]:召唤定值区号");

    int nRet = FIX_CALL_SUCCESS;

    m_poCallProtocol = m_pApp->SwitchPcProt(typeYXQ);

    if (m_poCallProtocol == NULL)
    {
        DFLOG_ERROR("[定值召唤]: 未找到规约对象");
        return FIX_CALL_ERROR;
    }

    m_poCallProtocol->SetSrvData(&m_pApp->m_exchange, &m_pApp->m_mailBox, m_pApp->m_pFixedValue);
    m_pApp->m_nRunAreaNo = 0;

    // 发送召唤定值区邮件
    nRet = SendCallAreaNoMail();
    if (nRet < 0)
    {
        return nRet;
    }

    // 等待邮件回复
    nRet = waitForReply(FIX_CALL_WAIT_TIME);

    if (nRet == FIX_CALL_SUCCESS)
    {
        m_pApp->m_nRunAreaNo = m_poCallProtocol->GetCurAreaNo();
    }

    return nRet;
}

/**
 * @brief 召唤定值
 *
 * @param iAreaNo
 * @return int
 */
int CFixValueService::CallDZ(int iAreaNo)
{
    DFLOG_INFO("[定值召唤]:召唤定值 %d", iAreaNo);

    int nRet = FIX_CALL_SUCCESS;

    m_poCallProtocol = m_pApp->SwitchPcProt(typeDZ);

    if (m_poCallProtocol == NULL)
    {
        DFLOG_ERROR("[定值召唤]: 未找到规约对象");
        return FIX_CALL_ERROR;
    }

    m_poCallProtocol->SetSrvData(&m_pApp->m_exchange, &m_pApp->m_mailBox, m_pApp->m_pFixedValue);

    // 发送召唤定值区邮件
    nRet = SendCallSetValueMail(iAreaNo);
    if (nRet < 0)
    {
        return nRet;
    }

    // 等待邮件回复
    nRet = waitForReply(FIX_CALL_WAIT_TIME);

    if (nRet == FIX_CALL_SUCCESS)
    {
        m_poCallProtocol->SaveValueToDB();
    }

    return nRet;
}

/**
 * @brief 召唤软压板
 *
 * @return int
 */
int CFixValueService::CallRYB()
{
    DFLOG_INFO("[定值召唤]:召唤软压板");

    int nRet = FIX_CALL_SUCCESS;
    m_pApp->m_nRunAreaNo = 0;

    m_poCallProtocol = m_pApp->SwitchPcProt(typeRYB);

    if (m_poCallProtocol == NULL)
    {
        DFLOG_ERROR("[定值召唤]: 未找到规约对象");
        return FIX_CALL_ERROR;
    }

    m_poCallProtocol->SetSrvData(&m_pApp->m_exchange, &m_pApp->m_mailBox, m_pApp->m_pFixedValue);

    // 发送召唤定值区邮件
    nRet = SendCallRYBMail();
    if (nRet < 0)
    {
        return nRet;
    }

    // 等待邮件回复
    nRet = waitForReply(FIX_CALL_WAIT_TIME);

    if (nRet == FIX_CALL_SUCCESS)
    {
        m_poCallProtocol->SaveValueToDB();
    }

    return nRet;
}

/**
 * @brief 召唤装置定值区号
 *
 * @param szDevName 装置代码
 * @param iBoardNo 板号
 * @param iAreaNo 区号
 * @return true
 * @return false
 */
int CFixValueService::SendCallAreaNoMail()
{
    int iRet = m_poCallProtocol->SendCallAreaNoMail();
    if (iRet < 0)
    {
        DFLOG_ERROR("[定值区号召唤]: 邮件发送失败");
        return FIX_CALL_ERROR;
    }
    DFLOG_INFO("[定值区号召唤]: 邮件发送成功");
    return FIX_CALL_SUCCESS;
}

/**
 * @brief 召唤定值
 *
 * @param iAreaNo
 * @return int
 */
int CFixValueService::SendCallSetValueMail(int iAreaNo)
{
    int iRet = m_poCallProtocol->SendCallValueMail(iAreaNo);
    if (iRet < 0)
    {
        DFLOG_ERROR("[定值召唤]: 邮件发送失败");
        return FIX_CALL_ERROR;
    }

    DFLOG_INFO("[定值召唤]: 邮件发送成功");
    return FIX_CALL_SUCCESS;
}

/**
 * @brief 召唤软压板
 *
 * @return int
 */
int CFixValueService::SendCallRYBMail()
{
    int iRet = m_poCallProtocol->SendCallRYBMail();
    if (iRet < 0)
    {
        DFLOG_ERROR("[软压板召唤]: 邮件发送失败");
        return FIX_CALL_ERROR;
    }
    DFLOG_INFO("[软压板召唤]: 邮件发送成功");
    return FIX_CALL_SUCCESS;
}

/**
 * @fn		void CFixValueThread::InitDayCallList()
 * @brief	初始化日执行列表
 *
 * @param	无
 * @return	无
 * @see
 *
 */
std::vector<DZJZ_DEVICE> CFixValueService::GetDZJZDeviceList()
{
    DZJZ_DEVICE *pBuf = NULL;
    std::vector<DZJZ_DEVICE> deviceList;

    // 从数据库中获取所有的定值召唤设备
    int nRet = m_pApp->m_baseDB.Isql("select 编号,名称 from xopensdb.dbo.低周减载装置参数表", (void **)&pBuf);

    if (nRet > 0)
    {
        deviceList.reserve(nRet); // 预先分配内存
        deviceList.assign(pBuf, pBuf + nRet);

        for (int i = 0; i < deviceList.size(); i++)
        {
            DFLOG_DEBUG("%d|%s", deviceList[i].rtuno, deviceList[i].desc);
        }
    }

    DFLOG_INFO("[定值召唤]:获取到%d个定值召唤设备", deviceList.size());

    return deviceList;
}

/**
 * @brief 获取定值召唤设备列表
 *
 * @param subStation 所属厂站
 * @param area 区域
 * @param roundType 轮类型
 * @return std::vector<DZJZ_DEVICE> 定值召唤设备列表
 */
std::vector<DZJZ_DEVICE> CFixValueService::GetDZJZDeviceList(const QString &subStation, int area, int roundType)
{
    DFLOG_INFO("[定值召唤]: subStation=%s area=%d roundType=%d", subStation.toLocal8Bit().data(), area, roundType);

    DZJZ_DEVICE *pBuf = NULL;
    std::vector<DZJZ_DEVICE> deviceList;

    QString sWhere = " where 1=1";
    if (!subStation.isEmpty())
        sWhere += " and 所属厂站='" + subStation + "'";

    if (area > 0)
        sWhere += " and 所属分区=" + QString::number(area);

    if (roundType > 0)
    {
        if (roundType == 1) // 基础轮
            sWhere += " and 轮类型 in (1,2,3,4,5)";
        else if (roundType == 2)
            sWhere += " and 轮类型 in (6)";
        else if (roundType == 3)
            sWhere += " and 轮类型 in(7,8)";
    }

    QString qsCond = QString("select distinct 所属装置,装置名称 from xopensdb.dbo.低周减载轮次项视图 %1").arg(sWhere);
    DFLOG_INFO("[定值召唤]: sql=%s", qsCond.toAscii().data());

    // 从数据库中获取所有的定值召唤设备
    int nRet = m_pApp->m_baseDB.Isql(qsCond.toAscii().data(), (void **)&pBuf);

    if (nRet > 0)
    {
        deviceList.reserve(nRet); // 预先分配内存
        deviceList.assign(pBuf, pBuf + nRet);

        for (int i = 0; i < deviceList.size(); i++)
        {
            DFLOG_DEBUG("%d|%s", deviceList[i].rtuno, deviceList[i].desc);
        }
    }

    return deviceList;
}

/**
 * @brief 重载实时库
 *
 */
void CFixValueService::reloadRTDB()
{
    rtdb_proc rtdbproc;
    if (!rtdbproc.recall("ems", "", "", "低周减载装置定值信息表"))
    {
        DFLOG_ERROR("[定值召唤]:重载实时库失败");
    }
    DFLOG_INFO("[定值召唤]:重载实时库成功");
}

/**
 * @brief 创建事项
 *
 */
void CFixValueService::CreateEvent(int category, int type, int state_value, QString char_info, QString member_name0 /*= QString::null*/, QString member_name1 /*= QString::null*/, QString group_name /*= QString::null*/)
{
    quint16 cat_no = category;
    quint16 type_no = type;

    SYS_EVENT sysevent;
    event_proc netevent;
    memset(&sysevent, 0, sizeof(SYS_EVENT));
    QDateTime dataTime = QDateTime::currentDateTime();
    QDate date = dataTime.date();
    QTime time = dataTime.time();

    sysevent.event_sort = cat_no;
    sysevent.event_type = type_no;
    sysevent.security = 0;
    sysevent.PrintOut = 0;
    sysevent.ToneOut = 0;
    sysevent.DiskOut = sysevent.DispOut = TRUE;
    sysevent.state_value = state_value;
    sysevent.ymd = date.year() * 10000 + date.month() * 100 + date.day();
    sysevent.hmsms = time.hour() * 10000000 + time.minute() * 100000 + time.second() * 1000 + time.msec();

    if (!member_name0.isNull())
        strncpy(sysevent.member_name0, (const char *)member_name0.toAscii(), SHORTNAME_LEN - 1);
    if (!member_name1.isNull())
        strncpy(sysevent.member_name1, (const char *)member_name1.toAscii(), SHORTNAME_LEN - 1);
    if (!group_name.isNull())
        strncpy(sysevent.group_name, (const char *)group_name.toAscii(), SHORTNAME_LEN - 1);
    if (!char_info.isNull())
        strncpy(sysevent.char_info, (const char *)char_info.toAscii(), FULLDESCRIBE_LEN - 1);

    netevent.save_event(&sysevent, 1);
}