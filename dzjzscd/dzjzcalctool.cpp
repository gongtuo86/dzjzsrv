#include "dzjzcalctool.h"
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#include <QCoreApplication>
#include <algorithm>
#include <sstream>

#include "dflogger.h"
#include "dzjzevent.h"

extern DZJZ_Event dzjzEnt;

int DZJZ_CalcTool::opentblf = 0;                               // 实时库表打开标志

RdbTable DZJZ_CalcTool::roundTbl;                              // 轮次表
RdbTable DZJZ_CalcTool::roundItemTbl;                          // 轮次项表
RdbTable DZJZ_CalcTool::areaTbl;                               // 区域表
RdbTable DZJZ_CalcTool::roundStaTbl;                           // 轮次统计表
RdbTable DZJZ_CalcTool::progTbl;                               // 程序注册表

int DZJZ_CalcTool::roundTblTime = 0;                           // 轮次表加载时间
int DZJZ_CalcTool::roundItemTblTime = 0;                       // 轮次项表加载时间
int DZJZ_CalcTool::areaTblTime = 0;                            // 区域表加载时间
int DZJZ_CalcTool::roundStaTblTime = 0;                        // 伦次统计表加载时间
int DZJZ_CalcTool::progTblTime = 0;                            // 程序注册表加载时间

ItemVecMap DZJZ_CalcTool::roundItemMap;                        // 轮次项和轮次关系
ItemVecMap DZJZ_CalcTool::areaItemMap;                         // 轮次项和区域关系
ItemVecMap DZJZ_CalcTool::roundTypeItemMap;                    // 轮次项和轮次统计表关系
RoundVecMap DZJZ_CalcTool::roundTypeRoundMap;                  // 轮次类型和轮次关系

float DZJZ_CalcTool::dzjzk = GetProgramRunPara("dzjzk", 1.0f); // K系数
TDZJZ_AREA *DZJZ_CalcTool::pAllArea = NULL;                    // 全网区域计算
float DZJZ_CalcTool::sumP = 0.0f;                              // 全网负荷

/**
 * @brief 构造函数
 *
 */
DZJZ_CalcTool::DZJZ_CalcTool(void)
{
    if (!opentblf)
    {
        if (roundTbl.Login(MyUserName, MyPassWord) != RDB_OK)
        {
            opentblf = 0;
            return;
        }
        if (roundTbl.OpenTable(DZJZ_ROUND_TBLNAME) != RDB_OK)
        {
            opentblf = 0;
            return;
        }

        if (roundItemTbl.Login(MyUserName, MyPassWord) != RDB_OK)
        {
            opentblf = 0;
            return;
        }
        if (roundItemTbl.OpenTable(DZJZ_ROUND_ITEM_TBLNAME) != RDB_OK)
        {
            opentblf = 0;
            return;
        }

        if (areaTbl.Login(MyUserName, MyPassWord) != RDB_OK)
        {
            opentblf = 0;
            return;
        }
        if (areaTbl.OpenTable(DZJZ_AREA_TBLNAME) != RDB_OK)
        {
            opentblf = 0;
            return;
        }

        if (progTbl.Login(MyUserName, MyPassWord) != RDB_OK)
        {
            opentblf = 0;
            return;
        }
        if (progTbl.OpenTable(PROGRAMREGIST_TBLNAME) != RDB_OK)
        {
            opentblf = 0;
            return;
        }

        if (roundStaTbl.Login(MyUserName, MyPassWord) != RDB_OK)
        {
            opentblf = 0;
            return;
        }
        if (roundStaTbl.OpenTable(DZJZ_ROUND_STATIC_TBLNAME) != RDB_OK)
        {
            opentblf = 0;
            return;
        }

        opentblf = 1;

        checkModify();
    }
}

/**
 * @brief 计算轮次项
 *
 */
void DZJZ_CalcTool::Calc()
{
    checkModify();

    sumP = GetSysSumP(); // 获取系统总负荷

    calcRound();
    calcRoundSta();
    calcArea();
}

