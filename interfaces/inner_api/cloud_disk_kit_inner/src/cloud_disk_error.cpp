/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "cloud_disk_error.h"

#include <unordered_map>

#include "cloud_disk_error_code.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace FileManagement {

namespace {
const std::unordered_map<int32_t, int32_t> CLOUD_DISK_API_ERR_CODE_TABLE = {
    {E_OK, CLOUD_DISK_OK},
    {E_PERMISSION, CLOUD_DISK_PERMISSION_DENIED},
    {E_NOT_SUPPORT, CLOUD_DISK_NOT_SUPPORTED},
    {E_INVALID_PARAM, CLOUD_DISK_INVALID_ARG},
    {E_SYNC_FOLDER_PATH_UNAUTHORIZED, CLOUD_DISK_SYNC_FOLDER_PATH_UNAUTHORIZED},
    {E_IPC_FAILED, CLOUD_DISK_IPC_FAILED},
    {E_SYNC_FOLDER_NOT_REGISTERED, CLOUD_DISK_SYNC_FOLDER_NOT_REGISTERED},
    {E_SYNC_FOLDER_PATH_NOT_EXIST, CLOUD_DISK_SYNC_FOLDER_PATH_NOT_EXIST},
    {E_TRY_AGAIN, CLOUD_DISK_TRY_AGAIN},
    {E_SYSTEM_RESTRICTED, CLOUD_DISK_NOT_ALLOWED},
    {E_NOT_A_DIRECTORY, CLOUD_DISK_NOT_A_DIRECTORY},
    {E_FILE_NOT_EXIST, CLOUD_DISK_FILE_NOT_EXIST},
    {E_NAME_TOO_LONG, CLOUD_DISK_NAME_TOO_LONG},
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

int32_t ConvertToCloudDiskApiErrCode(int32_t errCode)
{
    auto iter = CLOUD_DISK_API_ERR_CODE_TABLE.find(errCode);
    if (iter != CLOUD_DISK_API_ERR_CODE_TABLE.end()) {
        HILOG_INFO("CloudDiskError::ConvertToCloudDiskApiErrCode err=%{public}d apiErr=%{public}d",
            errCode, iter->second);
        return iter->second;
    }
    HILOG_ERROR("CloudDiskError::ConvertToCloudDiskApiErrCode unknown err=%{public}d", errCode);
    return CLOUD_DISK_TRY_AGAIN;
}

std::string GetCloudDiskErrMsg(int32_t errCode)
{
    auto iter = CLOUD_DISK_ERR_MSG_TABLE.find(errCode);
    if (iter != CLOUD_DISK_ERR_MSG_TABLE.end()) {
        return iter->second;
    }
    return CLOUD_DISK_ERR_MSG_TABLE.at(CLOUD_DISK_TRY_AGAIN);
}
} // namespace

CloudDiskErrorInfo GetCloudDiskErrorInfo(int32_t errCode)
{
    int32_t cloudDiskErrCode = ConvertToCloudDiskApiErrCode(errCode);
    return {cloudDiskErrCode, GetCloudDiskErrMsg(cloudDiskErrCode)};
}
} // namespace FileManagement
} // namespace OHOS
