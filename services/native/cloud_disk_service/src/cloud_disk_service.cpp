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

#include <sys/stat.h>
#include "bundle_observer.h"
#ifdef SUPPORT_CLOUD_DISK_MANAGER
#include "cloud_disk_service_manager.h"
#endif
#include "notify_work_service.h"
#include "parameters.h"
#include "file_access_framework_errno.h"
#include "os_account_manager.h"
#include "system_ability_definition.h"
#include "iservice_registry.h"

using namespace std;
namespace OHOS {
namespace FileAccessFwk {
const std::string cloud_disk_permissionName = "ohos.permission.ACCESS_CLOUD_DISK_INFO";
constexpr int32_t NO_ERROR = 0;
constexpr int32_t INVALID_USER_ID = 10001;

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
int32_t FileAccessService::IsDirectoryExists(const std::string &physicalPath)
{
    struct stat buf {};
    int ret = stat(physicalPath.c_str(), &buf);
    if (ret == -1) {
        HILOG_ERROR("stat failed, errno is %{public}d", errno);
        return E_INVALID_PARAM;
    }
    if ((buf.st_mode & S_IFMT) != S_IFDIR) {
        HILOG_ERROR("path is not Dir");
        return E_INVALID_PARAM;
    };
    return ERR_OK;
}

int32_t FileAccessService::GetCurrentUserId()
{
    std::vector<int32_t> userIds{};
    auto ret = AccountSA::OsAccountManager::QueryActiveOsAccountIds(userIds);
    if (ret != NO_ERROR || userIds.empty()) {
        HILOG_ERROR("query active os account id failed, ret = %{public}d", ret);
        return INVALID_USER_ID;
    }
    HILOG_INFO("FileAccessService::GetCurrentUserId end, userId : %{public}d.", userIds[0]);
    return userIds[0];
}

void FileAccessService::SetWorkStatus(bool turnOn)
{
    std::string work_status = system::GetParameter(SYNCFOLDER_WORK_STATUS_KEY, "");
    HILOG_INFO("Param: %{private}s value is: %{public}s",
        SYNCFOLDER_WORK_STATUS_KEY.c_str(), work_status.c_str());
    
    std::string expectedCurrentStatus = turnOn ? SYNCFOLDER_WORK_STATUS_OFF : SYNCFOLDER_WORK_STATUS_ON;
    std::string targetStatus = turnOn ? SYNCFOLDER_WORK_STATUS_ON : SYNCFOLDER_WORK_STATUS_OFF;
    
    if (work_status.compare(expectedCurrentStatus) == 0) {
        bool isSetSucc = system::SetParameter(SYNCFOLDER_WORK_STATUS_KEY, targetStatus);
        HILOG_INFO("SetParameter %{private}s to %{public}s, result %{public}d.",
            SYNCFOLDER_WORK_STATUS_KEY.c_str(), targetStatus.c_str(), isSetSucc);
    }
}

int32_t FileAccessService::IsSyncFolderInTable(const std::string& path,
    std::vector<SyncFolderExt>& syncFolderExts, int userId)
{
    auto& rootManager = SynchronousRootManager::GetInstance();
    if (!rootManager.GetAllRootInfosByUserId(userId, syncFolderExts)) {
        HILOG_ERROR("Get RootInfos By User failed.");
        return E_SYNC_FOLDER_NOT_REGISTERED;
    }
    auto it = std::find_if(syncFolderExts.begin(), syncFolderExts.end(),
        [&path](const SyncFolderExt& info) {
            return info.path_ == path;
        });
    if (it == syncFolderExts.end()) {
        HILOG_ERROR("Root not found for path: %{private}s", path.c_str());
        return E_SYNC_FOLDER_NOT_REGISTERED;
    }
    return ERR_OK;
}

int32_t FileAccessService::ValidateSyncFolder(const SyncFolder &syncFolder,
    const std::string& bundleName, int index, int32_t userId)
{
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    auto& rootManager = SynchronousRootManager::GetInstance();
    auto ret = rootManager.IsSyncFolderValid(syncFolder, bundleName, index, userId);
    if (ret != ERR_OK) {
        HILOG_ERROR("SyncFolder invalid");
        return ret;
    }

    std::string physicalPath;
    if ((!UfsAccessTokenHelper::PathToPhysicalPath(syncFolder.path_, std::to_string(userId), physicalPath))
        || (IsDirectoryExists(physicalPath) != ERR_OK)) {
        HILOG_ERROR("Get PhysicalPath failed");
        return E_SYNC_FOLDER_PATH_NOT_EXIST;
    }

    bool isPermission = false;
    UfsAccessTokenHelper::CheckUriPersistentPermission(syncFolder.path_, isPermission);
    if (!isPermission) {
        HILOG_ERROR("is not Persistent");
        return E_SYNC_FOLDER_PATH_UNAUTHORIZED;
    }
    return ERR_OK;
#else
    return E_NOT_SUPPORT;
#endif
}

int32_t FileAccessService::Register(const SyncFolder &syncFolder)
{
    std::lock_guard<std::mutex> lock(syncFolderMtx_);
    HILOG_INFO("FileAccessService::Register begin");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    IncreaseCnt(__func__);
    int index;
    std::string bundleName;
    if (UfsAccessTokenHelper::GetCallerBundleNameAndIndex(bundleName, index) != ERR_OK) {
        HILOG_ERROR("Get caller bundleName and index failed.");
        DecreaseCnt(__func__);
        return E_TRY_AGAIN;
    }
    int32_t userId = UfsAccessTokenHelper::GetUserId();
    auto ret = ValidateSyncFolder(syncFolder, bundleName, index, userId);
    if (ret != ERR_OK) {
        HILOG_ERROR("SyncFolder validation failed");
        DecreaseCnt(__func__);
        return ret;
    }
    auto& rootManager = SynchronousRootManager::GetInstance();
    if (!rootManager.PutSynchronousRoot(syncFolder, bundleName, userId, index)) {
        HILOG_ERROR("Put rootinfo fail");
        DecreaseCnt(__func__);
        return E_REGISTER_SYNC_FOLDER_FAILED;
    }
    ret = FileManagement::CloudDiskService::CloudDiskServiceManager::GetInstance().
        RegisterSyncFolder(userId, bundleName, syncFolder.path_);
    if (ret != ERR_OK) {
        rootManager.DeleteSynchronousRoot(syncFolder.path_, userId);
        HILOG_ERROR("RegisterSyncFolder failed");
        DecreaseCnt(__func__);
        return ret;
    }
    SetWorkStatus(true);
    HILOG_INFO("FileAccessService::Register success");
    SyncFolderExt syncFolderExt;
    syncFolderExt.path_ = syncFolder.path_;
    syncFolderExt.bundleName_ = bundleName;
    syncFolderExt.displayName_ = syncFolder.displayName_;
    syncFolderExt.displayNameResId_ = syncFolder.displayNameResId_;
    if (!NotifyWorkService::GetInstance().NotifySyncFolderEvent(syncFolderExt,
        NotifyWorkService::EventType::REGISTER)) {
        HILOG_ERROR("FileAccessService::Register NotifySyncFolderEvent failed");
    }
    DecreaseCnt(__func__);
    return ERR_OK;
#else
    return E_NOT_SUPPORT;
#endif
}

int32_t FileAccessService::Unregister(const std::string &path)
{
    std::lock_guard<std::mutex> lock(syncFolderMtx_);
    HILOG_INFO("FileAccessService::Unregister begin");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    IncreaseCnt(__func__);
    int index;
    std::string bundleName;
    if (UfsAccessTokenHelper::GetCallerBundleNameAndIndex(bundleName, index) != ERR_OK) {
        HILOG_ERROR("Get caller bundleName and index failed.");
        DecreaseCnt(__func__);
        return E_INVALID_PARAM;
    }
    int32_t userId = UfsAccessTokenHelper::GetUserId();
    std::vector<SyncFolderExt> syncFolderExts;
    auto ret = IsSyncFolderInTable(path, syncFolderExts, userId);
    if (ret != ERR_OK) {
        HILOG_ERROR("SyncFolder not in table,Unregister failed");
        DecreaseCnt(__func__);
        return ERR_OK;
    }
    SyncFolderExt syncFolderExt;
    for (auto &it : syncFolderExts) {
        if (it.path_ == path) {
            syncFolderExt.path_ = it.path_;
            syncFolderExt.bundleName_ = it.bundleName_;
            syncFolderExt.state_ = it.state_;
            syncFolderExt.displayName_ = it.displayName_;
            syncFolderExt.displayNameResId_ = it.displayNameResId_;
            break;
        }
    }
    if (syncFolderExt.bundleName_ != bundleName) {
        HILOG_ERROR("bundleName Not Match");
        DecreaseCnt(__func__);
        return E_INVALID_PARAM;
    }
    ret = DoUnregister(path, userId, bundleName, syncFolderExt);
    if (ret != ERR_OK) {
        DecreaseCnt(__func__);
        return ret;
    }
    DecreaseCnt(__func__);
    return ERR_OK;
#else
    return E_NOT_SUPPORT;
#endif
}

int32_t FileAccessService::DoUnregister(const std::string &path, int userId, const std::string &bundleName,
    const SyncFolderExt& syncFolderExt)
{
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    auto ret = FileManagement::CloudDiskService::CloudDiskServiceManager::GetInstance().
        UnregisterSyncFolder(userId, bundleName, path);
    if (ret != ERR_OK) {
        HILOG_ERROR("UnregisterSyncFolder failed");
        return ret;
    }
    auto& rootManager = SynchronousRootManager::GetInstance();
    if (!rootManager.DeleteSynchronousRoot(path, userId)) {
        HILOG_ERROR("Delete root failed for path: %{private}s", path.c_str());
        return E_REMOVE_SYNC_FOLDER_FAILED;
    }
    if (rootManager.GetRootCount() == 0) {
        SetWorkStatus(false);
    }
    if (!NotifyWorkService::GetInstance().NotifySyncFolderEvent(syncFolderExt,
        NotifyWorkService::EventType::UNREGISTER)) {
        HILOG_ERROR("FileAccessService::Unregister otifySyncFolderEvent failed");
    }
    return ret;
#else
    return E_NOT_SUPPORT;
#endif
}

int32_t FileAccessService::UnregisterAllByBundle
    (const std::string& bundleName, const int32_t userId, const int32_t index)
{
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    HILOG_INFO("FileAccessService::UnregisterAllByBundle begin");
    IncreaseCnt(__func__);
    std::lock_guard<std::mutex> lock(syncFolderMtx_);
    auto& rootManager = SynchronousRootManager::GetInstance();
    std::vector<SyncFolder> syncFolders;
    if (!rootManager.GetRootInfosByUserAndBundle(bundleName, index, userId, syncFolders)) {
        HILOG_ERROR("GetRootInfosByUserAndBundle failed.");
        DecreaseCnt(__func__);
        return E_INVALID_PARAM;
    }
    for (const auto &folder : syncFolders) {
        bool isDeleted = rootManager.DeleteSynchronousRoot(folder.path_, userId);
        if (!isDeleted) {
            HILOG_ERROR("Failed to delete sync root: %{public}s", folder.path_.c_str());
            continue;
        }
        auto ret = FileManagement::CloudDiskService::CloudDiskServiceManager::GetInstance().
            UnregisterSyncFolder(userId, bundleName, folder.path_);
        if (ret != ERR_OK) {
            HILOG_ERROR("UnregisterSyncFolder failed, ret : %{public}d", ret);
            continue;
        }
        SyncFolderExt syncFolderExt;
        syncFolderExt.path_ = folder.path_;
        syncFolderExt.bundleName_ = bundleName;
        syncFolderExt.state_ = folder.state_;
        syncFolderExt.displayName_ = folder.displayName_;
        syncFolderExt.displayNameResId_ = folder.displayNameResId_;
        if (!NotifyWorkService::GetInstance().NotifySyncFolderEvent(syncFolderExt,
            NotifyWorkService::EventType::UNREGISTER)) {
                HILOG_ERROR("FileAccessService::Active NotifySyncFolderDetail failed");
        }
    }
    if (rootManager.GetRootCount() == 0) {
        SetWorkStatus(false);
    }
    HILOG_INFO("FileAccessService::UnregisterAllByBundle success");
    DecreaseCnt(__func__);
    return ERR_OK;
#else
    return E_NOT_SUPPORT;
#endif
}

int32_t FileAccessService::Changestate(const std::string &path, const State& newState)
{
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    HILOG_INFO("FileAccessService::Changestate begin");
    int index;
    std::string bundleName;
    
    if (UfsAccessTokenHelper::GetCallerBundleNameAndIndex(bundleName, index) != ERR_OK) {
        HILOG_ERROR("Get caller bundleName and index failed.");
        return E_INVALID_PARAM;
    }

    int32_t userId = UfsAccessTokenHelper::GetUserId();

    auto& rootManager = SynchronousRootManager::GetInstance();
    std::vector<SyncFolderExt> syncFolderExts;
    auto ret = IsSyncFolderInTable(path, syncFolderExts, userId);
    if (ret != ERR_OK) {
        HILOG_ERROR("SyncFolder not in table, Changestate failed");
        return ret;
    }
    if (!rootManager.UpdateSynchronousRootState(path, bundleName, userId, index, newState)) {
        HILOG_ERROR("Update root state failed for path: %{private}s", path.c_str());
        return E_TRY_AGAIN;
    }
    SyncFolderExt syncFolderExt;
    for (auto &it : syncFolderExts) {
        if (it.path_ == path) {
            syncFolderExt.path_ = it.path_;
            syncFolderExt.state_ = newState;
            syncFolderExt.bundleName_ = it.bundleName_;
            syncFolderExt.displayName_ = it.displayName_;
            syncFolderExt.displayNameResId_ = it.displayNameResId_;
            break;
        }
    }
    NotifyWorkService::EventType state =
        (newState == State::ACTIVE) ? NotifyWorkService::EventType::ACTIVE : NotifyWorkService::EventType::INACTIVE;
    if (!NotifyWorkService::GetInstance().NotifySyncFolderEvent(syncFolderExt, state)) {
        HILOG_ERROR("FileAccessService::Active NotifySyncFolderEvent failed");
    }
    HILOG_INFO("FileAccessService::Changestate success");
    return ERR_OK;
#else
    return E_NOT_SUPPORT;
#endif
}

int32_t FileAccessService::Active(const std::string &path)
{
    HILOG_INFO("FileAccessService::Active begin");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    IncreaseCnt(__func__);
    auto res = Changestate(path, State::ACTIVE);
    HILOG_INFO("FileAccessService::Active res: %{public}d", res);
    DecreaseCnt(__func__);
    return res;
#else
    return E_NOT_SUPPORT;
#endif
}

int32_t FileAccessService::Deactive(const std::string &path)
{
    HILOG_INFO("FileAccessService::Deactive begin");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    IncreaseCnt(__func__);
    auto res = Changestate(path, State::INACTIVE);
    HILOG_INFO("FileAccessService::Deactive res: %{public}d", res);
    DecreaseCnt(__func__);
    return res;
#else
    return E_NOT_SUPPORT;
#endif
}

int32_t FileAccessService::GetSyncFolders(std::vector<SyncFolder> &syncFolders)
{
    HILOG_INFO("FileAccessService::GetSyncFolders begin");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    IncreaseCnt(__func__);
    int index;
    std::string bundleName;
    if (UfsAccessTokenHelper::GetCallerBundleNameAndIndex(bundleName, index) != ERR_OK) {
        HILOG_ERROR("Get caller bundleName and index failed.");
        DecreaseCnt(__func__);
        return E_INVALID_PARAM;
    }
    int32_t userId = UfsAccessTokenHelper::GetUserId();
    auto& rootManager = SynchronousRootManager::GetInstance();
    if (!rootManager.GetRootInfosByUserAndBundle(bundleName, index, userId, syncFolders)) {
        HILOG_ERROR("Get root infos failed for bundleName: %{public}s, index: %{public}d, userId: %{public}d",
            bundleName.c_str(), index, userId);
        DecreaseCnt(__func__);
        return E_SYNC_FOLDER_NOT_REGISTERED;
    }

    HILOG_INFO("FileAccessService::GetRoots success, found %{public}zu roots", syncFolders.size());
    DecreaseCnt(__func__);
    return ERR_OK;
#else
    return E_NOT_SUPPORT;
#endif
}

int32_t FileAccessService::GetAllSyncFolders(std::vector<SyncFolderExt>& syncFolderExts)
{
    HILOG_INFO("FileAccessService::GetAllSyncFolders begin");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    IncreaseCnt(__func__);
    if (!UfsAccessTokenHelper::CheckCallerPermission(cloud_disk_permissionName)) {
        HILOG_ERROR("CheckCallerPermission failed");
        DecreaseCnt(__func__);
        return E_PERMISSION;
    }
    std::vector<std::map<std::string, std::string>> syncFolders;
    int32_t userId = UfsAccessTokenHelper().GetUserId();
    if (!SynchronousRootManager::GetInstance().GetAllRootInfosByUserId(userId, syncFolderExts)) {
        HILOG_ERROR("Get all root infos failed");
        DecreaseCnt(__func__);
        return E_SYNC_FOLDER_NOT_REGISTERED;
    }
    HILOG_INFO("Found %{public}zu roots", syncFolderExts.size());
    int32_t cnt = SynchronousRootManager::GetInstance().GetRootCount();
    HILOG_INFO("Found %{public}d", cnt);
    DecreaseCnt(__func__);
    return ERR_OK;
#else
    return E_NOT_SUPPORT;
#endif
}

int32_t FileAccessService::UpdateDisplayName(const std::string &path, const std::string &displayName)
{
    HILOG_INFO("FileAccessService::UpdateDisplayName begin");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    IncreaseCnt(__func__);
    int index;
    std::string bundleName;
    
    if (UfsAccessTokenHelper::GetCallerBundleNameAndIndex(bundleName, index) != ERR_OK) {
        HILOG_ERROR("Get caller bundleName and index failed.");
        DecreaseCnt(__func__);
        return E_INVALID_PARAM;
    }

    int32_t userId = UfsAccessTokenHelper::GetUserId();
    
    auto& rootManager = SynchronousRootManager::GetInstance();
    if (!rootManager.validateDisplayName(displayName)) {
        HILOG_ERROR("INVALID DISPLAYNAME");
        DecreaseCnt(__func__);
        return E_INVALID_PARAM;
    }
    std::vector<SyncFolderExt> syncFolderExts;
    auto ret = IsSyncFolderInTable(path, syncFolderExts, userId);
    if (ret != ERR_OK) {
        HILOG_ERROR("SyncFolder not in table,Unregister failed");
        DecreaseCnt(__func__);
        return ret;
    }
    if (!rootManager.UpdateDisplayName(path, bundleName, userId, index, displayName)) {
        HILOG_ERROR("Update root state failed for path: %{private}s", path.c_str());
        DecreaseCnt(__func__);
        return E_TRY_AGAIN;
    }
    SyncFolderExt syncFolderExt;
    for (auto &it : syncFolderExts) {
        if (it.path_ == path) {
            syncFolderExt.path_ = it.path_;
            syncFolderExt.bundleName_ = it.bundleName_;
            syncFolderExt.state_ = it.state_;
            syncFolderExt.displayName_ = displayName;
            syncFolderExt.displayNameResId_ = it.displayNameResId_;
            break;
        }
    }
    if (!NotifyWorkService::GetInstance().NotifySyncFolderEvent(syncFolderExt, NotifyWorkService::EventType::UPDATE)) {
        HILOG_ERROR("FileAccessService::UpdateDisplayName otifySyncFolderEvent failed");
    }
    DecreaseCnt(__func__);
    return ERR_OK;
#else
    return E_NOT_SUPPORT;
#endif
}

int32_t FileAccessService::UnregisterForSa(const string &path)
{
    HILOG_INFO("FileAccessService::UnregisterForSa begin");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    IncreaseCnt(__func__);
    
    if (!UfsAccessTokenHelper::CheckCallerPermission(cloud_disk_permissionName)) {
        HILOG_ERROR("CheckCallerPermission failed");
        DecreaseCnt(__func__);
        return E_NOT_SUPPORT;
    }
    int32_t userId = GetCurrentUserId();
    std::vector<SyncFolderExt> syncFolderExts;
    auto ret = IsSyncFolderInTable(path, syncFolderExts, userId);
    if (ret != ERR_OK) {
        HILOG_ERROR("SyncFolder not in table,Unregister failed");
        DecreaseCnt(__func__);
        return ret;
    }
    auto& rootManager = SynchronousRootManager::GetInstance();
    if (!rootManager.DeleteSynchronousRoot(path, userId)) {
        HILOG_ERROR("Delete root failed");
        DecreaseCnt(__func__);
        return E_REMOVE_SYNC_FOLDER_FAILED;
    }
    if (rootManager.GetRootCount() == 0) {
        SetWorkStatus(false);
    }
    SyncFolderExt syncFolderExt;
    for (auto &it : syncFolderExts) {
        if (it.path_ == path) {
            syncFolderExt.path_ = it.path_;
            syncFolderExt.bundleName_ = it.bundleName_;
            syncFolderExt.state_ = it.state_;
            syncFolderExt.displayName_ = it.displayName_;
            syncFolderExt.displayNameResId_ = it.displayNameResId_;
            break;
        }
    }

    if (!NotifyWorkService::GetInstance().NotifySyncFolderEvent(syncFolderExt,
        NotifyWorkService::NotifyWorkService::EventType::INACTIVE)) {
        HILOG_ERROR("FileAccessService::UnregisterForSa otifySyncFolderEvent failed");
    }
    DecreaseCnt(__func__);
    return ERR_OK;
#else
    return E_NOT_SUPPORT;
#endif
}

int32_t FileAccessService::GetAllSyncFoldersForSa(std::vector<SyncFolderExt> &syncFolderExts)
{
    HILOG_INFO("FileAccessService::GetAllSyncFoldersForSa begin");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    IncreaseCnt(__func__);
    if (!UfsAccessTokenHelper::CheckCallerPermission(cloud_disk_permissionName)) {
        HILOG_ERROR("CheckCallerPermission failed");
        DecreaseCnt(__func__);
        return E_PERMISSION;
    }
    auto currentUserId = GetCurrentUserId();
    if (currentUserId == INVALID_USER_ID) {
        HILOG_ERROR("INVALID USERID");
        DecreaseCnt(__func__);
        return E_INVALID_PARAM;
    }
    if (!SynchronousRootManager::GetInstance().GetAllRootInfosByUserId(currentUserId, syncFolderExts)) {
        HILOG_ERROR("Get all root infos failed");
        DecreaseCnt(__func__);
        return E_SYNC_FOLDER_NOT_REGISTERED;
    }
    HILOG_INFO("Found %{public}zu roots", syncFolderExts.size());
    DecreaseCnt(__func__);
    return ERR_OK;
#else
    return E_NOT_SUPPORT;
#endif
}

void FileAccessService::OnStart(const SystemAbilityOnDemandReason &startReason)
{
    HILOG_INFO("FileAccessService::OnStart reson");
    AddSystemAbilityListener(COMMON_EVENT_SERVICE_ID);
    if (startReason.HasExtraData()) {
        auto task = [obj {wptr<FileAccessService>(this)}]() {
            auto ptr = obj.promote();
            if (ptr == nullptr) {
                HILOG_ERROR("OnStart ptr is nullptr");
                return;
            }
            std::lock_guard<std::mutex> lock(obj->observerMux_);
            if (ptr->bundleObserver_ == nullptr) {
                HILOG_INFO("OnStart reason new bundleObserver_");
                EventFwk::MatchingSkills matchingSkills;
                matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
                EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
                ptr->bundleObserver_ = std::make_shared<BundleObserver>(subscriberInfo, obj);
            }
            ptr->bundleObserver_->HandleBundleBroadcast();
        };
        handleBroadCastThreadPool_.AddTask(task);
    }
    OnStart();
}

void FileAccessService::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    HILOG_INFO("FileAccessService::OnAddSystemAbility Begin");
    if (systemAbilityId == COMMON_EVENT_SERVICE_ID) {
        std::lock_guard<std::mutex> lock(observerMux_);
        if (bundleObserver_ == nullptr || !isRegister_)  {
            HILOG_ERROR("OnAddSystemAbility, bundleObserver is null, Will register bundle broadcast");
            RegisterBundleBroadcast();
        }
    }
}

void FileAccessService::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    HILOG_INFO("FileAccessService::OnRemoveSystemAbility");
}


