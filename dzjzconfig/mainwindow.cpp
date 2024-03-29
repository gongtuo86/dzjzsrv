#include "mainwindow.h"

#include <QDebug>
#include <QMessageBox>
#include <QMenu>
#include <QProgressDialog>
#include <QThreadPool>
#include <QTimer>
#include <QFileDialog>

#include "commonformdialog.h"
#include "dbmanager.h"
#include "dflogger.h"
#include "dmDelegate.h"
#include "dzjzconfigutil.h"
#include "operationdelegate.h"
#include "sybase.h"
#include "ui_mainwindow.h"
#include "jsontreedialog.h"
#include "fixvaluesrv.h"
#include "callRunnable.h"
#include "rightmanage4.h"
#include "userRightDefine.h"
#include "dzjzfileparser.h"

static char *arrUfBaseFreq[] = {
    "uf_base1_freq",
    "uf_base2_freq",
    "uf_base3_freq",
    "uf_base4_freq",
    "uf_base5_freq",
    "uf_base6_freq",
    "uf_spec1_freq",
    "uf_spec2_freq"};

static char *arrUfBaseTime[] = {
    "uf_base1_time",
    "uf_base2_time",
    "uf_base3_time",
    "uf_base4_time",
    "uf_base5_time",
    "uf_base6_time",
    "uf_spec1_time",
    "uf_spec2_time"};

static char *arrUfBaseRequire[] = {
    "uf_base1_require",
    "uf_base2_require",
    "uf_base3_require",
    "uf_base4_require",
    "uf_base5_require",
    "uf_base6_require",
    "uf_spec1_require",
    "uf_spec2_require"};

static char *arrUfBaseRequireRate[] = {
    "uf_base1_require_rate",
    "uf_base2_require_rate",
    "uf_base3_require_rate",
    "uf_base4_require_rate",
    "uf_base5_require_rate",
    "uf_base6_require_rate",
    "uf_spec1_require_rate",
    "uf_spec2_require_rate"};

static char *arrUVBaseVolt[] = {
    "uv_base1_volt",
    "uv_base2_volt",
    "uv_spec1_volt",
    "uv_spec2_volt"};

static char *arrUvBaseTime[] = {
    "uv_base1_time",
    "uv_base2_time",
    "uv_spec1_time",
    "uv_spec2_time"};

static char *arrUvBaseRequire[] = {
    "uv_base1_require",
    "uv_base2_require",
    "uv_spec1_require",
    "uv_spec2_require"};

static char *arrUvBaseRequireRate[] = {
    "uv_base1_require_rate",
    "uv_base2_require_rate",
    "uv_spec1_require_rate",
    "uv_spec2_require_rate"};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    m_pRightManage = NULL;

    ui->setupUi(this);

    this->setWindowTitle(QString::fromLocal8Bit("第三道防线维护工具"));

    startdb(1, 1);

    m_pDbManager = DBManager::getInstance();

    connect(ui->m_listWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(onModuleItemClicked(QListWidgetItem *)));

    setupModels();

    // 设置左侧列表宽度
    ui->m_listWidget->setIconSize(QSize(32, 32));
    QIcon icon[5] = {
        QIcon(":/qss_icons/light/rc/area.png"),
        QIcon(":/qss_icons/light/rc/device.png"),
        QIcon(":/qss_icons/light/rc/item.png"),
        QIcon(":/qss_icons/light/rc/task.png"),
        QIcon(":/qss_icons/light/rc/import.png")};
    for (int i = 0; i < ui->m_listWidget->count(); ++i)
    {
        QListWidgetItem *pItem = ui->m_listWidget->item(i);
        pItem->setIcon(icon[i]);
    }
    int width = ui->m_listWidget->sizeHintForColumn(0) + 2 * ui->m_listWidget->frameWidth() + 20;
    ui->m_listWidget->setFixedWidth(width);

    QIcon btnIcon = QIcon(":/qss_icons/light/rc/add.png");
    ui->m_pushButtonAddArea->setIcon(btnIcon);
    ui->m_pushButtonAddArea->setIconSize(QSize(32, 32));
    ui->m_pushButtonDevice->setIcon(btnIcon);
    ui->m_pushButtonDevice->setIconSize(QSize(32, 32));
    ui->m_pushButtonDevicePara->setIcon(btnIcon);
    ui->m_pushButtonDevicePara->setIconSize(QSize(32, 32));
    ui->m_addRoundItemPushButton->setIcon(btnIcon);
    ui->m_addRoundItemPushButton->setIconSize(QSize(32, 32));
    ui->m_addTaskPushButton->setIcon(btnIcon);
    ui->m_addTaskPushButton->setIconSize(QSize(32, 32));

    QIcon editBtnIcon = QIcon(":/qss_icons/light/rc/edit.png");
    ui->m_roundItemEditPushButton->setIcon(editBtnIcon);
    ui->m_roundItemEditPushButton->setIconSize(QSize(32, 32));

    QIcon callBtnIcon = QIcon(":/qss_icons/light/rc/call.png");
    ui->m_callPushButton->setIcon(callBtnIcon);
    ui->m_callPushButton->setIconSize(QSize(32, 32));

    QIcon selectFileBtnIcon = QIcon(":/qss_icons/light/rc/selectfile.png");
    ui->m_pushButtonRoundSelect->setIcon(selectFileBtnIcon);
    ui->m_pushButtonRoundSelect->setIconSize(QSize(32, 32));

    QIcon importBtnIcon = QIcon(":/qss_icons/light/rc/import.png");
    ui->m_pushButtonRoundImport->setIcon(importBtnIcon);
    ui->m_pushButtonRoundImport->setIconSize(QSize(32, 32));

    onModuleItemClicked(ui->m_listWidget->item(0));

    this->setEnabled(false);

    QTimer::singleShot(0, this, SLOT(OnLogin()));
}

MainWindow::~MainWindow()
{
    closedb();
    delete ui;
}

void MainWindow::OnLogin()
{
    if (m_pRightManage == NULL)
        m_pRightManage = new RightManage4(this, "dzjzconfig");
    QStringList strList;
    strList.append(QString("%1").arg(RIGHT_MODIFY_MAIN_PARA));

    if (m_pRightManage->dispPassWordDialog(&strList, this) == QDialog::Accepted)
    {
        this->setEnabled(true);
    }
    else
    {
        DFLOG_DEBUG("OnLogin exit");
        QApplication::exit();
    }
}

void MainWindow::setupModels()
{
    setupAreaTable();
    setupRoundTree();
    setupRoundItemTable();
    setupDeviceTable();
    setupDeviceParaTable();
    setupTaskTable();
    setupRoundFileModel();
}

void MainWindow::setupAreaTable()
{
    // 区域模型
    m_areaModel = new QStandardItemModel(this);
    m_areaModel->setHorizontalHeaderLabels(QStringList() << QString::fromLocal8Bit("编号")
                                                         << QString::fromLocal8Bit("名称")
                                                         << QString::fromLocal8Bit("类型")
                                                         << QString::fromLocal8Bit("关联厂站")
                                                         << QString::fromLocal8Bit("操作"));

    // ui->m_tableViewArea->setAlternatingRowColors(true);
    ui->m_tableViewArea->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // ui->m_tableViewArea->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->m_tableViewArea->setModel(m_areaModel);
    ui->m_tableViewArea->setItemDelegateForColumn(2, new DMDelegateInt(m_pDbManager->m_areaTypeMap, ui->m_tableViewArea));
    ui->m_tableViewArea->setItemDelegateForColumn(3, new DMDelegateString(m_pDbManager->m_staIdNameMap, ui->m_tableViewArea));
    m_BtnDelegateArea = new OperationDelegate(ui->m_tableViewArea);
    connect(ui->m_pushButtonAddArea, SIGNAL(clicked()), this, SLOT(onAddButtonAreaClicked()));
    connect(m_BtnDelegateArea, SIGNAL(detailButtonClicked(QModelIndex)), this, SLOT(onDetailButtonAreaClicked(QModelIndex)));
    connect(m_BtnDelegateArea, SIGNAL(deleteButtonClicked(QModelIndex)), this, SLOT(onDeleteButtonAreaClicked(QModelIndex)));
    connect(m_BtnDelegateArea, SIGNAL(modifyButtonClicked(QModelIndex)), this, SLOT(onModifyButtonAreaClicked(QModelIndex)));
    ui->m_tableViewArea->setItemDelegateForColumn(4, m_BtnDelegateArea);
}

void MainWindow::setupRoundTree()
{
    // 轮次模型
    m_roundModel = new QStandardItemModel(this);
    m_roundModel->setHorizontalHeaderLabels(QStringList() << QString::fromLocal8Bit("名称")
                                                          << QString::fromLocal8Bit("编号")
                                                          << QString::fromLocal8Bit("所属地区")
                                                          << QString::fromLocal8Bit("功能类型")
                                                          << QString::fromLocal8Bit("轮类型")
                                                          << QString::fromLocal8Bit("频率或电压整定值")
                                                          << QString::fromLocal8Bit("动作延时整定值")
                                                          << QString::fromLocal8Bit("下发应切荷量"));
    // ui->m_roundTreeView->setAlternatingRowColors(true);
    ui->m_roundTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->m_roundTreeView->setModel(m_roundModel);
    // 当选中项发生变化时，更新标签的内容
    connect(ui->m_roundTreeView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(onRoundSelectionModelChanged(const QItemSelection &, const QItemSelection &)));

    m_roundContextMenu = new QMenu(this);
    m_roundContextMenu->addAction(QString::fromLocal8Bit("添加轮次"), this, SLOT(onAddRoundActionTriggered()));
    m_roundContextMenu->addAction(QString::fromLocal8Bit("修改轮次"), this, SLOT(onModifyRoundActionTriggered()));
    m_roundContextMenu->addAction(QString::fromLocal8Bit("删除轮次"), this, SLOT(onDeleteRoundActionTriggered()));
    ui->m_roundTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->m_roundTreeView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showRoundContextMenu(const QPoint &)));

    ui->m_roundTreeView->header()->setHidden(true);

    // 隐藏不需要的列
    ui->m_roundTreeView->setColumnHidden(1, true);
    ui->m_roundTreeView->setColumnHidden(2, true);
    ui->m_roundTreeView->setColumnHidden(3, true);
    ui->m_roundTreeView->setColumnHidden(4, true);
    ui->m_roundTreeView->setColumnHidden(5, true);
    ui->m_roundTreeView->setColumnHidden(6, true);
    ui->m_roundTreeView->setColumnHidden(7, true);

    ui->m_roundTreeView->header()->setResizeMode(QHeaderView::ResizeToContents);
}

void MainWindow::setupRoundItemTable()
{
    // 区域模型
    m_roundItemModel = new QStandardItemModel(this);
    m_roundItemModel->setHorizontalHeaderLabels(QStringList() << QString::fromLocal8Bit("编号")
                                                              << QString::fromLocal8Bit("名称")
                                                              << QString::fromLocal8Bit("所属分区")
                                                              << QString::fromLocal8Bit("所属轮次")
                                                              << QString::fromLocal8Bit("关联馈线")
                                                              << QString::fromLocal8Bit("负荷类型")
                                                              << QString::fromLocal8Bit("投退计划")
                                                              << QString::fromLocal8Bit("关联开关")
                                                              << QString::fromLocal8Bit("有功代码")
                                                              << QString::fromLocal8Bit("关联装置")
                                                              << QString::fromLocal8Bit("关联出口")
                                                              << QString::fromLocal8Bit("操作"));

    // ui->m_roundItemTableView->setAlternatingRowColors(true);
    ui->m_roundItemTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // ui->m_roundItemTableView->setModel(m_roundItemModel);
    ui->m_roundItemTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->m_roundItemTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->m_roundItemTableView->setSortingEnabled(true);

    m_roundItemProxyModel = new RoundItemFilterProxyModel(this);
    m_roundItemProxyModel->setSourceModel(m_roundItemModel);

    ui->m_roundItemTableView->setModel(m_roundItemProxyModel);
    ui->m_areaFilterComboBox->setProperty("filterType", "area");
    ui->m_roundFilterComboBox->setProperty("filterType", "round");
    ui->m_loadTypeFilterComboBox->setProperty("filterType", "loadType");
    ui->m_strapFilterComboBox->setProperty("filterType", "strap");
    ui->m_deviceFilterComboBox->setProperty("filterType", "device");

    connect(ui->m_areaFilterComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateRoundItemFilter(int)));
    connect(ui->m_areaFilterComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDeviceOptions(int)));
    connect(ui->m_roundFilterComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateRoundItemFilter(int)));
    connect(ui->m_loadTypeFilterComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateRoundItemFilter(int)));
    connect(ui->m_strapFilterComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateRoundItemFilter(int)));
    connect(ui->m_deviceFilterComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateRoundItemFilter(int)));

    // 禁止第11列排序
    ui->m_roundItemTableView->horizontalHeader()->setResizeMode(11, QHeaderView::Interactive);

    ui->m_roundItemTableView->setItemDelegateForColumn(2, new DMDelegateInt(m_pDbManager->m_subAreaIdNameMap, ui->m_roundItemTableView));
    ui->m_roundItemTableView->setItemDelegateForColumn(3, new DMDelegateInt(m_pDbManager->m_roundIdNameMap, ui->m_roundItemTableView));
    ui->m_roundItemTableView->setItemDelegateForColumn(4, new DMDelegateString(m_pDbManager->m_lineIdNameMap, ui->m_roundItemTableView));
    ui->m_roundItemTableView->setItemDelegateForColumn(5, new DMDelegateInt(m_pDbManager->m_loadTypeMap, ui->m_roundItemTableView));
    ui->m_roundItemTableView->setItemDelegateForColumn(6, new DMDelegateInt(m_pDbManager->m_strapMap, ui->m_roundItemTableView));
    ui->m_roundItemTableView->setItemDelegateForColumn(7, new DMDelegateString(m_pDbManager->m_breakIdNameMap, ui->m_roundItemTableView));
    ui->m_roundItemTableView->setItemDelegateForColumn(8, new DMDelegateString(m_pDbManager->m_tmIdNameMap, ui->m_roundItemTableView));
    ui->m_roundItemTableView->setItemDelegateForColumn(9, new DMDelegateInt(m_pDbManager->m_deviceIdNameMap, ui->m_roundItemTableView));

    m_BtnDelegateRoundItem = new OperationDelegate(ui->m_roundItemTableView);
    connect(m_BtnDelegateRoundItem, SIGNAL(detailButtonClicked(QModelIndex)), this, SLOT(onDetailButtonRoundItemClicked(QModelIndex)));
    connect(m_BtnDelegateRoundItem, SIGNAL(deleteButtonClicked(QModelIndex)), this, SLOT(onDeleteButtonRoundItemClicked(QModelIndex)));
    connect(m_BtnDelegateRoundItem, SIGNAL(modifyButtonClicked(QModelIndex)), this, SLOT(onModifyButtonRoundItemClicked(QModelIndex)));
    ui->m_roundItemTableView->setItemDelegateForColumn(m_roundItemModel->columnCount() - 1, m_BtnDelegateRoundItem);
    connect(ui->m_addRoundItemPushButton, SIGNAL(clicked()), this, SLOT(onAddRoundItemButtonClicked()));
    connect(ui->m_roundItemEditPushButton, SIGNAL(clicked()), this, SLOT(onEditRoundItemButtonClicked()));

    ui->m_roundItemTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->m_roundItemTableView, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showRoundItemContextMenu(const QPoint &)));
    m_roundItemContextMenu = new QMenu(this);

    m_setAreaMenu = new QMenu(QString::fromLocal8Bit("设置分区"), this);
    m_roundItemContextMenu->addMenu(m_setAreaMenu);

    m_setRoundMenu = new QMenu(QString::fromLocal8Bit("设置轮次"), this);
    m_roundItemContextMenu->addMenu(m_setRoundMenu);

    m_setStrapMenu = new QMenu(QString::fromLocal8Bit("设置投退"), this);
    m_roundItemContextMenu->addMenu(m_setStrapMenu);

    m_setLoadTypeMenu = new QMenu(QString::fromLocal8Bit("设置负荷类型"), this);
    m_roundItemContextMenu->addMenu(m_setLoadTypeMenu);

    m_roundItemContextMenu->addAction(QString::fromLocal8Bit("设置关联装置"), this, SLOT(onSetDevice()));

    updateComboBox(ui->m_areaFilterComboBox, m_pDbManager->m_subAreaIdNameMap);
    updateComboBox(ui->m_roundFilterComboBox, m_pDbManager->m_roundIdNameMap);
    updateComboBox(ui->m_strapFilterComboBox, m_pDbManager->m_strapMap);
    updateComboBox(ui->m_loadTypeFilterComboBox, m_pDbManager->m_loadTypeMap);
    updateComboBox(ui->m_deviceFilterComboBox, m_pDbManager->m_deviceIdNameMap);
}

