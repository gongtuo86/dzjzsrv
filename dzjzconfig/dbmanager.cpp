#include "dbmanager.h"

#include <QMap>
#include <QDebug>

#include "netsub.h"
#include "dto.h"
#include "dzjzconfigutil.h"

DBManager::DBManager(QObject *parent) : QObject(parent)
{
    m_areaTypeMap = getDMConfig("���ܼ�����������");
    m_roundFuncTypeMap = getDMConfig("���ܼ����ִι�������");
    m_loadTypeMap = getDMConfig("���ܼ��ظ�������");
    m_strapMap = getDMConfig("���ܼ���Ͷ�˼ƻ�����");
    m_deviceTypeMap = getDMConfig("���ܼ���װ������");
    m_deviceFuncTypeMap = getDMConfig("���ܼ���װ�ù�������");
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
        "select ���,����,����,��վ��� from xopensdb.���ܼ������������ a"
        " left join xopensdb.���ܼ�������վ������ b on a.���=b.�������� order by ���");
    return getList<AreaVo>(query);
}

QVector<RoundDto> DBManager::getRoundList()
{
    QString query = QString::fromLocal8Bit("select ���,����,��������,��������,������,Ƶ�ʻ��ѹ����ֵ,������ʱ����ֵ,�·�Ӧ�к���,�ƻ��кɱ��� from xopensdb.dbo.���ܼ����ִβ�����");
    return getList<RoundDto>(query);
}

/**
 * @brief �ִ�����
 *
 * @return QVector<RoundDto>
 */
QMap<int, QString> DBManager::getRoundTypeMap()
{
    QString query = QString::fromLocal8Bit("select ���,���� from xopensdb.dbo.���ܼ����ִ����ͱ�");
    return getMapFromQuery<RoundTypeDto>(query);
}

QMap<int, QString> DBManager::getDMConfig(const char *strType)
{
    QString query = QString::fromLocal8Bit("select ��ֵ,��Ŀ from xopensdb.dbo.DM���ñ� where ��������='%1'")
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
    if (updateTable(areaDto, "xopensdb.���ܼ������������") != CS_SUCCEED)
    {
        return CS_FAIL;
    }

    AreaSubStationDto areaSubStationDto;
    areaSubStationDto.areaId = area.id;
    strcpy(areaSubStationDto.staId, area.staId);
    if (deleteTable(area.id, "xopensdb.���ܼ�������վ������", "��������") != CS_SUCCEED)
    {
        return CS_FAIL;
    }

    return insertTable(areaSubStationDto, "xopensdb.���ܼ�������վ������");
}

int DBManager::insertAreaTable(const AreaVo &area)
{
    AreaDto areaDto;
    areaDto.id = area.id;
    areaDto.type = area.type;
    strcpy(areaDto.name, area.name);
    if (insertTable(areaDto, "xopensdb.���ܼ������������") != CS_SUCCEED)
    {
        return CS_FAIL;
    }

    AreaSubStationDto areaSubStationDto;
    areaSubStationDto.areaId = area.id;
    strcpy(areaSubStationDto.staId, area.staId);

    return insertTable(areaSubStationDto, "xopensdb.���ܼ�������վ������");
}

void DBManager::reloadAreaTable()
{
    m_areaList = getAreaVoList();
    m_allAreaIdNameMap = getAreaIdNameMap(m_areaList, 1);
    m_subAreaIdNameMap = getAreaIdNameMap(m_areaList, 2);
}

int DBManager::insertRoundTable(const RoundDto &round)
{
    return insertTable(round, "xopensdb.dbo.���ܼ����ִβ�����");
}

int DBManager::updateRoundTable(const RoundDto &round)
{
    return updateTable(round, "xopensdb.dbo.���ܼ����ִβ�����");
}

QVector<RoundItemDto> DBManager::getRoundItemList(int roundId)
{
    QString query = QString::fromLocal8Bit(
        "select ���,����,��������,�����ִ�,��������,��������,Ͷ�˼ƻ�,��������,�й�����,����װ��,��������"
        " from xopensdb.���ܼ����ִ�������� where 1=1");

    if (roundId > 0)
    {
        query += QString::fromLocal8Bit(" and �����ִ�=%1").arg(roundId);
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
    sprintf(func.isql, "select max(���) from %s", tableName);

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
    QString query = QString::fromLocal8Bit("select ���,���� from xopensdb.dbo.��վ������");
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
        "select a.���, a.����, a.��·ʼ�˳�վ��, b.���� from ��·������"
        " a, ��վ������ b where a.��·ʼ�˳�վ�� = b.��� ");

    return getList<LineDto>(query);
}

