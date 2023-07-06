#pragma once

#include <string>
#include <vector>

#include "dorbase.h"
#include "dzjzcommon.h"

/**
 * @brief 低周减载轮次项处理类
 *
 */
class DZJZ_RoundItem
{
    static int opentblf; // 实时库表打开标志

public:
    static RdbTable rounditemTbl;                                    // 轮次项表
    static RdbTable dzjzcdzTbl;                                      // 定值信息表
    static int roundItemTblTime;                                     // 轮次项表加载时间
    static int dzjzcdzTblTime;                                       // 定值信息表加载时间
    static std::unordered_map<std::string, T_DZJZ_DZ *> m_dzjzDZMap; // 定值信息map

public:
    DZJZ_RoundItem(void);
    ~DZJZ_RoundItem(void){};

    void freshData(void); // 定时刷新研判数据
    void freshRealValue(TDZJZ_ROUNDITEM *pItem);
    void checkModify();

private:
    void freshPValue(TDZJZ_ROUNDITEM *pItem);
    void freshBreakValue(TDZJZ_ROUNDITEM *pItem);
    void activeJudge(TDZJZ_ROUNDITEM *pItem);
    void valueJudge(TDZJZ_ROUNDITEM *pItme);
    void planJudge(TDZJZ_ROUNDITEM *pItem);
    void setStrapJudge(TDZJZ_ROUNDITEM *pItem);
    void strapJudge(TDZJZ_ROUNDITEM *pItem);
    void alarmJudge(TDZJZ_ROUNDITEM *pItem);
    void funcJudge(TDZJZ_ROUNDITEM *pItem);
    void calcPower(TDZJZ_ROUNDITEM *pItem);
    void printItemJson(TDZJZ_ROUNDITEM *pItem);
    void init();
    bool isStrapNormal(int strapJudge);
};
