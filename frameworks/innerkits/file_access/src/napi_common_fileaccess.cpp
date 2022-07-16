/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "napi_common_fileaccess.h"

#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "napi_common_want.h"

namespace OHOS {
namespace FileAccessFwk {
using namespace OHOS::AppExecFwk;
bool UnwrapInt64ByPropertyName(napi_env env, napi_value param, const char *propertyName, int64_t &value)
{
    napi_value jsValue = GetPropertyValueByPropertyName(env, param, propertyName, napi_number);
    if (jsValue != nullptr) {
        return UnwrapInt64FromJS2(env, jsValue, value);
    } else {
        return false;
    }
}

napi_value WrapUint32ToJS(napi_env env, uint32_t value)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_uint32(env, value, &result));
    return result;
}

uint32_t UnwrapUint32FromJS(napi_env env, napi_value param, uint32_t defaultValue)
{
    uint32_t value = defaultValue;
    if (napi_get_value_uint32(env, param, &value) == napi_ok) {
        return value;
    } else {
        return defaultValue;
    }
}

bool UnwrapUint32FromJS2(napi_env env, napi_value param, uint32_t &value)
{
    bool result = false;
    if (napi_get_value_uint32(env, param, &value) == napi_ok) {
        result = true;
    }
    return result;
}

bool UnwrapUint32ByPropertyName(napi_env env, napi_value param, const char *propertyName, uint32_t &value)
{
    napi_value jsValue = GetPropertyValueByPropertyName(env, param, propertyName, napi_number);
    if (jsValue != nullptr) {
        return UnwrapUint32FromJS2(env, jsValue, value);
    } else {
        return false;
    }
}

napi_value WrapBigIntUint64ToJS(napi_env env, uint64_t value)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_bigint_uint64(env, value, &result));
    return result;
}

uint64_t UnwrapBigIntUint64FromJS(napi_env env, napi_value param, uint64_t defaultValue)
{
    bool lossless = true;
    uint64_t value = defaultValue;
    if (napi_get_value_bigint_uint64(env, param, &value, &lossless) == napi_ok) {
        return value;
    } else {
        return defaultValue;
    }
}

bool UnwrapBigIntUint64FromJS2(napi_env env, napi_value param, uint64_t &defaultValue)
{
    bool lossless = true;
    if (napi_get_value_bigint_uint64(env, param, &defaultValue, &lossless) == napi_ok) {
        return true;
    } else {
        return false;
    }
}

bool UnwrapBigIntUint64ByPropertyName(napi_env env, napi_value param, const char *propertyName, uint64_t &value)
{
    napi_value jsValue = GetPropertyValueByPropertyName(env, param, propertyName, napi_bigint);
    if (jsValue != nullptr) {
        return UnwrapBigIntUint64FromJS2(env, jsValue, value);
    } else {
        return false;
    }
}

napi_value WrapFileInfo(napi_env env, const FileInfo &fileInfo)
{
    napi_value jsObject = nullptr;
    napi_value jsValue = nullptr;

    NAPI_CALL(env, napi_create_object(env, &jsObject));

    jsValue = nullptr;
    jsValue = OHOS::AppExecFwk::WrapStringToJS(env, fileInfo.uri.ToString());
    SetPropertyValueByPropertyName(env, jsObject, "uri", jsValue);

    jsValue = nullptr;
    jsValue = OHOS::AppExecFwk::WrapStringToJS(env, fileInfo.fileName);
    SetPropertyValueByPropertyName(env, jsObject, "fileName", jsValue);

    jsValue = nullptr;
    jsValue = OHOS::AppExecFwk::WrapStringToJS(env, fileInfo.mode);
    SetPropertyValueByPropertyName(env, jsObject, "mode", jsValue);

    jsValue = nullptr;
    jsValue = OHOS::AppExecFwk::WrapInt64ToJS(env, fileInfo.size);
    SetPropertyValueByPropertyName(env, jsObject, "size", jsValue);

    jsValue = nullptr;
    jsValue = OHOS::AppExecFwk::WrapInt64ToJS(env, fileInfo.mtime);
    SetPropertyValueByPropertyName(env, jsObject, "mtime", jsValue);

    jsValue = nullptr;
    jsValue = OHOS::AppExecFwk::WrapStringToJS(env, fileInfo.mimeType);
    SetPropertyValueByPropertyName(env, jsObject, "mimeType", jsValue);

    return jsObject;
}

