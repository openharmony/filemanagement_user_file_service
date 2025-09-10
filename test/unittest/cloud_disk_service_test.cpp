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
#include "file_access_service.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "notify_work_service.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::FileAccessFwk;
using namespace OHOS::FileManagement;

namespace OHOS {
namespace FileAccessFwk {
class CloudDiskServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void CloudDiskServiceTest::SetUpTestCase(void)
{
    // Setup code to run before all tests
}

void CloudDiskServiceTest::TearDownTestCase(void)
{
    // Teardown code to run after all tests
}

void CloudDiskServiceTest::SetUp()
{
    // Setup code to run before each test
}

void CloudDiskServiceTest::TearDown()
{
    // Teardown code to run after each test
}

/**
 * @tc.number: CloudDiskService_Register_001
 * @tc.name: Register cloud disk root info
 * @tc.desc: Verify Register function works correctly
 */
HWTEST_F(CloudDiskServiceTest, CloudDiskService_Register_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskServiceTest-begin CloudDiskService_Register_001";
    SyncFolder syncFolder;
    syncFolder.path_ = "/storage/Users/currentUser";

    sptr<FileAccessService> service = FileAccessService::GetInstance();
    ASSERT_NE(service, nullptr);
    int32_t result = service->Register(syncFolder);
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    EXPECT_EQ(result, ERR_OK);
#else
    EXPECT_EQ(result, E_SYSTEM_RESTRICTED);
#endif
    GTEST_LOG_(INFO) << "CloudDiskServiceTest-end CloudDiskService_Register_001";
}

/**
 * @tc.number: CloudDiskService_Unregister_001
 * @tc.name: Unregister cloud disk root info
 * @tc.desc: Verify Unregister function works correctly
 */
HWTEST_F(CloudDiskServiceTest, CloudDiskService_Unregister_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskServiceTest-begin CloudDiskService_Unregister_001";
    std::string path = "/storage/Users/currentUser";
    sptr<FileAccessService> service = FileAccessService::GetInstance();
    ASSERT_NE(service, nullptr);
    int32_t result = service->Unregister(path);
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    EXPECT_EQ(result, ERR_OK);
#else
    EXPECT_EQ(result, E_SYSTEM_RESTRICTED);
#endif
    GTEST_LOG_(INFO) << "CloudDiskServiceTest-end CloudDiskService_Unregister_001";
}

/**
 * @tc.number: CloudDiskService_Active_001
 * @tc.name: Active cloud disk
 * @tc.desc: Verify Active function works correctly
 */
HWTEST_F(CloudDiskServiceTest, CloudDiskService_Active_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskServiceTest-begin CloudDiskService_Active_001";
    std::string path = "/storage/Users/currentUser";

    sptr<FileAccessService> service = FileAccessService::GetInstance();
    ASSERT_NE(service, nullptr);
    int32_t result = service->Active(path);
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    EXPECT_EQ(result, ERR_OK);
#else
    EXPECT_EQ(result, E_SYSTEM_RESTRICTED);
#endif
    GTEST_LOG_(INFO) << "CloudDiskServiceTest-end CloudDiskService_Active_001";
}

/**
 * @tc.number: CloudDiskService_InActive_001
 * @tc.name: Deactive cloud disk
 * @tc.desc: Verify Deactive function works correctly
 */
HWTEST_F(CloudDiskServiceTest, CloudDiskService_InActive_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskServiceTest-begin CloudDiskService_InActive_001";
    std::string path = "/storage/Users/currentUser";

    sptr<FileAccessService> service = FileAccessService::GetInstance();
    ASSERT_NE(service, nullptr);
    int32_t result = service->Deactive(path);
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    EXPECT_EQ(result, ERR_OK);
#else
    EXPECT_EQ(result, E_SYSTEM_RESTRICTED);
#endif
    GTEST_LOG_(INFO) << "CloudDiskServiceTest-end CloudDiskService_InActive_001";
}

/**
 * @tc.number: CloudDiskService_GetSyncFolders_001
 * @tc.name: Get cloud disk roots
 * @tc.desc: Verify GetSyncFolders function works correctly
 */
HWTEST_F(CloudDiskServiceTest, CloudDiskService_GetSyncFolders_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskServiceTest-begin CloudDiskService_GetSyncFolders_001";
    std::vector<SyncFolder> syncFolders;

    sptr<FileAccessService> service = FileAccessService::GetInstance();
    ASSERT_NE(service, nullptr);
    int32_t result = service->GetSyncFolders(syncFolders);
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    EXPECT_EQ(result, ERR_OK);
#else
    EXPECT_EQ(result, E_SYSTEM_RESTRICTED);
#endif
    GTEST_LOG_(INFO) << "CloudDiskServiceTest-end CloudDiskService_GetSyncFolders_001";
}

/**
 * @tc.number: CloudDiskService_GetAllSyncFolders_001
 * @tc.name: Get all cloud disk roots
 * @tc.desc: Verify GetAllSyncFolders function works correctly
 */
HWTEST_F(CloudDiskServiceTest, CloudDiskService_GetAllSyncFolders_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskServiceTest-begin CloudDiskService_GetAllSyncFolders_001";
    std::vector<SyncFolderExt> syncFolderExts;

    sptr<FileAccessService> service = FileAccessService::GetInstance();
    ASSERT_NE(service, nullptr);
    int32_t result = service->GetAllSyncFolders(syncFolderExts);
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    EXPECT_EQ(result, ERR_OK);
#else
    EXPECT_EQ(result, E_SYSTEM_RESTRICTED);
#endif
    GTEST_LOG_(INFO) << "CloudDiskServiceTest-end CloudDiskService_GetAllSyncFolders_001";
}

/**
 * @tc.number: CloudDiskService_UpdateDisplayName_001
 * @tc.name: UpdateDisplayName function
 * @tc.desc: Verify UpdateDisplayName function works correctly when NotifySyncFolderEvent returns true
 */
HWTEST_F(CloudDiskServiceTest, CloudDiskService_UpdateDisplayName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskServiceTest-begin CloudDiskService_UpdateDisplayName_001";
    std::string path = "/storage/Users/currentUser";
    std::string displayName = "TestDisplayName";

    sptr<FileAccessService> service = FileAccessService::GetInstance();
    ASSERT_NE(service, nullptr);
    int32_t result = service->UpdateDisplayName(path, displayName);
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    EXPECT_EQ(result, ERR_OK);
#else
    EXPECT_EQ(result, E_SYSTEM_RESTRICTED);
#endif
    GTEST_LOG_(INFO) << "CloudDiskServiceTest-end CloudDiskService_UpdateDisplayName_001";
}

/**
 * @tc.number: CloudDiskService_IncreaseCnt_001
 * @tc.name: IncreaseCnt function
 * @tc.desc: Verify IncreaseCnt function works correctly
 */
HWTEST_F(CloudDiskServiceTest, CloudDiskService_IncreaseCnt_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskServiceTest-begin CloudDiskService_IncreaseCnt_001";
    sptr<FileAccessService> service = FileAccessService::GetInstance();
    ASSERT_NE(service, nullptr);

    int32_t initialCount = service->calledCount_;
    service->IncreaseCnt("test_func");
    EXPECT_EQ(service->calledCount_, initialCount + 1);
    GTEST_LOG_(INFO) << "CloudDiskServiceTest-end CloudDiskService_IncreaseCnt_001";
}

/**
 * @tc.number: CloudDiskService_DecreaseCnt_001
 * @tc.name: DecreaseCnt function
 * @tc.desc: Verify DecreaseCnt function works correctly when count > 0
 */
HWTEST_F(CloudDiskServiceTest, CloudDiskService_DecreaseCnt_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskServiceTest-begin CloudDiskService_DecreaseCnt_001";
    sptr<FileAccessService> service = FileAccessService::GetInstance();
    ASSERT_NE(service, nullptr);

    service->IncreaseCnt("test_func"); // Make sure count > 0
    int32_t initialCount = service->calledCount_;
    service->DecreaseCnt("test_func");
    EXPECT_EQ(service->calledCount_, initialCount - 1);
    GTEST_LOG_(INFO) << "CloudDiskServiceTest-end CloudDiskService_DecreaseCnt_001";
}

/**
 * @tc.number: CloudDiskService_DecreaseCnt_002
 * @tc.name: DecreaseCnt function
 * @tc.desc: Verify DecreaseCnt function works correctly when count is 0
 */
HWTEST_F(CloudDiskServiceTest, CloudDiskService_DecreaseCnt_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskServiceTest-begin CloudDiskService_DecreaseCnt_002";
    sptr<FileAccessService> service = FileAccessService::GetInstance();
    ASSERT_NE(service, nullptr);

    // Ensure count is 0
    service->calledCount_ = 0;

    int32_t initialCount = service->calledCount_;
    service->DecreaseCnt("test_func");
    EXPECT_EQ(service->calledCount_, initialCount); // Should remain 0
    GTEST_LOG_(INFO) << "CloudDiskServiceTest-end CloudDiskService_DecreaseCnt_002";
}
} // namespace FileAccessFwk
} // namespace OHOS