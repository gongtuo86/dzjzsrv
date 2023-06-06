#ifndef DZJZ_CONFIG_UTIL_H
#define DZJZ_CONFIG_UTIL_H

#include "dfjson/json.h"

static std::string jsonToString(const dfJson::Value& jsonValue)
{
    dfJson::StreamWriterBuilder writerBuilder;
    writerBuilder.settings_["indentation"] = ""; // ���ڸ�ʽ������Ŀո������������ǲ���Ҫ����
    writerBuilder.settings_["commentStyle"] = "None";
    writerBuilder.settings_["emitUTF8"] = true;  // ���utf8

    return dfJson::writeString(writerBuilder, jsonValue);
}

#endif // DZJZ_CONFIG_UTIL_H