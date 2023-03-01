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

#ifndef NAPI_COMMON_FILE_ACCESS_H
#define NAPI_COMMON_FILE_ACCESS_H

#include <map>
#include <string>
#include <vector>

#include "file_access_extension_info.h"
#include "napi_common_data.h"
#include "napi_common_util.h"
#include "want.h"

namespace OHOS {
namespace FileAccessFwk {
napi_value WrapUint32ToJS(napi_env env, uint32_t value);
uint32_t UnwrapUint32FromJS(napi_env env, napi_value param, uint32_t defaultValue = 0);
bool UnwrapUint32FromJS2(napi_env env, napi_value param, uint32_t &value);
bool UnwrapUint32ByPropertyName(napi_env env, napi_value param, const char *propertyName, uint32_t &value);

napi_value WrapBigIntUint64ToJS(napi_env env, uint64_t value);
uint64_t UnwrapBigIntUint64FromJS(napi_env env, napi_value param, uint64_t defaultValue = 0);
bool UnwrapBigIntUint64FromJS2(napi_env env, napi_value param, uint64_t &defaultValue);
bool UnwrapBigIntUint64ByPropertyName(napi_env env, napi_value param, const char *propertyName, uint64_t &value);

napi_value WrapArrayWantToJS(napi_env env, const std::vector<OHOS::AAFwk::Want> &wantVec);
bool UnwrapArrayWantFromJS(napi_env env, napi_value param, std::vector<OHOS::AAFwk::Want> &wantVec);
} // namespace FileAccessFwk
} // namespace OHOS
#endif // NAPI_COMMON_FILE_ACCESS_H
