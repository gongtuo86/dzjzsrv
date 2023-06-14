#include "jsontreedialog.h"

#include "dflogger.h"
#include "ui_jsontreedialog.h"

/**
 * @brief ���������ַ������������������¼��㡣
 *
 * @param searchString �µ������ַ���
 */
void SearchFilterProxyModel::setSearchString(const QString &searchString)
{
    m_searchString = searchString;
    invalidateFilter();
}

/**
 * @brief �ж�һ���Ƿ�Ӧ�ñ����������ܡ�
 *
 * @param sourceRow Դģ���е��к�
 * @param sourceParent ��һ�еĸ�����
 * @return true �����һ��Ӧ�ñ�����
 * @return false �����һ��Ӧ�ñ��ܾ�
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
 * @brief ����һ���µ�JsonTreeModel
 *
 * @param data ԴJSON����
 * @param isMultiSelect �Ƿ�֧�ֶ�ѡ
 * @param parent QObject�ĸ�����
 */
JsonTreeModel::JsonTreeModel(const dfJson::Value &data, bool isMultiSelect, QObject *parent)
    : QStandardItemModel(parent), m_isMultiSelect(isMultiSelect)
{
    auto *root = invisibleRootItem();
    parseJson(root, data);

    setHorizontalHeaderLabels({QString::fromLocal8Bit("����")});
}

/**
 * @brief �ݹ����JSON���ݣ�����������ӵ�QStandardItemModel�С�
 *
 * @param parent ��QStandardItem
 * @param value Ҫ������JSONֵ
 */
void JsonTreeModel::parseJson(QStandardItem *parent, const dfJson::Value &value)
{
    // ȷ�� 'value' ��һ�����󣬲��Ұ��� 'id' �� 'name' �ֶ�
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
                // ����Ϊ��ѡ��
                nameItem->setFlags(nameItem->flags() | Qt::ItemIsSelectable);
            }
        }
        else
        {
            // ���ڷ���·�ڵ㣬����Ϊ����ѡ��
            nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsUserCheckable);
        }

        parent->appendRow(nameItem);

        // ��� 'value' ���� 'children' �ֶΣ���ô�ݹ�ش��� 'children'
        if (value.isMember("children"))
        {
            const dfJson::Value &children = value["children"];
            for (const auto &child : children)
            {
                parseJson(nameItem, child);
            }
        }
    }
    // ��� 'value' ��һ�����飬��ô�������ÿ��Ԫ�ص��� 'parseJson'
    else if (value.isArray())
    {
        for (const auto &val : value)
        {
            parseJson(parent, val);
        }
    }
}

/**
 * @brief ����һ���µ�JsonTreeDialog
 *
 * @param data ԴJSON����
 * @param isMultiSelect �Ƿ�֧�ֶ�ѡ
 * @param parent QWidget�ĸ�����
 */
JsonTreeDialog::JsonTreeDialog(const dfJson::Value &data, bool isMultiSelect, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::JsonTreeDialog),
      m_model(new JsonTreeModel(data, isMultiSelect, this)),
      m_proxyModel(new SearchFilterProxyModel(this)),
      m_isMultiSelect(isMultiSelect)

{
    ui->setupUi(this);
    setWindowTitle(QString::fromLocal8Bit("����ѡ��"));
    m_proxyModel->setSourceModel(m_model);
    ui->m_treeView->setModel(m_proxyModel);
    m_selectedIds.clear();
    ui->m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    if (m_isMultiSelect)
    {
        // ��ѡģʽ������ itemChanged �ź�
        connect(m_model, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(onItemChanged(QStandardItem *)));
    }
    else
    {
        // ��ѡģʽ������ selectionChanged �ź�
        connect(ui->m_treeView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(onSelectionChanged(const QItemSelection &, const QItemSelection &)));
    }

    connect(ui->m_searchPushButton, SIGNAL(clicked()), this, SLOT(search()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    ui->m_treeView->expandAll();
}

/**
 * @brief ��������
 */
JsonTreeDialog::~JsonTreeDialog()
{
    delete m_proxyModel;
    delete m_model;
    delete ui;
}

/**
 * @brief ִ����������
 */
void JsonTreeDialog::search()
{
    const QString searchString = ui->m_searchLineEdit->text();
    m_proxyModel->setSearchString(searchString);
    ui->m_treeView->expandAll();
}

/**
 * @brief ����QStandardItem�ı�������ֻ�ڶ�ѡģʽ��ʹ�ã�
 *
 * @param item �����ı��QStandardItem
 */
void JsonTreeDialog::onItemChanged(QStandardItem *item)
{
    if (item->checkState() == Qt::Checked)
    {
        // ����ѡ�У����� id ��ӵ� m_selectedIds ��
        m_selectedIds.append(item->data().toString());
    }
    else
    {
        // ����ȡ��ѡ�У����� id �� m_selectedIds ���Ƴ�
        m_selectedIds.removeAll(item->data().toString());
    }
    DFLOG_DEBUG("%s", m_selectedIds.join(",").toLocal8Bit().data());
}

/**
 * @brief ��ȡ��ѡ�е���Ŀ��id�б�
 *
 * @return QString id�б���ַ�����ʾ
 */
QStringList JsonTreeDialog::getSelectedIds() const
{
    //DFLOG_DEBUG("%s", m_selectedIds.join(",").toLocal8Bit().data());
    return m_selectedIds;
}

/**
 * @brief ����ѡ��ı�������ֻ�ڵ�ѡģʽ��ʹ�ã�
 *
 * @param selected ��ѡ�����Ŀ
 * @param deselected ��ȡ��ѡ�����Ŀ
 */
void JsonTreeDialog::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);
    // ��� m_selectedIds �б�
    m_selectedIds.clear();

    // ��ȡ��ǰѡ�е���
    QModelIndexList indices = selected.indexes();

    // �����ǰ��ѡ�е���
    if (!indices.isEmpty())
    {
        // ��ȡѡ����
        QModelIndex sourceIndex = m_proxyModel->mapToSource(indices.first());
        QStandardItem *item = m_model->itemFromIndex(sourceIndex);

        // ��ѡ�����id��ӵ� m_selectedIds ��
        m_selectedIds.append(item->data().toString());
        //DFLOG_DEBUG("%s", m_selectedIds.join(",").toLocal8Bit().data());
    }
}

/**
 * @brief ��ȡ��ѡ�е���Ŀ�������б�
 *
 * @return QStringList ����ѡ����Ŀ���Ƶ��б�
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
