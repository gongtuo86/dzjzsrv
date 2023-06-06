#include "dzjzinspect.h"

#include "dfjson/json.h"
#include "dflogger.h"
#include "dzjzrounditem.h"
#include "dzjzsrv.h"

bool DZJZ_Inspect::isTableOpened = false; // ʵʱ���򿪱�־
RdbTable DZJZ_Inspect::periodInspectTbl;  // ����Ѳ�������
RdbTable DZJZ_Inspect::roundItemTbl;      // �ִ����
int DZJZ_Inspect::roundItemTblTime = 0;   // �ִ����ʱ��
int DZJZ_Inspect::periodInspectTime = 0;  // ����Ѳ�������ʱ��
ItemVecMap DZJZ_Inspect::roundItemMap;    // �ִ�������������ӳ��

/**
 * @brief ���캯��
 *
 */
DZJZ_Inspect::DZJZ_Inspect()
{
    DFLOG_INFO("[DZJZ_Inspect] Constructor called.");
    if (!isTableOpened)
    {
        if (periodInspectTbl.Login(MyUserName, MyPassWord) != RDB_OK)
        {
            DFLOG_WARN("[DZJZ_Inspect] Failed to login dzjzperiodinspect.");
            isTableOpened = false;
            return;
        }
        if (periodInspectTbl.OpenTable(DZJZ_PERIOD_INSPECT_TBLNAME) != RDB_OK)
        {
            DFLOG_WARN("[DZJZ_Inspect] Failed to open dzjzperiodinspect.");
            isTableOpened = false;
            return;
        }

        if (roundItemTbl.Login(MyUserName, MyPassWord) != RDB_OK)
        {
            DFLOG_WARN("[DZJZ_Inspect] Failed to login dzjzrounditem.");
            isTableOpened = false;
            return;
        }
        if (roundItemTbl.OpenTable(DZJZ_ROUND_ITEM_TBLNAME) != RDB_OK)
        {
            DFLOG_WARN("[DZJZ_Inspect] Failed to open dzjzrounditem.");
            isTableOpened = false;
            return;
        }

        DFLOG_INFO("[DZJZ_Inspect] Successfully opened tables.");
        isTableOpened = true;
    }
}

/**
 * @brief ��������
 *
 */
DZJZ_Inspect::~DZJZ_Inspect()
{
    DFLOG_INFO("[DZJZ_Inspect] Destructor called.");
    stop();
}

/**
 * @brief ��ʼִ������Ѳ�����
 *
 * @return int
 */
int DZJZ_Inspect::start()
{
    DFLOG_INFO("[DZJZ_Inspect] Starting periodic inspection service.");

    int i = 0;
    int periodInspectNum = 0;

    while (!stopFlag.load())
    {
        // DFLOG_INFO("[DZJZ_Inspect] Checking table modifications.");
        checkModify();

        roundItemTbl.LockTableStruct();
        periodInspectTbl.LockTableStruct();
        T_PERIOD_INSPECT_TASK *pTask = NULL;

        periodInspectNum = periodInspectTbl.GetRecordCount();

        for (i = 0; i < periodInspectNum; i++)
        {
            pTask = (T_PERIOD_INSPECT_TASK *)periodInspectTbl.GetRecordAddr(i);
            if (pTask == nullptr)
            {
                continue;
            }

            intertime now;
            get_intertime(&now);

            // ִ��Ѳ������
            if (isNeedInspect(pTask, now))
            {
                DFLOG_INFO("[DZJZ_Inspect] Performing inspection task %d.", pTask->id);
                doInspect(pTask, now);
            }
        }

        periodInspectTbl.UnlockTable();
        roundItemTbl.UnlockTable();

        DFLOG_INFO("[DZJZ_Inspect] Sleeping for 60 seconds.");
        sleepms(60, 0);
    }

    DFLOG_INFO("[DZJZ_Inspect] Stopping periodic inspection service.");
    return 0;
}

/**
 * @brief ִֹͣ������Ѳ�����
 *
 */
void DZJZ_Inspect::stop()
{
    stopFlag.store(true);
}

/**
 * @brief ���ʵʱ������
 *
 */
void DZJZ_Inspect::checkModify()
{
    DFLOG_INFO("[DZJZ_Inspect] Checking table modifications.");
    intertime tmptime;
    uchar chgflag = 0;

    roundItemTbl.GetStructTime(&tmptime);
    if (tmptime != roundItemTblTime)
    {
        roundItemTblTime = tmptime;
        chgflag = 1;
    }

    periodInspectTbl.GetStructTime(&tmptime);
    if (tmptime != periodInspectTime)
    {
        periodInspectTime = tmptime;
        chgflag = 1;
    }

    if (chgflag)
    {
        DFLOG_INFO("[DZJZ_Inspect] Table modification detected, reinitializing.");
        init();
    }
}