bool UnwrapFileInfo(napi_env env, napi_value param, FileInfo &fileInfo)
{
    if (!IsTypeForNapiValue(env, param, napi_object)) {
        return false;
    }

    std::string natValueString("");
    if (OHOS::AppExecFwk::UnwrapStringByPropertyName(env, param, "uri", natValueString)) {
        fileInfo.uri = Uri(natValueString);
    }

    natValueString = "";
    if (OHOS::AppExecFwk::UnwrapStringByPropertyName(env, param, "fileName", natValueString)) {
        fileInfo.fileName = natValueString;
    }

    natValueString = "";
    if (OHOS::AppExecFwk::UnwrapStringByPropertyName(env, param, "mode", natValueString)) {
        fileInfo.mode = natValueString;
    }

    int64_t natValueInt64 = ERR_OK;
    if (UnwrapInt64ByPropertyName(env, param, "size", natValueInt64)) {
        fileInfo.size = natValueInt64;
    }

    natValueInt64 = ERR_OK;
    if (UnwrapInt64ByPropertyName(env, param, "mtime", natValueInt64)) {
        fileInfo.mtime = natValueInt64;
    }

    natValueString = "";
    if (OHOS::AppExecFwk::UnwrapStringByPropertyName(env, param, "mimeType", natValueString)) {
        fileInfo.mimeType = natValueString;
    }

    return true;
}

napi_value WrapArrayFileInfoToJS(napi_env env, const std::vector<FileInfo> &fileInfoVec)
{
    napi_value jsArray = nullptr;
    napi_value jsValue = nullptr;
    uint32_t index = ERR_OK;
    NAPI_CALL(env, napi_create_array(env, &jsArray));

    for (uint32_t i = 0; i < fileInfoVec.size(); i++) {
        jsValue = WrapFileInfo(env, fileInfoVec[i]);
        if (napi_set_element(env, jsArray, index, jsValue) == napi_ok) {
            index++;
        }
    }
    return jsArray;
}

bool UnwrapArrayFileInfoFromJS(napi_env env, napi_value param, std::vector<FileInfo> &fileInfoVec)
{
    uint32_t arraySize = ERR_OK;
    napi_value jsValue = nullptr;
    if (!IsArrayForNapiValue(env, param, arraySize)) {
        return false;
    }

    fileInfoVec.clear();
    for (uint32_t i = 0; i < arraySize; i++) {
        jsValue = nullptr;
        FileInfo fileInfo;
        if (napi_get_element(env, param, i, &jsValue) != napi_ok) {
            return false;
        }

        if (!UnwrapFileInfo(env, jsValue, fileInfo)) {
            return false;
        }

        fileInfoVec.push_back(fileInfo);
    }
    return true;
}

napi_value WrapDeviceInfo(napi_env env, const DeviceInfo &deviceInfo)
{
    napi_value jsObject = nullptr;
    napi_value jsValue = nullptr;

    NAPI_CALL(env, napi_create_object(env, &jsObject));

    jsValue = nullptr;
    jsValue = OHOS::AppExecFwk::WrapStringToJS(env, deviceInfo.uri.ToString());
    SetPropertyValueByPropertyName(env, jsObject, "uri", jsValue);

    jsValue = nullptr;
    jsValue = OHOS::AppExecFwk::WrapStringToJS(env, deviceInfo.displayName);
    SetPropertyValueByPropertyName(env, jsObject, "displayName", jsValue);

    jsValue = nullptr;
    jsValue = OHOS::AppExecFwk::WrapStringToJS(env, deviceInfo.deviceId);
    SetPropertyValueByPropertyName(env, jsObject, "deviceId", jsValue);

    jsValue = nullptr;
    jsValue = WrapUint32ToJS(env, deviceInfo.flags);
    SetPropertyValueByPropertyName(env, jsObject, "flags", jsValue);

    return jsObject;
}

