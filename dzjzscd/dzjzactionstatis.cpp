#include "dzjzactionstatis.h"

#include "dflogger.h"
#include "dzjzsrv.h"

/**
 * @brief 根据动作信号读取轮次项
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
 * @brief 保存轮次项
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
        item.deviceID = pRoundItem.deviceid;
        strcpy(item.deviceName, pRoundItem.devicename);
        item.deviceType = pRoundItem.devtype;
        item.roundFuncType = pRoundItem.functype;
        strcpy(item.ofSta, pRoundItem.staid);     
        actRoundItemVec.push_back(item);
    }

    if (ismainserver())
    {
        saveTable(actRoundItemVec, "xopenshdb.dbo.低周减载轮次项动作表");
    }
}

/**
 * @brief 获取区域名称
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
    // DFLOG_DEBUG("区域名称：%s", areaName.c_str());
    return areaName;
}

/**
 * @brief 获取轮次类型名称
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
    // DFLOG_DEBUG("轮次类型名称：%s", typeName.c_str());
    return typeName;
}

/**
 * @brief 将unix时间戳转换为字符串
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
 * @brief 获取动作描述
 *
 * @param time
 * @param pDevice
 * @return std::string
 */
std::string DZJZ_ActionStatis::getActionDesc()
{
    // return "时间：" + time2str(time) + " 设备：" + deviceName + " 动作";
    return "低周减载动作 " + time2str(m_startTime);
}

/**
 * @brief 获取当前全网投运切荷量
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
    // DFLOG_DEBUG("全网投运切荷量：%f", totalLoad);
    return totalLoad;
}

/**
 * @brief 保存动作信息
 *
 * @param time
 * @param actionid
 * @param roundItemVec
 */
int DZJZ_ActionStatis::saveAction(const std::vector<TDZJZ_ROUNDITEM> &roundItemVec)
{
    TDZJZ_ACTION action;
    memset(&action, 0, sizeof(TDZJZ_ACTION));
    action.time = m_startTime;
    action.id = getMaxID("xopenshdb.dbo.低周减载装置动作表");
    // action.deviceID = deviceID;
    // strcpy(action.deviceName, deviceName.c_str());
    strcpy(action.description, getActionDesc().c_str());
    action.areaJudgeValue = getAllNetJudgePower();
    action.actionValue = calcActionJudgePower(roundItemVec);
    if (!ISZERO(action.areaJudgeValue))
    {
        action.actionPowerRate = action.actionValue / action.areaJudgeValue * 100;
    }
    action.actionFeederNum = roundItemVec.size();
    if (ismainserver())
    {
        saveTable(action, "xopenshdb.dbo.低周减载装置动作表");
    }

    return action.id;
}

/**
 * @brief 计算动作切荷量
 *
 */
float DZJZ_ActionStatis::calcActionJudgePower(const std::vector<TDZJZ_ROUNDITEM> &roundItemVec)
{
    float actionJudgePower = 0;
    for (auto &roundItem : roundItemVec)
    {
        actionJudgePower += roundItem.pvalue;
    }
    // DFLOG_DEBUG("动作切荷量：%f", actionJudgePower);
    return actionJudgePower;
}

/**
 * @brief 保存动作信息
 *
 * @param time 时间
 * @param actionid 对象名称
 * @param roundItemVec 轮次项
 */
void DZJZ_ActionStatis::saveActionInfo(const std::vector<TDZJZ_ROUNDITEM> &roundItemVec)
{
    // 保存动作
    int id = saveAction(roundItemVec);
    if (id != -1)
    {
        // 保存轮次项
        saveActionRoundItemVec(id, m_startTime, roundItemVec);
    }
    else
    {
        DFLOG_ERROR("动作id获取失败");
    }

    DFLOG_INFO("动作信息保存成功");
}

void DZJZ_ActionStatis::onActionTimerTimeout()
{
    while (m_timerRunning)
    {
        // 如果已经有60秒没有新的轮次项加入了，就保存
        if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - m_lastAddTime).count() >= 60)
        {
            std::lock_guard<std::mutex> lock(m_cacheMutex);
            if (!m_roundItemCache.empty())
            {
                DFLOG_DEBUG("已经有60秒没有新的轮次项生成, 记录所有动作轮次项");
                saveActionInfo(m_roundItemCache);
                m_roundItemCache.clear();
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
void DZJZ_ActionStatis::startActionTimer()
{
    m_timerRunning = true;
    m_actionTimerThread = std::thread(&DZJZ_ActionStatis::onActionTimerTimeout, this);
    DFLOG_DEBUG("Start action timer");
}

void DZJZ_ActionStatis::stopActionTimer()
{
    m_timerRunning = false;
    if (m_actionTimerThread.joinable())
    {
        m_actionTimerThread.join();
    }
    DFLOG_DEBUG("Stop action timer");
}

void DZJZ_ActionStatis::addRoundItem(const TDZJZ_ROUNDITEM &item)
{
    std::lock_guard<std::mutex> lock(m_cacheMutex);
    if (m_roundItemCache.empty())
    {
        get_intertime(&m_startTime);
    }
    m_roundItemCache.push_back(item);
    m_lastAddTime = std::chrono::system_clock::now();
}