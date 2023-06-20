#include "callRunnable.h"
#include "fixvaluesrv.h"

#include <QDebug>
#include <QCoreApplication>

CallWorkder::CallWorkder(QObject* parent)
    : QObject(parent), m_canceled(false)
{
}

CallWorkder::~CallWorkder()
{
    qDebug() << "CallWorker destroyed";
}

void CallWorkder::setId(int id)
{
    m_id = id;
}

void CallWorkder::cancel()
{
    qDebug() << "cancel slot called";
    m_canceled = true;
}

void CallWorkder::call()
{
    m_canceled = false;

    qDebug() << "call";

    CFixValueService fixSrv;
    int nRet = fixSrv.executeSingle(m_id);

    QCoreApplication::processEvents();

    if (m_canceled)
        return;

    emit resultReady(nRet);
}
