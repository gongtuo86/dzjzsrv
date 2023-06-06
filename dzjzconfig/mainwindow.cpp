#include "mainwindow.h"

#include <QDebug>
#include <QMessageBox>

#include "commonformdialog.h"
#include "dbmanager.h"
#include "deviceDelegate.h"
#include "dflogger.h"
#include "dmDelegate.h"
#include "dzjzconfigutil.h"
#include "operationdelegate.h"
#include "sybase.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    startdb(1, 1);

    m_pDbManager = DBManager::getInstance();

    connect(ui->m_listWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(onModuleItemClicked(QListWidgetItem *)));

    setupModels();

    // 设置左侧列表宽度
    int width = ui->m_listWidget->sizeHintForColumn(0) + 2 * ui->m_listWidget->frameWidth();
    ui->m_listWidget->setFixedWidth(width);

    showMaximized();
}

MainWindow::~MainWindow()
{
    closedb();
    delete ui;
}

void MainWindow::setupModels()
{
    setupAreaTable();
    setupRoundTree();
    setupRoundItemTable();
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

    ui->m_tableViewArea->setModel(m_areaModel);
    ui->m_tableViewArea->setColumnWidth(0, qMax(ui->m_tableViewArea->columnWidth(0), 10));
    ui->m_tableViewArea->setColumnWidth(1, qMax(ui->m_tableViewArea->columnWidth(1), 100));
    ui->m_tableViewArea->setColumnWidth(2, qMax(ui->m_tableViewArea->columnWidth(2), 100));
    ui->m_tableViewArea->setItemDelegateForColumn(2, new DMDelegateInt(m_pDbManager->m_areaTypeMap, ui->m_tableViewArea));
    ui->m_tableViewArea->setColumnWidth(3, qMax(ui->m_tableViewArea->columnWidth(3), 100));
    ui->m_tableViewArea->setItemDelegateForColumn(3, new DMDelegateString(m_pDbManager->m_staIdNameMap, ui->m_tableViewArea));
    ui->m_tableViewArea->setColumnWidth(4, qMax(ui->m_tableViewArea->columnWidth(4), 150));
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
    m_roundModel->setHorizontalHeaderLabels(QStringList() << QString::fromLocal8Bit("编号")
                                                          << QString::fromLocal8Bit("名称"));
    ui->m_roundTreeView->setModel(m_roundModel);
    QItemSelectionModel *roundSelectionModel = new QItemSelectionModel(m_roundModel, this);
    ui->m_roundTreeView->setSelectionModel(roundSelectionModel);
    // 当选中项发生变化时，更新标签的内容
    connect(roundSelectionModel, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(onRoundSelectionModelChanged(const QModelIndex &)));

    m_roundContextMenu = new QMenu(this);
    m_addRoundAction = new QAction(QString::fromLocal8Bit("添加"), this);
    m_modifyRoundAction = new QAction(QString::fromLocal8Bit("修改"), this);
    m_deleteRoundAction = new QAction(QString::fromLocal8Bit("删除"), this);
    m_roundContextMenu->addAction(m_addRoundAction);
    m_roundContextMenu->addAction(m_modifyRoundAction);
    m_roundContextMenu->addAction(m_deleteRoundAction);
    ui->m_roundTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->m_roundTreeView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showRoundContextMenu(const QPoint &)));
    connect(m_addRoundAction, SIGNAL(triggered()), this, SLOT(onAddRoundActionTriggered()));
    connect(m_modifyRoundAction, SIGNAL(triggered()), this, SLOT(onModifyRoundActionTriggered()));
    connect(m_deleteRoundAction, SIGNAL(triggered()), this, SLOT(onDeleteRoundActionTriggered()));
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
                                                              << QString::fromLocal8Bit("操作"));

    ui->m_roundItemTableView->setModel(m_roundItemModel);
    ui->m_roundItemTableView->setItemDelegateForColumn(2, new DMDelegateInt(m_pDbManager->m_areaIdNameMap, ui->m_roundItemTableView));
    ui->m_roundItemTableView->setItemDelegateForColumn(3, new DMDelegateInt(m_pDbManager->m_roundIdNameMap, ui->m_roundItemTableView));
    ui->m_roundItemTableView->setItemDelegateForColumn(4, new DMDelegateString(m_pDbManager->m_lineIdNameMap, ui->m_roundItemTableView));
    ui->m_roundItemTableView->setItemDelegateForColumn(5, new DMDelegateInt(m_pDbManager->m_loadTypeMap, ui->m_roundItemTableView));
    ui->m_roundItemTableView->setItemDelegateForColumn(6, new DMDelegateInt(m_pDbManager->m_strapMap, ui->m_roundItemTableView));
    ui->m_roundItemTableView->setItemDelegateForColumn(7, new DMDelegateString(m_pDbManager->m_breakIdNameMap, ui->m_roundItemTableView));
    m_BtnDelegateRoundItem = new OperationDelegate(ui->m_roundItemTableView);
    // connect(ui->m_pushButtonAddArea, SIGNAL(clicked()), this, SLOT(onAddButtonAreaClicked()));
    // connect(m_BtnDelegateArea, SIGNAL(detailButtonClicked(QModelIndex)), this, SLOT(onDetailButtonAreaClicked(QModelIndex)));
    // connect(m_BtnDelegateArea, SIGNAL(deleteButtonClicked(QModelIndex)), this, SLOT(onDeleteButtonAreaClicked(QModelIndex)));
    // connect(m_BtnDelegateArea, SIGNAL(modifyButtonClicked(QModelIndex)), this, SLOT(onModifyButtonAreaClicked(QModelIndex)));
    ui->m_roundItemTableView->setItemDelegateForColumn(8, m_BtnDelegateRoundItem);

    connect(ui->m_addRoundItemPushButton, SIGNAL(clicked()), this, SLOT(onAddRoundItemButtonClicked()));
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
    if (index == 2)
    {
        readRoundTable();

        for (int i = 0; i < 2; ++i)
        {
            ui->m_roundTreeView->resizeColumnToContents(i);
        }
    }
}

