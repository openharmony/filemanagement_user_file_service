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
#ifndef CLOUD_DISK_SYNC_FOLDER_MANAGER_IMPL_H
#define CLOUD_DISK_SYNC_FOLDER_MANAGER_IMPL_H

#include "cloud_disk_sync_folder_manager.h"

namespace OHOS {
namespace FileManagement {
class CloudDiskSyncFolderManagerImpl : public CloudDiskSyncFolderManager {
public:
    static CloudDiskSyncFolderManagerImpl &GetInstance();
    virtual int32_t Register(const SyncFolder rootInfo) override;
    virtual int32_t Unregister(const std::string path) override;
    virtual int32_t Active(const std::string path) override;
    virtual int32_t Deactive(const std::string path) override;
    virtual int32_t GetSyncFolders(std::vector<SyncFolder> &syncFolders) override;
    virtual int32_t UpdateDisplayName(const std::string path, const std::string displayName) override;
    // function for sa
    virtual int32_t UnregisterForSa(const std::string path) override;
    virtual int32_t GetAllSyncFoldersForSa(std::vector<SyncFolderExt> &syncFolderExts) override;
};
} // namespace FileManagement
} // namespace OHOS
#endif // CLOUD_DISK_SYNC_FOLDER_MANAGER_IMPL_H