/**
 * @file fixvaluesrv.cpp
 * @author ����
 * @brief ���ļ�Ϊ��ֵ�ٻ��������ʵ���ļ�
 * @version 0.1
 * @date 2023-04-21
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "fixvaluesrv.h"

#include <signal.h>

#include <QDateTime>

#include "dzjzinspect.h"
#include "dzjzsrv.h"
#include "eventdef.h"

/**
 * @brief ���캯��
 *
 */
CFixValueService::CFixValueService()
{
    m_pApp = new CProtApp;
    m_poCallProtocol = NULL;
    m_nRtuNo = 0;

#ifdef __unix
    sigset(SIGUSR2, SIG_IGN);
#endif
}

/**
 * @brief ��������
 *
 */
CFixValueService::~CFixValueService()
{
    if (m_pApp != NULL)
    {
        delete m_pApp;
        m_pApp = NULL;
    }
}

/**
 * @brief �����ٻ�
 *
 * @return int
 */
int CFixValueService::executeBatch()
{
    std::vector<DZJZ_DEVICE> listDevice = GetDZJZDeviceList();

    int nRet = FIX_CALL_SUCCESS;

    DFLOG_INFO("[��ֵ�����ٻ�]: ------------start---------------");

    for (int i = 0; i < listDevice.size(); i++)
    {
        executeSingle(listDevice[i].rtuno);
    }

    reloadRTDB();

    DFLOG_INFO("[��ֵ�����ٻ�]: ----------------end-----------------");

    return nRet;
}

/**
 * @brief �������ٻ�
 *
 * @param strPara
 * @param progressCallback
 * @return std::string
 */
std::string CFixValueService::executeInspectionByCond(const std::string &strPara, ProgressCallback progressCallback)
{
    DFLOG_INFO("[װ��Ѳ��]: ------------start---------------");

    dfJson::Value jProgress;

    // ��������
    dfJson::Value jPara;
    dfJson::CharReaderBuilder readerBuilder;
    std::istringstream iss(strPara);
    dfJson::parseFromStream(readerBuilder, iss, &jPara, nullptr);

    QString station = jPara["station"].asString().c_str();
    int areaId = jPara["area"].asInt();
    int roundType = jPara["round"].asInt();

    // ��ȡRTU
    std::vector<DZJZ_DEVICE> listDevice = GetDZJZDeviceList(station, areaId, roundType);
    DFLOG_INFO("[װ��Ѳ��]:��ȡ��%d���ٻ��豸", listDevice.size());

    // �����ٻ���ֵ
    for (int i = 0; i < listDevice.size(); i++)
    {
        jProgress["optype"] = "callDZ";
        jProgress["progress"] = (i + 1) * 60 / listDevice.size();
        jProgress["desc"] = QString("�ٻ�װ��[%1]%2��ֵ").arg(listDevice[i].rtuno).arg(listDevice[i].desc).toAscii().data();
        progressCallback(jsonToString(jProgress));

        executeSingle(listDevice[i].rtuno);
        jProgress["optype"] = "callDZ";
        jProgress["progress"] = (i + 1) * 60 / listDevice.size();
        jProgress["desc"] = QString("�ٻ�װ��[%1]%2��ֵ �ɹ�").arg(listDevice[i].rtuno).arg(listDevice[i].desc).toAscii().data();
        progressCallback(jsonToString(jProgress));
    }

    if (listDevice.size() > 0)
    {
        jProgress["optype"] = "callDZ";
        jProgress["progress"] = 70;
        jProgress["desc"] = "����ʵʱ��...";
        progressCallback(jsonToString(jProgress));
        reloadRTDB();
        usleep(5000 * 1000);
    }

    // ִ��Ѳ��
    jProgress["optype"] = "callDZ";
    jProgress["progress"] = 80;
    jProgress["desc"] = "Ѳ��װ��...";
    progressCallback(jsonToString(jProgress));

    DZJZ_Inspect inspect;
    dfJson::Value jRes;
    inspect.doInspect(station.toAscii().data(), areaId, roundType, jRes);

    jProgress["optype"] = "callDZ";
    jProgress["progress"] = 100;
    jProgress["desc"] = "Ѳ�����";
    progressCallback(jsonToString(jProgress));

    progressCallback(jsonToString(jRes));

    DFLOG_INFO("[װ��Ѳ��]: ----------------end-----------------");

    // ���ؽ��
    return jsonToString(jRes);
}

/**
 * @brief �ȴ��ظ��ʼ�
 *
 * @param timeout
 * @return int
 */
