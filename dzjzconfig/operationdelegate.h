#ifndef CUSTOMDELEGATE_H
#define CUSTOMDELEGATE_H

#include <QApplication>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionButton>
#include <QStyledItemDelegate>
#include <QPoint>

class OperationDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    OperationDelegate(QObject *parent = 0);
    QRect buttonRect(const QStyleOptionViewItem &option, const QString &text, int buttonIndex) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
    void drawButton(QPainter *painter, const QStyleOptionButton &button, const QColor &normalColor, const QColor &hoverColor) const;

signals:
    void detailButtonClicked(const QModelIndex &index);
    void deleteButtonClicked(const QModelIndex &index);
    void modifyButtonClicked(const QModelIndex &index);

private:
    QPoint m_hoverPos;
};

#endif // CUSTOMDELEGATE_H
