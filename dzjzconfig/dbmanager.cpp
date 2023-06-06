#include "dbmanager.h"

#include <QMap>

#include "dto.h"

QVector<AreaDto> DBManager::getAreaList()
{
    QString query = QString::fromLocal8Bit("select ���,����,���� from xopensdb.dbo.���ܼ������������");
    return getList<AreaDto>(query);
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
    QString query = QString::fromLocal8Bit("select ���,����,��������,��������,������,Ƶ�ʻ��ѹ����ֵ,������ʱ����ֵ,�·�Ӧ�к��� from xopensdb.dbo.���ܼ����ִβ�����");
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
    QString query = QString::fromLocal8Bit("select ���,����,��������,�����ִ�,��������,��������,Ͷ�˼ƻ�,�������� from xopensdb.dbo.���ܼ����ִ�������� where �����ִ�=%1")
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

QMap<int, QString> DBManager::getAreaIdNameMap()
{
    QVector<AreaDto> areaList = DBManager::getAreaList();
    QMap<int, QString> areaMap;
    for (const AreaDto &area : areaList)
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
    QString query = QString::fromLocal8Bit("select ���,���� from xopensdb.dbo.��վ������");
    return getStrMapFromQuery<SubstationDto>(query);
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