void MainWindow::readAreaTable()
{
    QVector<AreaVo> areaList = m_pDbManager->getAreaVoList();
    populateAreaModel(areaList);
}

void MainWindow::populateRoundModel(const QVector<RoundDto> &roundList)
{
    int rowCount = roundList.size();
    m_roundModel->setRowCount(rowCount);
    for (int i = 0; i < rowCount; i++)
    {
        m_roundModel->setItem(i, 0, new QStandardItem(QString::number(roundList[i].id)));
        m_roundModel->setItem(i, 1, new QStandardItem(QString::fromLocal8Bit(roundList[i].name)));
        m_roundModel->setItem(i, 2, new QStandardItem(QString::number(roundList[i].areaId)));
        m_roundModel->setItem(i, 3, new QStandardItem(QString::number(roundList[i].funcType)));
        m_roundModel->setItem(i, 4, new QStandardItem(QString::number(roundList[i].roundType)));
        m_roundModel->setItem(i, 5, new QStandardItem(QString::number(roundList[i].fixValue)));
        m_roundModel->setItem(i, 6, new QStandardItem(QString::number(roundList[i].timeValue)));
        m_roundModel->setItem(i, 7, new QStandardItem(QString::number(roundList[i].requirePower)));
    }

    // 隐藏不需要的列
    ui->m_roundTreeView->setColumnHidden(2, true);
    ui->m_roundTreeView->setColumnHidden(3, true);
    ui->m_roundTreeView->setColumnHidden(4, true);
    ui->m_roundTreeView->setColumnHidden(5, true);
    ui->m_roundTreeView->setColumnHidden(6, true);
    ui->m_roundTreeView->setColumnHidden(7, true);
}

void MainWindow::readRoundTable()
{
    QVector<RoundDto> roundList = m_pDbManager->getRoundList();
    populateRoundModel(roundList);
}

void MainWindow::showAreaDialog(const QList<QPair<QString, QVariant>> &data, int act, const QModelIndex &index)
{
    CommonFormDialog dialog(data, act, this);
    dialog.setWindowTitle(act == CommonFormDialog::TYPE_MODIFY ? QString::fromLocal8Bit("区域修改") : QString::fromLocal8Bit("区域新增"));
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
            m_pDbManager->reloadRoundIdNameMap();
        }
        else
        {
            if (m_pDbManager->insertAreaTable(area) != CS_SUCCEED)
            {
                QMessageBox::warning(this, QString::fromLocal8Bit("新增失败"), QString::fromLocal8Bit("新增失败"));
                return;
            }
            updateAreaModel(area);
            m_pDbManager->reloadRoundIdNameMap();
        }
    }
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
        return;

    if (m_pDbManager->deleteTable(id, "xopensdb.dbo.低周减载区域厂站关联表", "所属分区") != CS_SUCCEED)
        return;

    m_areaModel->removeRow(index.row());
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
void MainWindow::onRoundSelectionModelChanged(const QModelIndex &current)
{
    QString areaName = m_pDbManager->m_areaIdNameMap.value(m_roundModel->index(current.row(), 2).data().toInt(), QString::fromLocal8Bit("未知"));
    ui->m_roundAreaLineEdit->setText(areaName);

    QString funcType = m_pDbManager->m_roundFuncTypeMap.value(m_roundModel->index(current.row(), 3).data().toInt(), QString::fromLocal8Bit("未知"));
    ui->m_roundFuncLineEdit->setText(funcType);

    QString roundType = m_pDbManager->m_roundTypeMap.value(m_roundModel->index(current.row(), 4).data().toInt(), QString::fromLocal8Bit("未知"));
    ui->m_roundTypeLineEdit->setText(roundType);
    ui->m_roundValueLineEdit->setText(m_roundModel->index(current.row(), 5).data().toString());
    ui->m_roundTimeLineEdit->setText(m_roundModel->index(current.row(), 6).data().toString());
    ui->m_roundRequireLineEdit->setText(m_roundModel->index(current.row(), 7).data().toString());

    readRoundItemTable(m_roundModel->index(current.row(), 0).data().toInt());
}