QVector<BreakDto> DBManager::getBreakList()
{
    QString query = QString::fromLocal8Bit(
        "select a.����,a.����, a.��վ����, b.���� from ң�Ų����� a, ��վ������ b "
        "where a.��վ����=b.��� and a.����>=300 and a.����<400");

    return getList<BreakDto>(query);
}

QMap<QString, QString> DBManager::getBreakIdNameMap()
{
    QString query = QString::fromLocal8Bit("select ����,���� from xopensdb.dbo.ң�Ų�����");
    return getStrMapFromQuery<BreakDto>(query);
}

int DBManager::updateRoundItemTable(const RoundItemDto &item)
{
    return updateTable(item, "xopensdb.dbo.���ܼ����ִ��������");
}

int DBManager::insertRoundItemTable(const RoundItemDto &item)
{
    return insertTable(item, "xopensdb.dbo.���ܼ����ִ��������");
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
    QString sql = QString::fromLocal8Bit("update xopensdb.���ܼ����ִ�������� set �����ִ�=0 where �����ִ�=%1").arg(id);

    if (updateTable(sql) != CS_SUCCEED)
        return CS_FAIL;

    if (deleteTable(id, "xopensdb.dbo.���ܼ����ִβ�����") != CS_SUCCEED)
        return CS_FAIL;

    return CS_SUCCEED;
}

QVector<DeviceDto> DBManager::getDeviceList()
{
    QString query = QString::fromLocal8Bit("select ���,����,װ������,��������,�����ִ�����,������վ from xopensdb.dbo.���ܼ���װ�ò�����");
    return getList<DeviceDto>(query);
}

int DBManager::updateDeviceTable(const DeviceDto &device)
{
    return updateTable(device, "xopensdb.dbo.���ܼ���װ�ò�����");
}

int DBManager::insertRoundItemTable(const DeviceDto &device)
{
    return insertTable(device, "xopensdb.dbo.���ܼ���װ�ò�����");
}

QVector<RtuDto> DBManager::getRtuList()
{
    QString query = QString::fromLocal8Bit(
        "select a.���,a.����,a.��վ����,b.���� from xopensdb.RTU������ a, ��վ������ b"
        " where a.��վ����=b.��� and a.RTU����=0 order by a.���");
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
                        "select ����װ��,�ִα��,ѹ��ID,Ƶ�ʻ��ѹ��ֵID,������ʱ��ֵID,�澯�ź�ID,�����ź�ID,���ھ���ID"
                        " from xopensdb.���ܼ���װ�ò����趨�� where ����װ��=%1 order by ����װ��,�ִα��")
                        .arg(id);
    return getList<DeviceParaDto>(query);
}

QVector<FixValueDto> DBManager::getFixValueList()
{
    QString query = QString::fromLocal8Bit(
        "select RTU��,װ������,���,���,���� from xopensdb.���ܼ���װ�ö�ֵ��Ϣ�� order by װ������,���,���");
    return getList<FixValueDto>(query);
}

int DBManager::deleteDeviceParaTable(int deviceId, int roundId)
{
    FUNC_STR func;
    CS_DATAFMT *datafmt = NULL;

    memset(&func, 0, sizeof(FUNC_STR));
    func.serverno = SERVER_DEFAULT;
    func.func = ISQL_COMMAND;
    sprintf(func.table, "%s", "xopensdb.���ܼ���װ�ò����趨��");
    sprintf(func.isql, "delete from xopensdb.���ܼ���װ�ò����趨�� where ����װ��=%d and �ִα��=%d", deviceId, roundId);
    return dbfisqlcommand(&func);
}

int DBManager::updateDeviceParaTable(const DeviceParaDto &devicePara)
{
    if (deleteDeviceParaTable(devicePara.id, devicePara.no) != CS_SUCCEED)
        return CS_FAIL;

    return insertTable(devicePara, "xopensdb.dbo.���ܼ���װ�ò����趨��");
}

int DBManager::insertDeviceParaTable(const DeviceParaDto &devicePara)
{
    return insertTable(devicePara, "xopensdb.dbo.���ܼ���װ�ò����趨��");
}

