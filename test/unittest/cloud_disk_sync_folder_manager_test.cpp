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

#include "cloud_disk_sync_folder_manager.h"
#include "file_access_framework_errno.h"
#include "file_access_service_client.h"
#include "file_access_service_mock.h"

using namespace testing;
using namespace testing::ext;

using namespace OHOS;
using namespace FileAccessFwk;

namespace {
    sptr<FileAccessServiceMock> g_fileAccessSvc = nullptr;
    bool g_svcTrue = false;
}

sptr<IFileAccessServiceBase> FileAccessServiceClient::GetInstance()
{
    if (g_svcTrue) {
        return iface_cast<IFileAccessServiceBase>(g_fileAccessSvc);
    } else {
        return nullptr;
    }
}

namespace OHOS {
namespace FileManagement {
class CloudDiskSyncFolderManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
    }
    static void TearDownTestCase()
    {
    }
    void SetUp()
    {
        g_fileAccessSvc = new (std::nothrow) FileAccessServiceMock();
        ASSERT_NE(g_fileAccessSvc, nullptr);
    }
    void TearDown()
    {
        g_fileAccessSvc = nullptr;
    }
};

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_Register_001
 * @tc.name: Register
 * @tc.desc: Test Register interface for failure case with null proxy.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_Register_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_Register_001 start";
    SyncFolder syncFolder;
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    g_svcTrue = false;
    auto res = cloudDiskSyncFolderManager.Register(syncFolder);
    EXPECT_EQ(res, E_TRY_AGAIN);
#else
    auto res = cloudDiskSyncFolderManager.Register(syncFolder);
    EXPECT_EQ(res, E_NOT_SUPPORT);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_Register_001 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_Register_002
 * @tc.name: Register
 * @tc.desc: Test Register interface for success case.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_Register_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_Register_002 start";
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    SyncFolder syncFolder;
    g_svcTrue = true;
    EXPECT_CALL(*g_fileAccessSvc, Register(_)).WillOnce(Return(0));
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
    auto res = cloudDiskSyncFolderManager.Register(syncFolder);
    EXPECT_EQ(res, 0);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_Register_002 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_Register_003
 * @tc.name: Register
 * @tc.desc: Test Register interface for service died and retry success case.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_Register_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_Register_003 start";
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    SyncFolder syncFolder;
    g_svcTrue = true;
    EXPECT_CALL(*g_fileAccessSvc, Register(_))
        .WillOnce(Return(E_SERVICE_DIED))  // E_SERVICE_DIED
        .WillOnce(Return(0));     // Success on retry
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
    auto res = cloudDiskSyncFolderManager.Register(syncFolder);
    EXPECT_EQ(res, 0);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_Register_003 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_Register_004
 * @tc.name: Register
 * @tc.desc: Test Register interface for service died and retry success case.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_Register_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_Register_004 start";
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    SyncFolder syncFolder;
    syncFolder.displayName_ = "test1";
    syncFolder.path_ = "/storage/user/CurrentUser/";
    g_svcTrue = true;
    EXPECT_CALL(*g_fileAccessSvc, Register(_))
        .WillOnce(Return(E_SERVICE_DIED))  // E_SERVICE_DIED
        .WillOnce(Return(0));     // Success on retry
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
    auto res = cloudDiskSyncFolderManager.Register(syncFolder);
    EXPECT_EQ(res, 0);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_Register_004 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_Unregister_001
 * @tc.name: Unregister
 * @tc.desc: Test Unregister interface for failure case with null proxy.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_Unregister_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_Unregister_001 start";
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    std::string uri = "";
    auto res = cloudDiskSyncFolderManager.Unregister(uri);
    EXPECT_EQ(res, E_INVALID_PARAM);

    uri = "test_uri";
    g_svcTrue = false;
    res = cloudDiskSyncFolderManager.Unregister(uri);
    EXPECT_EQ(res, E_TRY_AGAIN);