/**
 * @brief ��ʼ��
 *
 */
void DZJZ_Inspect::init()
{
    int i = 0;
    int j = 0;

    roundItemMap.clear();
    int nRoundItemCount = roundItemTbl.GetRecordCount();
    int nPeriodTaskCount = periodInspectTbl.GetRecordCount();
    roundItemMap.reserve(nPeriodTaskCount);

    for (i = 0; i < nPeriodTaskCount; i++)
    {
        std::vector<std::string> itemsToInspect;
        T_PERIOD_INSPECT_TASK *pTask = (T_PERIOD_INSPECT_TASK *)periodInspectTbl.GetRecordAddr(i);
        if (pTask == nullptr)
            continue;
        // DFLOG_DEBUG("[DZJZ_Inspect] task %d vec init start", pTask->id);
        for (j = 0; j < nRoundItemCount; j++)
        {
            TDZJZ_ROUNDITEM *pRoundItem = (TDZJZ_ROUNDITEM *)roundItemTbl.GetRecordAddr(j);
            if (pRoundItem == nullptr)
                continue;
            // ������������վ�������ִ�
            if (strlen(pTask->areaId) == 0 || isContain(pTask->areaId, std::to_string(pRoundItem->areaid)))
            {
                // DFLOG_DEBUG("areaid meet");
                if (strlen(pTask->stationId) == 0 || isContain(pTask->stationId, pRoundItem->staid))
                {
                    // DFLOG_DEBUG("stationId meet");
                    if (strlen(pTask->roundId) == 0 || isContain(pTask->roundId, std::to_string(pRoundItem->roundid)))
                    {
                        // DFLOG_DEBUG("roundId meet");
                        roundItemMap[pTask->id].push_back(pRoundItem);
                        itemsToInspect.push_back(std::to_string(pRoundItem->id));
                    }
                }
            }
        }
        DFLOG_INFO("[DZJZ_Inspect] task %d need to inspect item {%s}", pTask->id, join(itemsToInspect, ",").c_str());
        // DFLOG_DEBUG("[DZJZ_Inspect] task vec init end");
    }
}

/**
 * @brief ���������ͻ�ȡ��������
 *
 * @param roundType 1--������ 2--������ 3--������
 * @return int
 */
int DZJZ_Inspect::getRoundBaseType(int roundType)
{
    if (roundType >= 1 && roundType <= 5)
        return 1;
    else if (roundType == 6)
        return 2;
    else
        return 3;
}

/**
 * @brief ִ��Ѳ��
 *
 * @param subStation  ��վ
 * @param areaId ����
 * @param roundBaseType �ִλ�������
 * @param jResult  Ѳ����
 * @return int
 */
