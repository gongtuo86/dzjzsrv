#include "dzjzrounditem.h"

#include <sstream>

#include "cmdefs.h"
#include "dflogger.h"
#include "dzjzevent.h"
#include "rdbscd.h"

int DZJZ_RoundItem::opentblf = 0;                                         // 实时库表打开标志
RdbTable DZJZ_RoundItem::rounditemTbl;                                    // 轮次项表
RdbTable DZJZ_RoundItem::dzjzcdzTbl;                                      // 定值信息表
std::unordered_map<std::string, T_DZJZ_DZ *> DZJZ_RoundItem::m_dzjzDZMap; // 定值信息map
int DZJZ_RoundItem::roundItemTblTime = 0;                                 // 轮次项表加载时间
int DZJZ_RoundItem::dzjzcdzTblTime = 0;                                   // 定值信息表加载时间

extern DZJZ_Event dzjzEnt;

/**
 * @brief 构造函数
 *
 */
DZJZ_RoundItem::DZJZ_RoundItem()
{
    if (!opentblf)
    {
        if (rounditemTbl.Login(MyUserName, MyPassWord) != RDB_OK)
        {
            opentblf = 0;
            return;
        }
        if (rounditemTbl.OpenTable(DZJZ_ROUND_ITEM_TBLNAME) != RDB_OK)
        {
            opentblf = 0;
            return;
        }

        if (dzjzcdzTbl.Login(MyUserName, MyPassWord) != RDB_OK)
        {
            opentblf = 0;
            return;
        }
        if (dzjzcdzTbl.OpenTable(DZJZ_DZ_TBLNAME) != RDB_OK)
        {
            opentblf = 0;
            return;
        }

        opentblf = 1;

        checkModify();
    }
}

void DZJZ_RoundItem::checkModify()
{
    intertime tmptime;
    uchar chgflag = 0;

    rounditemTbl.GetStructTime(&tmptime);
    if (tmptime != roundItemTblTime)
    {
        roundItemTblTime = tmptime;
        chgflag = 1;
    }

    dzjzcdzTbl.GetStructTime(&tmptime);
    if (tmptime != dzjzcdzTblTime)
    {
        dzjzcdzTblTime = tmptime;
        chgflag = 1;
    }

    if (chgflag)
    {
        init();
    }
}

void DZJZ_RoundItem::init()
{
    DFLOG_INFO("DZJZ_RoundItem init");
    int i = 0;

    m_dzjzDZMap.clear();

    dzjzcdzTbl.LockTableStruct();
    int nCount = dzjzcdzTbl.GetRecordCount();
    for (i = 0; i < nCount; i++)
    {
        T_DZJZ_DZ *pBuf = (T_DZJZ_DZ *)dzjzcdzTbl.GetRecordAddr(i);
        if (pBuf == nullptr)
            continue;
        std::string key = std::to_string(pBuf->rtuno) + ":" +
                          std::to_string(pBuf->type) + ":" +
                          std::to_string(pBuf->groupno) + ":" +
                          std::to_string(pBuf->no);
        m_dzjzDZMap[key] = pBuf;
        // DFLOG_DEBUG("m_dzjzDZMap[%s] = %s", key.c_str(), pBuf->desc);
    }
    dzjzcdzTbl.UnlockTable();
}

/**
 * @brief 按json格式打印
 *
 * @param pItem
 */
void DZJZ_RoundItem::printItemJson(TDZJZ_ROUNDITEM *pItem)
{
    std::stringstream ss;
    ss << "{ "
       << "id: " << pItem->id << ", "
       << "name: " << pItem->name << ", "

       << "strapid: " << pItem->strapid << ", "
       << "strapplan: " << static_cast<int>(pItem->strapplan) << ", "
       << "strapreal: " << static_cast<int>(pItem->strapreal) << ", "
       << "strapjudge: " << static_cast<int>(pItem->strapjudge) << ", "

       << "realvalueid: " << pItem->realvalueid << ", "
       << "planvalue: " << pItem->planvalue << ", "
       << "realvalue: " << pItem->realvalue << ", "
       << "realtimeid: " << pItem->realtimeid << ", "
       << "plantime: " << pItem->plantime << ", "
       << "realtime: " << pItem->realtime << ", "
       << "valuejudge: " << static_cast<int>(pItem->valuejudge) << ", "

       << "pvalue: " << pItem->pvalue << ", "
       << "activejudge: " << static_cast<int>(pItem->activejudge) << ", "

       << "loadtype: " << static_cast<int>(pItem->loadtype) << ", "
       << "planjudge: " << static_cast<int>(pItem->planjudge) << ","

       << "alarmid: " << pItem->alarmid << ", "
       << "funcjudge: " << static_cast<int>(pItem->funcjudge) << ", "

       << "linkedbreak: " << pItem->linkedbreak << ", "
       << "breakstate: " << static_cast<int>(pItem->breakstate) << ", "

       << "judgepower: " << pItem->judgepower

       << " }";

    DFLOG_DEBUG("%s", ss.str().c_str());
}