#else
    std::string uri = "test_uri";
    auto res = cloudDiskSyncFolderManager.Unregister(uri);
    EXPECT_EQ(res, E_NOT_SUPPORT);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_Unregister_001 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_Unregister_002
 * @tc.name: Unregister
 * @tc.desc: Test Unregister interface for success case.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_Unregister_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_Unregister_002 start";
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    std::string uri = "test_uri";
    g_svcTrue = true;
    EXPECT_CALL(*g_fileAccessSvc, Unregister(_)).WillOnce(Return(0));
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
    auto res = cloudDiskSyncFolderManager.Unregister(uri);
    EXPECT_EQ(res, 0);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_Unregister_002 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_Unregister_003
 * @tc.name: Unregister
 * @tc.desc: Test Unregister interface for service died and retry success case.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_Unregister_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_Unregister_003 start";
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    std::string uri = "test_uri";
    g_svcTrue = true;
    EXPECT_CALL(*g_fileAccessSvc, Unregister(_))
        .WillOnce(Return(E_SERVICE_DIED))  // E_SERVICE_DIED
        .WillOnce(Return(0));     // Success on retry
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
    auto res = cloudDiskSyncFolderManager.Unregister(uri);
    EXPECT_EQ(res, 0);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_Unregister_003 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_Active_001
 * @tc.name: Active
 * @tc.desc: Test Active interface for failure case with null proxy.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_Active_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_Active_001 start";
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    std::string uri = "";
    auto res = cloudDiskSyncFolderManager.Active(uri);
    EXPECT_EQ(res, E_INVALID_PARAM);

    uri = "test_uri";
    g_svcTrue = false;
    res = cloudDiskSyncFolderManager.Active(uri);
    EXPECT_EQ(res, E_TRY_AGAIN);
#else
    std::string uri = "test_uri";
    auto res = cloudDiskSyncFolderManager.Active(uri);
    EXPECT_EQ(res, E_NOT_SUPPORT);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_Active_001 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_Active_002
 * @tc.name: Active
 * @tc.desc: Test Active interface for success case.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_Active_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_Active_002 start";

#ifdef SUPPORT_CLOUD_DISK_MANAGER
    std::string uri = "test_uri";
    g_svcTrue = true;
    EXPECT_CALL(*g_fileAccessSvc, Active(_)).WillOnce(Return(0));
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
    auto res = cloudDiskSyncFolderManager.Active(uri);
    EXPECT_EQ(res, 0);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_Active_002 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_Active_003
 * @tc.name: Active
 * @tc.desc: Test Active interface for service died and retry success case.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_Active_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_Active_003 start";
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    std::string uri = "test_uri";
    g_svcTrue = true;
    EXPECT_CALL(*g_fileAccessSvc, Active(_))
        .WillOnce(Return(E_SERVICE_DIED))  // E_SERVICE_DIED
        .WillOnce(Return(0));     // Success on retry
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
    auto res = cloudDiskSyncFolderManager.Active(uri);
    EXPECT_EQ(res, 0);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_Active_003 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_InActive_001
 * @tc.name: Deactive
 * @tc.desc: Test Deactive interface for failure case with null proxy.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_InActive_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_InActive_001 start";
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    std::string uri = "";
    g_svcTrue = false;
    auto res = cloudDiskSyncFolderManager.Deactive(uri);
    EXPECT_EQ(res, E_INVALID_PARAM);

    uri = "test_uri";
    res = cloudDiskSyncFolderManager.Deactive(uri);
    EXPECT_EQ(res, E_TRY_AGAIN);
#else
    std::string uri = "test_uri";
    auto res = cloudDiskSyncFolderManager.Deactive(uri);
    EXPECT_EQ(res, E_NOT_SUPPORT);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_InActive_001 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_InActive_002
 * @tc.name: Deactive
 * @tc.desc: Test Deactive interface for success case.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_InActive_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_InActive_002 start";
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    std::string uri = "test_uri";
    g_svcTrue = true;
    EXPECT_CALL(*g_fileAccessSvc, Deactive(_)).WillOnce(Return(0));
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
    auto res = cloudDiskSyncFolderManager.Deactive(uri);
    EXPECT_EQ(res, 0);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_InActive_002 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_InActive_003
 * @tc.name: Deactive
 * @tc.desc: Test Deactive interface for service died and retry success case.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_InActive_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_InActive_003 start";
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    std::string uri = "test_uri";
    g_svcTrue = true;
    EXPECT_CALL(*g_fileAccessSvc, Deactive(_))
        .WillOnce(Return(E_SERVICE_DIED))  // E_SERVICE_DIED
        .WillOnce(Return(0));     // Success on retry
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
    auto res = cloudDiskSyncFolderManager.Deactive(uri);
    EXPECT_EQ(res, 0);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_InActive_003 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_GetSyncFolders_001
 * @tc.name: GetSyncFolders
 * @tc.desc: Test GetSyncFolders interface for failure case with null proxy.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_GetSyncFolders_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_GetSyncFolders_001 start";
    std::vector<SyncFolder> syncFolders;
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    g_svcTrue = false;
    auto res = cloudDiskSyncFolderManager.GetSyncFolders(syncFolders);
    EXPECT_EQ(res, E_TRY_AGAIN);
