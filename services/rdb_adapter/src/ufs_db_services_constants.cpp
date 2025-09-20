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

#include "ufs_db_services_constants.h"

namespace OHOS {
namespace FileAccessFwk {

const std::string PATH = "path";
const std::string STATE = "state";
const std::string DISPLAY_NAME_RES_ID = "displayNameResId";
const std::string BUNDLENAME = "bundleName";
const std::string INDEX = "appIndex";
const std::string CLOUD_DISK_DISPLAY_NAME = "displayName";
const std::string USERID = "userId";
const std::string SYNCHRONOUS_ROOT_TABLE = "synchronous_root_table";
const std::string SYNCHRONOUS_ROOT_DATA_RDB_PATH = "/data/service/el1/public/database/ufs_db/";
const std::string SYNCHRONOUS_ROOT_DATABASE_NAME = "ufs_synchronous_root_db.db";
const std::string ACTION_CONDITION = "userId = ? AND bundleName = ? AND appIndex = ? AND  path = ?";
const std::string DELETE_CONDITION = "userId = ? AND bundleName = ? AND appIndex = ? AND  path = ?";
const std::string DELETE_ALL_CONDITION = "userid = ? AND bundleName = ?";
const std::string SELECT_SYNCHRONOUS_ROOT_TABLE_WHERE_PATH_AND_UID =
    "SELECT * FROM synchronous_root_table WHERE path = ? AND userId = ?";
const std::string SELECT_SYNCHRONOUS_ROOT_TABLE_WHERE_USERID_AND_BUNDLENAME =
    "SELECT * FROM synchronous_root_table WHERE userId = ? AND bundleName = ? AND appIndex = ?";
const std::string SELECT_SYNCHRONOUS_ROOT_TABLE_WHERE_USERID =
    "SELECT * FROM synchronous_root_table WHERE userid = ?";
const std::string SELECT_SYNCHRONOUS_ROOT_TABLE = "SELECT * FROM synchronous_root_table";
const std::string CREATE_SYNCHRONOUS_ROOT_TABLE_SQL = "CREATE TABLE IF NOT EXISTS synchronous_root_table \
    ( \
        path               TEXT NOT NULL, \
        state              INTEGER, \
        displayName        TEXT, \
        displayNameResId   INTEGER, \
        bundleName         TEXT, \
        userId             INTEGER NOT NULL, \
        appIndex           INTEGER, \
        lastModifyTime     TEXT,  \
        PRIMARY KEY (path, userId) \
    );";

const std::string SYNCFOLDER_WORK_STATUS_KEY = "persist.clouddiskmanager.workstatus";
const std::string SYNCFOLDER_WORK_STATUS_ON = "true";
const std::string SYNCFOLDER_WORK_STATUS_OFF = "false";
} // namespace FileAccessFwk
} // namespace OHOS