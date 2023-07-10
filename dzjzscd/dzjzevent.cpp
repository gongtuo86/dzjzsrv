#include "dzjzevent.h"

#include <string>

#include "dzjzcommon.h"
#include "eventdef.h"
#include "dflogger.h"

/**
 * @brief 生成低周减载方案设置不合理事项
 *
 * @param pItem
 * @param act =1 生成事件，=0 恢复事件
 * @return int
 */
int DZJZ_Event::make_judgeplan_event(TDZJZ_ROUNDITEM *pItem, int act)
{
    if (pItem == NULL)
        return -1;

    SYS_CLOCK evtclock;
    get_curclock(&evtclock);
    SCDEVENT tmpevent;
    memset(&tmpevent, 0, sizeof(tmpevent));

    strcpy(tmpevent.sysevent.group_name, pItem->staid);
    strcpy(tmpevent.sysevent.member_name0, std::to_string(pItem->id).c_str());
    tmpevent.sysevent.event_sort = EVENT_SCADA;  // scada事件
    tmpevent.sysevent.event_type = ENTTYPE_DZJZ; // 低周减载

    tmpevent.sysevent.ymd = evtclock.year * 10000 + evtclock.month * 100 + evtclock.day;
    tmpevent.sysevent.hmsms = evtclock.hour * 10000000 + evtclock.minute * 100000 + evtclock.second * 1000 + evtclock.msecond;
    tmpevent.sysevent.state_value = act; // =1 生成事件，=0 恢复事件
    tmpevent.sysevent.state_value2 = 10;
    tmpevent.sysevent.DiskOut = 1;       // 存盘
    tmpevent.sysevent.DispOut = 1;       // 登录

    if (act)
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "减载方案不合理:轮次[%s] 轮次项[%s]", pItem->roundname, pItem->name);
    }
    else
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "减载方案恢复:轮次[%s] 轮次项[%s]", pItem->roundname, pItem->name);
    }

    DFLOG_INFO("%s", tmpevent.sysevent.char_info);

    if (ismainserver())
    {
        save_event(&tmpevent, 1);
    }
    return 0;
}

/**
 * @brief 生成低周减载定值研判事项
 * @param pItem
 * @param act =1 生成事件，=0 恢复事件
 * @return int
 */
int DZJZ_Event::make_judgevalue_event(TDZJZ_ROUNDITEM *pItem, int act)
{
    if (pItem == NULL)
        return -1;

    SYS_CLOCK evtclock;
    get_curclock(&evtclock);
    SCDEVENT tmpevent;
    memset(&tmpevent, 0, sizeof(tmpevent));

    strcpy(tmpevent.sysevent.group_name, pItem->staid);
    strcpy(tmpevent.sysevent.member_name0, std::to_string(pItem->id).c_str());
    tmpevent.sysevent.event_sort = EVENT_SCADA;  // scada事件
    tmpevent.sysevent.event_type = ENTTYPE_DZJZ; // 低周减载

    tmpevent.sysevent.ymd = evtclock.year * 10000 + evtclock.month * 100 + evtclock.day;
    tmpevent.sysevent.hmsms = evtclock.hour * 10000000 + evtclock.minute * 100000 + evtclock.second * 1000 + evtclock.msecond;
    tmpevent.sysevent.state_value = act; // =1 生成事件，=0 恢复事件
    tmpevent.sysevent.DiskOut = 1;       // 存盘
    tmpevent.sysevent.DispOut = 1;       // 登录

    std::string strRealValue = getFixedValue(pItem->realvalue, pItem->realtime, pItem->functype);
    std::string strPlanValue = getFixedValue(pItem->planvalue, pItem->plantime, pItem->functype);
    if (act)
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "定值不同:装置[%s] 轮次项[%s] 实际定值[%s] 计划定值[%s]", pItem->devicename, pItem->roundname, strRealValue.c_str(), strPlanValue.c_str());
    }
    else
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "定值恢复:装置[%s] 轮次项[%s] 实际定值[%s] 计划定值[%s]", pItem->devicename, pItem->roundname, strRealValue.c_str(), strPlanValue.c_str());
    }

    DFLOG_INFO("%s", tmpevent.sysevent.char_info);

    if (ismainserver())
    {
        save_event(&tmpevent, 1);
    }
    return 0;
}

