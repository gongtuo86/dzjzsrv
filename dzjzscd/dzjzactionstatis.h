#pragma once

#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>

#include "dzjzcommon.h"

class DZJZ_ActionStatis
{
public:
    DZJZ_ActionStatis(){};
    ~DZJZ_ActionStatis(){};
    int getRoundItemVec(char *name, std::vector<TDZJZ_ROUNDITEM> &vec);
    void saveActionRoundItemVec(int id, int time, const std::vector<TDZJZ_ROUNDITEM> &roundItemVec);
    std::string getAreaName(int areaID);
    std::string getRoundTypeName(int typeID);
    std::string time2str(int time);
    std::string getActionDesc();
    float getAllNetJudgePower();
    int saveAction(const std::vector<TDZJZ_ROUNDITEM> &roundItemVec);
    float calcActionJudgePower(const std::vector<TDZJZ_ROUNDITEM> &roundItemVec);
    void saveActionInfo(const std::vector<TDZJZ_ROUNDITEM> &roundItemVec);

    void onActionTimerTimeout();
    void startActionTimer();
    void stopActionTimer();
    void addRoundItem(const TDZJZ_ROUNDITEM &item);

private:
    std::atomic<bool> m_timerRunning;
    std::vector<TDZJZ_ROUNDITEM> m_roundItemCache;
    intertime m_startTime;
    std::thread m_actionTimerThread;
    std::chrono::time_point<std::chrono::system_clock> m_lastAddTime;
    std::mutex m_cacheMutex;
};