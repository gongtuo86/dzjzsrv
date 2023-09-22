#ifndef DZJZCALCTOOL_H
#define DZJZCALCTOOL_H

#include "dzjzcommon.h"

/**
 * @brief ���ܼ����ִ��������
 *
 */
class DZJZ_CalcTool
{
    static int opentblf;          // ʵʱ���򿪱�־
    static RdbTable roundItemTbl; // �ִ����
    static RdbTable roundTbl;     // �ִα�
    static RdbTable roundStaTbl;  // �ִ�ͳ�Ʊ�
    static RdbTable areaTbl;      // �����
    static RdbTable progTbl;      // ����ע���
    static RdbTable deviceTbl;    // װ�ñ�

    static ItemVecMap roundItemMap;       // �ִκ��ִ����ϵ
    static ItemVecMap areaItemMap;        // ������ִ����ϵ
    static ItemVecMap roundTypeItemMap;   // �ִ����ͺ��ִ���ϵ
    static ItemVecMap deviceItemMap;      // װ���ִ����ϵ
    static RoundVecMap roundTypeRoundMap; // �ִ����ͺ��ִι�ϵ
    // static RoundVecMap areaRoundMap;      // ������ִι�ϵ

    static int roundTblTime;     // �ִα�ʱ��
    static int roundItemTblTime; // �ִ����ʱ��
    static int roundStaTblTime;  // �ִ�ͳ�Ʊ�ʱ��
    static int areaTblTime;      // ����ʱ��
    static float dzjzk;          // ���ܼ����ִ�Ӧ�к���Kϵ��
    static int progTblTime;      // ����ע������ʱ��
    static TDZJZ_AREA *pAllArea; // ȫ������
    static float sumP;           // ���й�
    static float dzjzMaxP;       // ��ȼƻ���߸���
    static int deviceTblTime;    // װ�ñ�ʱ��

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