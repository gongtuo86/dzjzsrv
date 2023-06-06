#include "dflogger.h"
#include "dzjzevent.h"

const char *s_stapArr[] = {"应退已退", "应投未投", "应退未退", "应投已投"};

int ismainserver(void)
{
    net_cfg netcfg;
    if (netcfg.am_I_main_server(SCADA_SERVER) == TRUE)
        return (1);
    return (0);
}

/**
 * @brief 返回定值字符串
 *
 * @param value 频率或电压定值
 * @param time 延迟时间
 * @param type = 1 低频 =2 低压
 * @return std::string
 */
std::string getFixedValue(float value, float time, int type)
{
    char buf[128] = {0};
    if (type == 1)
    {
        std::sprintf(buf, "%.2fHz/%.2fs", value, time / 1000);
    }
    else if (type == 2)
    {
        std::sprintf(buf, "%.2fKV/%.2fs", value, time / 1000);
    }
    return std::string(buf);
}

/**
 * @brief 获取动作信号名称
 *
 * @param actionID
 * @return std::string
 */
std::string getActionName(const char *actionID)
{
    RdbSQL sql;
    sql.selectfrom(PROTSIG_TBLNAME);
    sql.selectattr("describe");
    sql.where("name", DATATYPE_STRING, const_cast<char *>(actionID));
    int nRet = sql.select(MyUserName, MyPassWord);
    if (nRet != OK)
    {
        DFLOG_ERROR("failed to execute sql, nRet=%d id=%s", nRet, actionID);
        return "";
    }
    int nRcdCnt = sql.getrcdcnt();
    if (nRcdCnt <= 0)
    {
        DFLOG_ERROR("no data id=%s", actionID);
        return "";
    }

    std::string actionName = sql.get_string(0, "describe");
    // DFLOG_DEBUG("动作信号名称：%s", actionName.c_str());
    return actionName;
}

/**
 * @brief Get the Max I D object
 *
 * @param tableName
 * @return int
 */
int getMaxID(const char *tableName)
{
    int i = 0;
    int nOrder = 0;
    FUNC_STR func;
    CS_FLOAT *p = NULL;
    memset(&func, 0, sizeof(FUNC_STR));
    func.func = SELECT_ISQL_RESULTS;
    func.serverno = SERVER_DEFAULT;
    sprintf(func.isql, "select max(ID) from %s", tableName);

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

std::string join(const std::vector<std::string> &items, const std::string &delimiter)
{
    std::string result;
    for (size_t i = 0; i < items.size(); ++i)
    {
        result += items[i];
        if (i < items.size() - 1)
        {
            result += delimiter;
        }
    }
    return result;
}

DZJZ_Event dzjzEnt;
