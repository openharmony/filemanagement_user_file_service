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

#include <memory>
#include <vector>

#include "filemgmt_libn.h"
#include "file_extension_info_napi.h"
#include "hilog_wrapper.h"
#include "napi_fileaccess_helper.h"
#include "napi_file_info_exporter.h"
#include "napi_file_iterator_exporter.h"
#include "napi_root_info_exporter.h"
#include "napi_root_iterator_exporter.h"

namespace OHOS {
namespace FileAccessFwk {
using namespace FileManagement::LibN;

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

    std::vector<std::unique_ptr<NExporter>> products;
    products.emplace_back(std::make_unique<NapiRootIteratorExporter>(env, exports));
    products.emplace_back(std::make_unique<NapiRootInfoExporter>(env, exports));
    products.emplace_back(std::make_unique<NapiFileIteratorExporter>(env, exports));
    products.emplace_back(std::make_unique<NapiFileInfoExporter>(env, exports));
    for (auto &&product : products) {
        if (!product->Export()) {
            HILOG_ERROR("INNER BUG. Failed to export class %{public}s", product->GetClassName().c_str());
            return nullptr;
        } else {
            HILOG_ERROR("Class %{public}s has been exported", product->GetClassName().c_str());
        }
    }

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