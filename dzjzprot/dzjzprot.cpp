#include "dzjzprot.h"

#include <QDateTime>
#include <QMap>
#include <QMutexLocker>
#include <QString>
#include <QVector>
#include <iomanip>
#include <set>

#include "dzjzsrv.h"
#include "eventdef.h"
#include "fixvaluesrv.h"
#include "netdef.h"

/**
 * @brief ����rtu��ȡ����ʽװ����Ϣ
 *
 * @param nRtuNo
 * @return dfJson::Value
 */
dfJson::Value getDeviceInfoByRtu(int nRtuNo)
{
    dfJson::Value jResponse;
    jResponse["optype"] = "getCentrialDevice";

    dfJson::Value items(dfJson::arrayValue);
    dfJson::Value jFrequency;
    dfJson::Value jFRounds(dfJson::arrayValue);
    dfJson::Value jVoltage;
    dfJson::Value jVRounds(dfJson::arrayValue);

    RdbSQL rsql;
    rsql.selectfrom("dzjzrounditem");
    rsql.selectattr("id");
    rsql.selectattr("roundtype");
    rsql.selectattr("name");
    rsql.selectattr("pvalue");
    rsql.selectattr("strapreal");
    rsql.selectattr("loadtype");
    rsql.selectattr("functype");
    rsql.selectattr("realvalue");
    rsql.selectattr("realtime");
    rsql.where("deviceid", DATATYPE_INT, &nRtuNo);
    if (rsql.select(RTDB_EMS_USER, "") == RDB_OK)
    {
        int nCnt = rsql.getrcdcnt();
        std::set<std::pair<int, int>> uniqueKey; // ���ڼ�¼�Ѿ����ֵ����ϼ�ֵ

        for (int i = 0; i < nCnt; i++)
        {
            dfJson::Value item;
            item["id"] = rsql.get_intval(i, "id");

            int roundType = rsql.get_intval(i, "roundtype");
            int nFuncType = rsql.get_intval(i, "functype");

            item["roundType"] = roundType;
            item["name"] = rsql.get_string(i, "name");
            item["pValue"] = roundToThreeDecimalPlaces(rsql.get_floatval(i, "pvalue"));
            item["strapReal"] = rsql.get_intval(i, "strapreal");
            item["loadType"] = rsql.get_intval(i, "loadtype");
            item["funcType"] = nFuncType;
            items.append(item);

            // ������ϼ�ֵ�Ƿ��Ѿ����ֹ�������ǣ���������ǰ��¼
            if (uniqueKey.count(std::make_pair(roundType, nFuncType)) > 0)
                continue;

            uniqueKey.insert(std::make_pair(roundType, nFuncType));

            dfJson::Value jRound;
            jRound["type"] = roundType;
            jRound["strap"] = item["strapReal"];
            jRound["fixValue"] = roundToThreeDecimalPlaces(rsql.get_floatval(i, "realvalue"));
            jRound["time"] = roundToThreeDecimalPlaces(qRound(rsql.get_floatval(i, "realtime")) / 1000);
            if (nFuncType == 1)
            {
                jFRounds.append(jRound);
            }
            else if (nFuncType == 2)
            {
                jVRounds.append(jRound);
            }
        }
    }

    jFrequency["rounds"] = jFRounds;
    jResponse["frequency"] = jFrequency;

    jVoltage["rounds"] = jVRounds;
    jResponse["voltage"] = jVoltage;

    jResponse["items"] = items;

    return jResponse;
}

/**
 * @brief ���ݳ�վ�����ȡ��վ����
 *
 * @param name
 * @return QString
 */
QString GetSubstationDesc(char* name)
{
    static QMap<QString, QString> cache;
    static QMutex mutex;

    QString qName(name);

    {
        QMutexLocker locker(&mutex);
        auto it = cache.find(qName);
        if (it != cache.end())
            return it.value();
    }

    RdbSQL rsql;
    rsql.selectfrom("substation");
    rsql.selectattr("describe");
    rsql.where("name", DATATYPE_STRING, name);
    if (rsql.select(RTDB_EMS_USER, "") == RDB_OK)
    {
        int nCnt = rsql.getrcdcnt();
        if (nCnt > 0)
        {
            QString description = rsql.get_string(0, "describe");

            QMutexLocker locker(&mutex);
            cache[qName] = description;

            return description;
        }
    }

    return "δ֪";
}

/**
 * @brief �����ٻ�
 *
 * @return int
 */
int executeBatch()
{
    CFixValueService fixValueService;
    return fixValueService.executeBatch();
}

std::string executeInspectionByCond(const std::string& jsonPara, ProgressCallback progressCallback)
{
    CFixValueService fixValueService;
    return fixValueService.executeInspectionByCond(jsonPara, progressCallback);
}

/**
 * @brief ����ʽװ��Ѳ��
 *
 * @param jsonPara
 * @return dfJson::Value
 */
dfJson::Value getCentrialDevice(const std::string& jsonPara)
{
    // ����json
    dfJson::Value jPara;
    dfJson::CharReaderBuilder readerBuilder;
    std::istringstream iss(jsonPara);
    dfJson::parseFromStream(readerBuilder, iss, &jPara, nullptr);

    int nRtuNo = jPara["rtu"].asInt();

    dfJson::Value jResponse = getDeviceInfoByRtu(nRtuNo);

    return jResponse;
}

/**
 * @brief ���������ں�ʱ��ת��Ϊ�ַ���
 *
 * @param date
 * @param time
 * @return QString
 */
QString convertToTimeString(int date, int time)
{
    // ������ת��ΪQString����
    QString dateString = QString::number(date);
    QString timeString = QString::number(time);

    // ʹ��QString��insert���������ں�ʱ���ַ����в���ָ���
    dateString.insert(4, "-").insert(7, "-");
    timeString.insert(2, ":").insert(5, ":").insert(8, ".");

    // �����ں�ʱ���ַ����ϲ�Ϊ������ʱ���ַ���
    QString dateTimeString = dateString + " " + timeString;

    // �������յ�ʱ���ַ���
    return dateTimeString;
}

/**
 * @brief ��ȡʵʱ����
 *
 * @return dfJson::Value
 */
dfJson::Value getRealEvent()
{
    event_proc eventProc;
    SYS_EVENT aEvent;
    memset(&aEvent, 0, sizeof(SYS_EVENT));
    if (eventProc.read_event(&aEvent))
    {
        if (aEvent.event_type != ENTTYPE_DZJZ)
        {
            return dfJson::Value();
        }

        dfJson::Value jResponse;
        jResponse["optype"] = "getRealEvent";
        jResponse["info"] = aEvent.char_info;
        jResponse["time"] = convertToTimeString(aEvent.ymd, aEvent.hmsms).toAscii().data();
        jResponse["substation"] = GetSubstationDesc(aEvent.group_name).toAscii().data(); // ��վ
        jResponse["state"] = (qint16)aEvent.state_value == 1 ? "����" : "����";
        return jResponse;
    }
    else
    {
        return dfJson::Value();
    }
}
