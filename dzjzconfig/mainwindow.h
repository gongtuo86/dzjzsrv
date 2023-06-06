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

    QMap<int, QString> m_areaTypeMap;        // ��������
    QMap<int, QString> m_roundFuncTypeMap;   // �ִι�������
    QMap<int, QString> m_areaIdNameMap;      // ����
    QMap<int, QString> m_roundTypeMap;       // �ִ�����
    QMap<int, QString> m_loadTypeMap;        // ��������
    QMap<int, QString> m_strapMap;           // Ͷ��

    QMap<QString, QString> m_staIdNameMap;   // ���վ

    QMap<QString, QString> m_lineIdNameMap;  // ����
    dfJson::Value m_lineTreeJson;            // ��·json
    QVector<LineDto> m_lineList;             // ��·�б�

    QMap<QString, QString> m_breakIdNameMap; // ����
    dfJson::Value m_breakJson;               // ����json
    QVector<BreakDto> m_breakList;           // �����б�

    QStandardItemModel *m_roundItemModel;
    QMap<int, QString> m_roundIdNameMap;       // �ִ�map
    OperationDelegate *m_BtnDelegateRoundItem; // �ִ��ť����

    QMenu *m_roundContextMenu;
    QAction *m_addRoundAction;
    QAction *m_modifyRoundAction;
    QAction *m_deleteRoundAction;
};

#endif // MAINWINDOW_H
