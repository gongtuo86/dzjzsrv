#include "dzjzevent.h"

#include <string>

#include "dzjzcommon.h"
#include "eventdef.h"
#include "dflogger.h"

/**
 * @brief ���ɵ��ܼ��ط������ò���������
 *
 * @param pItem
 * @param act =1 �����¼���=0 �ָ��¼�
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
    tmpevent.sysevent.event_sort = EVENT_SCADA;  // scada�¼�
    tmpevent.sysevent.event_type = ENTTYPE_DZJZ; // ���ܼ���

    tmpevent.sysevent.ymd = evtclock.year * 10000 + evtclock.month * 100 + evtclock.day;
    tmpevent.sysevent.hmsms = evtclock.hour * 10000000 + evtclock.minute * 100000 + evtclock.second * 1000 + evtclock.msecond;
    tmpevent.sysevent.state_value = act; // =1 �����¼���=0 �ָ��¼�
    tmpevent.sysevent.state_value2 = 10;
    tmpevent.sysevent.DiskOut = 1;       // ����
    tmpevent.sysevent.DispOut = 1;       // ��¼

    if (act)
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "���ط���������:�ִ�[%s] �ִ���[%s]", pItem->roundname, pItem->name);
    }
    else
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "���ط����ָ�:�ִ�[%s] �ִ���[%s]", pItem->roundname, pItem->name);
    }

    DFLOG_INFO("%s", tmpevent.sysevent.char_info);

    if (ismainserver())
    {
        save_event(&tmpevent, 1);
    }
    return 0;
}

/**
 * @brief ���ɵ��ܼ��ض�ֵ��������
 * @param pItem
 * @param act =1 �����¼���=0 �ָ��¼�
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
    tmpevent.sysevent.event_sort = EVENT_SCADA;  // scada�¼�
    tmpevent.sysevent.event_type = ENTTYPE_DZJZ; // ���ܼ���

    tmpevent.sysevent.ymd = evtclock.year * 10000 + evtclock.month * 100 + evtclock.day;
    tmpevent.sysevent.hmsms = evtclock.hour * 10000000 + evtclock.minute * 100000 + evtclock.second * 1000 + evtclock.msecond;
    tmpevent.sysevent.state_value = act; // =1 �����¼���=0 �ָ��¼�
    tmpevent.sysevent.DiskOut = 1;       // ����
    tmpevent.sysevent.DispOut = 1;       // ��¼

    std::string strRealValue = getFixedValue(pItem->realvalue, pItem->realtime, pItem->functype);
    std::string strPlanValue = getFixedValue(pItem->planvalue, pItem->plantime, pItem->functype);
    if (act)
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "��ֵ��ͬ:װ��[%s] �ִ���[%s] ʵ�ʶ�ֵ[%s] �ƻ���ֵ[%s]", pItem->devicename, pItem->roundname, strRealValue.c_str(), strPlanValue.c_str());
    }
    else
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "��ֵ�ָ�:װ��[%s] �ִ���[%s] ʵ�ʶ�ֵ[%s] �ƻ���ֵ[%s]", pItem->devicename, pItem->roundname, strRealValue.c_str(), strPlanValue.c_str());
    }

    DFLOG_INFO("%s", tmpevent.sysevent.char_info);

    if (ismainserver())
    {
        save_event(&tmpevent, 1);
    }
    return 0;
}

/**
 * @brief ���ɵ��ܼ��ط�����������
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
    tmpevent.sysevent.event_sort = EVENT_SCADA;  // scada�¼�
    tmpevent.sysevent.event_type = ENTTYPE_DZJZ; // ���ܼ���

    tmpevent.sysevent.ymd = evtclock.year * 10000 + evtclock.month * 100 + evtclock.day;
    tmpevent.sysevent.hmsms = evtclock.hour * 10000000 + evtclock.minute * 100000 + evtclock.second * 1000 + evtclock.msecond;
    tmpevent.sysevent.state_value = pItem->strapjudge; // ѹ������
    tmpevent.sysevent.DiskOut = 1;                     // ����
    tmpevent.sysevent.DispOut = 1;                     // ��¼

    std::string strapName = getStrapName(pItem->deviceid, pItem->strapid);
    if (pItem->strapjudge)
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "%s:װ��[%s] �ִ�[%s] �ִ���[%s] ѹ��[%s]", s_stapArr[pItem->strapjudge], pItem->devicename, pItem->roundname, pItem->name, strapName.c_str());
    }
    else
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "%s:װ��[%s] �ִ�[%s] �ִ���[%s] ѹ��[%s]", s_stapArr[pItem->strapjudge], pItem->devicename, pItem->roundname, pItem->name, strapName.c_str());
    }

    DFLOG_INFO("%s", tmpevent.sysevent.char_info);

    if (ismainserver())
    {
        save_event(&tmpevent, 1);
    }

    return 0;
}

/**
 * @brief ���ɵ��ܼ���������������
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
    tmpevent.sysevent.event_sort = EVENT_SCADA;  // scada�¼�
    tmpevent.sysevent.event_type = ENTTYPE_DZJZ; // ���ܼ���

    tmpevent.sysevent.ymd = evtclock.year * 10000 + evtclock.month * 100 + evtclock.day;
    tmpevent.sysevent.hmsms = evtclock.hour * 10000000 + evtclock.minute * 100000 + evtclock.second * 1000 + evtclock.msecond;
    tmpevent.sysevent.state_value = act; // =1 �����¼���=0 �ָ��¼�
    tmpevent.sysevent.DiskOut = 1;       // ����
    tmpevent.sysevent.DispOut = 1;       // ��¼

    if (act)
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "������������:�ִ�[%s] Ͷ���к���[%f] Ӧ�к���[%f]", pRound->name, pRound->judgepower, pRound->requirePower);
    }
    else
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "������������ָ�:�ִ�[%s] Ͷ���к���[%f] Ӧ�к���[%f]", pRound->name, pRound->judgepower, pRound->requirePower);
    }

    DFLOG_INFO("%s", tmpevent.sysevent.char_info);

    if (ismainserver())
    {
        save_event(&tmpevent, 1);
    }
    return 0;
}

/**
 * @brief װ�ö�������
 *
 * @param actid
 * @param act
 * @return int
 */