void MainWindow::showRoundContextMenu(const QPoint &pos)
{
    QModelIndex index = ui->m_roundTreeView->indexAt(pos);
    if (index.isValid())
    {
        m_roundContextMenu->exec(ui->m_roundTreeView->viewport()->mapToGlobal(pos));
    }
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
    comboBoxArea.options = m_pDbManager->m_areaIdNameMap;
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

void MainWindow::updateRoundModel(const RoundDto &newRound, int row)
{
    if (row == -1)
    {
        row = m_roundModel->rowCount();
    }
    m_roundModel->setItem(row, 0, new QStandardItem(QString::number(newRound.id)));
    m_roundModel->setItem(row, 1, new QStandardItem(QString::fromLocal8Bit(newRound.name)));
    m_roundModel->setItem(row, 2, new QStandardItem(QString::number(newRound.areaId)));
    m_roundModel->setItem(row, 3, new QStandardItem(QString::number(newRound.funcType)));
    m_roundModel->setItem(row, 4, new QStandardItem(QString::number(newRound.roundType)));
    m_roundModel->setItem(row, 5, new QStandardItem(QString::number(newRound.fixValue)));
    m_roundModel->setItem(row, 6, new QStandardItem(QString::number(newRound.timeValue)));
    m_roundModel->setItem(row, 7, new QStandardItem(QString::number(newRound.requirePower)));
}

int MainWindow::showRoundDialog(const QList<QPair<QString, QVariant>> &data, int act, const QModelIndex &index)
{
    CommonFormDialog dialog(data, act, this);
    dialog.setWindowTitle(act == CommonFormDialog::TYPE_MODIFY ? QString::fromLocal8Bit("轮次修改") : QString::fromLocal8Bit("轮次新增"));
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
            updateRoundModel(round, index.row());
        }
        else
        {
            if (m_pDbManager->insertRoundTable(round) != CS_SUCCEED)
            {
                QMessageBox::warning(this, QString::fromLocal8Bit("新增失败"), QString::fromLocal8Bit("新增失败"));
                return -1;
            }
            updateRoundModel(round);
        }
    }

    return 0;
}

void MainWindow::onAddRoundActionTriggered()
{
    RoundDto newRound;
    newRound.id = m_pDbManager->getMaxIDFromDataBase("xopensdb.dbo.低周减载轮次参数表");
    newRound.areaId = 2;
    newRound.funcType = 1;
    newRound.roundType = 1;
    newRound.fixValue = 49;
    newRound.timeValue = 0.3;
    newRound.requirePower = 0.0;

    QList<QPair<QString, QVariant>> data;
    populateRoundData(data, newRound);

    if (showRoundDialog(data, CommonFormDialog::TYPE_ADD, QModelIndex()) == 0)
    {
        QModelIndex newIndex = m_roundModel->index(m_roundModel->rowCount() - 1, 0);
        ui->m_roundTreeView->setCurrentIndex(newIndex);
    }
}

