/**
 * @file net103_sh.h
 * @author ����
 * @brief
 * @version 0.1
 * @date 2023-04-21
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __NET103_SH_H__
#define __NET103_SH_H__

#include "protdb.h"
#include "protocol.h"

#define CURR_REGION_NO 1

enum
{
    SELECTMODIFY_SH = 1,  // Ԥ�޸�
    EXECUTEMODIFY_SH = 2, // ִ���޸�
    CANCELMODIFY_SH = 3   // �����޸�
};

// ASDU��ַ
#ifdef __DF_MSB
typedef struct
{
    WORD DEVADDR : 8; // �豸��ַ
    WORD AREANO : 5;  // ��ֵ����
    WORD CPU : 3;     // CPU��
} Sh_AsduAddr;
#else
typedef struct
{
    WORD CPU : 3;     // CPU��
    WORD AREANO : 5;  // ��ֵ����
    WORD DEVADDR : 8; // �豸��ַ
} Sh_AsduAddr;
#endif

typedef struct
{
    union
    {
        Sh_AsduAddr AsduAddr;
        WORD WAsduAddr;
    } AsduAddr;
} W_AsduAddr_SH;

// �Ϻ�����273��ֵ��Ϣ
typedef struct tagNET103_SH_VALUE
{
    char sta[4];
    short protStyle;           // ��������
    short group_no;            // ���
    short no;                  // ���
    short code_loc;            // 2 λ��
    BYTE type;                 // 1 ��ֵ����
    char desc[PROT_DESC_LENS]; // 40����
    char unit[UNIT_LENS];      // ��λ
    float minValue;            // ��Сֵ
    float maxValue;            // ���ֵ
    float step;                // ����
    short accN;                // ����n
    short accM;                // ����m
} NET103_SH_VALUE;

class CNet103DB_SH : public CProtDB
{
public:
    int m_nValueInfoNum;           // ��ֵ��Ϣ����
    NET103_SH_VALUE *m_pValueInfo; // ��ֵ��Ϣ
public:
    CNet103DB_SH();
    virtual ~CNet103DB_SH();
    int LoadValueInfo(const PCPROT_PARA *pProt);                   // �����ݿ��еõ���ѡװ�����͵Ķ�ֵ��Ϣ
    const NET103_SH_VALUE *GetValueInfo() { return m_pValueInfo; } // ȡ��ֵ��Ϣָ��
    int GetValueNum() { return m_nValueInfoNum; }
};

class CNet103_SH : public CProtocol
{
public:
    enum PcProt
    {
        STATION = 4,
        RYB = 12,
        ZZPARA = 16,
        DZ = 3,
        YXQ = 11
    };
    int m_isEnd; // �Ƿ����
    int m_recOffset;
    int m_totalValueNum;
    int m_iFrameCount;                   // ���������
    CNet103DB_SH m_DB;                   // ���ݿ�
    int m_nValueInfoNum;                 // ��ֵ�������ж�ֵ��Ϣ����
    int m_nCurValueNum;                  // ��ǰ��ֵ����
    const NET103_SH_VALUE *m_pValueInfo; // ��ֵ��Ϣָ��
public:
    CNet103_SH();
    virtual ~CNet103_SH();

    int SendCallAreaNoMail();
    int SendCallValueMail(int iAreaNo);
    int SendCallRYBMail();
    int GetCurAreaNo();
    void MakeSaveValue(std::vector<DZJZ_FIXVALUE> &vec);

private:
    // �������,λ���Ҷ�ֵ����
    bool FindValue(int GroupNo, int IndexNo, int BitNo, FIXED_VALUE *Value);
    short FindVType(int GroupNo, int IndexNo, FIXED_VALUE *Value);
    bool IsMine();
    virtual void OnTimer(int nIDEvent); // ��ʱ��ѯ�ʼ�
    void ReceiveMail();                 // �յ�����
    void RecValue();                    // ���ն�ֵ
};
#endif