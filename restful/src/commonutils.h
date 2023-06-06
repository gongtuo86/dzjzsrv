#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <iconv.h>

#include <iostream>
#include <sstream>
#include <string>

#include "dfjson/json.h"

static std::string utf8ToGb2312(const std::string& str)
{
    iconv_t cd = iconv_open("GB2312", "UTF-8");
    if (cd == (iconv_t)-1)
    {
        std::cerr << "Error: Cannot open iconv descriptor" << std::endl;
        return "";
    }

    std::vector<char> inBuffer(str.begin(), str.end());
    size_t inBytesLeft = inBuffer.size();
    size_t outBufferSize = inBytesLeft * 4;
    std::vector<char> outBuffer(outBufferSize);

    char* inPointer = inBuffer.data();
    char* outPointer = outBuffer.data();
    size_t outBytesLeft = outBufferSize;

    if (iconv(cd, &inPointer, &inBytesLeft, &outPointer, &outBytesLeft) == (size_t)-1)
    {
        std::cerr << "Error: Cannot convert encoding" << std::endl;
        iconv_close(cd);
        return "";
    }

    iconv_close(cd);
    return std::string(outBuffer.data(), outBuffer.size() - outBytesLeft);
}

static std::string gb2312ToUtf8(const std::string& str)
{
    iconv_t cd = iconv_open("UTF-8", "GB2312");
    if (cd == (iconv_t)-1)
    {
        std::cerr << "Error: Cannot open iconv descriptor" << std::endl;
        return "";
    }

    std::vector<char> inBuffer(str.begin(), str.end());
    size_t inBytesLeft = inBuffer.size();
    size_t outBufferSize = inBytesLeft * 4;
    std::vector<char> outBuffer(outBufferSize);

    char* inPointer = inBuffer.data();
    char* outPointer = outBuffer.data();
    size_t outBytesLeft = outBufferSize;

    if (iconv(cd, &inPointer, &inBytesLeft, &outPointer, &outBytesLeft) == (size_t)-1)
    {
        std::cerr << "Error: Cannot convert encoding" << std::endl;
        iconv_close(cd);
        return "";
    }

    iconv_close(cd);
    return std::string(outBuffer.data(), outBuffer.size() - outBytesLeft);
}

/**
 * @brief 生成socketid，用于管理socket
 *
 * @return std::string
 */
static std::string generateSocketID()
{
   // return "test";

    size_t length = 32;
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<> distribution(0, characters.size() - 1);

    std::stringstream socketid;
    for (size_t i = 0; i < length; ++i)
    {
        socketid << characters[distribution(generator)];
    }

    return socketid.str();
}

/**
 * @brief 输出json字符串
 *
 * @param jsonValue
 * @return std::string
 */
static std::string jsonToString(const dfJson::Value& jsonValue)
{
    dfJson::StreamWriterBuilder writerBuilder;
    writerBuilder.settings_["indentation"] = ""; // 用于格式化输出的空格数，这里我们不需要缩进
    writerBuilder.settings_["commentStyle"] = "None";
    writerBuilder.settings_["emitUTF8"] = true;  // 输出utf8

    return dfJson::writeString(writerBuilder, jsonValue);
}
#endif