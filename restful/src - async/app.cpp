#include <QDir>
#include <QTextCodec>
#include <iostream>

#include "appcomponent.hpp"
#include "controller/RoomsController.hpp"
#include "dflogger.h"
#include "netdef.h"
#include "oatpp/network/Server.hpp"

void run() {
    // 组件容器，存储应用程序的所有组件
    AppComponent components;

    // 从组件容器中获取了一个名为router的组件，它是一个HTTP路由器，用于处理HTTP请求。
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

    // 创建了一个名为MyController的控制器对象，并将该控制器的所有端点添加到路由器中。
    // 控制器是一个处理HTTP请求的类，端点是控制器中定义的方法，用于处理不同的请求
    router->addController(std::make_shared<RoomsController>());

    // 从组件容器中获取了一个名为connectionHandler的组件，它是一个连接处理器，用于处理传入的连接请求
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler, "http");

    // 从组件容器中获取了一个名为connectionHandler的组件，它是一个连接处理器，用于处理传入的连接请求
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

    // 创建了一个服务器对象，并将TCP连接提供程序和连接处理器作为参数传递给服务器对象
    oatpp::network::Server server(connectionProvider, connectionHandler);

    // 输出服务器正在运行的端口号。OATPP_LOGI是一个宏，用于打印日志信息。
    OATPP_LOGI(
        "MyApp", "Server running on port %s", connectionProvider->getProperty("port").getData());

    // 启动服务器并开始监听传入的连接请求。一旦有连接请求到达，服务器将使用连接处理器处理请求
    server.run();
}

int main(int argc, const char *argv[]) {
    // 中文编码
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));  // 设置缺省解码字体类型
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));  // 设置缺省解码字体类型
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));  //

    // 日志
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
    DFLOG_INFO("日志路径: %s", strLogPath.toLocal8Bit().data());
    DFLogger::instance()->setRollingFileAppender(strLogPath.toStdString());

    // 初始化环境oat++，包括日志系统、对象计数器等
    oatpp::base::Environment::init();

    run();

    // 这行代码输出程序运行期间创建的对象数量和已创建但未销毁的对象数量。这些信息可用于检测内存泄漏等问题
    std::cout << "\nEnvironment:\n";
    std::cout << "objectsCount = " << oatpp::base::Environment::getObjectsCount() << "\n";
    std::cout << "objectsCreated = " << oatpp::base::Environment::getObjectsCreated() << "\n\n";

    // 这行代码销毁oat++环境，释放所有资源
    oatpp::base::Environment::destroy();

    return 0;
}