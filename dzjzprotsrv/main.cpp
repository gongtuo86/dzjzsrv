#include <QCoreApplication>
#include <QDir>
#include <QLocalServer>
#include <QLocalSocket>
#include <QTextCodec>
#include <atomic>
#include <thread>

#include "dflogger.h"
#include "dzjzprot.h"
#include "netdef.h"
#include "signal.h"

std::atomic<bool> checkStop(false); // ����ı�ʶ����

/**
 * @brief �жϽ����Ƿ��Ѿ�����
 *
 * @return true
 * @return false
 */
bool isProcExist()
{
    const QString serverName = "dzjzdzcallsrv";
    QLocalSocket socket;
    socket.connectToServer(serverName);

    // ������ӳɹ���˵���Ѿ���һ��ʵ��������
    if (socket.waitForConnected(500))
    {
        DFLOG_ERROR("An instance of the application is already running.");
        return true;
    }

    // ���򣬴���һ�����ط�����
    static QLocalServer server;
    server.removeServer(serverName);
    if (!server.listen(serverName))
    {
        DFLOG_ERROR("Failed to create a local server.");
        return true;
    }

    return false;
}

/**
 * @brief �����̼��
 *
 */
#ifdef __unix
void *proc_checkparent_thread(void *p)
#else
UINT proc_checkparent_thread(LPVOID pParam)
#endif
{
    DFLOG_INFO("dzjzprotsrv: proc_checkparent_thread start");

    proc_man procman;

    while (!checkStop)
    {
        if (!procman.CheckParentProc())
        {
            checkStop = true;
            DFLOG_INFO("dzjzprotsrv: procman.CheckParentProc returning 0");
            exit(1);
        }
        // DFLOG_INFO("dzjzprotsrv: procman.CheckParentProc running");
        usleep(1000 * 1000);
    }

    DFLOG_INFO("dzjzprotsrv: proc_checkparent_thread end");

    return NULL;
}

/**
 * @fn		int main(int argc, char* argv[])
 * @brief	�������������
 *
 * @param	argc, ��������
 * @param	argv, ��������
 * @return	��
 * @see
 *
 */
int main(int argc, char *argv[])
{
    // �ն��ź�
    //    sigset(SIGTTOU,SIG_IGN);
    //    sigset(SIGTTIN,SIG_IGN);
    //    sigset(SIGTSTP,SIG_IGN);

    //    sigset(SIGPIPE,SIG_IGN);
    //    sigset(SIGTSTP,SIG_IGN);
    // �ر��ն��ź�
    //    sigset(SIGHUP, SIG_IGN);

    // QCoreApplication app(argc, argv);

    // ���ı���
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312")); // ����ȱʡ������������
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));   // ����ȱʡ������������
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));       //

    proc_man procman;
    if (procman.is_proc_exist("dzjzprotsrv"))
    {
        DFLOG_INFO("dzjzprotsrv have already exist!");
        return FALSE;
    }
    procman.start_proc("dzjzprotsrv", "���ܼ��ض�ֵ�ٻ�����", "", NULL);

    if (!procman.CheckParentProc())
    {
        DFLOG_INFO("dzjzprotsrv: procman.CheckParentProc returning 0");
        exit(1);
    }

    // ������⸸�����߳�
    std::thread threadCheckParent(proc_checkparent_thread, nullptr);

    // ��־
    DFLogger::instance()->setOStreamAppender();
    QString strLogPath = QString("%1/log/dzjzsrv").arg(qgetenv(RUN_HOME).data());
    QDir logDir;
    if (!logDir.exists(strLogPath))
    {
        bool res = logDir.mkdir(strLogPath);
        if (!res)
        {
            DFLOG_WARN("Failed to create directory %s",
                       strLogPath.toLocal8Bit().data());
            return 0;
        }
    }
    strLogPath += "/dzjzprotsrv.log";
    DFLOG_INFO("��־·��: %s", strLogPath.toLocal8Bit().data());

    DFLogger::instance()->setRollingFileAppender(strLogPath.toStdString());

    int intervalMinutes = 1; // Ĭ�ϼ��Ϊ1����
    // �����������
    for (int i = 1; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "-time") == 0 && i + 1 < argc)
        {
            intervalMinutes = std::atoi(argv[i + 1]);
        }
        else if (std::strcmp(argv[i], "-manual") == 0)
        {
            DFLOG_DEBUG("[���ܼ��ض�ֵ�ٻ�����]:����ִ��");
            executeBatch();
            checkStop = true;
            if (threadCheckParent.joinable())
            {
                threadCheckParent.join();
            }
            return 0;
        }
    }

    DFLOG_DEBUG("[���ܼ��ض�ֵ�ٻ�����]:����ִ�� intervalMinutes: %d", intervalMinutes);

    net_cfg netcfg;
    while (true)
    {
        if (!netcfg.am_I_main_server(SCADA_SERVER))
        {
            DFLOG_INFO("��������������������ֵ�ٻ�����");
            continue;
        }
        // �����ٻ�
        executeBatch();

        // ����ָ����ʱ����
        std::this_thread::sleep_for(std::chrono::minutes(intervalMinutes));
    }

    checkStop = true;
    if (threadCheckParent.joinable())
    {
        threadCheckParent.join();
    }

    return 0;
}
