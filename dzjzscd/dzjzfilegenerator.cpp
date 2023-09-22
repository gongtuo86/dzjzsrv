#include "dzjzfilegenerator.h"

void DZJZ_Section::Write(QTextStream &out) const
{
    out << "<" << m_sectionName << ">\n";
    out << "@ ";
    for (const QString &header : m_headerList)
    {
        out << header << " ";
    }
    out << "\n";
    out << "//";
    for (const QString &desc : m_descriptionList)
    {
        out << desc << " ";
    }
    out << "\n";

    for (int i = 0; i < m_entries.size(); ++i)
    {
        out << "# ";
        for (const QString &header : m_headerList)
        {
            out << QString::fromStdString(m_entries[i][header.toStdString()].asString()) << "  ";
        }
        out << "\n";
    }
    out << "</" << m_sectionName << ">\n";
}

const QMap<int, QString> DZJZ_FileGenerator::m_funcTypeMap = getDMConfig("���ܼ����ִι�������");

const QMap<QString, QString> DZJZ_FileGenerator::m_subMap = getSubMap();

const QMap<int, QString> DZJZ_FileGenerator::m_loadTypeMap = getDMConfig("���ܼ��ظ�������");

const QMap<int, QString> DZJZ_FileGenerator::m_strapTypeMap = getDMConfig("���ܼ���Ͷ�˼ƻ�����");

const QMap<int, QString> DZJZ_FileGenerator::m_deviceTypeMap = getDMConfig("���ܼ���װ������");

DZJZ_FileGenerator::DZJZ_FileGenerator(const QString &fileName, const QString &type, const QString &datetime)
    : m_fileName(fileName), m_type(type), m_datetime(datetime)
{
}

bool DZJZ_FileGenerator::Generate(const QList<DZJZ_Section> &sections)
{
    QFile file(m_fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);

    // Write file header
    out << "<! system=" << m_system << " version=" << m_version << " type=" << m_type
        << " code=" << m_code << " data=" << m_data << " datetime='" << m_datetime << "' !>\n";

    for (const DZJZ_Section &section : sections)
    {
        section.Write(out);
    }

    file.close();
    return true;
}

/**
 * @brief Get the Round Base Type object
 *
 * @param roundType
 * @return QString
 */
QString DZJZ_FileGenerator::getRoundBaseType(int roundType)
{
    if (roundType >= 1 && roundType <= 6)
        return QString::fromLocal8Bit("������");
    else if (roundType >= 7 && roundType <= 8)
        return QString::fromLocal8Bit("������");
    else
        return QString::fromLocal8Bit("������");
}

/**
 * @brief Get the Round Base Type Num object
 *
 * @param roundType
 * @return int
 */
int DZJZ_FileGenerator::getRoundBaseTypeNum(int roundType)
{
    if (roundType <= 6)
        return roundType;
    else if (roundType == 7 && roundType <= 8)
        return 1;
    else if (roundType == 8)
        return 2;
    else
        return roundType - 6;
}