bool UnwrapDeviceInfo(napi_env env, napi_value param, DeviceInfo &deviceInfo)
{
    if (!IsTypeForNapiValue(env, param, napi_object)) {
        return false;
    }

    std::string natValueString("");
    if (OHOS::AppExecFwk::UnwrapStringByPropertyName(env, param, "uri", natValueString)) {
        deviceInfo.uri = Uri(natValueString);
    }

    natValueString = "";
    if (OHOS::AppExecFwk::UnwrapStringByPropertyName(env, param, "displayName", natValueString)) {
        deviceInfo.displayName = natValueString;
    }

    natValueString = "";
    if (OHOS::AppExecFwk::UnwrapStringByPropertyName(env, param, "deviceId", natValueString)) {
        deviceInfo.deviceId = natValueString;
    }

    uint32_t natValueUint32 = ERR_OK;
    if (UnwrapUint32ByPropertyName(env, param, "flags", natValueUint32)) {
        deviceInfo.flags = natValueUint32;
    }
    return true;
}

napi_value WrapArrayDeviceInfoToJS(napi_env env, const std::vector<DeviceInfo> &deviceInfoVec)
{
    napi_value jsArray = nullptr;
    napi_value jsValue = nullptr;
    uint32_t index = ERR_OK;
    NAPI_CALL(env, napi_create_array(env, &jsArray));

    for (uint32_t i = 0; i < deviceInfoVec.size(); i++) {
        jsValue = WrapDeviceInfo(env, deviceInfoVec[i]);
        if (napi_set_element(env, jsArray, index, jsValue) == napi_ok) {
            index++;
        }
    }
    return jsArray;
}

bool UnwrapArrayDeviceInfoFromJS(napi_env env, napi_value param, std::vector<DeviceInfo> &deviceInfoVec)
{
    uint32_t arraySize = ERR_OK;
    napi_value jsValue = nullptr;

    if (!IsArrayForNapiValue(env, param, arraySize)) {
        return false;
    }

    deviceInfoVec.clear();
    for (uint32_t i = 0; i < arraySize; i++) {
        jsValue = nullptr;
        DeviceInfo deviceInfo;
        if (napi_get_element(env, param, i, &jsValue) != napi_ok) {
            return false;
        }

        if (!UnwrapDeviceInfo(env, jsValue, deviceInfo)) {
            return false;
        }

        deviceInfoVec.push_back(deviceInfo);
    }
    return true;
}

napi_value WrapArrayWantToJS(napi_env env, const std::vector<OHOS::AAFwk::Want> &wantVec)
{
    napi_value jsArray = nullptr;
    napi_value jsValue = nullptr;
    uint32_t index = ERR_OK;
    NAPI_CALL(env, napi_create_array(env, &jsArray));

    for (uint32_t i = 0; i < wantVec.size(); i++) {
        jsValue = OHOS::AppExecFwk::WrapWant(env, wantVec[i]);
        if (napi_set_element(env, jsArray, index, jsValue) == napi_ok) {
            index++;
        }
    }
    return jsArray;
}

bool UnwrapArrayWantFromJS(napi_env env, napi_value param, std::vector<OHOS::AAFwk::Want> &wantVec)
{
    uint32_t arraySize = ERR_OK;
    napi_value jsValue = nullptr;

    if (!IsArrayForNapiValue(env, param, arraySize)) {
        return false;
    }

    wantVec.clear();
    for (uint32_t i = 0; i < arraySize; i++) {
        jsValue = nullptr;
        OHOS::AAFwk::Want want;
        if (napi_get_element(env, param, i, &jsValue) != napi_ok) {
            return false;
        }

        if (!OHOS::AppExecFwk::UnwrapWant(env, jsValue, want)) {
            return false;
        }

        wantVec.push_back(want);
    }
    return true;
}
} // namespace FileAccessFwk
} // namespace OHOS
