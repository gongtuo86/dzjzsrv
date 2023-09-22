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
#include "dzjzsrv.h"

extern DZJZ_Event dzjzEnt;

int DZJZ_CalcTool::opentblf = 0; // ʵʱ���򿪱�־

RdbTable DZJZ_CalcTool::roundTbl;     // �ִα�
RdbTable DZJZ_CalcTool::roundItemTbl; // �ִ����
RdbTable DZJZ_CalcTool::areaTbl;      // �����
RdbTable DZJZ_CalcTool::roundStaTbl;  // �ִ�ͳ�Ʊ�
RdbTable DZJZ_CalcTool::progTbl;      // ����ע���
RdbTable DZJZ_CalcTool::deviceTbl;    // װ�ñ�

int DZJZ_CalcTool::roundTblTime = 0;     // �ִα����ʱ��
int DZJZ_CalcTool::roundItemTblTime = 0; // �ִ�������ʱ��
int DZJZ_CalcTool::areaTblTime = 0;      // ��������ʱ��
int DZJZ_CalcTool::roundStaTblTime = 0;  // �״�ͳ�Ʊ����ʱ��
int DZJZ_CalcTool::progTblTime = 0;      // ����ע������ʱ��
int DZJZ_CalcTool::deviceTblTime = 0;    // װ�ñ����ʱ��

ItemVecMap DZJZ_CalcTool::roundItemMap;       // �ִ�����ִι�ϵ
ItemVecMap DZJZ_CalcTool::areaItemMap;        // �ִ���������ϵ
ItemVecMap DZJZ_CalcTool::roundTypeItemMap;   // �ִ�����ִ�ͳ�Ʊ��ϵ
ItemVecMap DZJZ_CalcTool::deviceItemMap;      // �ִ�����ִ�ͳ�Ʊ��ϵ
RoundVecMap DZJZ_CalcTool::roundTypeRoundMap; // �ִ����ͺ��ִι�ϵ

float DZJZ_CalcTool::dzjzk = GetProgramRunPara("dzjzk", 1.0f); // Kϵ��
TDZJZ_AREA *DZJZ_CalcTool::pAllArea = NULL;                    // ȫ���������
float DZJZ_CalcTool::sumP = 0.0f;                              // ȫ������

float DZJZ_CalcTool::dzjzMaxP = GetProgramRunPara("dzjzmap", 0.0f); // ��ȼƻ���߸���

/**
 * @brief ���캯��
 *
 */
DZJZ_CalcTool::DZJZ_CalcTool(void)
{
    struct TableInfo
    {
        RdbTable *table;
        char *tableName;
        int expectedRecordLength;
    };

    if (!opentblf)
    {
        TableInfo tables[] = {
            {&roundTbl, DZJZ_ROUND_TBLNAME, sizeof(TDZJZ_ROUND)},
            {&roundItemTbl, DZJZ_ROUND_ITEM_TBLNAME, sizeof(TDZJZ_ROUNDITEM)},
            {&areaTbl, DZJZ_AREA_TBLNAME, sizeof(TDZJZ_AREA)},
            {&progTbl, PROGRAMREGIST_TBLNAME, 0},
            {&roundStaTbl, DZJZ_ROUND_STATIC_TBLNAME, sizeof(TDZJZ_ROUNDSTA)},
            {&deviceTbl, DZJZ_DEVICE_TBLNAME, sizeof(TDZJZ_DEVICE)}};

        for (auto &tblInfo : tables)
        {
            if (tblInfo.table->Login(MyUserName, MyPassWord) != RDB_OK)
            {
                opentblf = 0;
                return;
            }
            if (tblInfo.table->OpenTable(tblInfo.tableName) != RDB_OK)
            {
                opentblf = 0;
                return;
            }
            if (tblInfo.expectedRecordLength && tblInfo.table->GetRecordLength() != tblInfo.expectedRecordLength)
            {
                DFLOG_ERROR("%s �ṹ��һ��", tblInfo.tableName);
            }
        }

        opentblf = 1;
        checkModify();
    }
}

/**
 * @brief �����ִ���
 *
 */
void DZJZ_CalcTool::Calc()
{
    checkModify();

    sumP = GetSysSumP(); // ��ȡϵͳ�ܸ���

    calcRound();
    calcRoundSta();
    calcArea();
    calcDevice();
}

/**
 * @brief ������ִ�
 *
 */