#else
    auto res = cloudDiskSyncFolderManager.GetSyncFolders(syncFolders);
    EXPECT_EQ(res, E_NOT_SUPPORT);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_GetSyncFolders_001 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_GetSyncFolders_002
 * @tc.name: GetSyncFolders
 * @tc.desc: Test GetSyncFolders interface for success case.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_GetSyncFolders_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_GetSyncFolders_002 start";
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    std::vector<SyncFolder> syncFolders;
    g_svcTrue = true;
    EXPECT_CALL(*g_fileAccessSvc, GetSyncFolders(_)).WillOnce(Return(0));
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
    auto res = cloudDiskSyncFolderManager.GetSyncFolders(syncFolders);
    EXPECT_EQ(res, 0);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_GetSyncFolders_002 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_GetSyncFolders_003
 * @tc.name: GetSyncFolders
 * @tc.desc: Test GetSyncFolders interface for service died and retry success case.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_GetSyncFolders_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_GetSyncFolders_003 start";
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    std::vector<SyncFolder> syncFolders;
    g_svcTrue = true;
    EXPECT_CALL(*g_fileAccessSvc, GetSyncFolders(_))
        .WillOnce(Return(E_SERVICE_DIED))  // E_SERVICE_DIED
        .WillOnce(Return(0));     // Success on retry
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
    auto res = cloudDiskSyncFolderManager.GetSyncFolders(syncFolders);
    EXPECT_EQ(res, 0);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_GetSyncFolders_003 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_UpdateDisplayName_001
 * @tc.name: UpdateDisplayName
 * @tc.desc: Test UpdateDisplayName interface for failure case with null proxy.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_UpdateDisplayName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_UpdateDisplayName_001 start";
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    std::string uri = "";
    std::string displayName = "test_display_name";
    g_svcTrue = false;
    auto res = cloudDiskSyncFolderManager.UpdateDisplayName(uri, displayName);
    EXPECT_EQ(res, E_INVALID_PARAM);

    uri = "test_uri";
    res = cloudDiskSyncFolderManager.UpdateDisplayName(uri, displayName);
    EXPECT_EQ(res, E_TRY_AGAIN);
#else
    std::string uri = "test_uri";
    std::string displayName = "test_display_name";
    auto res = cloudDiskSyncFolderManager.UpdateDisplayName(uri, displayName);
    EXPECT_EQ(res, E_NOT_SUPPORT);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_UpdateDisplayName_001 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_UpdateDisplayName_002
 * @tc.name: UpdateDisplayName
 * @tc.desc: Test UpdateDisplayName interface for success case.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_UpdateDisplayName_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_UpdateDisplayName_002 start";
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    std::string uri = "test_uri";
    std::string displayName = "test_display_name";
    g_svcTrue = true;
    EXPECT_CALL(*g_fileAccessSvc, UpdateDisplayName(_, _)).WillOnce(Return(0));
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
    auto res = cloudDiskSyncFolderManager.UpdateDisplayName(uri, displayName);
    EXPECT_EQ(res, 0);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_UpdateDisplayName_002 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_UpdateDisplayName_003
 * @tc.name: UpdateDisplayName
 * @tc.desc: Test UpdateDisplayName interface for service died and retry success case.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_UpdateDisplayName_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_UpdateDisplayName_003 start";
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    std::string uri = "test_uri";
    std::string displayName = "test_display_name";
    g_svcTrue = true;
    EXPECT_CALL(*g_fileAccessSvc, UpdateDisplayName(_, _))
        .WillOnce(Return(E_SERVICE_DIED))  // E_SERVICE_DIED
        .WillOnce(Return(0));     // Success on retry
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
    auto res = cloudDiskSyncFolderManager.UpdateDisplayName(uri, displayName);
    EXPECT_EQ(res, 0);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_UpdateDisplayName_003 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_UnregisterForSa_001
 * @tc.name: UnregisterForSa
 * @tc.desc: Test UnregisterForSa interface for failure case with null proxy.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_UnregisterForSa_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_UnregisterForSa_001 start";
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    std::string uri = "";
    g_svcTrue = false;
    auto res = cloudDiskSyncFolderManager.UnregisterForSa(uri);
    EXPECT_EQ(res, E_INVALID_PARAM);

    uri = "test_uri";
    res = cloudDiskSyncFolderManager.UnregisterForSa(uri);
    EXPECT_EQ(res, E_TRY_AGAIN);