int DZJZ_Inspect::doInspect(const char *subStation, int areaId, int roundBaseType, dfJson::Value &jResult)
{
    int i = 0;

    // ���ݳ�վ�������ִλ�ȡ����Ѳ���ִ���
    int nRoundItemCount = roundItemTbl.GetRecordCount();
    std::vector<std::string> itemsToInspect;
    RoundItemVec roundItemVec;

    for (i = 0; i < nRoundItemCount; i++)
    {
        TDZJZ_ROUNDITEM *pRoundItem = (TDZJZ_ROUNDITEM *)roundItemTbl.GetRecordAddr(i);
        if (pRoundItem == nullptr)
            continue;
        // ������������վ�������ִ�
        if (areaId == 0 || areaId == pRoundItem->areaid)
        {
            if (strlen(subStation) == 0 || strcmp(subStation, pRoundItem->staid) == 0)
            {
                if (roundBaseType == 0 || (roundBaseType == getRoundBaseType(pRoundItem->roundtype)))
                {
                    roundItemVec.push_back(pRoundItem);
                    itemsToInspect.push_back(std::to_string(pRoundItem->id));
                }
            }
        }
    }
    if (roundItemVec.size() == 0)
    {
        DFLOG_ERROR("[DZJZ_Inspect] roundItemVec.size() == 0.");
        return -1;
    }

    DFLOG_INFO("[DZJZ_Inspect] need to inspect item {%s}", join(itemsToInspect, ",").c_str());

    // Ѳ���ִ���
    intertime curtime;
    T_INSPECT_TASK_HISTORY taskHistory;
    std::vector<T_INSPECT_ROUNDITEM_HISTORY> inspectItemHistoryVec;
    memset(&taskHistory, 0, sizeof(T_INSPECT_TASK_HISTORY));
    taskHistory.id = getMaxID("xopenshdb.dbo.���ܼ���Ѳ������");
    taskHistory.inspectTaskId = 0;
    taskHistory.mode = 1;
    intertime now;
    get_intertime(&now);
    taskHistory.time = now;
    dfJson::Value array(dfJson::arrayValue);

    // Ѳ�������ִ���
    for (auto &item : roundItemVec)
    {
        dfJson::Value jItem;
        T_INSPECT_ROUNDITEM_HISTORY inspectItemHistory;
        memset(&inspectItemHistory, 0, sizeof(T_INSPECT_ROUNDITEM_HISTORY));
        inspectItemHistory.id = taskHistory.id;
        // Ѳ���ִ���
        inspectRoundItem(item, &inspectItemHistory);
        if (item->strapjudge == 1)
            taskHistory.uninvestNum++;
        else if (item->strapjudge == 2)
            taskHistory.unbackupNum++;

        if (item->devalarm == 1)
            taskHistory.deviceAlarmNum++;

        if (item->valuejudge == 1)
            taskHistory.valueJudgeNum++;

        DFLOG_INFO("[DZJZ_Inspect] �ֶ�Ѳ�� �ִ���:%d ӦͶδͶ:%d Ӧ��δ��:%d װ�ø澯:%d ��ֵ��ͬ:%d", item->id, item->strapjudge == 1, item->strapjudge == 2, item->devalarm, item->valuejudge);

        inspectItemHistoryVec.push_back(inspectItemHistory);

        jItem["id"] = item->id;
        jItem["name"] = item->name;
        jItem["roundName"] = item->roundname;
        jItem["linkedBreak"] = item->linkedbreak;
        jItem["breakState"] = item->breakstate;
        jItem["pValue"] = item->pvalue;
        jItem["realFixValue"] = getFixedValue(item->realvalue, item->realtime, item->functype);
        jItem["planFixValue"] = getFixedValue(item->planvalue, item->plantime, item->functype);
        jItem["realStrap"] = item->strapreal;
        jItem["planStrap"] = item->strapplan;
        jItem["uninvest"] = (item->strapjudge == 1);
        jItem["unbackup"] = (item->strapjudge == 2);
        jItem["judgeValue"] = item->valuejudge;
        jItem["devAlarm"] = item->devalarm;
        jItem["funcJudge"] = item->funcjudge;
        jItem["loadType"] = item->loadtype;
        jItem["devType"] = item->devtype;

        array.append(jItem);
    }

    DFLOG_INFO("[DZJZ_Inspect] �ֶ�Ѳ�� ӦͶδͶ:%d Ӧ��δ��:%d װ�ø澯:%d ��ֵ��ͬ%d", taskHistory.uninvestNum, taskHistory.unbackupNum, taskHistory.deviceAlarmNum, taskHistory.valueJudgeNum);

    // ����Ѳ����
    saveTable(taskHistory, "xopenshdb.dbo.���ܼ���Ѳ������");
    saveTable(inspectItemHistoryVec, "xopenshdb.dbo.���ܼ���Ѳ�������");

    jResult["uninvestNum"] = taskHistory.uninvestNum;
    jResult["unbackupNum"] = taskHistory.unbackupNum;
    jResult["deviceAlarmNum"] = taskHistory.deviceAlarmNum;
    jResult["valueJudgeNum"] = taskHistory.valueJudgeNum;
    jResult["data"] = array;

    return 0;
}

/**
 * @brief ִ�е�������Ѳ������
 *
 * @param pTask
 * @return int
 */
