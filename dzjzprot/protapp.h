/**
 * @file protapp.h
 * @author ����
 * @brief
 * @version 0.1
 * @date 2023-04-21
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef _DF_LIB_PROTAPP_H
#define _DF_LIB_PROTAPP_H

#include "netsub.h"
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#include "protdb.h"

class CProtocol;
class CNet103_SH;

enum TPYE
{
    typeDZ = 0,
    typeRYB,
    typeDZQ,
    typeYXQ,
    typeZZPARA,
    typeSTATION
};

class CProtApp
{
public:
    CProtDB m_baseDB;                         // �������ݿ�
    mail_box m_mailBox;                       // ����ǰ��
    int m_curProtNum;                         // ��ǰRTU������������
    EXCHANGE_INFO m_exchange;                 // ����͹�Լ֮��Ľ�����Ϣ
    FIXED_VALUE m_pFixedValue[MAX_VALUE_NUM]; // ��ֵ
    const PCPROT_PARA *m_pCurProt;            // ΢����������
    int m_nRunAreaNo;                         // ��ǰ���ж�ֵ��
    CNet103_SH *m_net103_sh;                  // cxm 23.3.9
public:
    CProtApp();
    ~CProtApp();

    // static CProtApp &getInstance();
    bool InitDBPara();
    void refreshProtList(int rtuno);
    void NewAllProt();
    void FreeAllProt();

    CProtocol *SwitchProtocol(int gene);
    CProtocol *SwitchPcProt(TPYE type);
    CProtocol *SwitchGene(int gene);
    bool containsPattern(const std::string &str, const std::string &pattern);
};

#endif // PROTAPP_H