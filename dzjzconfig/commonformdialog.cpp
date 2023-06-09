#include "commonformdialog.h"

#include <QComboBox>
#include <QDateTimeEdit>
#include <QDebug>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPair>
#include <QFormLayout>

#include "dflogger.h"
#include "jsontreedialog.h"
#include "dbmanager.h"

JsonDialogLineEdit::JsonDialogLineEdit(QWidget *parent) : QLineEdit(parent)
{
    setReadOnly(true);
}

void JsonDialogLineEdit::setJsonData(const dfJson::Value &jsonData)
{
    m_jsonData = jsonData;
}

void JsonDialogLineEdit::setMultiSelect(bool isMultiSelect)
{
    m_isMultiSelect = isMultiSelect;
}

void JsonDialogLineEdit::mouseDoubleClickEvent(QMouseEvent *event)
{
    JsonTreeDialog dialog(m_jsonData, m_isMultiSelect, this);
    if (!text().isEmpty())
    {
        dialog.setSelectedIds(text().split(","));
    }
    if (dialog.exec() == QDialog::Accepted)
    {
        QStringList ids = dialog.getSelectedIds();
        setText(ids.join(", "));
#if 0
        QStringList ids = dialog.getSelectedIds();
        QStringList names = dialog.getSelectedNames();
        if (ids.size() != names.size())
        {
            DFLOG_DEBUG("错误：ids 的数量与 names 的数量不匹配。");
            return;
        }
        QStringList formattedItems;
        for (int i = 0; i < ids.size(); ++i)
        {
            QString formattedItem = QString("[%1]%2").arg(ids.at(i), names.at(i));
            formattedItems.append(formattedItem);
        }

        m_selectedIds = ids;
        setText(formattedItems.join(", "));
#endif
    }
}