int DZJZ_Inspect::doInspect(T_PERIOD_INSPECT_TASK *pTask, intertime now)
{
    if (pTask == nullptr)
    {
        DFLOG_ERROR("[DZJZ_Inspect] inspectRoundItem called with nullptr arguments.");
        return -1;
    }

    pTask->lastInspectTime = now;

    // �ٻ���ֵ
    // callDZ(pTask);

    // ����pTask->id��ȡroundItemMap��Ӧ���ִ���
    RoundItemVec &roundItemVec = roundItemMap[pTask->id];
    if (roundItemVec.size() == 0)
    {
        DFLOG_ERROR("[DZJZ_Inspect] roundItemVec.size() == 0.");
        return -1;
    }

    intertime curtime;
    T_INSPECT_TASK_HISTORY taskHistory;
    std::vector<T_INSPECT_ROUNDITEM_HISTORY> inspectItemHistoryVec;
    memset(&taskHistory, 0, sizeof(T_INSPECT_TASK_HISTORY));
    taskHistory.id = getMaxID("xopenshdb.dbo.���ܼ���Ѳ������");
    taskHistory.inspectTaskId = pTask->id;
    taskHistory.mode = 0;
    taskHistory.time = now;
    // Ѳ�������ִ���
    for (auto &item : roundItemVec)
    {
        T_INSPECT_ROUNDITEM_HISTORY inspectItemHistory;
        memset(&inspectItemHistory, 0, sizeof(T_INSPECT_ROUNDITEM_HISTORY));
        inspectItemHistory.id = taskHistory.id;
        // Ѳ���ִ���
        inspectRoundItem(item, &inspectItemHistory);
        if (item->strapjudge == 1)
            taskHistory.uninvestNum++;
        else if (item->strapjudge == 2)
            taskHistory.unbackupNum++;

        if (item->devalarm == 1)
            taskHistory.deviceAlarmNum++;

        if (item->valuejudge == 1)
            taskHistory.valueJudgeNum++;

        DFLOG_INFO("[DZJZ_Inspect] Ѳ������:%d �ִ���:%d ӦͶδͶ:%d Ӧ��δ��:%d װ�ø澯:%d ��ֵ��ͬ:%d", pTask->id, item->id, item->strapjudge == 1, item->strapjudge == 2, item->devalarm, item->valuejudge);

        inspectItemHistoryVec.push_back(inspectItemHistory);
    }

    DFLOG_INFO("[DZJZ_Inspect] Ѳ������:%d ӦͶδͶ:%d Ӧ��δ��:%d װ�ø澯:%d ��ֵ��ͬ%d", pTask->id, taskHistory.uninvestNum, taskHistory.unbackupNum, taskHistory.deviceAlarmNum, taskHistory.valueJudgeNum);

    if (ismainserver())
    {
        // ����Ѳ����
        saveTable(taskHistory, "xopenshdb.dbo.���ܼ���Ѳ������");
        saveTable(inspectItemHistoryVec, "xopenshdb.dbo.���ܼ���Ѳ�������");
    }

    return 0;
}

/**
 * @brief �Ƿ���ҪѲ��
 *
 * @param pTask
 * @return true
 * @return false
 */
bool DZJZ_Inspect::isNeedInspect(T_PERIOD_INSPECT_TASK *pTask, intertime now)
{
    if (pTask == nullptr)
    {
        DFLOG_ERROR("[DZJZ_Inspect] isNeedInspect called with nullptr arguments.");
        return false;
    }

    bool hasStarted = now >= pTask->startTime;
    bool hasNotEnded = now <= pTask->endTime || pTask->endTime == 0;
    bool intervalElapsed = (now - pTask->lastInspectTime) > pTask->interval * 60;

    // DFLOG_INFO("[DZJZ_Inspect] hasStarted=%d hasNotEnded=%d intervalElapsed=%d", hasStarted, hasNotEnded, intervalElapsed);

    if (hasStarted && hasNotEnded && intervalElapsed)
    {
        DFLOG_INFO("[DZJZ_Inspect] Inspection needed for task ID %d", pTask->id);
        return true;
    }
    else
    {
        DFLOG_INFO("[DZJZ_Inspect] Inspection not needed for task ID %d", pTask->id);
        return false;
    }
    return false;
}

/**
 * @brief �ж��Ƿ�ȫ��ƥ��
 *
 * @param str
 * @param pattern
 * @return true
 * @return false
 */
bool DZJZ_Inspect::isContain(std::string str, std::string pattern)
{
    std::regex reg;
    reg.assign("\\b" + pattern + "\\b");
    return std::regex_search(str, reg);

    return true;
}

/**
 * @brief �ִ���Ѳ��
 *
 * @param pItem
 * @param pResult
 * @return int
 */
int DZJZ_Inspect::inspectRoundItem(TDZJZ_ROUNDITEM *pItem, T_INSPECT_ROUNDITEM_HISTORY *pResult)
{
    if (pItem == nullptr || pResult == nullptr)
    {
        DFLOG_ERROR("[DZJZ_Inspect] inspectRoundItem called with nullptr arguments.");
        return -1;
    }

    // DFLOG_INFO("[DZJZ_Inspect] Inspecting round item (ID: %d).", pItem->id);

    // ˢ��ֵ
    DZJZ_RoundItem roundItem;
    roundItem.freshRealValue(pItem);

    pResult->roundItemId = pItem->id;
    pResult->breakState = pItem->breakstate;
    pResult->pValue = pItem->pvalue;
    pResult->deviceAlarm = pItem->devalarm;
    pResult->funcJudge = pItem->funcjudge;
    strcpy(pResult->planValue, getFixedValue(pItem->planvalue, pItem->plantime, pItem->functype).c_str());
    strcpy(pResult->realValue, getFixedValue(pItem->realvalue, pItem->realtime, pItem->functype).c_str());
    pResult->strapJudge = pItem->strapjudge;
    pResult->strapPlan = pItem->strapplan;
    pResult->strapReal = pItem->strapreal;

    return 0;
}