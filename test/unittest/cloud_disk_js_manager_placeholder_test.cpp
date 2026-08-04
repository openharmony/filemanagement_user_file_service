/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <set>
#include <string>

#include "cloud_disk_placeholder_manager.h"
#include "file_access_framework_errno.h"

using namespace testing::ext;

using namespace OHOS;
using namespace FileAccessFwk;

namespace OHOS {
namespace FileManagement {
struct CloudDiskErrorInfo {
    int32_t code;
    std::string message;
};

CloudDiskErrorInfo GetCloudDiskErrorInfo(int32_t errCode);

namespace {
enum CloudDiskApiErrCode : int32_t {
    CLOUD_DISK_OK = 0,
    CLOUD_DISK_PERMISSION_DENIED = 201,
    CLOUD_DISK_NOT_SUPPORTED = 801,
    CLOUD_DISK_INVALID_ARG = 34400001,
    CLOUD_DISK_SYNC_FOLDER_PATH_UNAUTHORIZED = 34400002,
    CLOUD_DISK_IPC_FAILED = 34400003,
    CLOUD_DISK_SYNC_FOLDER_NOT_REGISTERED = 34400008,
    CLOUD_DISK_SYNC_FOLDER_PATH_NOT_EXIST = 34400010,
    CLOUD_DISK_TRY_AGAIN = 34400014,
    CLOUD_DISK_NOT_ALLOWED = 34400015,
    CLOUD_DISK_NOT_A_DIRECTORY = 34400023,
    CLOUD_DISK_FILE_NOT_EXIST = 34400024,
    CLOUD_DISK_NAME_TOO_LONG = 34400025,
};

enum CloudDiskInternalErrCode : int32_t {
    E_OK = 0,
    E_PERMISSION = 201,
    E_NOT_SUPPORT = 801,
    E_INVALID_PARAM = 34400001,
    E_SYNC_FOLDER_PATH_UNAUTHORIZED = 34400002,
    E_IPC_FAILED = 34400003,
    E_SYNC_FOLDER_NOT_REGISTERED = 34400008,
    E_SYNC_FOLDER_PATH_NOT_EXIST = 34400010,
    E_TRY_AGAIN = 34400014,
    E_SYSTEM_RESTRICTED = 34400015,
    E_NOT_A_DIRECTORY = 34400023,
    E_FILE_NOT_EXIST = 34400024,
    E_NAME_TOO_LONG = 34400025,
};

constexpr int32_t UNKNOWN_ERROR = -1;

const std::set<int32_t> PLACEHOLDER_API_ERR_CODE = {
    CLOUD_DISK_OK,
    CLOUD_DISK_PERMISSION_DENIED,
    CLOUD_DISK_NOT_SUPPORTED,
    CLOUD_DISK_INVALID_ARG,
    CLOUD_DISK_SYNC_FOLDER_PATH_UNAUTHORIZED,
    CLOUD_DISK_IPC_FAILED,
    CLOUD_DISK_SYNC_FOLDER_NOT_REGISTERED,
    CLOUD_DISK_SYNC_FOLDER_PATH_NOT_EXIST,
    CLOUD_DISK_TRY_AGAIN,
    CLOUD_DISK_NOT_ALLOWED,
    CLOUD_DISK_NOT_A_DIRECTORY,
    CLOUD_DISK_FILE_NOT_EXIST,
    CLOUD_DISK_NAME_TOO_LONG,
};
} // namespace

class CloudDiskJSManagerPlaceholderTest : public testing::Test {
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
 * @tc.number: user_file_service_cloud_disk_js_manager_placeholder_IsPlaceholderFile_001
 * @tc.name: IsPlaceholderFile
 * @tc.desc: Test IsPlaceholderFile interface for unsupported feature.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskJSManagerPlaceholderTest, CloudDiskJSManager_IsPlaceholderFile_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskJSManager_IsPlaceholderFile_001 start";
    bool isPlaceholder = false;
    CloudDiskPlaceholderManager cloudDiskPlaceholderManager("/storage/Users/currentUser/sync");
    auto res = cloudDiskPlaceholderManager.IsPlaceholderFile("dir/file.txt", isPlaceholder);
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    EXPECT_NE(PLACEHOLDER_API_ERR_CODE.find(res), PLACEHOLDER_API_ERR_CODE.end());
#else
    EXPECT_EQ(res, E_NOT_SUPPORT);
#endif
    GTEST_LOG_(INFO) << "CloudDiskJSManager_IsPlaceholderFile_001 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_js_manager_placeholder_CloudDiskError_001
 * @tc.name: CloudDiskError
 * @tc.desc: Test CloudDisk placeholder error message for sync folder unauthorized.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskJSManagerPlaceholderTest, CloudDiskJSManager_CloudDiskError_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskJSManager_CloudDiskError_001 start";
    auto errorInfo = GetCloudDiskErrorInfo(E_SYNC_FOLDER_PATH_UNAUTHORIZED);
    EXPECT_EQ(errorInfo.code, CLOUD_DISK_SYNC_FOLDER_PATH_UNAUTHORIZED);
    EXPECT_EQ(errorInfo.message, "CLOUD_DISK_SYNC_FOLDER_PATH_UNAUTHORIZED");
    GTEST_LOG_(INFO) << "CloudDiskJSManager_CloudDiskError_001 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_js_manager_placeholder_CloudDiskError_002
 * @tc.name: CloudDiskError
 * @tc.desc: Test CloudDisk placeholder error message for not allowed and unknown error.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskJSManagerPlaceholderTest, CloudDiskJSManager_CloudDiskError_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskJSManager_CloudDiskError_002 start";
    auto notAllowedErrorInfo = GetCloudDiskErrorInfo(CLOUD_DISK_NOT_ALLOWED);
    auto unknownErrorInfo = GetCloudDiskErrorInfo(UNKNOWN_ERROR);
    EXPECT_EQ(notAllowedErrorInfo.code, CLOUD_DISK_NOT_ALLOWED);
    EXPECT_EQ(notAllowedErrorInfo.message, "CLOUD_DISK_NOT_ALLOWED");
    EXPECT_EQ(unknownErrorInfo.code, CLOUD_DISK_TRY_AGAIN);
    EXPECT_EQ(unknownErrorInfo.message, "CLOUD_DISK_TRY_AGAIN");
    GTEST_LOG_(INFO) << "CloudDiskJSManager_CloudDiskError_002 end";
}

/**
 * @tc.number: user_file_service_cloud_disk_js_manager_placeholder_CloudDiskError_003
 * @tc.name: CloudDiskError
 * @tc.desc: Test CloudDisk placeholder error code conversion for public cloud disk errors.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(CloudDiskJSManagerPlaceholderTest, CloudDiskJSManager_CloudDiskError_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CloudDiskJSManager_CloudDiskError_003 start";
    EXPECT_EQ(GetCloudDiskErrorInfo(E_INVALID_PARAM).code, CLOUD_DISK_INVALID_ARG);
    EXPECT_EQ(GetCloudDiskErrorInfo(E_SYSTEM_RESTRICTED).code, CLOUD_DISK_NOT_ALLOWED);
    EXPECT_EQ(GetCloudDiskErrorInfo(E_NOT_A_DIRECTORY).code, CLOUD_DISK_NOT_A_DIRECTORY);
    EXPECT_EQ(GetCloudDiskErrorInfo(E_FILE_NOT_EXIST).code, CLOUD_DISK_FILE_NOT_EXIST);
    EXPECT_EQ(GetCloudDiskErrorInfo(E_NAME_TOO_LONG).code, CLOUD_DISK_NAME_TOO_LONG);
    EXPECT_EQ(GetCloudDiskErrorInfo(UNKNOWN_ERROR).code, CLOUD_DISK_TRY_AGAIN);
    GTEST_LOG_(INFO) << "CloudDiskJSManager_CloudDiskError_003 end";
}
} // namespace FileManagement
} // namespace OHOS
