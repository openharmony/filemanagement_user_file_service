/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#ifndef UFS_MOCK_RDB_H
#define UFS_MOCK_RDB_H
#include "gmock/gmock.h"
#include "rdb_store.h"
#include "rdb_helper.h"
namespace OHOS::FileAccessFwk {
namespace MOC {
static std::shared_ptr<OHOS::NativeRdb::RdbStore> g_mockRdbStore = nullptr;
}
void MockGetRdbStore(std::shared_ptr<OHOS::NativeRdb::RdbStore> mockRdbStore)
{
    MOC::g_mockRdbStore = mockRdbStore;
}
void MockSetRdbStore()
{
    MOC::g_mockRdbStore = nullptr;
}
}

namespace OHOS::NativeRdb {
using namespace OHOS::FileAccessFwk;
inline std::shared_ptr<RdbStore> RdbHelper::GetRdbStore(
    const RdbStoreConfig& config, int version, RdbOpenCallback& openCallback, int& errCode)
{
    return MOC::g_mockRdbStore;
}
class MockResultSet : public AbsSharedResultSet {
public:
    MOCK_METHOD(int, GoToFirstRow, (), (override));
    MOCK_METHOD(int, GoToNextRow, (), (override));
    MOCK_METHOD(int, GetString, (int columnIndex, std::string& value), (override));
    MOCK_METHOD(int, GetInt, (int columnIndex, int& value), (override));
    MOCK_METHOD(int, Close, (), (override));
    MOCK_METHOD(int, GetColumnCount, (int& count), (override));
    MOCK_METHOD(int, GetRowCount, (int &count), (override));
};
class MockRdbStore : public RdbStore {
public:
    MockRdbStore() = default;
    MOCK_METHOD(int, ExecuteSql, (const std::string& sql, const std::vector<ValueObject>& bindArgs), (override));
    MOCK_METHOD(int, InsertWithConflictResolution,
        (int64_t & outRowId, const std::string& table, const ValuesBucket& values,
            ConflictResolution conflictResolution),
        (override));

    MOCK_METHOD(int, Delete,
        (int& deletedRows, const std::string& table, const std::string& whereClause,
            const std::vector<ValueObject>& bindArgs),
        (override));

    MOCK_METHOD(std::shared_ptr<AbsSharedResultSet>, Query,
        (const AbsRdbPredicates& predicates, const std::vector<std::string>& columns), (override));
    MOCK_METHOD(int, BeginTransaction, (), (override));
    MOCK_METHOD(int, RollBack, (), (override));
    MOCK_METHOD(int, Commit, (), (override));
    MOCK_METHOD(int, Insert, (int64_t& outRowId, const std::string& table,
        const ValuesBucket& initialValues), (override));
    MOCK_METHOD2(Restore, int(const std::string&, const std::vector<uint8_t>&));
    MOCK_METHOD(int, Update,
        (int& changedRows, const std::string& table, const ValuesBucket& values,
            const std::string& whereClause, const std::vector<ValueObject>& bindArgs),
        (override));
    MOCK_METHOD(std::shared_ptr<ResultSet>, QueryByStep,
        (const std::string& sql, const std::vector<ValueObject>& bindArgs, bool preCount),
        (override));
    ~MockRdbStore() override = default;

    int BatchInsert(
        int64_t& outInsertNum, const std::string& table, const std::vector<ValuesBucket>& initialBatchValues) override
    {
        return NativeRdb::E_ERROR;
    };
    int Replace(int64_t& outRowId, const std::string& table, const ValuesBucket& initialValues) override
    {
        return NativeRdb::E_ERROR;
    };

    int Update(int& changedRows, const std::string& table, const ValuesBucket& values, const std::string& whereClaus,
        const std::vector<std::string>& whereArgs) override
    {
        return NativeRdb::E_ERROR;
    };
    int UpdateWithConflictResolution(int& changedRows, const std::string& table, const ValuesBucket& values,
        const std::string& whereClause, const std::vector<std::string>& whereArgs,
        ConflictResolution conflictResolution) override
    {
        return NativeRdb::E_ERROR;
    };
    int UpdateWithConflictResolution(int& changedRows, const std::string& table, const ValuesBucket& values,
        const std::string& whereClause, const std::vector<ValueObject>& bindArgs,
        ConflictResolution conflictResolution) override
    {
        return NativeRdb::E_ERROR;
    };

    std::shared_ptr<AbsSharedResultSet> Query(int& errCode, bool distinct, const std::string& table,
        const std::vector<std::string>& columns, const std::string& whereClause,
        const std::vector<ValueObject>& bindArgs, const std::string& groupBy, const std::string& indexName,
        const std::string& orderBy, const int& limit, const int& offset) override
    {
        return nullptr;
    };
    std::shared_ptr<AbsSharedResultSet> QuerySql(
        const std::string& sql, const std::vector<std::string>& selectionArgs) override
    {
        return nullptr;
    };
    std::shared_ptr<AbsSharedResultSet> QuerySql(
        const std::string& sql, const std::vector<ValueObject>& selectionArgs) override
    {
        return nullptr;
    };
    std::shared_ptr<ResultSet> QueryByStep(
        const std::string& sql, const std::vector<std::string>& selectionArgs) override
    {
        return nullptr;
    };

