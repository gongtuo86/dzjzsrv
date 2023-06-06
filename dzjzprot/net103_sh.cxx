/**
 * @file net103_sh.cxx
 * @author 巩拓
 * @brief 保信103
 * @version 0.1
 * @date 2023-04-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "net103_sh.h"
#include "tablemap_ch.h"
#include "protapp.h"

/**
 * @brief 构造函数
 * 
 */
CNet103DB_SH::CNet103DB_SH()
{
    m_nValueInfoNum = 0;
    m_pValueInfo = NULL;
}

/**
 * @brief 析构函数
 * 
 */
CNet103DB_SH::~CNet103DB_SH()
{
    if (m_pValueInfo)
        free(m_pValueInfo);
}

/**
 * @brief 加载保护信息
 * 
 * @return int 
 */
int CNet103DB_SH::LoadValueInfo(const PCPROT_PARA* pProt)
{
    if( pProt == NULL )
        return -1;

    char isqlcmd[100] = "";
    int ret = 0;
    if (m_pValueInfo)
        free(m_pValueInfo);
    m_pValueInfo = NULL;
    m_nValueInfoNum = 0;

    sprintf(isqlcmd, "select * from %s where 装置类型=%d and 组号=%d and 厂站代码='%s' order by 序号,位号",
            SOUTHNET103_SET_VALUE_TAB, pProt->type1, pProt->addr2, pProt->station);
    ret = Isql(isqlcmd, (void **)&m_pValueInfo);
    if (ret < 0)
    {
        m_pValueInfo = NULL;
        m_nValueInfoNum = 0;
    }
    else
        m_nValueInfoNum = ret;
    return ret;
}

/**
 * @brief 构造函数
 *
 */
CNet103_SH::CNet103_SH() : CProtocol()
{
    m_iFrameCount = 0;
    m_nValueInfoNum = 0;
    m_pValueInfo = NULL;
    m_isEnd = 0;
    m_recOffset = 0;
    m_nCurValueNum = 0;
    m_totalValueNum = 0;
}

/**
 * @brief 析构函数
 * 
 */
CNet103_SH::~CNet103_SH()
{
}

/**
 * @brief 定值召唤区号
 *
 * @param iBoardNo
 * @param iAreaNo
 * @param ucDevPara
 * @param pcSecDev
 * @param pcDev
 * @return int <0 失败 >=0 成功
 */
int CNet103_SH::SendCallAreaNoMail()
{
    int ret = 0;
    m_isEnd = 0;
    m_recOffset = 0;
    m_nCurValueNum = 0;
    m_totalValueNum = 0;
    m_iFrameCount = 0;
    m_bRecvOver = false;

    ret = m_DB.LoadValueInfo(&m_pExchange->prot_para);
    m_nValueInfoNum = m_DB.GetValueNum();
    m_pValueInfo = m_DB.GetValueInfo();

    if (m_nValueInfoNum <= 0 || m_pValueInfo == NULL)
    {
        DFLOG_ERROR("周期召唤定值召唤当前定值个数小于等于0!\n");
        return -1;
    }

    SetProcState(STATE_LOADVALUE);

    WORD txasduaddr;
    W_AsduAddr_SH asduaddr;

    asduaddr.AsduAddr.AsduAddr.AREANO = 1;
    asduaddr.AsduAddr.AsduAddr.CPU = 1;
    asduaddr.AsduAddr.AsduAddr.DEVADDR = m_pExchange->prot_para.type1; // 0-读子站的,其他读装置的
    txasduaddr = asduaddr.AsduAddr.WAsduAddr;

    int txcount = 0;
    m_pExchange->send[txcount++] = 0x15; // 类型标识
    m_pExchange->send[txcount++] = 0x81; // 可变结构限定词
    m_pExchange->send[txcount++] = 0x2a; // 传送原因
    m_pExchange->send[txcount++] = (BYTE)(txasduaddr % 256);
    m_pExchange->send[txcount++] = (BYTE)(txasduaddr / 256);

    m_pExchange->send[txcount++] = 0xfe;                                 // 功能类型
    m_pExchange->send[txcount++] = 0xf1;                                 // 信息序号
    m_pExchange->send[txcount++] = 0x00;                                 // 返回信息标识符
    m_pExchange->send[txcount++] = 0x01;                                 // 通用分类标识数目 nog
    m_pExchange->send[txcount++] = (BYTE)(m_pExchange->prot_para.addr2); // 组号
    m_pExchange->send[txcount++] = 0x00;                                 // 条目号
    m_pExchange->send[txcount++] = 0x01;                                 // 描述类别(KOD)

    m_pExchange->lens = txcount;

    SendMsgToAfert();

    return 1;
}

