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
#include <regex>
#include "file_access_framework_errno.h"
#include "cloud_disk_synchronous_root_manager.h"
#include "file_access_helper.h"

namespace OHOS {
namespace FileAccessFwk {
namespace {
    const int32_t MAX_CLOUD_DISK_DISPLAY_NAMELENGTH = 255;
    const int32_t MAX_ROOT_COUNT = 9;
}

bool SynchronousRootManager::Init()
{
    {
        std::lock_guard<std::mutex> lock(rdbMutex_);
        if (rdbStore_ == nullptr) {
            HILOG_ERROR("rdbStore_ is nullptr");
            return false;
        }
        if (!(rdbStore_->Init())) {
            rdbStore_ = nullptr;
            HILOG_ERROR("rdbStore_ Init failed");
            return false;
        }
    }
    if (!CreateTable()) {
        HILOG_ERROR("CreateTable failed");
        return false;
    }
    HILOG_INFO("init end!");
    return true;
}

bool SynchronousRootManager::PutSynchronousRoot(const SyncFolder& rootInfo,
    const std::string& bundleName, int32_t userId, int32_t index)
{
    ValuesBucket values;
    values.PutString(BUNDLENAME, bundleName);
    values.PutString(PATH, rootInfo.path_);
    values.PutInt(STATE, static_cast<int>(State::INACTIVE));
    values.PutString(CLOUD_DISK_DISPLAY_NAME, rootInfo.displayName_);
    values.PutInt(DISPLAY_NAME_RES_ID, rootInfo.displayNameResId_);
    values.PutInt(USERID, userId);
    values.PutInt(INDEX, index);
    PutTimeStamp(values);
    int64_t rowId = -1;
    {
        std::lock_guard<std::mutex> lock(rdbMutex_);
        if (rdbStore_ == nullptr) {
            HILOG_ERROR("rdbStore_ is nullptr");
            return false;
        }

        auto ret = rdbStore_->Put(rowId, SYNCHRONOUS_ROOT_TABLE, values);
        if (!ret) {
            HILOG_ERROR("synchronous_root_table insert failed");
            return false;
        }
    }

    HILOG_INFO("synchronous_root_table insert success!");
    return true;
}

bool SynchronousRootManager::DeleteSynchronousRoot(const std::string& path, int32_t userId)
{
    {
        std::lock_guard<std::mutex> lock(rdbMutex_);
        if (rdbStore_ == nullptr) {
            HILOG_ERROR("rdbStore_ is nullptr");
            return false;
        }
        int32_t deleteRows = -1;
        int32_t ret = rdbStore_->Delete(deleteRows, SYNCHRONOUS_ROOT_TABLE, "path = ? AND userId = ?",
            std::vector<ValueObject>{ValueObject(path), ValueObject(userId)});
        if (!ret) {
            HILOG_ERROR("delete table data failed");
            return false;
        }
        HILOG_INFO("synchronous_root_table delete row : %{public}d!", deleteRows);
    }
    return true;
}

bool SynchronousRootManager::DeleteAllSynRootsByUserAndBundle(
    const std::string& bundleName, int32_t userId, int32_t index)
{
    std::lock_guard<std::mutex> lock(rdbMutex_);
    if (rdbStore_ == nullptr) {
        HILOG_ERROR("rdbStore_ is nullptr");
        return false;
    }
    
    int32_t deleteRows = 0;
    bool ret = rdbStore_->Delete(deleteRows, SYNCHRONOUS_ROOT_TABLE, DELETE_ALL_CONDITION,
        std::vector<ValueObject>{ValueObject(userId), ValueObject(bundleName), ValueObject(index)});
    if (!ret) {
        HILOG_ERROR("Delete all roots failed for bundle:%{public}s, index:%{public}d, user:%{public}d",
            bundleName.c_str(), index, userId);
        return false;
    }
    
    HILOG_INFO("Deleted %{public}d roots for bundle: %{public}s, index: %{public}d, user: %{public}d",
        deleteRows, bundleName.c_str(), index, userId);
    return true;
}

bool SynchronousRootManager::UpdateSynchronousRootState(const std::string& path,
    const std::string& bundleName, int32_t userId, int32_t index, const State& newState)
{
    SyncFolder rootInfo;
    rootInfo.path_ = path;
    if (!GetSynchronousRootByPathAndUserId(rootInfo, userId)) {
        HILOG_ERROR("get synchronous root failed");
        return false;
    }
    int32_t changedRows = CHANGEROWCNT_INIT;
    ValuesBucket values;
    values.PutString(BUNDLENAME, bundleName);
    values.PutString(PATH, rootInfo.path_);
    values.PutInt(STATE, static_cast<int>(newState));
    values.PutInt(DISPLAY_NAME_RES_ID, rootInfo.displayNameResId_);
    values.PutInt(USERID, userId);
    values.PutInt(INDEX, index);
    PutTimeStamp(values);
    {
        std::lock_guard<std::mutex> lock(rdbMutex_);
        auto ret = rdbStore_->Update(changedRows, SYNCHRONOUS_ROOT_TABLE, values, ACTION_CONDITION,
            std::vector<ValueObject>{ ValueObject(userId), ValueObject(bundleName),
                ValueObject(index), ValueObject(rootInfo.path_)});
        if (!ret) {
            HILOG_ERROR("rdbAdapter update failed ret:%{public}d", ret);
            return false;
        }
        if (changedRows != 1) {
            HILOG_ERROR("update affected rows is not 1, actual: %{public}d", changedRows);
            return false;
        }
    }
    return true;
}

bool SynchronousRootManager::UpdateDisplayName(const std::string& path,
    const std::string& bundleName, int32_t userId, int32_t index, const std::string& displayName)
{
    SyncFolder rootInfo;
    rootInfo.path_ = path;
    if (!GetSynchronousRootByPathAndUserId(rootInfo, userId)) {
        HILOG_ERROR("get synchronous root failed");
        return false;
    }
    int32_t changedRows = CHANGEROWCNT_INIT;
    ValuesBucket values;
    values.PutString(BUNDLENAME, bundleName);
    values.PutString(CLOUD_DISK_DISPLAY_NAME, displayName);
    values.PutString(PATH, rootInfo.path_);
    values.PutInt(STATE, static_cast<int>(rootInfo.state_));
    values.PutInt(DISPLAY_NAME_RES_ID, rootInfo.displayNameResId_);
    values.PutInt(USERID, userId);
    values.PutInt(INDEX, index);
    PutTimeStamp(values);
    {
        std::lock_guard<std::mutex> lock(rdbMutex_);
        auto ret = rdbStore_->Update(changedRows, SYNCHRONOUS_ROOT_TABLE, values, ACTION_CONDITION,
            std::vector<ValueObject>{ ValueObject(userId), ValueObject(bundleName),
                ValueObject(index), ValueObject(rootInfo.path_)});
        if (!ret) {
            HILOG_ERROR("rdbAdapter update failed ret:%{public}d", ret);
            return false;
        }
        if (changedRows != 1) {
            HILOG_ERROR("update affected rows is not 1, actual: %{public}d", changedRows);
            return false;
        }
    }
    return true;
}

bool SynchronousRootManager::GetRootInfosByUserAndBundle(
    const std::string& bundleName, int32_t index, int32_t userId, std::vector<SyncFolder>& syncFolders)
{
    if (rdbStore_ == nullptr) {
        HILOG_ERROR("rdbStore_ is nullptr");
        return false;
    }

    // 执行查询
    std::shared_ptr<ResultSet> resultSet = rdbStore_->Get(
        SELECT_SYNCHRONOUS_ROOT_TABLE_WHERE_USERID_AND_BUNDLENAME,
        std::vector<ValueObject>{ ValueObject(userId), ValueObject(bundleName), ValueObject(index)});
    if (resultSet == nullptr) {
        HILOG_ERROR("ResultSet is nullptr");
        return false;
    }

    // 获取查询结果
    int32_t rowCount = -1;
    resultSet->GetRowCount(rowCount);
    if (rowCount <= 0) {
        HILOG_INFO("No root info found: %{public}d, bundleName: %{public}s, index: %{public}d",
            userId, bundleName.c_str(), index);
        resultSet->Close();
        return true; // 无数据不视为错误，返回空列表
    }

    // 遍历结果集
    int32_t ret = resultSet->GoToFirstRow();
    while (ret == ERR_OK) {
        RowEntity rowEntity;
        if (resultSet->GetRow(rowEntity) != ERR_OK) {
            HILOG_ERROR("Failed to get row data");
            break;
        }

        SyncFolder info;
        info.path_ = static_cast<std::string>(rowEntity.Get(PATH));
        info.state_ = static_cast<State>((int)rowEntity.Get(STATE));
        info.displayName_ = static_cast<std::string>(rowEntity.Get(CLOUD_DISK_DISPLAY_NAME));
        info.displayNameResId_ = static_cast<uint32_t>(static_cast<int>(rowEntity.Get(DISPLAY_NAME_RES_ID)));
        syncFolders.push_back(info);

        ret = resultSet->GoToNextRow();
    }
    resultSet->Close();
    return true;
}

bool SynchronousRootManager::GetAllRootInfosByUserId(
    int32_t userId, std::vector<SyncFolderExt>& syncFolderExts)
{
    if (rdbStore_ == nullptr) {
        HILOG_ERROR("rdbStore_ is nullptr");
        return false;
    }

    std::shared_ptr<ResultSet> resultSet = rdbStore_->Get(
        SELECT_SYNCHRONOUS_ROOT_TABLE_WHERE_USERID, std::vector<ValueObject>{ValueObject(userId)});

    if (resultSet == nullptr) {
        HILOG_ERROR("ResultSet is nullptr");
        return false;
    }
    int32_t ret = resultSet->GoToFirstRow();
    while (ret == ERR_OK) {
        RowEntity rowEntity;
        if (resultSet->GetRow(rowEntity) != ERR_OK) {
            break;
        }
        SyncFolderExt info;
        info.path_ = static_cast<std::string>(rowEntity.Get(PATH));
        info.state_ = static_cast<State>((int)rowEntity.Get(STATE));
        info.displayNameResId_ = static_cast<uint32_t>(static_cast<int>(rowEntity.Get(DISPLAY_NAME_RES_ID)));
        info.bundleName_ = static_cast<std::string>(rowEntity.Get(BUNDLENAME));
        info.displayName_ = static_cast<std::string>(rowEntity.Get(CLOUD_DISK_DISPLAY_NAME));
        syncFolderExts.push_back(info);
        ret = resultSet->GoToNextRow();
    }
    resultSet->Close();
    return true;
}

bool SynchronousRootManager::GetSynchronousRootByPathAndUserId(SyncFolder& syncFolder, int32_t userId)
{
    if (rdbStore_ == nullptr) {
        HILOG_ERROR("rdbStore_ is nullptr");
        return false;
    }
    std::shared_ptr<ResultSet> resultSet = rdbStore_->Get(SELECT_SYNCHRONOUS_ROOT_TABLE_WHERE_PATH_AND_UID,
        std::vector<ValueObject>{ ValueObject(syncFolder.path_), ValueObject(userId)});
    if (resultSet == nullptr) {
        HILOG_ERROR("ResultSet is nullptr");
        return false;
    }
    int32_t rowCount = -1;
    resultSet->GetRowCount(rowCount);
    if (rowCount == 0) {
        HILOG_ERROR("syncFolder not find");
        resultSet->Close();
        return false;
    }
    int32_t ret = resultSet->GoToFirstRow();
    if (ret != ERR_OK) {
        HILOG_ERROR("syncFolder not find");
        resultSet->Close();
        return false;
    }
    RowEntity rowEntity;
    if (resultSet->GetRow(rowEntity) != ERR_OK) {
        HILOG_ERROR("get resultSet failed");
        resultSet->Close();
        return false;
    }
    syncFolder.path_ = static_cast<std::string>(rowEntity.Get(PATH));
    syncFolder.state_ = static_cast<State>((int)rowEntity.Get(STATE));
    syncFolder.displayNameResId_ = static_cast<uint32_t>(static_cast<int>(rowEntity.Get(DISPLAY_NAME_RES_ID)));
    syncFolder.displayName_ = static_cast<std::string>(rowEntity.Get(CLOUD_DISK_DISPLAY_NAME));
    resultSet->Close();
    return true;
}

int32_t SynchronousRootManager::GetRootNumByUserIdAndBundleName(const std::string& bundleName,
    int32_t index, int32_t userId)
{
    if (rdbStore_ == nullptr) {
        HILOG_ERROR("rdbStore_ is nullptr");
        return false;
    }
    std::shared_ptr<ResultSet> resultSet = rdbStore_->Get(
        SELECT_SYNCHRONOUS_ROOT_TABLE_WHERE_USERID_AND_BUNDLENAME,
        std::vector<ValueObject>{ ValueObject(userId), ValueObject(bundleName), ValueObject(index)});
    if (resultSet == nullptr) {
        HILOG_ERROR("ResultSet is nullptr");
        return false;
    }
    int32_t rowCount = -1;
    resultSet->GetRowCount(rowCount);
    resultSet->Close();
    HILOG_INFO("Total root count: %{public}d", rowCount);
    return rowCount;
}

int32_t SynchronousRootManager::GetRootCount()
{
    if (rdbStore_ == nullptr) {
        HILOG_ERROR("rdbStore_ is nullptr");
        return -1;
    }

    std::shared_ptr<ResultSet> resultSet = rdbStore_->Get(
        "SELECT * FROM synchronous_root_table", std::vector<ValueObject>());
    if (resultSet == nullptr) {
        HILOG_ERROR("ResultSet is nullptr");
        return -1;
    }

    int32_t rowCount = -1;
    resultSet->GetRowCount(rowCount);

    HILOG_INFO("Total root count: %{public}d", rowCount);
    resultSet->Close();
    return rowCount;
}

bool SynchronousRootManager::CreateTable()
{
    std::lock_guard<std::mutex> lock(rdbMutex_);
    if (rdbStore_ == nullptr) {
        HILOG_ERROR("rdbStore_ is nullptr");
        return false;
    }
    auto ret = rdbStore_->CreateTable();
    if (!ret) {
        HILOG_ERROR("table create failed");
        return false;
    }
    return true;
}

int32_t SynchronousRootManager::CheckExistParentOrChildRoot(
    const std::string& path, const std::string& bundleName, int userId)
{
    std::lock_guard<std::mutex> lock(rdbMutex_);
    if (rdbStore_ == nullptr) {
        HILOG_ERROR("rdbStore_ is nullptr");
        return E_PERMISSION;
    }

    std::shared_ptr<ResultSet> resultSet = rdbStore_->Get(
        SELECT_SYNCHRONOUS_ROOT_TABLE,
        std::vector<ValueObject>{});

    if (resultSet == nullptr) {
        HILOG_ERROR("ResultSet is nullptr");
        return E_PERMISSION;
    }

    int32_t rowCount = -1;
    resultSet->GetRowCount(rowCount);
    if (rowCount == 0) {
        HILOG_INFO("no found any SyncFolder");
        resultSet->Close();
        return ERR_OK;
    }

    // 遍历结果集检查是否存在父/子目录
    int32_t exist = ERR_OK;
    std::string currentPathNormalized = path;
    int32_t ret = resultSet->GoToFirstRow();
    while (ret == ERR_OK) {
        RowEntity rowEntity;
        if (resultSet->GetRow(rowEntity) != ERR_OK) {
            HILOG_ERROR("get resultSet row failed");
            break;
        }

        std::string targetPathNormalized = static_cast<std::string>(rowEntity.Get(PATH));
        std::string targetBundleName = static_cast<std::string>(rowEntity.Get(BUNDLENAME));
        int targetUserId = static_cast<int>(rowEntity.Get(USERID));
        if (IsParentOrChildPath(currentPathNormalized, targetPathNormalized) &&
            (targetUserId == userId)) {
            if (targetBundleName == bundleName) {
                exist = E_SYNC_FOLDER_CONFLICT_SELF;
            } else {
                exist = E_SYNC_FOLDER_CONFLICT_OTHER;
            }
            break;
        }
        ret = resultSet->GoToNextRow();
    }
    resultSet->Close();
    return exist;
}

std::string SynchronousRootManager::NormalizeUri(const std::string& path)
{
    std::string normalized = path;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    while (!normalized.empty() && normalized.back() == '/') {
        normalized.pop_back();
    }
    return normalized;
}

bool SynchronousRootManager::IsParentOrChildPath(const std::string& path1, const std::string& path2)
{
    if (path1.empty() || path2.empty()) {
        return false;
    }
    if (path1.compare(path2) == 0) {
        return true;
    } else if (path1.find(path2) == 0) {
        size_t len = path2.length();
        return path1.length() > len && path1[len] == '/';
    } else if (path2.find(path1) == 0) {
        size_t len = path1.length();
        return path2.length() > len && path2[len] == '/';
    }
    return false;
}

bool SynchronousRootManager::IsPathAllowed(const std::string& path)
{
    std::string tmpPath = path;
    if (!tmpPath.empty() && tmpPath.back() != '/') {
        tmpPath += "/";
    }

    const std::string baseDirectory = "/storage/Users/currentUser/";
    const std::string blacklistedSubDirectory = "/storage/Users/currentUser/appdata/";

    if (tmpPath.find(baseDirectory) != 0) {
        HILOG_ERROR("get baseDirectory failed");
        return false;
    }

    if (tmpPath.find(blacklistedSubDirectory) == 0) {
        HILOG_ERROR("get blacklistedSubDirectory");
        return false;
    }

    return true;
}

void SynchronousRootManager::PutTimeStamp(ValuesBucket& values)
{
    std::time_t now = std::time(nullptr);
    char currentTime[80];
    std::strftime(currentTime, sizeof(currentTime), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    values.PutString("lastModifyTime", currentTime);
}

bool SynchronousRootManager::validateDisplayName(const std::string& displayName)
{
    if (displayName.empty()) {
        HILOG_INFO("Display name is empty");
        return true;
    }

    if (displayName.length() > MAX_CLOUD_DISK_DISPLAY_NAMELENGTH) {
        HILOG_ERROR("Display name cannot exceed 255 characters");
        return false;
    }
    
    // 检查非法字符
    const std::string invalidChars = "\\/:*?<>|\"";
    for (char c : invalidChars) {
        if (displayName.find(c) != std::string::npos) {
            HILOG_ERROR("Display name contains invalid character: %{public}s", std::string(1, c).c_str());
            return false;
        }
    }
    
    // 检查特殊名称
    if (displayName == "." || displayName == "..") {
        HILOG_ERROR("Display name cannot be '.' or '..'");
        return false;
    }
    
    // 检查是否全部为空格
    if (displayName.find_first_not_of(' ') == std::string::npos) {
        HILOG_ERROR("Display name cannot consist of only spaces");
        return false;
    }
      
    return true;
}

int32_t SynchronousRootManager::IsSyncFolderValid(const SyncFolder& folder, const std::string& bundleName,
    int32_t index, int32_t userId)
{
    std::string path = folder.path_;
    std::string displayName = folder.displayName_;
    
    if ((!IsPathAllowed(path)) || (!FileAccessHelper::IsFilePathValid(path))) {
        return E_INVALID_PARAM;
    }
    
    if (!validateDisplayName(displayName)) {
        return E_INVALID_PARAM;
    }

    if (GetRootNumByUserIdAndBundleName(bundleName, index, userId) > MAX_ROOT_COUNT) {
        HILOG_ERROR("The number of roots has reached the maximum limit.");
        return E_SYNC_FOLDER_LIMIT_EXCEEDED;
    }
    auto ret = CheckExistParentOrChildRoot(path, bundleName, userId);
    if (ret != ERR_OK) {
        HILOG_ERROR("The root exist parent or child root.");
        return ret;
    }
    
    return ERR_OK;
}
} // namespace FileAccessFwk
} // namespace OHOS