#include "dbmanager.h"

#include <QMap>
#include <QDebug>

#include "netsub.h"
#include "dto.h"
#include "dzjzconfigutil.h"

DBManager::DBManager(QObject *parent) : QObject(parent)
{
    m_areaTypeMap = getDMConfig("低周减载区域类型");
    m_roundFuncTypeMap = getDMConfig("低周减载轮次功能类型");
    m_loadTypeMap = getDMConfig("低周减载负荷类型");
    m_strapMap = getDMConfig("低周减载投退计划类型");
    m_deviceTypeMap = getDMConfig("低周减载装置类型");
    m_deviceFuncTypeMap = getDMConfig("低周减载装置功能类型");
    m_roundTypeMap = getRoundTypeMap();
    m_staIdNameMap = getStaIdNameMap();
    m_staJson = getStaJson();

    reloadRound();

    reloadAreaTable();

    m_lineList = getLineList();
    m_lineIdNameMap = getLineIdNameMap(m_lineList);
    m_lineJson = getLineJson(m_lineList);

    m_breakerList = getBreakList();
    m_breakIdNameMap = getBreakerIdNameMap(m_breakerList);
    m_breakerJson = getBreakerJson(m_breakerList);

    m_rtuList = getRtuList();
    m_rtuIdNameMap = getRtuIdNameMap(m_rtuList);
    m_rtuJson = getRtuJson(m_rtuList);

    reloadDevice();

    m_tmList = getTMList();
    m_tmIdNameMap = getTMNameMap(m_tmList);
    m_tmJson = getTMJson(m_tmList);

    m_tsList = getTSList();
    m_tsIdNameMap = getTSNameMap(m_tsList);
    // m_tsJson = getTSJson(m_tsList);

    reloadFixValue();
}

QVector<AreaVo> DBManager::getAreaVoList()
{
    QString query = QString::fromLocal8Bit(
        "select 编号,名称,类型,厂站编号 from xopensdb.低周减载区域参数表 a"
        " left join xopensdb.低周减载区域厂站关联表 b on a.编号=b.所属分区 order by 编号");
    return getList<AreaVo>(query);
}

QVector<RoundDto> DBManager::getRoundList()
{
    QString query = QString::fromLocal8Bit("select 编号,名称,所属地区,功能类型,轮类型,频率或电压整定值,动作延时整定值,下发应切荷量,计划切荷比例 from xopensdb.dbo.低周减载轮次参数表");
    return getList<RoundDto>(query);
}

/**
 * @brief 轮次类型
 *
 * @return QVector<RoundDto>
 */
QMap<int, QString> DBManager::getRoundTypeMap()
{
    QString query = QString::fromLocal8Bit("select 编号,名称 from xopensdb.dbo.低周减载轮次类型表");
    return getMapFromQuery<RoundTypeDto>(query);
}

QMap<int, QString> DBManager::getDMConfig(const char *strType)
{
    QString query = QString::fromLocal8Bit("select 数值,项目 from xopensdb.dbo.DM配置表 where 配置类型='%1'")
                        .arg(QString::fromLocal8Bit(strType));
    return getMapFromQuery<DMDto>(query);
}

int DBManager::deleteTable(int id, const char *tableName, const char *primaryKey)
{
    FUNC_STR func;
    CS_DATAFMT *datafmt = NULL;

    memset(&func, 0, sizeof(FUNC_STR));
    func.serverno = SERVER_DEFAULT;
    func.func = ISQL_COMMAND;
    sprintf(func.table, "%s", tableName);
    sprintf(func.isql, "delete from %s where %s=%d", tableName, primaryKey, id);

    return dbfisqlcommand(&func);
}

int DBManager::updateAreaTable(const AreaVo &area)
{
    AreaDto areaDto;
    areaDto.id = area.id;
    areaDto.type = area.type;
    strcpy(areaDto.name, area.name);
    if (updateTable(areaDto, "xopensdb.低周减载区域参数表") != CS_SUCCEED)
    {
        return CS_FAIL;
    }

    AreaSubStationDto areaSubStationDto;
    areaSubStationDto.areaId = area.id;
    strcpy(areaSubStationDto.staId, area.staId);
    if (deleteTable(area.id, "xopensdb.低周减载区域厂站关联表", "所属分区") != CS_SUCCEED)
    {
        return CS_FAIL;
    }

    return insertTable(areaSubStationDto, "xopensdb.低周减载区域厂站关联表");
}

