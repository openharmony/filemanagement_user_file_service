/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include <mutex>
#include <chrono>
#include <unistd.h>
#include "ufs_rdb_adapter.h"
#include "rdb_errno.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace FileAccessFwk {
using namespace std::chrono_literals;
constexpr int32_t UFS_SYNCROOT_V1 = 1;
constexpr int32_t UFS_SYNCROOT_V2 = 2;

namespace {
    const std::set<std::string> TABLES = {
        "synchronous_root_table",
    };
    const std::string TAG = "UfsRdbAdapter";
    std::vector<uint8_t> emp{};
}

bool RdbAdapter::Init()
{
    int32_t retryTimes = RDB_INIT_MAX_TIMES;
    uint32_t attemptedTimes = 0;
    while (retryTimes > 0) {
        attemptedTimes++;
        if (GetRDBPtr()) {
            uint32_t totalTimeMs = (attemptedTimes - 1) * RDB_INIT_INTERVAL_TIME / 1000;
            HILOG_INFO("rder init success, attempted times: %u, total time: %dms",
                attemptedTimes, totalTimeMs);
            return true;
        }
        usleep(RDB_INIT_INTERVAL_TIME);
        retryTimes--;
    }
    uint32_t totalTimeMs = attemptedTimes * RDB_INIT_INTERVAL_TIME / 1000;
    HILOG_ERROR("rdbit failed, attempted times: %u, total time: %dms",
        attemptedTimes, totalTimeMs);
    return false;
}

bool RdbAdapter::UnInit()
{
    HILOG_INFO("rdbAdapter unInit");
    {
        std::lock_guard<std::mutex> lock(rdbAdapterMtx_);
        store_ = nullptr;
    }
    return true;
}

bool RdbAdapter::Put(int64_t& outRowId, const std::string& table, const ValuesBucket& values)
{
    if (TABLES.find(table) == TABLES.end()) {
        HILOG_ERROR("table does not exist");
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(rdbAdapterMtx_);
        if (store_ == nullptr) {
            HILOG_ERROR("RDBStore_ is null");
            return false;
        }
        int32_t ret = store_->Insert(outRowId, table, values);
        if (ret == E_SQLITE_CORRUPT) {
            HILOG_ERROR("database corrupt ret:%{public}d", ret);
            int32_t restoreRet = store_->Restore("");
            if (restoreRet != E_OK) {
                HILOG_ERROR("Restore failed restoreRet:%{public}d", restoreRet);
                return false;
            }
            ret = store_->Insert(outRowId, table, values);
        }
        if (ret != E_OK) {
            HILOG_ERROR("rdbAdapter put failed ret:%{public}d", ret);
            return false;
        }
    }
    return true;
}

bool RdbAdapter::Delete(int32_t& deleteRows, const std::string& table, const std::string& whereClause,
    const std::vector<ValueObject>& bindArgs)
{
    if (TABLES.find(table) == TABLES.end()) {
        HILOG_ERROR("table does not exist");
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(rdbAdapterMtx_);
        if (store_ == nullptr) {
            HILOG_ERROR("RDBStore_ is null");
            return false;
        }
        int32_t ret = store_->Delete(deleteRows, table, whereClause, bindArgs);
        if (ret == E_SQLITE_CORRUPT) {
            HILOG_ERROR("database corrupt ret:%{public}d", ret);
            int32_t restoreRet = store_->Restore("");
            if (restoreRet != E_OK) {
                HILOG_ERROR("Restore failed restoreRet:%{public}d", restoreRet);
                return false;
            }
            ret = store_->Delete(deleteRows, table, whereClause, bindArgs);
        }
        if (ret != E_OK) {
            HILOG_ERROR("rdbAdapter delete failed ret:%{public}d", ret);
            return false;
        }
    }
    return true;
}

bool RdbAdapter::Update(int32_t& changedRows, const std::string& table, const ValuesBucket& values,
    const std::string& whereClause, const std::vector<ValueObject>& bindArgs)
{
    if (TABLES.find(table) == TABLES.end()) {
        HILOG_ERROR("table does not exist");
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(rdbAdapterMtx_);
        if (store_ == nullptr) {
            HILOG_ERROR("RDBStore_ is null");
            return false;
        }
        int32_t ret = store_->Update(changedRows, table, values, whereClause, bindArgs);
        if (ret == E_SQLITE_CORRUPT) {
            HILOG_ERROR("database corrupt ret:%{public}d", ret);
            int32_t restoreRet = store_->Restore("");
            if (restoreRet != E_OK) {
                HILOG_ERROR("Restore failed restoreRet:%{public}d", restoreRet);
                return false;
            }
            ret = store_->Update(changedRows, table, values, whereClause, bindArgs);
        }
        if (ret != E_OK) {
            HILOG_ERROR("rdbAdapter update failed ret:%{public}d", ret);
            return false;
        }
    }
    return true;
}

std::shared_ptr<ResultSet> RdbAdapter::Get(const std::string& sql, const std::vector<ValueObject>& args)
{
    std::shared_ptr<ResultSet> resultSet = nullptr;
    {
        std::lock_guard<std::mutex> lock(rdbAdapterMtx_);
        if (store_ == nullptr) {
            HILOG_ERROR("RDBStore_ is null");
            return nullptr;
        }
        resultSet = store_->QueryByStep(sql, args);
        if (resultSet == nullptr) {
            HILOG_ERROR("resultSet is null");
            return nullptr;
        }
        int32_t rowCount = -1;
        int32_t ret = resultSet->GetRowCount(rowCount);
        if (ret == E_SQLITE_CORRUPT) {
            HILOG_ERROR("database corrupt ret:%{public}d", ret);
            resultSet->Close();
            ret = store_->Restore("");
            if (ret != E_OK) {
                HILOG_ERROR("Restore failed ret:%{public}d", ret);
                return nullptr;
            }
            resultSet = store_->QueryByStep(sql, args);
        }
    }
    return resultSet;
}