void MainWindow::onModifyRoundActionTriggered()
{
    QModelIndex currentIndex = ui->m_roundTreeView->currentIndex();
    if (!currentIndex.isValid())
    {
        return;
    }

    RoundDto currentRound;
    currentRound.id = m_roundModel->index(currentIndex.row(), 0).data().toInt();
    strncpy(currentRound.name, m_roundModel->index(currentIndex.row(), 1).data().toString().toLocal8Bit().data(), sizeof(currentRound.name) - 1);
    currentRound.areaId = m_roundModel->index(currentIndex.row(), 2).data().toInt();
    currentRound.funcType = m_roundModel->index(currentIndex.row(), 3).data().toInt();
    currentRound.roundType = m_roundModel->index(currentIndex.row(), 4).data().toInt();
    currentRound.fixValue = m_roundModel->index(currentIndex.row(), 5).data().toDouble();
    currentRound.timeValue = m_roundModel->index(currentIndex.row(), 6).data().toDouble();
    currentRound.requirePower = m_roundModel->index(currentIndex.row(), 7).data().toDouble();

    QList<QPair<QString, QVariant>> data;
    populateRoundData(data, currentRound);

    if (showRoundDialog(data, CommonFormDialog::TYPE_MODIFY, currentIndex) == 0)
    {
        ui->m_roundTreeView->setCurrentIndex(currentIndex);
    }
}

void MainWindow::onDeleteRoundActionTriggered()
{
    QModelIndex currentIndex = ui->m_roundTreeView->currentIndex();
    int id = m_roundModel->index(currentIndex.row(), 0).data().toInt();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  QString::fromLocal8Bit("删除"),
                                  QString::fromLocal8Bit("确定删除编号为%1的轮次吗?").arg(id),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No)
        return;

    if (m_pDbManager->deleteTable(id, "xopensdb.dbo.低周减载轮次参数表") != CS_SUCCEED)
        return;

    m_roundModel->removeRow(currentIndex.row());
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
        m_roundItemModel->setItem(i, 0, new QStandardItem(QString::number(roundItemList[i].id)));
        m_roundItemModel->setItem(i, 1, new QStandardItem(QString::fromLocal8Bit(roundItemList[i].name)));
        m_roundItemModel->setItem(i, 2, new QStandardItem(QString::number(roundItemList[i].areaId)));
        m_roundItemModel->setItem(i, 3, new QStandardItem(QString::number(roundItemList[i].roundId)));
        m_roundItemModel->setItem(i, 4, new QStandardItem(roundItemList[i].linkedLine));
        m_roundItemModel->setItem(i, 5, new QStandardItem(QString::number(roundItemList[i].loadType)));
        m_roundItemModel->setItem(i, 6, new QStandardItem(QString::number(roundItemList[i].strapPlan)));
        m_roundItemModel->setItem(i, 7, new QStandardItem(roundItemList[i].linkedBreak));
    }
}

void MainWindow::onAddRoundItemButtonClicked()
{
    RoundItemDto item;
    memset(&item, 0, sizeof(item));
    item.id = m_pDbManager->getMaxIDFromDataBase("xopensdb.dbo.低周减载轮次项参数表");
    strncpy(item.name, "", sizeof(item.name) - 1);
    item.areaId = 0;
    item.roundId = 0;
    strcpy(item.linkedLine, "");
    item.loadType = 0;
    item.strapPlan = 0;
    strcpy(item.linkedBreak, "");

    QList<QPair<QString, QVariant>> data;
    populateRoundItemData(data, item);

    showRoundItemDialog(data, CommonFormDialog::TYPE_ADD, QModelIndex());
}

void MainWindow::populateRoundItemData(QList<QPair<QString, QVariant>> &data, const RoundItemDto &item)
{
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("编号"), QString::number(item.id)));
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("名称"), QString::fromLocal8Bit(item.name)));
    ComboBoxData<int> comboBoxArea;
    comboBoxArea.currentValue = item.areaId;
    comboBoxArea.options = m_pDbManager->m_areaIdNameMap;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("所属分区"), QVariant::fromValue(comboBoxArea)));

    ComboBoxData<int> comboBoxRound;
    comboBoxRound.currentValue = item.roundId;
    comboBoxRound.options = m_pDbManager->m_roundIdNameMap;
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("所属轮次"), QVariant::fromValue(comboBoxRound)));

    JsonDialogData lineData;
    lineData.initialText = item.linkedLine;
    lineData.isMultiSelect = false;
    lineData.jsonData = getLineJson(m_pDbManager->m_lineList);
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
    breakData.jsonData = getBreakerJson(m_pDbManager->m_breakerList);
    data.append(QPair<QString, QVariant>(QString::fromLocal8Bit("关联开关"), QVariant::fromValue(breakData)));
}

RoundItemDto MainWindow::extractRoundItemData(const QList<QPair<QString, QVariant>> &updatedData)
{
    RoundItemDto newRoundItem;
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
    }
    return newRoundItem;
}

