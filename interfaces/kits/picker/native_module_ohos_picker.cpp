/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "picker_n_exporter.h"

extern const char _binary_picker_js_start[];
extern const char _binary_picker_js_end[];
extern const char _binary_picker_abc_start[];
extern const char _binary_picker_abc_end[];

namespace OHOS {
namespace FileAccessFwk {
using namespace Picker;
using namespace FileManagement::LibN;

EXTERN_C_START
static napi_value Export(napi_env env, napi_value exports)
{
    std::vector<unique_ptr<NExporter>> products;
    products.emplace_back(make_unique<PickerNExporter>(env, exports));

    for (auto &&product : products) {
        std::string nExporterName = product->GetClassName();
        if (!product->Export()) {
            HILOG_ERROR("INNER BUG. Failed to export class %{public}s for module trash", nExporterName.c_str());
            return nullptr;
        } else {
            HILOG_INFO("Class %{public}s for module trash has been exported", nExporterName.c_str());
        }
    }
    return exports;
}
EXTERN_C_END

extern "C" __attribute__((visibility("default"))) void NAPI_file_picker_GetJSCode(const char** buf,
    int* bufLen)
{
    if (buf != nullptr) {
        *buf = _binary_picker_js_start;
    }

    if (bufLen != nullptr) {
        *bufLen = _binary_picker_js_end - _binary_picker_js_start;
    }
}

extern "C" __attribute__((visibility("default"))) void NAPI_file_picker_GetABCCode(const char** buf,
    int* bufLen)
{
    if (buf != nullptr) {
        *buf = _binary_picker_abc_start;
    }

    if (bufLen != nullptr) {
        *bufLen = _binary_picker_abc_end - _binary_picker_abc_start;
    }
}

/*
 * module define
 */
static napi_module g_module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Export,
    .nm_modname = "file.picker",
    .nm_priv = reinterpret_cast<void *>(0),
    .reserved = {0}
};

/*
 * module register
 */
extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&g_module);
}
} // namespace Media
} // namespace OHOS
