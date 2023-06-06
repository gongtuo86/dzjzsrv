#pragma once

#include <vector>

#include "dzjzcommon.h"
#include "rdbscd.h"

/**
 * @brief ���ܼ����������
 *
 */
class DZJZ_Event : public Event
{
public:
    DZJZ_Event(){};
    int make_judgeplan_event(TDZJZ_ROUNDITEM *pItem, int act);
    int make_judgevalue_event(TDZJZ_ROUNDITEM *pItem, int act);
    int make_judgefunc_event(TDZJZ_ROUNDITEM *pItem);
    int make_capacity_event(TDZJZ_ROUND *pItem, int act);
    int make_action_event(SYS_CLOCK &time, int deviceID, const std::string &deviceName, const std::vector<TDZJZ_ROUNDITEM> &roundItemVec, int act);
};