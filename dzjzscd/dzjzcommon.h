#ifndef DZJZCOMMON_H
#define DZJZCOMMON_H

#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

#include "dorbasedef.h"
#include "lendefine.h"
#include "rdbscd.h"
#include "sybase.h"

enum DZJZ_LOADTYPE
{
    HIGS_RISK_USERS,                 // 高危用户
    IMPORTANT_USERS,                 // 重要用户
    HIGH_PROPORTION_OF_LIVELIHOODM,  // 高比例民生
    MEDIUM_PROPORTION_OF_LIVELIHOOD, // 中比例民生
    LOW_PROPORTION_OF_LIVELIHOOD,    // 低比例民生
    PURE_INDUSTRIAL_LOAD             // 纯工业负荷
};

const int ALL_AREA_ID = 1;

extern const char *s_stapArr[];

/**
 * @brief 低周减载轮次项表结构
 *
 */
typedef struct
{
    int id;                         // 轮次项编号
    char name[256];                 // 轮次项名称
    uchar strapreal;                // 功能压板实际状态
    uchar strapjudge;               // 功能压板状态研判
    float realvalue;                // 频率或电压实际定值
    float realtime;                 // 动作延时实际定值
    uchar valuejudge;               // 定值研判
    float pvalue;                   // 有功功率
    uchar activejudge;              // 有源馈线研判
    uchar planjudge;                // 减载方案研判
    uchar funcjudge;                // 功能研判
    float judgepower;               // 投运切荷量
    uchar breakstate;               // 开关状态
    char linkedbreak[DEF_CODE_LEN]; // 关联开关
    float planvalue;                // 频率或电压整定值
    float plantime;                 // 动作延时整定值
    char realvalueid[DEF_CODE_LEN]; // 频率或电压定值id
    char realtimeid[DEF_CODE_LEN];  // 动作延时定值id
    uchar loadtype;                 // 负荷类型
    uchar strapplan;                // 投退计划
    char strapid[256];              // 压板id
    char alarmid[256];              // 告警信号id
    int roundid;                    // 所属轮次
    uchar devalarm;                 // 装置告警
    int areaid;                     // 所属分区
    char staid[SUBSTATION_LEN];     // 所属变电站
    float planpower;                // 计划切荷量
    char roundname[256];            // 轮次名称
    char feedername[256];           // 馈线名称
    int roundtype;                  // 轮次类型
    float standbyPower;             // 备用切荷量
    float planStandbyPower;         // 计划备用切荷量
    char actionid[DEF_CODE_LEN];    // 动作信号id
    int deviceid;                   // 装置id
    char devicename[256];           // 装置名称
    uchar functype;                 // 轮次功能类型
    uchar devtype;                  // 装置类型
    char pname[24];                 // 有功代码
    int lastAalrm;                  // 上次告警时间
} TDZJZ_ROUNDITEM;

/**
 * @brief 低周减载轮次表结构
 *
 */
typedef struct
{
    int id;                 // 编号
    char name[256];         // 名称
    uchar funtype;          // 功能类型
    int roundtype;          // 轮类型
    float judgepower;       // 投运切荷量
    float planpower;        // 计划切荷量
    float requirePower;     // 应切荷量
    float issuePower;       // 下发切荷量
    float standbyPower;     // 备用切荷量
    float planStandbyPower; // 计划备用切荷量
    uchar judgeRequire;     // 投运切荷量研判
    int lastAlarm;          // 上次告警时间
} TDZJZ_ROUND;

/**
 * @brief 低周减载轮次项表结构
 *
 */
class TDZJZ_ROUNDITEM_HISTORY
{
public:
    int time;                   // 时间
    int id;                     // 轮次项编号
    char name[256];             // 轮次项名称
    uchar strapreal;            // 功能压板实际状态
    float pvalue;               // 有功功率
    uchar loadtype;             // 负荷类型
    uchar strapplan;            // 投退计划
    int areaid;                 // 所属分区
    char staid[SUBSTATION_LEN]; // 所属变电站
    int roundtype;              // 轮次类型

    TDZJZ_ROUNDITEM_HISTORY() = default;

    /**
     * @brief 构造函数
     *
     * @param time
     * @param item
     */
    TDZJZ_ROUNDITEM_HISTORY(int time, const TDZJZ_ROUNDITEM &item)
    {
        this->time = time;
        this->id = item.id;
        strcpy(this->name, item.name);
        this->strapreal = item.strapreal;
        this->pvalue = item.pvalue;
        this->loadtype = item.loadtype;
        this->strapplan = item.strapplan;
        this->areaid = item.areaid;
        strcpy(this->staid, item.staid);
        this->roundtype = item.roundtype;
        // DFLOG_DEBUG("{带参构造函数:time=%d, id=%d}", this->time, this->id);
    }
};

/**
 * @brief 低周减载轮次统计结构
 *
 */
