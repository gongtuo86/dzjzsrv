/**
 * @file protocol.cpp
 * @author 巩拓
 * @brief
 * @version 0.1
 * @date 2023-04-21
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <iomanip>

#include "protapp.h"
#include "protocol.h"
#include "protdef.h"
#include "dflogger.h"

/**
 * @brief 构造函数
 *
 */
CProtocol::CProtocol()
{
    m_bRecvOver = false;
    m_procState = STATE_NULL;
    m_catchedNum = 0;
    m_bHaveMail = false;
    m_boardNum = 0;
    memset(m_pBoardInfo, 0, sizeof(m_pBoardInfo));

    m_pExchange = NULL;
    m_pMailBox = NULL;
    m_pFixedValue = NULL;
}

/**
 * @brief 析构函数
 *
 */
CProtocol::~CProtocol()
{
}

void CProtocol::SetSrvData(EXCHANGE_INFO *pExchange, mail_box *pMailBox, FIXED_VALUE *pFixedValue)
{
    m_pExchange = pExchange;
    m_pMailBox = pMailBox;
    m_pFixedValue = pFixedValue;
}

/**
 * @brief 定时执行
 *
 * @param nIDEvent
 */
void CProtocol::OnTimer(int nIDEvent)
{
    int Len = 0, ret = 0;
    MAIL_HEAD src, trg;
    BYTE *pBuf = NULL;

    if (m_pMailBox == NULL)
        return;

    ret = m_pMailBox->srv_result(&src, &trg, (void **)&pBuf, &Len);
    if (ret == 0 || pBuf == NULL)
        return;

    m_bHaveMail = true;
    memcpy(m_pExchange->recv, pBuf + 1, Len - 1);
    m_pExchange->lens = Len - 1;

    std::ostringstream logStream;
    logStream << "[recv]:";
    for (int i = 0; i < Len; i++)
    {
        logStream << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<int>(pBuf[i]) << " ";
    }
    DFLOG_INFO("%s", logStream.str().c_str());
}

/**
 * @brief 设置处理状态
 *
 * @param state
 * @return int
 */
int CProtocol::SetProcState(int state)
{
    int last_state = m_procState;

    m_procState = state;

    return last_state;
}

/**
 * @brief 保存数据到数据库
 *
 */
void CProtocol::SaveValueToDB()
{
    std::vector<DZJZ_FIXVALUE> vec;
    MakeSaveValue(vec);

    // 删除原记录
    CProtDB::DeleteDZRecords(m_pExchange->prot_para.rtu_no, m_pExchange->prot_para.type1, m_pExchange->prot_para.addr2);

    // 批量插入
    CProtDB::SaveDZRecords(vec);
}

/**
 * @brief 发送数据到前置
 *
 * @return int
 */
int CProtocol::SendMsgToAfert()
{
    BYTE mailbuf[4096];
    BYTE *str = mailbuf;
    int msglen = 0;

    m_bHaveMail = false;

    memset(mailbuf, 0, sizeof(mailbuf));
    str[0] = MAILPROTCMD;
    str[1] = BYTE(m_procState);
    str[2] = (m_pExchange->prot_para.rtu_no) % 256;
    str[3] = (m_pExchange->prot_para.rtu_no) / 256;
    str[4] = (char)(m_pExchange->prot_para.gui_yue) == PROTTYPE_LFP2 ? PROTTYPE_LFP : (char)(m_pExchange->prot_para.gui_yue);
    str += 8;

    msglen = m_pExchange->lens;
    memcpy(str, m_pExchange->send, msglen);

    std::ostringstream logStream;
    logStream << "[send]";
    for (int i = 0; i < msglen + 8; i++)
    {
        logStream << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<int>(mailbuf[i]) << " ";
    }
    DFLOG_INFO("%s", logStream.str().c_str());

    return m_pMailBox->srv_request("", (void *)mailbuf, msglen + 8, FALSE);
}

/**
 * @brief 是否有邮件
 *
 * @return true
 * @return false
 */
bool CProtocol::HaveMail()
{
    return m_bHaveMail;
}