/**
 * @brief 计算各轮次
 *
 */
void DZJZ_CalcTool::calcRound()
{
    roundTbl.LockTableStruct();
    const int nRoundCnt = roundTbl.GetRecordCount();

    float nAllRequirePower = 0.0f; // 全网应切荷量
    for (int i = 0; i < nRoundCnt; ++i)
    {
        TDZJZ_ROUND *pRound = (TDZJZ_ROUND *)roundTbl.GetRecordAddr(i);
        if (!pRound)
            continue;

        float judgePower = 0.0f;
        float planPower = 0.0f;
        float standbyPower = 0.0f;
        float planStandbyPower = 0.0f;
        const RoundItemVec &roundItemVec = roundItemMap[pRound->id];
        const int roundSize = roundItemVec.size();
        // DFLOG_DEBUG("id:%d count:%d", pRound->id, roundSize);

        for (const auto &roundItem : roundItemVec)
        {
            judgePower += roundItem->judgepower;
            planPower += roundItem->planpower;
            standbyPower += roundItem->standbyPower;
            planStandbyPower += roundItem->planStandbyPower;
        }

        pRound->judgepower = judgePower;
        pRound->planpower = planPower;
        pRound->requirePower = pRound->issuePower * dzjzk;
        pRound->standbyPower = standbyPower;
        pRound->planStandbyPower = planStandbyPower;
        nAllRequirePower += pRound->requirePower;

        judgeRequire(pRound);

        // printRoundJson(pRound);
    }

    if (pAllArea != nullptr)
    {
        pAllArea->requirePower = nAllRequirePower;
    }
    roundTbl.UnlockTable();
}

/**
 * @brief 计算轮次统计
 *
 */
void DZJZ_CalcTool::calcRoundSta()
{
    roundStaTbl.LockTableStruct();
    const int nRoundStaCnt = roundStaTbl.GetRecordCount();

    for (int i = 0; i < nRoundStaCnt; ++i)
    {
        TDZJZ_ROUNDSTA *pRoundSta = (TDZJZ_ROUNDSTA *)roundStaTbl.GetRecordAddr(i);
        if (!pRoundSta)
            continue;

        float judgepower = 0.0f;
        float planpower = 0.0f;
        float standbyPower = 0.0f;
        float planStandbyPower = 0.0f;
        int planFeederNum = 0;
        int realFeederNum = 0;
        int planStandbyFeederNum = 0;
        int realStandbyFeederNum = 0;

        const RoundItemVec &roundItemVec = roundTypeItemMap[pRoundSta->id];
        const int roundItemSize = roundItemVec.size();
        // DFLOG_DEBUG("id:%d item count:%d", pRoundSta->id, roundItemSize);

        for (const auto &roundItem : roundItemVec)
        {
            judgepower += roundItem->judgepower;
            planpower += roundItem->planpower;
            standbyPower += roundItem->standbyPower;
            planStandbyPower += roundItem->planStandbyPower;
            if (roundItem->strapplan == 1)
            {
                planFeederNum++;
            }
            else
            {
                planStandbyFeederNum++;
            }

            if (roundItem->strapjudge == 3 && roundItem->funcjudge == 0)
            {
                realFeederNum++;
            }
            else
            {
                realStandbyFeederNum++;
            }
        }

        pRoundSta->planFeederNum = planFeederNum;
        pRoundSta->realFeederNum = realFeederNum;
        pRoundSta->realStandbyFeederNum = realStandbyFeederNum;
        pRoundSta->planStandbyFeederNum = planStandbyFeederNum;
        pRoundSta->judgepower = judgepower;
        pRoundSta->planpower = planpower;
        pRoundSta->standbyPower = standbyPower;
        pRoundSta->planStandbyPower = planStandbyPower;

        if (roundItemSize != 0)
        {
            pRoundSta->realFeederRate = std::min(static_cast<float>(pRoundSta->realFeederNum) / roundItemSize * 100, 100.0f);
            pRoundSta->planFeederRate = std::min(static_cast<float>(pRoundSta->planFeederNum) / roundItemSize * 100, 100.0f);
        }

        // 计算轮次类型应切荷量
        float requirePower = 0.0f;
        const RoundVec &roundVec = roundTypeRoundMap[pRoundSta->id];
        const int roundSize = roundItemVec.size();
        // DFLOG_DEBUG("id:%d round count:%d", pRoundSta->id, roundSize);
        for (const auto &round : roundVec)
        {
            requirePower += round->requirePower;
        }
        pRoundSta->requirePower = requirePower;

        if (!ISZERO(pRoundSta->requirePower))
        {
            pRoundSta->realCompRate = std::min(static_cast<float>(100, pRoundSta->judgepower / pRoundSta->requirePower * 100), 100.0f);
            pRoundSta->planCompRate = std::min(static_cast<float>(100, pRoundSta->planpower / pRoundSta->requirePower * 100), 100.0f);
        }
        else
        {
            pRoundSta->realCompRate = 0.0f;
            pRoundSta->planCompRate = 0.0f;
        }

        if (!ISZERO(sumP))
        {
            pRoundSta->realPowerRate = std::min(static_cast<float>(100, pRoundSta->judgepower / sumP * 100), 100.0f);
            pRoundSta->planPowerRate = std::min(static_cast<float>(100, pRoundSta->planpower / sumP * 100), 100.0f);
        }
        else
        {
            pRoundSta->realPowerRate = 0.0f;
            pRoundSta->planPowerRate = 0.0f;
        }

        // printRoundStaJson(pRoundSta);
    }
    roundStaTbl.UnlockTable();
}