void MainWindow::setupDeviceTable()
{
    // 区域模型
    m_deviceModel = new QStandardItemModel(this);
    m_deviceModel->setHorizontalHeaderLabels(QStringList() << QString::fromLocal8Bit("编号")
                                                           << QString::fromLocal8Bit("名称")
                                                           << QString::fromLocal8Bit("装置类型")
                                                           << QString::fromLocal8Bit("功能类型")
                                                           << QString::fromLocal8Bit("关联轮次项数")
                                                           << QString::fromLocal8Bit("所属厂站")
                                                           << QString::fromLocal8Bit("操作"));

    // ui->m_tableViewDevice->setAlternatingRowColors(true);
    ui->m_tableViewDevice->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->m_tableViewDevice->setModel(m_deviceModel);
    ui->m_tableViewDevice->setItemDelegateForColumn(2, new DMDelegateInt(m_pDbManager->m_deviceTypeMap, ui->m_tableViewDevice));
    ui->m_tableViewDevice->setItemDelegateForColumn(3, new DMDelegateInt(m_pDbManager->m_deviceFuncTypeMap, ui->m_tableViewDevice));
    ui->m_tableViewDevice->setItemDelegateForColumn(5, new DMDelegateString(m_pDbManager->m_staIdNameMap, ui->m_tableViewDevice));
    m_BtnDelegateDevice = new OperationDelegate(ui->m_tableViewDevice);
    connect(m_BtnDelegateDevice, SIGNAL(detailButtonClicked(QModelIndex)), this, SLOT(onDetailButtonDeviceClicked(QModelIndex)));
    connect(m_BtnDelegateDevice, SIGNAL(deleteButtonClicked(QModelIndex)), this, SLOT(onDeleteButtonDeviceClicked(QModelIndex)));
    connect(m_BtnDelegateDevice, SIGNAL(modifyButtonClicked(QModelIndex)), this, SLOT(onModifyButtonDeviceClicked(QModelIndex)));
    ui->m_tableViewDevice->setItemDelegateForColumn(6, m_BtnDelegateDevice);
    connect(ui->m_pushButtonDevice, SIGNAL(clicked()), this, SLOT(onAddDeviceButtonClicked()));
    connect(ui->m_tableViewDevice, SIGNAL(clicked(QModelIndex)), this, SLOT(onDeviceTableRowSelected(QModelIndex)));
}

void MainWindow::setupDeviceParaTable()
{
    m_deviceParaModel = new QStandardItemModel(this);
    m_deviceParaModel->setHorizontalHeaderLabels(QStringList() << QString::fromLocal8Bit("装置编号")
                                                               << QString::fromLocal8Bit("轮次编号")
                                                               << QString::fromLocal8Bit("压板ID")
                                                               << QString::fromLocal8Bit("频率或电压定值ID")
                                                               << QString::fromLocal8Bit("动作延时定值ID")
                                                               << QString::fromLocal8Bit("告警信号ID")
                                                               << QString::fromLocal8Bit("动作信号ID")
                                                               << QString::fromLocal8Bit("出口矩阵ID")
                                                               << QString::fromLocal8Bit("操作"));

    // ui->m_tableViewDevicePara->setAlternatingRowColors(true);
    ui->m_tableViewDevicePara->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->m_tableViewDevicePara->setModel(m_deviceParaModel);
    ui->m_tableViewDevicePara->setWordWrap(true);
    ui->m_tableViewDevicePara->hideColumn(0);
    ui->m_tableViewDevicePara->setItemDelegateForColumn(1, new DMDelegateInt(m_pDbManager->m_roundIdNameMap, ui->m_tableViewDevicePara));
    ui->m_tableViewDevicePara->setItemDelegateForColumn(2, new DMDelegateMulti<QString>(m_pDbManager->m_fixValueMap, ui->m_tableViewDevicePara));
    ui->m_tableViewDevicePara->setItemDelegateForColumn(3, new DMDelegateString(m_pDbManager->m_fixValueMap, ui->m_tableViewDevicePara));
    ui->m_tableViewDevicePara->setItemDelegateForColumn(4, new DMDelegateString(m_pDbManager->m_fixValueMap, ui->m_tableViewDevicePara));
    ui->m_tableViewDevicePara->setItemDelegateForColumn(5, new DMDelegateMulti<QString>(m_pDbManager->m_tsIdNameMap, ui->m_tableViewDevicePara));
    ui->m_tableViewDevicePara->setItemDelegateForColumn(6, new DMDelegateString(m_pDbManager->m_tsIdNameMap, ui->m_tableViewDevicePara));
    ui->m_tableViewDevicePara->setItemDelegateForColumn(7, new DMDelegateString(m_pDbManager->m_fixValueMap, ui->m_tableViewDevicePara));

    m_BtnDelegateDevicePara = new OperationDelegate(ui->m_tableViewDevicePara);
    connect(m_BtnDelegateDevicePara, SIGNAL(detailButtonClicked(QModelIndex)), this, SLOT(onDetailButtonDeviceParaClicked(QModelIndex)));
    connect(m_BtnDelegateDevicePara, SIGNAL(deleteButtonClicked(QModelIndex)), this, SLOT(onDeleteButtonDeviceParaClicked(QModelIndex)));
    connect(m_BtnDelegateDevicePara, SIGNAL(modifyButtonClicked(QModelIndex)), this, SLOT(onModifyButtonDeviceParaClicked(QModelIndex)));
    ui->m_tableViewDevicePara->setItemDelegateForColumn(m_deviceParaModel->columnCount() - 1, m_BtnDelegateDevicePara);
    connect(ui->m_pushButtonDevicePara, SIGNAL(clicked()), this, SLOT(onAddDeviceParaButtonClicked()));

    connect(ui->m_callPushButton, SIGNAL(clicked()), this, SLOT(onCallButtonClicked()));

    ui->m_tableViewDevicePara->resizeColumnsToContents();
    ui->m_tableViewDevicePara->resizeRowsToContents();
}

/**
 * @brief 加载区域数据
 *
 * @param areaList
 */
void MainWindow::populateAreaModel(const QVector<AreaVo> &areaList)
{
    int rowCount = areaList.size();
    m_areaModel->setRowCount(rowCount);
    for (int i = 0; i < rowCount; i++)
    {
        const AreaVo &area = areaList[i];
        m_areaModel->setItem(i, 0, new QStandardItem(QString::number(area.id)));
        m_areaModel->setItem(i, 1, new QStandardItem(QString::fromLocal8Bit(area.name)));
        m_areaModel->setItem(i, 2, new QStandardItem(QString::number(area.type)));
        m_areaModel->setItem(i, 3, new QStandardItem(area.staId));
        m_areaModel->setItem(i, 4, new QStandardItem());
    }
}

void MainWindow::onModuleItemClicked(QListWidgetItem *item)
{
    // 获取点击的item的索引
    int index = ui->m_listWidget->row(item);
    // 切换到相应的页面
    ui->m_stackedWidget->setCurrentIndex(index);

    if (index == 0)
    {
        readAreaTable();
        ui->m_tableViewArea->resizeColumnsToContents();
    }
    else if (index == 1)
    {
        readDeviceTable();
        ui->m_tableViewDevice->resizeColumnsToContents();
    }
    else if (index == 2)
    {
        readRoundTable();
    }
    else if (index == 3)
    {
        readTaskTable();
        ui->m_taskTableView->resizeColumnsToContents();
    }
    else if (index == 4)
    {
        // TODO
    }
}

void MainWindow::readAreaTable()
{
    m_pDbManager->reloadAreaTable();
    populateAreaModel(m_pDbManager->m_areaList);
}

void MainWindow::populateRoundModel(const QVector<RoundDto> &roundList)
{
    m_roundModel->removeRows(0, m_roundModel->rowCount());

    QStandardItem *rootItem = new QStandardItem(QString::fromLocal8Bit("全部"));
    m_roundModel->appendRow(QList<QStandardItem *>()
                            << rootItem
                            << new QStandardItem(QString::number(-1)));

    for (const auto &round : roundList)
    {
        rootItem->appendRow(QList<QStandardItem *>()
                            << new QStandardItem(QString::fromLocal8Bit(round.name))
                            << new QStandardItem(QString::number(round.id))
                            << new QStandardItem(QString::number(round.areaId))
                            << new QStandardItem(QString::number(round.funcType))
                            << new QStandardItem(QString::number(round.roundType))
                            << new QStandardItem(QString::number(round.fixValue))
                            << new QStandardItem(QString::number(round.timeValue))
                            << new QStandardItem(QString::number(round.requirePower)));
    }

    ui->m_roundTreeView->expandAll();
    // int columnWidth = ui->m_roundTreeView->columnWidth(0);
    //  ui->m_roundTreeView->setFixedWidth(columnWidth);
}

void MainWindow::readRoundTable()
{
    m_pDbManager->reloadRound();
    populateRoundModel(m_pDbManager->m_roundList);
}

void MainWindow::showAreaDialog(const QList<QPair<QString, QVariant>> &data, int act, const QModelIndex &index)
{
    CommonFormDialog dialog(data, act, this);
    static const QStringList sTitleList = {QString::fromLocal8Bit("区域详情"), QString::fromLocal8Bit("区域修改"), QString::fromLocal8Bit("区域新增")};
    dialog.setWindowTitle(sTitleList[act]);
    if (act == CommonFormDialog::TYPE_MODIFY)
    {
        dialog.setEnabled(QString::fromLocal8Bit("编号"), false);
    }
    if (dialog.exec() == QDialog::Accepted)
    {
        QList<QPair<QString, QVariant>> updatedData = dialog.getData();

        AreaVo area = extractAreaData(updatedData);

        if (act == CommonFormDialog::TYPE_MODIFY)
        {
            if (m_pDbManager->updateAreaTable(area) != CS_SUCCEED)
            {
                QMessageBox::warning(this, QString::fromLocal8Bit("修改失败"), QString::fromLocal8Bit("修改失败"));
                return;
            }
            updateAreaModel(area, index.row());
            m_pDbManager->reloadAreaTable();
            m_pDbManager->loadRdb("低周减载区域参数表");
            m_pDbManager->loadRdb("低周减载轮次项视图");
            updateComboBox(ui->m_areaFilterComboBox, m_pDbManager->m_subAreaIdNameMap);
        }
        else
        {
            if (m_pDbManager->insertAreaTable(area) != CS_SUCCEED)
            {
                QMessageBox::warning(this, QString::fromLocal8Bit("新增失败"), QString::fromLocal8Bit("新增失败"));
                return;
            }
            updateAreaModel(area);
            m_pDbManager->reloadAreaTable();
            m_pDbManager->loadRdb("低周减载区域参数表");
            m_pDbManager->loadRdb("低周减载轮次项视图");
            updateComboBox(ui->m_areaFilterComboBox, m_pDbManager->m_subAreaIdNameMap);
        }
    }
    ui->m_tableViewArea->resizeColumnsToContents();
}

void MainWindow::onDetailButtonAreaClicked(QModelIndex index)
{
    AreaVo area;
    memset(&area, 0, sizeof(area));
    area.id = m_areaModel->index(index.row(), 0).data().toInt();
    strncpy(area.name, m_areaModel->index(index.row(), 1).data().toString().toLocal8Bit().data(), sizeof(area.name) - 1);
    area.type = m_areaModel->index(index.row(), 2).data().toInt();
    strncpy(area.staId, m_areaModel->index(index.row(), 3).data().toString().toLocal8Bit().data(), sizeof(area.staId) - 1);

    QList<QPair<QString, QVariant>> data;
    populateAreaData(data, area);

    showAreaDialog(data, CommonFormDialog::TYPE_DETAIL, index);
}

void MainWindow::onDeleteButtonAreaClicked(QModelIndex index)
{
    int id = m_areaModel->index(index.row(), 0).data().toInt();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  QString::fromLocal8Bit("删除"),
                                  QString::fromLocal8Bit("确定删除编号为%1的区域吗?").arg(id),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No)
        return;

    if (m_pDbManager->deleteTable(id, "xopensdb.dbo.低周减载区域参数表") != CS_SUCCEED)
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("删除失败"), QString::fromLocal8Bit("删除失败"));
        return;
    }

    if (m_pDbManager->deleteTable(id, "xopensdb.dbo.低周减载区域厂站关联表", "所属分区") != CS_SUCCEED)
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("删除失败"), QString::fromLocal8Bit("删除失败"));
        return;
    }

    m_areaModel->removeRow(index.row());
    m_pDbManager->reloadAreaTable();
    m_pDbManager->loadRdb("低周减载区域参数表");
    m_pDbManager->loadRdb("低周减载轮次项视图");
    updateComboBox(ui->m_areaFilterComboBox, m_pDbManager->m_subAreaIdNameMap);
}