/**
 * @brief 定时执行函数
 * 
 * @param nIDEvent 
 */
void CNet103_SH::OnTimer(int nIDEvent)
{
    CProtocol::OnTimer(nIDEvent);
    ReceiveMail();
}

/**
 * @brief 收到报文
 * 
 */
void CNet103_SH::ReceiveMail()
{
    if (!HaveMail())
        return;

    m_bHaveMail = false;

    if (m_isEnd || !IsMine())
        return;

    BYTE tmpTypeID, tmpINF;
    tmpTypeID = m_pExchange->recv[0];
    tmpINF = m_pExchange->recv[6];

    switch (tmpTypeID) // 类型标识
    {
    case 0x0a:
        if (tmpINF == 0xf1) // 定值
        {
            RecValue();
            if (m_isEnd)
            {
                setRecvOver();
            }
        }
        break;
    default:
        DFLOG_ERROR("CNet103_SH::ReceiveMail() error type");
        break;
    }
}

/**
 * @brief 是否是我的报文
 * 
 * @return true 
 * @return false 
 */
bool CNet103_SH::IsMine()
{
    ushort tmpaddr;
    W_AsduAddr_SH asduaddr;
    BYTE devaddr;

    tmpaddr = m_pExchange->recv[3] + m_pExchange->recv[4] * 256;
    asduaddr.AsduAddr.WAsduAddr = tmpaddr;
    devaddr = asduaddr.AsduAddr.AsduAddr.DEVADDR;

    if (devaddr != m_pExchange->prot_para.type1)
    {
        DFLOG_WARN("CNet103_SH::IsMine() error devaddr");
        return false;
    }
    return true;
}

/**
 * @brief 记录定值
 * 
 */
