#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QStandardItemModel>

#include "dfjson/json.h"
#include "dto.h"

class QListWidgetItem;
class OperationDelegate;

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

    QMap<QString, QString> getLineIdNameMap(const QVector<LineDto> &lines);
    dfJson::Value getLineJson(const QVector<LineDto> &lines);

    QMap<QString, QString> getBreakerIdNameMap(const QVector<BreakDto> &breakers);
    dfJson::Value getBreakerJson(const QVector<BreakDto> &lines);

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

private:
    Ui::MainWindow *ui;

    QStandardItemModel *m_areaModel;
    OperationDelegate *m_BtnDelegateArea;

    QStandardItemModel *m_roundModel;

    QMap<int, QString> m_areaTypeMap;        // 区域类型
    QMap<int, QString> m_roundFuncTypeMap;   // 轮次功能类型
    QMap<int, QString> m_areaIdNameMap;      // 区域
    QMap<int, QString> m_roundTypeMap;       // 轮次类型
    QMap<int, QString> m_loadTypeMap;        // 负荷类型
    QMap<int, QString> m_strapMap;           // 投退

    QMap<QString, QString> m_staIdNameMap;   // 变电站

    QMap<QString, QString> m_lineIdNameMap;  // 馈线
    dfJson::Value m_lineTreeJson;            // 线路json
    QVector<LineDto> m_lineList;             // 线路列表

    QMap<QString, QString> m_breakIdNameMap; // 开关
    dfJson::Value m_breakJson;               // 开关json
    QVector<BreakDto> m_breakList;           // 开关列表

    QStandardItemModel *m_roundItemModel;
    QMap<int, QString> m_roundIdNameMap;       // 轮次map
    OperationDelegate *m_BtnDelegateRoundItem; // 轮次项按钮代理

    QMenu *m_roundContextMenu;
    QAction *m_addRoundAction;
    QAction *m_modifyRoundAction;
    QAction *m_deleteRoundAction;
};

#endif // MAINWINDOW_H
