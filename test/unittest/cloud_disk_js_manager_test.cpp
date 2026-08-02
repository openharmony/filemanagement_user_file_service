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

#include <set>

#include "cloud_disk_js_manager.h"
#include "file_access_framework_errno.h"
#include "file_access_service_client.h"

using namespace testing;
using namespace testing::ext;

using namespace OHOS;
using namespace FileAccessFwk;

namespace OHOS {
namespace FileManagement {
namespace {
constexpr int32_t CLOUD_DISK_NOT_ALLOWED = 34400015;
constexpr int32_t CLOUD_DISK_NOT_A_DIRECTORY = 34400023;
constexpr int32_t CLOUD_DISK_FILE_NOT_EXIST = 34400024;
constexpr int32_t CLOUD_DISK_NAME_TOO_LONG = 34400025;
constexpr int32_t E_NOT_A_DIRECTORY = 34400023;
constexpr int32_t E_FILE_NOT_EXIST = 34400024;
constexpr int32_t E_NAME_TOO_LONG = 34400025;
constexpr int32_t UNKNOWN_ERROR = -1;

const std::set<int32_t> PLACEHOLDER_ERR_CODE = {
    ERR_OK,
    E_INVALID_PARAM,
    E_SYNC_FOLDER_PATH_UNAUTHORIZED,
    E_IPC_FAILED,
    E_SYNC_FOLDER_NOT_REGISTERED,
    E_SYNC_FOLDER_PATH_NOT_EXIST,
    E_TRY_AGAIN,
    CLOUD_DISK_NOT_ALLOWED,
    CLOUD_DISK_NOT_A_DIRECTORY,
    CLOUD_DISK_FILE_NOT_EXIST,
    CLOUD_DISK_NAME_TOO_LONG,
    E_NOT_SUPPORT,
    E_PERMISSION,
};
} // namespace

class CloudDiskJSManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
    }
    static void TearDownTestCase()
    {
    }
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};

/**
 * @tc.number: user_file_service_cloud_disk_js_manager_GetAllSyncFolders_001
 * @tc.name: GetAllSyncFolders
 * @tc.desc: Test GetAllSyncFolders interface for failure case with null proxy.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskJSManagerTest, CloudDiskJSManager_GetAllSyncFolders_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskJSManager_GetAllSyncFolders_001 start";
    std::vector<SyncFolderExt> syncFolderExts;
    CloudDiskJSManager cloudDiskJSManager;
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    auto res = cloudDiskJSManager.GetAllSyncFolders(syncFolderExts);
    EXPECT_EQ(res, E_PERMISSION_SYS);
#else
    auto res = cloudDiskJSManager.GetAllSyncFolders(syncFolderExts);
    EXPECT_EQ(res, E_NOT_SUPPORT);
#endif
    GTEST_LOG_(INFO) << "CloudDiskJSManager_GetAllSyncFolders_001 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_js_manager_IsPlaceholderFile_001
 * @tc.name: IsPlaceholderFile
 * @tc.desc: Test IsPlaceholderFile interface for unsupported feature.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskJSManagerTest, CloudDiskJSManager_IsPlaceholderFile_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskJSManager_IsPlaceholderFile_001 start";
    bool isPlaceholder = false;
    CloudDiskJSManager cloudDiskJSManager("/storage/Users/currentUser/sync");
    auto res = cloudDiskJSManager.IsPlaceholderFile("dir/file.txt", isPlaceholder);
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    EXPECT_NE(PLACEHOLDER_ERR_CODE.find(res), PLACEHOLDER_ERR_CODE.end());
#else
    EXPECT_EQ(res, E_NOT_SUPPORT);
#endif
    GTEST_LOG_(INFO) << "CloudDiskJSManager_IsPlaceholderFile_001 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_js_manager_GetCloudDiskErrMsg_001
 * @tc.name: GetCloudDiskErrMsg
 * @tc.desc: Test GetCloudDiskErrMsg interface for sync folder unauthorized.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskJSManagerTest, CloudDiskJSManager_GetCloudDiskErrMsg_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskJSManager_GetCloudDiskErrMsg_001 start";
    auto res = CloudDiskJSManager::GetCloudDiskErrMsg(E_SYNC_FOLDER_PATH_UNAUTHORIZED);
    EXPECT_EQ(res, "CLOUD_DISK_SYNC_FOLDER_PATH_UNAUTHORIZED");
    GTEST_LOG_(INFO) << "CloudDiskJSManager_GetCloudDiskErrMsg_001 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_js_manager_GetCloudDiskErrMsg_002
 * @tc.name: GetCloudDiskErrMsg
 * @tc.desc: Test GetCloudDiskErrMsg interface for not allowed and unknown error.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskJSManagerTest, CloudDiskJSManager_GetCloudDiskErrMsg_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskJSManager_GetCloudDiskErrMsg_002 start";
    EXPECT_EQ(CloudDiskJSManager::GetCloudDiskErrMsg(CLOUD_DISK_NOT_ALLOWED), "CLOUD_DISK_NOT_ALLOWED");
    EXPECT_EQ(CloudDiskJSManager::GetCloudDiskErrMsg(UNKNOWN_ERROR), "CLOUD_DISK_IPC_FAILED");
    GTEST_LOG_(INFO) << "CloudDiskJSManager_GetCloudDiskErrMsg_002 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_js_manager_ConvertToCloudDiskApiErrCode_001
 * @tc.name: ConvertToCloudDiskApiErrCode
 * @tc.desc: Test ConvertToCloudDiskApiErrCode interface for public cloud disk errors.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskJSManagerTest, CloudDiskJSManager_ConvertToCloudDiskApiErrCode_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskJSManager_ConvertToCloudDiskApiErrCode_001 start";
    EXPECT_EQ(CloudDiskJSManager::ConvertToCloudDiskApiErrCode(E_INVALID_PARAM), E_INVALID_PARAM);
    EXPECT_EQ(CloudDiskJSManager::ConvertToCloudDiskApiErrCode(E_SYSTEM_RESTRICTED), CLOUD_DISK_NOT_ALLOWED);
    EXPECT_EQ(CloudDiskJSManager::ConvertToCloudDiskApiErrCode(E_NOT_A_DIRECTORY), CLOUD_DISK_NOT_A_DIRECTORY);
    EXPECT_EQ(CloudDiskJSManager::ConvertToCloudDiskApiErrCode(E_FILE_NOT_EXIST), CLOUD_DISK_FILE_NOT_EXIST);
    EXPECT_EQ(CloudDiskJSManager::ConvertToCloudDiskApiErrCode(E_NAME_TOO_LONG), CLOUD_DISK_NAME_TOO_LONG);
    EXPECT_EQ(CloudDiskJSManager::ConvertToCloudDiskApiErrCode(UNKNOWN_ERROR), E_IPC_FAILED);
    GTEST_LOG_(INFO) << "CloudDiskJSManager_ConvertToCloudDiskApiErrCode_001 end";
}
} // namespace FileManagement
} // namespace OHOS
