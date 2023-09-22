#ifndef DZJZ_FILE_GENERATOR_H
#define DZJZ_FILE_GENERATOR_H

#include <iostream>
#include <fstream>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QDateTime>

#include "dfjson/json.h"
#include "dorbase.h"
#include "dzjzsrv.h"
#include "dzjzcommon.h"

class DZJZ_Section
{
private:
    QString m_sectionName;
    QStringList m_headerList;
    QStringList m_descriptionList;
    dfJson::Value m_entries;

public:
    DZJZ_Section(const QString &sectionName, const QStringList &headerList,
                 const QStringList &descriptionList, const dfJson::Value &entries)
        : m_sectionName(sectionName), m_headerList(headerList), m_descriptionList(descriptionList), m_entries(entries) {}

    void Write(QTextStream &out) const;
};

class DZJZ_FileGenerator
{
private:
    QString m_fileName;
    QString m_system = QString::fromLocal8Bit("地区电网");
    QString m_version = QString::fromLocal8Bit("1.0");
    QString m_type;
    QString m_code = QString::fromLocal8Bit("UTF-8");
    QString m_data = QString::fromLocal8Bit("1.0");
    QString m_datetime;

    static const QMap<int, QString> m_funcTypeMap;
    static const QMap<QString, QString> m_subMap;
    static const QMap<int, QString> m_loadTypeMap;
    static const QMap<int, QString> m_strapTypeMap;
    static const QMap<int, QString> m_deviceTypeMap;

public:
    DZJZ_FileGenerator(const QString &fileName,
                       const QString &type,
                       const QString &datetime);

    bool Generate(const QList<DZJZ_Section> &sections);

    /**
     * @brief Get the Round Base Type object
     *
     * @param roundType
     * @return QString
     */
    static QString getRoundBaseType(int roundType);

    static int getRoundBaseTypeNum(int roundType);

    static bool DZJZ_GenRoundsFile(const QDateTime &qDateTime);

    static bool DZJZ_GenDevicesFile(const QDateTime &qDateTime);

    static bool DZJZ_GenLineFile(const QDateTime &qDateTime);

    static bool DZJZ_GenActFile(const QDateTime &qDateTime, int nCycle);
};

QMap<int, QString> getDMConfig(const QString &strType)
{
    typedef struct
    {
        int id;
        char name[40];
    } DMDto;

    QString query = QString::fromLocal8Bit("select 数值,项目 from xopensdb.dbo.DM配置表 where 配置类型='%1'")
                        .arg(strType);

    QMap<int, QString> map;
    FUNC_STR func;
    DMDto *pBuf = nullptr;
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

QMap<QString, QString> getSubMap()
{
    typedef struct
    {
        char name[SUBSTATION_LEN];
        char desc[DESCRIBE_LEN];
    } SubDto;

    QString query = QString::fromLocal8Bit("select 编号,名称 from xopensdb.dbo.厂站参数表");

    QMap<QString, QString> map;
    FUNC_STR func;
    SubDto *pBuf = nullptr;
    memset(&func, 0, sizeof(FUNC_STR));
    func.func = SELECT_ISQL_RESULTS;
    func.serverno = SERVER_DEFAULT;
    snprintf(func.isql, sizeof(func.isql) - 1, "%s", query.toLocal8Bit().data());
    dbfselectisqlresults(&func, nullptr, (void **)&pBuf);
    for (int i = 0; i < func.ret_roxnum; i++)
    {
        map[pBuf[i].name] = QString::fromLocal8Bit(pBuf[i].desc);
    }

    if (pBuf != nullptr)
    {
        free(pBuf);
        pBuf = nullptr;
    }

    return map;
}


#endif
