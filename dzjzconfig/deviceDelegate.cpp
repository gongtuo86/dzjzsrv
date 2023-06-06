#include "deviceDelegate.h"

void DeviceDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    QString value = index.model()->data(index, Qt::EditRole).toString();
    QString text = QString("[%1]%2").arg(value).arg(m_valueMap.value(value, QString::fromLocal8Bit("δ֪")));

    painter->save();
    painter->setClipRect(opt.rect);

    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, option.palette.highlight());
        painter->setPen(option.palette.highlightedText().color());
    }

    painter->drawText(opt.rect, Qt::AlignLeft | Qt::AlignVCenter, text);
    painter->restore();
}

QSize DeviceDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    QString text = QString("[%1]%2").arg(value).arg(m_valueMap.value(value, QString::fromLocal8Bit("δ֪")));

    QFontMetrics metrics(option.font);
    int textWidth = metrics.width(text);
    int textHeight = metrics.height();

    return QSize(textWidth, textHeight);
}