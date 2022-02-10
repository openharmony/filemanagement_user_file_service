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

#include "storage_manager_inf.h"

#include "file_manager_service_def.h"
#include "file_manager_service_errno.h"
#include "log.h"

using namespace std;
namespace OHOS {
namespace FileManagerService {
#ifdef VOLUME_ENABLE
static bool GetMountPointFromPath(const string &path, string &mountPoint)
{
    size_t len = MOUNT_POINT_ROOT.size();
    std::string head = path.substr(0, len);
    std::string body = path.substr(len);
    if (head != MOUNT_POINT_ROOT || body.size() == 0) {
        ERR_LOG("invalid mountPoint %{public}s, head check fail", path.c_str());
        return false;
    }

    size_t index = body.find("/");
    if (index != std::string::npos) {
        mountPoint = MOUNT_POINT_ROOT + body.substr(0, index);
    } else {
        mountPoint = path;
    }
    return true;
}

int StorageManagerInf::Connect()
{
    DEBUG_LOG("StorageManagerConnect::Connect start");
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        ERR_LOG("StorageManagerConnect::Connect samgr == nullptr");
        return FAIL;
    }
    auto object = sam->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID);
    if (object == nullptr) {
        ERR_LOG("StorageManagerConnect::Connect object == nullptr");
        return FAIL;
    }
    storageManager_ = iface_cast<StorageManager::IStorageManager>(object);
    if (storageManager_ == nullptr) {
        ERR_LOG("StorageManagerConnect::Connect service == nullptr");
        return FAIL;
    }
    DEBUG_LOG("StorageManagerConnect::Connect end");
    return SUCCESS;
}

std::vector<StorageManager::VolumeExternal> StorageManagerInf::GetAllVolumes()
{
    vector<StorageManager::VolumeExternal> result = {};
    if (Connect() != SUCCESS) {
        ERR_LOG("GetTotalSizeOfVolume:Connect error");
        return result;
    }
    return storageManager_->GetAllVolumes();
}

bool StorageManagerInf::GetMountedVolumes(vector<string> &vecRootPath)
{
    bool succ = false;
    vector<StorageManager::VolumeExternal> result = GetAllVolumes();
    if (result.size() == 0) {
        ERR_LOG("empty volume result");
        return succ;
    }
    for (auto vol : result) {
        DEBUG_LOG("find vol.GetPath() %{public}d %{public}d", vol.GetState(), VolumeState::MOUNTED);
        if (vol.GetState() == VolumeState::MOUNTED) {
            DEBUG_LOG("mounted find vol.GetPath() %{public}d %{public}d", vol.GetState(), VolumeState::MOUNTED);
            vecRootPath.emplace_back(EXTERNAL_STORAGE_URI + vol.GetPath());
            succ = true;
        }
    }
    return succ;
}

bool StorageManagerInf::StoragePathValidCheck(const string &path)
{
    string mountPoint;
    if (!GetMountPointFromPath(path, mountPoint)) {
        ERR_LOG("uri path is invalid");
        return false;
    }
    vector<StorageManager::VolumeExternal> result = GetAllVolumes();
    if (result.size() == 0) {
        ERR_LOG("empty volume result");
        return false;
    }
    bool succ = false;
    for (auto vol : result) {
        if (vol.GetPath() ==  mountPoint && vol.GetState() == VolumeState::MOUNTED) {
            succ = true;
            break;
        }
    }
    return succ;
}
#endif
} // FileManagerService
} // OHOS
