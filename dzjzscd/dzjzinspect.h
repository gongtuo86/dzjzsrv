#pragma once
#include <atomic>
#include <string>

#include "dfjson/json.h"
#include "dzjzcommon.h"

/**
 * @brief 低周减载巡检类
 *
 */
class DZJZ_Inspect
{
    static bool isTableOpened;
    static RdbTable periodInspectTbl; // 周期巡检任务表
    static RdbTable roundItemTbl;     // 轮次项表
    static int roundItemTblTime;      // 轮次项表时间
    static int periodInspectTime;     // 周期巡检任务表时间

    static ItemVecMap roundItemMap;   // 轮次项和周期任务的映射
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