void DZJZ_CalcTool::calcRound()
{
    roundTbl.LockTableStruct();
    const int nRoundCnt = roundTbl.GetRecordCount();

    float nAllRequirePower = 0.0f; // ȫ��Ӧ�к���
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
        if (!ISZERO(sumP))
        {
            pRound->realPowerRate = pRound->judgepower / sumP * 100;
        }

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
 * @brief �����ִ�ͳ��
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

        // �����ִ�����Ӧ�к���
        float requirePower = 0.0f;
        // float planPowerRate = 0.0f;
        const RoundVec &roundVec = roundTypeRoundMap[pRoundSta->id];
        const int roundSize = roundItemVec.size();
        // DFLOG_DEBUG("id:%d round count:%d", pRoundSta->id, roundSize);
        for (const auto &round : roundVec)
        {
            requirePower += round->requirePower;
            // planPowerRate += round->planPowerRate;
        }
        pRoundSta->requirePower = requirePower;
        // pRoundSta->planPowerRate = planPowerRate;

        if (!ISZERO(pRoundSta->requirePower))
        {
            pRoundSta->realCompRate = std::min(pRoundSta->judgepower / pRoundSta->requirePower * 100, 100.0f);
            pRoundSta->planCompRate = std::min(pRoundSta->planpower / pRoundSta->requirePower * 100, 100.0f);
        }
        else
        {
            pRoundSta->realCompRate = 0.0f;
            pRoundSta->planCompRate = 0.0f;
        }

        if (!ISZERO(sumP))
        {
            pRoundSta->realPowerRate = std::min(pRoundSta->judgepower / sumP * 100, 100.0f);
            pRoundSta->planPowerRate = std::min(pRoundSta->planpower / sumP * 100, 100.0f);
        }
        else
        {
            pRoundSta->realPowerRate = 0.0f;
        }

        // printRoundStaJson(pRoundSta);
    }
    roundStaTbl.UnlockTable();
}

/**
 * @brief ����װ����Ϣ
 *
 */
void DZJZ_CalcTool::calcDevice()
{
    deviceTbl.LockTableStruct();
    int nCnt = deviceTbl.GetRecordCount();
    for (int i = 0; i < nCnt; i++)
    {
        TDZJZ_DEVICE *pDevice = (TDZJZ_DEVICE *)deviceTbl.GetRecordAddr(i);
        if (!pDevice)
            continue;

        RoundItemVec &roundItemVec = deviceItemMap[pDevice->id];

        bool alarm = false;
        int sfRet = NOT_CHECKED; // ����״̬��ʼʱ��������
        int sfReason = UNKOWN_RASON;
        bool sgRet = false;

        for (const auto roundItem : roundItemVec)
        {
            // �����һ����澯�͸澯
            if (roundItem->devalarm)
            {
                alarm = true;
            }

            // ӦͶδͶ
            if (roundItem->strapjudge == ACT_EXIT)
            {
                sfRet = NOT_SUBMITTED;
            }
            // Ӧ��δ��
            else if (roundItem->strapjudge == EXIT_ACT)
            {
                sfRet = NOT_RETURNED;
            }
            else if (isStrapNormal(roundItem->strapjudge))
            {
                sfRet = CONSISTENT;
            }

            if (roundItem->valuejudge)
            {
                sgRet = true;
            }

            if (alarm && sfRet != NOT_CHECKED && sgRet)
            {
                break;
            }
        }

        pDevice->alarm = alarm;
        pDevice->sfRet = sfRet;
        pDevice->sfReason = UNKOWN_RASON;
        pDevice->diRet = NOT_CHECKED;
        pDevice->diReason = UNKOWN_RASON;
        pDevice->sgRet = sgRet;
        pDevice->sgReason = UNKOWN_RASON;
    }
}

