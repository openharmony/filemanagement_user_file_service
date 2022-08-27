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

#include "file_extension_info_napi.h"
#include "hilog_wrapper.h"
#include "napi_fileaccess_helper.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace FileAccessFwk {
EXTERN_C_START
/*
 * The module initialization.
 */
static napi_value Init(napi_env env, napi_value exports)
{
    FileAccessHelperInit(env, exports);
    InitDeviceFlag(env, exports);
    InitDocumentFlag(env, exports);
    InitNotifyType(env, exports);
    InitDeviceType(env, exports);
    InitFileInfo(env, exports);
    InitRootInfo(env, exports);
    return exports;
}
EXTERN_C_END

/*
 * The module definition.
 */
static napi_module _module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "data.fileAccess",
    .nm_priv = ((void *)0),
    .reserved = {0}
};

/*
 * The module registration.
 */
extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&_module);
}
} // namespace FileAccessFwk
} // namespace OHOS