int DBManager::insertAreaTable(const AreaVo &area)
{
    AreaDto areaDto;
    areaDto.id = area.id;
    areaDto.type = area.type;
    strcpy(areaDto.name, area.name);
    if (insertTable(areaDto, "xopensdb.低周减载区域参数表") != CS_SUCCEED)
    {
        return CS_FAIL;
    }

    AreaSubStationDto areaSubStationDto;
    areaSubStationDto.areaId = area.id;
    strcpy(areaSubStationDto.staId, area.staId);

    return insertTable(areaSubStationDto, "xopensdb.低周减载区域厂站关联表");
}

void DBManager::reloadAreaTable()
{
    m_areaList = getAreaVoList();
    m_allAreaIdNameMap = getAreaIdNameMap(m_areaList, 1);
    m_subAreaIdNameMap = getAreaIdNameMap(m_areaList, 2);
}

int DBManager::insertRoundTable(const RoundDto &round)
{
    return insertTable(round, "xopensdb.dbo.低周减载轮次参数表");
}

int DBManager::updateRoundTable(const RoundDto &round)
{
    return updateTable(round, "xopensdb.dbo.低周减载轮次参数表");
}

QVector<RoundItemDto> DBManager::getRoundItemList(int roundId)
{
    QString query = QString::fromLocal8Bit(
        "select 编号,名称,所属分区,所属轮次,关联馈线,负荷类型,投退计划,关联开关,有功代码,关联装置,关联出口"
        " from xopensdb.低周减载轮次项参数表 where 1=1");

    if (roundId > 0)
    {
        query += QString::fromLocal8Bit(" and 所属轮次=%1").arg(roundId);
    }

    return getList<RoundItemDto>(query);
}

/**
 * @brief Get the Max I D object
 *
 * @param tableName
 * @return int
 */
int DBManager::getMaxIDFromDataBase(const char *tableName)
{
    int i = 0;
    int nOrder = 1;
    FUNC_STR func;
    CS_FLOAT *p = NULL;
    memset(&func, 0, sizeof(FUNC_STR));
    func.func = SELECT_ISQL_RESULTS;
    func.serverno = SERVER_DEFAULT;
    sprintf(func.isql, "select max(编号) from %s", tableName);

    dbfselectisqlresults(&func, NULL, (void **)&p);

    if (func.ret_roxnum == 1)
    {
        nOrder = (int)*p + 1;
    }

    if (p != NULL)
    {
        free(p);
        p = NULL;
    }

    // DFLOG_DEBUG("nOrder = %d", nOrder);
    return nOrder;
}

QMap<int, QString> DBManager::getAreaIdNameMap(const QVector<AreaVo> &areas, int type)
{
    QMap<int, QString> areaMap;
    for (const AreaVo &area : areas)
    {
        if (area.type == type)
        {
            areaMap[area.id] = QString::fromLocal8Bit(area.name);
        }
    }
    return areaMap;
}

QMap<int, QString> DBManager::getRoundIdNameMap(const QVector<RoundDto> &list)
{
    QMap<int, QString> map;
    for (const RoundDto &item : list)
    {
        map[item.id] = QString::fromLocal8Bit(item.name);
    }
    return map;
}

QMap<QString, QString> DBManager::getStaIdNameMap()
{
    QString query = QString::fromLocal8Bit("select 编号,名称 from xopensdb.dbo.厂站参数表");
    QMap<QString, QString> map = getStrMapFromQuery<SubstationDto>(query);
    map[""] = "";
    return map;
}