void FileAccessService::RegisterBundleBroadcast()
{
    HILOG_INFO("FileAccessService::RegisterBundleBroadcast Begin");
    if (bundleObserver_ == nullptr) {
        EventFwk::MatchingSkills matchingSkills;
        matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
        EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
        bundleObserver_ = std::make_shared<BundleObserver>(subscriberInfo, wptr<FileAccessService>(this));
    }
    EventFwk::CommonEventManager::NewSubscribeCommonEvent(bundleObserver_);
    isRegister_ = true;
    HILOG_INFO("FileAccessService::RegisterBundleBroadcast End");
}

void FileAccessService::UnRegisterBundleBroadcast()
{
    HILOG_INFO("FileAccessService::UnRegisterBundleBroadcast Begin");
    std::lock_guard<std::mutex> lock(observerMux_);
    if (bundleObserver_ == nullptr) {
        HILOG_ERROR("UnRegisterBundleBroadcast error, bundleObserver is null");
        return;
    }
    int32_t ret = EventFwk::CommonEventManager::NewUnSubscribeCommonEventSync(bundleObserver_);
    HILOG_INFO("FileAccessService::UnRegisterBundleBroadcast End. ret = %{public}d", ret);
}
} // namespace FileAccessFwk
} // namespace OHOS