typedef struct
{
    int id;                   // 编号
    char name[256];           // 名称
    float judgepower;         // 投运切荷量
    float planpower;          // 计划切荷量
    int realFeederNum;        // 实际投运馈线数
    int planFeederNum;        // 计划投运馈线数
    float realFeederRate;     // 实际馈线投运率
    float planFeederRate;     // 计划馈线投运率
    float requirePower;       // 应切荷量
    float standbyPower;       // 备用切荷量
    float planStandbyPower;   // 计划备用切荷量
    float realCompRate;       // 实际投运完成率
    float planCompRate;       // 计划投运完成率
    float realPowerRate;      // 实际投运总负荷占比
    float planPowerRate;      // 计划投运总负荷占比
    int realStandbyFeederNum; // 实际备用馈线数
    int planStandbyFeederNum; // 计划备用馈线数
} TDZJZ_ROUNDSTA;

/**
 * @brief 低周减载轮次表结构
 *
 */
class TDZJZ_ROUNDSTA_HISTORY
{
public:
    int time;                 // 编号
    int id;                   // 编号
    float judgepower;         // 投运切荷量
    float planpower;          // 计划切荷量
    int realFeederNum;        // 实际投运馈线数
    int planFeederNum;        // 计划投运馈线数
    float realFeederRate;     // 实际馈线投运率
    float planFeederRate;     // 计划馈线投运率
    float requirePower;       // 应切荷量
    float standbyPower;       // 备用切荷量
    float planStandbyPower;   // 计划备用切荷量
    float realCompRate;       // 实际投运完成率
    float planCompRate;       // 计划投运完成率
    float realPowerRate;      // 实际投运总负荷占比
    float planPowerRate;      // 计划投运总负荷占比
    int realStandbyFeederNum; // 实际备用馈线数
    int planStandbyFeederNum; // 计划备用馈线数
    TDZJZ_ROUNDSTA_HISTORY() = default;
    /**
     * @brief 构造函数
     *
     * @param time
     * @param data
     */
    TDZJZ_ROUNDSTA_HISTORY(int time, const TDZJZ_ROUNDSTA &data)
    {
        this->time = time;
        this->id = data.id;
        this->judgepower = data.judgepower;
        this->planpower = data.planpower;
        this->realFeederNum = data.realFeederNum;
        this->planFeederNum = data.planFeederNum;
        this->realFeederRate = data.realFeederRate;
        this->planFeederRate = data.planFeederRate;
        this->requirePower = data.requirePower;
        this->standbyPower = data.standbyPower;
        this->planStandbyPower = data.planStandbyPower;
        this->realCompRate = data.realCompRate;
        this->planCompRate = data.planCompRate;
        this->realPowerRate = data.realPowerRate;
        this->planPowerRate = data.planPowerRate;
        this->realStandbyFeederNum = data.realStandbyFeederNum;
        this->planStandbyFeederNum = data.planStandbyFeederNum;
    };
};

/**
 * @brief 低周减载轮次表结构
 *
 */
class TDZJZ_ROUND_HISTORY
{
public:
    int time;               // 时间
    int id;                 // 编号
    float judgepower;       // 投运切荷量
    float planpower;        // 计划切荷量
    float requirePower;     // 应切荷量
    float issuePower;       // 下发切荷量
    float standbyPower;     // 备用切荷量
    float planStandbyPower; // 计划备用切荷量
    uchar judgeRequire;     // 投运切荷量研判

    TDZJZ_ROUND_HISTORY() = default;
    TDZJZ_ROUND_HISTORY(int time, const TDZJZ_ROUND &data)
    {
        this->time = time;
        this->id = data.id;
        this->judgepower = data.judgepower;
        this->planpower = data.planpower;
        this->requirePower = data.requirePower;
        this->issuePower = data.issuePower;
        this->standbyPower = data.standbyPower;
        this->planStandbyPower = data.planStandbyPower;
        this->judgeRequire = data.judgeRequire;
    }
};

/**
 * @brief 低周减载区域表结构
 *
 */
typedef struct
{
    int id;                   // 区域id
    char name[256];           // 名称
    uchar type;               // 类型
    float judgepower;         // 投运切荷量
    float planpower;          // 计划切荷量
    int realFeederNum;        // 实际投运馈线数
    int planFeederNum;        // 计划投运馈线数
    float realFeederRate;     // 实际馈线投运率
    float planFeederRate;     // 计划馈线投运率
    float requirePower;       // 应切荷量
    float standbyPower;       // 备用切荷量
    float planStandbyPower;   // 计划备用切荷量
    float realCompRate;       // 实际投运完成率
    float planCompRate;       // 计划投运完成率
    float realPowerRate;      // 实际投运总负荷占比
    float planPowerRate;      // 计划投运总负荷占比
    int realStandbyFeederNum; // 实际备用馈线数
    int planStandbyFeederNum; // 计划备用馈线数
} TDZJZ_AREA;

/**
 * @brief 低周减载区域表结构
 *
 */