void MainWindow::onModifyButtonAreaClicked(QModelIndex index)
{
    AreaVo area;
    memset(&area, 0, sizeof(area));
    area.id = m_areaModel->index(index.row(), 0).data().toInt();
    strncpy(area.name, m_areaModel->index(index.row(), 1).data().toString().toLocal8Bit().data(), sizeof(area.name) - 1);
    area.type = m_areaModel->index(index.row(), 2).data().toInt();
    strncpy(area.staId, m_areaModel->index(index.row(), 3).data().toString().toLocal8Bit().data(), sizeof(area.staId) - 1);

    QList<QPair<QString, QVariant>> data;
    populateAreaData(data, area);

    showAreaDialog(data, CommonFormDialog::TYPE_MODIFY, index);
}

void MainWindow::onAddButtonAreaClicked()
{
    AreaVo area;
    memset(&area, 0, sizeof(area));
    area.id = m_pDbManager->getMaxIDFromDataBase("xopensdb.dbo.低周减载区域参数表");
    strncpy(area.name, "", sizeof(area.name) - 1);
    area.type = 2;
    strncpy(area.staId, "", sizeof(area.staId) - 1);

    QList<QPair<QString, QVariant>> data;
    populateAreaData(data, area);

    showAreaDialog(data, CommonFormDialog::TYPE_ADD, QModelIndex());
}

/**
 * @brief 轮次点击事件
 *
 * @param current
 */
void MainWindow::onRoundSelectionModelChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList selectedIndexes = selected.indexes();
    if (!selectedIndexes.isEmpty())
    {
        QModelIndex index = selectedIndexes.first();

        QString areaName = m_pDbManager->m_allAreaIdNameMap.value(m_roundModel->sibling(index.row(), 2, index).data().toInt(), QString::fromLocal8Bit("未知"));
        ui->m_roundAreaLineEdit->setText(areaName);

        QString funcType = m_pDbManager->m_roundFuncTypeMap.value(m_roundModel->sibling(index.row(), 3, index).data().toInt(), QString::fromLocal8Bit("未知"));
        ui->m_roundFuncLineEdit->setText(funcType);

        QString roundType = m_pDbManager->m_roundTypeMap.value(m_roundModel->sibling(index.row(), 4, index).data().toInt(), QString::fromLocal8Bit("未知"));
        ui->m_roundTypeLineEdit->setText(roundType);

        ui->m_roundValueLineEdit->setText(m_roundModel->sibling(index.row(), 5, index).data().toString());
        ui->m_roundTimeLineEdit->setText(m_roundModel->sibling(index.row(), 6, index).data().toString());
        ui->m_roundRequireLineEdit->setText(m_roundModel->sibling(index.row(), 7, index).data().toString());

        readRoundItemTable(m_roundModel->sibling(index.row(), 1, index).data().toInt());
    }
}

void MainWindow::showRoundContextMenu(const QPoint &pos)
{
    m_roundContextMenu->exec(ui->m_roundTreeView->viewport()->mapToGlobal(pos));
}

void MainWindow::populateAreaData(QList<QPair<QString, QVariant>> &data, const AreaVo &area)
{
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("编号"), QString::number(area.id)));
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("名称"), QString::fromLocal8Bit(area.name)));

    ComboBoxData<int> typeData;
    typeData.currentValue = area.type;
    typeData.options = m_pDbManager->m_areaTypeMap;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("类型"), QVariant::fromValue(typeData)));

    ComboBoxData<QString> staData;
    staData.currentValue = area.staId;
    staData.options = m_pDbManager->m_staIdNameMap;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("关联厂站"), QVariant::fromValue(staData)));
}

AreaVo MainWindow::extractAreaData(const QList<QPair<QString, QVariant>> &updatedData)
{
    AreaVo newArea;
    memset(&newArea, 0, sizeof(newArea));
    for (const auto &pair : updatedData)
    {
        if (pair.first == QString::fromLocal8Bit("编号"))
        {
            newArea.id = pair.second.toInt();
        }
        else if (pair.first == QString::fromLocal8Bit("名称"))
        {
            strncpy(newArea.name, pair.second.toString().toLocal8Bit().data(), sizeof(newArea.name) - 1);
        }
        else if (pair.first == QString::fromLocal8Bit("类型"))
        {
            newArea.type = pair.second.toInt();
        }
        else if (pair.first == QString::fromLocal8Bit("关联厂站"))
        {
            strncpy(newArea.staId, pair.second.toString().toLocal8Bit().data(), sizeof(newArea.staId) - 1);
        }
    }
    return newArea;
}

void MainWindow::updateAreaModel(const AreaVo &newArea, int row)
{
    if (row == -1)
    {
        row = m_areaModel->rowCount();
    }
    m_areaModel->setItem(row, 0, new QStandardItem(QString::number(newArea.id)));
    m_areaModel->setItem(row, 1, new QStandardItem(QString::fromLocal8Bit(newArea.name)));
    m_areaModel->setItem(row, 2, new QStandardItem(QString::number(newArea.type)));
    m_areaModel->setItem(row, 3, new QStandardItem(QString::fromLocal8Bit(newArea.staId)));
    m_areaModel->setItem(row, 4, new QStandardItem(""));
}

void MainWindow::populateRoundData(QList<QPair<QString, QVariant>> &data, const RoundDto &round)
{
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("编号"), QString::number(round.id)));
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("名称"), QString::fromLocal8Bit(round.name)));
    ComboBoxData<int> comboBoxArea;
    comboBoxArea.currentValue = round.areaId;
    comboBoxArea.options = m_pDbManager->m_allAreaIdNameMap;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("所属地区"), QVariant::fromValue(comboBoxArea)));
    ComboBoxData<int> comboBoxFunc;
    comboBoxFunc.currentValue = round.funcType;
    comboBoxFunc.options = m_pDbManager->m_roundFuncTypeMap;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("功能类型"), QVariant::fromValue(comboBoxFunc)));
    ComboBoxData<int> comboBoxType;
    comboBoxType.currentValue = round.roundType;
    comboBoxType.options = m_pDbManager->m_roundTypeMap;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("轮次类型"), QVariant::fromValue(comboBoxType)));
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("频率/电压整定值"), round.fixValue));
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("动作延时整定值"), round.timeValue));
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("下发应切荷量"), round.requirePower));
}

RoundDto MainWindow::extractRoundData(const QList<QPair<QString, QVariant>> &updatedData)
{
    RoundDto newRound;
    memset(&newRound, 0, sizeof(newRound));
    for (const auto &pair : updatedData)
    {
        if (pair.first == QString::fromLocal8Bit("编号"))
        {
            newRound.id = pair.second.toInt();
        }
        else if (pair.first == QString::fromLocal8Bit("名称"))
        {
            strncpy(newRound.name, pair.second.toString().toLocal8Bit().data(), sizeof(newRound.name) - 1);
        }
        else if (pair.first == QString::fromLocal8Bit("所属地区"))
        {
            newRound.areaId = pair.second.toInt();
        }
        else if (pair.first == QString::fromLocal8Bit("功能类型"))
        {
            newRound.funcType = pair.second.toInt();
        }
        else if (pair.first == QString::fromLocal8Bit("轮次类型"))
        {
            newRound.roundType = pair.second.toInt();
        }
        else if (pair.first == QString::fromLocal8Bit("频率/电压整定值"))
        {
            newRound.fixValue = pair.second.toDouble();
        }
        else if (pair.first == QString::fromLocal8Bit("动作延时整定值"))
        {
            newRound.timeValue = pair.second.toDouble();
        }
        else if (pair.first == QString::fromLocal8Bit("下发应切荷量"))
        {
            newRound.requirePower = pair.second.toDouble();
        }
    }
    return newRound;
}

void MainWindow::updateRoundModel(const RoundDto &newRound, const QModelIndex &index)
{
    if (index.parent().isValid() && index.isValid())
    {
        QStandardItem *parentItem = m_roundModel->itemFromIndex(index.parent());
        parentItem->child(index.row(), 0)->setText(QString::fromLocal8Bit(newRound.name));
        parentItem->child(index.row(), 1)->setText(QString::number(newRound.id));
        parentItem->child(index.row(), 2)->setText(QString::number(newRound.areaId));
        parentItem->child(index.row(), 3)->setText(QString::number(newRound.funcType));
        parentItem->child(index.row(), 4)->setText(QString::number(newRound.roundType));
        parentItem->child(index.row(), 5)->setText(QString::number(newRound.fixValue));
        parentItem->child(index.row(), 6)->setText(QString::number(newRound.timeValue));
        parentItem->child(index.row(), 7)->setText(QString::number(newRound.requirePower));
    }
    else
    {
        QStandardItem *rootItem = m_roundModel->item(0);
        rootItem->appendRow(QList<QStandardItem *>()
                            << new QStandardItem(QString::fromLocal8Bit(newRound.name))
                            << new QStandardItem(QString::number(newRound.id))
                            << new QStandardItem(QString::number(newRound.areaId))
                            << new QStandardItem(QString::number(newRound.funcType))
                            << new QStandardItem(QString::number(newRound.roundType))
                            << new QStandardItem(QString::number(newRound.fixValue))
                            << new QStandardItem(QString::number(newRound.timeValue))
                            << new QStandardItem(QString::number(newRound.requirePower)));
    }
}

int MainWindow::showRoundDialog(const QList<QPair<QString, QVariant>> &data, int act, const QModelIndex &index)
{
    CommonFormDialog dialog(data, act, this);
    static const QStringList sTitleList = {QString::fromLocal8Bit("轮次详情"), QString::fromLocal8Bit("轮次修改"), QString::fromLocal8Bit("轮次新增")};
    dialog.setWindowTitle(sTitleList[act]);
    if (act == CommonFormDialog::TYPE_MODIFY)
    {
        dialog.setEnabled(QString::fromLocal8Bit("编号"), false);
    }

    if (dialog.exec() == QDialog::Accepted)
    {
        QList<QPair<QString, QVariant>> updatedData = dialog.getData();

        RoundDto round = extractRoundData(updatedData);

        if (act == CommonFormDialog::TYPE_MODIFY)
        {
            if (m_pDbManager->updateRoundTable(round) != CS_SUCCEED)
            {
                QMessageBox::warning(this, QString::fromLocal8Bit("修改失败"), QString::fromLocal8Bit("修改失败"));
                return -1;
            }
            updateRoundModel(round, index);
            m_pDbManager->reloadRound();
            updateComboBox(ui->m_roundFilterComboBox, m_pDbManager->m_roundIdNameMap);
            QItemSelection newSelection(index, index);
            onRoundSelectionModelChanged(newSelection, QItemSelection());

            m_pDbManager->loadRdb("低周减载轮次参数表");
            m_pDbManager->loadRdb("低周减载轮次项视图");
        }
        else
        {
            if (m_pDbManager->insertRoundTable(round) != CS_SUCCEED)
            {
                QMessageBox::warning(this, QString::fromLocal8Bit("新增失败"), QString::fromLocal8Bit("新增失败"));
                return -1;
            }
            updateRoundModel(round, QModelIndex());
            m_pDbManager->reloadRound();
            updateComboBox(ui->m_roundFilterComboBox, m_pDbManager->m_roundIdNameMap);
            QStandardItem *rootItem = m_roundModel->item(0);
            QModelIndex newIndex = rootItem->child(rootItem->rowCount() - 1, 0)->index();
            ui->m_roundTreeView->setCurrentIndex(newIndex);

            m_pDbManager->loadRdb("低周减载轮次参数表");
            m_pDbManager->loadRdb("低周减载轮次项视图");
        }
    }

    return 0;
}

void MainWindow::onAddRoundActionTriggered()
{
    RoundDto newRound;
    newRound.id = m_pDbManager->getMaxIDFromDataBase("xopensdb.dbo.低周减载轮次参数表");
    strcpy(newRound.name, "");
    newRound.areaId = 1;
    newRound.funcType = 1;
    newRound.roundType = 1;
    newRound.fixValue = 49;
    newRound.timeValue = 0.3;
    newRound.requirePower = 0.0;

    QList<QPair<QString, QVariant>> data;
    populateRoundData(data, newRound);

    showRoundDialog(data, CommonFormDialog::TYPE_ADD, QModelIndex());
}

void MainWindow::onModifyRoundActionTriggered()
{
    QModelIndex currentIndex = ui->m_roundTreeView->currentIndex();

    if (!currentIndex.parent().isValid())
    {
        return;
    }

    RoundDto currentRound;
    currentRound.id = m_roundModel->index(currentIndex.row(), 1, currentIndex.parent()).data().toInt();
    strncpy(currentRound.name, m_roundModel->index(currentIndex.row(), 0, currentIndex.parent()).data().toString().toLocal8Bit().data(), sizeof(currentRound.name) - 1);
    currentRound.areaId = m_roundModel->index(currentIndex.row(), 2, currentIndex.parent()).data().toInt();
    currentRound.funcType = m_roundModel->index(currentIndex.row(), 3, currentIndex.parent()).data().toInt();
    currentRound.roundType = m_roundModel->index(currentIndex.row(), 4, currentIndex.parent()).data().toInt();
    currentRound.fixValue = m_roundModel->index(currentIndex.row(), 5, currentIndex.parent()).data().toDouble();
    currentRound.timeValue = m_roundModel->index(currentIndex.row(), 6, currentIndex.parent()).data().toDouble();
    currentRound.requirePower = m_roundModel->index(currentIndex.row(), 7, currentIndex.parent()).data().toDouble();

    QList<QPair<QString, QVariant>> data;
    populateRoundData(data, currentRound);

    showRoundDialog(data, CommonFormDialog::TYPE_MODIFY, currentIndex);
}

void MainWindow::onDeleteRoundActionTriggered()
{
    QModelIndex currentIndex = ui->m_roundTreeView->currentIndex();

    if (!currentIndex.parent().isValid())
    {
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  QString::fromLocal8Bit("删除"),
                                  QString::fromLocal8Bit("确定删除轮次[%1]吗?")
                                      .arg(m_roundModel->index(currentIndex.row(), 0, currentIndex.parent()).data().toString()),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No)
        return;

    int id = m_roundModel->index(currentIndex.row(), 1, currentIndex.parent()).data().toInt();
    if (m_pDbManager->deleteRoundTable(id) != CS_SUCCEED)
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("删除失败"), QString::fromLocal8Bit("删除失败"));
        return;
    }
    m_roundModel->itemFromIndex(currentIndex.parent())->removeRow(currentIndex.row());
    m_pDbManager->reloadRound();

    m_pDbManager->loadRdb("低周减载轮次参数表");
    m_pDbManager->loadRdb("低周减载轮次项视图");

    updateComboBox(ui->m_roundFilterComboBox, m_pDbManager->m_roundIdNameMap);
}

void MainWindow::readRoundItemTable(int roundId)
{
    QVector<RoundItemDto> roundItemList = m_pDbManager->getRoundItemList(roundId);
    populateRoundItemModel(roundItemList);

    ui->m_roundItemTableView->resizeColumnsToContents();
}

