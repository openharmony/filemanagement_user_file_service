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

#include "cloud_disk_js_manager.h"

#include <set>
#include <unordered_map>

#ifdef SUPPORT_CLOUD_DISK_MANAGER
#include "cloud_disk_service_manager.h"
#endif
#include "ipc_skeleton.h"
#include "tokenid_kit.h"
#include "file_access_framework_errno.h"
#include "file_access_service_client.h"

namespace OHOS {
namespace FileManagement {
using namespace FileAccessFwk;

namespace {
enum CloudDiskApiErrCode : int32_t {
    CLOUD_DISK_OK = 0,
    CLOUD_DISK_PERMISSION_DENIED = 201,
    CLOUD_DISK_NOT_SUPPORTED = 801,
    CLOUD_DISK_INVALID_ARG = 34400001,
    CLOUD_DISK_SYNC_FOLDER_PATH_UNAUTHORIZED = 34400002,
    CLOUD_DISK_IPC_FAILED = 34400003,
    CLOUD_DISK_SYNC_FOLDER_NOT_REGISTERED = 34400008,
    CLOUD_DISK_SYNC_FOLDER_PATH_NOT_EXIST = 34400010,
    CLOUD_DISK_TRY_AGAIN = 34400014,
    CLOUD_DISK_NOT_ALLOWED = 34400015,
    CLOUD_DISK_NOT_A_DIRECTORY = 34400023,
    CLOUD_DISK_FILE_NOT_EXIST = 34400024,
    CLOUD_DISK_NAME_TOO_LONG = 34400025,
};

const std::unordered_map<int32_t, int32_t> CLOUD_DISK_API_ERR_CODE_TABLE = {
    {OHOS::ERR_OK, CLOUD_DISK_OK},
    {E_PERMISSION, CLOUD_DISK_PERMISSION_DENIED},
    {E_NOT_SUPPORT, CLOUD_DISK_NOT_SUPPORTED},
    {E_INVALID_PARAM, CLOUD_DISK_INVALID_ARG},
    {E_SYNC_FOLDER_PATH_UNAUTHORIZED, CLOUD_DISK_SYNC_FOLDER_PATH_UNAUTHORIZED},
    {E_IPC_FAILED, CLOUD_DISK_IPC_FAILED},
    {E_SYNC_FOLDER_NOT_REGISTERED, CLOUD_DISK_SYNC_FOLDER_NOT_REGISTERED},
    {E_SYNC_FOLDER_PATH_NOT_EXIST, CLOUD_DISK_SYNC_FOLDER_PATH_NOT_EXIST},
    {E_TRY_AGAIN, CLOUD_DISK_TRY_AGAIN},
    {E_SYSTEM_RESTRICTED, CLOUD_DISK_NOT_ALLOWED},
    {CLOUD_DISK_NOT_A_DIRECTORY, CLOUD_DISK_NOT_A_DIRECTORY},
    {CLOUD_DISK_FILE_NOT_EXIST, CLOUD_DISK_FILE_NOT_EXIST},
    {CLOUD_DISK_NAME_TOO_LONG, CLOUD_DISK_NAME_TOO_LONG},
};

const std::unordered_map<int32_t, std::string> CLOUD_DISK_ERR_MSG_TABLE = {
    {CLOUD_DISK_OK, "CLOUD_DISK_OK"},
    {CLOUD_DISK_PERMISSION_DENIED, "CLOUD_DISK_PERMISSION_DENIED"},
    {CLOUD_DISK_NOT_SUPPORTED, "CLOUD_DISK_NOT_SUPPORTED"},
    {CLOUD_DISK_INVALID_ARG, "CLOUD_DISK_INVALID_ARG"},
    {CLOUD_DISK_SYNC_FOLDER_PATH_UNAUTHORIZED, "CLOUD_DISK_SYNC_FOLDER_PATH_UNAUTHORIZED"},
    {CLOUD_DISK_IPC_FAILED, "CLOUD_DISK_IPC_FAILED"},
    {CLOUD_DISK_SYNC_FOLDER_NOT_REGISTERED, "CLOUD_DISK_SYNC_FOLDER_NOT_REGISTERED"},
    {CLOUD_DISK_SYNC_FOLDER_PATH_NOT_EXIST, "CLOUD_DISK_SYNC_FOLDER_PATH_NOT_EXIST"},
    {CLOUD_DISK_TRY_AGAIN, "CLOUD_DISK_TRY_AGAIN"},
    {CLOUD_DISK_NOT_ALLOWED, "CLOUD_DISK_NOT_ALLOWED"},
    {CLOUD_DISK_NOT_A_DIRECTORY, "CLOUD_DISK_NOT_A_DIRECTORY"},
    {CLOUD_DISK_FILE_NOT_EXIST, "CLOUD_DISK_FILE_NOT_EXIST"},
    {CLOUD_DISK_NAME_TOO_LONG, "CLOUD_DISK_NAME_TOO_LONG"},
};
} // namespace

#ifdef SUPPORT_CLOUD_DISK_MANAGER
constexpr int MAX_RETRY_TIMES = 3;

static bool IsSystemApp()
{
    uint64_t accessTokenIDEx = OHOS::IPCSkeleton::GetCallingFullTokenID();
    return OHOS::Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(accessTokenIDEx);
}

std::set<int32_t> cloudDiskJSErrCode = {
    FileAccessFwk::ERR_OK,                          // Success
    E_PERMISSION,                                   // Permission verification failed
    E_PERMISSION_SYS,                               // is not system app
    E_IPC_FAILED,                                   // IPC error
    E_TRY_AGAIN,                                    // Try again
    E_SYSTEM_RESTRICTED,                            // System restricted
};

static int32_t ConvertErrCode(int32_t errCode)
{
    if (cloudDiskJSErrCode.find(errCode) == cloudDiskJSErrCode.end()) {
        HILOG_ERROR("Not cloudDisk JS errcode: %{public}d", errCode);
        return E_IPC_FAILED;
    }
    return errCode;
}

#endif

int32_t CloudDiskJSManager::ConvertToCloudDiskApiErrCode(int32_t errCode)
{
    auto iter = CLOUD_DISK_API_ERR_CODE_TABLE.find(errCode);
    if (iter != CLOUD_DISK_API_ERR_CODE_TABLE.end()) {
        return iter->second;
    }
    HILOG_ERROR("Not cloudDisk API errcode: %{public}d", errCode);
    return CLOUD_DISK_IPC_FAILED;
}

std::string CloudDiskJSManager::GetCloudDiskErrMsg(int32_t errCode)
{
    int32_t cloudDiskErrCode = ConvertToCloudDiskApiErrCode(errCode);
    auto iter = CLOUD_DISK_ERR_MSG_TABLE.find(cloudDiskErrCode);
    if (iter != CLOUD_DISK_ERR_MSG_TABLE.end()) {
        return iter->second;
    }
    return CLOUD_DISK_ERR_MSG_TABLE.at(CLOUD_DISK_IPC_FAILED);
}

CloudDiskJSManager::CloudDiskJSManager(const std::string &syncFolderPath) : syncFolderPath_(syncFolderPath)
{
}

int CloudDiskJSManager::GetAllSyncFolders(std::vector<SyncFolderExt> &syncFolderExts)
{
    HILOG_INFO("CloudDiskJSManager::GetAllSyncFolders in");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    if (!IsSystemApp()) {
        HILOG_ERROR("GetAllSyncFolders check IsSystemAppByFullTokenID failed");
        return E_PERMISSION_SYS;
    }
    auto proxy = FileAccessServiceClient::GetInstance();
    if (proxy == nullptr) {
        HILOG_ERROR("GetAllSyncFolders get SA failed");
        return E_TRY_AGAIN;
    }
    int ret = proxy->GetAllSyncFolders(syncFolderExts);
    int retryTimes = 1;
    while (ret == E_SERVICE_DIED && retryTimes <= MAX_RETRY_TIMES) {
        proxy = FileAccessServiceClient::GetInstance();
        if (proxy == nullptr) {
            HILOG_ERROR("GetAllSyncFolders get SA failed");
        } else {
            ret = proxy->GetAllSyncFolders(syncFolderExts);
        }
        ++retryTimes;
    }
    return ConvertErrCode(ret);
#endif
    return E_NOT_SUPPORT;
}

int CloudDiskJSManager::IsPlaceholderFile(const std::string &relativePath, bool &isPlaceholder)
{
    HILOG_INFO("CloudDiskJSManager::IsPlaceholderFile in");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    auto ret = FileManagement::CloudDiskService::CloudDiskServiceManager::GetInstance().
        IsPlaceholderFile(syncFolderPath_, relativePath, isPlaceholder);
    return ConvertToCloudDiskApiErrCode(ret);
#endif
    return E_NOT_SUPPORT;
}
} // namespace FileAccessFwk
} // namespace OHOS
