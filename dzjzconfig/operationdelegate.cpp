#include "operationdelegate.h"

#include <QDebug>
#include <QPushButton>
#include <QStylePainter>

OperationDelegate::OperationDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

QRect OperationDelegate::buttonRect(const QStyleOptionViewItem &option, const QString &text, int buttonIndex) const
{
    QFontMetrics fm(option.font);
    int buttonWidth = fm.width(text) + 10;

    int buttonLeft = option.rect.left() + buttonIndex * (buttonWidth + 10);
    return QRect(buttonLeft, option.rect.top(), buttonWidth, option.rect.height());
}

void OperationDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionButton detailButton;
    QStyleOptionButton deleteButton;
    QStyleOptionButton modifyButton;

    detailButton.text = QString::fromLocal8Bit("详情");
    deleteButton.text = QString::fromLocal8Bit("删除");
    modifyButton.text = QString::fromLocal8Bit("修改");

    detailButton.rect = buttonRect(option, detailButton.text, 0);
    deleteButton.rect = buttonRect(option, deleteButton.text, 1);
    modifyButton.rect = buttonRect(option, modifyButton.text, 2);

    // Set the button colors
    painter->setBrush(QColor("#008B8B"));
    painter->setPen(QColor("#008B8B"));
    painter->drawRoundedRect(detailButton.rect, 5, 5);

    painter->setBrush(QColor("#B22222"));
    painter->setPen(QColor("#B22222"));
    painter->drawRoundedRect(deleteButton.rect, 5, 5);

    painter->setBrush(QColor("#00688B"));
    painter->setPen(QColor("#00688B"));
    painter->drawRoundedRect(modifyButton.rect, 5, 5);

    painter->setPen(Qt::white);
    painter->drawText(detailButton.rect, Qt::AlignCenter, detailButton.text);
    painter->drawText(deleteButton.rect, Qt::AlignCenter, deleteButton.text);
    painter->drawText(modifyButton.rect, Qt::AlignCenter, modifyButton.text);
}

// 编辑事件
bool OperationDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

        int x = mouseEvent->x();
        int y = mouseEvent->y();

        if (buttonRect(option, QString::fromLocal8Bit("详情"), 0).contains(mouseEvent->pos()))
        {
            qDebug() << "detail clicked";
            emit detailButtonClicked(index);
        }
        else if (buttonRect(option, QString::fromLocal8Bit("删除"), 1).contains(mouseEvent->pos()))
        {
            qDebug() << "delete clicked";
            emit deleteButtonClicked(index);
        }
        else if (buttonRect(option, QString::fromLocal8Bit("修改"), 2).contains(mouseEvent->pos()))
        {
            qDebug() << "modify clicked";
            emit modifyButtonClicked(index);
        }
    }

    return true;
}

QSize OperationDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(150, 30); // 返回适当的大小
}