void MainWindow::populateRoundItemModel(const QVector<RoundItemDto> &roundItemList)
{
    int rowCount = roundItemList.size();
    m_roundItemModel->setRowCount(rowCount);
    for (int i = 0; i < rowCount; i++)
    {
        QStandardItem *item0 = new QStandardItem();
        item0->setData(roundItemList[i].id, Qt::EditRole);
        item0->setData(roundItemList[i].id, Qt::DisplayRole);
        m_roundItemModel->setItem(i, 0, item0);

        // m_roundItemModel->setItem(i, 0, new QStandardItem(QString::number(roundItemList[i].id)));
        m_roundItemModel->setItem(i, 1, new QStandardItem(QString::fromLocal8Bit(roundItemList[i].name)));
        m_roundItemModel->setItem(i, 2, new QStandardItem(QString::number(roundItemList[i].areaId)));
        m_roundItemModel->setItem(i, 3, new QStandardItem(QString::number(roundItemList[i].roundId)));
        m_roundItemModel->setItem(i, 4, new QStandardItem(roundItemList[i].linkedLine));
        m_roundItemModel->setItem(i, 5, new QStandardItem(QString::number(roundItemList[i].loadType)));
        m_roundItemModel->setItem(i, 6, new QStandardItem(QString::number(roundItemList[i].strapPlan)));
        m_roundItemModel->setItem(i, 7, new QStandardItem(roundItemList[i].linkedBreak));
        m_roundItemModel->setItem(i, 8, new QStandardItem(roundItemList[i].pName));
        m_roundItemModel->setItem(i, 9, new QStandardItem(QString::number(roundItemList[i].deviceId)));
        m_roundItemModel->setItem(i, 10, new QStandardItem(QString::number(roundItemList[i].assocexit)));
    }
}

void MainWindow::onAddRoundItemButtonClicked()
{
    QModelIndex currentIndex = ui->m_roundTreeView->currentIndex();
    int roundId = 0;
    if (currentIndex.isValid() && currentIndex.parent().isValid())
    {
        roundId = m_roundModel->sibling(currentIndex.row(), 4, currentIndex).data().toInt();
    }
    else
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("请选择轮次"));
        return;
    }

    RoundItemDto item;
    memset(&item, 0, sizeof(item));
    item.id = m_pDbManager->getMaxIDFromDataBase("xopensdb.dbo.低周减载轮次项参数表");
    strncpy(item.name, "", sizeof(item.name) - 1);
    item.areaId = 0;
    item.roundId = roundId;
    strcpy(item.linkedLine, "");
    item.loadType = 5;
    item.strapPlan = 0;
    strcpy(item.linkedBreak, "");
    strcpy(item.pName, "");
    item.deviceId = 0;
    item.assocexit = 0;

    QList<QPair<QString, QVariant>> data;
    populateRoundItemData(data, item);

    showRoundItemDialog(data, CommonFormDialog::TYPE_ADD, QModelIndex());
}

void MainWindow::onEditRoundItemButtonClicked()
{
    QModelIndex currentIndex = ui->m_roundTreeView->currentIndex();
    int roundId = 0;
    if (currentIndex.isValid() && currentIndex.parent().isValid())
    {
        roundId = m_roundModel->sibling(currentIndex.row(), 4, currentIndex).data().toInt();
    }
    else
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("请选择轮次"));
        return;
    }

    QStringList oldIds;
    int rowCount = m_roundItemModel->rowCount();
    for (int i = 0; i < rowCount; ++i)
    {
        int id = m_roundItemModel->item(i, 0)->data(Qt::EditRole).toInt();
        oldIds.append(QString::number(id));
    }
    DFLOG_DEBUG("oldIds: %s", oldIds.join(",").toLocal8Bit().data());

    dfJson::Value roundItemJson = m_pDbManager->getRoundItemJson();
    JsonTreeDialog dialog(roundItemJson, true, this);
    dialog.setSelectedIds(oldIds);
    if (dialog.exec() == QDialog::Accepted)
    {
        QStringList newIds = dialog.getSelectedIds();

        QStringList deselectedIds;
        for (const QString &id : oldIds)
        {
            if (!newIds.contains(id))
            {
                deselectedIds.append(id);
            }
        }

        if (!deselectedIds.isEmpty())
        {
            QString qsSql = QString::fromLocal8Bit("update xopensdb.低周减载轮次项参数表 set 所属轮次=0 where 编号 in (%1)")
                                .arg(deselectedIds.join(","));
            if (!m_pDbManager->updateTable(qsSql))
            {
                QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("更新失败"));
                return;
            }
        }

        if (!newIds.isEmpty())
        {
            QString qsSql = QString::fromLocal8Bit("update xopensdb.低周减载轮次项参数表 set 所属轮次=%1 where 编号 in (%2)")
                                .arg(roundId)
                                .arg(newIds.join(","));

            if (!m_pDbManager->updateTable(qsSql))
            {
                QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("更新失败"));
                return;
            }
        }

        readRoundItemTable(roundId);
    }
}

void MainWindow::populateRoundItemData(QList<QPair<QString, QVariant>> &data, const RoundItemDto &item)
{
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("编号"), QString::number(item.id)));
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("名称"), QString::fromLocal8Bit(item.name)));
    ComboBoxData<int> comboBoxArea;
    comboBoxArea.currentValue = item.areaId;
    comboBoxArea.options = m_pDbManager->m_subAreaIdNameMap;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("所属分区"), QVariant::fromValue(comboBoxArea)));

    ComboBoxData<int> comboBoxRound;
    comboBoxRound.currentValue = item.roundId;
    comboBoxRound.options = m_pDbManager->m_roundIdNameMap;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("所属轮次"), QVariant::fromValue(comboBoxRound)));

    JsonDialogData lineData;
    lineData.initialText = item.linkedLine;
    lineData.isMultiSelect = false;
    lineData.jsonData = m_pDbManager->m_lineJson;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("关联馈线"), QVariant::fromValue(lineData)));

    ComboBoxData<int> comboBoxLoadType;
    comboBoxLoadType.currentValue = item.loadType;
    comboBoxLoadType.options = m_pDbManager->m_loadTypeMap;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("负荷类型"), QVariant::fromValue(comboBoxLoadType)));

    ComboBoxData<int> comboBoxStrapType;
    comboBoxStrapType.currentValue = item.strapPlan;
    comboBoxStrapType.options = m_pDbManager->m_strapMap;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("投退计划"), QVariant::fromValue(comboBoxStrapType)));

    JsonDialogData breakData;
    breakData.initialText = item.linkedBreak;
    breakData.isMultiSelect = false;
    breakData.jsonData = m_pDbManager->m_breakerJson;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("关联开关"), QVariant::fromValue(breakData)));

    JsonDialogData pData;
    pData.initialText = item.pName;
    pData.isMultiSelect = false;
    pData.jsonData = m_pDbManager->m_tmJson;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("有功代码"), QVariant::fromValue(pData)));

    JsonDialogData deviceData;
    deviceData.initialText = QString::number(item.deviceId);
    deviceData.isMultiSelect = false;
    deviceData.jsonData = m_pDbManager->m_deviceJson;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("关联装置"), QVariant::fromValue(deviceData)));

    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("关联出口"), item.assocexit));
}

RoundItemDto MainWindow::extractRoundItemData(const QList<QPair<QString, QVariant>> &updatedData)
{
    RoundItemDto newRoundItem;
    memset(&newRoundItem, 0, sizeof(newRoundItem));
    for (const auto &pair : updatedData)
    {
        if (pair.first == QString::fromLocal8Bit("编号"))
        {
            newRoundItem.id = pair.second.toInt();
        }
        else if (pair.first == QString::fromLocal8Bit("名称"))
        {
            strncpy(newRoundItem.name, pair.second.toString().toLocal8Bit().data(), sizeof(newRoundItem.name) - 1);
        }
        else if (pair.first == QString::fromLocal8Bit("所属分区"))
        {
            newRoundItem.areaId = pair.second.toInt();
        }
        else if (pair.first == QString::fromLocal8Bit("所属轮次"))
        {
            newRoundItem.roundId = pair.second.toInt();
        }
        else if (pair.first == QString::fromLocal8Bit("关联馈线"))
        {
            strncpy(newRoundItem.linkedLine, pair.second.toString().toLocal8Bit().data(), sizeof(newRoundItem.linkedLine) - 1);
        }
        else if (pair.first == QString::fromLocal8Bit("负荷类型"))
        {
            newRoundItem.loadType = pair.second.toInt();
        }
        else if (pair.first == QString::fromLocal8Bit("投退计划"))
        {
            newRoundItem.strapPlan = pair.second.toInt();
        }
        else if (pair.first == QString::fromLocal8Bit("关联开关"))
        {
            strncpy(newRoundItem.linkedBreak, pair.second.toString().toLocal8Bit().data(), sizeof(newRoundItem.linkedBreak) - 1);
        }
        else if (pair.first == QString::fromLocal8Bit("有功代码"))
        {
            strncpy(newRoundItem.pName, pair.second.toString().toLocal8Bit().data(), sizeof(newRoundItem.pName) - 1);
        }
        else if (pair.first == QString::fromLocal8Bit("关联装置"))
        {
            newRoundItem.deviceId = pair.second.toInt();
        }
        else if (pair.first == QString::fromLocal8Bit("关联出口"))
        {
            newRoundItem.assocexit = pair.second.toInt();
        }
    }
    return newRoundItem;
}

void MainWindow::showRoundItemDialog(const QList<QPair<QString, QVariant>> &data, int act, const QModelIndex &index)
{
    CommonFormDialog dialog(data, act, this);
    static const QStringList sTitleList = {
        QString::fromLocal8Bit("轮次项详情"),
        QString::fromLocal8Bit("轮次项修改"),
        QString::fromLocal8Bit("轮次项新增")};
    dialog.setWindowTitle(sTitleList[act]);
    if (act == CommonFormDialog::TYPE_MODIFY)
    {
        dialog.setEnabled(QString::fromLocal8Bit("编号"), false);
    }
    if (dialog.exec() == QDialog::Accepted)
    {
        QList<QPair<QString, QVariant>> updatedData = dialog.getData();

        RoundItemDto item = extractRoundItemData(updatedData);

        if (act == CommonFormDialog::TYPE_MODIFY)
        {
            if (m_pDbManager->updateRoundItemTable(item) != CS_SUCCEED)
            {
                QMessageBox::warning(this, QString::fromLocal8Bit("修改失败"), QString::fromLocal8Bit("修改失败"));
                return;
            }
            updateRoundItemModel(item, index.row());

            m_pDbManager->loadRdb("低周减载轮次项视图");
        }
        else
        {
            if (m_pDbManager->insertRoundItemTable(item) != CS_SUCCEED)
            {
                QMessageBox::warning(this, QString::fromLocal8Bit("新增失败"), QString::fromLocal8Bit("新增失败"));
                return;
            }
            updateRoundItemModel(item);

            m_pDbManager->loadRdb("低周减载轮次项视图");
        }
    }
    ui->m_roundItemTableView->resizeColumnsToContents();
}

void MainWindow::updateRoundItemModel(const RoundItemDto &newRoundItem, int row)
{
    if (row == -1)
    {
        row = m_roundItemModel->rowCount();
    }
    QStandardItem *item0 = new QStandardItem();
    item0->setData(newRoundItem.id, Qt::EditRole);
    item0->setData(newRoundItem.id, Qt::DisplayRole);
    m_roundItemModel->setItem(row, 0, item0);
    m_roundItemModel->setItem(row, 1, new QStandardItem(QString::fromLocal8Bit(newRoundItem.name)));
    m_roundItemModel->setItem(row, 2, new QStandardItem(QString::number(newRoundItem.areaId)));
    m_roundItemModel->setItem(row, 3, new QStandardItem(QString::number(newRoundItem.roundId)));
    m_roundItemModel->setItem(row, 4, new QStandardItem(newRoundItem.linkedLine));
    m_roundItemModel->setItem(row, 5, new QStandardItem(QString::number(newRoundItem.loadType)));
    m_roundItemModel->setItem(row, 6, new QStandardItem(QString::number(newRoundItem.strapPlan)));
    m_roundItemModel->setItem(row, 7, new QStandardItem(newRoundItem.linkedBreak));
    m_roundItemModel->setItem(row, 8, new QStandardItem(newRoundItem.pName));
    m_roundItemModel->setItem(row, 9, new QStandardItem(QString::number(newRoundItem.deviceId)));
    m_roundItemModel->setItem(row, 10, new QStandardItem(QString::number(newRoundItem.assocexit)));
}

void MainWindow::onDetailButtonRoundItemClicked(QModelIndex proxyIndex)
{
    QModelIndex sourceIndex = m_roundItemProxyModel->mapToSource(proxyIndex);

    RoundItemDto item;
    memset(&item, 0, sizeof(item));
    item.id = m_roundItemModel->index(sourceIndex.row(), 0).data().toInt();
    strncpy(item.name, m_roundItemModel->index(sourceIndex.row(), 1).data().toString().toLocal8Bit().data(), sizeof(item.name) - 1);
    item.areaId = m_roundItemModel->index(sourceIndex.row(), 2).data().toInt();
    item.roundId = m_roundItemModel->index(sourceIndex.row(), 3).data().toInt();
    strncpy(item.linkedLine, m_roundItemModel->index(sourceIndex.row(), 4).data().toString().toLocal8Bit().data(), sizeof(item.linkedLine) - 1);
    item.loadType = m_roundItemModel->index(sourceIndex.row(), 5).data().toInt();
    item.strapPlan = m_roundItemModel->index(sourceIndex.row(), 6).data().toInt();
    strncpy(item.linkedBreak, m_roundItemModel->index(sourceIndex.row(), 7).data().toString().toLocal8Bit().data(), sizeof(item.linkedBreak) - 1);
    strncpy(item.pName, m_roundItemModel->index(sourceIndex.row(), 8).data().toString().toLocal8Bit().data(), sizeof(item.pName) - 1);
    item.deviceId = m_roundItemModel->index(sourceIndex.row(), 9).data().toInt();
    item.assocexit = m_roundItemModel->index(sourceIndex.row(), 10).data().toInt();

    QList<QPair<QString, QVariant>> data;
    populateRoundItemData(data, item);

    showRoundItemDialog(data, CommonFormDialog::TYPE_DETAIL, sourceIndex);
}

