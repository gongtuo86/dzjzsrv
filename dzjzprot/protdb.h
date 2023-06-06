/**
 * @file protdb.h
 * @author ����
 * @brief
 * @version 0.1
 * @date 2023-04-21
 *
 * @copyright Copyright (c) 2023
 *
 */
#if !defined(_PROTDB_H)
#define _PROTDB_H

#include <qobject.h>
#include "protdef.h"
#include "sybase.h"

class CProtDB : public QObject
{
    Q_OBJECT
public:
    CProtDB();
    virtual ~CProtDB();

public:
    // �������ݿ������
    int StartDB();
    int Isql(char *isqlcmd, void **pBuff);

    // װ�س�վ������
    // ���ô˺������벻Ҫ����LoadPara()
    // ����ֱ�ӵ���LoadPcProtPara()��װ��ָ����վ��΢������������
    // ע��!!!!! ÿ�ε��ô˺����󶼱����ٴε���LoadPcProtPara()
    int LoadStationPara();
    int LoadRtuPara(); // װ��RTU������
    // װ��΢������������
    // ע��!!!!! ���ô˺���ǰ���ȵ���LoadRtuPara()
    // name = NULL װ������RTU��΢����������
    // ֻװ��ָ����վ��΢����������
    int LoadPcProtPara();

    const RTU_PARA *GetRtuPara() { return m_pRtuPara; } // ��ȡ��΢��������RTU�Ĳ���
    int GetRtuNum() { return m_nRtuNum; }               // ��ȡ�б���������RTU��

    // rtu_no: RTU��
    // pprot_num: ָ�򷵻�΢����������������ָ��
    // ����ֵ��ָ�����ڴ�RTU��΢����������
    const PCPROT_PARA *GetPcProtPara(short rtu_no, int *pcprot_num);

    static bool DeleteDZRecords(short nRtuNo, short addr, short groupno);
    static void SaveDZRecords(std::vector<DZJZ_FIXVALUE> &recordList);

protected:
    int m_dbStatus;                   // ���ݿ�״̬
    int m_nRtuNum;                    // ��΢������������RTU����
    RTU_PARA *m_pRtuPara;             // RTU����
    int m_nStationNum;                // ��΢�����������ĳ�վ����
    CHZ_PARA *m_pStationPara;         // ��վ����
    int *m_protNumArray;              // ���水RTU�����΢��������������������
    P_PCPROT_PARA *m_pcProtParaArray; // ���水RTU�����΢������������ָ������

private:
    void FreeProtPara();
};

#endif
