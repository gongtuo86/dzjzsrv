/**
 * @file protapp.cpp
 * @author ����
 * @brief
 * @version 0.1
 * @date 2023-04-21
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "protapp.h"

#include <signal.h>

#include <regex>

#include "cmdefs.h"
#include "dflogger.h"
#include "net103_sh.h"

// /**
//  * @brief ��ȡ����
//  *
//  * @return CProtApp&
//  */
// CProtApp &CProtApp::getInstance()
// {
//     static CProtApp instance;
//     return instance;
// }

/**
 * @brief ���캯��
 *
 */
CProtApp::CProtApp()
{
    m_mailBox = "RTU";
    m_net103_sh = NULL;
    m_nRunAreaNo = 0;
    m_pCurProt = NULL;

    memset(&m_exchange, 0, sizeof(m_exchange));
    memset(m_pFixedValue, 0, sizeof(m_pFixedValue));

    NewAllProt();
    InitDBPara();
}

/**
 * @brief ��������
 *
 */
CProtApp::~CProtApp()
{
    FreeAllProt();
}

/**
 * @brief ��ʼ������Э��
 *
 */
void CProtApp::NewAllProt()
{
    if (m_net103_sh == NULL)
        m_net103_sh = new CNet103_SH;
}

void CProtApp::FreeAllProt()
{
    if (m_net103_sh != NULL)
    {
        delete m_net103_sh;
        m_net103_sh = NULL;
    }
}

/**
 * @brief ��ʼ������
 *
 * @return true
 * @return false
 */
bool CProtApp::InitDBPara()
{
    int ret = true;

    if (m_baseDB.StartDB() != DB_STARTUP)
    {
        DFLOG_WARN("----------startdb error! ��ʼ��������ʧ��\n");
        ret = false;
    }

    /**********************************************************/

    /***************************ȡ��վ����**************************/
    ret = m_baseDB.LoadStationPara();
    if (ret == DB_GET_FAILED || ret == 0)
    {
        DFLOG_WARN("------------LoadStationPara error!\n");
        ret = false;
    }

    /***************************ȡRTU����**************************/
    ret = m_baseDB.LoadRtuPara();
    if (ret == DB_GET_FAILED || ret == 0)
    {
        DFLOG_WARN("------------LoadRtuPara error!\n");
        ret = false;
    }

    return ret;
}

/**
 * @brief �л�Э��
 *
 * @param gene
 * @return CProtocol*
 */
CProtocol *CProtApp::SwitchProtocol(int gene)
{
    switch (gene)
    {
    case PROTTYPE_NET103_SH:
        return m_net103_sh;
        break;
    }
    return NULL;
}

bool CProtApp::containsPattern(const std::string &str, const std::string &pattern)
{
    std::regex regexPattern(pattern);

    if (std::regex_search(str, regexPattern))
    {
        DFLOG_WARN("str: %s, pattern: %s = true", str.c_str(), pattern.c_str());
        return true; // �ַ���ƥ��������ʽģʽ
    }
    //DFLOG_WARN("str: %s, pattern: %s = false", str.c_str(), pattern.c_str());
    return false; // ������ƥ������
}

/**
 * @brief �л���������
 *
 * @param type
 * @return CProtocol*
 */
CProtocol *CProtApp::SwitchPcProt(TPYE type)
{
    memset(&(m_exchange.prot_para), 0, sizeof(PCPROT_PARA));

    std::string pattern;
    switch (type)
    {
    case typeDZ:
        pattern = "^(?!.*װ�ò�����ֵ��).*��ֵ��.*$";
        break;
    case typeRYB:
        pattern = "��ѹ����";
        break;
    case typeYXQ:
        pattern = "��ֵ������";
        break;
    }

    // int targetAddr = -1;
    // switch (type)
    // {
    // case typeDZ:
    //     targetAddr = CNet103_SH::DZ;
    //     break;
    // case typeZZPARA:
    //     targetAddr = CNet103_SH::ZZPARA;
    //     break;
    // case typeRYB:
    //     targetAddr = CNet103_SH::RYB;
    //     break;
    // case typeYXQ:
    //     targetAddr = CNet103_SH::YXQ;
    //     break;
    // case typeSTATION:
    //     targetAddr = CNet103_SH::STATION;
    //     break;
    // }

    for (int i = 0; i < m_curProtNum; i++)
    {
        if (m_pCurProt + i == NULL)
            continue;
        // if (m_pCurProt[i].gui_yue == PROTTYPE_NET103_SH && m_pCurProt[i].addr2 == targetAddr)
        if (m_pCurProt[i].gui_yue == PROTTYPE_NET103_SH && containsPattern(m_pCurProt[i].desc, pattern))
        {
            memcpy(&(m_exchange.prot_para), &m_pCurProt[i], sizeof(PCPROT_PARA));
            DFLOG_DEBUG("type1=%d, targetAddr2=%d", m_exchange.prot_para.type1, m_exchange.prot_para.addr2);
            return SwitchGene(m_pCurProt[i].gui_yue);
        }
    }

    return NULL;
}

/**
 * @brief �л���Լ
 *
 * @param gene
 * @return CProtocol*
 */
CProtocol *CProtApp::SwitchGene(int gene)
{
    CProtocol *pProt = NULL;
    if ((pProt = SwitchProtocol(gene)) == NULL)
    {
        DFLOG_ERROR("The system does not hold out this protocol");
    }
    return pProt;
}

/**
 * @brief ˢ�±��������б�
 *
 * @param rtuno
 */
void CProtApp::refreshProtList(int rtuno)
{
    char acRtuCode[PROT_CODE_LENS] = {0};

    m_pCurProt = NULL;
    m_curProtNum = 0;

    const RTU_PARA *pRTU = m_baseDB.GetRtuPara();
    if (pRTU == NULL)
    {
        return;
    }

    for (int i = 0; i < m_baseDB.GetRtuNum(); i++)
    {
        if (pRTU[i].rtuno == rtuno)
        {
            strcpy(acRtuCode, pRTU[i].code);
            m_pCurProt = m_baseDB.GetPcProtPara(rtuno, &m_curProtNum);
            break;
        }
    }
    return;
}
