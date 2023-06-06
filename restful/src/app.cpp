#include <QDir>
#include <QString>
#include <QTextCodec>
#include <iostream>

#include "ThreadPool.h"
#include "appcomponent.hpp"
#include "controller/mycontroller.hpp"
#include "dflogger.h"
#include "netdef.h"
#include "oatpp/network/Server.hpp"
#include "sybase.h"

ThreadPool g_threadPool(8);                                              // �̳߳�
std::unordered_map<std::string, std::shared_ptr<WSListener>> g_peerById; // �ͻ���socket
std::mutex g_peerByIdLock;

std::unordered_map<std::string, std::shared_ptr<TaskState>> g_tasks;
std::mutex g_tasksLock;

std::atomic<bool> checkStop(false); // ����ı�ʶ����

void run()
{
    /* Register Components in scope of run() method */
    AppComponent components;

    /* Get router component */
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

    /* Create MyController and add all of its endpoints to router */
    router->addController(std::make_shared<MyController>());

    /* Get connection handler component */
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>,
                    connectionHandler, "http");

    /* Get connection provider component */
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>,
                    connectionProvider);

    /* Create server which takes provided TCP connections and passes them to
     * HTTP connection handler */
    oatpp::network::Server server(connectionProvider, connectionHandler);

    /* Priny info about server port */
    OATPP_LOGI("MyApp", "Server running on port %s",
               connectionProvider->getProperty("port").getData());

    /* Run server */
    server.run();
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
    DFLOG_INFO("dzjzwebrsrv: proc_checkparent_thread start");

    proc_man procman;

    while (!checkStop)
    {
        if (!procman.CheckParentProc())
        {
            checkStop = true;
            DFLOG_INFO("dzjzwebrsrv: procman.CheckParentProc returning 0");
            exit(1);
        }
        usleep(1000 * 1000);
    }

    DFLOG_INFO("dzjzwebrsrv: proc_checkparent_thread end");

    return NULL;
}

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312")); // ����ȱʡ������������
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));   // ����ȱʡ������������
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));       //

    startdb(1, 1); // �������ݿ�

    proc_man procman;
    if (procman.is_proc_exist("dzjzwebrsrv"))
    {
        DFLOG_INFO("dzjzwebrsrv have already exist!");
        return FALSE;
    }
    procman.start_proc("dzjzwebrsrv", "���ܼ���RESTful����", "", NULL);

    if (!procman.CheckParentProc())
    {
        DFLOG_INFO("dzjzwebrsrv: procman.CheckParentProc returning 0");
        exit(1);
    }

    // ��־
    DFLogger::instance()->setOStreamAppender();
    QString strLogPath =
        QString("%1/log/dzjzsrv").arg(qgetenv(RUN_HOME).data());
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
    strLogPath += "/dzjzwebrsrv.log";
    DFLOG_INFO("��־·��: %s", strLogPath.toAscii().data());
    DFLogger::instance()->setRollingFileAppender(strLogPath.toStdString());

    // ������⸸�����߳�
    std::thread threadCheckParent(proc_checkparent_thread, nullptr);

    oatpp::base::Environment::init();

    run();

    oatpp::base::Environment::destroy();

    checkStop = true;
    if (threadCheckParent.joinable())
    {
        threadCheckParent.join();
    }

    closedb();

    return 0;
}