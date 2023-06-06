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
 * @brief 保留3位小数
 *
 * @param value
 * @return std::string
 */
std::string roundToThreeDecimalPlaces(float value)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(3) << value;
    return stream.str();
}

/**
 * @brief 根据rtu获取轮次项
 *
 * @param nRtuNo
 * @return dfJson::Value
 */
dfJson::Value getRoundItemInfoByRtu(int nRtuNo)
{
    dfJson::Value items(dfJson::arrayValue);

    RdbSQL rsql;
    rsql.selectfrom("dzjzrounditem");
    rsql.selectattr("id");
    rsql.selectattr("roundtype");
    rsql.selectattr("name");
    rsql.selectattr("pvalue");
    rsql.selectattr("strapreal");
    rsql.selectattr("loadtype");
    rsql.selectattr("functype");
    rsql.where("deviceid", DATATYPE_INT, &nRtuNo);
    if (rsql.select(RTDB_EMS_USER, "") == RDB_OK)
    {
        int nCnt = rsql.getrcdcnt();
        for (int i = 0; i < nCnt; i++)
        {
            dfJson::Value item;
            item["id"] = rsql.get_intval(i, "id");
            item["roundType"] = rsql.get_intval(i, "roundtype");
            item["name"] = rsql.get_string(i, "name");
            item["pValue"] = roundToThreeDecimalPlaces(rsql.get_floatval(i, "pvalue"));
            item["strapReal"] = rsql.get_intval(i, "strapreal");
            item["loadType"] = rsql.get_intval(i, "loadtype");
            item["funcType"] = rsql.get_intval(i, "functype");
            items.append(item);
        }
    }

    return items;
}

/**
 * @brief 根据厂站代码获取厂站描述
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

    return "未知";
}

/**
 * @brief 根据rtu获取装置信息
 *
 * @param nRtuNo
 * @param jResponse
 */
void getDeviceInfoByRtu(int nRtuNo, dfJson::Value& jResponse)
{
    dfJson::Value jFrequency;
    dfJson::Value jFRounds(dfJson::arrayValue);

    dfJson::Value jVoltage;
    dfJson::Value jVRounds(dfJson::arrayValue);

    RdbSQL rsql;
    rsql.selectfrom("dzjzrounditem");
    rsql.selectattr("roundtype");
    rsql.selectattr("strapreal");
    rsql.selectattr("realvalue");
    rsql.selectattr("realtime");
    rsql.selectattr("functype");
    rsql.where("deviceid", DATATYPE_INT, &nRtuNo);
    if (rsql.select(RTDB_EMS_USER, "") == RDB_OK)
    {
        int nCnt = rsql.getrcdcnt();
        std::set<std::pair<int, int>> uniqueKey; // 用于记录已经出现的联合键值

        for (int i = 0; i < nCnt; i++)
        {
            int roundType = rsql.get_intval(i, "roundType");
            int nFuncType = rsql.get_intval(i, "functype");
            // 检查联合键值是否已经出现过，如果是，则跳过当前记录
            if (uniqueKey.count(std::make_pair(roundType, nFuncType)) > 0)
                continue;

            uniqueKey.insert(std::make_pair(roundType, nFuncType));

            dfJson::Value jRound;
            jRound["type"] = roundType;
            jRound["strap"] = rsql.get_intval(i, "strapreal");
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

    return;
}

/**
 * @brief 批量召唤
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
 * @brief 集中式装置巡视
 *
 * @param jsonPara
 * @return dfJson::Value
 */
dfJson::Value getCentrialDevice(const std::string& jsonPara)
{
    // 解析json
    dfJson::Value jPara;
    dfJson::CharReaderBuilder readerBuilder;
    std::istringstream iss(jsonPara);
    dfJson::parseFromStream(readerBuilder, iss, &jPara, nullptr);

    int nRtuNo = jPara["rtu"].asInt();

    dfJson::Value jResponse;
    jResponse["optype"] = "getCentrialDevice";

    // 获取装置所有轮次信息
    jResponse["items"] = getRoundItemInfoByRtu(nRtuNo);

    getDeviceInfoByRtu(nRtuNo, jResponse);

    return jResponse;
}

/**
 * @brief 将整数日期和时间转换为字符串
 *
 * @param date
 * @param time
 * @return QString
 */
QString convertToTimeString(int date, int time)
{
    // 将整数转换为QString类型
    QString dateString = QString::number(date);
    QString timeString = QString::number(time);

    // 使用QString的insert函数在日期和时间字符串中插入分隔符
    dateString.insert(4, "-").insert(7, "-");
    timeString.insert(2, ":").insert(5, ":").insert(8, ".");

    // 将日期和时间字符串合并为完整的时间字符串
    QString dateTimeString = dateString + " " + timeString;

    // 返回最终的时间字符串
    return dateTimeString;
}

/**
 * @brief 获取实时事项
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
        jResponse["substaion"] = GetSubstationDesc(aEvent.group_name).toAscii().data(); // 厂站
        jResponse["state"] = (qint16)aEvent.state_value == 1 ? "动作" : "复归";
        return jResponse;
    }
    else
    {
        return dfJson::Value();
    }
}