dfJson::Value DBManager::getStaJson()
{
    dfJson::Value root(dfJson::arrayValue);
    for (auto it = m_staIdNameMap.begin(); it != m_staIdNameMap.end(); ++it)
    {
        if (it.key().isEmpty())
            continue;
        dfJson::Value item;
        item["id"] = it.key().toLocal8Bit().data();
        item["name"] = it.value().toLocal8Bit().data();
        item["type"] = "substationItem";
        root.append(item);
    }
    return root;
}

QVector<LineDto> DBManager::getLineList()
{
    QString query = QString::fromLocal8Bit(
        "select a.编号, a.名称, a.线路始端厂站名, b.名称 from 线路参数表"
        " a, 厂站参数表 b where a.线路始端厂站名 = b.编号 ");

    return getList<LineDto>(query);
}

QVector<BreakDto> DBManager::getBreakList()
{
    QString query = QString::fromLocal8Bit(
        "select a.代码,a.描述, a.厂站代码, b.名称 from 遥信参数表 a, 厂站参数表 b "
        "where a.厂站代码=b.编号 and a.类型>=300 and a.类型<400");

    return getList<BreakDto>(query);
}

QMap<QString, QString> DBManager::getBreakIdNameMap()
{
    QString query = QString::fromLocal8Bit("select 代码,描述 from xopensdb.dbo.遥信参数表");
    return getStrMapFromQuery<BreakDto>(query);
}

int DBManager::updateRoundItemTable(const RoundItemDto &item)
{
    return updateTable(item, "xopensdb.dbo.低周减载轮次项参数表");
}

int DBManager::insertRoundItemTable(const RoundItemDto &item)
{
    return insertTable(item, "xopensdb.dbo.低周减载轮次项参数表");
}

QMap<QString, QString> DBManager::getBreakerIdNameMap(const QVector<BreakDto> &breakers)
{
    QMap<QString, QString> breakIdNameMap;
    for (const auto &breaker : breakers)
    {
        breakIdNameMap.insert(QString::fromLocal8Bit(breaker.id), QString::fromLocal8Bit(breaker.name));
    }
    return breakIdNameMap;
}

QMap<QString, QString> DBManager::getLineIdNameMap(const QVector<LineDto> &lines)
{
    QMap<QString, QString> lineIdNameMap;
    for (const auto &line : lines)
    {
        lineIdNameMap.insert(QString::fromLocal8Bit(line.id), QString::fromLocal8Bit(line.name));
    }
    return lineIdNameMap;
}

void DBManager::reloadRound()
{
    m_roundList = getRoundList();
    m_roundIdNameMap = getRoundIdNameMap(m_roundList);
    m_roundJson = getRoundJson();
}

int DBManager::updateTable(const QString &sql)
{
    FUNC_STR func;
    CS_DATAFMT *datafmt = NULL;

    memset(&func, 0, sizeof(FUNC_STR));
    func.serverno = SERVER_DEFAULT;
    func.func = ISQL_COMMAND;
    sprintf(func.isql, "%s", sql.toLocal8Bit().data());

    return dbfisqlcommand(&func);
}

int DBManager::deleteRoundTable(int id)
{
    QString sql = QString::fromLocal8Bit("update xopensdb.低周减载轮次项参数表 set 所属轮次=0 where 所属轮次=%1").arg(id);

    if (updateTable(sql) != CS_SUCCEED)
        return CS_FAIL;

    if (deleteTable(id, "xopensdb.dbo.低周减载轮次参数表") != CS_SUCCEED)
        return CS_FAIL;

    return CS_SUCCEED;
}

QVector<DeviceDto> DBManager::getDeviceList()
{
    QString query = QString::fromLocal8Bit("select 编号,名称,装置类型,功能类型,关联轮次项数,所属厂站 from xopensdb.dbo.低周减载装置参数表");
    return getList<DeviceDto>(query);
}

int DBManager::updateDeviceTable(const DeviceDto &device)
{
    return updateTable(device, "xopensdb.dbo.低周减载装置参数表");
}

int DBManager::insertRoundItemTable(const DeviceDto &device)
{
    return insertTable(device, "xopensdb.dbo.低周减载装置参数表");
}

