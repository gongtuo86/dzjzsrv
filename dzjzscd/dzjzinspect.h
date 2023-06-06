#pragma once
#include <atomic>
#include <string>

#include "dfjson/json.h"
#include "dzjzcommon.h"

/**
 * @brief ���ܼ���Ѳ����
 *
 */
class DZJZ_Inspect
{
    static bool isTableOpened;
    static RdbTable periodInspectTbl; // ����Ѳ�������
    static RdbTable roundItemTbl;     // �ִ����
    static int roundItemTblTime;      // �ִ����ʱ��
    static int periodInspectTime;     // ����Ѳ�������ʱ��

    static ItemVecMap roundItemMap;   // �ִ�������������ӳ��
    std::atomic<bool> stopFlag{false};

public:
    DZJZ_Inspect();
    ~DZJZ_Inspect();

    int start();
    void stop();
    int doInspect(const char *subStation, int areaId, int roundId, dfJson::Value &result);

private:
    void checkModify();
    void init();
    int doInspect(T_PERIOD_INSPECT_TASK *pTask, intertime now);
    bool isNeedInspect(T_PERIOD_INSPECT_TASK *pTask, intertime now);
    bool isContain(std::string str, std::string pattern);
    int inspectRoundItem(TDZJZ_ROUNDITEM *pItem, T_INSPECT_ROUNDITEM_HISTORY *pResult);
    int getRoundBaseType(int roundType);
};