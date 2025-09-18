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

#include "cloud_disk_js_manager.h"
#include "file_access_framework_errno.h"
#include "file_access_service_client.h"

using namespace testing;
using namespace testing::ext;

using namespace OHOS;
using namespace FileAccessFwk;

namespace OHOS {
namespace FileManagement {
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
} // namespace FileManagement
} // namespace OHOS