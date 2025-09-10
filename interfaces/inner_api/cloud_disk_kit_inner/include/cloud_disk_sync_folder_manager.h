/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#ifndef CLOUD_DISK_SYNC_FOLDER_MANAGER_H
#define CLOUD_DISK_SYNC_FOLDER_MANAGER_H

#include "cloud_disk_comm.h"

namespace OHOS {
namespace FileManagement {
class CloudDiskSyncFolderManager {
public:
    CLOUD_DISK_EXPORT static CloudDiskSyncFolderManager &GetInstance();
    virtual int32_t Register(const SyncFolder rootInfo) = 0;
    virtual int32_t Unregister(const std::string path) = 0;
    virtual int32_t Active(const std::string path) = 0;
    virtual int32_t Deactive(const std::string path) = 0;
    virtual int32_t GetSyncFolders(std::vector<SyncFolder> &syncFolders) = 0;
    virtual int32_t UpdateDisplayName(const std::string path, const std::string displayName) = 0;
    // function for sa
    virtual int32_t UnregisterForSa(const std::string path) = 0;
    virtual int32_t GetAllSyncFoldersForSa(std::vector<SyncFolderExt> &syncFolderExts) = 0;
};
} // namespace FileManagement
} // namespace OHOS
#endif // CLOUD_DISK_SYNC_FOLDER_MANAGER_H