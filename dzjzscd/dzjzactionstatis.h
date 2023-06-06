#pragma once

#include <vector>
#include <string>

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
    std::string getActionDesc(int time, const std::string &deviceName);
    float getAllNetJudgePower();
    int saveAction(int time, int deviceID, const std::string &deviceName, const std::vector<TDZJZ_ROUNDITEM> &roundItemVec);
    float calcActionJudgePower(const std::vector<TDZJZ_ROUNDITEM> &roundItemVec);
    void saveActionInfo(int time, int deviceID, const std::string &deviceName, const std::vector<TDZJZ_ROUNDITEM> &roundItemVec);
};