/**
 * @brief 生成低周减载方案研判事项
 *
 * @param pItem
 * @param act
 * @return int
 */
int DZJZ_Event::make_judgefunc_event(TDZJZ_ROUNDITEM *pItem)
{
    if (pItem == NULL)
        return -1;

    SYS_CLOCK evtclock;
    get_curclock(&evtclock);
    SCDEVENT tmpevent;
    memset(&tmpevent, 0, sizeof(tmpevent));

    strcpy(tmpevent.sysevent.group_name, pItem->staid);
    strcpy(tmpevent.sysevent.member_name0, std::to_string(pItem->id).c_str());
    tmpevent.sysevent.event_sort = EVENT_SCADA;  // scada事件
    tmpevent.sysevent.event_type = ENTTYPE_DZJZ; // 低周减载

    tmpevent.sysevent.ymd = evtclock.year * 10000 + evtclock.month * 100 + evtclock.day;
    tmpevent.sysevent.hmsms = evtclock.hour * 10000000 + evtclock.minute * 100000 + evtclock.second * 1000 + evtclock.msecond;
    tmpevent.sysevent.state_value = pItem->strapjudge; // 压板研判
    tmpevent.sysevent.DiskOut = 1;                     // 存盘
    tmpevent.sysevent.DispOut = 1;                     // 登录

    std::string strapName = getStrapName(pItem->deviceid, pItem->strapid);
    if (pItem->strapjudge)
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "%s:装置[%s] 轮次[%s] 轮次项[%s] 压板[%s]", s_stapArr[pItem->strapjudge], pItem->devicename, pItem->roundname, pItem->name, strapName.c_str());
    }
    else
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "%s:装置[%s] 轮次[%s] 轮次项[%s] 压板[%s]", s_stapArr[pItem->strapjudge], pItem->devicename, pItem->roundname, pItem->name, strapName.c_str());
    }

    DFLOG_INFO("%s", tmpevent.sysevent.char_info);

    if (ismainserver())
    {
        save_event(&tmpevent, 1);
    }

    return 0;
}

/**
 * @brief 生成低周减载容量不足事项
 *
 */
int DZJZ_Event::make_capacity_event(TDZJZ_ROUND *pRound, int act)
{
    if (pRound == NULL)
        return -1;

    SYS_CLOCK evtclock;
    get_curclock(&evtclock);
    SCDEVENT tmpevent;
    memset(&tmpevent, 0, sizeof(tmpevent));

    strcpy(tmpevent.sysevent.group_name, "xt");
    strcpy(tmpevent.sysevent.member_name0, std::to_string(pRound->id).c_str());
    tmpevent.sysevent.event_sort = EVENT_SCADA;  // scada事件
    tmpevent.sysevent.event_type = ENTTYPE_DZJZ; // 低周减载

    tmpevent.sysevent.ymd = evtclock.year * 10000 + evtclock.month * 100 + evtclock.day;
    tmpevent.sysevent.hmsms = evtclock.hour * 10000000 + evtclock.minute * 100000 + evtclock.second * 1000 + evtclock.msecond;
    tmpevent.sysevent.state_value = act; // =1 生成事件，=0 恢复事件
    tmpevent.sysevent.DiskOut = 1;       // 存盘
    tmpevent.sysevent.DispOut = 1;       // 登录

    if (act)
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "减载容量不足:轮次[%s] 投运切荷量[%f] 应切荷量[%f]", pRound->name, pRound->judgepower, pRound->requirePower);
    }
    else
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "减载容量不足恢复:轮次[%s] 投运切荷量[%f] 应切荷量[%f]", pRound->name, pRound->judgepower, pRound->requirePower);
    }

    DFLOG_INFO("%s", tmpevent.sysevent.char_info);

    if (ismainserver())
    {
        save_event(&tmpevent, 1);
    }
    return 0;
}

/**
 * @brief 装置动作事项
 *
 * @param actid
 * @param act
 * @return int
 */
