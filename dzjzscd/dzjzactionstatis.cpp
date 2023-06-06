#include "dzjzactionstatis.h"

#include "dflogger.h"
#include "dzjzsrv.h"

/**
 * @brief ���ݶ����źŶ�ȡ�ִ���
 *
 * @param name
 * @param vec
 */
int DZJZ_ActionStatis::getRoundItemVec(char *name, std::vector<TDZJZ_ROUNDITEM> &vec)
{
    vec.clear();

    RdbSQL sql;
    sql.selectfrom(DZJZ_ROUND_ITEM_TBLNAME);
    sql.where("actionid", DATATYPE_STRING, name);
    int nRet = sql.select(MyUserName, MyPassWord);
    if (nRet != OK)
    {
        DFLOG_ERROR("failed to execute sql, nRet=%d name=%s", nRet, name);
        return -1;
    }

    int nRcdCnt = sql.getrcdcnt();
    if (nRcdCnt <= 0)
    {
        DFLOG_ERROR("no data name=%s", name);
        return -2;
    }

    for (int i = 0; i < nRcdCnt; i++)
    {
        TDZJZ_ROUNDITEM *pItem = (TDZJZ_ROUNDITEM *)sql.GetRecordAddr(i);
        if (pItem != nullptr)
        {
            // DFLOG_DEBUG("id=%d", pItem->id);
            vec.push_back(*pItem);
        }
    }
    return 0;
}

/**
 * @brief �����ִ���
 *
 * @param time
 * @param roundItemVec
 */
void DZJZ_ActionStatis::saveActionRoundItemVec(int id, int time, const std::vector<TDZJZ_ROUNDITEM> &roundItemVec)
{
    std::vector<TDZJZ_ACTION_ROUND_ITEM> actRoundItemVec;
    for (auto &pRoundItem : roundItemVec)
    {
        TDZJZ_ACTION_ROUND_ITEM item;
        item.id = id;
        item.time = time;
        item.roundItemID = pRoundItem.id;
        strcpy(item.feederName, pRoundItem.feedername);
        item.pValue = pRoundItem.pvalue;
        item.areaID = pRoundItem.areaid;
        strcpy(item.areaName, getAreaName(item.areaID).c_str());
        item.roundType = pRoundItem.roundtype;
        strcpy(item.roundTypeName, getRoundTypeName(item.roundType).c_str());
        strcpy(item.actionID, pRoundItem.actionid);
        strcpy(item.actionName, getActionName(item.actionID).c_str());
        actRoundItemVec.push_back(item);
    }

    if (ismainserver())
    {
        saveTable(actRoundItemVec, "xopenshdb.dbo.���ܼ����ִ������");
    }
}

/**
 * @brief ��ȡ��������
 *
 * @param areaID
 * @return std::string
 */
std::string DZJZ_ActionStatis::getAreaName(int areaID)
{
    RdbSQL sql;
    sql.selectfrom(DZJZ_AREA_TBLNAME);
    sql.selectattr("name");
    sql.where("id", DATATYPE_INT, &areaID);
    int nRet = sql.select(MyUserName, MyPassWord);
    if (nRet != OK)
    {
        DFLOG_ERROR("failed to execute sql, nRet=%d id=%d", nRet, areaID);
        return "";
    }
    int nRcdCnt = sql.getrcdcnt();
    if (nRcdCnt <= 0)
    {
        DFLOG_ERROR("no data id=%d", areaID);
        return "";
    }

    std::string areaName = sql.get_string(0, "name");
    // DFLOG_DEBUG("�������ƣ�%s", areaName.c_str());
    return areaName;
}

/**
 * @brief ��ȡ�ִ���������
 *
 * @param typeID
 * @return std::string
 */
std::string DZJZ_ActionStatis::getRoundTypeName(int typeID)
{
    RdbSQL sql;
    sql.selectfrom(DZJZ_ROUND_STATIC_TBLNAME);
    sql.selectattr("name");
    sql.where("id", DATATYPE_INT, &typeID);
    int nRet = sql.select(MyUserName, MyPassWord);
    if (nRet != OK)
    {
        DFLOG_ERROR("failed to execute sql, nRet=%d id=%d", nRet, typeID);
        return "";
    }
    int nRcdCnt = sql.getrcdcnt();
    if (nRcdCnt <= 0)
    {
        DFLOG_ERROR("no data id=%d", typeID);
        return "";
    }

    std::string typeName = sql.get_string(0, "name");
    // DFLOG_DEBUG("�ִ��������ƣ�%s", typeName.c_str());
    return typeName;
}

