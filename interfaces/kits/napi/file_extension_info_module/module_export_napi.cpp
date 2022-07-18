/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "module_export_napi.h"

#include "file_extension_info_napi.h"
#include "js_native_api_types.h"
#include "napi/native_api.h"

namespace OHOS {
namespace FileAccessFwk {
napi_value FileExtensionInfoExport(napi_env env, napi_value exports)
{
    InitFlag(env, exports);
    InitFileInfo(env, exports);
    InitDeviceInfo(env, exports);

    return exports;
}

NAPI_MODULE(fileExtensionInfo, FileExtensionInfoExport)
} // namespace FileAccessFwk
} // namespace OHOS