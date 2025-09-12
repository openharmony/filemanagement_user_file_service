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

#include "cloud_disk_sync_folder_manager_impl.h"
#include "file_access_service_client.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace FileManagement {
using namespace FileAccessFwk;
#ifdef SUPPORT_CLOUD_DISK_MANAGER
constexpr int MAX_RETRY_TIMES = 3;
std::set<int32_t> cloudDiskJSErrCode = {
    FileAccessFwk::ERR_OK,                          // Success
    E_PERMISSION,                                   // Permission verification failed
    E_PERMISSION_SYS,                               // is not system app
    E_INVALID_PARAM,                                // Invalid parameter
    E_SYNC_FOLDER_PATH_UNAUTHORIZED,                // Sync folder unauthorized
    E_IPC_FAILED,                                   // IPC error
    E_SYNC_FOLDER_LIMIT_EXCEEDED,                   // Sync folder limit exceeded
    E_SYNC_FOLDER_CONFLICT_SELF,                    // Conflict with own app's Sync folder
    E_SYNC_FOLDER_CONFLICT_OTHER,                   // Conflict with other app's Sync folder
    E_REGISTER_SYNC_FOLDER_FAILED,                  // Failed to register Sync folder
    E_SYNC_FOLDER_NOT_REGISTERED,                   // Sync folder not registered
    E_REMOVE_SYNC_FOLDER_FAILED,                    // Failed to remove Sync folder
    E_SYNC_FOLDER_NOT_EXIST,                        // Sync folder not exist
    E_LISTENER_NOT_REGISTERED,                      // Listener not registered
    E_LISTENER_ALREADY_REGISTERED,                  // Listener already registered
    E_INVALID_CHANGE_SEQUENCE,                      // Invalid change sequence
    E_TRY_AGAIN,                                    // Try again
    E_INTERNAL_ERROR,                               // Internal error
    E_SYSTEM_RESTRICTED,                            // System restricted
};

int32_t ConvertErrCode(int32_t errCode)
{
    if (cloudDiskJSErrCode.find(errCode) == cloudDiskJSErrCode.end()) {
        return E_IPC_FAILED;
    }
    return errCode;
}
#endif

CloudDiskSyncFolderManagerImpl &CloudDiskSyncFolderManagerImpl::GetInstance()
{
    static CloudDiskSyncFolderManagerImpl instance;
    return instance;
}

int CloudDiskSyncFolderManagerImpl::Register(const SyncFolder syncFolder)
{
    HILOG_INFO("CloudDiskSyncFolderManagerImpl::Register in");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    auto proxy = FileAccessServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOG_ERROR("Register get SA failed");
        return E_INTERNAL_ERROR;
    }
    int ret = proxy->Register(syncFolder);
    int retryTimes = 1;
    while (ret == E_SERVICE_DIED && retryTimes <= MAX_RETRY_TIMES) {
        proxy = FileAccessServiceClient::GetInstance();
        if (proxy == nullptr) {
            HILOG_ERROR("Register get SA failed");
        } else {
            ret = proxy->Register(syncFolder);
        }
        ++retryTimes;
    }
    return ConvertErrCode(ret);
#endif
    return E_SYSTEM_RESTRICTED;
}

int CloudDiskSyncFolderManagerImpl::Unregister(const std::string path)
{
    HILOG_INFO("CloudDiskSyncFolderManagerImpl::Unregister in");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    if (path.empty()) {
        HILOG_ERROR("Unregister path is empty");
        return E_INVALID_PARAM;
    }
    auto proxy = FileAccessServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOG_ERROR("Unregister get SA failed");
        return E_INTERNAL_ERROR;
    }
    int ret = proxy->Unregister(path);
    int retryTimes = 1;
    while (ret == E_SERVICE_DIED && retryTimes <= MAX_RETRY_TIMES) {
        proxy = FileAccessServiceClient::GetInstance();
        if (proxy == nullptr) {
            HILOG_ERROR("Register get SA failed");
        } else {
            ret = proxy->Unregister(path);
        }
        ++retryTimes;
    }
    return ConvertErrCode(ret);
#endif
    return E_SYSTEM_RESTRICTED;
}

int CloudDiskSyncFolderManagerImpl::Active(const std::string path)
{
    HILOG_INFO("CloudDiskSyncFolderManagerImpl::Active in");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    if (path.empty()) {
        HILOG_ERROR("Unregister path is empty");
        return E_INVALID_PARAM;
    }
    auto proxy = FileAccessServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOG_ERROR("Active get SA failed");
        return E_INTERNAL_ERROR;
    }
    int ret = proxy->Active(path);
    int retryTimes = 1;
    while (ret == E_SERVICE_DIED && retryTimes <= MAX_RETRY_TIMES) {
        proxy = FileAccessServiceClient::GetInstance();
        if (proxy == nullptr) {
            HILOG_ERROR("Active get SA failed");
        } else {
            ret = proxy->Active(path);
        }
        ++retryTimes;
    }
    return ConvertErrCode(ret);
#endif
    return E_SYSTEM_RESTRICTED;
}