int DZJZ_Event::make_action_event(SYS_CLOCK &evtclock, int deviceID, const std::string &deviceName, const std::vector<TDZJZ_ROUNDITEM> &roundItemVec, int act)
{
    // 遍历roundItemVec，生成事项
    for (auto &item : roundItemVec)
    {
        SCDEVENT tmpevent;
        memset(&tmpevent, 0, sizeof(tmpevent));

        strcpy(tmpevent.sysevent.group_name, item.staid);
        strcpy(tmpevent.sysevent.member_name0, std::to_string(deviceID).c_str());
        tmpevent.sysevent.event_sort = EVENT_SCADA;  // scada事件
        tmpevent.sysevent.event_type = ENTTYPE_DZJZ; // 低周减载

        tmpevent.sysevent.ymd = evtclock.year * 10000 + evtclock.month * 100 + evtclock.day;
        tmpevent.sysevent.hmsms = evtclock.hour * 10000000 + evtclock.minute * 100000 + evtclock.second * 1000 + evtclock.msecond;
        tmpevent.sysevent.state_value = act; // =1 生成事件，=0 恢复事件
        tmpevent.sysevent.DiskOut = 1;       // 存盘
        tmpevent.sysevent.DispOut = 1;       // 登录

        if (act)
        {
            std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "低周减载动作：装置[%s] 轮次[%s] 负荷[%s] 信号[%s]", deviceName.c_str(), item.roundname, item.feedername, getActionName(item.actionid).c_str());
        }
        else
        {
            std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "低周减载复归：装置[%s] 轮次[%s] 负荷[%s] 信号[%s]", deviceName.c_str(), item.roundname, item.feedername, getActionName(item.actionid).c_str());
        }

        DFLOG_INFO("%s", tmpevent.sysevent.char_info);

        if (ismainserver())
        {
            save_event(&tmpevent, 1);
        }
    }

    return 0;
}

/**
 * @brief 生成低周减载有源馈线研判
 *
 * @param pItem
 * @param act =1 生成事件，=0 恢复事件
 * @return int
 */
int DZJZ_Event::make_active_event(TDZJZ_ROUNDITEM *pItem, int act)
{
    if (pItem == NULL)
        return -1;

    SYS_CLOCK evtclock;
    get_curclock(&evtclock);
    SCDEVENT tmpevent;
    memset(&tmpevent, 0, sizeof(tmpevent));

    strcpy(tmpevent.sysevent.group_name, pItem->staid);
    strcpy(tmpevent.sysevent.member_name0, std::to_string(pItem->id).c_str());
    tmpevent.sysevent.event_sort = EVENT_SCADA;  // scada事件
    tmpevent.sysevent.event_type = ENTTYPE_DZJZ; // 低周减载

    tmpevent.sysevent.ymd = evtclock.year * 10000 + evtclock.month * 100 + evtclock.day;
    tmpevent.sysevent.hmsms = evtclock.hour * 10000000 + evtclock.minute * 100000 + evtclock.second * 1000 + evtclock.msecond;
    tmpevent.sysevent.state_value = act; // =1 生成事件，=0 恢复事件
    tmpevent.sysevent.state_value2 = 10;
    tmpevent.sysevent.DiskOut = 1;       // 存盘
    tmpevent.sysevent.DispOut = 1;       // 登录

    if (act)
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "有源馈线告警:轮次[%s] 轮次项[%s]", pItem->roundname, pItem->name);
    }
    else
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "有源馈线告警恢复:轮次[%s] 轮次项[%s]", pItem->roundname, pItem->name);
    }

    DFLOG_INFO("%s", tmpevent.sysevent.char_info);

    if (ismainserver())
    {
        save_event(&tmpevent, 1);
    }
    return 0;
}

int DZJZ_Event::make_exit_event(TDZJZ_ROUNDITEM *pItem, int act)
{
    if (pItem == NULL)
        return -1;

    SYS_CLOCK evtclock;
    get_curclock(&evtclock);
    SCDEVENT tmpevent;
    memset(&tmpevent, 0, sizeof(tmpevent));

    strcpy(tmpevent.sysevent.group_name, pItem->staid);
    strcpy(tmpevent.sysevent.member_name0, std::to_string(pItem->id).c_str());
    tmpevent.sysevent.event_sort = EVENT_SCADA;  // scada事件
    tmpevent.sysevent.event_type = ENTTYPE_DZJZ; // 低周减载

    tmpevent.sysevent.ymd = evtclock.year * 10000 + evtclock.month * 100 + evtclock.day;
    tmpevent.sysevent.hmsms = evtclock.hour * 10000000 + evtclock.minute * 100000 + evtclock.second * 1000 + evtclock.msecond;
    tmpevent.sysevent.state_value = act; // =1 生成事件，=0 恢复事件
    tmpevent.sysevent.state_value2 = 10;
    tmpevent.sysevent.DiskOut = 1;       // 存盘
    tmpevent.sysevent.DispOut = 1;       // 登录

    if (act)
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "出口矩阵异常:轮次[%s] 轮次项[%s] 装置[%s]", pItem->roundname, pItem->name, pItem->devicename);
    }
    else
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "出口矩阵恢复正常:轮次[%s] 轮次项[%s] 装置[%s]", pItem->roundname, pItem->name, pItem->devicename);
    }

    DFLOG_INFO("%s", tmpevent.sysevent.char_info);

    if (ismainserver())
    {
        save_event(&tmpevent, 1);
    }
    return 0;
}

