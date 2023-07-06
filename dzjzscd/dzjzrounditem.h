#pragma once

#include <string>
#include <vector>

#include "dorbase.h"
#include "dzjzcommon.h"

/**
 * @brief ���ܼ����ִ������
 *
 */
class DZJZ_RoundItem
{
    static int opentblf; // ʵʱ���򿪱�־

public:
    static RdbTable rounditemTbl;                                    // �ִ����
    static RdbTable dzjzcdzTbl;                                      // ��ֵ��Ϣ��
    static int roundItemTblTime;                                     // �ִ�������ʱ��
    static int dzjzcdzTblTime;                                       // ��ֵ��Ϣ�����ʱ��
    static std::unordered_map<std::string, T_DZJZ_DZ *> m_dzjzDZMap; // ��ֵ��Ϣmap

public:
    DZJZ_RoundItem(void);
    ~DZJZ_RoundItem(void){};

    void freshData(void); // ��ʱˢ����������
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