QVector<RtuDto> DBManager::getRtuList()
{
    QString query = QString::fromLocal8Bit(
        "select a.序号,a.描述,a.子站代码,b.名称 from xopensdb.RTU参数表 a, 厂站参数表 b"
        " where a.子站代码=b.编号 and a.RTU类型=0 order by a.序号");
    return getList<RtuDto>(query);
}

QMap<int, QString> DBManager::getRtuIdNameMap(const QVector<RtuDto> &rtus)
{
    QMap<int, QString> rtuIdNameMap;
    for (const auto &rtu : rtus)
    {
        rtuIdNameMap.insert(rtu.id, QString::fromLocal8Bit(rtu.name));
    }
    return rtuIdNameMap;
}

QString DBManager::getStaByRtuId(int rtuId)
{
    for (const auto &rtu : m_rtuList)
    {
        if (rtu.id == rtuId)
        {
            return rtu.staId;
        }
    }
}

QVector<DeviceParaDto> DBManager::getDeviceParaList(int id)
{
    QString query = QString::fromLocal8Bit(
                        "select 所属装置,轮次编号,压板ID,频率或电压定值ID,动作延时定值ID,告警信号ID,动作信号ID,出口矩阵ID"
                        " from xopensdb.低周减载装置参数设定表 where 所属装置=%1 order by 所属装置,轮次编号")
                        .arg(id);
    return getList<DeviceParaDto>(query);
}

QVector<FixValueDto> DBManager::getFixValueList()
{
    QString query = QString::fromLocal8Bit(
        "select RTU号,装置类型,组号,序号,描述 from xopensdb.低周减载装置定值信息表 order by 装置类型,组号,序号");
    return getList<FixValueDto>(query);
}

int DBManager::deleteDeviceParaTable(int deviceId, int roundId)
{
    FUNC_STR func;
    CS_DATAFMT *datafmt = NULL;

    memset(&func, 0, sizeof(FUNC_STR));
    func.serverno = SERVER_DEFAULT;
    func.func = ISQL_COMMAND;
    sprintf(func.table, "%s", "xopensdb.低周减载装置参数设定表");
    sprintf(func.isql, "delete from xopensdb.低周减载装置参数设定表 where 所属装置=%d and 轮次编号=%d", deviceId, roundId);
    return dbfisqlcommand(&func);
}

int DBManager::updateDeviceParaTable(const DeviceParaDto &devicePara)
{
    if (deleteDeviceParaTable(devicePara.id, devicePara.no) != CS_SUCCEED)
        return CS_FAIL;

    return insertTable(devicePara, "xopensdb.dbo.低周减载装置参数设定表");
}

int DBManager::insertDeviceParaTable(const DeviceParaDto &devicePara)
{
    return insertTable(devicePara, "xopensdb.dbo.低周减载装置参数设定表");
}

int DBManager::deleteDeviceTable(int id)
{
    if (deleteTable(id, "xopensdb.dbo.低周减载装置参数表") != CS_SUCCEED)
        return CS_FAIL;

    if (deleteTable(id, "xopensdb.dbo.低周减载装置参数设定表", "所属装置") != CS_SUCCEED)
        return CS_FAIL;

    return CS_SUCCEED;
}

QMap<int, QString> DBManager::getDeviceIdNameMap(const QVector<DeviceDto> &list)
{
    QMap<int, QString> map;
    for (const auto &device : list)
    {
        map[device.id] = QString::fromLocal8Bit(device.name);
    }

    return map;
}

void DBManager::reloadDevice()
{
    m_deviceList = getDeviceList();
    m_deviceIdNameMap = getDeviceIdNameMap(m_deviceList);
    m_deviceJson = getDeviceJson(m_deviceList);
}

QVector<TMDto> DBManager::getTMList()
{
    QString query = QString::fromLocal8Bit(
        "select a.代码,a.描述, a.厂站代码,b.名称 from 遥测参数表 a, 厂站参数表 b "
        "where a.厂站代码=b.编号 and (a.描述 like '%%有功%%' or a.描述 like '%%P%%' or a.描述 like '%%p%%') order by a.代码");

    return getList<TMDto>(query);
}