int CFixValueService::waitForReply(int timeout)
{
    time_t startTime = time(NULL);
    time_t currentTime;

    while (true)
    {
        m_poCallProtocol->OnTimer(1);

        if (m_poCallProtocol->isRecvOver())
        {
            return FIX_CALL_SUCCESS;
        }

        currentTime = time(NULL);

        if ((currentTime - startTime) >= timeout)
        {
            DFLOG_INFO("[��ֵ�ٻ�]���ȴ���ʱ");
            return FIX_CALL_OVERTIME;
        }

        usleep(200000);
    }

    return FIX_CALL_OVERTIME;
}

/**
 * @brief ����ִ��
 *
 * @param nRtuNo
 * @return int
 */
int CFixValueService::executeSingle(int nRtuNo)
{
    int nRet = FIX_CALL_SUCCESS;
    QString logMsg;

    DFLOG_INFO("[��ֵ�ٻ�]: RTU[%d]", nRtuNo);

    m_nRtuNo = nRtuNo;

    // ˢ��RTU��������
    m_pApp->refreshProtList(nRtuNo);

    // �ٻ���ֵ��
    nRet = CallAreaNo();

    if (nRet != FIX_CALL_SUCCESS)
    {
        logMsg = PrintFixCallErr(nRet);
        CreateEvent(EVENT_SCADA, ENTTYPE_DZJZ, 1, logMsg, QString::number(m_nRtuNo));
        DFLOG_INFO("%s", logMsg.toAscii().constData());

        return nRet;
    }

    usleep(500000);

    // �ٻ���ֵ
    nRet = CallDZ(m_pApp->m_nRunAreaNo);
    if (nRet != FIX_CALL_SUCCESS)
    {
        logMsg = PrintFixCallErr(nRet);
        CreateEvent(EVENT_SCADA, ENTTYPE_DZJZ, 1, logMsg, QString::number(m_nRtuNo));
        DFLOG_INFO("%s", logMsg.toAscii().constData());
        return nRet;
    }
    usleep(500000);

    // �ٻ���ѹ��
    nRet = CallRYB();

    logMsg = PrintFixCallErr(nRet);
    CreateEvent(EVENT_SCADA, ENTTYPE_DZJZ, 1, logMsg, QString::number(m_nRtuNo));
    DFLOG_INFO("%s", logMsg.toAscii().constData());

    usleep(500000);

    return nRet;
}

/**
 * @brief ����nRet��ֵ������Ӧ����־��Ϣ
 *
 * @param nRet
 * @return std::string
 */
QString CFixValueService::PrintFixCallErr(int nRet)
{
    QString logMessage;

    switch (nRet)
    {
    case FIX_CALL_SUCCESS:
        logMessage = QString("[��ֵ�ٻ� RTU:%1]:�ɹ�").arg(m_nRtuNo);
        break;
    case FIX_CALL_ERROR:
        logMessage = QString("[��ֵ�ٻ� RTU:%1]:ʧ��").arg(m_nRtuNo);
        break;
    case FIX_CALL_OVERTIME:
        logMessage = QString("[��ֵ�ٻ� RTU:%1]:��ʱ").arg(m_nRtuNo);
        break;
    case FIX_CALL_SEND_ERROR:
        logMessage = QString("[��ֵ�ٻ� RTU:%1]:�ʼ�����ʧ��").arg(m_nRtuNo);
        break;
    default:
        logMessage = QString("[��ֵ�ٻ� RTU:%1]:δ֪����").arg(m_nRtuNo);
        break;
    }

    return logMessage;
}

/**
 * @brief �ٻ���ֵ��
 *
 * @return int
 */
int CFixValueService::CallAreaNo()
{
    DFLOG_INFO("[��ֵ�ٻ�]:�ٻ���ֵ����");

    int nRet = FIX_CALL_SUCCESS;

    m_poCallProtocol = m_pApp->SwitchPcProt(typeYXQ);

    if (m_poCallProtocol == NULL)
    {
        DFLOG_ERROR("[��ֵ�ٻ�]: δ�ҵ���Լ����");
        return FIX_CALL_ERROR;
    }

    m_poCallProtocol->SetSrvData(&m_pApp->m_exchange, &m_pApp->m_mailBox, m_pApp->m_pFixedValue);
    m_pApp->m_nRunAreaNo = 0;

    // �����ٻ���ֵ���ʼ�
    nRet = SendCallAreaNoMail();
    if (nRet < 0)
    {
        return nRet;
    }

    // �ȴ��ʼ��ظ�
    nRet = waitForReply(FIX_CALL_WAIT_TIME);

    if (nRet == FIX_CALL_SUCCESS)
    {
        m_pApp->m_nRunAreaNo = m_poCallProtocol->GetCurAreaNo();
    }

    return nRet;
}

/**
 * @brief �ٻ���ֵ
 *
 * @param iAreaNo
 * @return int
 */
