/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

napi_value WrapArrayWantToJS(napi_env env, const std::vector<OHOS::AAFwk::Want> &wantVec)
{
    napi_value jsArray = nullptr;
    napi_value jsValue = nullptr;
    uint32_t index = ERR_OK;
    NAPI_CALL(env, napi_create_array(env, &jsArray));
    for (auto want : wantVec) {
        jsValue = OHOS::AppExecFwk::WrapWant(env, want);
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

napi_value GetUndefinedValue(napi_env env)
{
    napi_value result{};
    napi_get_undefined(env, &result);
    return result;
}

napi_status GetStringValue(napi_env env, napi_value value, std::string &result)
{
    size_t tempSize = 0;
    if (napi_get_value_string_utf8(env, value, nullptr, 0, &tempSize) != napi_ok) {
        return napi_generic_failure;
    }
    result.reserve(tempSize);
    result.resize(tempSize);
    if (napi_get_value_string_utf8(env, value, result.data(), tempSize + 1, &tempSize) != napi_ok) {
        return napi_generic_failure;
    }
    return napi_ok;
}
} // namespace FileAccessFwk
} // namespace OHOS