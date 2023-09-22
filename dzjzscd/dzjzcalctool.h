#ifndef DZJZCALCTOOL_H
#define DZJZCALCTOOL_H

#include "dzjzcommon.h"

/**
 * @brief 低周减载轮次项计算类
 *
 */
class DZJZ_CalcTool
{
    static int opentblf;          // 实时库表打开标志
    static RdbTable roundItemTbl; // 轮次项表
    static RdbTable roundTbl;     // 轮次表
    static RdbTable roundStaTbl;  // 轮次统计表
    static RdbTable areaTbl;      // 区域表
    static RdbTable progTbl;      // 程序注册表
    static RdbTable deviceTbl;    // 装置表

    static ItemVecMap roundItemMap;       // 轮次和轮次项关系
    static ItemVecMap areaItemMap;        // 区域和轮次项关系
    static ItemVecMap roundTypeItemMap;   // 轮次类型和轮次项系
    static ItemVecMap deviceItemMap;      // 装置轮次项关系
    static RoundVecMap roundTypeRoundMap; // 轮次类型和轮次关系
    // static RoundVecMap areaRoundMap;      // 区域和轮次关系

    static int roundTblTime;     // 轮次表时间
    static int roundItemTblTime; // 轮次项表时间
    static int roundStaTblTime;  // 轮次统计表时间
    static int areaTblTime;      // 区域时间
    static float dzjzk;          // 低周减载轮次应切荷量K系数
    static int progTblTime;      // 程序注册表加载时间
    static TDZJZ_AREA *pAllArea; // 全网区域
    static float sumP;           // 总有功
    static float dzjzMaxP;       // 年度计划最高负荷
    static int deviceTblTime;    // 装置表时间

public:
    DZJZ_CalcTool(void);
    ~DZJZ_CalcTool(void){};
    void Calc();

private:
    void calcRound();
    void calcArea();
    void calcRoundSta();
    void calcDevice();
    void printRoundJson(TDZJZ_ROUND *pRound);
    void printRoundStaJson(TDZJZ_ROUNDSTA *pRound);
    void printAreaJson(TDZJZ_AREA *pArea);
    void judgeRequire(TDZJZ_ROUND *pRound);
    void checkModify();
    void init();
    float GetSysSumP();
};

#endif