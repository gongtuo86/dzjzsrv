#include <QApplication>
#include <QTextCodec>
#include <QFile>
#include <QTextStream>
#include <QTranslator>
#include <QDebug>

#include "dflogger.h"
#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QStringList qmList = {"alarmapplogin_zh.qm", "qt4_zh_CN.qm"};
    for (const auto& qmPath : qmList)
    {
        QTranslator* translator = new QTranslator();
        if (translator->load(QString("%1/uif/i18n/%2").arg(qgetenv("RUNHOME").data()).arg(qmPath)))
        {
            qApp->installTranslator(translator);
        }
        else
        {
            qDebug() << QString::fromLocal8Bit("翻译文件加载失败:") << QString("%1/uif/i18n/%2").arg(qgetenv("RUNHOME").data()).arg(qmPath);
            delete translator;
        }
    }

    QFile file(":/qdarkstyle/light/lightstyle.qss");
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream stream(&file);
        a.setStyleSheet(stream.readAll());
    }
    else
    {
        qDebug("can not open qss");
    }

    // 日志
    DFLogger::instance()->setOStreamAppender();

    // 中文编码
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312")); // 设置缺省解码字体类型
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));   // 设置缺省解码字体类型
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));       //

    MainWindow w;
    w.showMaximized();

    return a.exec();
}