#else
    std::string uri = "test_uri";
    auto res = cloudDiskSyncFolderManager.UnregisterForSa(uri);
    EXPECT_EQ(res, E_NOT_SUPPORT);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_UnregisterForSa_001 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_UnregisterForSa_002
 * @tc.name: UnregisterForSa
 * @tc.desc: Test UnregisterForSa interface for success case.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_UnregisterForSa_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_UnregisterForSa_002 start";
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    std::string uri = "test_uri";
    g_svcTrue = true;
    EXPECT_CALL(*g_fileAccessSvc, UnregisterForSa(_)).WillOnce(Return(0));
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
    auto res = cloudDiskSyncFolderManager.UnregisterForSa(uri);
    EXPECT_EQ(res, 0);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_UnregisterForSa_002 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_UnregisterForSa_003
 * @tc.name: UnregisterForSa
 * @tc.desc: Test UnregisterForSa interface for service died and retry success case.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_UnregisterForSa_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_UnregisterForSa_003 start";
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    std::string uri = "test_uri";
    g_svcTrue = true;
    EXPECT_CALL(*g_fileAccessSvc, UnregisterForSa(_))
        .WillOnce(Return(E_SERVICE_DIED))  // E_SERVICE_DIED
        .WillOnce(Return(0));     // Success on retry
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
    auto res = cloudDiskSyncFolderManager.UnregisterForSa(uri);
    EXPECT_EQ(res, 0);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_UnregisterForSa_003 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_GetAllSyncFoldersForSa_001
 * @tc.name: GetAllSyncFoldersForSa
 * @tc.desc: Test GetAllSyncFoldersForSa interface for failure case with null proxy.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_GetAllSyncFoldersForSa_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_GetAllSyncFoldersForSa_001 start";
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    std::vector<SyncFolderExt> syncFolderExts;
    g_svcTrue = false;
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
    auto res = cloudDiskSyncFolderManager.GetAllSyncFoldersForSa(syncFolderExts);
    EXPECT_EQ(res, E_TRY_AGAIN);
#else
    std::vector<SyncFolderExt> syncFolderExts;
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
    auto res = cloudDiskSyncFolderManager.GetAllSyncFoldersForSa(syncFolderExts);
    EXPECT_EQ(res, E_NOT_SUPPORT);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_GetAllSyncFoldersForSa_001 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_GetAllSyncFoldersForSa_002
 * @tc.name: GetAllSyncFoldersForSa
 * @tc.desc: Test GetAllSyncFoldersForSa interface for success case.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_GetAllSyncFoldersForSa_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_GetAllSyncFoldersForSa_002 start";
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    std::vector<SyncFolderExt> syncFolderExts;
    g_svcTrue = true;
    EXPECT_CALL(*g_fileAccessSvc, GetAllSyncFoldersForSa(_)).WillOnce(Return(0));
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
    auto res = cloudDiskSyncFolderManager.GetAllSyncFoldersForSa(syncFolderExts);
    EXPECT_EQ(res, 0);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_GetAllSyncFoldersForSa_002 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_sync_folder_manager_GetAllSyncFoldersForSa_003
 * @tc.name: GetAllSyncFoldersForSa
 * @tc.desc: Test GetAllSyncFoldersForSa interface for service died and retry success case.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskSyncFolderManagerTest, CloudDiskSyncFolderManager_GetAllSyncFoldersForSa_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_GetAllSyncFoldersForSa_003 start";
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    std::vector<SyncFolderExt> syncFolderExts;
    g_svcTrue = true;
    EXPECT_CALL(*g_fileAccessSvc, GetAllSyncFoldersForSa(_))
        .WillOnce(Return(E_SERVICE_DIED))  // E_SERVICE_DIED
        .WillOnce(Return(0));     // Success on retry
    CloudDiskSyncFolderManager &cloudDiskSyncFolderManager = CloudDiskSyncFolderManager::GetInstance();
    auto res = cloudDiskSyncFolderManager.GetAllSyncFoldersForSa(syncFolderExts);
    EXPECT_EQ(res, 0);
#endif
    GTEST_LOG_(INFO) << "CloudDiskSyncFolderManager_GetAllSyncFoldersForSa_003 end";
}
} // namespace FileManagement
} // namespace OHOS