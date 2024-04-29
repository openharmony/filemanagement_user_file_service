/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_KITS_JS_PICKER_INCLUDE_PICKER_NAPI_UTILS_H
#define INTERFACES_KITS_JS_PICKER_INCLUDE_PICKER_NAPI_UTILS_H

#include <memory>
#include <vector>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Picker {
/* Constants for array index */
const int32_t PARAM0 = 0;
const int32_t PARAM1 = 1;
const int32_t PARAM2 = 2;
const int32_t PARAM3 = 3;
const int32_t PARAM4 = 4;

/* Constants for array size */
const int32_t ARGS_ZERO = 0;
const int32_t ARGS_ONE = 1;
const int32_t ARGS_TWO = 2;
const int32_t ARGS_THREE = 3;
const int32_t ARGS_FOUR = 4;
const int32_t ARGS_FIVE = 5;
const int32_t ARG_BUF_SIZE = 384; // 256 for display name and 128 for relative path
constexpr uint32_t NAPI_INIT_REF_COUNT = 1;

constexpr size_t NAPI_ARGC_MAX = 5;

// Error codes
const int32_t ERR_DEFAULT = 0;
const int32_t ERR_MEM_ALLOCATION = 2;
const int32_t ERR_INVALID_OUTPUT = 3;

struct JSAsyncContextOutput {
    napi_value error;
    napi_value data;
    bool status;
};

struct NapiClassInfo {
    std::string name;
    napi_ref *ref;
    napi_value (*constructor)(napi_env, napi_callback_info);
    std::vector<napi_property_descriptor> props;
};

/* Util class used by napi asynchronous methods for making call to js callback function */
class PickerNapiUtils {
public:
    static void CreateNapiErrorObject(napi_env env, napi_value &errorObj, const int32_t errCode,
        const std::string errMsg);
    static void InvokeJSAsyncMethod(napi_env env, napi_deferred deferred, napi_ref callbackRef, napi_async_work work,
        const JSAsyncContextOutput &asyncContext);
    template <class AsyncContext>
    static napi_value NapiCreateAsyncWork(napi_env env, std::unique_ptr<AsyncContext> &asyncContext,
        const std::string &resourceName,  void (*execute)(napi_env, void *),
        void (*complete)(napi_env, napi_status, void *));
};

} // namespace Picker
} // namespace OHOS
#endif // INTERFACES_KITS_JS_PICKER_INCLUDE_PICKER_NAPI_UTILS_H
