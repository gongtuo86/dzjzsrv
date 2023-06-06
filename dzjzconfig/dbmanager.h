#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QMap>
#include <QObject>
#include <QVector>

#include "dto.h"

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
    static int updateTable(const T &recond, const char *tableName);

    template <typename T>
    static int insertTable(const T &record, const char *tableName);

    template <typename T>
    static QVector<T> getList(const QString &query);

    template <typename T>
    static QMap<int, QString> getMapFromQuery(const QString &query);

    template <typename T>
    static QMap<QString, QString> getStrMapFromQuery(const QString &query);

    static int deleteTable(int id, const char *tableName, const char *primaryKey = "编号");
    static int getMaxIDFromDataBase(const char *tableName);

    static QMap<int, QString> getAreaIdNameMap();
    static QMap<int, QString> getRoundIdNameMap();
    static QMap<QString, QString> getStaIdNameMap();
    static QVector<LineDto> getLineList();

    static QVector<BreakDto> getBreakList();
    static QMap<QString, QString> getBreakIdNameMap();

    static QVector<AreaDto> getAreaList();
    static QVector<AreaVo> getAreaVoList();
    static int updateAreaTable(const AreaVo &area);
    static int insertAreaTable(const AreaVo &area);

    static QVector<RoundDto> getRoundList();
    static int insertRoundTable(const RoundDto &round);
    static int updateRoundTable(const RoundDto &round);

    static QVector<RoundItemDto> getRoundItemList(int roundId);
    static int updateRoundItemTable(const RoundItemDto &item);
    static int insertRoundItemTable(const RoundItemDto &item);

    static QMap<int, QString> getRoundTypeMap();

    static QMap<int, QString> getDMConfig(const char *strType);

private:
    explicit DBManager(QObject *parent = 0) : QObject(parent) {}
    ~DBManager() {}
    DBManager(const DBManager &) = delete;
    DBManager &operator=(const DBManager &) = delete;

public:
    static QVector<BreakDto> m_breakList;           // 开关列表
    static QMap<QString, QString> m_breakIdNameMap; // 开关
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
    // DFLOG_DEBUG("recordlen=%d", func.recorderlen);
    // DFLOG_DEBUG("structlen=%d", sizeof(T));

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
    for (int i = 0; i < func.ret_roxnum; i++)
    {
        list.append(pBuf[i]);
    }

    if (pBuf != nullptr)
    {
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