QMap<QString, QString> DBManager::getTMNameMap(const QVector<TMDto> &list)
{
    QMap<QString, QString> map;
    for (const auto &tm : list)
    {
        map[tm.id] = QString::fromLocal8Bit(tm.name);
    }

    return map;
}

dfJson::Value DBManager::getTMJson(const QVector<TMDto> &list)
{
    QMap<QString, QVector<TMDto>> stationMap;

    // 将所有线路按变电站 ID 分类
    for (const auto &tm : list)
    {
        stationMap[QString::fromLocal8Bit(tm.staId)].push_back(tm);
    }

    // 构造 JSON 对象
    dfJson::Value root(dfJson::arrayValue); // 创建一个空的Json数组

    QMap<QString, QVector<TMDto>>::iterator it;
    for (it = stationMap.begin(); it != stationMap.end(); ++it)
    {
        // 变电站
        dfJson::Value station(dfJson::objectValue);    // 创建一个空的Json对象
        station["id"] = it.key().toLocal8Bit().data(); // 变电站 ID
        station["name"] = it.value().first().staName;  // 变电站名称
        station["type"] = "substation";                // 变电站类型

        dfJson::Value children(dfJson::arrayValue); // 创建一个空的Json数组

        // 开关
        for (const auto &tm : it.value())
        {
            dfJson::Value child(dfJson::objectValue); // 创建一个空的Json对象
            child["id"] = tm.id;
            child["name"] = tm.name;
            child["type"] = "tm";
            children.append(child);
        }

        station["children"] = children;
        root.append(station);
    }

    // qDebug() << QString::fromLocal8Bit(jsonToString(root).c_str());

    return root;
}

QVector<TSDto> DBManager::getTSList()
{
    QString query = QString::fromLocal8Bit(
        "select a.代码,a.描述,a.厂站代码,b.名称,a.RTU号,c.描述 from 遥信参数表 a,"
        "厂站参数表 b, RTU参数表 c where a.厂站代码=b.编号 and a.RTU号=c.序号 order by a.RTU号, a.YX号");

    return getList<TSDto>(query);
}

QMap<QString, QString> DBManager::getTSNameMap(const QVector<TSDto> &list)
{
    QMap<QString, QString> map;

    for (const auto &ts : list)
    {
        map[ts.id] = QString::fromLocal8Bit(ts.name);
    }
    return map;
}

dfJson::Value DBManager::getTSJson(int nRtuNo)
{
    dfJson::Value root;

    for (const auto &ts : m_tsList)
    {
        if (nRtuNo != ts.rtuNo)
            continue;

        dfJson::Value tsJson;
        tsJson["id"] = ts.id;
        tsJson["name"] = ts.name;
        tsJson["type"] = "ts";
        root.append(tsJson);
    }

    return root;
}

#if 0 
dfJson::Value DBManager::getTSJson(int nRtuNo, const QVector<TSDto> &list)
{
    QMap<QString, dfJson::Value> substations;
    QMap<QString, QMap<int, dfJson::Value>> rtus;

    for (const auto &ts : list)
    {
        QString staId = QString::fromLocal8Bit(ts.staId);
        int rtuNo = ts.rtuNo;

        if (substations.find(staId) == substations.end())
        {
            substations[staId]["id"] = ts.staId;
            substations[staId]["name"] = ts.staName;
            substations[staId]["type"] = "substation";
        }

        if (rtus[staId].find(rtuNo) == rtus[staId].end())
        {
            rtus[staId][rtuNo]["id"] = QString::number(ts.rtuNo).toLocal8Bit().data();
            rtus[staId][rtuNo]["name"] = ts.rtuName;
            rtus[staId][rtuNo]["type"] = "rtu";
        }

        dfJson::Value tsJson;
        tsJson["id"] = ts.id;
        tsJson["name"] = ts.name;
        tsJson["type"] = "ts";
        rtus[staId][rtuNo]["children"].append(tsJson);
    }

    for (auto it = substations.begin(); it != substations.end(); ++it)
    {
        QString staId = it.key();
        for (auto itRtu = rtus[staId].begin(); itRtu != rtus[staId].end(); ++itRtu)
        {
            it.value()["children"].append(itRtu.value());
        }
    }

    dfJson::Value root;
    for (auto it = substations.begin(); it != substations.end(); ++it)
    {
        root.append(it.value());
    }

    // qDebug() << QString::fromLocal8Bit(jsonToString(root).c_str());

    return root;
}
#endif

