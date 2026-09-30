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
#include <thread>
#include <vector>

#include "cloud_disk_js_manager.h"
#include "cloud_disk_manager_utils.h"
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

/**
 * @tc.number: user_file_service_cloud_disk_js_manager_CreateAccessorId_001
 * @tc.name: CreateAccessorId
 * @tc.desc: Concurrent instance creation produces ids without duplicates.
 * @tc.size: MEDIUM
 * @tc.type: RELI
 * @tc.level Level 2
 */
HWTEST_F(CloudDiskJSManagerTest, CloudDiskJSManager_CreateAccessorId_001, TestSize.Level2)
{
    constexpr size_t countPerThread = 64;
    std::vector<uint64_t> ids(countPerThread * 2);
    auto create = [&ids](size_t offset) {
        for (size_t i = 0; i < countPerThread; ++i) {
            ids[offset + i] = CreateCloudDiskAccessorId();
        }
    };
    std::thread first(create, 0);
    std::thread second(create, countPerThread);
    first.join();
    second.join();
    EXPECT_EQ(std::set<uint64_t>(ids.begin(), ids.end()).size(), ids.size());
}
} // namespace FileManagement
} // namespace OHOS