int DZJZ_Event::make_action_event(SYS_CLOCK &evtclock, int deviceID, const std::string &deviceName, const std::vector<TDZJZ_ROUNDITEM> &roundItemVec, int act)
{
    // ����roundItemVec����������
    for (auto &item : roundItemVec)
    {
        SCDEVENT tmpevent;
        memset(&tmpevent, 0, sizeof(tmpevent));

        strcpy(tmpevent.sysevent.group_name, item.staid);
        strcpy(tmpevent.sysevent.member_name0, std::to_string(deviceID).c_str());
        tmpevent.sysevent.event_sort = EVENT_SCADA;  // scada�¼�
        tmpevent.sysevent.event_type = ENTTYPE_DZJZ; // ���ܼ���

        tmpevent.sysevent.ymd = evtclock.year * 10000 + evtclock.month * 100 + evtclock.day;
        tmpevent.sysevent.hmsms = evtclock.hour * 10000000 + evtclock.minute * 100000 + evtclock.second * 1000 + evtclock.msecond;
        tmpevent.sysevent.state_value = act; // =1 �����¼���=0 �ָ��¼�
        tmpevent.sysevent.DiskOut = 1;       // ����
        tmpevent.sysevent.DispOut = 1;       // ��¼

        if (act)
        {
            std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "���ܼ��ض�����װ��[%s] �ִ�[%s] ����[%s] �ź�[%s]", deviceName.c_str(), item.roundname, item.feedername, getActionName(item.actionid).c_str());
        }
        else
        {
            std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "���ܼ��ظ��飺װ��[%s] �ִ�[%s] ����[%s] �ź�[%s]", deviceName.c_str(), item.roundname, item.feedername, getActionName(item.actionid).c_str());
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
 * @brief ���ɵ��ܼ�����Դ��������
 *
 * @param pItem
 * @param act =1 �����¼���=0 �ָ��¼�
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
    tmpevent.sysevent.event_sort = EVENT_SCADA;  // scada�¼�
    tmpevent.sysevent.event_type = ENTTYPE_DZJZ; // ���ܼ���

    tmpevent.sysevent.ymd = evtclock.year * 10000 + evtclock.month * 100 + evtclock.day;
    tmpevent.sysevent.hmsms = evtclock.hour * 10000000 + evtclock.minute * 100000 + evtclock.second * 1000 + evtclock.msecond;
    tmpevent.sysevent.state_value = act; // =1 �����¼���=0 �ָ��¼�
    tmpevent.sysevent.state_value2 = 10;
    tmpevent.sysevent.DiskOut = 1;       // ����
    tmpevent.sysevent.DispOut = 1;       // ��¼

    if (act)
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "��Դ���߸澯:�ִ�[%s] �ִ���[%s]", pItem->roundname, pItem->name);
    }
    else
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "��Դ���߸澯�ָ�:�ִ�[%s] �ִ���[%s]", pItem->roundname, pItem->name);
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
    tmpevent.sysevent.event_sort = EVENT_SCADA;  // scada�¼�
    tmpevent.sysevent.event_type = ENTTYPE_DZJZ; // ���ܼ���

    tmpevent.sysevent.ymd = evtclock.year * 10000 + evtclock.month * 100 + evtclock.day;
    tmpevent.sysevent.hmsms = evtclock.hour * 10000000 + evtclock.minute * 100000 + evtclock.second * 1000 + evtclock.msecond;
    tmpevent.sysevent.state_value = act; // =1 �����¼���=0 �ָ��¼�
    tmpevent.sysevent.state_value2 = 10;
    tmpevent.sysevent.DiskOut = 1;       // ����
    tmpevent.sysevent.DispOut = 1;       // ��¼

    if (act)
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "���ھ����쳣:�ִ�[%s] �ִ���[%s] װ��[%s]", pItem->roundname, pItem->name, pItem->devicename);
    }
    else
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "���ھ���ָ�����:�ִ�[%s] �ִ���[%s] װ��[%s]", pItem->roundname, pItem->name, pItem->devicename);
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
    tmpevent.sysevent.event_sort = EVENT_SCADA;  // scada�¼�
    tmpevent.sysevent.event_type = ENTTYPE_DZJZ; // ���ܼ���

    tmpevent.sysevent.ymd = evtclock.year * 10000 + evtclock.month * 100 + evtclock.day;
    tmpevent.sysevent.hmsms = evtclock.hour * 10000000 + evtclock.minute * 100000 + evtclock.second * 1000 + evtclock.msecond;
    tmpevent.sysevent.state_value = act; // =1 �����¼���=0 �ָ��¼�
    tmpevent.sysevent.state_value2 = 10;
    tmpevent.sysevent.DiskOut = 1;       // ����
    tmpevent.sysevent.DispOut = 1;       // ��¼

    if (act)
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "װ�ø澯:�ִ�[%s] �ִ���[%s] װ��[%s]", pItem->roundname, pItem->name, pItem->devicename);
    }
    else
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "װ�ø澯����:�ִ�[%s] �ִ���[%s] װ��[%s]", pItem->roundname, pItem->name, pItem->devicename);
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
    tmpevent.sysevent.event_sort = EVENT_SCADA;  // scada�¼�
    tmpevent.sysevent.event_type = ENTTYPE_DZJZ; // ���ܼ���

    tmpevent.sysevent.ymd = evtclock.year * 10000 + evtclock.month * 100 + evtclock.day;
    tmpevent.sysevent.hmsms = evtclock.hour * 10000000 + evtclock.minute * 100000 + evtclock.second * 1000 + evtclock.msecond;
    tmpevent.sysevent.state_value = act; // =1 �����¼���=0 �ָ��¼�
    tmpevent.sysevent.state_value2 = 10;
    tmpevent.sysevent.DiskOut = 1;       // ����
    tmpevent.sysevent.DispOut = 1;       // ��¼

    if (act)
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "�ִ����ܸ澯:�ִ�[%s] �ִ���[%s] װ��[%s]", pItem->roundname, pItem->name, pItem->devicename);
    }
    else
    {
        std::snprintf(tmpevent.sysevent.char_info, sizeof(tmpevent.sysevent.char_info), "�ִ����ܸ澯:�ִ�[%s] �ִ���[%s] װ��[%s]", pItem->roundname, pItem->name, pItem->devicename);
    }

    DFLOG_INFO("%s", tmpevent.sysevent.char_info);

    if (ismainserver())
    {
        save_event(&tmpevent, 1);
    }
    return 0;

    return 0;
}