/**
 * @file protdb.cpp
 * @author ����
 * @brief ���ļ�Ϊ���ݿ����ʵ���ļ�
 * @version 0.1
 * @date 2023-04-21
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "protdb.h"

#include "dfdbdef.h"
#include "dflogger.h"
#include "langtr.h"
#include "protapp.h"
#include "sybase.h"
#include "tablemap_ch.h"
#include "dzjzsrv.h"

/**
 * @brief ���캯��
 *
 */
CProtDB::CProtDB()
{
    m_dbStatus = CS_FAIL;
    m_nStationNum = 0;
    m_pStationPara = NULL;
    m_nRtuNum = 0;
    m_pRtuPara = NULL;
    m_protNumArray = NULL;
    m_pcProtParaArray = NULL;
}

/**
 * @brief ��������
 *
 */
CProtDB::~CProtDB()
{
    FreeProtPara();
}

/**
 * @brief ��ʼ�����ݿ�
 *
 * @return int
 */
int CProtDB::StartDB()
{
    if ((startdb(1, 1)) != CS_SUCCEED)
    {
        closedb();
        m_dbStatus = DB_STARTUP;
    }
    else
        m_dbStatus = DB_STARTUP;

    return m_dbStatus;
}

/**
 * @brief �ͷŻ���
 *
 */
void CProtDB::FreeProtPara()
{
    int i = 0;
    for (i = 0; i < m_nRtuNum; i++)
    {
        if (m_pcProtParaArray[i] != NULL)
        {
            free(m_pcProtParaArray[i]);
            m_pcProtParaArray[i] = NULL;
        }
    }
    if (m_pcProtParaArray)
        delete[] m_pcProtParaArray;
    if (m_protNumArray)
        delete[] m_protNumArray;

    if (m_pRtuPara)
        free(m_pRtuPara);
    if (m_pStationPara)
        free(m_pStationPara);
    m_pRtuPara = NULL;
    m_nRtuNum = 0;
    m_pcProtParaArray = NULL;
    m_protNumArray = NULL;
    m_pStationPara = NULL;
    m_nStationNum = 0;
}

/**
 * @brief ���س�վ����
 *
 * @return int
 */
int CProtDB::LoadStationPara()
{
    if (m_dbStatus != DB_STARTUP)
        return DB_UNOPENED;
    char isqlcmd[200] = "";
    int ret = 0;

    FreeProtPara();

    strcpy(isqlcmd, _LANGTR("select ��վ��,���,���� from ��վ������ where ��� in(select distinct ��վ���� from ΢������������) order by ��� "));
    ret = Isql(isqlcmd, (void **)&m_pStationPara);

    if (ret > 0)
        m_nStationNum = ret;
    else
        m_nStationNum = 0;

    return ret;
}

/**
 * @brief ����RTU����
 *
 * @return int
 */
int CProtDB::LoadRtuPara()
{
    int ret = 0;
    char isqlcmd[1024] = "";
    int tmplen;

    if (m_pRtuPara)
        free(m_pRtuPara);
    m_pRtuPara = NULL;
    m_nRtuNum = 0;

    sprintf(isqlcmd, "select %s,%s,%s,%s from %s where %s in(select distinct %s from %s) order by %s ",
            QFD_NO, QFD_SUBSTATION2_FLAG, QFD_NAME, QFD_DESCRIPTION, RTU_PARAM_TAB, QFD_NO, QFD_RTU_NO, PROT_INFO_PARAM_TAB, QFD_NO);
    tmplen = strlen(isqlcmd);
    ret = Isql(isqlcmd, (void **)&m_pRtuPara);

    if (ret > 0)
        m_nRtuNum = ret;
    else
        m_nRtuNum = 0;
    //DFLOG_DEBUG("%s ȡ��%d����¼", RTU_PARAM_TAB, m_nRtuNum);
    if (m_nRtuNum > 0)
    {
        m_pcProtParaArray = new P_PCPROT_PARA[m_nRtuNum];
        if (m_pcProtParaArray == NULL)
            return 0;
        memset(m_pcProtParaArray, 0, m_nRtuNum * sizeof(P_PCPROT_PARA));
        m_protNumArray = new int[m_nRtuNum];
        if (m_protNumArray == NULL)
        {
            delete[] m_pcProtParaArray;
            m_pcProtParaArray = NULL;
            return 0;
        }
        memset(m_protNumArray, 0, sizeof(int) * m_nRtuNum);

        LoadPcProtPara();
    }
    return ret;
}

/**
 * @brief ִ��isql���
 *
 * @param sqlcmd
 * @param pBuff
 * @return int
 */
