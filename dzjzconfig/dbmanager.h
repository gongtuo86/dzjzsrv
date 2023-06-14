#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QMap>
#include <QObject>
#include <QVector>

#include "dto.h"
#include "dfjson/json.h"
#include "dflogger.h"

class DBManager : public QObject
{
    Q_OBJECT
public:
    static DBManager *getInstance()
    {
        static DBManager instance;
        return &instance;
    }

    template <typename T>
    int updateTable(const T &recond, const char *tableName);

    template <typename T>
    int insertTable(const T &record, const char *tableName);

    template <typename T>
    QVector<T> getList(const QString &query);

    template <typename T>
    QMap<int, QString> getMapFromQuery(const QString &query);

    template <typename T>
    QMap<QString, QString> getStrMapFromQuery(const QString &query);

    int deleteTable(int id, const char *tableName, const char *primaryKey = "编号");
    int updateTable(const QString &sql);
    int getMaxIDFromDataBase(const char *tableName);


    QMap<QString, QString> getStaIdNameMap();

    QVector<BreakDto> getBreakList();
    QMap<QString, QString> getBreakIdNameMap();
    dfJson::Value getBreakerJson(const QVector<BreakDto> &breakers);

    QVector<AreaVo> getAreaVoList();
    QMap<int, QString> getAreaIdNameMap(const QVector<AreaVo> &areas, int type);
    int updateAreaTable(const AreaVo &area);
    int insertAreaTable(const AreaVo &area);
    void reloadAreaTable();

    QVector<RoundDto> getRoundList();
    int insertRoundTable(const RoundDto &round);
    int updateRoundTable(const RoundDto &round);
    QMap<int, QString> getRoundIdNameMap(const QVector<RoundDto> &list);
    void reloadRound();
    int deleteRoundTable(int id);

    QVector<RoundItemDto> getRoundItemList(int roundId);
    int updateRoundItemTable(const RoundItemDto &item);
    int insertRoundItemTable(const RoundItemDto &item);

    QMap<int, QString> getRoundTypeMap();

    QMap<int, QString> getDMConfig(const char *strType);
    QMap<QString, QString> getBreakerIdNameMap(const QVector<BreakDto> &breakers);

    QVector<LineDto> getLineList();
    QMap<QString, QString> getLineIdNameMap(const QVector<LineDto> &lines);
    dfJson::Value getLineJson(const QVector<LineDto> &lines);

    QVector<DeviceDto> getDeviceList();
    int updateDeviceTable(const DeviceDto &device);
    int insertRoundItemTable(const DeviceDto &device);
    QVector<RtuDto> getRtuList();
    QMap<int, QString> getRtuIdNameMap(const QVector<RtuDto> &rtus);
    dfJson::Value getRtuJson(const QVector<RtuDto> &rtus);
    QString getStaByRtuId(int rtu);
    QMap<int, QString> getDeviceIdNameMap(const QVector<DeviceDto> &list);
    dfJson::Value getDeviceJson(const QVector<DeviceDto> &devices);
    void reloadDevice();
    int deleteDeviceTable(int id);

    QVector<DeviceParaDto> getDeviceParaList(int id);
    int updateDeviceParaTable(const DeviceParaDto &devicePara);
    int insertDeviceParaTable(const DeviceParaDto &devicePara);
    int updateRoundItemCount(int deviceId, int nCount);
    QMap<int, QString> getDeviceOptionsForArea(int areaId);
    int deleteDeviceParaTable(int deviceId, int roundId);

    QVector<TMDto> getTMList();
    QMap<QString, QString> getTMNameMap(const QVector<TMDto> &list);
    dfJson::Value getTMJson(const QVector<TMDto> &list);

    QVector<TSDto> getTSList();
    QMap<QString, QString> getTSNameMap(const QVector<TSDto> &list);
    dfJson::Value getTSJson(const QVector<TSDto> &list);

    QVector<FixValueDto> getFixValueList();
    dfJson::Value getFixValueJson(int id);
    QMap<QString, QString> getFixValuMap(const QVector<FixValueDto> &list);
    void reloadFixValue();

private:
    explicit DBManager(QObject *parent = 0);
    ~DBManager(){};
    DBManager(const DBManager &) = delete;
    DBManager &operator=(const DBManager &) = delete;

public:
    QMap<int, QString> m_areaTypeMap;       // 区域类型
    QMap<int, QString> m_roundFuncTypeMap;  // 轮次功能类型
    QMap<int, QString> m_roundTypeMap;      // 轮次类型
    QMap<int, QString> m_loadTypeMap;       // 负荷类型
    QMap<int, QString> m_strapMap;          // 投退
    QMap<int, QString> m_deviceTypeMap;     // 装置类型
    QMap<int, QString> m_deviceFuncTypeMap; // 装置功能类型

    QVector<AreaVo> m_areaList;             //
    QMap<int, QString> m_allAreaIdNameMap;  // 区域
    QMap<int, QString> m_subAreaIdNameMap;  // 变电站区域

    QMap<QString, QString> m_staIdNameMap;  // 变电站