    int ExecuteAndGetLong(int64_t& outValue, const std::string& sql, const std::vector<ValueObject>& bindArgs) override
    {
        return NativeRdb::E_ERROR;
    };
    int ExecuteAndGetString(
        std::string& outValue, const std::string& sql, const std::vector<ValueObject>& bindArgs) override
    {
        return NativeRdb::E_ERROR;
    };
    int ExecuteForLastInsertedRowId(
        int64_t& outValue, const std::string& sql, const std::vector<ValueObject>& bindArgs) override
    {
        return NativeRdb::E_ERROR;
    };
    int ExecuteForChangedRowCount(
        int64_t& outValue, const std::string& sql, const std::vector<ValueObject>& bindArgs) override
    {
        return NativeRdb::E_ERROR;
    };
    int Backup(const std::string& databasePath, const std::vector<uint8_t>& destEncryptKey) override
    {
        return NativeRdb::E_ERROR;
    };
    int Attach(
        const std::string& alias, const std::string& pathName, const std::vector<uint8_t> destEncryptKey) override
    {
        return NativeRdb::E_ERROR;
    };

    int Count(int64_t& outValue, const AbsRdbPredicates& predicates) override
    {
        return NativeRdb::E_ERROR;
    };

    std::shared_ptr<ResultSet> QueryByStep(
        const AbsRdbPredicates& predicates, const std::vector<std::string>& columns, bool preCount) override
    {
        return nullptr;
    };
    std::shared_ptr<ResultSet> RemoteQuery(const std::string& device, const AbsRdbPredicates& predicates,
        const std::vector<std::string>& columns, int& errCode) override
    {
        return nullptr;
    };
    int Update(int& changedRows, const ValuesBucket& values, const AbsRdbPredicates& predicates) override
    {
        return NativeRdb::E_ERROR;
    };

    virtual int GetStatus()
    {
        return NativeRdb::E_ERROR;
    };
    virtual void SetStatus(int status) {};
    int GetVersion(int& version) override
    {
        return NativeRdb::E_ERROR;
    };
    int SetVersion(int version) override
    {
        return NativeRdb::E_ERROR;
    };

    bool IsInTransaction() override
    {
        return false;
    };
    std::string GetPath() override
    {
        return "";
    }
    bool IsHoldingConnection() override
    {
        return false;
    };
    bool IsOpen() const override
    {
        return false;
    };
    bool IsReadOnly() const override
    {
        return false;
    };
    bool IsMemoryRdb() const override
    {
        return false;
    };
    virtual int ChangeDbFileForRestore(
        const std::string newPath, const std::string backupPath, const std::vector<uint8_t>& newKey)
    {
        return NativeRdb::E_ERROR;
    };

    int SetDistributedTables(const std::vector<std::string>& tables, int type,
        const DistributedRdb::DistributedConfig& distributedConfig) override
    {
        return E_ERROR;
    };

    std::string ObtainDistributedTableName(const std::string& device, const std::string& table, int& errCode) override
    {
        return "";
    }

    int Sync(const SyncOption& option, const AbsRdbPredicates& predicate, const AsyncBrief& async) override
    {
        return E_ERROR;
    };

    int Sync(const SyncOption& option, const AbsRdbPredicates& predicate, const AsyncDetail& async) override
    {
        return E_ERROR;
    };

    int Sync(const SyncOption& option, const std::vector<std::string>& tables, const AsyncDetail& async) override
    {
        return E_ERROR;
    };

    int Subscribe(const SubscribeOption& option, std::shared_ptr<RdbStoreObserver> observer) override
    {
        return E_ERROR;
    };

    int UnSubscribe(const SubscribeOption& option, std::shared_ptr<RdbStoreObserver> observer) override
    {
        return E_ERROR;
    };

    int RegisterAutoSyncCallback(std::shared_ptr<DetailProgressObserver> syncObserver) override
    {
        return E_ERROR;
    };

    int UnregisterAutoSyncCallback(std::shared_ptr<DetailProgressObserver> syncObserver) override
    {
        return E_ERROR;
    };

    int Notify(const std::string& event) override
    {
        return E_ERROR;
    }

    virtual bool DropDeviceData(const std::vector<std::string>& devices, const DropOption& option)
    {
        return false;
    };

    ModifyTime GetModifyTime(
        const std::string& table, const std::string& columnName, std::vector<PRIKey>& keys) override
    {
        return {};
    };

    int CleanDirtyData(const std::string& table, uint64_t cursor) override
    {
        return E_ERROR;
    };

    int GetRebuilt(RebuiltType& rebuilt) override
    {
        return E_OK;
    }
};

}
#endif  // UFS_MOCK_RDB_H