void CNet103_SH::RecValue()
{
    // 0 address
    // 8 通用分类标识数目NOG
    // 9 通用分类标识序号(GIN_H)
    // 10 通用分类标识序号(GIN_L
    // 11 描述类别(KOD)
    // 12 数据类型(DATATYPE)
    // 13 数据宽度(DATASIZE)
    // 14 数目(NUMBER)
    // 15 通用分类标识数据 GID
    unsigned char ngd;
    ngd = m_pExchange->recv[8];

    unsigned char tmpDataSize, tmpDataType, tmpNumber;
    int no, i, ii, readOffSet, tmpLen;
    int ginSerial, ginGroup;
    bool ret;
    char Text[100] = "";
    no = ngd & 0x3f;
    // 取得定值的个数
    m_nCurValueNum += no;
    int tmpInt = ngd >> 7; // 后续状态位
    if (tmpInt == 0)
        m_isEnd = 1;
    else
        m_isEnd = 0;
    readOffSet = 9;
    for (i = 0; i < no; i++)
    {
        // 组号
        ginGroup = m_pExchange->recv[readOffSet];
        // 条目号
        ginSerial = m_pExchange->recv[readOffSet + 1];

        tmpDataType = m_pExchange->recv[readOffSet + 3];
        tmpDataSize = m_pExchange->recv[readOffSet + 4];
        tmpNumber = m_pExchange->recv[readOffSet + 5];
        // 最多只能有一个定值
        tmpNumber = tmpNumber & 0x01;
        // 此处的6表示GIN_H~NUMBER的字节个数
        tmpLen = 6 + tmpDataSize * tmpNumber;
        // 由于可能多桢送定值，将收到的报文暂存在发送缓冲区
        memcpy(m_pExchange->send + m_recOffset, m_pExchange->recv + readOffSet, tmpLen);
        m_recOffset += tmpLen;
        readOffSet += tmpLen;
    }
    m_iFrameCount++;
    if (!m_isEnd)
    {
        return;
    }
    DFLOG_INFO("received NO.%d frame,it is over!", m_iFrameCount);

    // 接收结束，开始处理定值信息
    memset(m_pFixedValue, 0, sizeof(FIXED_VALUE) * MAX_VALUE_NUM); // 定值信息
    memset(m_pBoardInfo, 0, sizeof(m_pBoardInfo));
    readOffSet = 0;
    unsigned char tuch[4];
    unsigned char tmpStr[20];
    memset(tmpStr, 0, 20);
    BYTE yxval;
    for (i = 0; i < m_nCurValueNum; i++)
    {
        // 组号
        ginGroup = m_pExchange->send[readOffSet];
        // 条目号
        ginSerial = m_pExchange->send[readOffSet + 1];

        tmpDataType = m_pExchange->send[readOffSet + 3];
        tmpDataSize = m_pExchange->send[readOffSet + 4];
        tmpNumber = m_pExchange->send[readOffSet + 5];
        // 最多只能有一个定值
        tmpNumber = tmpNumber & 0x01;
        tmpLen = 6 + tmpDataSize * tmpNumber;

        short tmptype;
        tmptype = FindVType(ginGroup, ginSerial, m_pFixedValue + m_totalValueNum);
        // DFLOG_INFO("定值[%d] 组号:%d 序号:%d 类型:%d 长度:%d 定值类型:%d", i, ginGroup, ginSerial, tmpDataType, tmpDataSize, tmptype);
        if (tmptype < 0)
        {
            readOffSet += tmpLen;
            continue;
        }
        // 控制字
        if (tmptype == ONOFF_VALUE) // 20050629 dcy
        {
            if (tmpDataSize == 1) // 1个字节 一个定值
            {
                memcpy((void *)tuch, (void *)(m_pExchange->send + readOffSet + 6), 1);
                BYTE temp = *((BYTE *)tuch);
                FindValue(ginGroup, ginSerial, 0, m_pFixedValue + m_totalValueNum); // 定值描述
                (m_pFixedValue + m_totalValueNum)->no = ginSerial;
                if (tmpDataType == 9)
                {
                    if ((temp & 0x03) == 0x02)
                    {
                        yxval = 1;
                    }
                    else
                        yxval = 0;
                }
                else
                    yxval = (temp & 0x01);
                (m_pFixedValue + m_totalValueNum)->value = yxval;
                m_totalValueNum++;
            }
            else if (tmpDataSize == 2) // 双字节
            {
#ifdef __DF_MSB
                SunShort(m_pExchange->send + readOffSet + 6, (unsigned char *)tuch);
#else
                memcpy((void *)tuch, (void *)(m_pExchange->send + readOffSet + 6), 2);
#endif
                WORD temp = *((WORD *)tuch);
                for (ii = 0; ii < 16; ii++)
                {
                    ret = FindValue(ginGroup, ginSerial, ii, m_pFixedValue + m_totalValueNum); // 定值描述
                    if (!ret)
                        continue;
                    (m_pFixedValue + m_totalValueNum)->no = ginSerial;
                    if (temp & 0x01)
                        (m_pFixedValue + m_totalValueNum)->value = 1;
                    else
                        (m_pFixedValue + m_totalValueNum)->value = 0;
                    temp = temp >> 1;
                    m_totalValueNum++;
                }
            }
            else if (tmpDataSize == 4) // 四字节-取后两个字节
            {
#ifdef __DF_MSB
                SunFloat(m_pExchange->send + readOffSet + 6, (unsigned char *)tuch);
#else
                memcpy((void *)tuch, (void *)(m_pExchange->send + readOffSet + 6), 4);
#endif
                float tmpf = *((float *)tuch);
                unsigned short temp;
                temp = (unsigned short)(short)tmpf;
                for (ii = 0; ii < 16; ii++)
                {
                    ret = FindValue(ginGroup, ginSerial, ii, m_pFixedValue + m_totalValueNum); // 定值描述
                    if (!ret)
                        continue;
                    (m_pFixedValue + m_totalValueNum)->no = ginSerial;
                    if (temp & 0x01)
                        (m_pFixedValue + m_totalValueNum)->value = 1;
                    else
                        (m_pFixedValue + m_totalValueNum)->value = 0;
                    temp = temp >> 1;
                    m_totalValueNum++;
                }
            }
        }
        else
        {                                                                       // 其它类型定值
            FindValue(ginGroup, ginSerial, 0, m_pFixedValue + m_totalValueNum); // 定值描述
            (m_pFixedValue + m_totalValueNum)->no = ginSerial;
            if (tmpDataSize == 4)
            {
#ifdef __DF_MSB
                SunFloat(m_pExchange->send + readOffSet + 6, (unsigned char *)tuch);
#else
                memcpy((void *)tuch, (void *)(m_pExchange->send + readOffSet + 6), 4);
#endif

                switch (tmpDataType)
                {
                case 1: // ASCII8位码
                case 2: // 成组8位串
                    (m_pFixedValue + m_totalValueNum)->value = (float)(m_pExchange->send[readOffSet + 9] & 0x01);
                    m_totalValueNum++;
                    break;
                case 3: // unit
                    (m_pFixedValue + m_totalValueNum)->value = (float)*((unsigned int *)tuch);
                    m_totalValueNum++;
                    break;
                case 4: // int
                    (m_pFixedValue + m_totalValueNum)->value = (float)*((int *)tuch);
                    m_totalValueNum++;
                    break;
                case 7:
                case 8: // R32.23
                    (m_pFixedValue + m_totalValueNum)->value = *((float *)tuch);
                    m_totalValueNum++;
                    break;
                default:
                    (m_pFixedValue + m_totalValueNum)->value = *((unsigned short *)tuch);
                    m_totalValueNum++;
                    break;
                }
            }
            else if (tmpDataSize == 2)
            {
#ifdef __DF_MSB
                SunShort(m_pExchange->send + readOffSet + 6, (unsigned char *)tuch);
#else
                memcpy((void *)tuch, (void *)(m_pExchange->send + readOffSet + 6), 2);
#endif
                switch (tmpDataType)
                {
                case 1:
                case 2: // bit
                    (m_pFixedValue + m_totalValueNum)->value = (float)(m_pExchange->send[readOffSet + 7] & 0x01);
                    m_totalValueNum++;
                    break;
                case 3: // unsigned int
                    (m_pFixedValue + m_totalValueNum)->value = *((unsigned short *)tuch);
                    m_totalValueNum++;
                    break;
                case 4: // int
                    (m_pFixedValue + m_totalValueNum)->value = *((short *)tuch);
                    m_totalValueNum++;
                    break;
                default:
                    break;
                }
            }
            else if (tmpDataSize == 1)
            {
                switch (tmpDataType)
                {
                case 1:
                case 2: // bit
                    (m_pFixedValue + m_totalValueNum)->value = (float)(m_pExchange->send[readOffSet + 6] & 0x01);
                    m_totalValueNum++;
                    break;
                case 3: // unsigned int
                    (m_pFixedValue + m_totalValueNum)->value = (unsigned char)m_pExchange->send[readOffSet + 6];
                    m_totalValueNum++;
                    break;
                case 4: // int
                    (m_pFixedValue + m_totalValueNum)->value = (char)m_pExchange->send[readOffSet + 6];
                    m_totalValueNum++;
                    break;
                default:
                    break;
                }
            }
            else // 字符串cxm20230314
            {
                memcpy((m_pFixedValue + m_totalValueNum)->str_value, m_pExchange->send + readOffSet + 6, tmpDataSize);
                m_totalValueNum++;
            }
        }
        readOffSet += tmpLen;
    }
    m_boardNum = 1;
    m_pBoardInfo->value_num = m_totalValueNum;
    m_catchedNum = m_totalValueNum;

    SetProcState(STATE_LOADVALUE_RETURN);
}

