#include "jsontreedialog.h"

#include "dflogger.h"
#include "ui_jsontreedialog.h"

/**
 * @brief 设置搜索字符串，触发过滤器重新计算。
 *
 * @param searchString 新的搜索字符串
 */
void SearchFilterProxyModel::setSearchString(const QString &searchString)
{
    m_searchString = searchString;
    invalidateFilter();
}

/**
 * @brief 判断一行是否应该被过滤器接受。
 *
 * @param sourceRow 源模型中的行号
 * @param sourceParent 这一行的父索引
 * @return true 如果这一行应该被接受
 * @return false 如果这一行应该被拒绝
 */
bool SearchFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    if (sourceModel()->data(index).toString().contains(m_searchString))
    {
        return true;
    }
    for (int i = 0; i < sourceModel()->rowCount(index); ++i)
    {
        if (filterAcceptsRow(i, index))
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief 构造一个新的JsonTreeModel
 *
 * @param data 源JSON数据
 * @param isMultiSelect 是否支持多选
 * @param parent QObject的父对象
 */
JsonTreeModel::JsonTreeModel(const dfJson::Value &data, bool isMultiSelect, QObject *parent)
    : QStandardItemModel(parent), m_isMultiSelect(isMultiSelect)
{
    auto *root = invisibleRootItem();

    if (m_isMultiSelect)
    {
        QStandardItem *allItem = new QStandardItem(QString::fromLocal8Bit("全部"));
        allItem->setCheckable(true);
        allItem->setData("root");

        root->appendRow(allItem);
        parseJson(allItem, data);
    }
    else
    {
        parseJson(root, data);
    }

    setHorizontalHeaderLabels({QString::fromLocal8Bit("名称"), QString::fromLocal8Bit("ID")});
}

/**
 * @brief 递归解析JSON数据，并将数据添加到QStandardItemModel中。
 *
 * @param parent 父QStandardItem
 * @param value 要解析的JSON值
 */
void JsonTreeModel::parseJson(QStandardItem *parent, const dfJson::Value &value)
{
    if (value.isObject() && value.isMember("id") && value.isMember("name"))
    {
        QString id = QString::fromLocal8Bit(value["id"].asCString());
        QString name = QString::fromLocal8Bit(value["name"].asCString());
        QString type = QString::fromLocal8Bit(value["type"].asCString());

        QStandardItem *nameItem = new QStandardItem(name);
        QStandardItem *idItem = new QStandardItem(id);
        nameItem->setData(id);

        if (m_isMultiSelect)
        {
            nameItem->setCheckable(true);
        }
        else
        {
            if (type != "substation")
            {
                nameItem->setFlags(nameItem->flags() | Qt::ItemIsSelectable);
            }
            else
            {
                nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsSelectable);
            }
        }

        QList<QStandardItem *> rowItems;
        rowItems << nameItem << idItem;
        parent->appendRow(rowItems);

        if (value.isMember("children"))
        {
            const dfJson::Value &children = value["children"];
            for (const auto &child : children)
            {
                parseJson(nameItem, child);
            }
        }
    }
    else if (value.isArray())
    {
        for (const auto &val : value)
        {
            parseJson(parent, val);
        }
    }
}

/**
 * @brief 构造一个新的JsonTreeDialog
 *
 * @param data 源JSON数据
 * @param isMultiSelect 是否支持多选
 * @param parent QWidget的父对象
 */
JsonTreeDialog::JsonTreeDialog(const dfJson::Value &data, bool isMultiSelect, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::JsonTreeDialog),
      m_model(new JsonTreeModel(data, isMultiSelect, this)),
      m_proxyModel(new SearchFilterProxyModel(this)),
      m_isMultiSelect(isMultiSelect)

