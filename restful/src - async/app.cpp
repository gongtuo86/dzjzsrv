#include <QDir>
#include <QTextCodec>
#include <iostream>

#include "appcomponent.hpp"
#include "controller/RoomsController.hpp"
#include "dflogger.h"
#include "netdef.h"
#include "oatpp/network/Server.hpp"

void run() {
    // ����������洢Ӧ�ó�����������
    AppComponent components;

    // ����������л�ȡ��һ����Ϊrouter�����������һ��HTTP·���������ڴ���HTTP����
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

    // ������һ����ΪMyController�Ŀ��������󣬲����ÿ����������ж˵���ӵ�·�����С�
    // ��������һ������HTTP������࣬�˵��ǿ������ж���ķ��������ڴ���ͬ������
    router->addController(std::make_shared<RoomsController>());

    // ����������л�ȡ��һ����ΪconnectionHandler�����������һ�����Ӵ����������ڴ��������������
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler, "http");

    // ����������л�ȡ��һ����ΪconnectionHandler�����������һ�����Ӵ����������ڴ��������������
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

    // ������һ�����������󣬲���TCP�����ṩ��������Ӵ�������Ϊ�������ݸ�����������
    oatpp::network::Server server(connectionProvider, connectionHandler);

    // ����������������еĶ˿ںš�OATPP_LOGI��һ���꣬���ڴ�ӡ��־��Ϣ��
    OATPP_LOGI(
        "MyApp", "Server running on port %s", connectionProvider->getProperty("port").getData());

    // ��������������ʼ�����������������һ�����������󵽴��������ʹ�����Ӵ�������������
    server.run();
}

int main(int argc, const char *argv[]) {
    // ���ı���
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));  // ����ȱʡ������������
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));  // ����ȱʡ������������
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));  //

    // ��־
    DFLogger::instance()->setOStreamAppender();
    QString strLogPath = QString("%1/log/dfwebrsrv").arg(qgetenv(RUN_HOME).data());
    QDir logDir;
    if (!logDir.exists(strLogPath)) {
        bool res = logDir.mkdir(strLogPath);
        if (!res) {
            DFLOG_WARN("Failed to create directory %s", strLogPath.toLocal8Bit().data());
            return 0;
        }
    }
    strLogPath += "/dfwebrsrv.log";
    DFLOG_INFO("��־·��: %s", strLogPath.toLocal8Bit().data());
    DFLogger::instance()->setRollingFileAppender(strLogPath.toStdString());

    // ��ʼ������oat++��������־ϵͳ�������������
    oatpp::base::Environment::init();

    run();

    // ���д���������������ڼ䴴���Ķ����������Ѵ�����δ���ٵĶ�����������Щ��Ϣ�����ڼ���ڴ�й©������
    std::cout << "\nEnvironment:\n";
    std::cout << "objectsCount = " << oatpp::base::Environment::getObjectsCount() << "\n";
    std::cout << "objectsCreated = " << oatpp::base::Environment::getObjectsCreated() << "\n\n";

    // ���д�������oat++�������ͷ�������Դ
    oatpp::base::Environment::destroy();

    return 0;
}