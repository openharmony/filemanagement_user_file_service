/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_KITS_JS_PICKER_INCLUDE_NAPI_ERROR_H
#define INTERFACES_KITS_JS_PICKER_INCLUDE_NAPI_ERROR_H

#include <string>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "picker_napi_utils.h"

namespace OHOS {
namespace Picker {
#define EXPORT __attribute__ ((visibility ("default")))
struct NapiError {
    int32_t error = 0;
    std::string apiName;
    void SetApiName(const std::string &Name);
    void HandleError(napi_env env, napi_value &errorObj);
    EXPORT static void ThrowError(napi_env env, int32_t err, const std::string &errMsg = "");
    EXPORT static void ThrowError(napi_env env, int32_t err, const char *func, int32_t line,
        const std::string &errMsg = "");
};
} // namespace Picker
} // namespace OHOS
#endif // INTERFACES_KITS_JS_PICKER_INCLUDE_NAPI_ERROR_H