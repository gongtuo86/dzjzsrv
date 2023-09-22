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
    QString m_system = QString::fromLocal8Bit("????");
    QString m_version = QString::fromLocal8Bit("1.0");
    QString m_type;
    QString m_code = QString::fromLocal8Bit("UTF-8");
    QString m_data = QString::fromLocal8Bit("1.0");
    QString m_datetime;

    QMap<int, QString> m_funcTypeMap;
    QMap<QString, QString> m_subMap;
    QMap<int, QString> m_loadTypeMap;
    QMap<int, QString> m_strapTypeMap;
    QMap<int, QString> m_deviceTypeMap;

    DZJZ_FileGenerator();
    DZJZ_FileGenerator(const DZJZ_FileGenerator &) = delete;
    void operator=(const DZJZ_FileGenerator &) = delete;

public:
    static DZJZ_FileGenerator &gentInstance();
    void SetPara(const QString &fileName, const QString &type, const QString &datetime);
    bool Generate(const QList<DZJZ_Section> &sections);
    QString getRoundBaseType(int roundType);
    int getRoundBaseTypeNum(int roundType);
    QMap<int, QString> getDMConfig(const QString &strType);
    QMap<QString, QString> getSubMap();

    bool DZJZ_GenRoundsFile(const QDateTime &qDateTime);
    bool DZJZ_GenDevicesFile(const QDateTime &qDateTime);
    bool DZJZ_GenLineFile(const QDateTime &qDateTime);
    bool DZJZ_GenActFile(const QDateTime &qDateTime, int nCycle);
};

#endif
