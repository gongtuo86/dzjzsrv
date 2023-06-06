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

    painter->fillRect(detailButton.rect, Qt::blue);  // 使用红色填充增加按钮的背景
    painter->fillRect(deleteButton.rect, Qt::red);   // 使用绿色填充删除按钮的背景
    painter->fillRect(modifyButton.rect, Qt::green); // 使用蓝色填充修改按钮的背景

    QPalette palette = QApplication::style()->standardPalette();
    palette.setColor(QPalette::ButtonText, Qt::white);

    detailButton.palette = palette;
    deleteButton.palette = palette;
    modifyButton.palette = palette;

    QApplication::style()->drawControl(QStyle::CE_PushButtonLabel, &detailButton, painter);
    QApplication::style()->drawControl(QStyle::CE_PushButtonLabel, &deleteButton, painter);
    QApplication::style()->drawControl(QStyle::CE_PushButtonLabel, &modifyButton, painter);
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