/**
 * @brief 根据定值号找定值信息
 * 
 * @param GroupNo 
 * @param IndexNo 
 * @param BitNo 
 * @param Value 
 * @return true 
 * @return false 
 */
bool CNet103_SH::FindValue(int GroupNo, int IndexNo, int BitNo, FIXED_VALUE *Value) // 根据定值号找定值信息
{
    if (m_pValueInfo == NULL)
        return false;
    int curIndex;
    int i = 0;
    for (i = 0; i < m_nValueInfoNum; i++)
    {
        if (((m_pValueInfo + i)->group_no == GroupNo) && ((m_pValueInfo + i)->no == IndexNo))
        {
            Value->value_type = (char)(m_pValueInfo + i)->type; // 定值类型
            strcpy(Value->desc, (m_pValueInfo + i)->desc);      // 描述
            strcpy(Value->unit, (m_pValueInfo + i)->unit);      // 单位
            curIndex = i;
            break;
        }
    }
    if (i == m_nValueInfoNum)
    {
        return true;
    }
    // 非控制字
    if ((m_pValueInfo + curIndex)->type != ONOFF_VALUE)
        return true;
    // 控制字 剔除没有参数的量
    for (i = curIndex; i < m_nValueInfoNum; i++)
    {
        if (((m_pValueInfo + i)->no == IndexNo) && ((m_pValueInfo + i)->code_loc == BitNo))
        {
            Value->value_type = (char)(m_pValueInfo + i)->type; // 定值类型
            strcpy(Value->desc, (m_pValueInfo + i)->desc);      // 描述
            return true;
        }
    }
    return false;
}