void MainWindow::onDeleteButtonRoundItemClicked(QModelIndex proxyIndex)
{
    QModelIndex sourceIndex = m_roundItemProxyModel->mapToSource(proxyIndex);
    int id = m_roundItemModel->index(sourceIndex.row(), 0).data().toInt();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  QString::fromLocal8Bit("删除"),
                                  QString::fromLocal8Bit("确定删除编号为%1的轮次项吗?").arg(id),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No)
        return;

    if (m_pDbManager->deleteTable(id, "xopensdb.dbo.低周减载轮次项参数表") != CS_SUCCEED)
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("删除失败"), QString::fromLocal8Bit("删除失败"));
        return;
    }

    m_roundItemModel->removeRow(sourceIndex.row());

    m_pDbManager->loadRdb("低周减载轮次项视图");
}

void MainWindow::onModifyButtonRoundItemClicked(QModelIndex proxyIndex)
{
    QModelIndex sourceIndex = m_roundItemProxyModel->mapToSource(proxyIndex);

    RoundItemDto item;
    memset(&item, 0, sizeof(item));
    item.id = m_roundItemModel->index(sourceIndex.row(), 0).data().toInt();
    strncpy(item.name, m_roundItemModel->index(sourceIndex.row(), 1).data().toString().toLocal8Bit().data(), sizeof(item.name) - 1);
    item.areaId = m_roundItemModel->index(sourceIndex.row(), 2).data().toInt();
    item.roundId = m_roundItemModel->index(sourceIndex.row(), 3).data().toInt();
    strncpy(item.linkedLine, m_roundItemModel->index(sourceIndex.row(), 4).data().toString().toLocal8Bit().data(), sizeof(item.linkedLine) - 1);
    item.loadType = m_roundItemModel->index(sourceIndex.row(), 5).data().toInt();
    item.strapPlan = m_roundItemModel->index(sourceIndex.row(), 6).data().toInt();
    strncpy(item.linkedBreak, m_roundItemModel->index(sourceIndex.row(), 7).data().toString().toLocal8Bit().data(), sizeof(item.linkedBreak) - 1);
    strncpy(item.pName, m_roundItemModel->index(sourceIndex.row(), 8).data().toString().toLocal8Bit().data(), sizeof(item.pName) - 1);
    item.deviceId = m_roundItemModel->index(sourceIndex.row(), 9).data().toInt();
    item.assocexit = m_roundItemModel->index(sourceIndex.row(), 10).data().toInt();

    QList<QPair<QString, QVariant>> data;
    populateRoundItemData(data, item);

    showRoundItemDialog(data, CommonFormDialog::TYPE_MODIFY, sourceIndex);
}

void MainWindow::readDeviceTable()
{
    m_pDbManager->reloadDevice();
    populateDeviceModel(m_pDbManager->m_deviceList);
}

void MainWindow::populateDeviceModel(const QVector<DeviceDto> &deviceList)
{
    int rowCount = deviceList.size();
    m_deviceModel->setRowCount(rowCount);
    for (int i = 0; i < rowCount; i++)
    {
        const DeviceDto &device = deviceList[i];
        m_deviceModel->setItem(i, 0, new QStandardItem(QString::number(device.id)));
        m_deviceModel->setItem(i, 1, new QStandardItem(QString::fromLocal8Bit(device.name)));
        m_deviceModel->setItem(i, 2, new QStandardItem(QString::number(device.type)));
        m_deviceModel->setItem(i, 3, new QStandardItem(QString::number(device.funcType)));
        m_deviceModel->setItem(i, 4, new QStandardItem(QString::number(device.itemNums)));
        m_deviceModel->setItem(i, 5, new QStandardItem(QString::fromLocal8Bit(device.staId)));
    }
}

void MainWindow::onAddDeviceButtonClicked()
{
    DeviceDto device;
    memset(&device, 0, sizeof(device));
    device.id = 0;
    strncpy(device.name, "", sizeof(device.name) - 1);
    device.type = 1;
    device.funcType = 3;
    device.itemNums = 0;
    strcpy(device.staId, "");

    QList<QPair<QString, QVariant>> data;
    populateDeviceData(data, device);

    showDeviceDialog(data, CommonFormDialog::TYPE_ADD, QModelIndex());
}

void MainWindow::onDetailButtonDeviceClicked(QModelIndex index)
{
    DeviceDto device;
    memset(&device, 0, sizeof(device));
    device.id = m_deviceModel->index(index.row(), 0).data().toInt();
    strncpy(device.name, m_deviceModel->index(index.row(), 1).data().toString().toLocal8Bit().data(), sizeof(device.name) - 1);
    device.type = m_deviceModel->index(index.row(), 2).data().toInt();
    device.funcType = m_deviceModel->index(index.row(), 3).data().toInt();
    device.itemNums = m_deviceModel->index(index.row(), 4).data().toInt();
    strncpy(device.staId, m_deviceModel->index(index.row(), 5).data().toString().toLocal8Bit().data(), sizeof(device.staId) - 1);

    QList<QPair<QString, QVariant>> data;
    populateDeviceData(data, device);

    showDeviceDialog(data, CommonFormDialog::TYPE_DETAIL, index);
}

void MainWindow::onModifyButtonDeviceClicked(QModelIndex index)
{
    DeviceDto device;
    memset(&device, 0, sizeof(device));
    device.id = m_deviceModel->index(index.row(), 0).data().toInt();
    strncpy(device.name, m_deviceModel->index(index.row(), 1).data().toString().toLocal8Bit().data(), sizeof(device.name) - 1);
    device.type = m_deviceModel->index(index.row(), 2).data().toInt();
    device.funcType = m_deviceModel->index(index.row(), 3).data().toInt();
    device.itemNums = m_deviceModel->index(index.row(), 4).data().toInt();
    strncpy(device.staId, m_deviceModel->index(index.row(), 5).data().toString().toLocal8Bit().data(), sizeof(device.staId) - 1);

    QList<QPair<QString, QVariant>> data;
    populateDeviceData(data, device);

    showDeviceDialog(data, CommonFormDialog::TYPE_MODIFY, index);
}

void MainWindow::onDeleteButtonDeviceClicked(QModelIndex index)
{
    int id = m_deviceModel->index(index.row(), 0).data().toInt();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  QString::fromLocal8Bit("删除"),
                                  QString::fromLocal8Bit("确定删除编号为%1的装置吗?").arg(id),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No)
        return;

    if (m_pDbManager->deleteDeviceTable(id) != CS_SUCCEED)
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("删除失败"), QString::fromLocal8Bit("删除失败"));
        return;
    }

    m_deviceModel->removeRow(index.row());
    m_deviceParaModel->removeRows(0, m_deviceParaModel->rowCount());
    m_pDbManager->reloadDevice();
    m_pDbManager->loadRdb("低周减载轮次项视图");
    updateComboBox(ui->m_deviceFilterComboBox, m_pDbManager->m_deviceIdNameMap);
}

void MainWindow::populateDeviceData(QList<QPair<QString, QVariant>> &data, const DeviceDto &device)
{
    JsonDialogData rtuData;
    rtuData.initialText = QString::number(device.id);
    rtuData.isMultiSelect = false;
    rtuData.jsonData = m_pDbManager->m_rtuJson;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("编号"), QVariant::fromValue(rtuData)));

    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("名称"), QString::fromLocal8Bit(device.name)));

    ComboBoxData<int> comboBoxType;
    comboBoxType.currentValue = device.type;
    comboBoxType.options = m_pDbManager->m_deviceTypeMap;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("装置类型"), QVariant::fromValue(comboBoxType)));

    ComboBoxData<int> comboBoxFuncType;
    comboBoxFuncType.currentValue = device.funcType;
    comboBoxFuncType.options = m_pDbManager->m_deviceFuncTypeMap;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("功能类型"), QVariant::fromValue(comboBoxFuncType)));

    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("关联轮次项数"), QString::number(device.itemNums)));

    ComboBoxData<QString> staData;
    staData.currentValue = device.staId;
    staData.options = m_pDbManager->m_staIdNameMap;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("所属厂站"), QVariant::fromValue(staData)));
}

void MainWindow::showDeviceDialog(const QList<QPair<QString, QVariant>> &data, int act, const QModelIndex &index)
{
    CommonFormDialog dialog(data, act, this);
    static const QStringList sTitleList = {QString::fromLocal8Bit("装置详情"), QString::fromLocal8Bit("装置修改"), QString::fromLocal8Bit("装置新增")};
    dialog.setWindowTitle(sTitleList[act]);

    if (act == CommonFormDialog::TYPE_MODIFY)
    {
        dialog.setEnabled(QString::fromLocal8Bit("编号"), false);
        dialog.setEnabled(QString::fromLocal8Bit("所属厂站"), false);
    }

    dialog.setEnabled(QString::fromLocal8Bit("关联轮次项数"), false);

    if (dialog.exec() == QDialog::Accepted)
    {
        QList<QPair<QString, QVariant>> updatedData = dialog.getData();

        DeviceDto device = extractDeviceData(updatedData);

        if (act == CommonFormDialog::TYPE_MODIFY)
        {
            if (m_pDbManager->updateDeviceTable(device) != CS_SUCCEED)
            {
                QMessageBox::warning(this, QString::fromLocal8Bit("修改失败"), QString::fromLocal8Bit("修改失败"));
                return;
            }
            updateDeviceModel(device, index.row());
            m_pDbManager->reloadDevice();
            m_pDbManager->loadRdb("低周减载轮次项视图");
            updateComboBox(ui->m_deviceFilterComboBox, m_pDbManager->m_deviceIdNameMap);
        }
        else
        {
            if (m_pDbManager->insertRoundItemTable(device) != CS_SUCCEED)
            {
                QMessageBox::warning(this, QString::fromLocal8Bit("新增失败"), QString::fromLocal8Bit("新增失败"));
                return;
            }
            updateDeviceModel(device);
            m_pDbManager->reloadDevice();
            m_pDbManager->loadRdb("低周减载轮次项视图");
            updateComboBox(ui->m_deviceFilterComboBox, m_pDbManager->m_deviceIdNameMap);
        }
    }
    ui->m_tableViewDevice->resizeColumnsToContents();
}

DeviceDto MainWindow::extractDeviceData(const QList<QPair<QString, QVariant>> &updatedData)
{
    DeviceDto newDevice;
    memset(&newDevice, 0, sizeof(newDevice));
    for (const auto &pair : updatedData)
    {
        if (pair.first == QString::fromLocal8Bit("编号"))
        {
            newDevice.id = pair.second.toInt();
        }
        else if (pair.first == QString::fromLocal8Bit("名称"))
        {
            strncpy(newDevice.name, pair.second.toString().toLocal8Bit().data(), sizeof(newDevice.name) - 1);
        }
        else if (pair.first == QString::fromLocal8Bit("装置类型"))
        {
            newDevice.type = pair.second.toInt();
        }
        else if (pair.first == QString::fromLocal8Bit("功能类型"))
        {
            newDevice.funcType = pair.second.toInt();
        }
        else if (pair.first == QString::fromLocal8Bit("关联轮次项数"))
        {
            newDevice.itemNums = pair.second.toInt();
        }
        else if (pair.first == QString::fromLocal8Bit("所属厂站"))
        {
            strncpy(newDevice.staId, pair.second.toString().toLocal8Bit().data(), sizeof(newDevice.staId) - 1);
        }
    }
    return newDevice;
}

void MainWindow::updateDeviceModel(const DeviceDto &newDevice, int row)
{
    if (row == -1)
    {
        row = m_deviceModel->rowCount();
    }
    m_deviceModel->setItem(row, 0, new QStandardItem(QString::number(newDevice.id)));
    m_deviceModel->setItem(row, 1, new QStandardItem(QString::fromLocal8Bit(newDevice.name)));
    m_deviceModel->setItem(row, 2, new QStandardItem(QString::number(newDevice.type)));
    m_deviceModel->setItem(row, 3, new QStandardItem(QString::number(newDevice.funcType)));
    m_deviceModel->setItem(row, 4, new QStandardItem(QString::number(newDevice.itemNums)));
    m_deviceModel->setItem(row, 5, new QStandardItem(QString::fromLocal8Bit(newDevice.staId)));
}

void MainWindow::onDeviceTableRowSelected(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return;
    }
    //    DFLOG_DEBUG("row: %d", index.row());
    int deviceId = m_deviceModel->item(index.row(), 0)->text().toInt();
    readDeviceParaTable(deviceId);
}

void MainWindow::readDeviceParaTable(int deviceId)
{
    QVector<DeviceParaDto> list = m_pDbManager->getDeviceParaList(deviceId);
    populateDeviceParaModel(list);

    ui->m_tableViewDevicePara->resizeColumnsToContents();
    ui->m_tableViewDevicePara->resizeRowsToContents();
}

void MainWindow::populateDeviceParaModel(const QVector<DeviceParaDto> &list)
{
    int rowCount = list.size();
    m_deviceParaModel->setRowCount(rowCount);
    for (int i = 0; i < rowCount; i++)
    {
        m_deviceParaModel->setItem(i, 0, new QStandardItem(QString::number(list[i].id)));
        m_deviceParaModel->setItem(i, 1, new QStandardItem(QString::number(list[i].no)));
        m_deviceParaModel->setItem(i, 2, new QStandardItem(QString::fromLocal8Bit(list[i].strapId)));
        m_deviceParaModel->setItem(i, 3, new QStandardItem(QString::fromLocal8Bit(list[i].fixValueId)));
        m_deviceParaModel->setItem(i, 4, new QStandardItem(QString::fromLocal8Bit(list[i].timeValueId)));
        m_deviceParaModel->setItem(i, 5, new QStandardItem(QString::fromLocal8Bit(list[i].alarmId)));
        m_deviceParaModel->setItem(i, 6, new QStandardItem(QString::fromLocal8Bit(list[i].actionId)));
        m_deviceParaModel->setItem(i, 7, new QStandardItem(QString::fromLocal8Bit(list[i].exitId)));
    }
}

void MainWindow::populateDeviceParaData(QList<QPair<QString, QVariant>> &data, const DeviceParaDto &devicePara)
{
    dfJson::Value fixValueJson = m_pDbManager->getFixValueJson(devicePara.id);
    dfJson::Value tsJson = m_pDbManager->getTSJson(devicePara.id);

    JsonDialogData rtuData;
    rtuData.initialText = QString::number(devicePara.id);
    rtuData.isMultiSelect = false;
    rtuData.jsonData = m_pDbManager->m_deviceJson;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("装置编号"), QVariant::fromValue(rtuData)));

    ComboBoxData<int> comboBoxNo;
    comboBoxNo.currentValue = devicePara.no;
    comboBoxNo.options = m_pDbManager->m_roundIdNameMap;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("轮次编号"), QVariant::fromValue(comboBoxNo)));

    JsonDialogData strapData;
    strapData.initialText = QString::fromLocal8Bit(devicePara.strapId);
    strapData.isMultiSelect = true;
    strapData.jsonData = fixValueJson;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("压板ID"), QVariant::fromValue(strapData)));

    JsonDialogData fixValueData;
    fixValueData.initialText = QString::fromLocal8Bit(devicePara.fixValueId);
    fixValueData.isMultiSelect = false;
    fixValueData.jsonData = fixValueJson;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("频率或电压定值ID"), QVariant::fromValue(fixValueData)));

    JsonDialogData timeData;
    timeData.initialText = QString::fromLocal8Bit(devicePara.timeValueId);
    timeData.isMultiSelect = false;
    timeData.jsonData = fixValueJson;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("动作延时定值ID"), QVariant::fromValue(timeData)));

    JsonDialogData alarmData;
    alarmData.initialText = QString::fromLocal8Bit(devicePara.alarmId);
    alarmData.isMultiSelect = true;
    alarmData.jsonData = tsJson;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("告警信号ID"), QVariant::fromValue(alarmData)));

    JsonDialogData actionData;
    actionData.initialText = QString::fromLocal8Bit(devicePara.actionId);
    actionData.isMultiSelect = false;
    actionData.jsonData = tsJson;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("动作信号ID"), QVariant::fromValue(actionData)));

    JsonDialogData exitData;
    exitData.initialText = QString::fromLocal8Bit(devicePara.exitId);
    exitData.isMultiSelect = false;
    exitData.jsonData = fixValueJson;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("出口矩阵ID"), QVariant::fromValue(exitData)));
}

