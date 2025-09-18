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

#include "ipc_skeleton.h"
#include "tokenid_kit.h"
#include "file_access_framework_errno.h"
#include "file_access_service_client.h"

namespace OHOS {
namespace FileManagement {
using namespace FileAccessFwk;
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
} // namespace FileAccessFwk
} // namespace OHOS