/**
 * @brief 根据定值号找定值类型
 * 
 * @param GroupNo 
 * @param IndexNo 
 * @param Value 
 * @return short 
 */
short CNet103_SH::FindVType(int GroupNo, int IndexNo, FIXED_VALUE *Value)
{
    short ret = -1;
    if (m_pValueInfo == NULL)
        return -1;
    int i = 0;
    for (i = 0; i < m_nValueInfoNum; i++)
    {
        if (((m_pValueInfo + i)->group_no == GroupNo) && ((m_pValueInfo + i)->no == IndexNo))
        {
            ret = (char)(m_pValueInfo + i)->type; // 定值类型
            break;
        }
    }
    if (i == m_nValueInfoNum)
    {
        return -1;
    }
    return ret;
}

/**
 * @brief 生成定值数据
 * 
 * @param pcProt 
 * @param vec 
 */
void CNet103_SH::MakeSaveValue(std::vector<DZJZ_FIXVALUE> &vec)
{
    char value[256] = "", type[20] = "", format[20] = "";

    for (int i = 0; i < m_catchedNum; i++)
    {
        value[0] = type[0] = 0;
        strcpy(format, "%d");

        switch (m_pFixedValue[i].value_type)
        {
        case NUMERICAL_VALUE:
            strcpy(type, "十进制");
            strcpy(format, "%.3f");
            sprintf(value, format, m_pFixedValue[i].value);
            break;
        case ONOFF_VALUE:
            strcpy(type, "两态型");
            sprintf(value, format, (int)m_pFixedValue[i].value);
            break;
        case STR_VALUE:
            strcpy(type, "字串型");
            strcpy(format, "%s");
            if (strlen(m_pFixedValue[i].str_value) > 0)
            {
                sprintf(value, format, m_pFixedValue[i].str_value);
            }
            break;
#ifdef USING_IEC61850_STR
        case INT_VALUE:
            strcpy(type, "整型");
            strcpy(format, "%d");
            sprintf(value, format, (int)m_pFixedValue[i].int_value);
            break;
        case HEX_VALUE_LARGE:
            strcpy(type, "大十六进制");
            strcpy(format, "%X");
            sprintf(value, format, (int)m_pFixedValue[i].int_value);
            break;
#endif
        case AREA_NO:
        case GROUP_NO:
            strcpy(type, "区号/组号");
            sprintf(value, format, (int)m_pFixedValue[i].value);
            break;
        case HEX_VALUE:
            strcpy(type, "十六进制");
            strcpy(format, "%X");
            sprintf(value, format, (int)m_pFixedValue[i].value);
            break;
        default:
            continue;
        }

        DZJZ_FIXVALUE aValue;
        memset(&aValue, 0, sizeof(DZJZ_FIXVALUE));
        aValue.rtuno = m_pExchange->prot_para.rtu_no;
        strcpy(aValue.station, m_pExchange->prot_para.station);
        aValue.gui_yue = m_pExchange->prot_para.gui_yue;
        aValue.type = m_pExchange->prot_para.type1;
        aValue.groupno = m_pExchange->prot_para.addr2;
        aValue.no = m_pFixedValue[i].no;
        strcpy(aValue.value, value);
        strcpy(aValue.desc, m_pFixedValue[i].desc);
        strcpy(aValue.unit, m_pFixedValue[i].unit);
        strcpy(aValue.value_type, type);
        aValue.print();

        vec.push_back(aValue);
    }
}

/**
 * @brief 返回运行定值区号
 * 
 */
int CNet103_SH::GetCurAreaNo()
{
    if (m_catchedNum <= 0 || m_pFixedValue == NULL)
        return 0;

    return (int)m_pFixedValue[0].value;
}

/**
 * @brief 定值召唤
 * 
 * @param iAreaNo 
 * @return int 
 */
