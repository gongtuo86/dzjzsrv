#pragma once

#include <QApplication>

#include "sybase.h"

class AreaDto
{
public:
    int id;
    char name[256];
    quint8 type;
};

class AreaVo
{
public:
    int id;
    char name[256];
    quint8 type;
    char staId[SUBSTATION_LEN];
};

class RoundDto
{
public:
    int id;             // 代码
    char name[256];     // 名称
    int areaId;         // 区域id
    quint8 funcType;    // 功能类型 --低频 -低压
    int roundType;      // 轮次类型 --第几轮
    float fixValue;     // 定值
    float timeValue;    // 延时
    float requirePower; // 应切荷量
};

class DMDto
{
public:
    int id;
    char name[40];
};

class LineDto
{
public:
    char id[POWDEVNAME_LEN];
    char name[DESCRIBE_LEN];
    char staId[SUBSTATION_LEN];
    char staName[DESCRIBE_LEN];
};

class BreakDto
{
public:
    char id[DEF_CODE_LEN];
    char name[DESCRIBE_LEN];
    char staId[SUBSTATION_LEN];
    char staName[DESCRIBE_LEN];
};

class RoundTypeDto
{
public:
    int id;
    char name[256];
};

class RoundItemDto
{
public:
    int id;
    char name[256];
    int areaId;
    int roundId;
    char linkedLine[POWDEVNAME_LEN];
    quint8 loadType;
    quint8 strapPlan;
    char linkedBreak[DEF_CODE_LEN];
};

class SubstationDto
{
public:
    char id[SUBSTATION_LEN];
    char name[DESCRIBE_LEN];
};

class AreaSubStationDto
{
public:
    char staId[SUBSTATION_LEN];
    int areaId;
};
