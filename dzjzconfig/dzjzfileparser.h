#ifndef DZJZFILEPARSER_H
#define DZJZFILEPARSER_H

#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>

#include "dfjson/json.h"

class DZJZ_FileParser
{
public:
    DZJZ_FileParser(const QString &filePath);

    dfJson::Value parse();
    QString toJsonString() const;

private:
    QString m_filePath;
    dfJson::Value m_root;

    bool parseBlock(QTextStream &stream, const QString &blockName);
    dfJson::Value parseData(const QString &line, const QStringList &columnNames);
    QStringList parseColumnNames(const QString &line);
};

#endif // DZJZFILEPARSER_H