bool DZJZ_FileGenerator::DZJZ_GenRoundsFile(const QDateTime &qDateTime)
{
    QString fileName = QString::fromLocal8Bit("changxing_ufvls_rounds_%1.CIME")
            .arg(qDateTime.toString("yyyyMMddhhmmss"));

    QString strDateTime = qDateTime.toString("yyyyMMddhhmmss");

    QStringList headerList = {"ufvls_type",
                              "rounds_type",
                              "rounds_num",
                              "plan_load_rate",
                              "plan_load_val",
                              "cur_load_rate",
                              "cur_load_val",
                              "ufvls_status"};

    QStringList descsList = {QString::fromLocal8Bit("��������"),
                             QString::fromLocal8Bit("�ִ�����"),
                             QString::fromLocal8Bit("�ִ����"),
                             QString::fromLocal8Bit("�ִμƻ��кɱ���"),
                             QString::fromLocal8Bit("�ִμƻ��к���"),
                             QString::fromLocal8Bit("�ִ�ʵ���кɱ���"),
                             QString::fromLocal8Bit("�ִ�ʵ���к���"),
                             QString::fromLocal8Bit("�ִμ���״̬")};

    dfJson::Value items(dfJson::arrayValue);

    RdbSQL rsql;
    rsql.selectfrom("dzjzround");
    rsql.selectattr("functype");
    rsql.selectattr("roundtype");    // �ڼ���
    rsql.selectattr("planpwrrate");  // �ƻ��кɱ���
    rsql.selectattr("issuepower");   // �ƻ��к���
    rsql.selectattr("realpwrrate");  // ʵ���кɱ���
    rsql.selectattr("judgepower");   // ʵ���к���
    rsql.selectattr("judgerequire"); // ����״̬
    if (rsql.select(RTDB_EMS_USER, "") == RDB_OK)
    {
        int nCnt = rsql.getrcdcnt();
        for (int i = 0; i < nCnt; i++)
        {
            dfJson::Value item;
            item["ufvls_type"] = m_funcTypeMap[rsql.get_intval(i, "functype")].toLocal8Bit().data();

            int roundType = rsql.get_intval(i, "roundtype");
            item["rounds_type"] = getRoundBaseType(roundType).toLocal8Bit().data();
            item["rounds_num"] = getRoundBaseTypeNum(roundType); // �ִ����
            item["plan_load_rate"] = roundToThreeDecimalPlaces(rsql.get_floatval(i, "planpwrrate"));
            item["plan_load_val"] = roundToThreeDecimalPlaces(rsql.get_floatval(i, "issuepower"));
            item["cur_load_rate"] = roundToThreeDecimalPlaces(rsql.get_floatval(i, "realpwrrate"));
            item["cur_load_val"] = roundToThreeDecimalPlaces(rsql.get_floatval(i, "judgepower"));
            item["ufvls_status"] = rsql.get_intval(i, "judgerequire");
            items.append(item);
        }
    }
    DZJZ_Section section(QString::fromLocal8Bit("���ִμ��ؽ��::��������"), headerList, descsList, items);

    DZJZ_FileGenerator fileGen(fileName, "ufvls_rounds", strDateTime);
    QList<DZJZ_Section> sections = {section};
    fileGen.Generate(sections);
}

/**
 * @brief �豸���������ļ�
 *
 * @param qDateTime
 * @return true
 * @return false
 */
bool DZJZ_FileGenerator::DZJZ_GenDevicesFile(const QDateTime &qDateTime)
{
    // �����ļ���
    QString fileName = QString::fromLocal8Bit("changxing_ufvls_dev_%1.CIME")
            .arg(qDateTime.toString("yyyyMMddhhmmss"));
    QString strDateTime = qDateTime.toString("yyyyMMddhhmmss");

    // �����豸���ֵ�ͷ��������
    QStringList headerList = {"stname", "name", "dev_type", "alarm_status",
                              "sf_ret", "sf_reason", "di_ret", "di_reason", "sg_ret", "sg_reason"};

    QStringList descsList = {QString::fromLocal8Bit("���վ����"),
                             QString::fromLocal8Bit("�豸����"),
                             QString::fromLocal8Bit("װ������"),
                             QString::fromLocal8Bit("�澯״̬"),
                             QString::fromLocal8Bit("��ѹ�����н���"),
                             QString::fromLocal8Bit("��ѹ������ʧ��ԭ��"),
                             QString::fromLocal8Bit("Ӳѹ�����н���"),
                             QString::fromLocal8Bit("Ӳѹ������ʧ��ԭ��"),
                             QString::fromLocal8Bit("��ֵ���н���"),
                             QString::fromLocal8Bit("��ֵ����ʧ��ԭ��")};

    dfJson::Value items(dfJson::arrayValue);

    RdbSQL rsql;
    rsql.selectfrom("dzjzdev");
    rsql.selectattr("id");
    rsql.selectattr("name");
    rsql.selectattr("ofsta");
    rsql.selectattr("type");
    rsql.selectattr("alarm");
    rsql.selectattr("sfret");
    rsql.selectattr("sfreason");
    rsql.selectattr("diret");
    rsql.selectattr("direason");
    rsql.selectattr("sgret");
    rsql.selectattr("sgreason");

    if (rsql.select(RTDB_EMS_USER, "") == RDB_OK)
    {
        int nCnt = rsql.getrcdcnt();
        for (int i = 0; i < nCnt; i++)
        {
            dfJson::Value item;
            item["stname"] = m_subMap[rsql.get_string(i, "ofsta")].toLocal8Bit().data();
            item["name"] = rsql.get_string(i, "name");
            item["dev_type"] = rsql.get_intval(i, "type");
            item["alarm_status"] = rsql.get_intval(i, "alarm");
            item["sf_ret"] = rsql.get_intval(i, "sfret");
            item["sf_reason"] = rsql.get_intval(i, "sfreason");
            item["di_ret"] = rsql.get_intval(i, "diret");
            item["di_reason"] = rsql.get_intval(i, "direason");
            item["sg_ret"] = rsql.get_intval(i, "sgret");
            item["sg_reason"] = rsql.get_intval(i, "sgreason");
            items.append(item);
        }
    }

    DZJZ_Section section(QString::fromLocal8Bit("�豸::��������"), headerList, descsList, items);

    DZJZ_FileGenerator fileGen(fileName, "ufvls_dev", strDateTime);
    QList<DZJZ_Section> sections = {section};
    fileGen.Generate(sections);
    return true;
}