int DZJZ_Event::make_alarm_event(TDZJZ_ROUNDITEM *pItem, int act)
{
    if (pItem == NULL)
        return -1;

    SYS_CLOCK evtclock;
    get_curclock(&evtclock);
    SCDEVENT tmpevent;
    memset(&tmpevent, 0, sizeof(tmpevent));

    strcpy(tmpevent.sysevent.group_name, pItem->staid);
    strcpy(tmpevent.sysevent.member_name0, std::to_string(pItem->id).c_str());
    tmpevent.sysevent.event_sort = EVENT_SCADA;  // scada事件
    tmpevent.sysevent.event_type = ENTTYPE_DZJZ; // 低周减载

    tmpevent.sysevent.ymd = evtclock.year * 10000 + evtclock.month * 100 + evtclock.day;
    tmpevent.sysevent.hmsms = evtclock.hour * 10000000 + evtclock.minute * 100000 + evtclock.second * 1000 + evtclock.msecond;
    tmpevent.sysevent.state_value = act; // =1 生成事件，=0 恢复事件
    tmpevent.sysevent.state_value2 = 10;
    tmpevent.sysevent.DiskOut = 1;       // 存盘
    tmpevent.sysevent.DispOut = 1;       // 登录

    if (act)
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "装置告警:轮次[%s] 轮次项[%s] 装置[%s]", pItem->roundname, pItem->name, pItem->devicename);
    }
    else
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "装置告警复归:轮次[%s] 轮次项[%s] 装置[%s]", pItem->roundname, pItem->name, pItem->devicename);
    }

    DFLOG_INFO("%s", tmpevent.sysevent.char_info);

    if (ismainserver())
    {
        save_event(&tmpevent, 1);
    }
    return 0;

    return 0;
}

int DZJZ_Event::make_funcJudge_event(TDZJZ_ROUNDITEM *pItem, int act)
{
    if (pItem == NULL)
        return -1;

    SYS_CLOCK evtclock;
    get_curclock(&evtclock);
    SCDEVENT tmpevent;
    memset(&tmpevent, 0, sizeof(tmpevent));

    strcpy(tmpevent.sysevent.group_name, pItem->staid);
    strcpy(tmpevent.sysevent.member_name0, std::to_string(pItem->id).c_str());
    tmpevent.sysevent.event_sort = EVENT_SCADA;  // scada事件
    tmpevent.sysevent.event_type = ENTTYPE_DZJZ; // 低周减载

    tmpevent.sysevent.ymd = evtclock.year * 10000 + evtclock.month * 100 + evtclock.day;
    tmpevent.sysevent.hmsms = evtclock.hour * 10000000 + evtclock.minute * 100000 + evtclock.second * 1000 + evtclock.msecond;
    tmpevent.sysevent.state_value = act; // =1 生成事件，=0 恢复事件
    tmpevent.sysevent.state_value2 = 10;
    tmpevent.sysevent.DiskOut = 1;       // 存盘
    tmpevent.sysevent.DispOut = 1;       // 登录

    if (act)
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "轮次项总告警:轮次[%s] 轮次项[%s] 装置[%s]", pItem->roundname, pItem->name, pItem->devicename);
    }
    else
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "轮次项总告警:轮次[%s] 轮次项[%s] 装置[%s]", pItem->roundname, pItem->name, pItem->devicename);
    }

    DFLOG_INFO("%s", tmpevent.sysevent.char_info);

    if (ismainserver())
    {
        save_event(&tmpevent, 1);
    }
    return 0;

    return 0;
}