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

    detailButton.text = QString::fromLocal8Bit("����");
    deleteButton.text = QString::fromLocal8Bit("ɾ��");
    modifyButton.text = QString::fromLocal8Bit("�޸�");

    detailButton.rect = buttonRect(option, detailButton.text, 0);
    deleteButton.rect = buttonRect(option, deleteButton.text, 1);
    modifyButton.rect = buttonRect(option, modifyButton.text, 2);

    painter->fillRect(detailButton.rect, Qt::blue);  // ʹ�ú�ɫ������Ӱ�ť�ı���
    painter->fillRect(deleteButton.rect, Qt::red);   // ʹ����ɫ���ɾ����ť�ı���
    painter->fillRect(modifyButton.rect, Qt::green); // ʹ����ɫ����޸İ�ť�ı���

    QPalette palette = QApplication::style()->standardPalette();
    palette.setColor(QPalette::ButtonText, Qt::white);

    detailButton.palette = palette;
    deleteButton.palette = palette;
    modifyButton.palette = palette;

    QApplication::style()->drawControl(QStyle::CE_PushButtonLabel, &detailButton, painter);
    QApplication::style()->drawControl(QStyle::CE_PushButtonLabel, &deleteButton, painter);
    QApplication::style()->drawControl(QStyle::CE_PushButtonLabel, &modifyButton, painter);
}

// �༭�¼�
bool OperationDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

        int x = mouseEvent->x();
        int y = mouseEvent->y();

        if (buttonRect(option, QString::fromLocal8Bit("����"), 0).contains(mouseEvent->pos()))
        {
            qDebug() << "detail clicked";
            emit detailButtonClicked(index);
        }
        else if (buttonRect(option, QString::fromLocal8Bit("ɾ��"), 1).contains(mouseEvent->pos()))
        {
            qDebug() << "delete clicked";
            emit deleteButtonClicked(index);
        }
        else if (buttonRect(option, QString::fromLocal8Bit("�޸�"), 2).contains(mouseEvent->pos()))
        {
            qDebug() << "modify clicked";
            emit modifyButtonClicked(index);
        }
    }

    return true;
}

QSize OperationDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(150, 30); // �����ʵ��Ĵ�С
}