/**
 * @brief 刷新有功和开关状态
 *
 */
void DZJZ_RoundItem::freshData(void)
{
    checkModify();

    // DFLOG_DEBUG("====>>freshData start");
    int i, roundItemNums;
    TDZJZ_ROUNDITEM *pRoundItem = nullptr;

    rounditemTbl.LockTableStruct();
    roundItemNums = rounditemTbl.GetRecordCount();

    for (i = 0; i < roundItemNums; i++)
    {
        pRoundItem = (TDZJZ_ROUNDITEM *)rounditemTbl.GetRecordAddr(i);
        freshRealValue(pRoundItem);
    }

    rounditemTbl.UnlockTable();
    // DFLOG_DEBUG("====>>freshData end");
}

/**
 * @brief 刷新实时值
 *
 * @param pItem
 */

void DZJZ_RoundItem::freshRealValue(TDZJZ_ROUNDITEM *pItem)
{
    if (pItem == nullptr)
        return;

    freshPValue(pItem);
    freshBreakValue(pItem);
    activeJudge(pItem);
    valueJudge(pItem);
    planJudge(pItem);
    strapJudge(pItem);
    funcJudge(pItem);
    calcPower(pItem);

    // printItemJson(pItem);
}

/**
 * @brief 刷新有功
 *
 * @param pItem
 */
void DZJZ_RoundItem::freshPValue(TDZJZ_ROUNDITEM *pItem)
{
    if (pItem == nullptr)
        return;
    char ananame[DEF_CODE_LEN];
    strcpy(ananame, pItem->pname);

    Analog ana;
    if (ana.GetValue(ananame, &pItem->pvalue) != OK)
    {
        DFLOG_WARN("pname %s can not find", ananame);
    }
}

/**
 * @brief 刷新开关状态
 *
 * @param pItem
 */
void DZJZ_RoundItem::freshBreakValue(TDZJZ_ROUNDITEM *pItem)
{
    if (pItem == nullptr)
        return;
    Break brk;
    if (brk.getdata(pItem->linkedbreak, &pItem->breakstate) != OK)
    {
        Protsig protsig;
        if (protsig.getdata(pItem->linkedbreak, &pItem->breakstate) != OK)
        {
            DFLOG_ERROR("开关 %s 遥信不存在", pItem->linkedbreak);
        }
    }
    // DFLOG_DEBUG("开关 %s 遥信值=%d", pItem->linkedbreak, pItem->breakstate);
}

/**
 * @brief 有源馈线研判
 *
 * @param pItem
 */
void DZJZ_RoundItem::activeJudge(TDZJZ_ROUNDITEM *pItem)
{
    if (pItem == nullptr)
        return;

    const bool bAlarm = pItem->pvalue <= -1e-7;

    if (pItem->activejudge != bAlarm)
    {
        pItem->activejudge = bAlarm;
        RdbBackupTable(MyUserName, MyPassWord, DZJZ_ROUND_ITEM_TBLNAME);
        if (bAlarm)
            DFLOG_INFO("item %d 有源馈线告警", pItem->id);
        else
            DFLOG_INFO("item %d 有源馈线告警恢复", pItem->id);
    }
}

/**
 * @brief 定值研判
 *
 * @param pItme
 */
