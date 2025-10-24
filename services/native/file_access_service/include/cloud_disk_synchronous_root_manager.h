/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_CLOUD_DISK_SYNCHRONOUS_ROOT_MANAGER_H
#define OHOS_CLOUD_DISK_SYNCHRONOUS_ROOT_MANAGER_H

#include <map>
#include <string>
#include <memory>
#include <mutex>
#include <vector>

#include "hilog_wrapper.h"
#include "ufs_rdb_adapter.h"
#include "cloud_disk_comm.h"

namespace OHOS {
namespace FileAccessFwk {

    using State = FileManagement::State;
    using FileManagement::State;
    using SyncFolder = FileManagement::SyncFolder;
    using FileManagement::SyncFolder;
    using SyncFolderExt = FileManagement::SyncFolderExt;
    using FileManagement::SyncFolderExt;

using namespace OHOS::NativeRdb;

class SynchronousRootManager {
public:
    ~SynchronousRootManager() = default;
    static SynchronousRootManager& GetInstance()
    {
        static SynchronousRootManager instance;
        return instance;
    }
    SynchronousRootManager(const SynchronousRootManager&) = delete;
    SynchronousRootManager& operator=(const SynchronousRootManager&) = delete;
    bool Init();
    bool UnInit();
    bool PutSynchronousRoot(const SyncFolder& rootInfo,
        const std::string& bundleName, int32_t userId, int32_t index);
    bool DeleteSynchronousRoot(const std::string& path, int32_t userId);
    bool DeleteAllSynRootsByUserAndBundle(
        const std::string& bundleName, int32_t userId, int32_t index);
    bool UpdateSynchronousRootState(const std::string& rootUri,
        const std::string& bundleName, int32_t userId, int32_t index, const State& newState);
    bool GetSynchronousRootByPathAndUserId(SyncFolder& syncFolder, int32_t userId);
    bool GetRootInfosByUserAndBundle(const std::string& bundleName, int32_t index,
        int32_t userId, std::vector<SyncFolder>& rootInfos);

    bool GetAllSyncFolderInfosByUserId(int32_t userId, std::vector<SyncFolderExt>& syncFolderExts);
    int32_t GetRootNumByUserIdAndBundleName(const std::string& bundleName,
        int32_t index, int32_t userId);
    bool UpdateDisplayName(const std::string& rootUri, const std::string& bundleName,
        int32_t userId, int32_t index, const std::string& displayName);
    int32_t GetRootCount();
    int32_t CheckExistParentOrChildRoot(const std::string& path,
        const std::string& bundleName, int userId);
    std::string NormalizeUri(const std::string& uri);
    bool IsParentOrChildPath(const std::string& path1, const std::string& path2);
    bool IsPathAllowed(const std::string& path);
    void PutTimeStamp(ValuesBucket& values);
    int32_t IsSyncFolderValid(const SyncFolder& folder, const std::string& bundleName,
    int32_t index, int32_t userId);
    bool validatePath(const std::string& path);
    bool validateDisplayName(const std::string& displayName);

private:
    SynchronousRootManager()
    {
        rdbStore_ = std::make_shared<RdbAdapter>();
        Init();
    }
    bool CreateTable();
    int32_t CreateUniqueIndex();
private:
    std::shared_ptr<IRdbAdapter> rdbStore_;
    std::mutex aclMutex_;
    std::mutex rdbMutex_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // OHOS_CLOUD_DISK_SYNCHRONUS_ROOT_MANAGER_H