class TDZJZ_AREA_HISTORY
{
public:
    int time;                 // 编号
    int id;                   // 区域id
    float judgepower;         // 投运切荷量
    float planpower;          // 计划切荷量
    int realFeederNum;        // 实际投运馈线数
    int planFeederNum;        // 计划投运馈线数
    float realFeederRate;     // 实际馈线投运率
    float planFeederRate;     // 计划馈线投运率
    float requirePower;       // 应切荷量
    float standbyPower;       // 备用切荷量
    float planStandbyPower;   // 计划备用切荷量
    float realCompRate;       // 实际投运完成率
    float planCompRate;       // 计划投运完成率
    float realPowerRate;      // 实际投运总负荷占比
    float planPowerRate;      // 计划投运总负荷占比
    int realStandbyFeederNum; // 实际备用馈线数
    int planStandbyFeederNum; // 计划备用馈线数
    TDZJZ_AREA_HISTORY() = default;
    /**
     * @brief 构造函数
     *
     * @param time
     * @param data
     */
    TDZJZ_AREA_HISTORY(int time, const TDZJZ_AREA &data)
    {
        this->time = time;
        this->id = data.id;
        this->judgepower = data.judgepower;
        this->planpower = data.planpower;
        this->realFeederNum = data.realFeederNum;
        this->planFeederNum = data.planFeederNum;
        this->realFeederRate = data.realFeederRate;
        this->planFeederRate = data.planFeederRate;
        this->requirePower = data.requirePower;
        this->standbyPower = data.standbyPower;
        this->planStandbyPower = data.planStandbyPower;
        this->realCompRate = data.realCompRate;
        this->planCompRate = data.planCompRate;
        this->realPowerRate = data.realPowerRate;
        this->planPowerRate = data.planPowerRate;
        this->realStandbyFeederNum = data.realStandbyFeederNum;
        this->planStandbyFeederNum = data.planStandbyFeederNum;
    };
};

class TDZJZ_ACTION_ROUND_ITEM
{
public:
    int id;                              // ID
    int time;                            // 动作时间
    int roundItemID;                     // 轮次项ID
    char feederName[256];                // 馈线名称
    float pValue;                        // 有功值
    int areaID;                          // 区域ID
    char areaName[256];                  // 区域名称
    int roundType;                       // 轮次类型
    char roundTypeName[256];             // 轮次类型名称
    char actionID[DEF_CODE_LEN];         // 动作ID
    char actionName[256];                // 动作名称
    TDZJZ_ACTION_ROUND_ITEM() = default; // 构造函数
};

/**
 * @brief 低周减载动作表
 *
 */
class TDZJZ_ACTION
{
public:
    int id;                // 动作ID
    int time;              // 动作时间
    char description[256]; // 动作描述
    float areaJudgeValue;  // 地区投运切荷量
    float actionValue;     // 动作切荷量
    float actionPowerRate; // 动作切荷占比
    int actionFeederNum;   // 动作馈线数
    TDZJZ_ACTION() = default;
};

/**
 * @brief 周期巡检任务
 *
 */
class T_PERIOD_INSPECT_TASK
{
public:
    int id;              // 周期巡检任务id
    char name[256];      // 周期巡检任务名称
    int startTime;       // 开始时间
    int endTime;         // 结束时间
    int interval;        // 间隔时间
    char areaId[256];    // 地区id
    char stationId[256]; // 变电站id
    char roundId[256];   // 巡检轮次id
    int lastInspectTime; // 上次巡检时间
};

class T_INSPECT_TASK_HISTORY
{
public:
    int id;             // 巡检结果id
    uchar mode;         // 巡检模式 =0 周期巡检 =1 手动巡检
    int inspectTaskId;  // 周期任务id
    int time;           // 巡检时间
    int uninvestNum;    // 应投未投
    int unbackupNum;    // 应退未退
    int deviceAlarmNum; // 装置异常
    int valueJudgeNum;  // 定值不同
};

class T_INSPECT_ROUNDITEM_HISTORY
{
public:
    int id;             // 巡检结果id
    int roundItemId;    // 轮次项id
    uchar breakState;   // 开关状态
    float pValue;       // 有功功率
    char realValue[16]; // 实际定值
    char planValue[16]; // 计划定值
    uchar strapReal;    // 实际压板状态
    uchar strapPlan;    // 计划压板状态
    uchar strapJudge;   // 压板判断
    uchar deviceAlarm;  // 装置告警
    uchar funcJudge;    // 总告警
};

class T_DZJZ_DZ
{
public:
    short rtuno;
    short type;
    short groupno;
    short no;
    char value[20];
    char desc[160];
    char unit[5];
    char vtype[20];
};

using RoundItemVec = std::vector<TDZJZ_ROUNDITEM *>;
using ItemVecMap = std::unordered_map<int, RoundItemVec>;

using RoundVec = std::vector<TDZJZ_ROUND *>;
using RoundVecMap = std::unordered_map<int, RoundVec>;

extern int ismainserver(void);
extern std::string getFixedValue(float value, float time, int type);
extern std::string getActionName(const char *actionID);
extern int getMaxID(const char *tableName);
extern std::string join(const std::vector<std::string> &items, const std::string &delimiter);

#endif // COMMON_H