void MainWindow::showDeviceParaDialog(const QList<QPair<QString, QVariant>> &data, int act, const QModelIndex &index)
{
    CommonFormDialog dialog(data, act, this);
    static const QStringList sTitleList = {
        QString::fromLocal8Bit("参数详情"),
        QString::fromLocal8Bit("参数修改"),
        QString::fromLocal8Bit("参数新增")};
    dialog.setWindowTitle(sTitleList[act]);
    dialog.setEnabled(QString::fromLocal8Bit("装置编号"), false);
    if (dialog.exec() == QDialog::Accepted)
    {
        QList<QPair<QString, QVariant>> updatedData = dialog.getData();

        DeviceParaDto item = extractDeviceParaData(updatedData);

        if (act == CommonFormDialog::TYPE_MODIFY)
        {
            if (m_pDbManager->updateDeviceParaTable(item) != CS_SUCCEED)
            {
                QMessageBox::warning(this, QString::fromLocal8Bit("修改失败"), QString::fromLocal8Bit("修改失败"));
                return;
            }
            updateDeviceParaModel(item, index.row());
            m_pDbManager->loadRdb("低周减载轮次项视图");
        }
        else
        {
            if (m_pDbManager->insertDeviceParaTable(item) != CS_SUCCEED)
            {
                QMessageBox::warning(this, QString::fromLocal8Bit("新增失败"), QString::fromLocal8Bit("新增失败"));
                return;
            }
            updateDeviceParaModel(item);
            m_pDbManager->loadRdb("低周减载轮次项视图");
        }
    }

    ui->m_tableViewDevicePara->resizeColumnsToContents();
    ui->m_tableViewDevicePara->resizeRowsToContents();
}

void MainWindow::updateDeviceParaModel(const DeviceParaDto &newDevice, int row)
{
    if (row == -1)
    {
        row = m_deviceParaModel->rowCount();
    }
    m_deviceParaModel->setItem(row, 0, new QStandardItem(QString::number(newDevice.id)));
    m_deviceParaModel->setItem(row, 1, new QStandardItem(QString::number(newDevice.no)));
    m_deviceParaModel->setItem(row, 2, new QStandardItem(newDevice.strapId));
    m_deviceParaModel->setItem(row, 3, new QStandardItem(newDevice.fixValueId));
    m_deviceParaModel->setItem(row, 4, new QStandardItem(newDevice.timeValueId));
    m_deviceParaModel->setItem(row, 5, new QStandardItem(newDevice.alarmId));
    m_deviceParaModel->setItem(row, 6, new QStandardItem(newDevice.actionId));
    m_deviceParaModel->setItem(row, 7, new QStandardItem(newDevice.exitId));
}

DeviceParaDto MainWindow::extractDeviceParaData(const QList<QPair<QString, QVariant>> &updatedData)
{
    DeviceParaDto newDevicePara;
    memset(&newDevicePara, 0, sizeof(newDevicePara));
    for (const auto &pair : updatedData)
    {
        if (pair.first == QString::fromLocal8Bit("装置编号"))
        {
            newDevicePara.id = pair.second.toInt();
        }
        else if (pair.first == QString::fromLocal8Bit("轮次编号"))
        {
            newDevicePara.no = pair.second.toInt();
        }
        else if (pair.first == QString::fromLocal8Bit("压板ID"))
        {
            strncpy(newDevicePara.strapId, pair.second.toString().toLocal8Bit().data(), sizeof(newDevicePara.strapId) - 1);
        }
        else if (pair.first == QString::fromLocal8Bit("频率或电压定值ID"))
        {
            strncpy(newDevicePara.fixValueId, pair.second.toString().toLocal8Bit().data(), sizeof(newDevicePara.fixValueId) - 1);
        }
        else if (pair.first == QString::fromLocal8Bit("动作延时定值ID"))
        {
            strncpy(newDevicePara.timeValueId, pair.second.toString().toLocal8Bit().data(), sizeof(newDevicePara.timeValueId) - 1);
        }
        else if (pair.first == QString::fromLocal8Bit("告警信号ID"))
        {
            strncpy(newDevicePara.alarmId, pair.second.toString().toLocal8Bit().data(), sizeof(newDevicePara.alarmId) - 1);
        }
        else if (pair.first == QString::fromLocal8Bit("动作信号ID"))
        {
            strncpy(newDevicePara.actionId, pair.second.toString().toLocal8Bit().data(), sizeof(newDevicePara.actionId) - 1);
        }
        else if (pair.first == QString::fromLocal8Bit("出口矩阵ID"))
        {
            strncpy(newDevicePara.exitId, pair.second.toString().toLocal8Bit().data(), sizeof(newDevicePara.exitId) - 1);
        }
    }
    return newDevicePara;
}

void MainWindow::onAddDeviceParaButtonClicked()
{
    QModelIndex currentIndex = ui->m_tableViewDevice->currentIndex();

    DeviceParaDto devicePara;
    memset(&devicePara, 0, sizeof(devicePara));
    devicePara.id = m_deviceModel->data(m_deviceModel->index(currentIndex.row(), 0)).toInt();
    devicePara.no = 0;
    strncpy(devicePara.strapId, "", sizeof(devicePara.strapId) - 1);
    strncpy(devicePara.fixValueId, "", sizeof(devicePara.fixValueId) - 1);
    strncpy(devicePara.timeValueId, "", sizeof(devicePara.timeValueId) - 1);
    strncpy(devicePara.alarmId, "", sizeof(devicePara.alarmId) - 1);
    strncpy(devicePara.actionId, "", sizeof(devicePara.actionId) - 1);
    strncpy(devicePara.exitId, "", sizeof(devicePara.exitId) - 1);

    QList<QPair<QString, QVariant>> data;
    populateDeviceParaData(data, devicePara);

    showDeviceParaDialog(data, CommonFormDialog::TYPE_ADD, QModelIndex());
}

void MainWindow::onDetailButtonDeviceParaClicked(QModelIndex index)
{
    DeviceParaDto devicePara;
    memset(&devicePara, 0, sizeof(devicePara));
    devicePara.id = m_deviceParaModel->index(index.row(), 0).data().toInt();
    devicePara.no = m_deviceParaModel->index(index.row(), 1).data().toInt();
    strncpy(devicePara.strapId, m_deviceParaModel->index(index.row(), 2).data().toString().toLocal8Bit().data(), sizeof(devicePara.strapId) - 1);
    strncpy(devicePara.fixValueId, m_deviceParaModel->index(index.row(), 3).data().toString().toLocal8Bit().data(), sizeof(devicePara.fixValueId) - 1);
    strncpy(devicePara.timeValueId, m_deviceParaModel->index(index.row(), 4).data().toString().toLocal8Bit().data(), sizeof(devicePara.timeValueId) - 1);
    strncpy(devicePara.alarmId, m_deviceParaModel->index(index.row(), 5).data().toString().toLocal8Bit().data(), sizeof(devicePara.alarmId) - 1);
    strncpy(devicePara.actionId, m_deviceParaModel->index(index.row(), 6).data().toString().toLocal8Bit().data(), sizeof(devicePara.actionId) - 1);
    strncpy(devicePara.exitId, m_deviceParaModel->index(index.row(), 7).data().toString().toLocal8Bit().data(), sizeof(devicePara.exitId) - 1);

    QList<QPair<QString, QVariant>> data;
    populateDeviceParaData(data, devicePara);

    showDeviceParaDialog(data, CommonFormDialog::TYPE_DETAIL, index);
}

void MainWindow::onModifyButtonDeviceParaClicked(QModelIndex index)
{
    DeviceParaDto devicePara;
    memset(&devicePara, 0, sizeof(devicePara));
    devicePara.id = m_deviceParaModel->index(index.row(), 0).data().toInt();
    devicePara.no = m_deviceParaModel->index(index.row(), 1).data().toInt();
    strncpy(devicePara.strapId, m_deviceParaModel->index(index.row(), 2).data().toString().toLocal8Bit().data(), sizeof(devicePara.strapId) - 1);
    strncpy(devicePara.fixValueId, m_deviceParaModel->index(index.row(), 3).data().toString().toLocal8Bit().data(), sizeof(devicePara.fixValueId) - 1);
    strncpy(devicePara.timeValueId, m_deviceParaModel->index(index.row(), 4).data().toString().toLocal8Bit().data(), sizeof(devicePara.timeValueId) - 1);
    strncpy(devicePara.alarmId, m_deviceParaModel->index(index.row(), 5).data().toString().toLocal8Bit().data(), sizeof(devicePara.alarmId) - 1);
    strncpy(devicePara.actionId, m_deviceParaModel->index(index.row(), 6).data().toString().toLocal8Bit().data(), sizeof(devicePara.actionId) - 1);
    strncpy(devicePara.exitId, m_deviceParaModel->index(index.row(), 7).data().toString().toLocal8Bit().data(), sizeof(devicePara.exitId) - 1);

    QList<QPair<QString, QVariant>> data;
    populateDeviceParaData(data, devicePara);

    showDeviceParaDialog(data, CommonFormDialog::TYPE_MODIFY, index);
}

void MainWindow::onDeleteButtonDeviceParaClicked(QModelIndex index)
{
    int deviceId = m_deviceParaModel->index(index.row(), 0).data().toInt();
    int roundId = m_deviceParaModel->index(index.row(), 1).data().toInt();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  QString::fromLocal8Bit("删除"),
                                  QString::fromLocal8Bit("确定删除序号为%1的参数吗?").arg(roundId),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No)
        return;

    if (m_pDbManager->deleteDeviceParaTable(deviceId, roundId) != CS_SUCCEED)
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("删除失败"), QString::fromLocal8Bit("删除失败"));
        return;
    }

    m_deviceParaModel->removeRow(index.row());
    m_pDbManager->loadRdb("低周减载轮次项视图");
}

void MainWindow::showRoundItemContextMenu(const QPoint &pos)
{
    m_setAreaMenu->clear();
    for (auto iArea : m_pDbManager->m_subAreaIdNameMap.toStdMap())
    {
        addActionToMenu(m_setAreaMenu, iArea.first, iArea.second, SLOT(onSetArea()));
    }

    m_setRoundMenu->clear();
    for (auto iRound : m_pDbManager->m_roundIdNameMap.toStdMap())
    {
        addActionToMenu(m_setRoundMenu, iRound.first, iRound.second, SLOT(onSetRound()));
    }

    m_setStrapMenu->clear();
    for (auto iStrap : m_pDbManager->m_strapMap.toStdMap())
    {
        addActionToMenu(m_setStrapMenu, iStrap.first, iStrap.second, SLOT(onSetStrap()));
    }

    m_setLoadTypeMenu->clear();
    for (auto iLoadType : m_pDbManager->m_loadTypeMap.toStdMap())
    {
        addActionToMenu(m_setLoadTypeMenu, iLoadType.first, iLoadType.second, SLOT(onSetLoadType()));
    }

    m_roundItemContextMenu->exec(ui->m_roundItemTableView->mapToGlobal(pos));
}

void MainWindow::addActionToMenu(QMenu *menu, int id, const QString &name, const char *slot)
{
    auto action = new QAction(name, this);
    action->setData(id);
    connect(action, SIGNAL(triggered()), this, slot);
    menu->addAction(action);
}

void MainWindow::onSetArea()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
        return;

    int newAreaId = action->data().toInt();

    QModelIndexList selectedRows = ui->m_roundItemTableView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty())
        return;

    // Update database and model
    for (const QModelIndex &proxyIndex : selectedRows)
    {
        QModelIndex sourceIndex = m_roundItemProxyModel->mapToSource(proxyIndex);
        int id = m_roundItemModel->index(sourceIndex.row(), 0).data().toInt();
        QString sql = QString::fromLocal8Bit("update xopensdb.低周减载轮次项参数表 set 所属分区=%1 where 编号=%2")
                          .arg(newAreaId)
                          .arg(id);

        if (!m_pDbManager->updateTable(sql))
        {
            QMessageBox::warning(this, QString::fromLocal8Bit("更新失败"), QString::fromLocal8Bit("更新失败"));
            return;
        }
        m_roundItemModel->setItem(sourceIndex.row(), 2, new QStandardItem(QString::number(newAreaId)));
        m_pDbManager->loadRdb("低周减载轮次项视图");
    }
    ui->m_roundItemTableView->resizeColumnsToContents();
}

void MainWindow::onSetDevice()
{
    QModelIndexList selectedRows = ui->m_roundItemTableView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty())
        return;

    QString newDeviceId;
    // 弹出对话框
    dfJson::Value jsonData = m_pDbManager->m_deviceJson;
    JsonTreeDialog dialog(jsonData, false, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        QStringList deviceIds = dialog.getSelectedIds();
        if (!deviceIds.isEmpty())
        {
            newDeviceId = deviceIds.first();
        }
    }

    // 更新数据库 更新模型
    for (const QModelIndex &proxyIndex : selectedRows)
    {
        QModelIndex sourceIndex = m_roundItemProxyModel->mapToSource(proxyIndex);
        int id = m_roundItemModel->index(sourceIndex.row(), 0).data().toInt();
        QString sql = QString::fromLocal8Bit("update xopensdb.低周减载轮次项参数表 set 关联装置=%1 where 编号=%2")
                          .arg(newDeviceId)
                          .arg(id);

        if (!m_pDbManager->updateTable(sql))
        {
            QMessageBox::warning(this, QString::fromLocal8Bit("更新失败"), QString::fromLocal8Bit("更新失败"));
            return;
        }
        m_roundItemModel->setItem(sourceIndex.row(), 9, new QStandardItem(newDeviceId));
    }
    ui->m_roundItemTableView->resizeColumnsToContents();
}

