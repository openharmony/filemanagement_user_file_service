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

#ifndef OHOS_UFS_RDB_ADAPTER_H
#define OHOS_UFS_RDB_ADAPTER_H

#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include "rdb_helper.h"
#include "rdb_store.h"
#include "result_set.h"
#include <set>
#include "rdb_open_callback.h"

namespace OHOS {
namespace FileAccessFwk {

using namespace OHOS::NativeRdb;

class IRdbAdapter {
public:
    virtual ~IRdbAdapter() = default;
    
    virtual bool Init() = 0;
    virtual bool UnInit() = 0;
    virtual bool Put(int64_t& outRowId, const std::string& table, const NativeRdb::ValuesBucket& Values) = 0;
    virtual bool Delete(int32_t& deleteRows, const std::string& table, const std::string& whereClause,
        const std::vector<NativeRdb::ValueObject>& bindArgs) = 0;
    virtual bool Update(int32_t& changedRows, const std::string& table, const NativeRdb::ValuesBucket& values,
        const std::string& whereClause, const std::vector<NativeRdb::ValueObject>& bindArgs) = 0;
    virtual bool CreateTable() = 0;
    virtual std::shared_ptr<NativeRdb::ResultSet> Get(const std::string& sql,
        const std::vector<NativeRdb::ValueObject>& args) = 0;
    virtual bool GetRDBPtr() = 0;
};

class RdbAdapter : public IRdbAdapter {
public:
    RdbAdapter() = default;
    ~RdbAdapter() override = default;
    
    bool Init() override;
    bool UnInit() override;
    bool Put(int64_t& outRowId, const std::string& table, const NativeRdb::ValuesBucket& Values) override;
    bool Delete(int32_t& deleteRows, const std::string& table, const std::string& whereClause,
        const std::vector<NativeRdb::ValueObject>& bindArgs = {}) override;
    bool Update(int32_t& changedRows, const std::string& table, const NativeRdb::ValuesBucket& values,
        const std::string& whereClause, const std::vector<NativeRdb::ValueObject>& bindArgs = {}) override;
    bool CreateTable() override;
    std::shared_ptr<NativeRdb::ResultSet> Get(const std::string& sql,
        const std::vector<NativeRdb::ValueObject>& args = {}) override;
    bool GetRDBPtr() override;

private:
    std::shared_ptr<RdbStore> store_ = nullptr;
    std::mutex rdbAdapterMtx_;
};

class OpenCallback : public NativeRdb::RdbOpenCallback {
public:
    int32_t OnCreate(RdbStore& store) override;
    int32_t OnUpgrade(RdbStore& store, int oldVersion, int newVersion) override;
    bool CreateTable(RdbStore& store);
    bool CreateUniqueIndex(RdbStore& store);
private:
    std::mutex rdbStoreMtx_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // OHOS_UFS_RDB_ADAPTER_H
