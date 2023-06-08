#ifndef DMDELEGATE_H
#define DMDELEGATE_H

#include <QApplication>
#include <QPainter>
#include <QStyledItemDelegate>

template <typename T>
class DMDelegateBase : public QStyledItemDelegate
{
public:
    DMDelegateBase(QMap<T, QString> &valueMap, QObject *parent = 0)
        : QStyledItemDelegate(parent), m_valueMap(valueMap) {}

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QStyleOptionViewItem opt = option;
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
        else
        {
            painter->setPen(opt.palette.text().color());
        }

        painter->drawText(opt.rect, Qt::AlignLeft | Qt::AlignVCenter, text);
        painter->restore();
    }

    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        T value = qvariant_cast<T>(index.model()->data(index, Qt::EditRole));
        QString text = m_valueMap.value(value, QString::fromLocal8Bit(""));

        QFontMetrics metrics(option.font);
        int textWidth = metrics.width(text);
        int textHeight = metrics.height();

        return QSize(textWidth, textHeight);
    }

private:
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

#endif
