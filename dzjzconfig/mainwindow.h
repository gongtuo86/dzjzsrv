#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QStandardItemModel>

#include "dfjson/json.h"
#include "dto.h"

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
    void updateRoundModel(const RoundDto &newRound, int row = -1);
    int showRoundDialog(const QList<QPair<QString, QVariant>> &data, int act, const QModelIndex &index);

    void setupRoundItemTable();
    void readRoundItemTable(int roundId);
    void populateRoundItemModel(const QVector<RoundItemDto> &roundItemList);
    void populateRoundItemData(QList<QPair<QString, QVariant>> &data, const RoundItemDto &item);
    RoundItemDto extractRoundItemData(const QList<QPair<QString, QVariant>> &updatedData);
    void showRoundItemDialog(const QList<QPair<QString, QVariant>> &data, int act, const QModelIndex &index);
    void updateRoundItemModel(const RoundItemDto &newRoundItem, int row = -1);

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

    dfJson::Value getRtuJson(const QVector<RtuDto> &rtus);
    dfJson::Value getLineJson(const QVector<LineDto> &lines);
    dfJson::Value getBreakerJson(const QVector<BreakDto> &lines);
    dfJson::Value getDeviceJson(const QVector<DeviceDto> &devices);
    dfJson::Value getFixValueJson(const QVector<FixValueDto> &fixValues);

private slots:
    void onModuleItemClicked(QListWidgetItem *item);

    void onDetailButtonAreaClicked(QModelIndex index);
    void onDeleteButtonAreaClicked(QModelIndex index);
    void onModifyButtonAreaClicked(QModelIndex index);
    void onAddButtonAreaClicked();

    void onRoundSelectionModelChanged(const QModelIndex &current);
    void showRoundContextMenu(const QPoint &pos);
    void onAddRoundActionTriggered();
    void onModifyRoundActionTriggered();
    void onDeleteRoundActionTriggered();

    void onAddRoundItemButtonClicked();
    void onDeleteButtonRoundItemClicked(QModelIndex index);
    void onModifyButtonRoundItemClicked(QModelIndex index);
    void onDetailButtonRoundItemClicked(QModelIndex index);
    void onDeviceTableRowSelected(const QModelIndex &index);

    void onAddDeviceButtonClicked();
    void onDetailButtonDeviceClicked(QModelIndex index);
    void onModifyButtonDeviceClicked(QModelIndex index);
    void onDeleteButtonDeviceClicked(QModelIndex index);

    void onAddDeviceParaButtonClicked();
    void onDetailButtonDeviceParaClicked(QModelIndex index);
    void onModifyButtonDeviceParaClicked(QModelIndex index);
    void onDeleteButtonDeviceParaClicked(QModelIndex index);

private:
    Ui::MainWindow *ui;
    DBManager *m_pDbManager;

    QStandardItemModel *m_areaModel;
    OperationDelegate *m_BtnDelegateArea;

    QStandardItemModel *m_roundModel;

    QStandardItemModel *m_roundItemModel;
    OperationDelegate *m_BtnDelegateRoundItem;

    QStandardItemModel *m_deviceModel;
    OperationDelegate *m_BtnDelegateDevice;

    QStandardItemModel *m_deviceParaModel;
    OperationDelegate *m_BtnDelegateDevicePara;

    QMenu *m_roundContextMenu;
    QAction *m_addRoundAction;
    QAction *m_modifyRoundAction;
    QAction *m_deleteRoundAction;
};

#endif // MAINWINDOW_H
