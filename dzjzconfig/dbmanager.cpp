#include "dbmanager.h"

#include <QMap>

#include "dto.h"

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
    m_roundIdNameMap = getRoundIdNameMap();
    m_areaIdNameMap = getAreaIdNameMap();

    m_lineList = getLineList();
    m_lineIdNameMap = getLineIdNameMap(m_lineList);

    m_breakerList = getBreakList();
    m_breakIdNameMap = getBreakerIdNameMap(m_breakerList);

    m_rtuList = getRtuList();
    m_rtuIdNameMap = getRtuIdNameMap(m_rtuList);
}

QVector<AreaDto> DBManager::getAreaList()
{
    QString query = QString::fromLocal8Bit("select 编号,名称,类型 from xopensdb.dbo.低周减载区域参数表");
    return getList<AreaDto>(query);
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
    QString query = QString::fromLocal8Bit("select 编号,名称,所属地区,功能类型,轮类型,频率或电压整定值,动作延时整定值,下发应切荷量 from xopensdb.dbo.低周减载轮次参数表");
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
    QString query = QString::fromLocal8Bit("select 编号,名称,所属分区,所属轮次,关联馈线,负荷类型,投退计划,关联开关 from xopensdb.dbo.低周减载轮次项参数表 where 所属轮次=%1")
                        .arg(roundId);
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
    int nOrder = 0;
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

QMap<int, QString> DBManager::getAreaIdNameMap()
{
    QVector<AreaDto> areas = getAreaList();
    QMap<int, QString> areaMap;
    for (const AreaDto &area : areas)
    {
        areaMap[area.id] = QString::fromLocal8Bit(area.name);
    }
    return areaMap;
}

QMap<int, QString> DBManager::getRoundIdNameMap()
{
    QVector<RoundDto> list = DBManager::getRoundList();
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

void DBManager::reloadRoundIdNameMap()
{
    m_roundIdNameMap = getRoundIdNameMap();
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
                        "select 所属装置,轮次编号,压板ID,频率或电压定值ID,动作延时定值ID,告警信号ID,动作信号ID"
                        " from xopensdb.低周减载装置参数设定表 where 所属装置=%1 order by 所属装置,轮次编号")
                        .arg(id);
    return getList<DeviceParaDto>(query);
}

QVector<FixValueDto> DBManager::getFixValueList(int rtuNo)
{
    QString query = QString::fromLocal8Bit(
                        "select 装置类型,组号,序号,描述 from xopensdb.低周减载装置定值信息表 where RTU号=%1"
                        " order by 装置类型,组号,序号")
                        .arg(rtuNo);
    return getList<FixValueDto>(query);
}

int DBManager::updateDeviceParaTable(const DeviceParaDto &devicePara)
{
    if (deleteTable(devicePara.id, "xopensdb.dbo.低周减载装置参数设定表", "所属装置") != CS_SUCCEED)
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
