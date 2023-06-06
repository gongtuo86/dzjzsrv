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

std::atomic<bool> checkStop(false); // 共享的标识变量

/**
 * @brief 判断进程是否已经存在
 *
 * @return true
 * @return false
 */
bool isProcExist()
{
    const QString serverName = "dzjzdzcallsrv";
    QLocalSocket socket;
    socket.connectToServer(serverName);

    // 如果连接成功，说明已经有一个实例在运行
    if (socket.waitForConnected(500))
    {
        DFLOG_ERROR("An instance of the application is already running.");
        return true;
    }

    // 否则，创建一个本地服务器
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
 * @brief 父进程检测
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
 * @brief	程序入口主函数
 *
 * @param	argc, 参数个数
 * @param	argv, 参数内容
 * @return	无
 * @see
 *
 */
int main(int argc, char *argv[])
{
    // 终端信号
    //    sigset(SIGTTOU,SIG_IGN);
    //    sigset(SIGTTIN,SIG_IGN);
    //    sigset(SIGTSTP,SIG_IGN);

    //    sigset(SIGPIPE,SIG_IGN);
    //    sigset(SIGTSTP,SIG_IGN);
    // 关闭终端信号
    //    sigset(SIGHUP, SIG_IGN);

    // QCoreApplication app(argc, argv);

    // 中文编码
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312")); // 设置缺省解码字体类型
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));   // 设置缺省解码字体类型
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));       //

    proc_man procman;
    if (procman.is_proc_exist("dzjzprotsrv"))
    {
        DFLOG_INFO("dzjzprotsrv have already exist!");
        return FALSE;
    }
    procman.start_proc("dzjzprotsrv", "低周减载定值召唤服务", "", NULL);

    if (!procman.CheckParentProc())
    {
        DFLOG_INFO("dzjzprotsrv: procman.CheckParentProc returning 0");
        exit(1);
    }

    // 启动检测父进程线程
    std::thread threadCheckParent(proc_checkparent_thread, nullptr);

    // 日志
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
    DFLOG_INFO("日志路径: %s", strLogPath.toLocal8Bit().data());

    DFLogger::instance()->setRollingFileAppender(strLogPath.toStdString());

    int intervalMinutes = 1; // 默认间隔为1分钟
    // 解析命令参数
    for (int i = 1; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "-time") == 0 && i + 1 < argc)
        {
            intervalMinutes = std::atoi(argv[i + 1]);
        }
        else if (std::strcmp(argv[i], "-manual") == 0)
        {
            DFLOG_DEBUG("[低周减载定值召唤服务]:立即执行");
            executeBatch();
            checkStop = true;
            if (threadCheckParent.joinable())
            {
                threadCheckParent.join();
            }
            return 0;
        }
    }

    DFLOG_DEBUG("[低周减载定值召唤服务]:定期执行 intervalMinutes: %d", intervalMinutes);

    net_cfg netcfg;
    while (true)
    {
        if (!netcfg.am_I_main_server(SCADA_SERVER))
        {
            DFLOG_INFO("非主服务器，不启动定值召唤服务");
            continue;
        }
        // 立即召唤
        executeBatch();

        // 休眠指定的时间间隔
        std::this_thread::sleep_for(std::chrono::minutes(intervalMinutes));
    }

    checkStop = true;
    if (threadCheckParent.joinable())
    {
        threadCheckParent.join();
    }

    return 0;
}