dfJson::Value DBManager::getRtuJson(const QVector<RtuDto> &rtus)
{
    QMap<QString, QVector<RtuDto>> stationMap;

    // 将所有线路按变电站 ID 分类
    for (const auto &rtu : rtus)
    {
        stationMap[QString::fromLocal8Bit(rtu.staId)].push_back(rtu);
    }

    // 构造 JSON 对象
    dfJson::Value root(dfJson::arrayValue); // 创建一个空的Json数组

    QMap<QString, QVector<RtuDto>>::iterator it;
    for (it = stationMap.begin(); it != stationMap.end(); ++it)
    {
        // 变电站
        dfJson::Value station(dfJson::objectValue);    // 创建一个空的Json对象
        station["id"] = it.key().toLocal8Bit().data(); // 变电站 ID
        station["name"] = it.value().first().staName;  // 变电站名称
        station["type"] = "substation";                // 变电站类型

        dfJson::Value children(dfJson::arrayValue); // 创建一个空的Json数组

        // 线路
        for (const auto &rtu : it.value())
        {
            dfJson::Value child(dfJson::objectValue);                   // 创建一个空的Json对象
            child["id"] = QString::number(rtu.id).toLocal8Bit().data(); // 线路 ID
            child["name"] = rtu.name;                                   // 线路名称
            child["type"] = "rtu";                                      // 线路类型
            children.append(child);
        }

        station["children"] = children;
        root.append(station);
    }

    // qDebug() << QString::fromLocal8Bit(jsonToString(root).c_str());

    return root;
}

dfJson::Value DBManager::getFixValueJson(int id)
{
    // 构造 JSON 对象
    dfJson::Value root(dfJson::arrayValue);

    for (const auto &fixValue : m_fixValueList)
    {
        if (fixValue.rtuNo != id)
            continue;
        dfJson::Value child(dfJson::objectValue);
        child["id"] = QString("%1:%2:%3").arg(fixValue.type).arg(fixValue.groupNo).arg(fixValue.no).toLocal8Bit().data();
        child["name"] = fixValue.name;
        child["type"] = "fixvalue";
        root.append(child);
    }

    // qDebug() << QString::fromLocal8Bit(jsonToString(root).c_str());

    return root;
}

QMap<QString, QString> DBManager::getFixValuMap(const QVector<FixValueDto> &list)
{
    QMap<QString, QString> map;
    for (const auto &fixValue : list)
    {
        QString qsKey = QString("%1:%2:%3").arg(fixValue.type).arg(fixValue.groupNo).arg(fixValue.no).toLocal8Bit().data();
        QString qsValue = QString::fromLocal8Bit(fixValue.name);
        map.insert(qsKey, qsValue);
    }

    return map;
}

void DBManager::reloadFixValue()
{
    m_fixValueList = getFixValueList();
    m_fixValueMap = getFixValuMap(m_fixValueList);
}

dfJson::Value DBManager::getBreakerJson(const QVector<BreakDto> &breakers)
{
    QMap<QString, QVector<BreakDto>> stationMap;

    // 将所有线路按变电站 ID 分类
    for (const auto &breaker : breakers)
    {
        // qDebug("line.staName=%s line.name=%s", line.staName, line.name);
        stationMap[QString::fromLocal8Bit(breaker.staId)].push_back(breaker);
    }

    // 构造 JSON 对象
    dfJson::Value root(dfJson::arrayValue); // 创建一个空的Json数组

    QMap<QString, QVector<BreakDto>>::iterator it;
    for (it = stationMap.begin(); it != stationMap.end(); ++it)
    {
        // 变电站
        dfJson::Value station(dfJson::objectValue);    // 创建一个空的Json对象
        station["id"] = it.key().toLocal8Bit().data(); // 变电站 ID
        station["name"] = it.value().first().staName;  // 变电站名称
        station["type"] = "substation";                // 变电站类型

        dfJson::Value children(dfJson::arrayValue); // 创建一个空的Json数组

        // 开关
        for (const auto &breaker : it.value())
        {
            dfJson::Value child(dfJson::objectValue); // 创建一个空的Json对象
            child["id"] = breaker.id;                 // 开关ID
            child["name"] = breaker.name;             // 开关名称
            child["type"] = "breaker";                // 开关类型
            children.append(child);
        }

        station["children"] = children;
        root.append(station);
    }

    // qDebug() << QString::fromLocal8Bit(jsonToString(root).c_str());

    return root;
}

