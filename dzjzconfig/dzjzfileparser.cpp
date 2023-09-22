#include "dzjzfileparser.h"
#include "dflogger.h"

#include <QTextCodec>
#include <QDebug>

DZJZ_FileParser::DZJZ_FileParser(const QString &filePath) : m_filePath(filePath) {}

/**
 * @brief 解析文件
 *
 * @return true
 * @return false
 */
dfJson::Value DZJZ_FileParser::parse()
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return dfJson::Value();

    QTextStream in(&file);
    in.setCodec(QTextCodec::codecForName("UTF-8"));
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();

        // 忽略以 <! 开头和以 !> 结尾的行
        if (line.startsWith("<!") && line.endsWith("!>"))
            continue;

        if (line.startsWith("<") && line.endsWith(">") && !line.startsWith("</"))
        {
            QString blockName = line.mid(1, line.length() - 2);
            blockName.replace("::地区电网", "");
            parseBlock(in, blockName);
        }
    }

    file.close();

    qDebug() << "m_root=" << toJsonString();
    return m_root;
}

/**
 * @brief 解析块
 *
 * @param stream
 * @return true
 * @return false
 */
bool DZJZ_FileParser::parseBlock(QTextStream &stream, const QString &blockName)
{
    QString line;
    dfJson::Value blockData;
    dfJson::Value blockRows;
    QStringList columnNames;

    while (!stream.atEnd())
    {
        line = stream.readLine().trimmed();
        if (line.startsWith("@"))
        {
            // 解析列名
            columnNames = parseColumnNames(line);
            // for (const QString &columnName : columnNames)
            // {
            //     blockData["columns"].append(columnName.toLocal8Bit().data());
            // }
        }
        else if (line.startsWith("//"))
        {
            // 解析注释
        }
        else if (line.startsWith("#"))
        {
            // 解析数据
            blockRows.append(parseData(line, columnNames));
        }
        else if (line.startsWith("</"))
        {
            // blockData["data"] = blockRows;
            m_root[blockName.toLocal8Bit().data()] = blockRows;
            return true;
        }
    }
    return false;
}

/**
 * @brief 解析列名
 *
 * @param line
 * @return QStringList
 */
QStringList DZJZ_FileParser::parseColumnNames(const QString &line)
{
    return line.mid(1).split(QRegExp("\\s+"), QString::SkipEmptyParts);
}

/**
 * @brief 将m_root转换为json字符串
 *
 * @return QString
 */
QString DZJZ_FileParser::toJsonString() const
{
    dfJson::StreamWriterBuilder writerBuilder;
    // writerBuilder.settings_["indentation"] = ""; // 用于格式化输出的空格数，这里我们不需要缩进
    writerBuilder.settings_["commentStyle"] = "None";
    writerBuilder.settings_["emitUTF8"] = true; // 输出utf8

    std::string jsonString = dfJson::writeString(writerBuilder, m_root);
    return QString::fromStdString(jsonString);
}

/**
 * @brief 解析数据
 *
 * @param line
 * @return dfJson::Value
 */
dfJson::Value DZJZ_FileParser::parseData(const QString &line, const QStringList &columnNames)
{
    QStringList dataValues = line.mid(1).split(QRegExp("\\s+"), QString::SkipEmptyParts);
    dfJson::Value dataRow;
    for (int i = 0; i < columnNames.size() && i < dataValues.size(); ++i)
    {
        dataRow[columnNames[i].toLocal8Bit().data()] = dataValues[i].toLocal8Bit().data();
    }
    return dataRow;
}
