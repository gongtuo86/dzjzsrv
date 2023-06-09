#include <QApplication>
#include <QTextCodec>
#include <QFile>
#include <QTextStream>

#include "dflogger.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

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

    // ��־
    DFLogger::instance()->setOStreamAppender();

    // ���ı���
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312")); // ����ȱʡ������������
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));   // ����ȱʡ������������
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));       //

    MainWindow w;
    w.showMaximized();
    return a.exec();
}
