/**
 * @file protdb.h
 * @author 巩拓
 * @brief
 * @version 0.1
 * @date 2023-04-21
 *
 * @copyright Copyright (c) 2023
 *
 */
#if !defined(_PROTDB_H)
#define _PROTDB_H

#include <qobject.h>
#include "protdef.h"
#include "sybase.h"

class CProtDB : public QObject
{
    Q_OBJECT
public:
    CProtDB();
    virtual ~CProtDB();

public:
    // 连接数据库服务器
    int StartDB();
    int Isql(char *isqlcmd, void **pBuff);

    // 装载厂站参数表
    // 调用此函数后请不要调用LoadPara()
    // 而是直接调用LoadPcProtPara()来装载指定厂站的微机保护参数表
    // 注意!!!!! 每次调用此函数后都必需再次调用LoadPcProtPara()
    int LoadStationPara();
    int LoadRtuPara(); // 装载RTU参数表
    // 装载微机保护参数表
    // 注意!!!!! 调用此函数前请先调用LoadRtuPara()
    // name = NULL 装载所有RTU的微机保护参数
    // 只装载指定厂站的微机保护参数
    int LoadPcProtPara();

    const RTU_PARA *GetRtuPara() { return m_pRtuPara; } // 获取有微机保护的RTU的参数
    int GetRtuNum() { return m_nRtuNum; }               // 获取有保护参数的RTU数

    // rtu_no: RTU号
    // pprot_num: 指向返回微机保护个数整数的指针
    // 返回值：指向属于此RTU的微机保护参数
    const PCPROT_PARA *GetPcProtPara(short rtu_no, int *pcprot_num);

    static bool DeleteDZRecords(short nRtuNo, short addr, short groupno);
    static void SaveDZRecords(std::vector<DZJZ_FIXVALUE> &recordList);

protected:
    int m_dbStatus;                   // 数据库状态
    int m_nRtuNum;                    // 有微机保护参数的RTU个数
    RTU_PARA *m_pRtuPara;             // RTU参数
    int m_nStationNum;                // 有微机保护参数的厂站个数
    CHZ_PARA *m_pStationPara;         // 厂站参数
    int *m_protNumArray;              // 保存按RTU分类的微机保护参数个数的数组
    P_PCPROT_PARA *m_pcProtParaArray; // 保存按RTU分类的微机保护参数的指针数组

private:
    void FreeProtPara();
};

#endif
