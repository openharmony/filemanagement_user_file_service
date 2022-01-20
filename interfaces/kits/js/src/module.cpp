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

#include <memory>
#include <vector>

#include "file_manager_napi.h"
#include "log.h"

using namespace std;

namespace OHOS {
namespace FileManagerService {
using namespace DistributedFS;
static napi_value Export(napi_env env, napi_value exports)
{
    std::vector<unique_ptr<NExporter>> products;
    products.emplace_back(make_unique<FileManagerNapi>(env, exports));

    for (auto &&product : products) {
        if (!product->Export()) {
            ERR_LOG("INNER BUG. Failed to export class %{public}s for module filemanager",
                product->GetClassName().c_str());
            return nullptr;
        }
    }
    return exports;
}

NAPI_MODULE(filemanager, Export)
} // namespace FileManagerService
} // namespace OHOS