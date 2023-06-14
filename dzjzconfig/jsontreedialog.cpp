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
    parseJson(root, data);

    setHorizontalHeaderLabels({QString::fromLocal8Bit("名称")});
}

/**
 * @brief 递归解析JSON数据，并将数据添加到QStandardItemModel中。
 *
 * @param parent 父QStandardItem
 * @param value 要解析的JSON值
 */
void JsonTreeModel::parseJson(QStandardItem *parent, const dfJson::Value &value)
{
    // 确认 'value' 是一个对象，并且包含 'id' 和 'name' 字段
    if (value.isObject() && value.isMember("id") && value.isMember("name"))
    {
        QString id = QString::fromLocal8Bit(value["id"].asCString());
        QString name = QString::fromLocal8Bit(value["name"].asCString());
        QString type = QString::fromLocal8Bit(value["type"].asCString());

        QStandardItem *nameItem = new QStandardItem(name);
        nameItem->setData(id);

        if (type != "substation")
        {
            if (m_isMultiSelect)
            {
                nameItem->setCheckable(true);
            }
            else
            {
                // 设置为可选择
                nameItem->setFlags(nameItem->flags() | Qt::ItemIsSelectable);
            }
        }
        else
        {
            // 对于非线路节点，设置为不可选择
            nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsUserCheckable);
        }

        parent->appendRow(nameItem);

        // 如果 'value' 包含 'children' 字段，那么递归地处理 'children'
        if (value.isMember("children"))
        {
            const dfJson::Value &children = value["children"];
            for (const auto &child : children)
            {
                parseJson(nameItem, child);
            }
        }
    }
    // 如果 'value' 是一个数组，那么对数组的每个元素调用 'parseJson'
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
    if (item->checkState() == Qt::Checked)
    {
        // 如果项被选中，将其 id 添加到 m_selectedIds 中
        m_selectedIds.append(item->data().toString());
    }
    else
    {
        // 如果项被取消选中，将其 id 从 m_selectedIds 中移除
        m_selectedIds.removeAll(item->data().toString());
    }
    DFLOG_DEBUG("%s", m_selectedIds.join(",").toLocal8Bit().data());
}

/**
 * @brief 获取被选中的项目的id列表
 *
 * @return QString id列表的字符串表示
 */
QStringList JsonTreeDialog::getSelectedIds() const
{
    //DFLOG_DEBUG("%s", m_selectedIds.join(",").toLocal8Bit().data());
    return m_selectedIds;
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
        //DFLOG_DEBUG("%s", m_selectedIds.join(",").toLocal8Bit().data());
    }
}

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

void JsonTreeDialog::setSelectedIds(const QStringList &ids)
{
//    DFLOG_DEBUG("ids: %s", qPrintable(ids.join(",")));

    m_selectedIds.clear();

    if (ids.isEmpty())
        return;

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
            // m_selectedIds.append(ids.first());
        }
  //      DFLOG_DEBUG("%s", m_selectedIds.join(",").toLocal8Bit().data());
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
            // m_selectedIds.append(id);
        }
    }
 //   DFLOG_DEBUG("%s", m_selectedIds.join(",").toLocal8Bit().data());
}