/**
 * @brief ��·���������ļ�
 *
 * @param qDateTime
 * @return true
 * @return false
 */
bool DZJZ_FileGenerator::DZJZ_GenLineFile(const QDateTime &qDateTime)
{
    QString fileName = QString::fromLocal8Bit("changxing_ufvls_line_%1.CIME")
            .arg(qDateTime.toString("yyyyMMddhhmmss"));

    QString strDateTime = qDateTime.toString("yyyyMMddhhmmss");

    QStringList headerList = {"stname", "name", "ufvls_type", "rounds_type",
                              "rounds_num", "load_val", "load_type", "cur_status",
                              "plan_status", "dev_name"};

    QStringList descsList = {QString::fromLocal8Bit("���վ����"),
                             QString::fromLocal8Bit("��������"),
                             QString::fromLocal8Bit("��������"),
                             QString::fromLocal8Bit("�ִ�����"),
                             QString::fromLocal8Bit("�ִ����"),
                             QString::fromLocal8Bit("������"),
                             QString::fromLocal8Bit("��������"),
                             QString::fromLocal8Bit("ʵ��Ͷ��״̬"),
                             QString::fromLocal8Bit("�ƻ�Ͷ��״̬"),
                             QString::fromLocal8Bit("����װ����")};

    dfJson::Value items(dfJson::arrayValue);

    RdbSQL rsql;
    rsql.selectfrom("dzjzronditem");
    rsql.selectattr("staid");
    rsql.selectattr("name");
    rsql.selectattr("functype");
    rsql.selectattr("roundtype");
    rsql.selectattr("pvalue");
    rsql.selectattr("loadtype");
    rsql.selectattr("strapreal");
    rsql.selectattr("strapplan");
    rsql.selectattr("devicename");

    if (rsql.select(RTDB_EMS_USER, "") == RDB_OK)
    {
        int nCnt = rsql.getrcdcnt();
        for (int i = 0; i < nCnt; i++)
        {
            dfJson::Value item;
            item["stname"] = rsql.get_string(i, "staid");
            item["name"] = rsql.get_string(i, "name");
            item["ufvls_type"] = m_funcTypeMap[rsql.get_intval(i, "functype")].toLocal8Bit().data();
            int roundType = rsql.get_intval(i, "roundtype");
            item["rounds_type"] = getRoundBaseType(roundType).toLocal8Bit().data();
            item["rounds_num"] = getRoundBaseTypeNum(roundType);
            item["load_val"] = roundToThreeDecimalPlaces(rsql.get_floatval(i, "pvalue"));
            item["load_type"] = m_loadTypeMap[rsql.get_intval(i, "loadtype")].toLocal8Bit().data();
            item["cur_status"] = m_strapTypeMap[rsql.get_intval(i, "strapreal")].toLocal8Bit().data();
            item["plan_status"] = m_strapTypeMap[rsql.get_intval(i, "strapplan")].toLocal8Bit().data();
            item["dev_name"] = rsql.get_intval(i, "devicename");
            items.append(item);
        }
    }

    DZJZ_Section section(QString::fromLocal8Bit("��·::��������"), headerList, descsList, items);

    DZJZ_FileGenerator fileGen(fileName, "ufvls_line", strDateTime);
    QList<DZJZ_Section> sections = {section};
    fileGen.Generate(sections);
    return true; // ����true��ʾ�ɹ�
}