void DZJZ_RoundItem::valueJudge(TDZJZ_ROUNDITEM *pItem)
{
    if (pItem == nullptr)
    {
        return;
    }

    std::string valueKey = std::to_string(pItem->deviceid) + ":" + std::string(pItem->realvalueid);
    if (m_dzjzDZMap.find(valueKey) != m_dzjzDZMap.end())
    {
        pItem->realvalue = std::stof(m_dzjzDZMap[valueKey]->value);
    }
    else
    {
        DFLOG_WARN("%s can not find", valueKey.c_str());
        pItem->realvalue = 0.0;
    }

    std::string timeKey = std::to_string(pItem->deviceid) + ":" + std::string(pItem->realtimeid);
    if (m_dzjzDZMap.find(timeKey) != m_dzjzDZMap.end())
    {
        pItem->realtime = std::stof(m_dzjzDZMap[timeKey]->value);
    }
    else
    {
        DFLOG_WARN("%s can not find", timeKey.c_str());
        pItem->realtime = 0;
    }

    bool bAlarm = !(ISZERO(pItem->realvalue - pItem->planvalue) &&
                    ISZERO(pItem->realtime - pItem->plantime));

    if (pItem->valuejudge != bAlarm)
    {
        pItem->valuejudge = bAlarm;
        RdbBackupTable(MyUserName, MyPassWord, DZJZ_ROUND_ITEM_TBLNAME);
        if (bAlarm)
        {
            dzjzEnt.make_judgevalue_event(pItem, 1);
            DFLOG_INFO("pItem %d 定值研判告警", pItem->id);
        }
        else
        {
            dzjzEnt.make_judgevalue_event(pItem, 0);
            DFLOG_INFO("pItem %d 定值研判恢复", pItem->id);
        }
    }
}

/**
 * @brief 低周减载方案研判 是否含有民生负荷
 *
 * @param pItem
 */
void DZJZ_RoundItem::planJudge(TDZJZ_ROUNDITEM *pItem)
{
    if (pItem == nullptr)
        return;

    // 只有第一轮和第二轮判定，其他轮若告警则恢复
    if (pItem->roundtype != 1 || pItem->roundtype != 2)
    {
        if (pItem->planjudge == 1)
        {
            pItem->planjudge = 0;
            RdbBackupTable(MyUserName, MyPassWord, DZJZ_ROUND_ITEM_TBLNAME);
            dzjzEnt.make_judgeplan_event(pItem, 0);
            DFLOG_INFO("pItem %d 方案研判恢复", pItem->id);
        }
        return;
    }

    bool bAlarm = pItem->loadtype <= LOW_PROPORTION_OF_LIVELIHOOD;
    if (bAlarm != pItem->planjudge)
    {
        pItem->planjudge = bAlarm;
        RdbBackupTable(MyUserName, MyPassWord, DZJZ_ROUND_ITEM_TBLNAME);
        if (bAlarm)
        {
            DFLOG_INFO("pItem %d 方案研判告警", pItem->id);
            dzjzEnt.make_judgeplan_event(pItem, 1);
        }
        else
        {
            DFLOG_INFO("pItem %d 方案研判恢复", pItem->id);
            dzjzEnt.make_judgeplan_event(pItem, 0);
        }
    }
}

/**
 * @brief 压板状态功能设置
 *
 * @param pItem
 * @param strapJudge
 * @param logMsg
 */
void DZJZ_RoundItem::setStrapJudge(TDZJZ_ROUNDITEM *pItem)
{
    if (pItem == nullptr)
        return;

    uchar strapJudge = pItem->strapreal << 1 | pItem->strapplan;
    if (pItem->strapjudge != strapJudge)
    {
        pItem->strapjudge = strapJudge;
        RdbBackupTable(MyUserName, MyPassWord, DZJZ_ROUND_ITEM_TBLNAME);
        DFLOG_INFO("pItem %d 功能压板状态[%s]", pItem->id, s_stapArr[strapJudge]);
        dzjzEnt.make_judgefunc_event(pItem);
    }
}

/**
 * @brief 分割字符串
 *
 * @param str
 * @param ids
 */
void DZJZ_RoundItem::splitString(const char *str, std::vector<std::string> &ids)
{
    ids.clear();
    std::istringstream iss(str);
    std::string strapid;
    while (std::getline(iss, strapid, ','))
    {
        ids.push_back(strapid);
    }
}

