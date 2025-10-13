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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>
#include <memory>
#include <vector>

#include "cloud_disk_synchronous_root_manager.h"
#include "rdb_store_mock.h"
#include "rdb_helper.h"
#include "ufs_rdb_adapter.h"
#include "cloud_disk_comm.h"
#include "file_access_framework_errno.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::NativeRdb;

namespace OHOS {
namespace FileAccessFwk {

class MockRdbAdapter : public IRdbAdapter {
public:
    MOCK_METHOD(bool, Init, (), (override));
    MOCK_METHOD(bool, UnInit, (), (override));
    MOCK_METHOD(bool, Put, (int64_t& outRowId, const std::string& table, const ValuesBucket& Values), (override));
    MOCK_METHOD(bool, Delete, (int32_t& deleteRows, const std::string& table, const std::string& whereClause,
        const std::vector<ValueObject>& bindArgs), (override));
    MOCK_METHOD(bool, Update, (int32_t& changedRows, const std::string& table, const ValuesBucket& values,
        const std::string& whereClause, const std::vector<ValueObject>& bindArgs), (override));
    MOCK_METHOD(bool, CreateTable, (), (override));
    MOCK_METHOD(std::shared_ptr<ResultSet>, Get, (const std::string& sql,
        const std::vector<ValueObject>& args), (override));
    MOCK_METHOD(bool, GetRDBPtr, (), (override));
};

class MockResultSet : public AbsSharedResultSet {
public:
    MOCK_METHOD(int, GoToFirstRow, (), (override));
    MOCK_METHOD(int, GoToNextRow, (), (override));
    MOCK_METHOD(int, GetString, (int columnIndex, std::string& value), (override));
    MOCK_METHOD(int, GetInt, (int columnIndex, int& value), (override));
    MOCK_METHOD(int, Close, (), (override));
    MOCK_METHOD(int, GetColumnCount, (int& count), (override));
    MOCK_METHOD(int, GetRow, (RowEntity& rowEntity), (override));
    MOCK_METHOD(int, GetRowCount, (int &count), (override));
};

class CloudDiskSynchronousRootManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static std::shared_ptr<MockRdbAdapter> mockRdbAdapter;
};

std::shared_ptr<MockRdbAdapter> CloudDiskSynchronousRootManagerTest::mockRdbAdapter = nullptr;

void CloudDiskSynchronousRootManagerTest::SetUpTestCase(void)
{
    mockRdbAdapter = std::make_shared<MockRdbAdapter>();
}

void CloudDiskSynchronousRootManagerTest::TearDownTestCase(void)
{
    mockRdbAdapter = nullptr;
}

void CloudDiskSynchronousRootManagerTest::SetUp()
{
    // 重置mock对象
    ::testing::Mock::VerifyAndClearExpectations(mockRdbAdapter.get());
}

void CloudDiskSynchronousRootManagerTest::TearDown()
{
    // 测试结束时清理
    ::testing::Mock::VerifyAndClearExpectations(mockRdbAdapter.get());
}

/**
 * @tc.name: Init_001
 * @tc.desc: Test Init success case.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, Init_001, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    EXPECT_CALL(*mockRdbAdapter, Init())
        .WillOnce(Return(true));
    EXPECT_CALL(*mockRdbAdapter, CreateTable())
        .WillOnce(Return(true));
    
    EXPECT_TRUE(manager.Init());
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: Init_002
 * @tc.desc: Test Init failed when rdbStore_ is nullptr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, Init_002, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = nullptr;
    EXPECT_FALSE(manager.Init());
}

/**
 * @tc.name: Init_003
 * @tc.desc: Test Init failed when RdbAdapter Init returns false.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, Init_003, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    EXPECT_CALL(*mockRdbAdapter, Init())
        .WillOnce(Return(false));

    EXPECT_FALSE(manager.Init());
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: Init_004
 * @tc.desc: Test Init failed when CreateTable returns false.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, Init_004, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    EXPECT_CALL(*mockRdbAdapter, Init())
        .WillOnce(Return(true));
    EXPECT_CALL(*mockRdbAdapter, CreateTable())
        .WillOnce(Return(false));
    
    EXPECT_FALSE(manager.Init());
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: PutSynchronousRoot_001
 * @tc.desc: Test PutSynchronousRoot success case.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, PutSynchronousRoot_001, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    SyncFolder rootInfo;
    rootInfo.path_ = "/test/path";
    rootInfo.displayName_ = "TestFolder";
    rootInfo.displayNameResId_ = 1001;
    
    std::string bundleName = "com.test.app";
    int32_t userId = 100;
    int32_t index = 0;
    
    EXPECT_CALL(*mockRdbAdapter, Put(_, _, _))
        .WillOnce(Return(true));

    bool result = manager.PutSynchronousRoot(rootInfo, bundleName, userId, index);
    EXPECT_TRUE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: PutSynchronousRoot_002
 * @tc.desc: Test PutSynchronousRoot failed when rdbStore_ is nullptr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, PutSynchronousRoot_002, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = nullptr;
    SyncFolder rootInfo;
    rootInfo.path_ = "/test/path";
    rootInfo.displayName_ = "TestFolder";
    rootInfo.displayNameResId_ = 1001;
    
    std::string bundleName = "com.test.app";
    int32_t userId = 100;
    int32_t index = 0;
    
    bool result = manager.PutSynchronousRoot(rootInfo, bundleName, userId, index);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: PutSynchronousRoot_003
 * @tc.desc: Test PutSynchronousRoot failed when Put returns false.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, PutSynchronousRoot_003, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    SyncFolder rootInfo;
    rootInfo.path_ = "/test/path";
    rootInfo.displayName_ = "TestFolder";
    rootInfo.displayNameResId_ = 1001;
    
    std::string bundleName = "com.test.app";
    int32_t userId = 100;
    int32_t index = 0;
    
    EXPECT_CALL(*mockRdbAdapter, Put(_, _, _))
        .WillOnce(Return(false));

    bool result = manager.PutSynchronousRoot(rootInfo, bundleName, userId, index);
    EXPECT_FALSE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: DeleteSynchronousRoot_001
 * @tc.desc: Test DeleteSynchronousRoot success case.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, DeleteSynchronousRoot_001, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string path = "/test/path";
    int32_t userId = 100;
    
    EXPECT_CALL(*mockRdbAdapter, Delete(_, _, _, _))
        .WillOnce(Return(true));
    
    bool result = manager.DeleteSynchronousRoot(path, userId);
    EXPECT_TRUE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: DeleteSynchronousRoot_002
 * @tc.desc: Test DeleteSynchronousRoot failed when rdbStore_ is nullptr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, DeleteSynchronousRoot_002, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = nullptr;
    std::string path = "/test/path";
    int32_t userId = 100;
    bool result = manager.DeleteSynchronousRoot(path, userId);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: DeleteSynchronousRoot_003
 * @tc.desc: Test DeleteSynchronousRoot failed when Delete returns false.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, DeleteSynchronousRoot_003, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string path = "/test/path";
    int32_t userId = 100;
    
    EXPECT_CALL(*mockRdbAdapter, Delete(_, _, _, _))
        .WillOnce(Return(false));
    
    bool result = manager.DeleteSynchronousRoot(path, userId);
    EXPECT_FALSE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: DeleteAllSynRootsByUserAndBundle_001
 * @tc.desc: Test DeleteAllSynRootsByUserAndBundle success case.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, DeleteAllSynRootsByUserAndBundle_001, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string bundleName = "com.test.app";
    int32_t userId = 100;
    int32_t index = 0;
    
    EXPECT_CALL(*mockRdbAdapter, Delete(_, _, _, _))
        .WillOnce(Return(true));
    
    bool result = manager.DeleteAllSynRootsByUserAndBundle(bundleName, userId, index);
    EXPECT_TRUE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: DeleteAllSynRootsByUserAndBundle_002
 * @tc.desc: Test DeleteAllSynRootsByUserAndBundle failed when rdbStore_ is nullptr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, DeleteAllSynRootsByUserAndBundle_002, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = nullptr;
    std::string bundleName = "com.test.app";
    int32_t userId = 100;
    int32_t index = 0;
    bool result = manager.DeleteAllSynRootsByUserAndBundle(bundleName, userId, index);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: DeleteAllSynRootsByUserAndBundle_003
 * @tc.desc: Test DeleteAllSynRootsByUserAndBundle failed when Delete returns false.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, DeleteAllSynRootsByUserAndBundle_003, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string bundleName = "com.test.app";
    int32_t userId = 100;
    int32_t index = 0;
    
    EXPECT_CALL(*mockRdbAdapter, Delete(_, _, _, _))
        .WillOnce(Return(false));
    
    bool result = manager.DeleteAllSynRootsByUserAndBundle(bundleName, userId, index);
    EXPECT_FALSE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: UpdateSynchronousRootState_001
 * @tc.desc: Test UpdateSynchronousRootState success case.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, UpdateSynchronousRootState_001, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string path = "/test/path";
    std::string bundleName = "com.test.app";
    int32_t userId = 100;
    int32_t index = 0;
    State newState = State::ACTIVE;
    
    // 创建mock结果集
    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();
    
    // 模拟GetSynchronousRootByPathAndUserId调用
    EXPECT_CALL(*mockRdbAdapter, Get(_, _))
        .WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GetRowCount(_))
        .WillOnce(DoAll(SetArgReferee<0>(1), Return(E_OK)));
    EXPECT_CALL(*mockResultSet, GoToFirstRow())
        .WillOnce(Return(E_OK));
    EXPECT_CALL(*mockResultSet, GetRow(_))
        .WillOnce(Return(E_OK));

    
    EXPECT_CALL(*mockRdbAdapter, Update(_, _, _, _, _))
        .WillOnce(DoAll(SetArgReferee<0>(1), Return(true)));
    
    bool result = manager.UpdateSynchronousRootState(path, bundleName, userId, index, newState);
    EXPECT_TRUE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: UpdateSynchronousRootState_002
 * @tc.desc: Test UpdateSynchronousRootState failed when GetSynchronousRootByPathAndUserId fails.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, UpdateSynchronousRootState_002, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string path = "/test/path";
    std::string bundleName = "com.test.app";
    int32_t userId = 100;
    int32_t index = 0;
    State newState = State::ACTIVE;
    
    // 模拟GetSynchronousRootByPathAndUserId调用
    EXPECT_CALL(*mockRdbAdapter, Get(_, _))
        .WillOnce(Return(nullptr));
    
    bool result = manager.UpdateSynchronousRootState(path, bundleName, userId, index, newState);
    EXPECT_FALSE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: UpdateSynchronousRootState_003
 * @tc.desc: Test UpdateSynchronousRootState failed when Update returns false.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, UpdateSynchronousRootState_003, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string path = "/test/path";
    std::string bundleName = "com.test.app";
    int32_t userId = 100;
    int32_t index = 0;
    State newState = State::ACTIVE;
    
    // 创建mock结果集
    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();
    
    // 模拟GetSynchronousRootByPathAndUserId调用
    EXPECT_CALL(*mockRdbAdapter, Get(_, _))
        .WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GetRowCount(_))
        .WillOnce(DoAll(SetArgReferee<0>(1), Return(E_OK)));
    EXPECT_CALL(*mockResultSet, GoToFirstRow())
        .WillOnce(Return(E_OK));
    EXPECT_CALL(*mockResultSet, GetRow(_))
        .WillOnce(Return(E_OK));

    
    EXPECT_CALL(*mockRdbAdapter, Update(_, _, _, _, _))
        .WillOnce(Return(false));
    
    bool result = manager.UpdateSynchronousRootState(path, bundleName, userId, index, newState);
    EXPECT_FALSE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: UpdateSynchronousRootState_004
 * @tc.desc: Test UpdateSynchronousRootState failed when changed rows is not 1.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, UpdateSynchronousRootState_004, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string path = "/test/path";
    std::string bundleName = "com.test.app";
    int32_t userId = 100;
    int32_t index = 0;
    State newState = State::ACTIVE;
    
    // 创建mock结果集
    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();
    
    // 模拟GetSynchronousRootByPathAndUserId调用
    EXPECT_CALL(*mockRdbAdapter, Get(_, _))
        .WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GetRowCount(_))
        .WillOnce(DoAll(SetArgReferee<0>(1), Return(E_OK)));
    EXPECT_CALL(*mockResultSet, GoToFirstRow())
        .WillOnce(Return(E_OK));
    EXPECT_CALL(*mockResultSet, GetRow(_))
        .WillOnce(Return(E_OK));

    
    EXPECT_CALL(*mockRdbAdapter, Update(_, _, _, _, _))
        .WillOnce(DoAll(SetArgReferee<0>(2), Return(true)));
    
    bool result = manager.UpdateSynchronousRootState(path, bundleName, userId, index, newState);
    EXPECT_FALSE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: UpdateDisplayName_001
 * @tc.desc: Test UpdateDisplayName success case.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, UpdateDisplayName_001, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string path = "/test/path";
    std::string bundleName = "com.test.app";
    int32_t userId = 100;
    int32_t index = 0;
    std::string displayName = "NewDisplayName";
    
    // 创建mock结果集
    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();
    
    // 模拟GetSynchronousRootByPathAndUserId调用
    EXPECT_CALL(*mockRdbAdapter, Get(_, _))
        .WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GetRowCount(_))
        .WillOnce(DoAll(SetArgReferee<0>(1), Return(E_OK)));
    EXPECT_CALL(*mockResultSet, GoToFirstRow())
        .WillOnce(Return(E_OK));
    EXPECT_CALL(*mockResultSet, GetRow(_))
        .WillOnce(Return(E_OK));

    
    // 模拟Update操作成功
    EXPECT_CALL(*mockRdbAdapter, Update(_, _, _, _, _))
        .WillOnce(DoAll(SetArgReferee<0>(1), Return(true)));

    bool result = manager.UpdateDisplayName(path, bundleName, userId, index, displayName);
    EXPECT_TRUE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: UpdateDisplayName_002
 * @tc.desc: Test UpdateDisplayName failed when GetSynchronousRootByPathAndUserId fails.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, UpdateDisplayName_002, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string path = "/test/path";
    std::string bundleName = "com.test.app";
    int32_t userId = 100;
    int32_t index = 0;
    std::string displayName = "NewDisplayName";
    
    // 模拟GetSynchronousRootByPathAndUserId调用
    EXPECT_CALL(*mockRdbAdapter, Get(_, _))
        .WillOnce(Return(nullptr));
    
    bool result = manager.UpdateDisplayName(path, bundleName, userId, index, displayName);
    EXPECT_FALSE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: UpdateDisplayName_003
 * @tc.desc: Test UpdateDisplayName failed when Update returns false.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, UpdateDisplayName_003, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string path = "/test/path";
    std::string bundleName = "com.test.app";
    int32_t userId = 100;
    int32_t index = 0;
    std::string displayName = "NewDisplayName";
    
    // 创建mock结果集
    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();
    
    // 模拟GetSynchronousRootByPathAndUserId调用
    EXPECT_CALL(*mockRdbAdapter, Get(_, _))
        .WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GetRowCount(_))
        .WillOnce(DoAll(SetArgReferee<0>(1), Return(E_OK)));
    EXPECT_CALL(*mockResultSet, GoToFirstRow())
        .WillOnce(Return(E_OK));
    EXPECT_CALL(*mockResultSet, GetRow(_))
        .WillOnce(Return(E_OK));

    
    EXPECT_CALL(*mockRdbAdapter, Update(_, _, _, _, _))
        .WillOnce(Return(false));
    
    bool result = manager.UpdateDisplayName(path, bundleName, userId, index, displayName);
    EXPECT_FALSE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: UpdateDisplayName_004
 * @tc.desc: Test UpdateDisplayName failed when changed rows is not 1.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, UpdateDisplayName_004, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string path = "/test/path";
    std::string bundleName = "com.test.app";
    int32_t userId = 100;
    int32_t index = 0;
    std::string displayName = "NewDisplayName";
    
    // 创建mock结果集
    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();
    
    // 模拟GetSynchronousRootByPathAndUserId调用
    EXPECT_CALL(*mockRdbAdapter, Get(_, _))
        .WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GetRowCount(_))
        .WillOnce(DoAll(SetArgReferee<0>(1), Return(E_OK)));
    EXPECT_CALL(*mockResultSet, GoToFirstRow())
        .WillOnce(Return(E_OK));
    EXPECT_CALL(*mockResultSet, GetRow(_))
        .WillOnce(Return(E_OK));

    
    EXPECT_CALL(*mockRdbAdapter, Update(_, _, _, _, _))
        .WillOnce(DoAll(SetArgReferee<0>(2), Return(true)));
    
    bool result = manager.UpdateDisplayName(path, bundleName, userId, index, displayName);
    EXPECT_FALSE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: GetRootInfosByUserAndBundle_001
 * @tc.desc: Test GetRootInfosByUserAndBundle success case with data.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, GetRootInfosByUserAndBundle_001, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string bundleName = "com.test.app";
    int32_t userId = 100;
    int32_t index = 0;
    std::vector<SyncFolder> syncFolders;
    
    // 创建mock结果集
    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();
    
    // 模拟Get调用
    EXPECT_CALL(*mockRdbAdapter, Get(_, _))
        .WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GetRowCount(_))
        .WillOnce(DoAll(SetArgReferee<0>(1), Return(E_OK)));
    EXPECT_CALL(*mockResultSet, GoToFirstRow())
        .WillOnce(Return(E_OK));
    EXPECT_CALL(*mockResultSet, GetRow(_))
        .WillOnce(Return(E_OK));
    EXPECT_CALL(*mockResultSet, GoToNextRow())
        .WillOnce(Return(-1));
    
    bool result = manager.GetRootInfosByUserAndBundle(bundleName, index, userId, syncFolders);
    EXPECT_TRUE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: GetRootInfosByUserAndBundle_002
 * @tc.desc: Test GetRootInfosByUserAndBundle success case with no data.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, GetRootInfosByUserAndBundle_002, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string bundleName = "com.test.app";
    int32_t userId = 100;
    int32_t index = 0;
    std::vector<SyncFolder> syncFolders;
    
    // 创建mock结果集
    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();
    
    // 模拟Get调用，但没有数据
    EXPECT_CALL(*mockRdbAdapter, Get(_, _))
        .WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GetRowCount(_))
        .WillOnce(DoAll(SetArgReferee<0>(0), Return(E_OK)));

    
    // 需要通过依赖注入设置mockRdbAdapter才能进行此测试
    bool result = manager.GetRootInfosByUserAndBundle(bundleName, index, userId, syncFolders);
    EXPECT_TRUE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: GetRootInfosByUserAndBundle_003
 * @tc.desc: Test GetRootInfosByUserAndBundle failed when rdbStore_ is nullptr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, GetRootInfosByUserAndBundle_003, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = nullptr;
    std::string bundleName = "com.test.app";
    int32_t userId = 100;
    int32_t index = 0;
    std::vector<SyncFolder> syncFolders;
    bool result = manager.GetRootInfosByUserAndBundle(bundleName, index, userId, syncFolders);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: GetRootInfosByUserAndBundle_004
 * @tc.desc: Test GetRootInfosByUserAndBundle failed when ResultSet is nullptr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, GetRootInfosByUserAndBundle_004, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string bundleName = "com.test.app";
    int32_t userId = 100;
    int32_t index = 0;
    std::vector<SyncFolder> syncFolders;
    
    EXPECT_CALL(*mockRdbAdapter, Get(_, _))
        .WillOnce(Return(nullptr));
    
    bool result = manager.GetRootInfosByUserAndBundle(bundleName, index, userId, syncFolders);
    EXPECT_FALSE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: GetAllRootInfosByUserId_001
 * @tc.desc: Test GetAllRootInfosByUserId success case.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, GetAllRootInfosByUserId_001, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    int32_t userId = 100;
    std::vector<SyncFolderExt> syncFolderExts;
    
    // 创建mock结果集
    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();
    
    // 模拟Get调用
    EXPECT_CALL(*mockRdbAdapter, Get(_, _))
        .WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow())
        .WillOnce(Return(E_OK));
    EXPECT_CALL(*mockResultSet, GetRow(_))
        .WillOnce(Return(E_OK));
    EXPECT_CALL(*mockResultSet, GoToNextRow())
        .WillOnce(Return(-1));

    
    bool result = manager.GetAllRootInfosByUserId(userId, syncFolderExts);
    EXPECT_TRUE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: GetAllRootInfosByUserId_002
 * @tc.desc: Test GetAllRootInfosByUserId failed when rdbStore_ is nullptr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, GetAllRootInfosByUserId_002, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = nullptr;
    int32_t userId = 100;
    std::vector<SyncFolderExt> syncFolderExts;
    bool result = manager.GetAllRootInfosByUserId(userId, syncFolderExts);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: GetAllRootInfosByUserId_003
 * @tc.desc: Test GetAllRootInfosByUserId failed when ResultSet is nullptr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, GetAllRootInfosByUserId_003, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    int32_t userId = 100;
    std::vector<SyncFolderExt> syncFolderExts;
    
    EXPECT_CALL(*mockRdbAdapter, Get(_, _))
        .WillOnce(Return(nullptr));
    
    bool result = manager.GetAllRootInfosByUserId(userId, syncFolderExts);
    EXPECT_FALSE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: GetRootNumByUserIdAndBundleName_001
 * @tc.desc: Test GetRootNumByUserIdAndBundleName success case.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, GetRootNumByUserIdAndBundleName_001, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string bundleName = "com.test.app";
    int32_t userId = 100;
    int32_t index = 0;
    
    // 创建mock结果集
    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();
    
    // 模拟Get调用
    EXPECT_CALL(*mockRdbAdapter, Get(_, _))
        .WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GetRowCount(_))
        .WillOnce(DoAll(SetArgReferee<0>(5), Return(E_OK)));

    
    // 需要通过依赖注入设置mockRdbAdapter才能进行此测试
    int32_t result = manager.GetRootNumByUserIdAndBundleName(bundleName, index, userId);
    EXPECT_EQ(result, 5);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: GetRootNumByUserIdAndBundleName_002
 * @tc.desc: Test GetRootNumByUserIdAndBundleName failed when rdbStore_ is nullptr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, GetRootNumByUserIdAndBundleName_002, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = nullptr;
    std::string bundleName = "com.test.app";
    int32_t userId = 100;
    int32_t index = 0;
    auto result = manager.GetRootNumByUserIdAndBundleName(bundleName, index, userId);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: GetRootNumByUserIdAndBundleName_003
 * @tc.desc: Test GetRootNumByUserIdAndBundleName failed when ResultSet is nullptr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, GetRootNumByUserIdAndBundleName_003, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string bundleName = "com.test.app";
    int32_t userId = 100;
    int32_t index = 0;
    
    // 模拟Get调用返回nullptr
    EXPECT_CALL(*mockRdbAdapter, Get(_, _))
        .WillOnce(Return(nullptr));
    
    // 需要通过依赖注入设置mockRdbAdapter才能进行此测试
    int32_t result = manager.GetRootNumByUserIdAndBundleName(bundleName, index, userId);
    EXPECT_EQ(result, -1);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: GetRootCount_001
 * @tc.desc: Test GetRootCount success case.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, GetRootCount_001, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    
    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();
    EXPECT_CALL(*mockRdbAdapter, Get(_, _))
        .WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GetRowCount(_))
        .WillOnce(DoAll(SetArgReferee<0>(10), Return(E_OK)));

    int32_t result = manager.GetRootCount();
    EXPECT_EQ(result, 10);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: GetRootCount_002
 * @tc.desc: Test GetRootCount failed when rdbStore_ is nullptr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, GetRootCount_002, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = nullptr;
    int32_t result = manager.GetRootCount();
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: GetRootCount_003
 * @tc.desc: Test GetRootCount failed when ResultSet is nullptr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, GetRootCount_003, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    
    EXPECT_CALL(*mockRdbAdapter, Get(_, _))
        .WillOnce(Return(nullptr));
    
    int32_t result = manager.GetRootCount();
    EXPECT_EQ(result, -1);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: CheckExistParentOrChildRoot_001
 * @tc.desc: Test CheckExistParentOrChildRoot success case with no conflicts.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, CheckExistParentOrChildRoot_001, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string path = "/test/path";
    std::string bundleName = "com.test.app";
    int userId = 100;
    
    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();
    
    EXPECT_CALL(*mockRdbAdapter, Get(_, _))
        .WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GetRowCount(_))
        .WillOnce(DoAll(SetArgReferee<0>(0), Return(E_OK)));

    int32_t result = manager.CheckExistParentOrChildRoot(path, bundleName, userId);
    EXPECT_EQ(result, ERR_OK);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: CheckExistParentOrChildRoot_002
 * @tc.desc: Test CheckExistParentOrChildRoot failed when rdbStore_ is nullptr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, CheckExistParentOrChildRoot_002, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = nullptr;
    std::string path = "/test/path";
    std::string bundleName = "com.test.app";
    int userId = 100;
    
    int32_t result = manager.CheckExistParentOrChildRoot(path, bundleName, userId);
    EXPECT_EQ(result, E_PERMISSION);
}

/**
 * @tc.name: CheckExistParentOrChildRoot_003
 * @tc.desc: Test CheckExistParentOrChildRoot failed when ResultSet is nullptr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, CheckExistParentOrChildRoot_003, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string path = "/test/path";
    std::string bundleName = "com.test.app";
    int userId = 100;
    
    EXPECT_CALL(*mockRdbAdapter, Get(_, _))
        .WillOnce(Return(nullptr));
    
    int32_t result = manager.CheckExistParentOrChildRoot(path, bundleName, userId);
    EXPECT_EQ(result, E_PERMISSION);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: CheckExistParentOrChildRoot_004
 * @tc.desc: Test CheckExistParentOrChildRoot failed when ResultSet GetRowCount is 0.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, CheckExistParentOrChildRoot_004, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string path = "/test/path";
    std::string bundleName = "com.test.app";
    int userId = 100;
    
    std::shared_ptr<MockResultSet> mockResultSet = std::make_shared<MockResultSet>();
    EXPECT_CALL(*mockRdbAdapter, Get(_, _))
        .WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GetRowCount(_))
        .WillOnce(DoAll(SetArgReferee<0>(0), Return(E_OK)));

    int32_t result = manager.CheckExistParentOrChildRoot(path, bundleName, userId);
    EXPECT_EQ(result, ERR_OK);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: IsParentOrChildPath_001
 * @tc.desc: Test IsParentOrChildPath with identical paths.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, IsParentOrChildPath_001, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string path1 = "/test/path";
    std::string path2 = "/test/path";
    
    bool result = manager.IsParentOrChildPath(path1, path2);
    EXPECT_TRUE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: IsParentOrChildPath_002
 * @tc.desc: Test IsParentOrChildPath with parent-child paths.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, IsParentOrChildPath_002, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string path1 = "/test";
    std::string path2 = "/test/path";
    
    bool result = manager.IsParentOrChildPath(path1, path2);
    EXPECT_TRUE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: IsParentOrChildPath_003
 * @tc.desc: Test IsParentOrChildPath with child-parent paths.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, IsParentOrChildPath_003, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string path1 = "/test/path";
    std::string path2 = "/test";
    
    bool result = manager.IsParentOrChildPath(path1, path2);
    EXPECT_TRUE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: IsParentOrChildPath_004
 * @tc.desc: Test IsParentOrChildPath with unrelated paths.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, IsParentOrChildPath_004, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string path1 = "/test1/path";
    std::string path2 = "/test2/path";
    
    bool result = manager.IsParentOrChildPath(path1, path2);
    EXPECT_FALSE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: IsParentOrChildPath_005
 * @tc.desc: Test IsParentOrChildPath with empty paths.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, IsParentOrChildPath_005, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string path1 = "";
    std::string path2 = "/test/path";
    
    bool result = manager.IsParentOrChildPath(path1, path2);
    EXPECT_FALSE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: IsPathAllowed_001
 * @tc.desc: Test IsPathAllowed with valid path.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, IsPathAllowed_001, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string path = "/storage/Users/currentUser/test";
    
    bool result = manager.IsPathAllowed(path);
    EXPECT_TRUE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: IsPathAllowed_002
 * @tc.desc: Test IsPathAllowed with invalid base directory.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, IsPathAllowed_002, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string path = "/invalid/path";
    
    bool result = manager.IsPathAllowed(path);
    EXPECT_FALSE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: IsPathAllowed_003
 * @tc.desc: Test IsPathAllowed with blacklisted subdirectory.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, IsPathAllowed_003, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string path = "/storage/Users/currentUser/appdata/test";
    
    bool result = manager.IsPathAllowed(path);
    EXPECT_FALSE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: validateDisplayName_001
 * @tc.desc: Test validateDisplayName with valid name.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, validateDisplayName_001, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string displayName = "ValidName";
    
    bool result = manager.validateDisplayName(displayName);
    EXPECT_TRUE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: validateDisplayName_002
 * @tc.desc: Test validateDisplayName with empty name.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, validateDisplayName_002, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string displayName = "";
    
    bool result = manager.validateDisplayName(displayName);
    EXPECT_TRUE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: validateDisplayName_003
 * @tc.desc: Test validateDisplayName with too long name.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, validateDisplayName_003, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string displayName(256, 'a'); // 超过255个字符
    
    bool result = manager.validateDisplayName(displayName);
    EXPECT_FALSE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: validateDisplayName_004
 * @tc.desc: Test validateDisplayName with invalid characters.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, validateDisplayName_004, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string displayName = "Invalid/Name";
    
    bool result = manager.validateDisplayName(displayName);
    EXPECT_FALSE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: validateDisplayName_005
 * @tc.desc: Test validateDisplayName with special names.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, validateDisplayName_005, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string displayName = ".";
    
    bool result = manager.validateDisplayName(displayName);
    EXPECT_FALSE(result);
    
    displayName = "..";
    result = manager.validateDisplayName(displayName);
    EXPECT_FALSE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: validateDisplayName_006
 * @tc.desc: Test validateDisplayName with only spaces.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, validateDisplayName_006, TestSize.Level1)
{
    SynchronousRootManager& manager = SynchronousRootManager::GetInstance();
    manager.rdbStore_ = mockRdbAdapter;
    std::string displayName = "   ";
    
    bool result = manager.validateDisplayName(displayName);
    EXPECT_FALSE(result);
    manager.rdbStore_ = nullptr;
}

/**
 * @tc.name: IsSyncFolderValid001
 * @tc.desc: Test Cloud disk syncFolder is valid.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CloudDiskSynchronousRootManagerTest, IsSyncFolderValid001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager IsSyncFolderValid001 start";
    SyncFolder folder;
    folder.path_ = "/storage/Users/currentUser/appdata/test1";
    std::string bundleName;
    int32_t index = 0;
    int32_t userId = 0;
    auto& rootManager = SynchronousRootManager::GetInstance();
    int32_t ret = rootManager.IsSyncFolderValid(folder, bundleName, index, userId);
    EXPECT_EQ(ret, E_INVALID_PARAM);

    folder.path_ = "/storage/Users/currentUser/test1/../test2";
    ret = rootManager.IsSyncFolderValid(folder, bundleName, index, userId);
    EXPECT_EQ(ret, E_INVALID_PARAM);

    folder.path_ = "/storage/Users/currentUser/test1/";
    folder.displayName_ = "..";
    ret = rootManager.IsSyncFolderValid(folder, bundleName, index, userId);
    EXPECT_EQ(ret, E_INVALID_PARAM);
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager IsSyncFolderValid001 end";
}
} // namespace FileAccessFwk
} // namespace OHOS