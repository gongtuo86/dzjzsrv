#ifndef DEVICE_DELEGATE_H
#define DEVICE_DELEGATE_H

#include <QApplication>
#include <QPainter>
#include <QStyledItemDelegate>

class DeviceDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    DeviceDelegate(QMap<QString, QString> &valueMap, QObject *parent = 0)
        : QStyledItemDelegate(parent), m_valueMap(valueMap) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    QMap<QString, QString> &m_valueMap;
};

#endif
