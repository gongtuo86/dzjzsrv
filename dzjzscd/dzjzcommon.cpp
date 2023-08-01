#include "dflogger.h"
#include "dzjzevent.h"

const char *s_stapArr[] = {"Ӧ������", "ӦͶδͶ", "Ӧ��δ��", "ӦͶ��Ͷ"};

int ismainserver(void)
{
    net_cfg netcfg;
    if (netcfg.am_I_main_server(SCADA_SERVER) == TRUE)
        return (1);
    return (0);
}

/**
 * @brief ���ض�ֵ�ַ���
 *
 * @param value Ƶ�ʻ��ѹ��ֵ
 * @param time �ӳ�ʱ��
 * @param type = 1 ��Ƶ =2 ��ѹ
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
 * @brief ��ȡ�����ź�����
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
    // DFLOG_DEBUG("�����ź����ƣ�%s", actionName.c_str());
    return actionName;
}

/**
 * @brief ��ȡѹ���ź�����
 *
 * @param actionID
 * @return std::string
 */
std::string getStrapName(int nRtuNo, const char *id)
{
    if (id == NULL || strlen(id) == 0)
        return "";
    std::vector<std::string> strapids;
    splitString(id, strapids, ',');

    std::vector<std::string> strapNames;

    for (const auto &strapId : strapids)
    {
        std::vector<std::string> fieldVals;
        splitString(strapId.c_str(), fieldVals, ':');

        try
        {
            short type = std::stoi(fieldVals[0]);
            short groupNo = std::stoi(fieldVals[1]);
            short no = std::stoi(fieldVals[2]);

            RdbSQL sql;
            sql.selectfrom(DZJZ_DZ_TBLNAME);
            sql.selectattr("desc");
            sql.where("rtuno", DATATYPE_SHORT, &nRtuNo);
            sql.where("type", DATATYPE_SHORT, &type);
            sql.where("groupno", DATATYPE_SHORT, &groupNo);
            sql.where("no", DATATYPE_SHORT, &no);
            int nRet = sql.select(MyUserName, MyPassWord);
            if (nRet != OK)
            {
                DFLOG_ERROR("failed to execute sql, nRet=%d id=%s", nRet, id);
                continue;
            }
            int nRcdCnt = sql.getrcdcnt();
            if (nRcdCnt <= 0)
            {
                DFLOG_ERROR("no data id=%s", id);
                continue;
            }

            std::string name = sql.get_string(0, "desc");

            strapNames.push_back(name);
        }
        catch (std::invalid_argument &e)
        {
            DFLOG_ERROR("Invalid argument: %s, id=%s", e.what(), id);
        }
        catch (std::out_of_range &e)
        {
            DFLOG_ERROR("Out of range: %s, id=%s", e.what(), id);
        }
    }

    return join(strapNames, ",");
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

/**
 * @brief �ָ��ַ���
 *
 * @param str
 * @param ids
 */
void splitString(const char *str, std::vector<std::string> &ids, char delim)
{
    ids.clear();
    std::istringstream iss(str);
    std::string strapid;
    while (std::getline(iss, strapid, delim))
    {
        ids.push_back(strapid);
    }
}

DZJZ_Event dzjzEnt;