/**
 * @brief 低周减载功能压板状态研判
 *
 * @param pItem
 */
void DZJZ_RoundItem::strapJudge(TDZJZ_ROUNDITEM *pItem)
{
    if (!pItem || pItem->strapid[0] == '\0')
        return;

    std::vector<std::string> strapids;
    splitString(pItem->strapid, strapids);

    Protsig status;
    uchar tmpVal;
    uchar strapVal = 0x01;
    // 有一个压板未投入，就不算投入
    for (const std::string &strapid : strapids)
    {
        std::string valueKey = std::to_string(pItem->deviceid) + ":" + std::string(pItem->strapid);
        if (m_dzjzDZMap.find(valueKey) != m_dzjzDZMap.end())
        {
            tmpVal = std::stoi(m_dzjzDZMap[valueKey]->value);
        }
        else
        {
            DFLOG_WARN("%s can not find", valueKey.c_str());
            tmpVal = STAVAL_OFF;
        }
        if (tmpVal != STAVAL_ON)
        {
            strapVal = 0x00;
            break;
        }
    }
    pItem->strapreal = strapVal;

    setStrapJudge(pItem);
}

/**
 * @brief 相关告警信号计算
 *
 * @param pItem
 */
void DZJZ_RoundItem::alarmJudge(TDZJZ_ROUNDITEM *pItem)
{
    if (pItem == nullptr || pItem->alarmid[0] == '\0')
        return;

    std::vector<std::string> alarmids;
    splitString(pItem->alarmid, alarmids);
    uchar tmpVal;
    Protsig status;

    uchar nAlarm = 0x00;

    // 有一个告警信号就告警
    for (const std::string &alarmId : alarmids)
    {
        if (status.getdata(const_cast<char *>(alarmId.c_str()), &tmpVal) == OK && tmpVal == STAVAL_ON)
        {
            nAlarm = 0x01;
            break;
        }
    }

    if (nAlarm != pItem->devalarm)
    {
        pItem->devalarm = nAlarm;
        RdbBackupTable(MyUserName, MyPassWord, DZJZ_ROUND_ITEM_TBLNAME);
        if (nAlarm == 0x01)
            DFLOG_INFO("pItem %d 装置告警");
        else
            DFLOG_INFO("pItem %d 装置告警恢复");
    }

    return;
}

bool DZJZ_RoundItem::isStrapNormal(int strapJudge)
{
    if (strapJudge == 0 || strapJudge == 3)
        return true;
    else
        return false;

    return false;
}

/**
 * @brief 轮次项功能研判
 *
 * @param pItem
 */
void DZJZ_RoundItem::funcJudge(TDZJZ_ROUNDITEM *pItem)
{
    if (pItem == nullptr)
        return;

    bool valJudge = false;

    valJudge = !isStrapNormal(pItem->strapjudge) || pItem->valuejudge || pItem->devalarm || pItem->activejudge;

    if (valJudge != pItem->funcjudge)
    {
        pItem->funcjudge = valJudge;
        RdbBackupTable(MyUserName, MyPassWord, DZJZ_ROUND_ITEM_TBLNAME);

        if (valJudge)
        {
            DFLOG_INFO("pItem %d 轮次项功能研判告警", pItem->id);
        }
        else
        {
            DFLOG_INFO("pItem %d 轮次项功能研判恢复", pItem->id);
        }
    }
}

/**
 * @brief 计算切荷量
 *
 */
void DZJZ_RoundItem::calcPower(TDZJZ_ROUNDITEM *pItem)
{
    // 实际投运切荷量=馈线负荷*计划投入*功能研判结果
    pItem->judgepower = pItem->pvalue * pItem->strapplan * (!pItem->funcjudge);

    // 计划投运切荷量 = 馈线负荷*计划投入
    pItem->planpower = pItem->pvalue * pItem->strapplan;

    // 实际备用切荷量 = 馈线负荷*计划备用*功能研判结果
    pItem->standbyPower = pItem->pvalue * (!pItem->strapplan) * (!pItem->funcjudge);

    // 计划备用切荷量 = 馈线负荷*计划备用
    pItem->planStandbyPower = pItem->pvalue * (!pItem->strapplan);
}