#ifndef DMDELEGATE_H
#define DMDELEGATE_H

#include <QApplication>
#include <QPainter>
#include <QStyledItemDelegate>
#include "dflogger.h"

template <typename T>
class DMDelegateBase : public QStyledItemDelegate
{
public:
    DMDelegateBase(QMap<T, QString> &valueMap, QObject *parent = 0)
        : QStyledItemDelegate(parent), m_valueMap(valueMap) {}

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QStyleOptionViewItemV4 opt = option;
        initStyleOption(&opt, index);

        T value = qvariant_cast<T>(index.model()->data(index, Qt::EditRole));
        QString text = m_valueMap.value(value, QString::fromLocal8Bit(""));

        painter->save();
        painter->setClipRect(opt.rect);

        if (option.state & QStyle::State_Selected)
        {
            painter->fillRect(option.rect, option.palette.highlight());
            painter->setPen(opt.palette.highlightedText().color());
        }
        else if (option.state & QStyle::State_MouseOver)
        {
            painter->fillRect(option.rect, option.palette.midlight());
            painter->setPen(opt.palette.text().color());
        }
        else
        {
            painter->setPen(opt.palette.text().color());
        }

        opt.text = text;
        QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);

        painter->restore();
    }

    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        T value = qvariant_cast<T>(index.model()->data(index, Qt::EditRole));
        QString text = m_valueMap.value(value, QString::fromLocal8Bit(""));

        QFontMetrics metrics(option.font);
        int textWidth = metrics.width(text) + 10;
        int textHeight = metrics.height() + 10;

        return QSize(textWidth, textHeight);
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

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QStyleOptionViewItemV4 opt = option;
        this->initStyleOption(&opt, index);

        QStringList idList = index.model()->data(index, Qt::EditRole).toString().split(",");
        // DFLOG_DEBUG("idList=%s", index.model()->data(index, Qt::EditRole).toString().toLocal8Bit().data());
        QStringList nameList;
        for (const QString &id : idList)
        {
            // DFLOG_DEBUG("id=%s name=%s", id.toLocal8Bit().data(), this->m_valueMap.value(id, "").toLocal8Bit().data());
            nameList.append(this->m_valueMap.value(id, QString::fromLocal8Bit("")));
        }
        QString text = nameList.join("\n");
        // DFLOG_DEBUG("nameList=%s", text.toLocal8Bit().data());

        painter->save();
        painter->setClipRect(opt.rect);

        if (option.state & QStyle::State_Selected)
        {
            painter->fillRect(option.rect, option.palette.highlight());
            painter->setPen(opt.palette.highlightedText().color());
        }
        else if (option.state & QStyle::State_MouseOver)
        {
            painter->fillRect(option.rect, option.palette.midlight());
            painter->setPen(opt.palette.text().color());
        }
        else
        {
            painter->setPen(opt.palette.text().color());
        }

        painter->drawText(opt.rect, Qt::TextWordWrap, text);

        // opt.text = text;
        // QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);

        painter->restore();
    }

    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QStringList idList = index.model()->data(index, Qt::EditRole).toString().split(",");
        QStringList nameList;
        for (const QString &id : idList)
        {
            nameList.append(this->m_valueMap.value(id, QString::fromLocal8Bit("")));
        }
        QString text = nameList.join("\n");

        QFontMetrics metrics(option.font);
        QStringList lines = text.split("\n");
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

#endif