/**
 * @brief ��unixʱ���ת��Ϊ�ַ���
 *
 */

std::string DZJZ_ActionStatis::time2str(int time)
{
    char buf[32] = {0};
    time_t t = time;
    struct tm *tm = localtime(&t);
    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    return buf;
}

/**
 * @brief ��ȡ��������
 *
 * @param time
 * @param pDevice
 * @return std::string
 */
std::string DZJZ_ActionStatis::getActionDesc(int time, const std::string &deviceName)
{
    return "ʱ�䣺" + time2str(time) + " �豸��" + deviceName + " ����";
}

/**
 * @brief ��ȡ��ǰȫ��Ͷ���к���
 *
 * @return int
 */
float DZJZ_ActionStatis::getAllNetJudgePower()
{
    RdbSQL sql;
    sql.selectfrom(DZJZ_AREA_TBLNAME);
    sql.selectattr("judgepower");
    sql.where("id", DATATYPE_INT, (void *)&ALL_AREA_ID);
    int nRet = sql.select(MyUserName, MyPassWord);
    if (nRet != OK)
    {
        DFLOG_ERROR("failed to execute sql, nRet=%d id=%d", nRet, ALL_AREA_ID);
        return 0;
    }
    int nRcdCnt = sql.getrcdcnt();
    if (nRcdCnt <= 0)
    {
        DFLOG_ERROR("no data id=%d", ALL_AREA_ID);
        return 0;
    }

    float totalLoad = sql.get_floatval(0, "judgepower");
    // DFLOG_DEBUG("ȫ��Ͷ���к�����%f", totalLoad);
    return totalLoad;
}

/**
 * @brief ���涯����Ϣ
 *
 * @param time
 * @param actionid
 * @param roundItemVec
 */
int DZJZ_ActionStatis::saveAction(int time, int deviceID, const std::string &deviceName, const std::vector<TDZJZ_ROUNDITEM> &roundItemVec)
{
    TDZJZ_ACTION action;
    memset(&action, 0, sizeof(TDZJZ_ACTION));
    action.time = time;

    action.id = getMaxID("xopenshdb.dbo.���ܼ���װ�ö�����");
    action.deviceID = deviceID;
    strcpy(action.deviceName, deviceName.c_str());
    strcpy(action.description, getActionDesc(time, deviceName).c_str());
    action.areaJudgeValue = getAllNetJudgePower();
    action.actionValue = calcActionJudgePower(roundItemVec);
    if (!ISZERO(action.areaJudgeValue))
    {
        action.actionPowerRate = action.actionValue / action.areaJudgeValue * 100;
    }
    action.actionFeederNum = roundItemVec.size();
    if (ismainserver())
    {
        saveTable(action, "xopenshdb.dbo.���ܼ���װ�ö�����");
    }

    return action.id;
}

/**
 * @brief ���㶯���к���
 *
 */
float DZJZ_ActionStatis::calcActionJudgePower(const std::vector<TDZJZ_ROUNDITEM> &roundItemVec)
{
    float actionJudgePower = 0;
    for (auto &roundItem : roundItemVec)
    {
        actionJudgePower += roundItem.pvalue;
    }
    // DFLOG_DEBUG("�����к�����%f", actionJudgePower);
    return actionJudgePower;
}

/**
 * @brief ���涯����Ϣ
 *
 * @param time ʱ��
 * @param actionid ��������
 * @param roundItemVec �ִ���
 */
void DZJZ_ActionStatis::saveActionInfo(int time, int deviceID, const std::string &deviceName, const std::vector<TDZJZ_ROUNDITEM> &roundItemVec)
{
    // ���涯��
    int id = saveAction(time, deviceID, deviceName, roundItemVec);
    if (id != -1)
    {
        // �����ִ���
        saveActionRoundItemVec(id, time, roundItemVec);
    }
    else
    {
        DFLOG_ERROR("����id��ȡʧ��: deviceID=%d", deviceID);
    }

    DFLOG_INFO("������Ϣ����ɹ�: deviceID=%d", deviceID);
}