dfJson::Value DBManager::getDeviceJson(const QVector<DeviceDto> &devices)
{
    QMap<QString, QVector<DeviceDto>> stationMap;

    // 将所有线路按变电站 ID 分类
    for (const auto &device : devices)
    {
        stationMap[QString::fromLocal8Bit(device.staId)].push_back(device);
    }

    // 构造 JSON 对象
    dfJson::Value root(dfJson::arrayValue); // 创建一个空的Json数组

    QMap<QString, QVector<DeviceDto>>::iterator it;
    for (it = stationMap.begin(); it != stationMap.end(); ++it)
    {
        // 变电站
        dfJson::Value station(dfJson::objectValue);                                      // 创建一个空的Json对象
        station["id"] = it.key().toLocal8Bit().data();                                   // 变电站 ID
        station["name"] = m_staIdNameMap[it.value().first().staId].toLocal8Bit().data(); // 变电站名称
        station["type"] = "substation";                                                  // 变电站类型

        dfJson::Value children(dfJson::arrayValue); // 创建一个空的Json数组

        // 线路
        for (const auto &device : it.value())
        {
            dfJson::Value child(dfJson::objectValue); // 创建一个空的Json对象
            child["id"] = QString::number(device.id).toLocal8Bit().data();
            child["name"] = device.name;
            child["type"] = "device";
            children.append(child);
        }

        station["children"] = children;
        root.append(station);
    }

    // qDebug() << QString::fromLocal8Bit(jsonToString(root).c_str());

    return root;
}

dfJson::Value DBManager::getLineJson(const QVector<LineDto> &lines)
{
    QMap<QString, QVector<LineDto>> stationMap;

    // 将所有线路按变电站 ID 分类
    for (const auto &line : lines)
    {
        // qDebug("line.staName=%s line.name=%s", line.staName, line.name);
        stationMap[QString::fromLocal8Bit(line.staId)].push_back(line);
    }

    // 构造 JSON 对象
    dfJson::Value root(dfJson::arrayValue); // 创建一个空的Json数组

    QMap<QString, QVector<LineDto>>::iterator it;
    for (it = stationMap.begin(); it != stationMap.end(); ++it)
    {
        // 变电站
        dfJson::Value station(dfJson::objectValue);    // 创建一个空的Json对象
        station["id"] = it.key().toLocal8Bit().data(); // 变电站 ID
        station["name"] = it.value().first().staName;  // 变电站名称
        station["type"] = "substation";                // 变电站类型

        dfJson::Value children(dfJson::arrayValue); // 创建一个空的Json数组

        // 线路
        for (const auto &line : it.value())
        {
            dfJson::Value child(dfJson::objectValue); // 创建一个空的Json对象
            child["id"] = line.id;                    // 线路 ID
            child["name"] = line.name;                // 线路名称
            child["type"] = "line";                   // 线路类型
            children.append(child);
        }

        station["children"] = children;
        root.append(station);
    }

    // qDebug() << QString::fromLocal8Bit(jsonToString(root).c_str());

    return root;
}

int DBManager::updateRoundItemCount(int deviceId, int nCount)
{
    FUNC_STR func;
    CS_DATAFMT *datafmt = NULL;

    memset(&func, 0, sizeof(FUNC_STR));
    func.serverno = SERVER_DEFAULT;
    func.func = ISQL_COMMAND;
    sprintf(func.table, "%s", "xopensdb.低周减载装置参数表");
    sprintf(func.isql, "update xopensdb.低周减载装置参数表 set 关联轮次项数=%d where 编号=%d", nCount, deviceId);

    return dbfisqlcommand(&func);
}

