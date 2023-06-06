/**
 * @file protocol.h
 * @author ����
 * @brief
 * @version 0.1
 * @date 2023-04-21
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "netsub.h"
#include "protdef.h"
#include "fertcmdef.h"
#include "protapp.h"

#include <qobject.h>

class CProtocol : public QObject
{
    Q_OBJECT
public:
    CProtocol();
    virtual ~CProtocol();

    void SetSrvData(EXCHANGE_INFO *pExchange, mail_box *pMailBox, FIXED_VALUE *pFixedValue);

    virtual void OnTimer(int nIDEvent);
    virtual int SendCallAreaNoMail() { return -1; }
    virtual int SendCallValueMail(int iAreaNo = 0) { return -1; };
    virtual int SendCallRYBMail() { return -1; }

    bool isRecvOver() { return m_bRecvOver; }
    // ���ʼ�����ǰ�û�
    int SendMsgToAfert();
    // �趨����״̬
    int SetProcState(int state);
    // ���ʼ�
    bool HaveMail();
    void setRecvOver(bool flag = true) { m_bRecvOver = flag; }
    virtual void SaveValueToDB();
    virtual int GetCurAreaNo(){};
    virtual void MakeSaveValue(std::vector<DZJZ_FIXVALUE> &vec){};

protected:
    bool m_bHaveMail;
    bool m_bRecvOver;                       // ���ν��ս�����־
    int m_procState;                        // ����״̬
    mail_box *m_pMailBox;                   // �ʲ�
    EXCHANGE_INFO *m_pExchange;             // ����͹�Լ֮��Ľ�����Ϣ
    FIXED_VALUE *m_pFixedValue;             // ��ֵ��Ϣ
    int m_boardNum;                         // ����
    BOARD_INFO m_pBoardInfo[MAX_BOARD_NUM]; // ����Ϣ
    int m_catchedNum;                       // �ٻ��õ��Ķ�ֵ����
};

#endif // PROTOCOL_H
