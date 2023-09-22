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
    HIGS_RISK_USERS,                 // ��Σ�û�
    IMPORTANT_USERS,                 // ��Ҫ�û�
    HIGH_PROPORTION_OF_LIVELIHOODM,  // �߱�������
    MEDIUM_PROPORTION_OF_LIVELIHOOD, // �б�������
    LOW_PROPORTION_OF_LIVELIHOOD,    // �ͱ�������
    PURE_INDUSTRIAL_LOAD             // ����ҵ����
};

const int ALL_AREA_ID = 1;

const int ALARM_OVER_TIME = 86400; // �ٴθ澯���

extern const char *s_stapArr[];

/**
 * @brief ���ܼ����ִ����ṹ
 *
 */
typedef struct
{
    int id;                         // �ִ�����
    char name[256];                 // �ִ�������
    uchar strapreal;                // ����ѹ��ʵ��״̬
    uchar strapjudge;               // ����ѹ��״̬����
    float realvalue;                // Ƶ�ʻ��ѹʵ�ʶ�ֵ
    float realtime;                 // ������ʱʵ�ʶ�ֵ
    uchar valuejudge;               // ��ֵ����
    float pvalue;                   // �й�����
    uchar activejudge;              // ��Դ��������
    uchar planjudge;                // ���ط�������
    uchar funcjudge;                // ��������
    float judgepower;               // Ͷ���к���
    uchar breakstate;               // ����״̬
    char linkedbreak[DEF_CODE_LEN]; // ��������
    float planvalue;                // Ƶ�ʻ��ѹ����ֵ
    float plantime;                 // ������ʱ����ֵ
    char realvalueid[DEF_CODE_LEN]; // Ƶ�ʻ��ѹ��ֵid
    char realtimeid[DEF_CODE_LEN];  // ������ʱ��ֵid
    uchar loadtype;                 // ��������
    uchar strapplan;                // Ͷ�˼ƻ�
    char strapid[256];              // ѹ��id
    char alarmid[256];              // �澯�ź�id
    int roundid;                    // �����ִ�
    uchar devalarm;                 // װ�ø澯
    int areaid;                     // ��������
    char staid[SUBSTATION_LEN];     // �������վ
    float planpower;                // �ƻ��к���
    char roundname[256];            // �ִ�����
    char feedername[256];           // ��������
    int roundtype;                  // �ִ�����
    float standbyPower;             // �����к���
    float planStandbyPower;         // �ƻ������к���
    char actionid[DEF_CODE_LEN];    // �����ź�id
    int deviceid;                   // װ��id
    char devicename[256];           // װ������
    uchar functype;                 // �ִι�������
    uchar devtype;                  // װ������
    char pname[24];                 // �й�����
    int lastAalarm;                 // �ϴθ澯ʱ��
    uchar assocexit;                // ��������
    char exitid[DEF_CODE_LEN];      // ����id
    uchar exitjudge;                // ��������
} TDZJZ_ROUNDITEM;

/**
 * @brief ���ܼ����ִα�ṹ
 *
 */
typedef struct
{
    int id;                 // ���
    char name[256];         // ����
    uchar funtype;          // ��������
    int roundtype;          // ������
    float judgepower;       // Ͷ���к���      SUM(�й�*�ƻ�Ͷ��*�����������ִ���)
    float planpower;        // �ƻ�Ͷ���к���  SUM(�й�*�ƻ�Ͷ����ִ���)
    float requirePower;     // Ӧ�к���        �·��ƻ��к���*K
    float issuePower;       // �·��к���       ʡ���·�
    float standbyPower;     // �����к���      SUM(�й�*�ƻ�����*�����������ִ���)
    float planStandbyPower; // �ƻ������к���   SUM(�й�*�ƻ����õ��ִ���)
    uchar judgeRequire;     // Ͷ���к������� =0 ���� =1-���в��� =2 ����
    int lastAlarm;          // �ϴθ澯ʱ��
    float planPowerRate;    // �ƻ��кɱ���     �·��ƻ��кɱ���
    float realPowerRate;    // ʵ���кɱ���      (Ͷ���к���/�ܸ���)*100%
} TDZJZ_ROUND;

/**
 * @brief ���ܼ����ִ����ṹ
 *
 */
class TDZJZ_ROUNDITEM_HISTORY
{
public:
    int time;                   // ʱ��
    int id;                     // �ִ�����
    char name[256];             // �ִ�������
    uchar strapreal;            // ����ѹ��ʵ��״̬
    float pvalue;               // �й�����
    uchar loadtype;             // ��������
    uchar strapplan;            // Ͷ�˼ƻ�
    int areaid;                 // ��������
    char staid[SUBSTATION_LEN]; // �������վ
    int roundtype;              // �ִ�����

    TDZJZ_ROUNDITEM_HISTORY() = default;

    /**
     * @brief ���캯��
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
        // DFLOG_DEBUG("{���ι��캯��:time=%d, id=%d}", this->time, this->id);
    }
};

/**
 * @brief ���ܼ����ִ�ͳ�ƽṹ
 *
 */
typedef struct
{
    int id;                   // ���
    char name[256];           // ����
    float judgepower;         // Ͷ���к���
    float planpower;          // �ƻ��к���
    int realFeederNum;        // ʵ��Ͷ��������
    int planFeederNum;        // �ƻ�Ͷ��������
    float realFeederRate;     // ʵ������Ͷ����
    float planFeederRate;     // �ƻ�����Ͷ����
    float requirePower;       // Ӧ�к���
    float standbyPower;       // �����к���
    float planStandbyPower;   // �ƻ������к���
    float realCompRate;       // ʵ��Ͷ�������
    float planCompRate;       // �ƻ�Ͷ�������
    float realPowerRate;      // ʵ��Ͷ���ܸ���ռ��
    float planPowerRate;      // �ƻ�Ͷ���ܸ���ռ��
    int realStandbyFeederNum; // ʵ�ʱ���������
    int planStandbyFeederNum; // �ƻ�����������
} TDZJZ_ROUNDSTA;

/**
 * @brief ���ܼ����ִα�ṹ
 *
 */
class TDZJZ_ROUNDSTA_HISTORY
{
public:
    int time;                 // ���
    int id;                   // ���
    float judgepower;         // Ͷ���к���
    float planpower;          // �ƻ��к���
    int realFeederNum;        // ʵ��Ͷ��������
    int planFeederNum;        // �ƻ�Ͷ��������
    float realFeederRate;     // ʵ������Ͷ����
    float planFeederRate;     // �ƻ�����Ͷ����
    float requirePower;       // Ӧ�к���
    float standbyPower;       // �����к���
    float planStandbyPower;   // �ƻ������к���
    float realCompRate;       // ʵ��Ͷ�������
    float planCompRate;       // �ƻ�Ͷ�������
    float realPowerRate;      // ʵ��Ͷ���ܸ���ռ��
    float planPowerRate;      // �ƻ�Ͷ���ܸ���ռ��
    int realStandbyFeederNum; // ʵ�ʱ���������
    int planStandbyFeederNum; // �ƻ�����������
    TDZJZ_ROUNDSTA_HISTORY() = default;
    /**
     * @brief ���캯��
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
 * @brief ���ܼ����ִα�ṹ
 *
 */
class TDZJZ_ROUND_HISTORY
{
public:
    int time;               // ʱ��
    int id;                 // ���
    float judgepower;       // Ͷ���к���
    float planpower;        // �ƻ��к���
    float requirePower;     // Ӧ�к���
    float issuePower;       // �·��к���
    float standbyPower;     // �����к���
    float planStandbyPower; // �ƻ������к���
    uchar judgeRequire;     // Ͷ���к�������
    float planPowerRate;    // �ƻ��кɱ���
    float realPowerRate;    // ʵ���кɱ���

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
        this->planPowerRate = data.planPowerRate;
        this->realPowerRate = data.realPowerRate;
    }
};

/**
 * @brief ���ܼ��������ṹ
 *
 */
typedef struct
{
    int id;                   // ����id
    char name[256];           // ����
    uchar type;               // ����
    float judgepower;         // Ͷ���к���
    float planpower;          // �ƻ��к���
    int realFeederNum;        // ʵ��Ͷ��������
    int planFeederNum;        // �ƻ�Ͷ��������
    float realFeederRate;     // ʵ������Ͷ����
    float planFeederRate;     // �ƻ�����Ͷ����
    float requirePower;       // Ӧ�к���
    float standbyPower;       // �����к���
    float planStandbyPower;   // �ƻ������к���
    float realCompRate;       // ʵ��Ͷ�������
    float planCompRate;       // �ƻ�Ͷ�������
    float realPowerRate;      // ʵ��Ͷ���ܸ���ռ��
    float planPowerRate;      // �ƻ�Ͷ���ܸ���ռ��
    int realStandbyFeederNum; // ʵ�ʱ���������
    int planStandbyFeederNum; // �ƻ�����������
} TDZJZ_AREA;

/**
 * @brief ���ܼ��������ṹ
 *
 */
class TDZJZ_AREA_HISTORY
{
public:
    int time;                 // ���
    int id;                   // ����id
    float judgepower;         // Ͷ���к���
    float planpower;          // �ƻ��к���
    int realFeederNum;        // ʵ��Ͷ��������
    int planFeederNum;        // �ƻ�Ͷ��������
    float realFeederRate;     // ʵ������Ͷ����
    float planFeederRate;     // �ƻ�����Ͷ����
    float requirePower;       // Ӧ�к���
    float standbyPower;       // �����к���
    float planStandbyPower;   // �ƻ������к���
    float realCompRate;       // ʵ��Ͷ�������
    float planCompRate;       // �ƻ�Ͷ�������
    float realPowerRate;      // ʵ��Ͷ���ܸ���ռ��
    float planPowerRate;      // �ƻ�Ͷ���ܸ���ռ��
    int realStandbyFeederNum; // ʵ�ʱ���������
    int planStandbyFeederNum; // �ƻ�����������
    TDZJZ_AREA_HISTORY() = default;
    /**
     * @brief ���캯��
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
    int time;                            // ����ʱ��
    int roundItemID;                     // �ִ���ID
    char feederName[256];                // ��������
    float pValue;                        // �й�ֵ
    int areaID;                          // ����ID
    char areaName[256];                  // ��������
    int roundType;                       // �ִ�����
    char roundTypeName[256];             // �ִ���������
    char actionID[DEF_CODE_LEN];         // ����ID
    char actionName[256];                // ��������
    int deviceID;                        // װ��ID
    char deviceName[256];                // װ������
    uchar deviceType;                    // װ������
    uchar roundFuncType;                 // �ִι�������
    char ofSta[SUBSTATION_LEN];          // ������վ
    TDZJZ_ACTION_ROUND_ITEM() = default; // ���캯��
};

/**
 * @brief ���ܼ��ض�����
 *
 */
class TDZJZ_ACTION
{
public:
    int id;                // ����ID
    int time;              // ����ʱ��
    char description[256]; // ��������
    float areaJudgeValue;  // ����Ͷ���к���
    float actionValue;     // �����к���
    float actionPowerRate; // �����к�ռ��
    int actionFeederNum;   // ����������
    TDZJZ_ACTION() = default;
};

/**
 * @brief ����Ѳ������
 *
 */
class T_PERIOD_INSPECT_TASK
{
public:
    int id;              // ����Ѳ������id
    char name[256];      // ����Ѳ����������
    int startTime;       // ��ʼʱ��
    int endTime;         // ����ʱ��
    int interval;        // ���ʱ��
    char areaId[256];    // ����id
    char stationId[256]; // ���վid
    char roundId[256];   // Ѳ���ִ�id
    int lastInspectTime; // �ϴ�Ѳ��ʱ��
};

class T_INSPECT_TASK_HISTORY
{
public:
    int id;             // Ѳ����id
    uchar mode;         // Ѳ��ģʽ =0 ����Ѳ�� =1 �ֶ�Ѳ��
    int inspectTaskId;  // ��������id
    int time;           // Ѳ��ʱ��
    int uninvestNum;    // ӦͶδͶ
    int unbackupNum;    // Ӧ��δ��
    int deviceAlarmNum; // װ���쳣
    int valueJudgeNum;  // ��ֵ��ͬ
};

class T_INSPECT_ROUNDITEM_HISTORY
{
public:
    int id;             // Ѳ����id
    int roundItemId;    // �ִ���id
    uchar breakState;   // ����״̬
    float pValue;       // �й�����
    char realValue[16]; // ʵ�ʶ�ֵ
    char planValue[16]; // �ƻ���ֵ
    uchar strapReal;    // ʵ��ѹ��״̬
    uchar strapPlan;    // �ƻ�ѹ��״̬
    uchar strapJudge;   // ѹ���ж�
    uchar deviceAlarm;  // װ�ø澯
    uchar funcJudge;    // �ܸ澯
};

class TDZJZ_DZ
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

class TDZJZ_DEVICE
{
public:
    int id;
    char name[256];
    char ofSta[SUBSTATION_LEN];
    uchar type;
    uchar alarm;
    uchar sfRet;
    uchar sfReason;
    uchar diRet;
    uchar diReason;
    uchar sgRet;
    uchar sgReason;
};

using RoundItemVec = std::vector<TDZJZ_ROUNDITEM *>;
using ItemVecMap = std::unordered_map<int, RoundItemVec>;

using RoundVec = std::vector<TDZJZ_ROUND *>;
using RoundVecMap = std::unordered_map<int, RoundVec>;

extern int ismainserver(void);
extern std::string getFixedValue(float value, float time, int type);
extern std::string getActionName(const char *actionID);
extern std::string getStrapName(int nRtuNo, const char *id);
extern int getMaxID(const char *tableName);
extern std::string join(const std::vector<std::string> &items, const std::string &delimiter);
extern void splitString(const char *str, std::vector<std::string> &ids, char delim);

#endif // COMMON_H
