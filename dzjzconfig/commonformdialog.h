#ifndef COMMONFORMDIALOG_H
#define COMMONFORMDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QVBoxLayout>
#include <QVariant>

#include "dfjson/json.h"

template <typename T>
class ComboBoxData
{
public:
    T currentValue;
    QMap<T, QString> options;
};
Q_DECLARE_METATYPE(ComboBoxData<int>)
Q_DECLARE_METATYPE(ComboBoxData<QString>)

class JsonDialogData
{
public:
    QString initialText;
    dfJson::Value jsonData;
    bool isMultiSelect;
};
Q_DECLARE_METATYPE(JsonDialogData)

class JsonDialogLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit JsonDialogLineEdit(QWidget *parent = NULL);
    void setJsonData(const dfJson::Value &jsonData);
    void setMultiSelect(bool isMultiSelect);

    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    dfJson::Value m_jsonData;
    bool m_isMultiSelect;
};

class CommonFormDialog : public QDialog
{
    Q_OBJECT
public:
    enum
    {
        TYPE_DETAIL,
        TYPE_MODIFY,
        TYPE_ADD
    };

public:
    CommonFormDialog(const QList<QPair<QString, QVariant>> &data, int type, QWidget *parent = 0);

    ~CommonFormDialog();

    QList<QPair<QString, QVariant>> getData();

private slots:
    void onRelatedFeedLineTextChanged(const QString &text);
    void onRelatedRtuTextChanged(const QString &text);

private:
    QMap<QString, QWidget *> m_controlMap;
};

#endif // COMMONFORMDIALOG_H