int CFixValueService::CallDZ(int iAreaNo)
{
    DFLOG_INFO("[��ֵ�ٻ�]:�ٻ���ֵ %d", iAreaNo);

    int nRet = FIX_CALL_SUCCESS;

    m_poCallProtocol = m_pApp->SwitchPcProt(typeDZ);

    if (m_poCallProtocol == NULL)
    {
        DFLOG_ERROR("[��ֵ�ٻ�]: δ�ҵ���Լ����");
        return FIX_CALL_ERROR;
    }

    m_poCallProtocol->SetSrvData(&m_pApp->m_exchange, &m_pApp->m_mailBox, m_pApp->m_pFixedValue);

    // �����ٻ���ֵ���ʼ�
    nRet = SendCallSetValueMail(iAreaNo);
    if (nRet < 0)
    {
        return nRet;
    }

    // �ȴ��ʼ��ظ�
    nRet = waitForReply(FIX_CALL_WAIT_TIME);

    if (nRet == FIX_CALL_SUCCESS)
    {
        m_poCallProtocol->SaveValueToDB();
    }

    return nRet;
}

/**
 * @brief �ٻ���ѹ��
 *
 * @return int
 */
int CFixValueService::CallRYB()
{
    DFLOG_INFO("[��ֵ�ٻ�]:�ٻ���ѹ��");

    int nRet = FIX_CALL_SUCCESS;
    m_pApp->m_nRunAreaNo = 0;

    m_poCallProtocol = m_pApp->SwitchPcProt(typeRYB);

    if (m_poCallProtocol == NULL)
    {
        DFLOG_ERROR("[��ֵ�ٻ�]: δ�ҵ���Լ����");
        return FIX_CALL_ERROR;
    }

    m_poCallProtocol->SetSrvData(&m_pApp->m_exchange, &m_pApp->m_mailBox, m_pApp->m_pFixedValue);

    // �����ٻ���ֵ���ʼ�
    nRet = SendCallRYBMail();
    if (nRet < 0)
    {
        return nRet;
    }

    // �ȴ��ʼ��ظ�
    nRet = waitForReply(FIX_CALL_WAIT_TIME);

    if (nRet == FIX_CALL_SUCCESS)
    {
        m_poCallProtocol->SaveValueToDB();
    }

    return nRet;
}

/**
 * @brief �ٻ�װ�ö�ֵ����
 *
 * @param szDevName װ�ô���
 * @param iBoardNo ���
 * @param iAreaNo ����
 * @return true
 * @return false
 */
int CFixValueService::SendCallAreaNoMail()
{
    int iRet = m_poCallProtocol->SendCallAreaNoMail();
    if (iRet < 0)
    {
        DFLOG_ERROR("[��ֵ�����ٻ�]: �ʼ�����ʧ��");
        return FIX_CALL_ERROR;
    }
    DFLOG_INFO("[��ֵ�����ٻ�]: �ʼ����ͳɹ�");
    return FIX_CALL_SUCCESS;
}

/**
 * @brief �ٻ���ֵ
 *
 * @param iAreaNo
 * @return int
 */
int CFixValueService::SendCallSetValueMail(int iAreaNo)
{
    int iRet = m_poCallProtocol->SendCallValueMail(iAreaNo);
    if (iRet < 0)
    {
        DFLOG_ERROR("[��ֵ�ٻ�]: �ʼ�����ʧ��");
        return FIX_CALL_ERROR;
    }

    DFLOG_INFO("[��ֵ�ٻ�]: �ʼ����ͳɹ�");
    return FIX_CALL_SUCCESS;
}

/**
 * @brief �ٻ���ѹ��
 *
 * @return int
 */
int CFixValueService::SendCallRYBMail()
{
    int iRet = m_poCallProtocol->SendCallRYBMail();
    if (iRet < 0)
    {
        DFLOG_ERROR("[��ѹ���ٻ�]: �ʼ�����ʧ��");
        return FIX_CALL_ERROR;
    }
    DFLOG_INFO("[��ѹ���ٻ�]: �ʼ����ͳɹ�");
    return FIX_CALL_SUCCESS;
}

/**
 * @fn		void CFixValueThread::InitDayCallList()
 * @brief	��ʼ����ִ���б�
 *
 * @param	��
 * @return	��
 * @see
 *
 */
std::vector<DZJZ_DEVICE> CFixValueService::GetDZJZDeviceList()
{
    DZJZ_DEVICE *pBuf = NULL;
    std::vector<DZJZ_DEVICE> deviceList;

    // �����ݿ��л�ȡ���еĶ�ֵ�ٻ��豸
    int nRet = m_pApp->m_baseDB.Isql("select ���,���� from xopensdb.dbo.���ܼ���װ�ò�����", (void **)&pBuf);

    if (nRet > 0)
    {
        deviceList.reserve(nRet); // Ԥ�ȷ����ڴ�
        deviceList.assign(pBuf, pBuf + nRet);

        for (int i = 0; i < deviceList.size(); i++)
        {
            DFLOG_DEBUG("%d|%s", deviceList[i].rtuno, deviceList[i].desc);
        }
    }

    DFLOG_INFO("[��ֵ�ٻ�]:��ȡ��%d����ֵ�ٻ��豸", deviceList.size());

    return deviceList;
}