int CProtDB::Isql(char *sqlcmd, void **pBuff)
{
    FUNC_STR func;
    CS_RETCODE ret;
    //	CS_DATAFMT *datafmt = NULL;
    func.func = SELECT_ISQL_RESULTS;
    func.serverno = SERVER_DEFAULT;
    strcpy(func.isql, sqlcmd);
    ret = dbfselectisqlresults(&func, NULL, pBuff);
    if (ret != CS_SUCCEED)
        return DB_GET_FAILED;
    return func.ret_roxnum;
}

/**
 * @brief ����΢����������
 *
 * @return int
 */
int CProtDB::LoadPcProtPara()
{
    int para_num = 0, ret = 0;
    char isqlcmd[1024] = "";
    if (m_dbStatus != CS_SUCCEED)
        return 0;
    if (m_pcProtParaArray == NULL)
        return 0;
    if (m_pRtuPara == NULL)
        return 0;
    P_PCPROT_PARA pcProtParaArray = NULL;

    // ��������Ƚ���������
    sprintf(isqlcmd, "select ����,����,��ȫ��,��վ����,RTU��,��Լ����,����1,����2,����3,����4,��ַ1,��ַ2,��ַ3,��ַ4,�Ƿ�ͳ��,�Ƿ���������,������� from ΢������������  order by RTU��");
    ret = Isql(isqlcmd, (void **)&pcProtParaArray);

    if (ret < 0)
    {
        //	m_protNumArray[i] = 0;
        return ret;
    }

    int i = 0, j = 0;
    int pointNum = 0;
    for (i = 0; i < m_nRtuNum; i++) // ��ѯm_pRtuPara[i].rtuno
    {
        pointNum = 0;
        for (j = 0; j < ret; j++) // ��ѯpcProtParaArray
        {
            if (m_pRtuPara[i].rtuno == pcProtParaArray[j].rtu_no)
            {
                m_pcProtParaArray[i] = (P_PCPROT_PARA)realloc((void *)m_pcProtParaArray[i], (pointNum + 1) * sizeof(PCPROT_PARA));
                memcpy(&m_pcProtParaArray[i][pointNum], &pcProtParaArray[j], sizeof(PCPROT_PARA));
                //DFLOG_DEBUG("NAME[%s] DESCRIPTION[%s] RTU_NO[%d] OF_SUBSTATION[%s]", m_pcProtParaArray[i][pointNum].code, m_pcProtParaArray[i][pointNum].desc, m_pcProtParaArray[i][pointNum].rtu_no, m_pcProtParaArray[i][pointNum].station);
                pointNum++;
                m_protNumArray[i] = pointNum;
            }
        }
    }

    if (pcProtParaArray)
    {
        free(pcProtParaArray);
    }
    pcProtParaArray = NULL;
    return ret;
}

/**
 * @brief ��ȡ΢����������
 *
 * @param rtu_no
 * @param pcprot_num
 * @return const PCPROT_PARA*
 */
const PCPROT_PARA *CProtDB::GetPcProtPara(short rtu_no, int *pcprot_num)
{
    if (m_nRtuNum <= 0 || pcprot_num == NULL)
        return 0;

    for (int i = 0; i < m_nRtuNum; i++)
    {
        if (rtu_no == m_pRtuPara[i].rtuno)
        {
            *pcprot_num = m_protNumArray[i];
            return m_pcProtParaArray[i];
        }
    }
    *pcprot_num = 0;
    return NULL;
}

/**
 * @brief ɾ����¼
 *
 * @return true
 * @return false
 */
bool CProtDB::DeleteDZRecords(short nRtuNo, short addr, short groupno)
{
    bool bRet = false;
    FUNC_STR func;
    CS_DATAFMT *datafmt = NULL;

    memset(&func, 0, sizeof(FUNC_STR));
    func.serverno = SERVER_DEFAULT;
    func.func = ISQL_COMMAND;
    sprintf(func.table, "xopensdb.dbo.���ܼ���װ�ö�ֵ��Ϣ��");
    sprintf(func.isql, "delete from ���ܼ���װ�ö�ֵ��Ϣ�� where RTU��=%d and װ������=%d and ���=%d", nRtuNo, addr, groupno);

    if (dbfisqlcommand(&func) == CS_SUCCEED)
    {
        bRet = true;
    }

    return bRet;
}

/**
 * @brief �����¼
 *
 * @param recordList
 */
void CProtDB::SaveDZRecords(std::vector<DZJZ_FIXVALUE> &recordList)
{
    saveTable(recordList, "xopensdb.dbo.���ܼ���װ�ö�ֵ��Ϣ��");
}
