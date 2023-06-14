#ifndef DZJZ_CONFIG_UTIL_H
#define DZJZ_CONFIG_UTIL_H

#include "dfjson/json.h"

static std::string jsonToString(const dfJson::Value &jsonValue)
{
    dfJson::StreamWriterBuilder writerBuilder;
    writerBuilder.settings_["indentation"] = ""; // 用于格式化输出的空格数，这里我们不需要缩进
    writerBuilder.settings_["commentStyle"] = "None";
    writerBuilder.settings_["emitUTF8"] = true;  // 输出utf8

    return dfJson::writeString(writerBuilder, jsonValue);
}

// class CustomStandardItemModel : public QStandardItemModel
// {
// public:
//     CustomStandardItemModel(QObject *parent = 0) : QStandardItemModel(parent) {}

// protected:
//     bool lessThan(const QModelIndex &left, const QModelIndex &right) const
//     {
//         bool ok1, ok2;
//         int leftNumber = left.data().toInt(&ok1);
//         int rightNumber = right.data().toInt(&ok2);

//         if (ok1 && ok2)
//             return leftNumber < rightNumber;
//         else
//             return QStandardItemModel::lessThan(left, right);
//     }
// };

#endif // DZJZ_CONFIG_UTIL_H