int CloudDiskSyncFolderManagerImpl::Deactive(const std::string path)
{
    HILOG_INFO("CloudDiskSyncFolderManagerImpl::Deactive in");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    if (path.empty()) {
        HILOG_ERROR("Deactive path is empty");
        return E_INVALID_PARAM;
    }
    auto proxy = FileAccessServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOG_ERROR("Deactive get SA failed");
        return E_INTERNAL_ERROR;
    }
    int ret = proxy->Deactive(path);
    int retryTimes = 1;
    while (ret == E_SERVICE_DIED && retryTimes <= MAX_RETRY_TIMES) {
        proxy = FileAccessServiceClient::GetInstance();
        if (proxy == nullptr) {
            HILOG_ERROR("Deactive get SA failed");
        } else {
            ret = proxy->Deactive(path);
        }
        ++retryTimes;
    }
    return ConvertErrCode(ret);
#endif
    return E_SYSTEM_RESTRICTED;
}

int CloudDiskSyncFolderManagerImpl::GetSyncFolders(std::vector<SyncFolder> &syncFolders)
{
    HILOG_INFO("CloudDiskSyncFolderManagerImpl::GetSyncFolders in");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    auto proxy = FileAccessServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOG_ERROR("GetSyncFolders get SA failed");
        return E_INTERNAL_ERROR;
    }
    int ret = proxy->GetSyncFolders(syncFolders);
    int retryTimes = 1;
    while (ret == E_SERVICE_DIED && retryTimes <= MAX_RETRY_TIMES) {
        proxy = FileAccessServiceClient::GetInstance();
        if (proxy == nullptr) {
            HILOG_ERROR("GetSyncFolders get SA failed");
        } else {
            ret = proxy->GetSyncFolders(syncFolders);
        }
        ++retryTimes;
    }
    return ConvertErrCode(ret);
#endif
    return E_SYSTEM_RESTRICTED;
}

int CloudDiskSyncFolderManagerImpl::UpdateDisplayName(const std::string path, const std::string displayName)
{
    HILOG_INFO("CloudDiskSyncFolderManagerImpl::UpdateDisplayName in");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    if (path.empty() || displayName.empty()) {
        HILOG_ERROR("UpdateDisplayName path or displayName is empty");
        return E_INVALID_PARAM;
    }
    auto proxy = FileAccessServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOG_ERROR("UpdateDisplayName get SA failed");
        return E_INTERNAL_ERROR;
    }
    int ret = proxy->UpdateDisplayName(path, displayName);
    int retryTimes = 1;
    while (ret == E_SERVICE_DIED && retryTimes <= MAX_RETRY_TIMES) {
        proxy = FileAccessServiceClient::GetInstance();
        if (proxy == nullptr) {
            HILOG_ERROR("UpdateDisplayName get SA failed");
        } else {
            ret = proxy->UpdateDisplayName(path, displayName);
        }
        ++retryTimes;
    }
    return ConvertErrCode(ret);
#endif
    return E_SYSTEM_RESTRICTED;
}

int32_t CloudDiskSyncFolderManagerImpl::GetAllSyncFoldersForSa(std::vector<SyncFolderExt> &syncFolderExts)
{
    HILOG_INFO("CloudDiskSyncFolderManagerImpl::GetAllSyncFoldersForSa in");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    auto proxy = FileAccessServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOG_ERROR("GetAllRootsForSa get SA failed");
        return E_INTERNAL_ERROR;
    }
    int ret = proxy->GetAllSyncFoldersForSa(syncFolderExts);
    int retryTimes = 1;
    while (ret == E_SERVICE_DIED && retryTimes <= MAX_RETRY_TIMES) {
        proxy = FileAccessServiceClient::GetInstance();
        if (proxy == nullptr) {
            HILOG_ERROR("GetAllRootsForSa get SA failed");
        } else {
            ret = proxy->GetAllSyncFoldersForSa(syncFolderExts);
        }
        ++retryTimes;
    }
    return ConvertErrCode(ret);
#endif
    return E_SYSTEM_RESTRICTED;
}

int32_t CloudDiskSyncFolderManagerImpl::UnregisterForSa(const std::string path)
{
    HILOG_INFO("CloudDiskSyncFolderManagerImpl::UnregisterForSa in");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    if (path.empty()) {
        HILOG_ERROR("Parameter error, path: %{public}s",  path.c_str());
        return E_INVALID_PARAM;
    }
    auto proxy = FileAccessServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOG_ERROR("UnregisterForSa get SA failed");
        return E_INTERNAL_ERROR;
    }
    int ret = proxy->UnregisterForSa(path);
    int retryTimes = 1;
    while (ret == E_SERVICE_DIED && retryTimes <= MAX_RETRY_TIMES) {
        proxy = FileAccessServiceClient::GetInstance();
        if (proxy == nullptr) {
            HILOG_ERROR("UnregisterForSa get SA failed");
        } else {
            ret = proxy->UnregisterForSa(path);
        }
        ++retryTimes;
    }
    return ConvertErrCode(ret);
#endif
    return E_SYSTEM_RESTRICTED;
}
} // namespace FileManagement
} // namespace OHOS