/**
 * @brief 计算各区域投运切荷量
 *
 */
void DZJZ_CalcTool::calcArea()
{
    areaTbl.LockTableStruct();
    int nCnt = areaTbl.GetRecordCount();
    for (int i = 0; i < nCnt; i++)
    {
        TDZJZ_AREA *pArea = (TDZJZ_AREA *)areaTbl.GetRecordAddr(i);
        if (!pArea)
            continue;

        float judgepower = 0.0f;
        float planpower = 0.0f;
        float standbyPower = 0.0f;
        float planStandbyPower = 0.0f;
        int planFeederNum = 0;
        int realFeederNum = 0;
        int planStandbyFeederNum = 0;
        int realStandbyFeederNum = 0;

        const RoundItemVec &roundItemVec = areaItemMap[pArea->id];
        const int roundSize = roundItemVec.size();
        // DFLOG_DEBUG("id:%d count:%d", pArea->id, roundSize);

        for (const auto &roundItem : roundItemVec)
        {
            judgepower += roundItem->judgepower;
            planpower += roundItem->planpower;
            standbyPower += roundItem->standbyPower;
            planStandbyPower += roundItem->planStandbyPower;
            if (roundItem->strapplan == 1)
            {
                planFeederNum++;
            }
            else
            {
                planStandbyFeederNum++;
            }

            if (roundItem->strapjudge == 3 && roundItem->funcjudge == 0)
            {
                realFeederNum++;
            }
            else
            {
                realStandbyFeederNum++;
            }
        }

        pArea->planFeederNum = planFeederNum;
        pArea->realFeederNum = realFeederNum;
        pArea->realStandbyFeederNum = realStandbyFeederNum;
        pArea->planStandbyFeederNum = planStandbyFeederNum;
        pArea->judgepower = judgepower;
        pArea->planpower = planpower;
        pArea->standbyPower = standbyPower;
        pArea->planStandbyPower = planStandbyPower;

        if (roundSize != 0)
        {
            pArea->realFeederRate = std::min(static_cast<float>(float(pArea->realFeederNum) / roundSize * 100), 100.0f);
            pArea->planFeederRate = std::min(static_cast<float>(float(pArea->planFeederNum) / roundSize * 100), 100.0f);
        }
        else
        {
            pArea->realFeederRate = 0;
            pArea->planFeederRate = 0;
        }

        // 投运完成率
        if (!ISZERO(pArea->requirePower))
        // if (!ISZERO(pArea->requirePower) && pArea->id == ALL_AREA_ID)
        {
            pArea->realCompRate = std::min(static_cast<float>(pArea->judgepower / pArea->requirePower * 100), 100.0f);
            pArea->planCompRate = std::min(static_cast<float>(pArea->planpower / pArea->requirePower * 100), 100.0f);
        }
        else
        {
            pArea->realCompRate = 0;
            pArea->planCompRate = 0;
        }

        // 投运负荷占比
        if (!ISZERO(sumP))
        // if (!ISZERO(sumP) && pArea->id == ALL_AREA_ID)
        {
            pArea->realPowerRate = std::min(static_cast<float>(pArea->judgepower / sumP * 100), 100.0f);
            pArea->planPowerRate = std::min(static_cast<float>(pArea->planpower / sumP * 100), 100.0f);
        }
        else
        {
            pArea->realPowerRate = 0;
            pArea->planPowerRate = 0;
        }

        // printAreaJson(pArea);
    }
    areaTbl.UnlockTable();
}