void MainWindow::onSetRound()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
        return;

    int newRoundId = action->data().toInt();

    QModelIndexList selectedRows = ui->m_roundItemTableView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty())
        return;

    // Update database and model
    for (const QModelIndex &proxyIndex : selectedRows)
    {
        QModelIndex sourceIndex = m_roundItemProxyModel->mapToSource(proxyIndex);
        int id = m_roundItemModel->index(sourceIndex.row(), 0).data().toInt();
        QString sql = QString::fromLocal8Bit("update xopensdb.低周减载轮次项参数表 set 所属轮次=%1 where 编号=%2")
                          .arg(newRoundId)
                          .arg(id);

        if (!m_pDbManager->updateTable(sql))
        {
            QMessageBox::warning(this, QString::fromLocal8Bit("更新失败"), QString::fromLocal8Bit("更新失败"));
            return;
        }
        m_roundItemModel->setItem(sourceIndex.row(), 3, new QStandardItem(QString::number(newRoundId)));
        m_pDbManager->loadRdb("低周减载轮次项视图");
    }
    ui->m_roundItemTableView->resizeColumnsToContents();
}

void MainWindow::onSetStrap()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
        return;

    int strapId = action->data().toInt();

    QModelIndexList selectedRows = ui->m_roundItemTableView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty())
        return;

    // Update database and model
    for (const QModelIndex &proxyIndex : selectedRows)
    {
        QModelIndex sourceIndex = m_roundItemProxyModel->mapToSource(proxyIndex);
        int id = m_roundItemModel->index(sourceIndex.row(), 0).data().toInt();
        QString sql = QString::fromLocal8Bit("update xopensdb.低周减载轮次项参数表 set 投退计划=%1 where 编号=%2")
                          .arg(strapId)
                          .arg(id);

        if (!m_pDbManager->updateTable(sql))
        {
            QMessageBox::warning(this, QString::fromLocal8Bit("更新失败"), QString::fromLocal8Bit("更新失败"));
            return;
        }
        m_roundItemModel->setItem(sourceIndex.row(), 6, new QStandardItem(QString::number(strapId)));
        m_pDbManager->loadRdb("低周减载轮次项视图");
    }
    ui->m_roundItemTableView->resizeColumnsToContents();
}

void MainWindow::onSetLoadType()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
        return;

    int loadType = action->data().toInt();

    QModelIndexList selectedRows = ui->m_roundItemTableView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty())
        return;

    // Update database and model
    for (const QModelIndex &proxyIndex : selectedRows)
    {
        QModelIndex sourceIndex = m_roundItemProxyModel->mapToSource(proxyIndex);
        int id = m_roundItemModel->index(sourceIndex.row(), 0).data().toInt();
        QString sql = QString::fromLocal8Bit("update xopensdb.低周减载轮次项参数表 set 负荷类型=%1 where 编号=%2")
                          .arg(loadType)
                          .arg(id);

        if (!m_pDbManager->updateTable(sql))
        {
            QMessageBox::warning(this, QString::fromLocal8Bit("更新失败"), QString::fromLocal8Bit("更新失败"));
            return;
        }
        m_roundItemModel->setItem(sourceIndex.row(), 5, new QStandardItem(QString::number(loadType)));
        m_pDbManager->loadRdb("低周减载轮次项视图");
    }
    ui->m_roundItemTableView->resizeColumnsToContents();
}

void MainWindow::updateComboBox(QComboBox *comboBox, const QMap<int, QString> &map)
{
    comboBox->clear(); // 清除旧的内容

    comboBox->addItem(QString::fromLocal8Bit("全部"), -1);

    QMapIterator<int, QString> i(map);
    while (i.hasNext())
    {
        i.next();
        comboBox->addItem(i.value(), i.key());
    }
    comboBox->addItem(QString::fromLocal8Bit("未定义"), 0);
}

void MainWindow::updateRoundItemFilter(int index)
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(sender());
    if (!comboBox)
        return;

    int currentData = comboBox->itemData(index).toInt();
    QString filterType = comboBox->property("filterType").toString();

    if (filterType == "area")
        m_roundItemProxyModel->setAreaFilter(currentData);
    else if (filterType == "round")
        m_roundItemProxyModel->setRoundFilter(currentData);
    else if (filterType == "loadType")
        m_roundItemProxyModel->setLoadTypeFilter(currentData);
    else if (filterType == "strap")
        m_roundItemProxyModel->setStrapFilter(currentData);
    else if (filterType == "device")
        m_roundItemProxyModel->setDeviceFilter(currentData);
}

void MainWindow::updateDeviceOptions(int index)
{
    int currentArea = ui->m_areaFilterComboBox->itemData(index).toInt();
    QMap<int, QString> newDeviceOptions = m_pDbManager->getDeviceOptionsForArea(currentArea);
    updateComboBox(ui->m_deviceFilterComboBox, newDeviceOptions);
}

void MainWindow::setupTaskTable()
{
    m_taskModel = new QStandardItemModel(this);
    m_taskModel->setHorizontalHeaderLabels(QStringList() << QString::fromLocal8Bit("编号")
                                                         << QString::fromLocal8Bit("名称")
                                                         << QString::fromLocal8Bit("周期开始时间")
                                                         << QString::fromLocal8Bit("周期结束时间")
                                                         << QString::fromLocal8Bit("周期(分钟)")
                                                         << QString::fromLocal8Bit("分区条件")
                                                         << QString::fromLocal8Bit("厂站条件")
                                                         << QString::fromLocal8Bit("轮次条件")
                                                         << QString::fromLocal8Bit("操作"));

    ui->m_taskTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->m_taskTableView->setModel(m_taskModel);
    ui->m_taskTableView->setWordWrap(true);
    ui->m_taskTableView->setItemDelegateForColumn(2, new TimestampDelegate(ui->m_taskTableView));
    ui->m_taskTableView->setItemDelegateForColumn(3, new TimestampDelegate(ui->m_taskTableView));

    ui->m_taskTableView->setItemDelegateForColumn(5, new DMDelegateMulti<int>(m_pDbManager->m_subAreaIdNameMap, ui->m_taskTableView));
    ui->m_taskTableView->setItemDelegateForColumn(6, new DMDelegateMulti<QString>(m_pDbManager->m_staIdNameMap, ui->m_taskTableView));
    ui->m_taskTableView->setItemDelegateForColumn(7, new DMDelegateMulti<int>(m_pDbManager->m_roundIdNameMap, ui->m_taskTableView));

    m_btnDelegateTask = new OperationDelegate(ui->m_taskTableView);
    connect(ui->m_addTaskPushButton, SIGNAL(clicked()), this, SLOT(onAddButtonTaskClicked()));
    connect(m_btnDelegateTask, SIGNAL(detailButtonClicked(QModelIndex)), this, SLOT(onDetailButtonTaskClicked(QModelIndex)));
    connect(m_btnDelegateTask, SIGNAL(deleteButtonClicked(QModelIndex)), this, SLOT(onDeleteButtonTaskClicked(QModelIndex)));
    connect(m_btnDelegateTask, SIGNAL(modifyButtonClicked(QModelIndex)), this, SLOT(onModifyButtonTaskClicked(QModelIndex)));
    ui->m_taskTableView->setItemDelegateForColumn(8, m_btnDelegateTask);

    ui->m_taskTableView->resizeColumnsToContents();
    ui->m_taskTableView->resizeRowsToContents();
}

void MainWindow::readTaskTable()
{
    QVector<TaskDto> list = m_pDbManager->getTaskList();
    populateTaskModel(list);

    ui->m_taskTableView->resizeColumnsToContents();
    ui->m_taskTableView->resizeRowsToContents();
}

void MainWindow::populateTaskModel(const QVector<TaskDto> &taskList)
{
    int rowCount = taskList.size();
    m_taskModel->setRowCount(rowCount);
    for (int i = 0; i < rowCount; i++)
    {
        const TaskDto &task = taskList[i];
        m_taskModel->setItem(i, 0, new QStandardItem(QString::number(task.id)));
        m_taskModel->setItem(i, 1, new QStandardItem(QString::fromLocal8Bit(task.name)));
        m_taskModel->setItem(i, 2, new QStandardItem(QString::number(task.startTime)));
        m_taskModel->setItem(i, 3, new QStandardItem(QString::number(task.endTime)));
        m_taskModel->setItem(i, 4, new QStandardItem(QString::number(task.period)));
        m_taskModel->setItem(i, 5, new QStandardItem(QString::fromLocal8Bit(task.areaCond)));
        m_taskModel->setItem(i, 6, new QStandardItem(QString::fromLocal8Bit(task.subCond)));
        m_taskModel->setItem(i, 7, new QStandardItem(QString::fromLocal8Bit(task.roundCond)));
        m_taskModel->setItem(i, 8, new QStandardItem());
    }
}

void MainWindow::onAddButtonTaskClicked()
{
    TaskDto task;
    memset(&task, 0, sizeof(task));
    task.id = m_pDbManager->getMaxIDFromDataBase("xopensdb.dbo.低周减载周期巡检任务表");
    strncpy(task.name, "", sizeof(task.name) - 1);
    task.startTime = QDateTime::currentDateTime().toTime_t();
    // DFLOG_DEBUG("task.startTime = %d", task.startTime);
    task.endTime = 0;
    task.period = 0;

    QList<QPair<QString, QVariant>> data;
    populateTaskData(data, task);

    showTaskDialog(data, CommonFormDialog::TYPE_ADD, QModelIndex());
}

void MainWindow::populateTaskData(QList<QPair<QString, QVariant>> &data, const TaskDto &task)
{
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("编号"), QString::number(task.id)));
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("名称"), QString::fromLocal8Bit(task.name)));
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("周期开始时间"), QDateTime::fromTime_t(task.startTime)));
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("周期结束时间"), QDateTime::fromTime_t(task.endTime)));
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("周期"), task.period));

    JsonDialogData areaCondData;
    areaCondData.initialText = QString::fromLocal8Bit(task.areaCond);
    areaCondData.isMultiSelect = true;
    areaCondData.jsonData = m_pDbManager->getAreaJson();
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("分区条件"), QVariant::fromValue(areaCondData)));

    JsonDialogData stationCondData;
    stationCondData.initialText = QString::fromLocal8Bit(task.subCond);
    stationCondData.isMultiSelect = true;
    stationCondData.jsonData = m_pDbManager->m_staJson;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("厂站条件"), QVariant::fromValue(stationCondData)));

    JsonDialogData roundCondData;
    roundCondData.initialText = QString::fromLocal8Bit(task.roundCond);
    roundCondData.isMultiSelect = true;
    roundCondData.jsonData = m_pDbManager->m_roundJson;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("轮次条件"), QVariant::fromValue(roundCondData)));
}

void MainWindow::showTaskDialog(const QList<QPair<QString, QVariant>> &data, int act, const QModelIndex &index)
{
    CommonFormDialog dialog(data, act, this);
    static const QStringList sTitleList = {QString::fromLocal8Bit("任务详情"), QString::fromLocal8Bit("任务修改"), QString::fromLocal8Bit("任务新增")};
    dialog.setWindowTitle(sTitleList[act]);
    if (act == CommonFormDialog::TYPE_MODIFY)
    {
        dialog.setEnabled(QString::fromLocal8Bit("编号"), false);
    }
    if (dialog.exec() == QDialog::Accepted)
    {
        QList<QPair<QString, QVariant>> updatedData = dialog.getData();

        TaskDto task = extractTaskData(updatedData);

        if (act == CommonFormDialog::TYPE_MODIFY)
        {
            if (m_pDbManager->updateTaskTable(task) != CS_SUCCEED)
            {
                QMessageBox::warning(this, QString::fromLocal8Bit("修改失败"), QString::fromLocal8Bit("修改失败"));
                return;
            }
            updateTaskModel(task, index.row());
            m_pDbManager->loadRdb("低周减载周期巡检任务表");
        }
        else
        {
            if (m_pDbManager->insertTaskTable(task) != CS_SUCCEED)
            {
                QMessageBox::warning(this, QString::fromLocal8Bit("新增失败"), QString::fromLocal8Bit("新增失败"));
                return;
            }
            updateTaskModel(task);
            m_pDbManager->loadRdb("低周减载周期巡检任务表");
        }
    }
    ui->m_taskTableView->resizeColumnsToContents();
    ui->m_taskTableView->resizeRowsToContents();
}

TaskDto MainWindow::extractTaskData(const QList<QPair<QString, QVariant>> &updatedData)
{
    TaskDto task;
    memset(&task, 0, sizeof(task));
    for (const auto &pair : updatedData)
    {
        if (pair.first == QString::fromLocal8Bit("编号"))
        {
            task.id = pair.second.toInt();
        }
        else if (pair.first == QString::fromLocal8Bit("名称"))
        {
            strncpy(task.name, pair.second.toString().toLocal8Bit().data(), sizeof(task.name) - 1);
        }
        else if (pair.first == QString::fromLocal8Bit("周期开始时间"))
        {
            task.startTime = pair.second.toDateTime().toTime_t();
        }
        else if (pair.first == QString::fromLocal8Bit("周期结束时间"))
        {
            task.endTime = pair.second.toDateTime().toTime_t();
        }
        else if (pair.first == QString::fromLocal8Bit("周期"))
        {
            task.period = pair.second.toInt();
        }
        else if (pair.first == QString::fromLocal8Bit("分区条件"))
        {
            strncpy(task.areaCond, pair.second.toString().toLocal8Bit().data(), sizeof(task.areaCond) - 1);
        }
        else if (pair.first == QString::fromLocal8Bit("厂站条件"))
        {
            strncpy(task.subCond, pair.second.toString().toLocal8Bit().data(), sizeof(task.subCond) - 1);
        }
        else if (pair.first == QString::fromLocal8Bit("轮次条件"))
        {
            strncpy(task.roundCond, pair.second.toString().toLocal8Bit().data(), sizeof(task.roundCond) - 1);
        }
    }
    return task;
}

void MainWindow::updateTaskModel(const TaskDto &newTask, int row)
{
    if (row == -1)
    {
        row = m_taskModel->rowCount();
    }
    m_taskModel->setItem(row, 0, new QStandardItem(QString::number(newTask.id)));
    m_taskModel->setItem(row, 1, new QStandardItem(QString::fromLocal8Bit(newTask.name)));
    m_taskModel->setItem(row, 2, new QStandardItem(QString::number(newTask.startTime)));
    m_taskModel->setItem(row, 3, new QStandardItem(QString::number(newTask.endTime)));
    m_taskModel->setItem(row, 4, new QStandardItem(QString::number(newTask.period)));
    m_taskModel->setItem(row, 5, new QStandardItem(QString::fromLocal8Bit(newTask.areaCond)));
    m_taskModel->setItem(row, 6, new QStandardItem(QString::fromLocal8Bit(newTask.subCond)));
    m_taskModel->setItem(row, 7, new QStandardItem(QString::fromLocal8Bit(newTask.roundCond)));
    m_taskModel->setItem(row, 8, new QStandardItem(""));
}

