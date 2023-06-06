#include <QApplication>
#include <QTextCodec>

#include "dflogger.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // ��־
    DFLogger::instance()->setOStreamAppender();

    // ���ı���
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312")); // ����ȱʡ������������
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));   // ����ȱʡ������������
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));       //

    MainWindow w;
    w.show();
    return a.exec();
}