{
    ui->setupUi(this);
    setWindowTitle(QString::fromLocal8Bit("参数选择"));
    m_proxyModel->setSourceModel(m_model);
    ui->m_treeView->setModel(m_proxyModel);
    m_selectedIds.clear();
    ui->m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->m_treeView->header()->setResizeMode(QHeaderView::ResizeToContents);

    if (m_isMultiSelect)
    {
        // 多选模式：监听 itemChanged 信号
        connect(m_model, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(onItemChanged(QStandardItem *)));
    }
    else
    {
        // 单选模式：监听 selectionChanged 信号
        connect(ui->m_treeView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(onSelectionChanged(const QItemSelection &, const QItemSelection &)));
    }

    connect(ui->m_searchPushButton, SIGNAL(clicked()), this, SLOT(search()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    ui->m_treeView->expandAll();
}

/**
 * @brief 析构函数
 */
JsonTreeDialog::~JsonTreeDialog()
{
    delete m_proxyModel;
    delete m_model;
    delete ui;
}

/**
 * @brief 执行搜索操作
 */
void JsonTreeDialog::search()
{
    const QString searchString = ui->m_searchLineEdit->text();
    m_proxyModel->setSearchString(searchString);
    ui->m_treeView->expandAll();
}

/**
 * @brief 处理QStandardItem改变的情况（只在多选模式下使用）
 *
 * @param item 发生改变的QStandardItem
 */
void JsonTreeDialog::onItemChanged(QStandardItem *item)
{
    m_model->blockSignals(true);

    if (item->checkState() == Qt::Checked)
    {
        for (int i = 0; i < item->rowCount(); ++i)
        {
            QStandardItem *childItem = item->child(i);
            if (childItem->checkState() != Qt::Checked)
            {
                childItem->setCheckState(Qt::Checked);
                onItemChanged(childItem);
            }
        }
    }
    else
    {
        for (int i = 0; i < item->rowCount(); ++i)
        {
            QStandardItem *childItem = item->child(i);
            if (childItem->checkState() != Qt::Unchecked)
            {
                childItem->setCheckState(Qt::Unchecked);
                onItemChanged(childItem);
            }
        }
    }
    updateParentItem(item);

    m_model->blockSignals(false);

    // DFLOG_DEBUG("%s", m_selectedIds.join(",").toLocal8Bit().data());
}

/**
 * @brief 获取被选中的项目的id列表
 *
 * @return QString id列表的字符串表示
 */
QStringList JsonTreeDialog::getSelectedIds()
{
    if (m_isMultiSelect)
    {
        m_selectedIds.clear();
        for (int i = 0; i < m_model->rowCount(); ++i)
        {
            QStandardItem *item = m_model->item(i);
            updateSelectedIds(item);
        }
    }

    return m_selectedIds;
}

void JsonTreeDialog::updateSelectedIds(QStandardItem *item)
{
    if (item->checkState() == Qt::Checked && item->rowCount() == 0)
    {
        m_selectedIds.append(item->data().toString());
    }

    for (int i = 0; i < item->rowCount(); ++i)
    {
        QStandardItem *childItem = item->child(i);
        updateSelectedIds(childItem);
    }
}

/**
 * @brief 处理选择改变的情况（只在单选模式下使用）
 *
 * @param selected 新选择的项目
 * @param deselected 被取消选择的项目
 */
void JsonTreeDialog::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);
    // 清空 m_selectedIds 列表
    m_selectedIds.clear();

    // 获取当前选中的项
    QModelIndexList indices = selected.indexes();

    // 如果当前有选中的项
    if (!indices.isEmpty())
    {
        // 获取选中项
        QModelIndex sourceIndex = m_proxyModel->mapToSource(indices.first());
        QStandardItem *item = m_model->itemFromIndex(sourceIndex);

        // 将选中项的id添加到 m_selectedIds 中
        m_selectedIds.append(item->data().toString());
        // DFLOG_DEBUG("%s", m_selectedIds.join(",").toLocal8Bit().data());
    }
}
#if 0
/**
 * @brief 获取被选中的项目的名称列表
 *
 * @return QStringList 包含选中项目名称的列表
 */
QStringList JsonTreeDialog::getSelectedNames() const
{
    QStringList selectedNames;
    for (const QString &id : m_selectedIds)
    {
        QModelIndexList matches = m_model->match(
            m_model->index(0, 0),
            Qt::UserRole + 1,
            id,
            -1,
            Qt::MatchRecursive);

        DFLOG_DEBUG("matches.size(): %d", matches.size());
        for (const QModelIndex &match : matches)
        {
            QStandardItem *item = m_model->itemFromIndex(match);
            QString name = item->text();
            selectedNames.append(name);
        }
    }

    // DFLOG_DEBUG("m_selectedIds, selectedNames: %s", qPrintable(m_selectedIds.join(",")), qPrintable(selectedNames.join(",")));
    return selectedNames;
}
#endif

void JsonTreeDialog::setSelectedIds(const QStringList &ids)
{
    m_selectedIds = ids;
    m_model->blockSignals(true);

    if (ids.isEmpty())
    {
        m_model->blockSignals(false);
        return;
    }

    if (!m_isMultiSelect)
    {
        QModelIndexList matches = m_model->match(
            m_model->index(0, 0),
            Qt::UserRole + 1,
            ids.first(),
            1,
            Qt::MatchRecursive);

        //       DFLOG_DEBUG("matches.size(): %d", matches.size());
        if (!matches.isEmpty())
        {
            QStandardItem *item = m_model->itemFromIndex(matches.first());
            ui->m_treeView->setCurrentIndex(m_proxyModel->mapFromSource(item->index()));
        }
        m_model->blockSignals(false);
        return;
    }

    for (const QString &id : ids)
    {
        QModelIndexList matches = m_model->match(
            m_model->index(0, 0),
            Qt::UserRole + 1,
            id,
            -1,
            Qt::MatchRecursive);

        for (const QModelIndex &match : matches)
        {
            QStandardItem *item = m_model->itemFromIndex(match);
            item->setCheckState(Qt::Checked);
            updateParentItem(item);
        }
    }
    m_model->blockSignals(false);
}

void JsonTreeDialog::updateParentItem(QStandardItem *item)
{
    if (!item || item->parent() == nullptr)
        return;

    QStandardItem *parent = item->parent();
    int checkedCount = 0;
    int childCount = parent->rowCount();

    for (int i = 0; i < childCount; ++i)
    {
        QStandardItem *childItem = parent->child(i);
        if (childItem->checkState() == Qt::Checked)
            ++checkedCount;
        else if (childItem->checkState() == Qt::PartiallyChecked)
        {
            parent->setCheckState(Qt::PartiallyChecked);
            updateParentItem(parent);
            return;
        }
    }

    if (checkedCount == 0)
        parent->setCheckState(Qt::Unchecked);
    else if (checkedCount == childCount)
        parent->setCheckState(Qt::Checked);
    else
        parent->setCheckState(Qt::PartiallyChecked);

    updateParentItem(parent);
}