void MainWindow::onDetailButtonTaskClicked(QModelIndex index)
{
    TaskDto task;
    memset(&task, 0, sizeof(task));
    task.id = m_taskModel->index(index.row(), 0).data().toInt();
    strncpy(task.name, m_taskModel->index(index.row(), 1).data().toString().toLocal8Bit().data(), sizeof(task.name) - 1);
    task.startTime = m_taskModel->index(index.row(), 2).data().toInt();
    task.endTime = m_taskModel->index(index.row(), 3).data().toInt();
    task.period = m_taskModel->index(index.row(), 4).data().toInt();
    strncpy(task.areaCond, m_taskModel->index(index.row(), 5).data().toString().toLocal8Bit().data(), sizeof(task.areaCond) - 1);
    strncpy(task.subCond, m_taskModel->index(index.row(), 6).data().toString().toLocal8Bit().data(), sizeof(task.subCond) - 1);
    strncpy(task.roundCond, m_taskModel->index(index.row(), 7).data().toString().toLocal8Bit().data(), sizeof(task.roundCond) - 1);

    QList<QPair<QString, QVariant>> data;
    populateTaskData(data, task);

    showTaskDialog(data, CommonFormDialog::TYPE_DETAIL, index);
}

void MainWindow::onDeleteButtonTaskClicked(QModelIndex index)
{
    int id = m_taskModel->index(index.row(), 0).data().toInt();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  QString::fromLocal8Bit("删除"),
                                  QString::fromLocal8Bit("确定删除编号为%1的任务吗?").arg(id),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No)
        return;

    if (m_pDbManager->deleteTable(id, "xopensdb.dbo.低周减载周期巡检任务表") != CS_SUCCEED)
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("删除失败"), QString::fromLocal8Bit("删除失败"));
        return;
    }

    m_taskModel->removeRow(index.row());
    ui->m_taskTableView->resizeColumnsToContents();
    ui->m_taskTableView->resizeRowsToContents();

    m_pDbManager->loadRdb("低周减载周期巡检任务表");
}

void MainWindow::onModifyButtonTaskClicked(QModelIndex index)
{
    TaskDto task;
    memset(&task, 0, sizeof(task));
    task.id = m_taskModel->index(index.row(), 0).data().toInt();
    strncpy(task.name, m_taskModel->index(index.row(), 1).data().toString().toLocal8Bit().data(), sizeof(task.name) - 1);
    task.startTime = m_taskModel->index(index.row(), 2).data().toInt();
    task.endTime = m_taskModel->index(index.row(), 3).data().toInt();
    task.period = m_taskModel->index(index.row(), 4).data().toInt();
    strncpy(task.areaCond, m_taskModel->index(index.row(), 5).data().toString().toLocal8Bit().data(), sizeof(task.areaCond) - 1);
    strncpy(task.subCond, m_taskModel->index(index.row(), 6).data().toString().toLocal8Bit().data(), sizeof(task.subCond) - 1);
    strncpy(task.roundCond, m_taskModel->index(index.row(), 7).data().toString().toLocal8Bit().data(), sizeof(task.roundCond) - 1);

    QList<QPair<QString, QVariant>> data;
    populateTaskData(data, task);

    showTaskDialog(data, CommonFormDialog::TYPE_MODIFY, index);
}

void MainWindow::onCallButtonClicked()
{
    QModelIndex currentIndex = ui->m_tableViewDevice->currentIndex();
    int id = m_deviceModel->data(m_deviceModel->index(currentIndex.row(), 0)).toInt();

    QProgressDialog dialog(QString::fromLocal8Bit("倒计时30秒"), QString::fromLocal8Bit("取消"), 0, 30, this);
    dialog.setWindowModality(Qt::WindowModal);
    // dialog.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    // dialog.setCancelButton(nullptr);

    CallWorkder *pWorker = new CallWorkder;
    pWorker->setId(id);

    QThread *thread = new QThread;
    pWorker->moveToThread(thread);

    QTimer *progressTimer = new QTimer(&dialog);

    connect(thread, SIGNAL(started()), pWorker, SLOT(call()));
    connect(pWorker, SIGNAL(resultReady(int)), this, SLOT(handleResult(int)));
    connect(pWorker, SIGNAL(resultReady(int)), &dialog, SLOT(cancel()));
    connect(pWorker, SIGNAL(resultReady(int)), progressTimer, SLOT(stop()));
    connect(pWorker, SIGNAL(resultReady(int)), thread, SLOT(quit()));

    connect(thread, SIGNAL(finished()), pWorker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), progressTimer, SLOT(deleteLater()));

    connect(progressTimer, SIGNAL(timeout()), this, SLOT(updateProgressDialog()));

    connect(&dialog, SIGNAL(canceled()), pWorker, SLOT(cancel()));
    connect(&dialog, SIGNAL(canceled()), progressTimer, SLOT(stop()));
    connect(&dialog, SIGNAL(canceled()), thread, SLOT(quit()));

    thread->start();

    progressTimer->start(1000);

    dialog.exec();
}

void MainWindow::onCallCancel()
{
    qDebug() << "canceled signal emitted";
}

void MainWindow::handleResult(int result)
{
    if (result != FIX_CALL_SUCCESS)
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("召唤失败"));
        return;
    }
    QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("召唤成功"));
    m_pDbManager->reloadFixValue();
    m_pDbManager->loadRdb("低周减载装置定值信息表");
}

void MainWindow::updateProgressDialog()
{
    QProgressDialog *dialog = qobject_cast<QProgressDialog *>(sender()->parent());
    if (dialog)
    {
        int value = dialog->value();
        if (value == dialog->maximum())
        {
            dialog->cancel();
        }
        else
        {
            dialog->setLabelText(QString::fromLocal8Bit("倒计时%1秒").arg(dialog->maximum() - value - 1));
            dialog->setValue(value + 1);
        }
    }
}

void MainWindow::setupRoundFileModel()
{
    // 区域模型
    m_roundFileModel = new QStandardItemModel(this);
    m_roundFileModel->setHorizontalHeaderLabels(QStringList() << QString::fromLocal8Bit("编号")
                                                              << QString::fromLocal8Bit("名称")
                                                              << QString::fromLocal8Bit("功能类型")
                                                              << QString::fromLocal8Bit("轮次类型")
                                                              << QString::fromLocal8Bit("原频率/电压整定值")
                                                              << QString::fromLocal8Bit("现频率/电压整定值")
                                                              << QString::fromLocal8Bit("原动作延时整定值")
                                                              << QString::fromLocal8Bit("现动作延时整定值")
                                                              << QString::fromLocal8Bit("原应切荷量")
                                                              << QString::fromLocal8Bit("现应切荷量")
                                                              << QString::fromLocal8Bit("原应切荷比例")
                                                              << QString::fromLocal8Bit("现应切荷比例"));

    ui->m_tableViewFileRound->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->m_tableViewFileRound->setModel(m_roundFileModel);
    ui->m_tableViewFileRound->resizeColumnsToContents();

    connect(ui->m_pushButtonRoundSelect, SIGNAL(clicked()), this, SLOT(onSelectRoundFile()));
    connect(ui->m_pushButtonRoundImport, SIGNAL(clicked()), this, SLOT(onImportRoundFileToDB()));
}

void MainWindow::onSelectRoundFile()
{
    BaseFileConfig ufConfig = {
        arrUfBaseFreq, arrUfBaseTime, arrUfBaseRequire, arrUfBaseRequireRate, 8};

    BaseFileConfig uvConfig = {
        arrUVBaseVolt, arrUvBaseTime, arrUvBaseRequire, arrUvBaseRequireRate, 4};

    m_pDbManager->reloadRound();

    QString filePath = QFileDialog::getOpenFileName(this,
                                                    QString::fromLocal8Bit("请选择文件..."),
                                                    QString("%1/%2").arg(qgetenv("RUNHOME_INC").data()).arg("dzjzsrv/docs"),
                                                    "CIME Files (*.CIME);;All Files (*)");

    if (!filePath.isEmpty())
    {
        DZJZ_FileParser parser(filePath);
        const dfJson::Value &jFile = parser.parse();
        dfJson::Value jFreqRow = getJsonConfig(jFile, "低频配置方案");
        dfJson::Value jVoltRow = getJsonConfig(jFile, "低压配置方案");

        m_roundFileModel->removeRows(0, m_roundFileModel->rowCount());

        for (int i = 0; i < m_pDbManager->m_roundList.count(); ++i)
        {
            const RoundDto &roundDB = m_pDbManager->m_roundList[i];
            BaseFileConfig *pConfig = nullptr;
            dfJson::Value *pJRow = nullptr;
            int pos = 0;
            if (i < 8)
            {
                pConfig = &ufConfig;
                pJRow = &jFreqRow;
                pos = i;
            }
            else
            {
                pConfig = &uvConfig;
                pJRow = &jVoltRow;
                pos = i - 8;
            }

            QList<QStandardItem *> items = generateRowItems(roundDB, *pJRow, *pConfig, pos);
            m_roundFileModel->appendRow(items);
        }

        ui->m_tableViewFileRound->resizeColumnsToContents();
    }
}

dfJson::Value MainWindow::getJsonConfig(const dfJson::Value &jFile, const char *key)
{
    if (jFile.isMember(key))
    {
        dfJson::Value config = jFile[key];
        if (config.size() > 0)
        {
            return config[0];
        }
    }
    return dfJson::Value();
}

// QList<QStandardItem *> MainWindow::generateRowItems(const RoundDto &roundDB, const dfJson::Value &jRow, const BaseFileConfig &config, int index)
// {
//     QList<QStandardItem *> items;

//     // Add common items
//     items.append(new QStandardItem(QString::number(roundDB.id)));
//     items.append(new QStandardItem(roundDB.name));
//     items.append(new QStandardItem(m_pDbManager->m_roundFuncTypeMap[roundDB.funcType]));
//     items.append(new QStandardItem(m_pDbManager->m_roundTypeMap[roundDB.roundType]));

//     generateStandardItem(roundDB.fixValue, jRow[config.value[index]].asCString(), items);
//     generateStandardItem(roundDB.timeValue / 1000, jRow[config.time[index]].asCString(), items);
//     generateStandardItem(roundDB.requirePower, jRow[config.require[index]].asCString(), items);
//     generateStandardItem(roundDB.requirePowerRate, jRow[config.requireRate[index]].asCString(), items);

//     return items;
// }

QList<QStandardItem *> MainWindow::generateRowItems(const RoundDto &roundDB, const dfJson::Value &jRow, const BaseFileConfig &config, int index)
{
    QList<QStandardItem *> items;

    // Add common items
    items.append(new QStandardItem(QString::number(roundDB.id)));
    items.append(new QStandardItem(roundDB.name));
    items.append(new QStandardItem(m_pDbManager->m_roundFuncTypeMap[roundDB.funcType]));
    items.append(new QStandardItem(m_pDbManager->m_roundTypeMap[roundDB.roundType]));

    if (jRow.isMember(config.value[index]) && jRow[config.value[index]].isString())
    {
        generateStandardItem(roundDB.fixValue, jRow[config.value[index]].asCString(), items);
    }
    else
    {
        items.append(new QStandardItem("0"));
        items.append(new QStandardItem("0"));
    }
    if (jRow.isMember(config.time[index]) && jRow[config.time[index]].isString())
    {
        generateStandardItem(roundDB.timeValue / 1000, jRow[config.time[index]].asCString(), items);
    }
    else
    {
        items.append(new QStandardItem("0"));
        items.append(new QStandardItem("0"));
    }

    if (jRow.isMember(config.require[index]) && jRow[config.require[index]].isString())
    {
        generateStandardItem(roundDB.requirePower, jRow[config.require[index]].asCString(), items);
    }
    else
    {
        items.append(new QStandardItem("0"));
        items.append(new QStandardItem("0"));
    }

    if (jRow.isMember(config.requireRate[index]) && jRow[config.requireRate[index]].isString())
    {
        generateStandardItem(roundDB.requirePowerRate, jRow[config.requireRate[index]].asCString(), items);
    }
    else
    {
        items.append(new QStandardItem("0"));
        items.append(new QStandardItem("0"));
    }

    return items;
}

void MainWindow::generateStandardItem(float dbValue, const QString &fileValue, QList<QStandardItem *> &items)
{
    QStandardItem *item1 = new QStandardItem(QString::number(dbValue, 'f', 2));
    QStandardItem *item2 = new QStandardItem(QString::number(fileValue.toFloat(), 'f', 2));

    if (qAbs(dbValue - fileValue.toFloat()) > 1e-6)
    {
        QBrush redBrush(Qt::red);
        item1->setBackground(redBrush);
        item2->setBackground(redBrush);
    }

    items.append(item1);
    items.append(item2);
}

/**
 * @brief 导入省调文件到数据库
 *
 */
void MainWindow::onImportRoundFileToDB()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "导入确认", "您确实要导入数据到数据库吗？",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No)
    {
        return;
    }

    m_pDbManager->m_roundList;

    for (int row = 0; row < m_roundFileModel->rowCount(); ++row)
    {
        m_pDbManager->m_roundList[row].fixValue = m_roundFileModel->data(m_roundFileModel->index(row, 5)).toFloat();
        m_pDbManager->m_roundList[row].timeValue = m_roundFileModel->data(m_roundFileModel->index(row, 7)).toFloat() * 1000;
        m_pDbManager->m_roundList[row].requirePower = m_roundFileModel->data(m_roundFileModel->index(row, 9)).toFloat();
        m_pDbManager->m_roundList[row].requirePowerRate = m_roundFileModel->data(m_roundFileModel->index(row, 11)).toFloat();
        m_pDbManager->updateRoundTable(m_pDbManager->m_roundList[row]);

        QModelIndex index;
        index = m_roundFileModel->index(row, 4);
        m_roundFileModel->setData(index, QString::number(m_pDbManager->m_roundList[row].fixValue, 'f', 2));

        index = m_roundFileModel->index(row, 6);
        m_roundFileModel->setData(index, QString::number(m_pDbManager->m_roundList[row].timeValue / 1000, 'f', 2));

        index = m_roundFileModel->index(row, 8);
        m_roundFileModel->setData(index, QString::number(m_pDbManager->m_roundList[row].requirePower, 'f', 2));

        index = m_roundFileModel->index(row, 10);
        m_roundFileModel->setData(index, QString::number(m_pDbManager->m_roundList[row].requirePowerRate, 'f', 2));
    }

    m_pDbManager->reloadRound();

    m_pDbManager->loadRdb("低周减载轮次参数表");
    m_pDbManager->loadRdb("低周减载轮次项视图");

    QMessageBox::information(this, "导入成功", "文件已成功导入到数据库。");
}