/**
 * @brief 按json格式打印
 *
 * @param pRound
 */
void DZJZ_CalcTool::printRoundJson(TDZJZ_ROUND *pRound)
{
    std::stringstream ss;
    ss << "{ "
       << "id: " << pRound->id << ", "
       << "name: " << pRound->name << ", "

       << "judgepower: " << pRound->judgepower
       << "planpower: " << pRound->planpower
       << "requirePower: " << pRound->requirePower
       << "issuePower: " << pRound->issuePower

       << " }";

    DFLOG_DEBUG("%s", ss.str().c_str());
}

/**
 * @brief 按json格式打印
 *
 * @param pRound
 */
void DZJZ_CalcTool::printRoundStaJson(TDZJZ_ROUNDSTA *pRoundSta)
{
    std::stringstream ss;
    ss << "{ "
       << "id: " << pRoundSta->id << ", "
       << "name: " << pRoundSta->name << ", "

       << "judgepower: " << pRoundSta->judgepower
       << "planpower: " << pRoundSta->planpower
       << "realFeederNum: " << pRoundSta->realFeederNum
       << "planFeederNum: " << pRoundSta->planFeederNum
       << "realFeederRate: " << pRoundSta->realFeederRate
       << "planFeederRate: " << pRoundSta->planFeederRate
       << "requirePower: " << pRoundSta->requirePower
       << " }";

    DFLOG_DEBUG("%s", ss.str().c_str());
}

/**
 * @brief 按json格式打印
 *
 * @param pRound
 */
void DZJZ_CalcTool::printAreaJson(TDZJZ_AREA *pArea)
{
    std::stringstream ss;
    ss << "{ "
       << "id: " << pArea->id << ", "
       << "name: " << pArea->name << ", "

       << "judgepower: " << pArea->judgepower
       << "planpower: " << pArea->planpower
       << "realFeederNum: " << pArea->realFeederNum
       << "planFeederNum: " << pArea->planFeederNum
       << "realFeederRate: " << pArea->realFeederRate
       << "planFeederRate: " << pArea->planFeederRate
       << "requirePower: " << pArea->requirePower

       << " }";

    DFLOG_DEBUG("%s", ss.str().c_str());
}

/**
 * @brief 检查是否更新
 *
 */
