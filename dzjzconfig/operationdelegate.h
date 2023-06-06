#ifndef CUSTOMDELEGATE_H
#define CUSTOMDELEGATE_H

#include <QApplication>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionButton>
#include <QStyledItemDelegate>

class OperationDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    OperationDelegate(QObject *parent = 0);

    QRect buttonRect(const QStyleOptionViewItem &option, const QString &text, int buttonIndex) const;

    // �༭ʱ��ʾ�ؼ���
    // QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    //
    // void setEditorData(QWidget *editor, const QModelIndex &index) const;
    // void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    // void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    // ����
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    // ��С
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    // �¼�
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

signals:
    void detailButtonClicked(const QModelIndex &index);
    void deleteButtonClicked(const QModelIndex &index);
    void modifyButtonClicked(const QModelIndex &index);

private:
};

#endif // CUSTOMDELEGATE_H
