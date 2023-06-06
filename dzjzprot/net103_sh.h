/**
 * @file net103_sh.h
 * @author 巩拓
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
    SELECTMODIFY_SH = 1,  // 预修改
    EXECUTEMODIFY_SH = 2, // 执行修改
    CANCELMODIFY_SH = 3   // 撤销修改
};

// ASDU地址
#ifdef __DF_MSB
typedef struct
{
    WORD DEVADDR : 8; // 设备地址
    WORD AREANO : 5;  // 定值区号
    WORD CPU : 3;     // CPU号
} Sh_AsduAddr;
#else
typedef struct
{
    WORD CPU : 3;     // CPU号
    WORD AREANO : 5;  // 定值区号
    WORD DEVADDR : 8; // 设备地址
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

// 上海保信273定值信息
typedef struct tagNET103_SH_VALUE
{
    char sta[4];
    short protStyle;           // 保护类型
    short group_no;            // 组号
    short no;                  // 序号
    short code_loc;            // 2 位号
    BYTE type;                 // 1 定值类型
    char desc[PROT_DESC_LENS]; // 40描述
    char unit[UNIT_LENS];      // 单位
    float minValue;            // 最小值
    float maxValue;            // 最大值
    float step;                // 步长
    short accN;                // 精度n
    short accM;                // 精度m
} NET103_SH_VALUE;

class CNet103DB_SH : public CProtDB
{
public:
    int m_nValueInfoNum;           // 定值信息个数
    NET103_SH_VALUE *m_pValueInfo; // 定值信息
public:
    CNet103DB_SH();
    virtual ~CNet103DB_SH();
    int LoadValueInfo(const PCPROT_PARA *pProt);                   // 从数据库中得到所选装置类型的定值信息
    const NET103_SH_VALUE *GetValueInfo() { return m_pValueInfo; } // 取定值信息指针
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
    int m_isEnd; // 是否结束
    int m_recOffset;
    int m_totalValueNum;
    int m_iFrameCount;                   // 接收桢计数
    CNet103DB_SH m_DB;                   // 数据库
    int m_nValueInfoNum;                 // 定值参数表中定值信息总数
    int m_nCurValueNum;                  // 当前定值个数
    const NET103_SH_VALUE *m_pValueInfo; // 定值信息指针
public:
    CNet103_SH();
    virtual ~CNet103_SH();

    int SendCallAreaNoMail();
    int SendCallValueMail(int iAreaNo);
    int SendCallRYBMail();
    int GetCurAreaNo();
    void MakeSaveValue(std::vector<DZJZ_FIXVALUE> &vec);

private:
    // 根据序号,位号找定值描述
    bool FindValue(int GroupNo, int IndexNo, int BitNo, FIXED_VALUE *Value);
    short FindVType(int GroupNo, int IndexNo, FIXED_VALUE *Value);
    bool IsMine();
    virtual void OnTimer(int nIDEvent); // 定时查询邮件
    void ReceiveMail();                 // 收到报文
    void RecValue();                    // 接收定值
};
#endif