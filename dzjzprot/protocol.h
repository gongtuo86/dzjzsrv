/**
 * @file protocol.h
 * @author 巩拓
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
    // 发邮件给主前置机
    int SendMsgToAfert();
    // 设定进程状态
    int SetProcState(int state);
    // 有邮件
    bool HaveMail();
    void setRecvOver(bool flag = true) { m_bRecvOver = flag; }
    virtual void SaveValueToDB();
    virtual int GetCurAreaNo(){};
    virtual void MakeSaveValue(std::vector<DZJZ_FIXVALUE> &vec){};

protected:
    bool m_bHaveMail;
    bool m_bRecvOver;                       // 本次接收结束标志
    int m_procState;                        // 进程状态
    mail_box *m_pMailBox;                   // 邮差
    EXCHANGE_INFO *m_pExchange;             // 界面和规约之间的交换信息
    FIXED_VALUE *m_pFixedValue;             // 定值信息
    int m_boardNum;                         // 板数
    BOARD_INFO m_pBoardInfo[MAX_BOARD_NUM]; // 板信息
    int m_catchedNum;                       // 召唤得到的定值总数
};

#endif // PROTOCOL_H
