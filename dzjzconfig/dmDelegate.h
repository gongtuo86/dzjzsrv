#ifndef DMDELEGATE_H
#define DMDELEGATE_H

#include <QApplication>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QDateTime>
#include <QVariant>

#include "dflogger.h"

template <typename T>
class DMDelegateBase : public QStyledItemDelegate
{
public:
    DMDelegateBase(QMap<T, QString> &valueMap, QObject *parent = 0)
        : QStyledItemDelegate(parent), m_valueMap(valueMap) {}

    QString displayText(const QVariant &value, const QLocale &locale) const override
    {
        Q_UNUSED(locale);

        T val = qvariant_cast<T>(value);
        return m_valueMap.value(val, QString::fromLocal8Bit(""));
    }

protected:
    QMap<T, QString> &m_valueMap;
};

class DMDelegateInt : public DMDelegateBase<int>
{
    Q_OBJECT

public:
    DMDelegateInt(QMap<int, QString> &valueMap, QObject *parent = 0)
        : DMDelegateBase<int>(valueMap, parent) {}
};

class DMDelegateString : public DMDelegateBase<QString>
{
    Q_OBJECT

public:
    DMDelegateString(QMap<QString, QString> &valueMap, QObject *parent = 0)
        : DMDelegateBase<QString>(valueMap, parent) {}
};

template <typename T>
class DMDelegateMulti : public DMDelegateBase<T>
{
public:
    DMDelegateMulti(QMap<T, QString> &valueMap, QObject *parent = 0)
        : DMDelegateBase<T>(valueMap, parent) {}

    QString displayText(const QVariant &value, const QLocale &locale) const override
    {
        Q_UNUSED(locale);

        QStringList idList = value.toString().split(",");
        // DFLOG_DEBUG("idList=%s", idList.join(",").toLocal8Bit().data());
        QStringList nameList;
        for (const QString &id : idList)
        {
            T key = QVariant(id).value<T>();
            // DFLOG_DEBUG("key=%s value=%s", id.toLocal8Bit().data(), this->m_valueMap.value(key, QString::fromLocal8Bit("")).toLocal8Bit().data());
            nameList.append(this->m_valueMap.value(key, QString::fromLocal8Bit("")));
        }
        return nameList.join(", ");
    }

    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QString text = displayText(index.model()->data(index, Qt::EditRole), QLocale());

        QFontMetrics metrics(option.font);
        QStringList lines = text.split(", ");
        int totalHeight = 0;
        int maxWidth = 0;
        for (const QString &line : lines)
        {
            QRect rect = metrics.boundingRect(line);
            totalHeight += rect.height();
            if (rect.width() > maxWidth)
            {
                maxWidth = rect.width() + 10;
            }
        }

        return QSize(maxWidth, totalHeight);
    }
};

class TimestampDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    TimestampDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    QString displayText(const QVariant &value, const QLocale &locale) const override
    {
        Q_UNUSED(locale);

        int timestamp = value.toInt();
        if (timestamp == 0)
            return "";
        // DFLOG_DEBUG("timestamp: %d", timestamp);
        QDateTime dateTime = QDateTime::fromTime_t(timestamp);
        return dateTime.toString("yyyy-MM-dd hh:mm:ss");
    }
};

#endif
