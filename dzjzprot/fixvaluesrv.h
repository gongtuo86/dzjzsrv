/**
 * @file fixvaluesrv.h
 * @author 巩拓
 * @brief 本文件为定值召唤服务类的头文件
 * @version 0.1
 * @date 2023-04-21
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef FIXVALUE_H
#define FIXVALUE_H

#include "dorbase.h"
#include "dzjzprot.h"
#include "protapp.h"
#include "protdb.h"
#include "protocol.h"

// 周期召唤的类型
#define CALL_TYPE_AREANO 0          // 区号
#define CALL_TYPE_DZ 1              // 定值
#define CALL_TYPE_RYB 2             // 软压板
#define CALL_TYPE_PARA 3            // 装置参数

#define FIX_CALL_SUCCESS 0          // 成功
#define FIX_CALL_OVERTIME -1        // 超时
#define FIX_CALL_ERROR -2           // 失败
#define FIX_CALL_SEND_ERROR -3      // 发送失败
#define FIX_CALL_PROT_PARA_ERROR -4 // 保护参数错误
#define FIX_CALL_PROCOL_ERROR -5    // 规约错误

#define FIX_CALL_WAIT_TIME 30       // 等待时间

/**
 * @brief 定值召唤服务类
 *
 */
class _EPROTMMI_CTRL_DLL_EXPORT CFixValueService
{
public:
    CFixValueService();
    ~CFixValueService();
    int executeBatch();
    int executeSingle(int nRtuNo);
    std::string executeInspectionByCond(const std::string &jsonPara, ProgressCallback progressCallback);

private:
    std::vector<DZJZ_DEVICE> GetDZJZDeviceList();
    std::vector<DZJZ_DEVICE> GetDZJZDeviceList(const QString &subStation, int area, int round);
    void reloadRTDB();
    void CreateEvent(int category, int type, int state_value, QString char_info, QString member_name0 = "", QString member_name1 = "", QString group_name = "");
    int SendCallAreaNoMail();
    int SendCallSetValueMail(int iAreaNo);
    int SendCallRYBMail();
    int waitForReply(int timeout);
    QString PrintFixCallErr(int nRet);
    int CallAreaNo();
    int CallDZ(int iAreaNo);
    int CallRYB();

private:
    CProtocol *m_poCallProtocol; // 定值召唤规约对象
    CProtApp *m_pApp;            // 规约应用对象

    int m_nRtuNo;                // 召唤的RTU号
};

#endif