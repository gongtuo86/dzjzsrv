#include "dzjzrounditem.h"

#include <sstream>

#include "cmdefs.h"
#include "dflogger.h"
#include "dzjzevent.h"
#include "rdbscd.h"

int DZJZ_RoundItem::opentblf = 0;                                         // ʵʱ���򿪱�־
RdbTable DZJZ_RoundItem::rounditemTbl;                                    // �ִ����
RdbTable DZJZ_RoundItem::dzjzcdzTbl;                                      // ��ֵ��Ϣ��
std::unordered_map<std::string, T_DZJZ_DZ *> DZJZ_RoundItem::m_dzjzDZMap; // ��ֵ��Ϣmap
int DZJZ_RoundItem::roundItemTblTime = 0;                                 // �ִ�������ʱ��
int DZJZ_RoundItem::dzjzcdzTblTime = 0;                                   // ��ֵ��Ϣ�����ʱ��

extern DZJZ_Event dzjzEnt;

/**
 * @brief ���캯��
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
 * @brief ��json��ʽ��ӡ
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
 * @brief ˢ���й��Ϳ���״̬
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
 * @brief ˢ��ʵʱֵ
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
    alarmJudge(pItem);
    exitJudge(pItem);
    funcJudge(pItem);
    calcPower(pItem);

    // printItemJson(pItem);
}

/**
 * @brief ˢ���й�
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
 * @brief ˢ�¿���״̬
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
            DFLOG_WARN("���� %s ң�Ų�����", pItem->linkedbreak);
        }
    }
    // DFLOG_DEBUG("���� %s ң��ֵ=%d", pItem->linkedbreak, pItem->breakstate);
}

/**
 * @brief ��Դ��������
 *
 * @param pItem
 */
void DZJZ_RoundItem::activeJudge(TDZJZ_ROUNDITEM *pItem)
{
    if (pItem == nullptr)
        return;

    intertime now;
    get_intertime(&now);
    int seconds = now - pItem->lastAalarm;

    const bool bAlarm = pItem->pvalue <= -1e-7;

    if (pItem->activejudge != bAlarm)
    {
        pItem->activejudge = bAlarm;
        pItem->lastAalarm = now;
        RdbBackupTable(MyUserName, MyPassWord, DZJZ_ROUND_ITEM_TBLNAME);
        if (bAlarm)
        {
            dzjzEnt.make_active_event(pItem, 1);
        }
        else
        {
            dzjzEnt.make_active_event(pItem, 0);
        }
    }
    else
    {
        if (bAlarm && seconds >= ALARM_OVER_TIME)
        {
            pItem->lastAalarm = now;
            dzjzEnt.make_active_event(pItem, 1);
        }
    }
}

/**
 * @brief ��ֵ����
 *
 * @param pItme
 */
void DZJZ_RoundItem::valueJudge(TDZJZ_ROUNDITEM *pItem)
{
    if (pItem == nullptr || pItem->realvalueid[0] == '\0' || pItem->realtimeid[0] == '\0')
    {
        return;
    }

    intertime now;
    get_intertime(&now);
    int seconds = now - pItem->lastAalarm;

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
        pItem->lastAalarm = now;
        RdbBackupTable(MyUserName, MyPassWord, DZJZ_ROUND_ITEM_TBLNAME);
        if (bAlarm)
        {
            dzjzEnt.make_judgevalue_event(pItem, 1);
        }
        else
        {
            dzjzEnt.make_judgevalue_event(pItem, 0);
        }
    }
    else
    {
        if (bAlarm && seconds >= ALARM_OVER_TIME)
        {
            pItem->lastAalarm = now;
            dzjzEnt.make_judgevalue_event(pItem, 1);
        }
    }
}

/**
 * @brief ���ܼ��ط������� �Ƿ�����������
 *
 * @param pItem
 */
