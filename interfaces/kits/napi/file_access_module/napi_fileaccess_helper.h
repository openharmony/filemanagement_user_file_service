/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_KITS_NAPI_FILE_ACCESS_EXT_ABILITY_NAPI_FILEACCESS_HELPER_H
#define INTERFACES_KITS_NAPI_FILE_ACCESS_EXT_ABILITY_NAPI_FILEACCESS_HELPER_H

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "napi_common.h"

namespace OHOS {
namespace FileAccessFwk {
    napi_value FileAccessHelperInit(napi_env env, napi_value exports);
    napi_value NAPI_OpenFile(napi_env env, napi_callback_info info);
    napi_value NAPI_CreateFile(napi_env env, napi_callback_info info);
    napi_value NAPI_Mkdir(napi_env env, napi_callback_info info);
    napi_value NAPI_Delete(napi_env env, napi_callback_info info);
    napi_value NAPI_Move(napi_env env, napi_callback_info info);
    napi_value NAPI_Rename(napi_env env, napi_callback_info info);
    napi_value NAPI_ListFile(napi_env env, napi_callback_info info);
    napi_value NAPI_GetRoots(napi_env env, napi_callback_info info);
}
} // namespace FileAccessFwk
#endif // INTERFACES_KITS_NAPI_FILE_ACCESS_EXT_ABILITY_NAPI_FILEACCESS_HELPER_H