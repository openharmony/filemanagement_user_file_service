/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include <string>
#include <memory>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "ufs_rdb_adapter.h"
#include "rdb_store_mock.h"


namespace OHOS {
namespace FileAccessFwk {
using namespace testing;
using namespace testing::ext;
using namespace OHOS::NativeRdb;

namespace {
    const std::string TAG = "rdbAdapterTest";
    const std::string SUCCESS_CREATE_TABLE_SQL = "CREATE TABLE IF NOT EXISTS synchronous_root_table \
    ( \
        path               TEXT PRIMARY KEY, \
        state              INTEGER, \
        displayName        TEXT, \
        displayNameResId   INTEGER, \
        bundleName         TEXT, \
        userId             INTEGER, \
        appIndex           INTEGER, \
        lastModifyTime     TEXT  \
    );";
    const std::string UN_SUCCESS_CREATE_TABLE_SQL = "NOT EXISTS \
    ( \
        path               TEXT PRIMARY KEY, \
        state              INTEGER, \
        displayName        TEXT, \
        displayNameResId   INTEGER, \
        bundleName         TEXT, \
        userId             INTEGER, \
        appIndex           INTEGER, \
        lastModifyTime     TEXT  \
    );";
    const std::string PATH = "path";
    const std::string STATE = "state";
    const std::string DISPLAY_NAME = "displayName";
    const std::string DISPLAY_NAME_RES_ID = "displayNameResId";
    const std::string BUNDLENAME = "bundleName";
    const std::string USERID = "userId";
    const std::string INDEX = "appIndex";
    const std::string LAST_MODIFY_TIME = "lastModifyTime";
}

class RdbAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static std::shared_ptr<RdbAdapter> store;
    static std::shared_ptr<MockRdbStore> mockStore; // 添加一个静态成员来保存 Mock 对象
    static std::mutex rdbAdapterTestMtx;
};

std::shared_ptr<RdbAdapter> RdbAdapterTest::store = nullptr;
std::shared_ptr<MockRdbStore> RdbAdapterTest::mockStore = nullptr;
std::mutex RdbAdapterTest::rdbAdapterTestMtx;

void RdbAdapterTest::SetUpTestCase(void)
{
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    store = std::make_shared<RdbAdapter>();
    mockStore = std::make_shared<MockRdbStore>();
    MockGetRdbStore(mockStore);
    store->store_ = mockStore;
}

void RdbAdapterTest::TearDownTestCase(void)
{
    MockSetRdbStore();
    store = nullptr;
    mockStore = nullptr;
}

void RdbAdapterTest::SetUp()
{
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    if (store->store_ == nullptr) {
        store->store_ = mockStore;
    }
}

void RdbAdapterTest::TearDown()
{
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    store->UnInit();
}

/**
 * @tc.name:
 * @tc.desc: UnInit success
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, UnInit001, TestSize.Level1)
{
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    auto errCode = store->UnInit();
    EXPECT_EQ(errCode, true);
}

/**
 * @tc.name:
 * @tc.desc: CreateTable success
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, CreateTable001, TestSize.Level1)
{
    EXPECT_CALL(*mockStore, ExecuteSql(_, _))
    .WillOnce(
        Return(0)
    );
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    auto errCode = store->CreateTable();
    EXPECT_EQ(errCode, true);
}

/**
 * @tc.name:
 * @tc.desc: CreateTable failed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, CreateTable002, TestSize.Level1)
{
    EXPECT_CALL(*mockStore, ExecuteSql(_, _))
    .WillOnce(
        Return(-1)
    );
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    auto errCode = store->CreateTable();
    EXPECT_EQ(errCode, false);
}

/**
 * @tc.name:
 * @tc.desc: CreateTable failed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, CreateTable003, TestSize.Level1)
{
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    store->UnInit();
    auto errCode = store->CreateTable();
    EXPECT_EQ(errCode, false);
}

/**
 * @tc.name:
 * @tc.desc: Put Success
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, Put001, TestSize.Level1)
{
    EXPECT_CALL(*mockStore, Insert(_, _, _))
    .WillOnce(
        Return(OHOS::NativeRdb::E_OK)
    );
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    int64_t outRowId = 0;
    std::string table = "synchronous_root_table";
    ValuesBucket values;
    values.PutString(BUNDLENAME, "bundleName");
    values.PutString(PATH, "path_");
    values.PutInt(STATE, 0);
    values.PutInt(DISPLAY_NAME_RES_ID, 123);
    values.PutInt(USERID, 100);
    values.PutInt(INDEX, 001);
    values.PutString("lastModifyTime", "currentTime");

    int32_t putErrCode = store->Put(outRowId, table, values);
    EXPECT_EQ(putErrCode, true);
}

/**
 * @tc.name:
 * @tc.desc: Put table does not exist failed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, Put002, TestSize.Level1)
{
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    int64_t outRowId = 0;
    std::string table = "trust_xxxdevice_table";
    ValuesBucket values;
    values.PutString(BUNDLENAME, "bundleName");
    values.PutString(PATH, "path_");
    values.PutInt(STATE, 0);
    values.PutInt(DISPLAY_NAME_RES_ID, 123);
    values.PutInt(USERID, 100);
    values.PutInt(INDEX, 001);
    values.PutString("lastModifyTime", "currentTime");
    int32_t putErrCode = store->Put(outRowId, table, values);
    EXPECT_EQ(putErrCode, false);
}

/**
 * @tc.name:
 * @tc.desc: Put failed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, Put003, TestSize.Level1)
{
    EXPECT_CALL(*mockStore, Insert(_, _, _))
    .WillOnce(
        Return(OHOS::NativeRdb::E_SQLITE_CORRUPT)
    );
    EXPECT_CALL(*mockStore, Restore(_, _))
    .WillOnce(
        Return(OHOS::NativeRdb::E_SQLITE_CORRUPT)
    );
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    int64_t outRowId1 = 0;
    std::string table = "synchronous_root_table";
    ValuesBucket values;
    values.PutString(BUNDLENAME, "bundleName");
    values.PutString(PATH, "path_");
    values.PutInt(STATE, 0);
    values.PutInt(DISPLAY_NAME_RES_ID, 123);
    values.PutInt(USERID, 100);
    values.PutInt(INDEX, 001);
    values.PutString("lastModifyTime", "currentTime");
    int32_t putErrCode1 = store->Put(outRowId1, table, values);
    EXPECT_EQ(putErrCode1, false);
}

/**
 * @tc.name:
 * @tc.desc: Put failed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, Put004, TestSize.Level1)
{
    EXPECT_CALL(*mockStore, Insert(_, _, _))
    .WillOnce(
        Return(OHOS::NativeRdb::E_SQLITE_CORRUPT)
    ).WillOnce(
        Return(OHOS::NativeRdb::E_OK)
    );;
    EXPECT_CALL(*mockStore, Restore(_, _))
    .WillOnce(
        Return(OHOS::NativeRdb::E_OK)
    );
    
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    int64_t outRowId1 = 0;
    std::string table = "synchronous_root_table";
    ValuesBucket values;
    values.PutString(BUNDLENAME, "bundleName");
    values.PutString(PATH, "path_");
    values.PutInt(STATE, 0);
    values.PutInt(DISPLAY_NAME_RES_ID, 123);
    values.PutInt(USERID, 100);
    values.PutInt(INDEX, 001);
    values.PutString("lastModifyTime", "currentTime");
    int32_t putErrCode1 = store->Put(outRowId1, table, values);
    EXPECT_EQ(putErrCode1, true);
}

/**
 * @tc.name: Put005
 * @tc.desc: Put failed, RDBStore_ is null.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, Put005, TestSize.Level1)
{
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    int64_t outRowId = 0;
    std::string table = "synchronous_root_table";
    ValuesBucket values;
    values.PutString(BUNDLENAME, "bundleName");
    values.PutString(PATH, "path_");
    values.PutInt(STATE, 0);
    values.PutInt(DISPLAY_NAME_RES_ID, 123);
    values.PutInt(USERID, 100);
    values.PutInt(INDEX, 001);
    values.PutString("lastModifyTime", "currentTime");
    store->UnInit();
    int32_t putErrCode = store->Put(outRowId, table, values);
    EXPECT_EQ(putErrCode, false);
    store->Init();
}

/**
 * @tc.name:
 * @tc.desc: Delete success
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, Delete001, TestSize.Level1)
{
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    std::string table = "synchronous_root_table";
    int32_t deleteRows = 0;
    std::string whereClause = "deviceId = ?";
    ValueObject valueObject(std::string("111aaa"));
    const std::vector<ValueObject>& bindArgs = {valueObject};

    EXPECT_CALL(*mockStore, Delete(
    ::testing::Ref(deleteRows),
    ::testing::Eq(table),
    ::testing::Eq(whereClause),
    ::testing::Eq(bindArgs)
    ))
    .WillOnce(Return(OHOS::NativeRdb::E_OK));

    int32_t deleteErrCode = store->Delete(deleteRows, table, whereClause, bindArgs);
    EXPECT_EQ(deleteErrCode, true);
}

/**
 * @tc.name:
 * @tc.desc: Delete table does not exist failed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, Delete002, TestSize.Level1)
{
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    int32_t deleteRows = 0;
    std::string deleteTable = "trust_xxxdevice_table";
    std::string whereClause = "deviceId = ?";
    ValueObject valueObject(std::string("111aaa"));
    const std::vector<ValueObject>& bindArgs = {valueObject};
    
    int32_t deleteErrCode = store->Delete(deleteRows, deleteTable, whereClause, bindArgs);
    EXPECT_EQ(deleteErrCode, false);
}

/**
 * @tc.name:
 * @tc.desc: Delete failed, RDBStore_ is null.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, Delete003, TestSize.Level1)
{
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    std::string table = "synchronous_root_table";
    int32_t deleteRows = 0;
    std::string deleteTable = "synchronous_root_table";
    std::string whereClause = "xxxdeviceId = ?";
    ValueObject valueObject(std::string("111aaa"));
    const std::vector<ValueObject>& bindArgs = {valueObject};
    store->UnInit();
    int32_t deleteErrCode = store->Delete(deleteRows, deleteTable, whereClause, bindArgs);
    EXPECT_EQ(deleteErrCode, false);
}

/**
 * @tc.name: Delete004
 * @tc.desc: Delete failed, Restore failed.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, Delete004, TestSize.Level1)
{
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    std::string table = "synchronous_root_table";
    int32_t deleteRows = 0;
    std::string whereClause = "deviceId = ?";
    ValueObject valueObject(std::string("111aaa"));
    const std::vector<ValueObject>& bindArgs = {valueObject};
    
    EXPECT_CALL(*mockStore, Delete(
    ::testing::Ref(deleteRows),
    ::testing::Eq(table),
    ::testing::Eq(whereClause),
    ::testing::Eq(bindArgs)
    ))
    .WillOnce(Return(OHOS::NativeRdb::E_SQLITE_CORRUPT));

    EXPECT_CALL(*mockStore, Restore(_, _))
    .WillOnce(
        Return(OHOS::NativeRdb::E_SQLITE_CORRUPT)
    );
    
    int32_t deleteErrCode = store->Delete(deleteRows, table, whereClause, bindArgs);
    EXPECT_EQ(deleteErrCode, false);
}

/**
 * @tc.name: Delete005
 * @tc.desc: Delete failed, Restore success.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, Delete005, TestSize.Level1)
{
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    std::string table = "synchronous_root_table";
    int32_t deleteRows = 0;
    std::string whereClause = "deviceId = ?";
    ValueObject valueObject(std::string("111aaa"));
    const std::vector<ValueObject>& bindArgs = {valueObject};

    EXPECT_CALL(*mockStore, Delete(
    ::testing::Ref(deleteRows),
    ::testing::Eq(table),
    ::testing::Eq(whereClause),
    ::testing::Eq(bindArgs)
    ))
    .WillOnce(Return(OHOS::NativeRdb::E_SQLITE_CORRUPT)).WillOnce(Return(OHOS::NativeRdb::E_OK));

    EXPECT_CALL(*mockStore, Restore(_, _))
    .WillOnce(
        Return(OHOS::NativeRdb::E_OK)
    );

    int32_t deleteErrCode = store->Delete(deleteRows, table, whereClause, bindArgs);
    EXPECT_EQ(deleteErrCode, true);
}

/**
 * @tc.name:
 * @tc.desc: Update success
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, Update001, TestSize.Level1)
{
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    std::string table = "synchronous_root_table";
    ValuesBucket newValues;
    newValues.Clear();
    newValues.PutString("deviceId", std::string("111aaa"));
    newValues.PutInt("deviceIdType", 3);
    newValues.PutString("deviceIdHash", std::string("222bbb"));
    newValues.PutInt("status", 2);

    int32_t changedRows = 0;
    std::string whereClause = "deviceId = ?";
    ValueObject valueObject(std::string("111aaa"));
    const std::vector<ValueObject>& bindArgs = {valueObject};

    EXPECT_CALL(*mockStore, Update(
    ::testing::Ref(changedRows),
    ::testing::Eq(table),
    _,
    ::testing::Eq(whereClause),
    ::testing::Eq(bindArgs)
    ))
    .WillOnce(DoAll(
        SetArgReferee<0>(1),
        Return(OHOS::NativeRdb::E_OK)
    ));
    
    auto updateErrCode = store->Update(changedRows, table, newValues, whereClause, bindArgs);
    EXPECT_EQ(updateErrCode, true);
}

/**
 * @tc.name:
 * @tc.desc: Update table does not exist failed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, Update002, TestSize.Level1)
{
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    std::string table = "synchXXXXus_root_table";
    ValuesBucket newValues;
    newValues.Clear();
    newValues.PutString("deviceId", std::string("111aaa"));
    newValues.PutInt("deviceIdType", 3);
    newValues.PutString("deviceIdHash", std::string("222bbb"));
    newValues.PutInt("status", 2);

    int32_t changedRows = 0;
    std::string whereClause = "deviceId = ?";
    ValueObject valueObject(std::string("111aaa"));
    const std::vector<ValueObject>& bindArgs = {valueObject};

    auto updateErrCode = store->Update(changedRows, table, newValues, whereClause, bindArgs);
    EXPECT_EQ(updateErrCode, false);
}

/**
 * @tc.name:
 * @tc.desc: Update failed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, Update003, TestSize.Level1)
{
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    std::string table = "synchronous_root_table";
    ValuesBucket newValues;
    newValues.Clear();
    newValues.PutString("deviceId", std::string("111aaa"));
    newValues.PutInt("deviceIdType", 3);
    newValues.PutString("deviceIdHash", std::string("222bbb"));
    newValues.PutInt("status", 2);

    int32_t changedRows = 0;
    std::string whereClause = "deviceId = ?";
    ValueObject valueObject(std::string("111aaa"));
    const std::vector<ValueObject>& bindArgs = {valueObject};

    EXPECT_CALL(*mockStore, Update(
    ::testing::Ref(changedRows),
    ::testing::Eq(table),
    _,
    ::testing::Eq(whereClause),
    ::testing::Eq(bindArgs)
    ))
    .WillOnce(DoAll(
        SetArgReferee<0>(1),
        Return(OHOS::NativeRdb::E_SQLITE_CORRUPT)
    ));
    
    EXPECT_CALL(*mockStore, Restore(_, _))
    .WillOnce(
        Return(OHOS::NativeRdb::E_SQLITE_CORRUPT)
    );
    auto updateErrCode = store->Update(changedRows, table, newValues, whereClause, bindArgs);
    EXPECT_EQ(updateErrCode, false);
}

/**
 * @tc.name:
 * @tc.desc: Update failed,retore success
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, Update004, TestSize.Level1)
{
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    std::string table = "synchronous_root_table";
    ValuesBucket newValues;
    newValues.Clear();
    newValues.PutString("deviceId", std::string("111aaa"));
    newValues.PutInt("deviceIdType", 3);
    newValues.PutString("deviceIdHash", std::string("222bbb"));
    newValues.PutInt("status", 2);

    int32_t changedRows = 0;
    std::string whereClause = "deviceId = ?";
    ValueObject valueObject(std::string("111aaa"));
    const std::vector<ValueObject>& bindArgs = {valueObject};

    EXPECT_CALL(*mockStore, Update(
    ::testing::Ref(changedRows),
    ::testing::Eq(table),
    _,
    ::testing::Eq(whereClause),
    ::testing::Eq(bindArgs)
    ))
    .WillOnce(DoAll(
        SetArgReferee<0>(1),
        Return(OHOS::NativeRdb::E_SQLITE_CORRUPT)
    )).WillOnce(DoAll(
        SetArgReferee<0>(1),
        Return(OHOS::NativeRdb::E_OK)
    ));
    
    EXPECT_CALL(*mockStore, Restore(_, _))
    .WillOnce(
        Return(OHOS::NativeRdb::E_OK)
    );

    auto updateErrCode = store->Update(changedRows, table, newValues, whereClause, bindArgs);
    EXPECT_EQ(updateErrCode, true);
}

/**
 * @tc.name: Update005
 * @tc.desc: Update failed, RDBStore_ is null.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, Update005, TestSize.Level1)
{
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    std::string table = "synchronous_root_table";
    ValuesBucket newValues;
    newValues.Clear();
    newValues.PutString("deviceId", std::string("111aaa"));
    newValues.PutInt("deviceIdType", 3);
    newValues.PutString("deviceIdHash", std::string("222bbb"));
    newValues.PutInt("status", 2);

    int32_t changedRows = 0;
    std::string whereClause = "deviceId = ?";
    ValueObject valueObject(std::string("111aaa"));
    const std::vector<ValueObject>& bindArgs = {valueObject};

    store->UnInit();
    int32_t updateErrCode = store->Update(changedRows, table, newValues, whereClause, bindArgs);
    EXPECT_EQ(updateErrCode, false);
}

/**
 * @tc.name:
 * @tc.desc: Get success
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, Get001, TestSize.Level1)
{
    std::string sql = "SELECT * FROM users WHERE id = ?";
    std::vector<ValueObject> bindArgs = { ValueObject(1) };

    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();

    EXPECT_CALL(*mockStore, QueryByStep(
        ::testing::Eq(sql),
        ::testing::Eq(bindArgs),
        _)).WillOnce(Return(mockResultSet));

    EXPECT_CALL(*mockResultSet, GetRowCount(_))
        .WillOnce(Return(OHOS::NativeRdb::E_OK));

    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    std::shared_ptr<ResultSet> resultSet = store->Get(sql, bindArgs);
    EXPECT_NE(resultSet, nullptr);
}

/**
 * @tc.name: Get002
 * @tc.desc: Get failed, RDBStore_ is null.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, Get002, TestSize.Level1)
{
    std::string sql = "SELECT * FROM users WHERE id = ?";
    std::vector<ValueObject> bindArgs = { ValueObject(1) };

    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();
    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    store->UnInit();
    std::shared_ptr<ResultSet> resultSet = store->Get(sql, bindArgs);
    EXPECT_EQ(resultSet, nullptr);
}

/**
 * @tc.name: Get003
 * @tc.desc: Get failed.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, Get003, TestSize.Level1)
{
    std::string sql = "SELECT * FROM users WHERE id = ?";
    std::vector<ValueObject> bindArgs = { ValueObject(1) };

    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();

    EXPECT_CALL(*mockStore, QueryByStep(
        ::testing::Eq(sql),
        ::testing::Eq(bindArgs),
        _)).WillOnce(Return(mockResultSet));

    EXPECT_CALL(*mockResultSet, GetRowCount(_))
        .WillOnce(Return(OHOS::NativeRdb::E_OK));

    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    std::shared_ptr<ResultSet> resultSet = store->Get(sql, bindArgs);
    EXPECT_EQ(resultSet, mockResultSet);
}

/**
 * @tc.name: Get004
 * @tc.desc: Get failed.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, Get004, TestSize.Level1)
{
    std::string sql = "SELECT * FROM users WHERE id = ?";
    std::vector<ValueObject> bindArgs = { ValueObject(1) };

    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();
    std::shared_ptr<MockResultSet> mockSecondResultSet = std::make_shared<MockResultSet>();

    EXPECT_CALL(*mockStore, QueryByStep(
        ::testing::Eq(sql),
        ::testing::Eq(bindArgs),
        _)).WillOnce(Return(mockResultSet));

    EXPECT_CALL(*mockResultSet, GetRowCount(_))
        .WillOnce(Return(OHOS::NativeRdb::E_SQLITE_CORRUPT));
    EXPECT_CALL(*mockStore, Restore(_, _))
    .WillOnce(
        Return(OHOS::NativeRdb::E_SQLITE_CORRUPT)
    );

    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    std::shared_ptr<ResultSet> resultSet = store->Get(sql, bindArgs);
    EXPECT_NE(resultSet, mockResultSet);
}

/**
 * @tc.name: Get005
 * @tc.desc: Get failed.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, Get005, TestSize.Level1)
{
    std::string sql = "SELECT * FROM users WHERE id = ?";
    std::vector<ValueObject> bindArgs = { ValueObject(1) };

    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();
    std::shared_ptr<MockResultSet> mockSecondResultSet = std::make_shared<MockResultSet>();

    EXPECT_CALL(*mockStore, QueryByStep(
        ::testing::Eq(sql),
        ::testing::Eq(bindArgs),
        _)).WillOnce(Return(mockResultSet)).WillOnce(Return(mockSecondResultSet));

    EXPECT_CALL(*mockResultSet, GetRowCount(_))
        .WillOnce(Return(OHOS::NativeRdb::E_SQLITE_CORRUPT));
    EXPECT_CALL(*mockStore, Restore(_, _))
    .WillOnce(
        Return(OHOS::NativeRdb::E_OK)
    );

    std::lock_guard<std::mutex> lock(rdbAdapterTestMtx);
    std::shared_ptr<ResultSet> resultSet = store->Get(sql, bindArgs);
    EXPECT_NE(resultSet, mockResultSet);
    EXPECT_EQ(resultSet, mockSecondResultSet);
}

/**
 * @tc.name: OnCreate001
 * @tc.desc: OnCreate failed.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, OnCreate001, TestSize.Level1)
{
    OpenCallback helper;
    EXPECT_CALL(*mockStore, ExecuteSql(_, _))
    .WillOnce(
        Return(NativeRdb::E_SQLITE_CORRUPT)
    );
    auto ret = helper.OnCreate(*(store->store_));
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: OnCreate002
 * @tc.desc: OnCreate success.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, OnCreate002, TestSize.Level1)
{
    OpenCallback helper;
    EXPECT_CALL(*mockStore, ExecuteSql(_, _))
    .WillOnce(
        Return(NativeRdb::E_OK)
    );
    auto ret = helper.OnCreate(*(store->store_));
    EXPECT_EQ(ret, NativeRdb::E_OK);
}

/**
 * @tc.name: OnUpgrade001
 * @tc.desc: OnCreate success.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RdbAdapterTest, OnUpgrade001, TestSize.Level1)
{
    OpenCallback helper;
    int oldVersion = 1;
    int newVersion = 2;
    auto ret = helper.OnUpgrade(*(store->store_), oldVersion, newVersion);
    EXPECT_EQ(ret, NativeRdb::E_OK);
}
} // namespace FileAccessFwk
} // namespace OHOS
