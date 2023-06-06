/**
 * @file protapp.h
 * @author 巩拓
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
    CProtDB m_baseDB;                         // 基本数据库
    mail_box m_mailBox;                       // 发往前置
    int m_curProtNum;                         // 当前RTU保护参数个数
    EXCHANGE_INFO m_exchange;                 // 界面和规约之间的交换信息
    FIXED_VALUE m_pFixedValue[MAX_VALUE_NUM]; // 定值
    const PCPROT_PARA *m_pCurProt;            // 微机保护参数
    int m_nRunAreaNo;                         // 当前运行定值区
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