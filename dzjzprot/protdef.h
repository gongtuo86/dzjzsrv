/**
 * @file protdef.h
 * @author ����
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

#define PROT_DESC_LENS 40        // ��������
#define UNIT_LENS 5              // ��λ
#define PROT_DESC_LENS_LARGE 160 // �������ĳ��� 2002.9.11 ++
#define EXCHANGE_BUFF_SIZE 20480 // 10240// 20151110 dcy TJ103����ĳģ�鶨ֵ250������ٻ�����ʱ�˴��������� 1024	//���ܷ��ͻ��������� //modify by xjl nr103
#define PROT_CODE_LENS 12        // ���볤��
#define MAX_VALUE_NUM 512        // 256	//��ֵ�����С yaz modify for 61850
#define PROT_CZCODE_LENS 4       // ��վ���볤��baihongyan 2012-04-27
#define PROT_CZDESC_LENS 60      // ��������baihongyan 2012-04-27

// ����״̬BEGIN
#define STATE_NULL 0               // ����״̬
#define STATE_LOADVALUE 1          // ����ȡ��ֵ
#define STATE_LOADVALUE_RETURN 2   // ��ֵ����
#define STATE_PREMODIFY 3          // Ԥ�޸�
#define STATE_PREMODIFY_RETURN 4   // ���ط�Уֵ
#define STATE_AFFIMMODIFY 5        // ȷ���޸�
#define STATE_CANCELMODIFY 6       // ȡ���޸�
#define STATE_LOADMEASURE 7        // ȡ����ֵ
#define STATE_LOADMEASURE_RETURN 8 // ����ֵ����
#define STATE_PROTRESET 9          // ���źŸ�������
#define STATE_REC_ACK 10           // �յ�Ack
#define STATE_REC_NAK 11           // �յ�Nak
#define STATE_MSG_OVER 12          // ���Ľ��� FOR WBX
#define STATE_HAS_MSG 13           // �б���	FOR WBX
#define STATE_CALL_YABAN 14        // ��ѯ��ѹ��
#define STATE_PREMODIFY_RETERR 15  // ��У����

#define MAX_BOARD_NUM 10 // �����

// ��ֵ����
#define NUMERICAL_VALUE 0 // ��ͨ��ֵ��
#define ONOFF_VALUE 1     // ��̬��(Ͷ���˳�, ��Ч��Ч)
#define HEX_VALUE 2       // ʮ������
#define STR_VALUE 3       // �ִ���
#define INT_VALUE 4       // ������

#define AREA_NO 5         // ����
#define GROUP_NO 6        // ���
#define HEX_VALUE_LARGE 7 // ʮ������DA

typedef struct tagFIXED_VALUE
{
    int no; // ���(����ֵ����)
    // 2002.9.11 �޸� char  desc[PROT_DESC_LENS];	//����
    char desc[PROT_DESC_LENS_LARGE]; // ����
    char value_type;                 // ��ֵ��ֵ����(��ͨ��ֵ�͡������͡�ʮ����������...)
    float value;                     // ��ֵ
    char change_flag;                // ��ֵ���޸ĵı�־
    float change_value;              // �޸ĺ�Ķ�ֵ
    char proof_flag;                 // ��ֵ�Ƿ��Ƿ�Уֵ
    float proof_value;               // ��Уֵ
    char unit[UNIT_LENS];            // ��λ
    char original_flag;
    char str_value[256];        // �ִ���ֵ
    char str_change_value[256]; // �޸ĺ���ִ���ֵ
    char str_proof_value[256];  // ��У�ִ�ֵ
#ifdef USING_IEC61850_STR       // 201307
    int int_value;              // ������ֵ
    int int_change_value;       // �޸ĺ��������ֵ
    int int_proof_value;        // ��У����ֵ
#endif
    float upper_limit; // ���� bhy added 2014-04-24
    float Low_limit;   // ���� bhy added 2014-04-24
    int decimalPlaces; // ��ֵС����λ��

    float range_max;  // �������ֵ dcy 20151106
    float range_min;  // ������Сֵ dcy 20151106
    float range_step; // ���̲��� dcy 20151106
} FIXED_VALUE;

#ifdef OLD_VER
typedef struct tagPC_PROT_PARA
{
    char code[PROT_CODE_LENS];    // ����
    char desc[PROT_DESC_LENS];    // ����
    BYTE security;                // ��ȫ��
    char station[PROT_CODE_LENS]; // ��վ����
    short rtu_no;                 // RTU��
    char line[PROT_CODE_LENS];    // ��·����
    short type1;                  // ����1
    short type2;                  // ����2
    short gui_yue;                // ��Լ����
    BYTE addr1;                   // ��ַ1
    BYTE addr2;                   // ��ַ2
    short addr3;                  // ��ַ3
    int addr4;                    // ��ʼ����ʱ��
    BYTE type3;                   // ��ʼ״̬
    BYTE type4;                   // �Ƿ�ͳ��
} PCPROT_PARA, *P_PCPROT_PARA;
#else
typedef struct tagPC_PROT_PARA
{
    char code[PROT_CODE_LENS];    // ����
    char desc[PROT_DESC_LENS];    // ����
    char security;                // ��ȫ��
    char station[PROT_CODE_LENS]; // ��վ����
    short rtu_no;                 // RTU��
    short gui_yue;                // ��Լ����
    short type1;                  // ����1
    short type2;                  // ����2
    short type3;                  // ����3
    short type4;                  // ����4
    short addr1;                  // ��ַ1
    short addr2;                  // ��ַ2
    short addr3;                  // ��ַ3
    short addr4;                  // ��ַ4
    char stat;                    // �Ƿ�ͳ��
    char has_groupno;             // �Ƿ���������
    char bay[JIANGENAME_LEN];     // �������
} PCPROT_PARA, *P_PCPROT_PARA;
#endif

typedef struct tagEXCHANGE
{
    PCPROT_PARA prot_para;          // ��ǰ��ѡ������Ԫ�Ŀ����
    int lens;                       // �����ֽ���
    uchar recv[EXCHANGE_BUFF_SIZE]; // ��ǰ�û��������ݵĻ�����
    uchar send[EXCHANGE_BUFF_SIZE]; // ���͸�ǰ�û������ݻ�����
} EXCHANGE_INFO;

// RTU����
typedef struct tagRTU_PARA
{
    short rtuno;               // RTU��
    unsigned char subrtu;      // �Ƿ��Ƕ�����վ
    char code[PROT_CODE_LENS]; // ����
    char desc[PROT_DESC_LENS]; // ����
} RTU_PARA;

// ��վ����
typedef struct tagCHZ_PARA
{
    int chzno;                   // ��վ��
    char code[PROT_CZCODE_LENS]; // ���
    char desc[PROT_CZDESC_LENS]; // ����
} CHZ_PARA;

// �������
typedef struct tagBAY_PARA
{
    char code[JIANGENAME_LEN];     // ����
    char desc[JIANGEDESCRIBE_LEN]; // ����
} BAY_PARA;

typedef struct tagBOARD_INFO
{
    char desc[PROT_DESC_LENS]; // ������
    int value_num;             // ÿ���Ķ�ֵ��
} BOARD_INFO;

typedef struct
{
    short rtuno;                     // rtu��
    char station[SUBSTATION_LEN];    // ��վ����
    short gui_yue;                   // ��Լ����
    short type;                      // װ������
    short groupno;                   // ��ֵ���
    short no;                        // ��ֵ���
    char value[20];                  // ��ֵ
    char desc[PROT_DESC_LENS_LARGE]; // ��ֵ����
    char unit[UNIT_LENS];            // ��λ
    char value_type[20];             // ��ֵ����
    void print()
    {
        //[��վ����-RTU��-װ������-��Լ����-��ֵ���](���):
        DFLOG_INFO("[%s-%d-%d-%d-%d]%d|%s|%s|%s|%s", station, rtuno, type, gui_yue, groupno, no, desc, value, unit, value_type);
    }
} DZJZ_FIXVALUE;

typedef struct
{
    int rtuno;      // rtu��
    char desc[256]; // ����

} DZJZ_DEVICE;

#endif // PROTDEF_H