QMap<int, QString> DBManager::getDeviceOptionsForArea(int areaId)
{
    QMap<int, QString> map;
    for (const auto &area : m_areaList)
    {
        if (areaId == area.id)
        {
            for (const auto &device : m_deviceList)
            {
                if (strcmp(area.staId, device.staId) == 0)
                {
                    map[device.id] = QString::fromLocal8Bit(device.name);
                }
            }
        }
    }

    return map;
}

dfJson::Value DBManager::getRoundItemJson()
{
    QVector<RoundItemDto> roundItemList = getRoundItemList(-1);

    QMap<int, QVector<RoundItemDto>> areaMap;
    for (const auto &item : roundItemList)
    {
        areaMap[item.areaId].push_back(item);
    }

    dfJson::Value root(dfJson::arrayValue); // 创建一个空的Json数组
    QMap<int, QVector<RoundItemDto>>::iterator it;
    for (it = areaMap.begin(); it != areaMap.end(); ++it)
    {
        dfJson::Value area(dfJson::objectValue);                          // 创建一个空的Json对象
        area["id"] = QString::number(it.key()).toLocal8Bit().data();      // 区域 ID
        area["name"] = m_subAreaIdNameMap[it.key()].toLocal8Bit().data(); // 区域名称
        area["type"] = "area";                                            // 区域类型

        dfJson::Value children(dfJson::arrayValue); // 创建一个空的Json数组

        for (const auto &item : it.value())
        {
            dfJson::Value child(dfJson::objectValue);                    // 创建一个空的Json对象
            child["id"] = QString::number(item.id).toLocal8Bit().data(); // 轮次项 ID
            child["name"] = item.name;                                   // 轮次项名称
            child["type"] = "roundItem";                                 // 轮次项类型
            children.append(child);
        }

        area["children"] = children;
        root.append(area);
    }

    return root;
}

QVector<TaskDto> DBManager::getTaskList()
{
    QString qstr = QString::fromLocal8Bit(
        "select 编号,名称,周期开始时间,周期结束时间,周期,分区条件,厂站条件,轮次条件 from xopensdb.低周减载周期巡检任务表");
    return getList<TaskDto>(qstr);
}

dfJson::Value DBManager::getAreaJson()
{
    // 构造 JSON 对象
    dfJson::Value root(dfJson::arrayValue);

    for (const auto &area : m_areaList)
    {
        if (area.type == 1)
            continue;
        dfJson::Value child(dfJson::objectValue);
        child["id"] = QString::number(area.id).toLocal8Bit().data();
        child["name"] = area.name;
        child["type"] = "area";
        root.append(child);
    }

    // qDebug() << QString::fromLocal8Bit(jsonToString(root).c_str());

    return root;
}

dfJson::Value DBManager::getRoundJson()
{
    // 构造 JSON 对象
    dfJson::Value root(dfJson::arrayValue);
    for (const auto &round : m_roundList)
    {
        dfJson::Value child(dfJson::objectValue);
        child["id"] = QString::number(round.id).toLocal8Bit().data();
        child["name"] = round.name;
        child["type"] = "round";
        root.append(child);
    }

    return root;
}

int DBManager::updateTaskTable(const TaskDto &task)
{
    return updateTable(task, "xopensdb.低周减载周期巡检任务表");
}

int DBManager::insertTaskTable(const TaskDto &task)
{
    return insertTable(task, "xopensdb.低周减载周期巡检任务表");
}

int DBManager::loadRdb(char *tableName)
{
    rtdb_proc rtdbproc;
    if (!rtdbproc.recall("ems", "", "", tableName))
    {
        DFLOG_DEBUG("加载实时库表失败: %s", tableName);
        return -1;
    }
    DFLOG_DEBUG("加载实时库表成功: %s", tableName);
    return 0;
}