void MainWindow::showRoundItemDialog(const QList<QPair<QString, QVariant>> &data, int act, const QModelIndex &index)
{
    CommonFormDialog dialog(data, act, this);
    dialog.setWindowTitle(act == CommonFormDialog::TYPE_MODIFY ? QString::fromLocal8Bit("轮次项修改") : QString::fromLocal8Bit("轮次项新增"));
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
        }
        else
        {
            if (m_pDbManager->insertRoundItemTable(item) != CS_SUCCEED)
            {
                QMessageBox::warning(this, QString::fromLocal8Bit("新增失败"), QString::fromLocal8Bit("新增失败"));
                return;
            }
            updateRoundItemModel(item);
        }
    }
}

void MainWindow::updateRoundItemModel(const RoundItemDto &newRoundItem, int row)
{
    if (row == -1)
    {
        row = m_roundItemModel->rowCount();
    }
    m_roundItemModel->setItem(row, 0, new QStandardItem(QString::number(newRoundItem.id)));
    m_roundItemModel->setItem(row, 1, new QStandardItem(QString::fromLocal8Bit(newRoundItem.name)));
    m_roundItemModel->setItem(row, 2, new QStandardItem(QString::number(newRoundItem.areaId)));
    m_roundItemModel->setItem(row, 3, new QStandardItem(QString::number(newRoundItem.roundId)));
    m_roundItemModel->setItem(row, 4, new QStandardItem(newRoundItem.linkedLine));
    m_roundItemModel->setItem(row, 5, new QStandardItem(QString::number(newRoundItem.loadType)));
    m_roundItemModel->setItem(row, 6, new QStandardItem(QString::number(newRoundItem.strapPlan)));
    m_roundItemModel->setItem(row, 7, new QStandardItem(newRoundItem.linkedBreak));
}

dfJson::Value MainWindow::getLineJson(const QVector<LineDto> &lines)
{
    QMap<QString, QVector<LineDto>> stationMap;

    // 将所有线路按变电站 ID 分类
    for (const auto &line : lines)
    {
        // qDebug("line.staName=%s line.name=%s", line.staName, line.name);
        stationMap[QString::fromLocal8Bit(line.staId)].push_back(line);
    }

    // 构造 JSON 对象
    dfJson::Value root(dfJson::arrayValue); // 创建一个空的Json数组

    QMap<QString, QVector<LineDto>>::iterator it;
    for (it = stationMap.begin(); it != stationMap.end(); ++it)
    {
        // 变电站
        dfJson::Value station(dfJson::objectValue);    // 创建一个空的Json对象
        station["id"] = it.key().toLocal8Bit().data(); // 变电站 ID
        station["name"] = it.value().first().staName;  // 变电站名称
        station["type"] = "substation";                // 变电站类型

        dfJson::Value children(dfJson::arrayValue);    // 创建一个空的Json数组

        // 线路
        for (const auto &line : it.value())
        {
            dfJson::Value child(dfJson::objectValue); // 创建一个空的Json对象
            child["id"] = line.id;                    // 线路 ID
            child["name"] = line.name;                // 线路名称
            child["type"] = "line";                   // 线路类型
            children.append(child);
        }

        station["children"] = children;
        root.append(station);
    }

    qDebug() << QString::fromLocal8Bit(jsonToString(root).c_str());

    return root;
}

dfJson::Value MainWindow::getBreakerJson(const QVector<BreakDto> &breakers)
{
    QMap<QString, QVector<BreakDto>> stationMap;

    // 将所有线路按变电站 ID 分类
    for (const auto &breaker : breakers)
    {
        // qDebug("line.staName=%s line.name=%s", line.staName, line.name);
        stationMap[QString::fromLocal8Bit(breaker.staId)].push_back(breaker);
    }

    // 构造 JSON 对象
    dfJson::Value root(dfJson::arrayValue); // 创建一个空的Json数组

    QMap<QString, QVector<BreakDto>>::iterator it;
    for (it = stationMap.begin(); it != stationMap.end(); ++it)
    {
        // 变电站
        dfJson::Value station(dfJson::objectValue);    // 创建一个空的Json对象
        station["id"] = it.key().toLocal8Bit().data(); // 变电站 ID
        station["name"] = it.value().first().staName;  // 变电站名称
        station["type"] = "substation";                // 变电站类型

        dfJson::Value children(dfJson::arrayValue);    // 创建一个空的Json数组

        // 开关
        for (const auto &breaker : it.value())
        {
            dfJson::Value child(dfJson::objectValue); // 创建一个空的Json对象
            child["id"] = breaker.id;                 // 开关ID
            child["name"] = breaker.name;             // 开关名称
            child["type"] = "breaker";                // 开关类型
            children.append(child);
        }

        station["children"] = children;
        root.append(station);
    }

    qDebug() << QString::fromLocal8Bit(jsonToString(root).c_str());

    return root;
}