CommonFormDialog::CommonFormDialog(const QList<QPair<QString, QVariant>> &data, int type, QWidget *parent) : QDialog(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    for (const auto &datum : data)
    {
        QLabel *label = new QLabel(datum.first, this);
        QWidget *control = NULL;

        switch (datum.second.type())
        {
            case QVariant::Int:
            {
                QSpinBox *spinBox = new QSpinBox(this);
                control = spinBox;
                spinBox->setValue(datum.second.toInt());
                break;
            }
            case QVariant::Double:
            {
                QDoubleSpinBox *doubleSpinBox = new QDoubleSpinBox(this);
                control = doubleSpinBox;
                doubleSpinBox->setValue(datum.second.toDouble());
                break;
            }
            case QVariant::DateTime:
            {
                QDateTimeEdit *dateTimeEdit = new QDateTimeEdit(this);
                control = dateTimeEdit;
                dateTimeEdit->setDateTime(datum.second.toDateTime());
                break;
            }
            default:
            {
                if (datum.second.userType() == qMetaTypeId<ComboBoxData<int>>())
                {
                    ComboBoxData<int> comboBoxData = datum.second.value<ComboBoxData<int>>();
                    QComboBox *comboBox = new QComboBox(this);
                    control = comboBox;
                    int currentIndex = 0;
                    for (auto itOpt = comboBoxData.options.begin(); itOpt != comboBoxData.options.end(); ++itOpt)
                    {
                        comboBox->addItem(itOpt.value(), QVariant(itOpt.key()));
                        if (itOpt.key() == comboBoxData.currentValue)
                        {
                            currentIndex = comboBox->count() - 1;
                        }
                    }
                    comboBox->setCurrentIndex(currentIndex);
                }
                else if (datum.second.userType() == qMetaTypeId<ComboBoxData<QString>>())
                {
                    ComboBoxData<QString> comboBoxData = datum.second.value<ComboBoxData<QString>>();
                    QComboBox *comboBox = new QComboBox(this);
                    control = comboBox;
                    int currentIndex = -1; // 用于保存匹配值的索引
                    for (auto itOpt = comboBoxData.options.begin(); itOpt != comboBoxData.options.end(); ++itOpt)
                    {
                        comboBox->addItem(itOpt.value(), QVariant(itOpt.key()));
                        // 检查当前值是否与选中值匹配
                        if (itOpt.key() == comboBoxData.currentValue)
                        {
                            currentIndex = comboBox->count() - 1; // 设置为最新添加的项的索引
                        }
                    }
                    comboBox->setCurrentIndex(currentIndex);
                }
                else if (datum.second.userType() == qMetaTypeId<JsonDialogData>())
                {
                    JsonDialogData jsonDialogData = datum.second.value<JsonDialogData>();
                    JsonDialogLineEdit *jsonDialogLineEdit = new JsonDialogLineEdit(this);
                    control = jsonDialogLineEdit;
                    jsonDialogLineEdit->setText(jsonDialogData.initialText);
                    jsonDialogLineEdit->setJsonData(jsonDialogData.jsonData);
                    jsonDialogLineEdit->setMultiSelect(jsonDialogData.isMultiSelect);
                    if (datum.first == QString::fromLocal8Bit("关联馈线"))
                    {
                        // 增加和名称的联动功能
                        connect(jsonDialogLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(onRelatedFeedLineTextChanged(const QString &)));
                    }
                    if (datum.first == QString::fromLocal8Bit("编号"))
                    {
                        connect(jsonDialogLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(onRelatedRtuTextChanged(const QString &)));
                    }
                }
                else
                {
                    QLineEdit *lineEdit = new QLineEdit(this);
                    control = lineEdit;
                    lineEdit->setText(datum.second.toString());
                }
                break;
            }
        }
        formLayout->addRow(label, control);
        m_controlMap.insert(datum.first, control);
    }

    mainLayout->addLayout(formLayout);

    QDialogButtonBox::StandardButtons buttons = (type == TYPE_DETAIL) ? QDialogButtonBox::Close : (QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(buttons, Qt::Horizontal, this);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
}

CommonFormDialog::~CommonFormDialog()
{
}

QList<QPair<QString, QVariant>> CommonFormDialog::getData()
{
    QList<QPair<QString, QVariant>> data;

    // 遍历布局中的所有子控件
    for (int i = 0; i < layout()->count(); ++i)
    {
        QLayoutItem *item = layout()->itemAt(i);
        if (item->layout())
        {
            // 假设第二个子控件是我们的输入控件
            QWidget *widget = item->layout()->itemAt(1)->widget();
            if (widget)
            {
                QPair<QString, QVariant> pair;
                // 获取字段的名称
                pair.first = static_cast<QLabel *>(item->layout()->itemAt(0)->widget())->text();

                // 根据控件的类型获取值
                if (QSpinBox *spinBox = qobject_cast<QSpinBox *>(widget))
                {
                    pair.second = spinBox->value();
                }
                else if (QDoubleSpinBox *doubleSpinBox = qobject_cast<QDoubleSpinBox *>(widget))
                {
                    pair.second = doubleSpinBox->value();
                }
                else if (QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit *>(widget))
                {
                    pair.second = dateTimeEdit->dateTime();
                }
                else if (QComboBox *comboBox = qobject_cast<QComboBox *>(widget))
                {
                    pair.second = comboBox->itemData(comboBox->currentIndex(), Qt::UserRole);
                }
                else if (JsonDialogLineEdit *jsonDialogLineEdit = qobject_cast<JsonDialogLineEdit *>(widget))
                {
                    pair.second = jsonDialogLineEdit->text();
                }
                else if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(widget))
                {
                    pair.second = lineEdit->text();
                }

                data.append(pair);
            }
        }
    }

    return data;
}

/**
 * @brief 关联馈线的文本框内容改变时，将内容同步到名称文本框中
 *
 * @param text
 */
void CommonFormDialog::onRelatedFeedLineTextChanged(const QString &text)
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(m_controlMap.value(QString::fromLocal8Bit("名称")));
    if (lineEdit)
    {
        lineEdit->setText(DBManager::getInstance()->m_lineIdNameMap[text]);
    }
}

void CommonFormDialog::onRelatedRtuTextChanged(const QString &text)
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(m_controlMap.value(QString::fromLocal8Bit("名称")));
    if (lineEdit)
    {
        lineEdit->setText(DBManager::getInstance()->m_rtuIdNameMap[text.toInt()]);
    }

    // 同时更新所属厂站内容
    QComboBox *stationComboBox = qobject_cast<QComboBox *>(m_controlMap.value(QString::fromLocal8Bit("所属厂站")));
    {
        if (stationComboBox)
        {
            QString stationId = DBManager::getInstance()->getStaByRtuId(text.toInt());
            int index = stationComboBox->findData(stationId);
            if (index != -1)
            {
                stationComboBox->setCurrentIndex(index);
            }
        }
    }
}

void CommonFormDialog::setEnabled(const QString &fieldName, bool readOnly)
{
    QWidget *widget = m_controlMap.value(fieldName);
    if (widget)
    {
        widget->setEnabled(readOnly);
    }
}
