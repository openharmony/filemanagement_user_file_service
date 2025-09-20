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

#ifndef OHOS_UFS_DB_SERVICES_CONSTANTS_H
#define OHOS_UFS_DB_SERVICES_CONSTANTS_H

#include <unordered_set>
#include <string>
#include <unistd.h>

namespace OHOS {
namespace FileAccessFwk {

#define S_IFMT  0170000

#define S_IFDIR 0040000
#define S_IFCHR 0020000
#define S_IFBLK 0060000
#define S_IFREG 0100000
#define S_IFIFO 0010000
#define S_IFLNK 0120000
#define S_IFSOCK 0140000

constexpr int32_t PERMISSION_VERIFY_FAIL = 201;
constexpr int32_t SysCapId = 14500001;
constexpr int32_t INVALID_PARAMETER = SysCapId + 1;
constexpr int32_t SYNCHRONOUS_ROOT_NOT_PERSISTED = SysCapId + 2;
constexpr int32_t IPC_COMMUNICATION_FAIL = SysCapId + 3;
constexpr int32_t SYNCHRONOUS_ROOT_REACHED_LIMIT = SysCapId + 4;
constexpr int32_t CONFLICT_WITH_CURRENT_APP = SysCapId + 5;
constexpr int32_t CONFLICT_WITH_OTHER_APP = SysCapId + 6;
constexpr int32_t CREATE_SYNCHRONOUS_ROOT_FAIL = SysCapId + 7;
constexpr int32_t SYNCHRONOUS_ROOT_NOT_EXIST = SysCapId + 8;
constexpr int32_t REMOVE_SYNCHRONOUS_ROOT_FAIL = SysCapId + 9;
constexpr int32_t PERSIST_SYNCHRONOUS_ROOT_PROPERTY_FAIL = SysCapId + 10;
constexpr int32_t SYSTEM_CONTROL_DISABLED = SysCapId + 11;

constexpr int32_t AND_LENGTH = 5;
constexpr int32_t WHERE_LENGTH = 7;
constexpr int32_t RDB_INIT_MAX_TIMES = 30;
constexpr int32_t RDB_INIT_INTERVAL_TIME = 100000;
constexpr int32_t CHANGEROWCNT_INIT = -1;
extern const std::string PATH;
extern const std::string STATE;
extern const std::string DISPLAY_NAME_RES_ID;
extern const std::string BUNDLENAME;
extern const std::string CLOUD_DISK_DISPLAY_NAME;
extern const std::string USERID;
extern const std::string INDEX;
extern const std::string SYNCHRONOUS_ROOT_DATA_RDB_PATH;
extern const std::string SYNCHRONOUS_ROOT_DATABASE_NAME;
extern const std::string CREATE_SYNCHRONOUS_ROOT_TABLE_SQL;

extern const std::string SELECT_SYNCHRONOUS_ROOT_TABLE_WHERE_PATH_AND_UID;
extern const std::string SELECT_SYNCHRONOUS_ROOT_TABLE_WHERE_USERID_AND_BUNDLENAME;
extern const std::string SELECT_SYNCHRONOUS_ROOT_TABLE_WHERE_USERID;
extern const std::string SELECT_SYNCHRONOUS_ROOT_TABLE;
extern const std::string SYNCHRONOUS_ROOT_TABLE;
extern const std::string ACTION_CONDITION;
extern const std::string DELETE_CONDITION;
extern const std::string DELETE_ALL_CONDITION;

extern const std::string SYNCFOLDER_WORK_STATUS_KEY;
extern const std::string SYNCFOLDER_WORK_STATUS_ON;
extern const std::string SYNCFOLDER_WORK_STATUS_OFF;
} // namespace FileAccessFwk
} // namespace OHOS
#endif // OHOS_UFS_DB_SERVICES_CONSTANTS_H