int DBManager::deleteDeviceTable(int id)
{
    if (deleteTable(id, "xopensdb.dbo.���ܼ���װ�ò�����") != CS_SUCCEED)
        return CS_FAIL;

    if (deleteTable(id, "xopensdb.dbo.���ܼ���װ�ò����趨��", "����װ��") != CS_SUCCEED)
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
        "select a.����,a.����, a.��վ����,b.���� from ң������� a, ��վ������ b "
        "where a.��վ����=b.��� and (a.���� like '%%�й�%%' or a.���� like '%%P%%' or a.���� like '%%p%%') order by a.����");

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

    // ��������·�����վ ID ����
    for (const auto &tm : list)
    {
        stationMap[QString::fromLocal8Bit(tm.staId)].push_back(tm);
    }

    // ���� JSON ����
    dfJson::Value root(dfJson::arrayValue); // ����һ���յ�Json����

    QMap<QString, QVector<TMDto>>::iterator it;
    for (it = stationMap.begin(); it != stationMap.end(); ++it)
    {
        // ���վ
        dfJson::Value station(dfJson::objectValue);    // ����һ���յ�Json����
        station["id"] = it.key().toLocal8Bit().data(); // ���վ ID
        station["name"] = it.value().first().staName;  // ���վ����
        station["type"] = "substation";                // ���վ����

        dfJson::Value children(dfJson::arrayValue); // ����һ���յ�Json����

        // ����
        for (const auto &tm : it.value())
        {
            dfJson::Value child(dfJson::objectValue); // ����һ���յ�Json����
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
        "select a.����,a.����,a.��վ����,b.����,a.RTU��,c.���� from ң�Ų����� a,"
        "��վ������ b, RTU������ c where a.��վ����=b.��� and a.RTU��=c.��� order by a.RTU��, a.YX��");

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

    // ��������·�����վ ID ����
    for (const auto &rtu : rtus)
    {
        stationMap[QString::fromLocal8Bit(rtu.staId)].push_back(rtu);
    }

    // ���� JSON ����
    dfJson::Value root(dfJson::arrayValue); // ����һ���յ�Json����

    QMap<QString, QVector<RtuDto>>::iterator it;
    for (it = stationMap.begin(); it != stationMap.end(); ++it)
    {
        // ���վ
        dfJson::Value station(dfJson::objectValue);    // ����һ���յ�Json����
        station["id"] = it.key().toLocal8Bit().data(); // ���վ ID
        station["name"] = it.value().first().staName;  // ���վ����
        station["type"] = "substation";                // ���վ����

        dfJson::Value children(dfJson::arrayValue); // ����һ���յ�Json����

        // ��·
        for (const auto &rtu : it.value())
        {
            dfJson::Value child(dfJson::objectValue);                   // ����һ���յ�Json����
            child["id"] = QString::number(rtu.id).toLocal8Bit().data(); // ��· ID
            child["name"] = rtu.name;                                   // ��·����
            child["type"] = "rtu";                                      // ��·����
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
    // ���� JSON ����
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

    // ��������·�����վ ID ����
    for (const auto &breaker : breakers)
    {
        // qDebug("line.staName=%s line.name=%s", line.staName, line.name);
        stationMap[QString::fromLocal8Bit(breaker.staId)].push_back(breaker);
    }

    // ���� JSON ����
    dfJson::Value root(dfJson::arrayValue); // ����һ���յ�Json����

    QMap<QString, QVector<BreakDto>>::iterator it;
    for (it = stationMap.begin(); it != stationMap.end(); ++it)
    {
        // ���վ
        dfJson::Value station(dfJson::objectValue);    // ����һ���յ�Json����
        station["id"] = it.key().toLocal8Bit().data(); // ���վ ID
        station["name"] = it.value().first().staName;  // ���վ����
        station["type"] = "substation";                // ���վ����

        dfJson::Value children(dfJson::arrayValue); // ����һ���յ�Json����

        // ����
        for (const auto &breaker : it.value())
        {
            dfJson::Value child(dfJson::objectValue); // ����һ���յ�Json����
            child["id"] = breaker.id;                 // ����ID
            child["name"] = breaker.name;             // ��������
            child["type"] = "breaker";                // ��������
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

    // ��������·�����վ ID ����
    for (const auto &device : devices)
    {
        stationMap[QString::fromLocal8Bit(device.staId)].push_back(device);
    }

    // ���� JSON ����
    dfJson::Value root(dfJson::arrayValue); // ����һ���յ�Json����

    QMap<QString, QVector<DeviceDto>>::iterator it;
    for (it = stationMap.begin(); it != stationMap.end(); ++it)
    {
        // ���վ
        dfJson::Value station(dfJson::objectValue);                                      // ����һ���յ�Json����
        station["id"] = it.key().toLocal8Bit().data();                                   // ���վ ID
        station["name"] = m_staIdNameMap[it.value().first().staId].toLocal8Bit().data(); // ���վ����
        station["type"] = "substation";                                                  // ���վ����

        dfJson::Value children(dfJson::arrayValue); // ����һ���յ�Json����

        // ��·
        for (const auto &device : it.value())
        {
            dfJson::Value child(dfJson::objectValue); // ����һ���յ�Json����
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

    // ��������·�����վ ID ����
    for (const auto &line : lines)
    {
        // qDebug("line.staName=%s line.name=%s", line.staName, line.name);
        stationMap[QString::fromLocal8Bit(line.staId)].push_back(line);
    }

    // ���� JSON ����
    dfJson::Value root(dfJson::arrayValue); // ����һ���յ�Json����

    QMap<QString, QVector<LineDto>>::iterator it;
    for (it = stationMap.begin(); it != stationMap.end(); ++it)
    {
        // ���վ
        dfJson::Value station(dfJson::objectValue);    // ����һ���յ�Json����
        station["id"] = it.key().toLocal8Bit().data(); // ���վ ID
        station["name"] = it.value().first().staName;  // ���վ����
        station["type"] = "substation";                // ���վ����

        dfJson::Value children(dfJson::arrayValue); // ����һ���յ�Json����

        // ��·
        for (const auto &line : it.value())
        {
            dfJson::Value child(dfJson::objectValue); // ����һ���յ�Json����
            child["id"] = line.id;                    // ��· ID
            child["name"] = line.name;                // ��·����
            child["type"] = "line";                   // ��·����
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
    sprintf(func.table, "%s", "xopensdb.���ܼ���װ�ò�����");
    sprintf(func.isql, "update xopensdb.���ܼ���װ�ò����� set �����ִ�����=%d where ���=%d", nCount, deviceId);

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

    dfJson::Value root(dfJson::arrayValue); // ����һ���յ�Json����
    QMap<int, QVector<RoundItemDto>>::iterator it;
    for (it = areaMap.begin(); it != areaMap.end(); ++it)
    {
        dfJson::Value area(dfJson::objectValue);                          // ����һ���յ�Json����
        area["id"] = QString::number(it.key()).toLocal8Bit().data();      // ���� ID
        area["name"] = m_subAreaIdNameMap[it.key()].toLocal8Bit().data(); // ��������
        area["type"] = "area";                                            // ��������

        dfJson::Value children(dfJson::arrayValue); // ����һ���յ�Json����

        for (const auto &item : it.value())
        {
            dfJson::Value child(dfJson::objectValue);                    // ����һ���յ�Json����
            child["id"] = QString::number(item.id).toLocal8Bit().data(); // �ִ��� ID
            child["name"] = item.name;                                   // �ִ�������
            child["type"] = "roundItem";                                 // �ִ�������
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
        "select ���,����,���ڿ�ʼʱ��,���ڽ���ʱ��,����,��������,��վ����,�ִ����� from xopensdb.���ܼ�������Ѳ�������");
    return getList<TaskDto>(qstr);
}

dfJson::Value DBManager::getAreaJson()
{
    // ���� JSON ����
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
    // ���� JSON ����
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
    return updateTable(task, "xopensdb.���ܼ�������Ѳ�������");
}

int DBManager::insertTaskTable(const TaskDto &task)
{
    return insertTable(task, "xopensdb.���ܼ�������Ѳ�������");
}

int DBManager::loadRdb(char *tableName)
{
    rtdb_proc rtdbproc;
    if (!rtdbproc.recall("ems", "", "", tableName))
    {
        DFLOG_DEBUG("����ʵʱ���ʧ��: %s", tableName);
        return -1;
    }
    DFLOG_DEBUG("����ʵʱ���ɹ�: %s", tableName);
    return 0;
}
