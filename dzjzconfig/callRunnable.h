#ifndef CALLRUNNABLE_H
#define CALLRUNNABLE_H

#include <QObject>

class CFixValueService;

class CallWorkder : public QObject
{
    Q_OBJECT

public:
    CallWorkder(QObject* parent = 0);
    ~CallWorkder();
    void setId(int id);

public slots:
    void call();
    void cancel();

signals:
    void resultReady(int result);

private:
    int m_id;
    bool m_canceled;
};

#endif // CALLRUNNABLE_H
