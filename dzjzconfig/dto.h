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
    int id;             // ����
    char name[256];     // ����
    int areaId;         // ����id
    quint8 funcType;    // �������� --��Ƶ -��ѹ
    int roundType;      // �ִ����� --�ڼ���
    float fixValue;     // ��ֵ
    float timeValue;    // ��ʱ
    float requirePower; // Ӧ�к���
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
    char pName[DEF_CODE_LEN];
    int deviceId;
    quint8 assocexit;
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

class DeviceDto
{
public:
    int id;
    char name[256];
    quint8 type;
    quint8 funcType;
    int itemNums;
    char staId[SUBSTATION_LEN];
};

class RtuDto
{
public:
    qint16 id;
    char name[40];
    char staId[12];
    char staName[DESCRIBE_LEN];
};

class DeviceParaDto
{
public:
    int id;
    int no;
    char strapId[256];
    char fixValueId[DEF_CODE_LEN];
    char timeValueId[DEF_CODE_LEN];
    char alarmId[256];
    char actionId[DEF_CODE_LEN];
};

class FixValueDto
{
public:
    qint16 rtuNo;
    qint16 type;
    qint16 groupNo;
    qint16 no;
    char name[160];
};

class TMDto
{
public:
    char id[DEF_CODE_LEN];
    char name[DESCRIBE_LEN];
    char staId[SUBSTATION_LEN];
    char staName[DESCRIBE_LEN];
};

class TSDto
{
public:
    char id[DEF_CODE_LEN];
    char name[DESCRIBE_LEN];
    char staId[SUBSTATION_LEN];
    char staName[DESCRIBE_LEN];
    qint16 rtuNo;
    char rtuName[40];
};

class TaskDto
{
public:
    int id;
    char name[256];
    int startTime;
    int endTime;
    int period;
    char areaCond[256];
    char subCond[256];
    char roundCond[256];
};