void DZJZ_CalcTool::checkModify()
{
    intertime tmptime;
    uchar chgflag = 0;

    roundTbl.GetStructTime(&tmptime);
    if (tmptime != roundTblTime)
    {
        roundTblTime = tmptime;
        chgflag = 1;
    }
    roundItemTbl.GetStructTime(&tmptime);
    if (tmptime != roundItemTblTime)
    {
        roundItemTblTime = tmptime;
        chgflag = 1;
    }
    areaTbl.GetStructTime(&tmptime);
    if (tmptime != areaTblTime)
    {
        areaTblTime = tmptime;
        chgflag = 1;
    }
    progTbl.GetStructTime(&tmptime);
    if (tmptime != progTblTime)
    {
        progTblTime = tmptime;
        chgflag = 1;
    }
    roundStaTbl.GetStructTime(&tmptime);
    if (tmptime != roundStaTblTime)
    {
        roundStaTblTime = tmptime;
        chgflag = 1;
    }

    if (chgflag)
    {
        init();
    }
}

/**
 * @brief 初始化轮次项关系
 *
 */
void DZJZ_CalcTool::init()
{
    DFLOG_INFO("DZJZ_CalcTool init");

    int i = 0;
    roundItemMap.clear();
    areaItemMap.clear();
    roundTypeItemMap.clear();
    roundTypeRoundMap.clear();

    int nRoundItemCount = roundItemTbl.GetRecordCount();
    // 预分配空间，避免频繁的内存分配和释放
    roundItemMap.reserve(roundTbl.GetRecordCount());
    areaItemMap.reserve(areaTbl.GetRecordCount());
    roundTypeItemMap.reserve(roundStaTbl.GetRecordCount());
    roundTypeRoundMap.reserve(roundStaTbl.GetRecordCount());

    for (i = 0; i < nRoundItemCount; i++)
    {
        TDZJZ_ROUNDITEM *pRoundItem = (TDZJZ_ROUNDITEM *)roundItemTbl.GetRecordAddr(i);
        if (pRoundItem == nullptr)
            continue;
        roundItemMap[pRoundItem->roundid].push_back(pRoundItem);
        areaItemMap[pRoundItem->areaid].push_back(pRoundItem);
        areaItemMap[ALL_AREA_ID].push_back(pRoundItem);
        roundTypeItemMap[pRoundItem->roundtype].push_back(pRoundItem);
    }

    int nRoundCount = roundTbl.GetRecordCount();
    for (i = 0; i < nRoundCount; i++)
    {
        TDZJZ_ROUND *pRound = (TDZJZ_ROUND *)roundTbl.GetRecordAddr(i);
        roundTypeRoundMap[pRound->roundtype].push_back(pRound);
    }

    pAllArea = (TDZJZ_AREA *)areaTbl.SearchRcdAddrByKey((void *)&ALL_AREA_ID);

    dzjzk = GetProgramRunPara("dzjzk", 1.0f);
}

// 获取有功总加
float DZJZ_CalcTool::GetSysSumP()
{
    Analog ana;
    float pvalue = 0.0f;
    if (ana.GetValue(SYS_P_NAME, &pvalue) != OK)
    {
        DFLOG_WARN("pname %s can not find", SYS_P_NAME);
        return 0;
    }
    return pvalue;
}

/**
 * @brief 减载容量研判
 *
 */
void DZJZ_CalcTool::judgeRequire(TDZJZ_ROUND *pRound)
{
    if (pRound == nullptr)
        return;

    const bool bAlarm = pRound->judgepower < pRound->requirePower;

    if (pRound->judgeRequire != bAlarm)
    {
        pRound->judgeRequire = bAlarm;
        RdbBackupTable(MyUserName, MyPassWord, DZJZ_ROUND_TBLNAME);
        if (bAlarm)
        {
            DFLOG_INFO("round %d 低周减载容量不足告警 投运切荷量[%f] 应切荷量[%f]", pRound->id, pRound->judgepower, pRound->requirePower);
            dzjzEnt.make_capacity_event(pRound, 1);
        }
        else
        {
            DFLOG_INFO("round %d 低周减载容量不足告警恢复 投运切荷量[%f] 应切荷量[%f]", pRound->id, pRound->judgepower, pRound->requirePower);
            dzjzEnt.make_capacity_event(pRound, 0);
        }
    }
}