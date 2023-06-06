#ifndef EPROTMMICTRL_H
#define EPROTMMICTRL_H

#include <functional>
#include <string>
#include "dfjson/json.h"

#ifdef EPROTMMICTRL_DLL
#ifdef WIN32
#define _EPROTMMI_CTRL_DLL_EXPORT __declspec(dllexport)
#else
#define _EPROTMMI_CTRL_DLL_EXPORT
#endif
#else
#ifdef WIN32
#define _EPROTMMI_CTRL_DLL_EXPORT __declspec(dllimport)
#else
#define _EPROTMMI_CTRL_DLL_EXPORT
#endif
#endif

using ProgressCallback = std::function<void(const std::string& progress)>;

_EPROTMMI_CTRL_DLL_EXPORT int executeBatch();
_EPROTMMI_CTRL_DLL_EXPORT std::string executeInspectionByCond(const std::string& jsonPara, ProgressCallback progressCallback);
_EPROTMMI_CTRL_DLL_EXPORT dfJson::Value getCentrialDevice(const std::string& jsonPara);
_EPROTMMI_CTRL_DLL_EXPORT dfJson::Value getRealEvent();

#endif // EPROTMMICTRL_H