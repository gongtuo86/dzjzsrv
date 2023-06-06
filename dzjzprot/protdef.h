/**
 * @file protdef.h
 * @author 巩拓
 * @brief 
 * @version 0.1
 * @date 2023-04-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef PROTDEF_H
#define PROTDEF_H

#include "lendefine.h"
#include "dorbase.h"
#include "dflogger.h"

typedef unsigned char BYTE;
typedef unsigned short WORD;

#define DB_STARTUP 1
#define DB_UNOPENED -1

#define DB_GET_FAILED -100

#define PROT_DESC_LENS 40        // 描述长度
#define UNIT_LENS 5              // 单位
#define PROT_DESC_LENS_LARGE 160 // 长描述的长度 2002.9.11 ++
#define EXCHANGE_BUFF_SIZE 20480 // 10240// 20151110 dcy TJ103保护某模块定值250多个，召唤描述时此处容量不够 1024	//接受发送缓冲区长度 //modify by xjl nr103
#define PROT_CODE_LENS 12        // 代码长度
#define MAX_VALUE_NUM 512        // 256	//定值数组大小 yaz modify for 61850
#define PROT_CZCODE_LENS 4       // 厂站代码长度baihongyan 2012-04-27
#define PROT_CZDESC_LENS 60      // 描述长度baihongyan 2012-04-27

// 进程状态BEGIN
#define STATE_NULL 0               // 空闲状态
#define STATE_LOADVALUE 1          // 正在取定值
#define STATE_LOADVALUE_RETURN 2   // 定值返回
#define STATE_PREMODIFY 3          // 预修改
#define STATE_PREMODIFY_RETURN 4   // 返回返校值
#define STATE_AFFIMMODIFY 5        // 确认修改
#define STATE_CANCELMODIFY 6       // 取消修改
#define STATE_LOADMEASURE 7        // 取测量值
#define STATE_LOADMEASURE_RETURN 8 // 测量值返回
#define STATE_PROTRESET 9          // 发信号复归命令
#define STATE_REC_ACK 10           // 收到Ack
#define STATE_REC_NAK 11           // 收到Nak
#define STATE_MSG_OVER 12          // 报文结束 FOR WBX
#define STATE_HAS_MSG 13           // 有报告	FOR WBX
#define STATE_CALL_YABAN 14        // 查询软压板
#define STATE_PREMODIFY_RETERR 15  // 返校错误

#define MAX_BOARD_NUM 10 // 板个数

// 定值类型
#define NUMERICAL_VALUE 0 // 普通数值型
#define ONOFF_VALUE 1     // 两态型(投入退出, 有效无效)
#define HEX_VALUE 2       // 十六进制
#define STR_VALUE 3       // 字串型
#define INT_VALUE 4       // 整数型

#define AREA_NO 5         // 区号
#define GROUP_NO 6        // 组号
#define HEX_VALUE_LARGE 7 // 十六进制DA

typedef struct tagFIXED_VALUE
{
    int no; // 序号(即定值类型)
    // 2002.9.11 修改 char  desc[PROT_DESC_LENS];	//描述
    char desc[PROT_DESC_LENS_LARGE]; // 描述
    char value_type;                 // 定值数值类型(普通数值型、开关型、十六进制数等...)
    float value;                     // 定值
    char change_flag;                // 定值被修改的标志
    float change_value;              // 修改后的定值
    char proof_flag;                 // 定值是否是返校值
    float proof_value;               // 返校值
    char unit[UNIT_LENS];            // 单位
    char original_flag;
    char str_value[256];        // 字串定值
    char str_change_value[256]; // 修改后的字串定值
    char str_proof_value[256];  // 返校字串值
#ifdef USING_IEC61850_STR       // 201307
    int int_value;              // 整数定值
    int int_change_value;       // 修改后的整数定值
    int int_proof_value;        // 返校整数值
#endif
    float upper_limit; // 上限 bhy added 2014-04-24
    float Low_limit;   // 下限 bhy added 2014-04-24
    int decimalPlaces; // 定值小数点位数

    float range_max;  // 量程最大值 dcy 20151106
    float range_min;  // 量程最小值 dcy 20151106
    float range_step; // 量程步长 dcy 20151106
} FIXED_VALUE;

#ifdef OLD_VER
typedef struct tagPC_PROT_PARA
{
    char code[PROT_CODE_LENS];    // 代码
    char desc[PROT_DESC_LENS];    // 描述
    BYTE security;                // 安全级
    char station[PROT_CODE_LENS]; // 厂站代码
    short rtu_no;                 // RTU号
    char line[PROT_CODE_LENS];    // 线路代码
    short type1;                  // 类型1
    short type2;                  // 类型2
    short gui_yue;                // 规约类型
    BYTE addr1;                   // 地址1
    BYTE addr2;                   // 地址2
    short addr3;                  // 地址3
    int addr4;                    // 起始运行时间
    BYTE type3;                   // 起始状态
    BYTE type4;                   // 是否统计
} PCPROT_PARA, *P_PCPROT_PARA;
#else
typedef struct tagPC_PROT_PARA
{
    char code[PROT_CODE_LENS];    // 代码
    char desc[PROT_DESC_LENS];    // 描述
    char security;                // 安全级
    char station[PROT_CODE_LENS]; // 厂站代码
    short rtu_no;                 // RTU号
    short gui_yue;                // 规约类型
    short type1;                  // 类型1
    short type2;                  // 类型2
    short type3;                  // 类型3
    short type4;                  // 类型4
    short addr1;                  // 地址1
    short addr2;                  // 地址2
    short addr3;                  // 地址3
    short addr4;                  // 地址4
    char stat;                    // 是否统计
    char has_groupno;             // 是否带保护组号
    char bay[JIANGENAME_LEN];     // 间隔代码
} PCPROT_PARA, *P_PCPROT_PARA;
#endif

typedef struct tagEXCHANGE
{
    PCPROT_PARA prot_para;          // 当前所选保护单元的库参数
    int lens;                       // 发送字节数
    uchar recv[EXCHANGE_BUFF_SIZE]; // 从前置机接收数据的缓冲区
    uchar send[EXCHANGE_BUFF_SIZE]; // 发送给前置机的数据缓冲区
} EXCHANGE_INFO;

// RTU参数
typedef struct tagRTU_PARA
{
    short rtuno;               // RTU号
    unsigned char subrtu;      // 是否是二级子站
    char code[PROT_CODE_LENS]; // 代码
    char desc[PROT_DESC_LENS]; // 描述
} RTU_PARA;

// 厂站参数
typedef struct tagCHZ_PARA
{
    int chzno;                   // 厂站号
    char code[PROT_CZCODE_LENS]; // 编号
    char desc[PROT_CZDESC_LENS]; // 名称
} CHZ_PARA;

// 间隔参数
typedef struct tagBAY_PARA
{
    char code[JIANGENAME_LEN];     // 代码
    char desc[JIANGEDESCRIBE_LEN]; // 描述
} BAY_PARA;

typedef struct tagBOARD_INFO
{
    char desc[PROT_DESC_LENS]; // 板描述
    int value_num;             // 每块板的定值数
} BOARD_INFO;

typedef struct
{
    short rtuno;                     // rtu号
    char station[SUBSTATION_LEN];    // 厂站代码
    short gui_yue;                   // 规约类型
    short type;                      // 装置类型
    short groupno;                   // 定值组号
    short no;                        // 定值序号
    char value[20];                  // 定值
    char desc[PROT_DESC_LENS_LARGE]; // 定值描述
    char unit[UNIT_LENS];            // 单位
    char value_type[20];             // 定值类型
    void print()
    {
        //[厂站代码-RTU号-装置类型-规约类型-定值组号](序号):
        DFLOG_INFO("[%s-%d-%d-%d-%d]%d|%s|%s|%s|%s", station, rtuno, type, gui_yue, groupno, no, desc, value, unit, value_type);
    }
} DZJZ_FIXVALUE;

typedef struct
{
    int rtuno;      // rtu号
    char desc[256]; // 描述

} DZJZ_DEVICE;

#endif // PROTDEF_H