/**
 * @brief ���ܼ��ض������ļ�
 *
 * @param qDateTime
 * @param nCycle
 * @return true
 * @return false
 */
bool DZJZ_FileGenerator::DZJZ_GenActFile(const QDateTime &qDateTime, int nCycle)
{
    QString fileName = QString::fromLocal8Bit("changxing_ufvls_act_%1.CIME")
            .arg(qDateTime.toString("yyyyMMddhhmmss"));

    QString strDateTime = qDateTime.toString("yyyyMMddhhmmss");

    QStringList headerList = {"event_time", "event_time_ms", "record_time", "record_time_ms",
                              "stname", "dev_name", "dev_type", "action_type",
                              "action_desc", "rounds_type", "rounds_num", "load_line", "load_val"};

    QStringList descsList = {QString::fromLocal8Bit("����ʱ��"),
                             QString::fromLocal8Bit("����ʱ�����"),
                             QString::fromLocal8Bit("��¼ʱ��"),
                             QString::fromLocal8Bit("��¼ʱ�����"),
                             QString::fromLocal8Bit("��վ��"),
                             QString::fromLocal8Bit("װ����"),
                             QString::fromLocal8Bit("װ������"),
                             QString::fromLocal8Bit("��������"),
                             QString::fromLocal8Bit("��������"),
                             QString::fromLocal8Bit("�ִ�����"),
                             QString::fromLocal8Bit("�����ִ�"),
                             QString::fromLocal8Bit("������·"),
                             QString::fromLocal8Bit("������")};

    dfJson::Value items(dfJson::arrayValue);

    FUNC_STR func;
    TDZJZ_ACTION_ROUND_ITEM *pBuf = nullptr;
    memset(&func, 0, sizeof(FUNC_STR));
    func.func = SELECT_ISQL_RESULTS;
    func.serverno = SERVER_DEFAULT;
    quint32 endTimeStamp = qDateTime.toTime_t();
    quint32 startTimeStamp = QDateTime(qDateTime.addSecs(-nCycle)).toTime_t();
    QString sSql = QString(
                "select * from xopenshdb.dbo.���ܼ����ִ������ where ʱ��>=%1 and ʱ��<=%2")
            .arg(startTimeStamp)
            .arg(endTimeStamp);
    snprintf(func.isql, sizeof(func.isql) - 1, "%s", sSql.toLocal8Bit().data());
    dbfselectisqlresults(&func, nullptr, (void **)&pBuf);
    for (int i = 0; i < func.ret_roxnum; i++)
    {
        if ((pBuf + i) != NULL)
        {
            dfJson::Value item;
            item["event_time"] = (pBuf + i)->time;
            item["event_time_ms"] = 0;
            item["record_time"] = (pBuf + i)->time;
            item["record_time_ms"] = 0;
            item["stname"] = m_subMap[(pBuf + i)->ofSta].toLocal8Bit().data();
            item["dev_name"] = (pBuf + i)->deviceName;
            item["dev_type"] = m_deviceTypeMap[(pBuf + i)->deviceType].toLocal8Bit().data();
            item["action_type"] = QString(m_funcTypeMap[(pBuf + i)->roundFuncType] + QString::fromLocal8Bit("����")).toLocal8Bit().data();
            item["action_desc"] = item["action_type"];
            item["rounds_type"] = getRoundBaseType((pBuf + i)->roundType).toLocal8Bit().data();
            item["rounds_num"] = getRoundBaseTypeNum((pBuf + i)->roundType);
            item["load_line"] = (pBuf + i)->feederName;
            item["load_val"] = (pBuf + i)->pValue;
            items.append(item);
        }
    }

    if (pBuf != nullptr)
    {
        free(pBuf);
        pBuf = nullptr;
    }

    DZJZ_Section section(QString::fromLocal8Bit("�����澯::��������"), headerList, descsList, items);

    DZJZ_FileGenerator fileGen(fileName, "ufvls_act", strDateTime);
    QList<DZJZ_Section> sections = {section};
    fileGen.Generate(sections);
    return true; // ����true��ʾ�ɹ�
}
