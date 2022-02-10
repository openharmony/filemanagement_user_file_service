/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef STORAGE_MANAGER_INTERFACE_H
#define STORAGE_MANAGER_INTERFACE_H
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include "ipc/storage_manager_proxy.h"
#include "ipc/storage_manager.h"
#include "istorage_manager.h"
namespace OHOS {
namespace FileManagerService {
class StorageManagerInf {
public:
    StorageManagerInf() = default;
    ~StorageManagerInf() = default;
    static int Connect();
    static std::vector<StorageManager::VolumeExternal> GetAllVolumes();
    static bool GetMountedVolumes(std::vector<std::string> &vecRootPath);
    static bool StoragePathValidCheck(const std::string &path);
private:
    inline static sptr<StorageManager::IStorageManager> storageManager_;
};
} // FileManagerService
} // OHOS
#endif // STORAGE_MANAGER_INTERFACE_H