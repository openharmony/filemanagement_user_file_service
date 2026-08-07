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

#ifndef CLOUD_DISK_ERROR_H
#define CLOUD_DISK_ERROR_H

#include <cstdint>
#include <string>

namespace OHOS {
namespace FileManagement {
enum CloudDiskInternalErrCode : int32_t {
    E_OK = 0,
    E_PERMISSION = 201,
    E_NOT_SUPPORT = 801,
    E_INVALID_PARAM = 34400001,
    E_SYNC_FOLDER_PATH_UNAUTHORIZED = 34400002,
    E_IPC_FAILED = 34400003,
    E_SYNC_FOLDER_NOT_REGISTERED = 34400008,
    E_SYNC_FOLDER_PATH_NOT_EXIST = 34400010,
    E_TRY_AGAIN = 34400014,
    E_SYSTEM_RESTRICTED = 34400015,
    E_NOT_A_DIRECTORY = 34400023,
    E_FILE_NOT_EXIST = 34400024,
    E_NAME_TOO_LONG = 34400025,
};

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

struct CloudDiskErrorInfo {
    int32_t code;
    std::string message;
};

CloudDiskErrorInfo GetCloudDiskErrorInfo(int32_t errCode);
CloudDiskErrorInfo GetCloudDiskApiErrorInfo(int32_t errCode);
} // namespace FileManagement
} // namespace OHOS

#endif // CLOUD_DISK_ERROR_H
