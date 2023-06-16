#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QStandardItemModel>
#include <QItemSelection>
#include <QComboBox>
#include <QSortFilterProxyModel>

#include "dfjson/json.h"
#include "dto.h"
#include "dzjzconfigutil.h"

class QListWidgetItem;
class OperationDelegate;
class DBManager;

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void setupModels();

    void setupAreaTable();
    void readAreaTable();
    void populateAreaModel(const QVector<AreaVo> &areaList);
    void populateAreaData(QList<QPair<QString, QVariant>> &data, const AreaVo &area);
    AreaVo extractAreaData(const QList<QPair<QString, QVariant>> &updatedData);
    void updateAreaModel(const AreaVo &newArea, int row = -1);
    void showAreaDialog(const QList<QPair<QString, QVariant>> &data, int act, const QModelIndex &index);

    void setupRoundTree();
    void readRoundTable();
    void populateRoundModel(const QVector<RoundDto> &roundList);
    void populateRoundData(QList<QPair<QString, QVariant>> &data, const RoundDto &round);
    RoundDto extractRoundData(const QList<QPair<QString, QVariant>> &updatedData);
    void updateRoundModel(const RoundDto &newRound, const QModelIndex &index);
    int showRoundDialog(const QList<QPair<QString, QVariant>> &data, int act, const QModelIndex &index);

    void setupRoundItemTable();
    void readRoundItemTable(int roundId);
    void populateRoundItemModel(const QVector<RoundItemDto> &roundItemList);
    void populateRoundItemData(QList<QPair<QString, QVariant>> &data, const RoundItemDto &item);
    RoundItemDto extractRoundItemData(const QList<QPair<QString, QVariant>> &updatedData);
    void showRoundItemDialog(const QList<QPair<QString, QVariant>> &data, int act, const QModelIndex &index);
    void updateRoundItemModel(const RoundItemDto &newRoundItem, int row = -1);
    void addActionToMenu(QMenu *menu, int id, const QString &name, const char *slot);
    void updateComboBox(QComboBox *comboBox, const QMap<int, QString> &map);

    void setupDeviceTable();
    void readDeviceTable();
    void populateDeviceModel(const QVector<DeviceDto> &deviceList);
    void populateDeviceData(QList<QPair<QString, QVariant>> &data, const DeviceDto &item);
    void showDeviceDialog(const QList<QPair<QString, QVariant>> &data, int act, const QModelIndex &index);
    DeviceDto extractDeviceData(const QList<QPair<QString, QVariant>> &updatedData);
    void updateDeviceModel(const DeviceDto &newDevice, int row = -1);

    void setupDeviceParaTable();
    void readDeviceParaTable(int deviceId);
    void populateDeviceParaModel(const QVector<DeviceParaDto> &deviceList);
    void populateDeviceParaData(QList<QPair<QString, QVariant>> &data, const DeviceParaDto &item);
    void showDeviceParaDialog(const QList<QPair<QString, QVariant>> &data, int act, const QModelIndex &index);
    DeviceParaDto extractDeviceParaData(const QList<QPair<QString, QVariant>> &updatedData);
    void updateDeviceParaModel(const DeviceParaDto &newDevice, int row = -1);

    void setupTaskTable();
    void readTaskTable();
    void populateTaskModel(const QVector<TaskDto> &areaList);
    void populateTaskData(QList<QPair<QString, QVariant>> &data, const TaskDto &task);
    void showTaskDialog(const QList<QPair<QString, QVariant>> &data, int act, const QModelIndex &index);
    TaskDto extractTaskData(const QList<QPair<QString, QVariant>> &updatedData);
    void updateTaskModel(const TaskDto &newTask, int row = -1);

private slots:
    void onModuleItemClicked(QListWidgetItem *item);

    void onDetailButtonAreaClicked(QModelIndex index);
    void onDeleteButtonAreaClicked(QModelIndex index);
    void onModifyButtonAreaClicked(QModelIndex index);
    void onAddButtonAreaClicked();

    void onRoundSelectionModelChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void showRoundContextMenu(const QPoint &pos);
    void onAddRoundActionTriggered();
    void onModifyRoundActionTriggered();
    void onDeleteRoundActionTriggered();

    void onAddRoundItemButtonClicked();
    void onEditRoundItemButtonClicked();
    void onDeleteButtonRoundItemClicked(QModelIndex index);
    void onModifyButtonRoundItemClicked(QModelIndex index);
    void onDetailButtonRoundItemClicked(QModelIndex index);
    void onDeviceTableRowSelected(const QModelIndex &index);
    void showRoundItemContextMenu(const QPoint &pos);
    void updateRoundItemFilter(int index);
    void updateDeviceOptions(int index);

    void onAddDeviceButtonClicked();
    void onDetailButtonDeviceClicked(QModelIndex index);
    void onModifyButtonDeviceClicked(QModelIndex index);
    void onDeleteButtonDeviceClicked(QModelIndex index);

    void onAddDeviceParaButtonClicked();
    void onDetailButtonDeviceParaClicked(QModelIndex index);
    void onModifyButtonDeviceParaClicked(QModelIndex index);
    void onDeleteButtonDeviceParaClicked(QModelIndex index);

    void onSetArea();
    void onSetRound();
    void onSetLoadType();
    void onSetStrap();
    void onSetDevice();

    void onAddButtonTaskClicked();
    void onDetailButtonTaskClicked(QModelIndex index);
    void onDeleteButtonTaskClicked(QModelIndex index);
    void onModifyButtonTaskClicked(QModelIndex index);

private:
    Ui::MainWindow *ui;
    DBManager *m_pDbManager;

    QStandardItemModel *m_areaModel;
    OperationDelegate *m_BtnDelegateArea;

    QStandardItemModel *m_roundModel;

    QStandardItemModel *m_roundItemModel;
    RoundItemFilterProxyModel *m_roundItemProxyModel;
    OperationDelegate *m_BtnDelegateRoundItem;

    QStandardItemModel *m_deviceModel;
    OperationDelegate *m_BtnDelegateDevice;

    QStandardItemModel *m_deviceParaModel;
    OperationDelegate *m_BtnDelegateDevicePara;

    QStandardItemModel *m_taskModel;
    OperationDelegate *m_btnDelegateTask;

    QMenu *m_roundContextMenu;
    QMenu *m_roundItemContextMenu;
    QMenu *m_setAreaMenu;
    QMenu *m_setRoundMenu;
    QMenu *m_setStrapMenu;
    QMenu *m_setLoadTypeMenu;
};

#endif // MAINWINDOW_H
