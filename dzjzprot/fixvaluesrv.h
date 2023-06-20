/**
 * @file fixvaluesrv.h
 * @author ����
 * @brief ���ļ�Ϊ��ֵ�ٻ��������ͷ�ļ�
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

// �����ٻ�������
#define CALL_TYPE_AREANO 0          // ����
#define CALL_TYPE_DZ 1              // ��ֵ
#define CALL_TYPE_RYB 2             // ��ѹ��
#define CALL_TYPE_PARA 3            // װ�ò���

#define FIX_CALL_SUCCESS 0          // �ɹ�
#define FIX_CALL_OVERTIME -1        // ��ʱ
#define FIX_CALL_ERROR -2           // ʧ��
#define FIX_CALL_SEND_ERROR -3      // ����ʧ��
#define FIX_CALL_PROT_PARA_ERROR -4 // ������������
#define FIX_CALL_PROCOL_ERROR -5    // ��Լ����

#define FIX_CALL_WAIT_TIME 30       // �ȴ�ʱ��

/**
 * @brief ��ֵ�ٻ�������
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
    CProtocol *m_poCallProtocol; // ��ֵ�ٻ���Լ����
    CProtApp *m_pApp;            // ��ԼӦ�ö���

    int m_nRtuNo;                // �ٻ���RTU��
};

#endif