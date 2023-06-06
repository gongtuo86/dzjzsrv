#ifndef JSONTREEEEXPLORER_H
#define JSONTREEEEXPLORER_H

#include <QDialog>
#include <QItemSelection>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>

#include "dfjson/json.h"

namespace Ui
{
class JsonTreeDialog;
}

class SearchFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    SearchFilterProxyModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {}
    void setSearchString(const QString &searchString);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QString m_searchString;
};

class JsonTreeModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit JsonTreeModel(const dfJson::Value &data, bool isMultiSelect, QObject *parent = nullptr);
    void parseJson(QStandardItem *parent, const dfJson::Value &value);

private:
    bool m_isMultiSelect;
};

class JsonTreeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JsonTreeDialog(const dfJson::Value &data, bool isMultiSelect, QWidget *parent = 0);
    ~JsonTreeDialog();
    QStringList getSelectedIds() const;
    QStringList getSelectedNames() const;
    void setSelectedIds(const QStringList &ids);

private slots:
    void search();
    void onItemChanged(QStandardItem *item);
    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    Ui::JsonTreeDialog *ui;
    JsonTreeModel *m_model;
    SearchFilterProxyModel *m_proxyModel;
    QStringList m_selectedIds;
    bool m_isMultiSelect;
};

#endif // JSONTREEEEXPLORER_H