/**
 * @brief ���������Ͷ���к���
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

        // Ͷ�������
        if (!ISZERO(pArea->requirePower))
        // if (!ISZERO(pArea->requirePower) && pArea->id == ALL_AREA_ID)
        {
            pArea->realCompRate = std::min(pArea->judgepower / pArea->requirePower * 100, 100.0f);
            pArea->planCompRate = std::min(pArea->planpower / pArea->requirePower * 100, 100.0f);
        }
        else
        {
            pArea->realCompRate = 0;
            pArea->planCompRate = 0;
        }

        // Ͷ�˸���ռ��
        if (!ISZERO(sumP))
        // if (!ISZERO(sumP) && pArea->id == ALL_AREA_ID)
        {
            pArea->realPowerRate = std::min(pArea->judgepower / sumP * 100, 100.0f);
            pArea->planPowerRate = std::min(pArea->planpower / sumP * 100, 100.0f);
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
 * @brief ��json��ʽ��ӡ
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
 * @brief ��json��ʽ��ӡ
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
 * @brief ��json��ʽ��ӡ
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
 * @brief ����Ƿ����
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
    deviceTbl.GetStructTime(&tmptime);
    if (tmptime != deviceTblTime)
    {
        deviceTblTime = tmptime;
        chgflag = 1;
    }

    if (chgflag)
    {
        init();
    }
}

/**
 * @brief ��ʼ���ִ����ϵ
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
    // Ԥ����ռ䣬����Ƶ�����ڴ������ͷ�
    roundItemMap.reserve(roundTbl.GetRecordCount());
    areaItemMap.reserve(areaTbl.GetRecordCount());
    roundTypeItemMap.reserve(roundStaTbl.GetRecordCount());
    roundTypeRoundMap.reserve(roundStaTbl.GetRecordCount());
    deviceItemMap.reserve(deviceTbl.GetRecordCount());

    for (i = 0; i < nRoundItemCount; i++)
    {
        TDZJZ_ROUNDITEM *pRoundItem = (TDZJZ_ROUNDITEM *)roundItemTbl.GetRecordAddr(i);
        if (pRoundItem == nullptr)
            continue;
        roundItemMap[pRoundItem->roundid].push_back(pRoundItem);
        areaItemMap[pRoundItem->areaid].push_back(pRoundItem);
        areaItemMap[ALL_AREA_ID].push_back(pRoundItem);
        roundTypeItemMap[pRoundItem->roundtype].push_back(pRoundItem);
        deviceItemMap[pRoundItem->deviceid].push_back(pRoundItem);
    }

    int nRoundCount = roundTbl.GetRecordCount();
    for (i = 0; i < nRoundCount; i++)
    {
        TDZJZ_ROUND *pRound = (TDZJZ_ROUND *)roundTbl.GetRecordAddr(i);
        roundTypeRoundMap[pRound->roundtype].push_back(pRound);
    }

    pAllArea = (TDZJZ_AREA *)areaTbl.SearchRcdAddrByKey((void *)&ALL_AREA_ID);

    dzjzk = GetProgramRunPara("dzjzk", 1.0f);
    dzjzMaxP = GetProgramRunPara("dzjzmax", 0.0f);
}

// ��ȡ�й��ܼ�
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
 * @brief ������������
 *
 */
void DZJZ_CalcTool::judgeRequire(TDZJZ_ROUND *pRound)
{
    if (pRound == nullptr)
        return;

    intertime now;
    get_intertime(&now);
    int seconds = now - pRound->lastAlarm;

    float loadControl;
    // ��������üƻ���󸺺�
    if (!ISZERO(dzjzMaxP))
    {
        // ���㸺�ɿ����� = ��ʵ��Ͷ���к���/�ܸ��ɣ�/(�ƻ��к���/�ƻ��������ֵ��
        loadControl = (pRound->judgepower / sumP) / (pRound->requirePower / dzjzMaxP) * 100;
    }
    else
    {
        loadControl = (pRound->judgepower / pRound->requirePower) * 100;
    }

    int nVal;
    if (loadControl >= 100 && loadControl <= 105)
    {
        nVal = 0; // ����
    }
    else if (loadControl < 100)
    {
        nVal = 1; // ����
    }
    else if (loadControl > 105)
    {
        nVal = 2; // ����
    }

    if (pRound->judgeRequire != nVal)
    {
        pRound->judgeRequire = nVal;
        pRound->lastAlarm = now;
        RdbBackupTable(MyUserName, MyPassWord, DZJZ_ROUND_TBLNAME);

        if (nVal == 0)
        {
            DFLOG_INFO("round %d ���� Ͷ���к���[%f] Ӧ�к���[%f]", pRound->id, pRound->judgepower, pRound->requirePower);
            dzjzEnt.make_capacity_event(pRound, 0);
        }
        else if (nVal == 1)
        {
            DFLOG_INFO("round %d ���� Ͷ���к���[%f] Ӧ�к���[%f]", pRound->id, pRound->judgepower, pRound->requirePower);
            dzjzEnt.make_capacity_event(pRound, 1);
        }
        else
        {
            DFLOG_INFO("round %d ���� Ͷ���к���[%f] Ӧ�к���[%f]", pRound->id, pRound->judgepower, pRound->requirePower);
            dzjzEnt.make_capacity_event(pRound, 1);
        }
    }
    else
    {
        if (seconds >= ALARM_OVER_TIME && (nVal == 1 || nVal == 2))
        {
            pRound->lastAlarm = now;
            dzjzEnt.make_capacity_event(pRound, 1);
            if (nVal == 1)
            {
                DFLOG_INFO("round %d ���� Ͷ���к���[%f] Ӧ�к���[%f]", pRound->id, pRound->judgepower, pRound->requirePower);
                dzjzEnt.make_capacity_event(pRound, 1);
            }
            else
            {
                DFLOG_INFO("round %d ���� Ͷ���к���[%f] Ӧ�к���[%f]", pRound->id, pRound->judgepower, pRound->requirePower);
                dzjzEnt.make_capacity_event(pRound, 1);
            }
        }
    }
}