#ifndef DZJZ_CONFIG_UTIL_H
#define DZJZ_CONFIG_UTIL_H

#include "dfjson/json.h"

static std::string jsonToString(const dfJson::Value& jsonValue)
{
    dfJson::StreamWriterBuilder writerBuilder;
    writerBuilder.settings_["indentation"] = ""; // 用于格式化输出的空格数，这里我们不需要缩进
    writerBuilder.settings_["commentStyle"] = "None";
    writerBuilder.settings_["emitUTF8"] = true;  // 输出utf8

    return dfJson::writeString(writerBuilder, jsonValue);
}

#endif // DZJZ_CONFIG_UTIL_H