int CNet103_SH::SendCallValueMail(int iAreaNo)
{
    int ret = 0;
    m_isEnd = 0;
    m_recOffset = 0;
    m_nCurValueNum = 0;
    m_totalValueNum = 0;
    m_iFrameCount = 0;
    m_bRecvOver = false;

    ret = m_DB.LoadValueInfo(&m_pExchange->prot_para);
    m_nValueInfoNum = m_DB.GetValueNum();
    m_pValueInfo = m_DB.GetValueInfo();

    if (m_nValueInfoNum <= 0 || m_pValueInfo == NULL)
    {
        DFLOG_ERROR("召唤定值召唤当前定值个数小于等于0!\n");
        return -1;
    }

    WORD txasduaddr;
    W_AsduAddr_SH asduaddr;

    asduaddr.AsduAddr.AsduAddr.AREANO = iAreaNo & 0XFF;
    asduaddr.AsduAddr.AsduAddr.CPU = 1;
    asduaddr.AsduAddr.AsduAddr.DEVADDR = m_pExchange->prot_para.type1; // 0-读子站的,其他读装置的
    txasduaddr = asduaddr.AsduAddr.WAsduAddr;

    int txcount = 0;
    m_pExchange->send[txcount++] = 0x15; // 类型标识
    m_pExchange->send[txcount++] = 0x81; // 可变结构限定词
    m_pExchange->send[txcount++] = 0x2a; // 传送原因
    m_pExchange->send[txcount++] = (BYTE)(txasduaddr % 256);
    m_pExchange->send[txcount++] = (BYTE)(txasduaddr / 256);

    m_pExchange->send[txcount++] = 0xfe;                                 // 功能类型
    m_pExchange->send[txcount++] = 0xf1;                                 // 信息序号
    m_pExchange->send[txcount++] = 0x00;                                 // 返回信息标识符
    m_pExchange->send[txcount++] = 0x01;                                 // 通用分类标识数目 nog
    m_pExchange->send[txcount++] = (BYTE)(m_pExchange->prot_para.addr2); // 组号
    m_pExchange->send[txcount++] = 0x00;                                 // 条目号
    m_pExchange->send[txcount++] = 0x01;                                 // 描述类别(KOD)

    m_pExchange->lens = txcount;

    SendMsgToAfert();

    return 1;
}

/**
 * @brief 召唤软压板
 * 
 * @return int 
 */
int CNet103_SH::SendCallRYBMail()
{
    int ret = 0;
    m_isEnd = 0;
    m_recOffset = 0;
    m_nCurValueNum = 0;
    m_totalValueNum = 0;
    m_iFrameCount = 0;
    m_bRecvOver = false;

    ret = m_DB.LoadValueInfo(&m_pExchange->prot_para);
    m_nValueInfoNum = m_DB.GetValueNum();
    m_pValueInfo = m_DB.GetValueInfo();

    if (m_nValueInfoNum <= 0 || m_pValueInfo == NULL)
    {
        DFLOG_ERROR("周期召唤定值召唤当前定值个数小于等于0!\n");
        return -1;
    }

    WORD txasduaddr;
    W_AsduAddr_SH asduaddr;

    asduaddr.AsduAddr.AsduAddr.AREANO = 0;
    asduaddr.AsduAddr.AsduAddr.CPU = 0;
    asduaddr.AsduAddr.AsduAddr.DEVADDR = m_pExchange->prot_para.type1; // 0-读子站的,其他读装置的
    txasduaddr = asduaddr.AsduAddr.WAsduAddr;

    int txcount = 0;
    m_pExchange->send[txcount++] = 0x15; // 类型标识
    m_pExchange->send[txcount++] = 0x81; // 可变结构限定词
    m_pExchange->send[txcount++] = 0x2a; // 传送原因
    m_pExchange->send[txcount++] = (BYTE)(txasduaddr % 256);
    m_pExchange->send[txcount++] = (BYTE)(txasduaddr / 256);

    m_pExchange->send[txcount++] = 0xfe;                                 // 功能类型
    m_pExchange->send[txcount++] = 0xf1;                                 // 信息序号
    m_pExchange->send[txcount++] = 0x00;                                 // 返回信息标识符
    m_pExchange->send[txcount++] = 0x01;                                 // 通用分类标识数目 nog
    m_pExchange->send[txcount++] = (BYTE)(m_pExchange->prot_para.addr2); // 组号
    m_pExchange->send[txcount++] = 0x00;                                 // 条目号
    m_pExchange->send[txcount++] = 0x01;                                 // 描述类别(KOD)

    m_pExchange->lens = txcount;

    SendMsgToAfert();

    return 1;
}