/**
 * @brief ��ȡ��ֵ�ٻ��豸�б�
 *
 * @param subStation ������վ
 * @param area ����
 * @param roundType ������
 * @return std::vector<DZJZ_DEVICE> ��ֵ�ٻ��豸�б�
 */
std::vector<DZJZ_DEVICE> CFixValueService::GetDZJZDeviceList(const QString &subStation, int area, int roundType)
{
    DFLOG_INFO("[��ֵ�ٻ�]: subStation=%s area=%d roundType=%d", subStation.toLocal8Bit().data(), area, roundType);

    DZJZ_DEVICE *pBuf = NULL;
    std::vector<DZJZ_DEVICE> deviceList;

    QString sWhere = " where 1=1";
    if (!subStation.isEmpty())
        sWhere += " and ������վ='" + subStation + "'";

    if (area > 0)
        sWhere += " and ��������=" + QString::number(area);

    if (roundType > 0)
    {
        if (roundType == 1) // ������
            sWhere += " and ������ in (1,2,3,4,5)";
        else if (roundType == 2)
            sWhere += " and ������ in (6)";
        else if (roundType == 3)
            sWhere += " and ������ in(7,8)";
    }

    QString qsCond = QString("select distinct ����װ��,װ������ from xopensdb.dbo.���ܼ����ִ�����ͼ %1").arg(sWhere);
    DFLOG_INFO("[��ֵ�ٻ�]: sql=%s", qsCond.toAscii().data());

    // �����ݿ��л�ȡ���еĶ�ֵ�ٻ��豸
    int nRet = m_pApp->m_baseDB.Isql(qsCond.toAscii().data(), (void **)&pBuf);

    if (nRet > 0)
    {
        deviceList.reserve(nRet); // Ԥ�ȷ����ڴ�
        deviceList.assign(pBuf, pBuf + nRet);

        for (int i = 0; i < deviceList.size(); i++)
        {
            DFLOG_DEBUG("%d|%s", deviceList[i].rtuno, deviceList[i].desc);
        }
    }

    return deviceList;
}

/**
 * @brief ����ʵʱ��
 *
 */
void CFixValueService::reloadRTDB()
{
    rtdb_proc rtdbproc;
    if (!rtdbproc.recall("ems", "", "", "���ܼ���װ�ö�ֵ��Ϣ��"))
    {
        DFLOG_ERROR("[��ֵ�ٻ�]:����ʵʱ��ʧ��");
    }
    DFLOG_INFO("[��ֵ�ٻ�]:����ʵʱ��ɹ�");
}

/**
 * @brief ��������
 *
 */
void CFixValueService::CreateEvent(int category, int type, int state_value, QString char_info, QString member_name0 /*= QString::null*/, QString member_name1 /*= QString::null*/, QString group_name /*= QString::null*/)
{
    quint16 cat_no = category;
    quint16 type_no = type;

    SYS_EVENT sysevent;
    event_proc netevent;
    memset(&sysevent, 0, sizeof(SYS_EVENT));
    QDateTime dataTime = QDateTime::currentDateTime();
    QDate date = dataTime.date();
    QTime time = dataTime.time();

    sysevent.event_sort = cat_no;
    sysevent.event_type = type_no;
    sysevent.security = 0;
    sysevent.PrintOut = 0;
    sysevent.ToneOut = 0;
    sysevent.DiskOut = sysevent.DispOut = TRUE;
    sysevent.state_value = state_value;
    sysevent.ymd = date.year() * 10000 + date.month() * 100 + date.day();
    sysevent.hmsms = time.hour() * 10000000 + time.minute() * 100000 + time.second() * 1000 + time.msec();

    if (!member_name0.isNull())
        strncpy(sysevent.member_name0, (const char *)member_name0.toAscii(), SHORTNAME_LEN - 1);
    if (!member_name1.isNull())
        strncpy(sysevent.member_name1, (const char *)member_name1.toAscii(), SHORTNAME_LEN - 1);
    if (!group_name.isNull())
        strncpy(sysevent.group_name, (const char *)group_name.toAscii(), SHORTNAME_LEN - 1);
    if (!char_info.isNull())
        strncpy(sysevent.char_info, (const char *)char_info.toAscii(), FULLDESCRIBE_LEN - 1);

    netevent.save_event(&sysevent, 1);
}