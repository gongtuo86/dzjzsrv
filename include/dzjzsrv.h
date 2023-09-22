#ifndef DZJZSRV_H
#define DZJZSRV_H

#include "dfjson/json.h"
#include "dorbase.h"
#include "sybase.h"
#include "dflogger.h"

#include <iomanip>

#ifndef MyUserName
#define MyUserName "scada"
#endif
#ifndef MyPassWord
#define MyPassWord "df_scada"
#endif

enum DZJZ_STRAP
{
    EXIT_EXIT = 0, // 应退已退
    ACT_EXIT = 1,  // 应投未投
    EXIT_ACT = 2,  // 应退未退
    ACT_ACT = 3,   // 应投已投
};

enum DZJZ_DEVICE_STATUS
{
    NOT_CHECKED = 0, // 未核对
    CONSISTENT,      // 一致
    NOT_SUBMITTED,   // 应投未投
    NOT_RETURNED,    // 应退未退
    VERIFYF_AILED    // 校核失败
};

enum DZJZ_DEVICE_REASON
{
    UNKOWN_RASON = 0,        // 未知原因
    NO_RESPONSE = 4,         // 设备无应答
    TIMEOUT = 6,             // 设备应答报文非法命令等待返回结果超时
    COMM_INTERRUPTED = 14,   // 通讯中断
    FUNC_NOT_SUPPORTED = 71, // 功能不支持
    NO_BASE_VALUE = 80,      // 没有基准值
    PLAT_TIMEOUT = 100,      // 后台处理超时
    MANUAL = 103,            // 人工设置不校核
    MAINTENANCE = 104,       // 检修
    COMM_FAIL = 109,         // 设备通讯中断

};

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

    if (func.recorderlen != sizeof(T))
    {
        DFLOG_ERROR("%s 数据库结构和结构体定义不一致", tableName);
        return;
    }
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
    writerBuilder.settings_["emitUTF8"] = true; // 输出utf8

    return dfJson::writeString(writerBuilder, jsonValue);
}

/**
 * @brief 保留3位小数
 *
 * @param value
 * @return std::string
 */
inline std::string roundToThreeDecimalPlaces(float value)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(3) << value;
    return stream.str();
}

inline bool isStrapNormal(int strapJudge)
{
    if (strapJudge == ACT_ACT || strapJudge == EXIT_EXIT)
        return true;
    else
        return false;

    return false;
}

#endif // DZJZSRV_H