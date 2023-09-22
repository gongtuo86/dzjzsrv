#include "dzjzfileparser.h"
#include "dflogger.h"

#include <QTextCodec>
#include <QDebug>

DZJZ_FileParser::DZJZ_FileParser(const QString &filePath) : m_filePath(filePath) {}

/**
 * @brief �����ļ�
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

        // ������ <! ��ͷ���� !> ��β����
        if (line.startsWith("<!") && line.endsWith("!>"))
            continue;

        if (line.startsWith("<") && line.endsWith(">") && !line.startsWith("</"))
        {
            QString blockName = line.mid(1, line.length() - 2);
            blockName.replace("::��������", "");
            parseBlock(in, blockName);
        }
    }

    file.close();

    qDebug() << "m_root=" << toJsonString();
    return m_root;
}

/**
 * @brief ������
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
            // ��������
            columnNames = parseColumnNames(line);
            // for (const QString &columnName : columnNames)
            // {
            //     blockData["columns"].append(columnName.toLocal8Bit().data());
            // }
        }
        else if (line.startsWith("//"))
        {
            // ����ע��
        }
        else if (line.startsWith("#"))
        {
            // ��������
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
 * @brief ��������
 *
 * @param line
 * @return QStringList
 */
QStringList DZJZ_FileParser::parseColumnNames(const QString &line)
{
    return line.mid(1).split(QRegExp("\\s+"), QString::SkipEmptyParts);
}

/**
 * @brief ��m_rootת��Ϊjson�ַ���
 *
 * @return QString
 */
QString DZJZ_FileParser::toJsonString() const
{
    dfJson::StreamWriterBuilder writerBuilder;
    // writerBuilder.settings_["indentation"] = ""; // ���ڸ�ʽ������Ŀո������������ǲ���Ҫ����
    writerBuilder.settings_["commentStyle"] = "None";
    writerBuilder.settings_["emitUTF8"] = true; // ���utf8

    std::string jsonString = dfJson::writeString(writerBuilder, m_root);
    return QString::fromStdString(jsonString);
}

/**
 * @brief ��������
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