bool RdbAdapter::GetRDBPtr()
{
    OpenCallback helper;
    RdbStoreConfig config(SYNCHRONOUS_ROOT_DATA_RDB_PATH + SYNCHRONOUS_ROOT_DATABASE_NAME);
    int32_t errCode = E_OK;
    {
        std::lock_guard<std::mutex> lock(rdbAdapterMtx_);
        store_ = RdbHelper::GetRdbStore(config, UFS_SYNCROOT_V2, helper, errCode);
        if (errCode != E_OK) {
            HILOG_ERROR("getRDBPtr failed errCode:%{public}d", errCode);
            return false;
        }
        if (store_ == nullptr) {
            HILOG_ERROR("RDBStore_ is null");
            return false;
        }
        NativeRdb::RebuiltType rebuiltType = NativeRdb::RebuiltType::NONE;
        errCode = store_->GetRebuilt(rebuiltType);
        if (errCode != E_OK) {
            HILOG_ERROR("GetRebuilt failed errCode:%{public}d", errCode);
            return false;
        }
        if (rebuiltType == NativeRdb::RebuiltType::REBUILT) {
            HILOG_ERROR("database corrupt");
            int32_t restoreRet = store_->Restore("");
            if (restoreRet != E_OK) {
                HILOG_ERROR("Restore failed restoreRet:%{public}d", restoreRet);
                return false;
            }
        }
    }
    return true;
}

bool RdbAdapter::CreateTable()
{
    std::lock_guard<std::mutex> lock(rdbAdapterMtx_);
    std::string sql = CREATE_SYNCHRONOUS_ROOT_TABLE_SQL;
    if (store_ == nullptr) {
        HILOG_ERROR("RDBStore_ is null");
        return false;
    }
    if (store_->ExecuteSql(sql) != E_OK) {
        HILOG_ERROR("rdbAdapter create table failed");
        return false;
    }
    return true;
}

int32_t OpenCallback::OnCreate(RdbStore& store)
{
    HILOG_INFO("rdbStore create");
    if (!CreateTable(store)) {
        HILOG_ERROR("CreateTable failed");
        return NativeRdb::E_ERROR;
    }
    HILOG_INFO("rdbStore create");
    return NativeRdb::E_OK;
}

 int32_t OpenCallback::OnUpgrade(RdbStore& store, int oldVersion, int newVersion)
{
    HILOG_INFO("rdbStore upgrade, oldVersion : %{public}d, newVersion : %{public}d", oldVersion, newVersion);
    if (oldVersion == UFS_SYNCROOT_V1 && newVersion > UFS_SYNCROOT_V1) {
        if (IsExistColumn(store, SYNCHRONOUS_ROOT_TABLE, IS_SUPPORT_PLACEHOLDER)) {
            HILOG_INFO("isSupportPlaceHolder column already exists, skip alter table");
        } else {
            if (store.ExecuteSql(ALTER_ADD_IS_SUPPORT_PLACEHOLDER_SQL) != NativeRdb::E_OK) {
                HILOG_ERROR("Failed to add isSupportPlaceHolder column");
                return NativeRdb::E_ERROR;
            }
        }
    }
    return NativeRdb::E_OK;
}

bool OpenCallback::IsExistColumn(RdbStore& store, const std::string& table, const std::string& column)
{
    std::string pragmSql = "PRAGMA table_info(" + table + ")";
    auto resultSet = store.QueryByStep(pragmSql);
    if (resultSet == nullptr) {
        HILOG_ERROR("QueryByStep PRAGMA table_info failed");
        return false;
    }
    int32_t columnCount = 0;
    if (resultSet->GetColumnCount(columnCount) != NativeRdb::E_OK) {
        HILOG_ERROR("GetColumnCount failed");
        resultSet->Close();
        return false;
    }
    int32_t nameIndex = -1;
    for (int32_t i = 0; i < columnCount; i++) {
        std::string columnName;
        if (resultSet->GetColumnName(i, columnName) == NativeRdb::E_OK && columnName == "name") {
            nameIndex = i;
            break;
        }
    }
    if (nameIndex < 0) {
        HILOG_ERROR("name column not found in PRAGMA table_info result");
        resultSet->Close();
        return false;
    }
    while (resultSet->GoToNextRow() == NativeRdb::E_OK) {
        std::string value;
        if (resultSet->GetString(nameIndex, value) == NativeRdb::E_OK && value == column) {
            resultSet->Close();
            return true;
        }
    }
    resultSet->Close();
    return false;
}

bool OpenCallback::CreateTable(RdbStore& store)
{
    std::lock_guard<std::mutex> lock(rdbStoreMtx_);
    if (store.ExecuteSql(CREATE_SYNCHRONOUS_ROOT_TABLE_SQL) != NativeRdb::E_OK) {
        HILOG_ERROR("synchronous_root_table create failed");
        return false;
    }
    return true;
}

} // namespace FileAccessFwk
} // namespace OHOS