void DZJZ_RoundItem::planJudge(TDZJZ_ROUNDITEM *pItem)
{
    if (pItem == nullptr)
        return;

    intertime now;
    get_intertime(&now);
    int seconds = now - pItem->lastAalarm;

    // ֻ�е�һ�ֺ͵ڶ����ж������������澯��ָ�
    if (pItem->roundtype != 1 && pItem->roundtype != 2)
    {
        if (pItem->planjudge == 1)
        {
            pItem->planjudge = 0;
            pItem->lastAalarm = now;
            RdbBackupTable(MyUserName, MyPassWord, DZJZ_ROUND_ITEM_TBLNAME);
            dzjzEnt.make_judgeplan_event(pItem, 0);
        }
        return;
    }

    bool bAlarm = pItem->loadtype <= LOW_PROPORTION_OF_LIVELIHOOD;
    if (bAlarm != pItem->planjudge)
    {
        pItem->planjudge = bAlarm;
        pItem->lastAalarm = now;
        RdbBackupTable(MyUserName, MyPassWord, DZJZ_ROUND_ITEM_TBLNAME);
        if (bAlarm)
        {
            dzjzEnt.make_judgeplan_event(pItem, 1);
        }
        else
        {
            dzjzEnt.make_judgeplan_event(pItem, 0);
        }
    }
    else
    {
        if (bAlarm && seconds >= ALARM_OVER_TIME)
        {
            pItem->lastAalarm = now;
            dzjzEnt.make_judgeplan_event(pItem, 1);
        }
    }
}

/**
 * @brief ѹ��״̬��������
 *
 * @param pItem
 * @param strapJudge
 * @param logMsg
 */
void DZJZ_RoundItem::setStrapJudge(TDZJZ_ROUNDITEM *pItem)
{
    if (pItem == nullptr)
        return;

    intertime now;
    get_intertime(&now);
    int seconds = now - pItem->lastAalarm;

    uchar strapJudge = pItem->strapreal << 1 | pItem->strapplan;
    if (pItem->strapjudge != strapJudge)
    {
        pItem->strapjudge = strapJudge;
        pItem->lastAalarm = now;
        RdbBackupTable(MyUserName, MyPassWord, DZJZ_ROUND_ITEM_TBLNAME);
        dzjzEnt.make_judgefunc_event(pItem);
    }
    else
    {
        if (seconds >= ALARM_OVER_TIME && (strapJudge == 1 || strapJudge == 2))
        {
            pItem->lastAalarm = now;
            dzjzEnt.make_judgefunc_event(pItem);
        }
    }
}

/**
 * @brief ���ܼ��ع���ѹ��״̬����
 *
 * @param pItem
 */
