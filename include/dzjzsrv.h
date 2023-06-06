#ifndef DZJZSRV_H
#define DZJZSRV_H

#include "dfjson/json.h"
#include "dorbase.h"
#include "sybase.h"

#ifndef MyUserName
#define MyUserName "scada"
#endif
#ifndef MyPassWord
#define MyPassWord "df_scada"
#endif

/**
 * @brief 读取表数据
 *
 * @tparam T1
 * @tparam T2
 * @param tableName
 * @param time
 * @return std::vector<T2>
 */
template <typename T1, typename T2>
void readTable(char *tableName, const intertime time, std::vector<T2> &recordList)
{
    T1 *pRecord = nullptr;
    recordList.clear();

    RdbTable table(MyUserName, MyPassWord);
    if (table.OpenTable(tableName) == RDB_OK)
    {
        table.LockTableStruct();

        int recordCount = table.GetRecordCount();

        recordList.reserve(recordCount);

        for (int i = 0; i < recordCount; i++)
        {
            pRecord = (T1 *)table.GetRecordAddr(i);
            if (pRecord == nullptr)
                continue;

            recordList.emplace_back(time, *pRecord);
        }

        table.UnlockTable();
    }

    return;
}

/**
 * @brief 保存表数据
 *
 * @tparam T
 * @param record
 * @param tableName
 */
template <typename T>
void saveTable(T &record, const char *tableName)
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
        int nRet = dbfinsertrecorders(&func, datafmt, (void *)&record);
        // DFLOG_DEBUG("nRet=%d", nRet);
    }

    if (datafmt != nullptr)
    {
        free(datafmt);
        datafmt = nullptr;
    }
}

/**
 * @brief 保存表数据
 *
 * @tparam T
 * @param recordList
 * @param tableName
 */
template <typename T>
void saveTable(std::vector<T> &recordList, const char *tableName)
{
    if (recordList.empty())
        return;

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
        func.func = BLKIN;
        func.roxnum = recordList.size();
        dbfBulkCopyIn(&func, datafmt, recordList.data());
    }

    if (datafmt != nullptr)
    {
        free(datafmt);
        datafmt = nullptr;
    }
}

inline std::string jsonToString(const dfJson::Value &jsonValue)
{
    dfJson::StreamWriterBuilder writerBuilder;
    writerBuilder.settings_["indentation"] = ""; // 用于格式化输出的空格数，这里我们不需要缩进
    writerBuilder.settings_["commentStyle"] = "None";
    writerBuilder.settings_["emitUTF8"] = true;  // 输出utf8

    return dfJson::writeString(writerBuilder, jsonValue);
}

#endif // DZJZSRV_H