    QVector<LineDto> m_lineList;            // 线路列表
    QMap<QString, QString> m_lineIdNameMap; // 馈线
    dfJson::Value m_lineJson;

    QVector<BreakDto> m_breakerList;         // 开关列表
    QMap<QString, QString> m_breakIdNameMap; // 开关
    dfJson::Value m_breakerJson;

    QVector<RoundDto> m_roundList;       // 轮次
    QMap<int, QString> m_roundIdNameMap; // 轮次map

    QVector<RtuDto> m_rtuList;           // Rtu
    QMap<int, QString> m_rtuIdNameMap;   // RTU map
    dfJson::Value m_rtuJson;

    QVector<DeviceDto> m_deviceList;
    QMap<int, QString> m_deviceIdNameMap; // 装置map
    dfJson::Value m_deviceJson;

    QVector<TMDto> m_tmList;
    QMap<QString, QString> m_tmIdNameMap;
    dfJson::Value m_tmJson;

    QVector<TSDto> m_tsList;
    QMap<QString, QString> m_tsIdNameMap;
    dfJson::Value m_tsJson;

    QVector<FixValueDto> m_fixValueList;
    QMap<QString, QString> m_fixValueMap;
};

template <typename T>
int DBManager::updateTable(const T &data, const char *tableName)
{
    if (deleteTable(data.id, tableName) != CS_SUCCEED)
        return CS_FAIL;

    return insertTable(data, tableName);
}
/**
 * @brief 保存表数据
 *
 * @tparam T
 * @param record
 * @param tableName
 */
template <typename T>
int DBManager::insertTable(const T &record, const char *tableName)
{
    FUNC_STR func;
    memset(&func, 0, sizeof(FUNC_STR));
    CS_DATAFMT *datafmt = nullptr;
    func.func = GET_TABLE_INFO;
    func.serverno = SERVER_DEFAULT;
    sprintf(func.table, "%s", tableName);
    int ret = dbfgettableinfo(&func, &datafmt);
    DFLOG_DEBUG("recordlen=%d", func.recorderlen);
    DFLOG_DEBUG("structlen=%d", sizeof(T));

    if (ret == CS_SUCCEED)
    {
        func.func = INSERT_RECORDERS;
        func.roxnum = 1;
        ret = dbfinsertrecorders(&func, datafmt, (void *)&record);
        // DFLOG_DEBUG("nRet=%d", nRet);
    }

    if (datafmt != nullptr)
    {
        free(datafmt);
        datafmt = nullptr;
    }

    return ret;
}

template <typename T>
QVector<T> DBManager::getList(const QString &query)
{
    QVector<T> list;
    FUNC_STR func;
    T *pBuf = nullptr;
    memset(&func, 0, sizeof(FUNC_STR));
    func.func = SELECT_ISQL_RESULTS;
    func.serverno = SERVER_DEFAULT;
    snprintf(func.isql, sizeof(func.isql) - 1, "%s", query.toLocal8Bit().data());
    dbfselectisqlresults(&func, nullptr, (void **)&pBuf);
    list.reserve(func.ret_roxnum);
    // DFLOG_DEBUG("sizeof T = %d", sizeof(T));
    for (int i = 0; i < func.ret_roxnum; i++)
    {
        if ((pBuf + i) != NULL)
            list.push_back(pBuf[i]);
    }

    if (pBuf != nullptr)
    {
        // DFLOG_DEBUG("sizeof pBuf = %d", sizeof(pBuf[0]));
        free(pBuf);
        pBuf = nullptr;
    }

    return list;
}

template <typename T>
QMap<int, QString> DBManager::getMapFromQuery(const QString &query)
{
    QMap<int, QString> map;
    FUNC_STR func;
    T *pBuf = nullptr;
    memset(&func, 0, sizeof(FUNC_STR));
    func.func = SELECT_ISQL_RESULTS;
    func.serverno = SERVER_DEFAULT;
    snprintf(func.isql, sizeof(func.isql) - 1, "%s", query.toLocal8Bit().data());
    dbfselectisqlresults(&func, nullptr, (void **)&pBuf);
    for (int i = 0; i < func.ret_roxnum; i++)
    {
        map[pBuf[i].id] = QString::fromLocal8Bit(pBuf[i].name);
    }

    if (pBuf != nullptr)
    {
        free(pBuf);
        pBuf = nullptr;
    }

    return map;
}

template <typename T>
QMap<QString, QString> DBManager::getStrMapFromQuery(const QString &query)
{
    QMap<QString, QString> map;
    FUNC_STR func;
    T *pBuf = nullptr;
    memset(&func, 0, sizeof(FUNC_STR));
    func.func = SELECT_ISQL_RESULTS;
    func.serverno = SERVER_DEFAULT;
    snprintf(func.isql, sizeof(func.isql) - 1, "%s", query.toLocal8Bit().data());
    dbfselectisqlresults(&func, nullptr, (void **)&pBuf);
    for (int i = 0; i < func.ret_roxnum; i++)
    {
        map[pBuf[i].id] = QString::fromLocal8Bit(pBuf[i].name);
    }

    if (pBuf != nullptr)
    {
        free(pBuf);
        pBuf = nullptr;
    }

    return map;
}
#endif