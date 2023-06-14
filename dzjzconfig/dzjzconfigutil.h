#ifndef DZJZ_CONFIG_UTIL_H
#define DZJZ_CONFIG_UTIL_H

#include <QSortFilterProxyModel>

#include "dfjson/json.h"

static std::string jsonToString(const dfJson::Value &jsonValue)
{
    dfJson::StreamWriterBuilder writerBuilder;
    writerBuilder.settings_["indentation"] = ""; // 用于格式化输出的空格数，这里我们不需要缩进
    writerBuilder.settings_["commentStyle"] = "None";
    writerBuilder.settings_["emitUTF8"] = true;  // 输出utf8

    return dfJson::writeString(writerBuilder, jsonValue);
}

class RoundItemFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    RoundItemFilterProxyModel(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent)
    {
    }

    void setAreaFilter(int area)
    {
        areaFilter = area;
        invalidateFilter();
    }

    void setRoundFilter(int round)
    {
        roundFilter = round;
        invalidateFilter();
    }

    void setLoadTypeFilter(int loadType)
    {
        loadTypeFilter = loadType;
        invalidateFilter();
    }

    void setStrapFilter(int strap)
    {
        strapFilter = strap;
        invalidateFilter();
    }

    void setDeviceFilter(int device)
    {
        deviceFilter = device;
        invalidateFilter();
    }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
    {
        QModelIndex areaIndex = sourceModel()->index(sourceRow, 2, sourceParent);
        QModelIndex roundIndex = sourceModel()->index(sourceRow, 3, sourceParent);
        QModelIndex loadTypeIndex = sourceModel()->index(sourceRow, 5, sourceParent);
        QModelIndex strapIndex = sourceModel()->index(sourceRow, 6, sourceParent);
        QModelIndex deviceIndex = sourceModel()->index(sourceRow, 9, sourceParent);

        return (sourceModel()->data(areaIndex).toInt() == areaFilter || areaFilter == -1) &&
               (sourceModel()->data(roundIndex).toInt() == roundFilter || roundFilter == -1) &&
               (sourceModel()->data(loadTypeIndex).toInt() == loadTypeFilter || loadTypeFilter == -1) &&
               (sourceModel()->data(strapIndex).toInt() == strapFilter || strapFilter == -1) &&
               (sourceModel()->data(deviceIndex).toInt() == deviceFilter || deviceFilter == -1);
    }

private:
    int areaFilter = -1;
    int roundFilter = -1;
    int loadTypeFilter = -1;
    int strapFilter = -1;
    int deviceFilter = -1;
};

#endif // DZJZ_CONFIG_UTIL_H