void DZJZ_RoundItem::strapJudge(TDZJZ_ROUNDITEM *pItem)
{
    if (!pItem || pItem->strapid[0] == '\0')
        return;

    std::vector<std::string> strapids;
    splitString(pItem->strapid, strapids, ',');

    uchar tmpVal;
    uchar strapVal = 0x01;
    // ��һ��ѹ��δͶ�룬�Ͳ���Ͷ��
    for (const std::string &strapid : strapids)
    {
        std::string valueKey = std::to_string(pItem->deviceid) + ":" + std::string(pItem->strapid);
        if (m_dzjzDZMap.find(valueKey) != m_dzjzDZMap.end())
        {
            tmpVal = std::stoi(m_dzjzDZMap[valueKey]->value);
        }
        else
        {
            DFLOG_WARN("strap id %s can not find", valueKey.c_str());
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
 * @brief ��ظ澯�źż���
 *
 * @param pItem
 */
void DZJZ_RoundItem::alarmJudge(TDZJZ_ROUNDITEM *pItem)
{
    if (pItem == nullptr || pItem->alarmid[0] == '\0')
        return;

    intertime now;
    get_intertime(&now);
    int seconds = now - pItem->lastAalarm;

    std::vector<std::string> alarmids;
    splitString(pItem->alarmid, alarmids, ',');
    uchar tmpVal;
    Protsig status;

    uchar nAlarm = 0x00;

    // ��һ���澯�źž͸澯
    for (const std::string &alarmId : alarmids)
    {
        int bOK = status.getdata(const_cast<char *>(alarmId.c_str()), &tmpVal);
        if ((bOK == OK) && tmpVal == STAVAL_ON)
        {
            nAlarm = 0x01;
            break;
        }
        else if (bOK != OK)
        {
            DFLOG_WARN("pItem %d can not find alarmid %s", pItem->id, alarmId.c_str());
        }
    }

    if (nAlarm != pItem->devalarm)
    {
        pItem->devalarm = nAlarm;
        RdbBackupTable(MyUserName, MyPassWord, DZJZ_ROUND_ITEM_TBLNAME);
        if (nAlarm == 0x01)
        {
            dzjzEnt.make_alarm_event(pItem, 1);
        }
        else
        {
            dzjzEnt.make_alarm_event(pItem, 0);
        }
    }
    else
    {
        if (nAlarm && seconds >= ALARM_OVER_TIME)
        {
            pItem->lastAalarm = now;
            dzjzEnt.make_alarm_event(pItem, 1);
        }
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
 * @brief �ִ��������
 *
 * @param pItem
 */
void DZJZ_RoundItem::funcJudge(TDZJZ_ROUNDITEM *pItem)
{
    if (pItem == nullptr)
        return;

    intertime now;
    get_intertime(&now);
    int seconds = now - pItem->lastAalarm;

    bool valJudge = false;

    valJudge = !isStrapNormal(pItem->strapjudge) || pItem->valuejudge || pItem->devalarm || pItem->activejudge;

    if (valJudge != pItem->funcjudge)
    {
        pItem->funcjudge = valJudge;
        RdbBackupTable(MyUserName, MyPassWord, DZJZ_ROUND_ITEM_TBLNAME);

        if (valJudge)
        {
            dzjzEnt.make_funcJudge_event(pItem, 1);
        }
        else
        {
            dzjzEnt.make_funcJudge_event(pItem, 0);
        }
    }
    else
    {
        if (valJudge && seconds >= ALARM_OVER_TIME)
        {
            pItem->lastAalarm = now;
            dzjzEnt.make_funcJudge_event(pItem, 1);
        }
    }
}

/**
 * @brief �����к���
 *
 */
void DZJZ_RoundItem::calcPower(TDZJZ_ROUNDITEM *pItem)
{
    // ʵ��Ͷ���к���=���߸���*�ƻ�Ͷ��*�������н��
    pItem->judgepower = pItem->pvalue * pItem->strapplan * (!pItem->funcjudge);

    // �ƻ�Ͷ���к��� = ���߸���*�ƻ�Ͷ��
    pItem->planpower = pItem->pvalue * pItem->strapplan;

    // ʵ�ʱ����к��� = ���߸���*�ƻ�����*�������н��
    pItem->standbyPower = pItem->pvalue * (!pItem->strapplan) * (!pItem->funcjudge);

    // �ƻ������к��� = ���߸���*�ƻ�����
    pItem->planStandbyPower = pItem->pvalue * (!pItem->strapplan);
}

/**
 * @brief ���ܼ��س�������
 *
 * @param pItem
 */
void DZJZ_RoundItem::exitJudge(TDZJZ_ROUNDITEM *pItem)
{
    // ��� pItem �� NULL ���� exitid �ǿյģ�ֱ�ӷ���
    if (pItem == NULL || pItem->exitid[0] == '\0')
    {
        return;
    }

    std::string valueKey = std::to_string(pItem->deviceid) + ":" + std::string(pItem->exitid);

    intertime now;
    get_intertime(&now);
    int elapsedSeconds = now - pItem->lastAalarm; // ��������ı�����

    // ������ m_dzjzDZMap ���ҵ�ֵ
    auto it = m_dzjzDZMap.find(valueKey);
    if (it == m_dzjzDZMap.end())
    {
        pItem->exitjudge = 0;
        DFLOG_WARN("pItem %d cant not find exitid %s.", pItem->id, valueKey.c_str());
        return;
    }

    int actualValue = std::stoi(it->second->value); // ��������ı�����
    int plannedValue = pItem->assocexit - 1;        // ��������ı�����

    // ȡ��Ӧ���ڵ�Ͷ��״̬
    int valBitN = (actualValue >> plannedValue) & 0x01;

    bool isAlarm = (valBitN != 1); // ��������ı�����

    // DFLOG_DEBUG("exitJudge:valueKey=%s actualValue=%d plannedValue=%d valBitN=%d isAlarm=%d",
    //             valueKey.c_str(), actualValue, plannedValue, valBitN, isAlarm);

    if (isAlarm != pItem->exitjudge)
    {
        pItem->exitjudge = isAlarm;
        pItem->lastAalarm = now;
        RdbBackupTable(MyUserName, MyPassWord, DZJZ_ROUND_ITEM_TBLNAME);
        DFLOG_DEBUG("pItem %d actualValue=%d plannedValue=%d", actualValue, plannedValue);
        dzjzEnt.make_exit_event(pItem, isAlarm ? 1 : 0); // ʹ������������򻯴���
    }
    else
    {
        if (isAlarm && elapsedSeconds >= ALARM_OVER_TIME)
        {
            pItem->lastAalarm = now;
            dzjzEnt.make_exit_event(pItem, 1);
        }
    }
}
