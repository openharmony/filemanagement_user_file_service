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

#include "file_access_service.h"
#include "notify_work_service.h"

using namespace std;
namespace OHOS {
namespace FileAccessFwk {

void FileAccessService::IncreaseCnt(const std::string &funcName)
{
    std::lock_guard<std::mutex> lock(calledMutex_);
    ++calledCount_;
    HILOG_INFO("Func name: %{public}s, count: %{public}d", funcName.c_str(), calledCount_);
}

void FileAccessService::DecreaseCnt(const std::string &funcName)
{
    std::lock_guard<std::mutex> lock(calledMutex_);
    if (calledCount_ > 0) {
        --calledCount_;
    } else {
        HILOG_ERROR("Invalid calledCount.");
    }
    HILOG_INFO("Func name: %{public}s, count: %{public}d", funcName.c_str(), calledCount_);
}

bool FileAccessService::IsCalledCountValid()
{
    std::lock_guard<std::mutex> lock(calledMutex_);
    return calledCount_ > 0;
}

int32_t FileAccessService::Register(const SyncFolder &syncFolder)
{
    HILOG_INFO("FileAccessService::Register begin");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    IncreaseCnt(__func__);
    SyncFolderExt syncFolderExt;
    syncFolderExt.path_ = syncFolder.path_;
    if (!NotifyWorkService::GetInstance().NotifySyncFolderEvent(syncFolderExt,
        NotifyWorkService::EventType::REGISTER)) {
        HILOG_ERROR("FileAccessService::Register NotifySyncFolderEvent failed");
    }
    DecreaseCnt(__func__);
    return ERR_OK;
#else
    return E_SYSTEM_RESTRICTED;
#endif
}

int32_t FileAccessService::Unregister(const std::string &path)
{
    HILOG_INFO("FileAccessService::Unregister begin");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    IncreaseCnt(__func__);
    SyncFolderExt syncFolderExt;
    syncFolderExt.path_ = path;
    if (!NotifyWorkService::GetInstance().NotifySyncFolderEvent(syncFolderExt,
        NotifyWorkService::EventType::UNREGISTER)) {
        HILOG_ERROR("FileAccessService::Unregister NotifySyncFolderEvent failed");
    }
    DecreaseCnt(__func__);
    return ERR_OK;
#else
    return E_SYSTEM_RESTRICTED;
#endif
}

int32_t FileAccessService::Active(const std::string &path)
{
    HILOG_INFO("FileAccessService::Active begin");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    IncreaseCnt(__func__);
    SyncFolderExt syncFolderExt;
    syncFolderExt.path_ = path;
    if (!NotifyWorkService::GetInstance().NotifySyncFolderEvent(syncFolderExt,
        NotifyWorkService::EventType::ACTIVE)) {
        HILOG_ERROR("FileAccessService::Active NotifySyncFolderEvent failed");
    }
    DecreaseCnt(__func__);
    return ERR_OK;
#else
    return E_SYSTEM_RESTRICTED;
#endif
}

int32_t FileAccessService::Deactive(const std::string &path)
{
    HILOG_INFO("FileAccessService::Deactive begin");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    IncreaseCnt(__func__);
    SyncFolderExt syncFolderExt;
    syncFolderExt.path_ = path;
    if (!NotifyWorkService::GetInstance().NotifySyncFolderEvent(syncFolderExt,
        NotifyWorkService::EventType::INACTIVE)) {
        HILOG_ERROR("FileAccessService::Deactive NotifySyncFolderEvent failed");
    }
    DecreaseCnt(__func__);
    return ERR_OK;
#else
    return E_SYSTEM_RESTRICTED;
#endif
}

int32_t FileAccessService::GetSyncFolders(std::vector<SyncFolder> &syncFolders)
{
    HILOG_INFO("FileAccessService::GetSyncFolders begin");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    IncreaseCnt(__func__);
    DecreaseCnt(__func__);
    return ERR_OK;
#else
    return E_SYSTEM_RESTRICTED;
#endif
}

int32_t FileAccessService::GetAllSyncFolders(std::vector<SyncFolderExt> &syncFolderExts)
{
    HILOG_INFO("FileAccessService::GetAllSyncFolders begin");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    IncreaseCnt(__func__);
    DecreaseCnt(__func__);
    return ERR_OK;
#else
    return E_SYSTEM_RESTRICTED;
#endif
}

int32_t FileAccessService::UpdateDisplayName(const std::string &path, const std::string &displayName)
{
    HILOG_INFO("FileAccessService::UpdateDisplayName begin");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    IncreaseCnt(__func__);
    SyncFolderExt syncFolderExt;
    syncFolderExt.path_ = path;
    syncFolderExt.displayName_ = displayName;
    if (!NotifyWorkService::GetInstance().NotifySyncFolderEvent(syncFolderExt,
        NotifyWorkService::EventType::UPDATE)) {
        HILOG_ERROR("FileAccessService::UpdateDisplayName NotifySyncFolderEvent failed");
    }
    DecreaseCnt(__func__);
    return ERR_OK;
#else
    return E_SYSTEM_RESTRICTED;
#endif
}

int32_t FileAccessService::UnregisterForSa(const string &path)
{
    HILOG_INFO("FileAccessService::UnregisterForSa begin");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    IncreaseCnt(__func__);
    SyncFolderExt syncFolderExt;
    syncFolderExt.path_ = path;
    if (!NotifyWorkService::GetInstance().NotifySyncFolderEvent(syncFolderExt,
        NotifyWorkService::EventType::UNREGISTER)) {
        HILOG_ERROR("FileAccessService::UnregisterForSa NotifySyncFolderEvent failed");
    }
    DecreaseCnt(__func__);
    return ERR_OK;
#else
    return E_SYSTEM_RESTRICTED;
#endif
}

int32_t FileAccessService::GetAllSyncFoldersForSa(std::vector<SyncFolderExt> &syncFolderExt)
{
    HILOG_INFO("FileAccessService::GetAllSyncFoldersForSa begin");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    IncreaseCnt(__func__);
    DecreaseCnt(__func__);
    return ERR_OK;
#else
    return E_SYSTEM_RESTRICTED;
#endif
}
} // namespace FileAccessFwk
} // namespace OHOS
