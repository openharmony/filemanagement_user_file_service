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

#include "notify_work_service.h"

#include "cloud_disk_comm.h"
#include "hilog_wrapper.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::FileAccessFwk;
using namespace OHOS::FileManagement;

namespace OHOS {
namespace FileAccessFwk {
class NotifyWorkServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void NotifyWorkServiceTest::SetUpTestCase(void)
{
    // Setup code to run before all tests
}

void NotifyWorkServiceTest::TearDownTestCase(void)
{
    // Teardown code to run after all tests
}

void NotifyWorkServiceTest::SetUp()
{
    // Setup code to run before each test
}

void NotifyWorkServiceTest::TearDown()
{
    // Teardown code to run after each test
}

/**
 * @tc.number: NotifyWorkService_NotifySyncFolderEvent_001
 * @tc.name: NotifySyncFolderEvent_001
 * @tc.desc: Verify NotifySyncFolderEvent works
 */
HWTEST_F(NotifyWorkServiceTest, NotifyWorkService_NotifySyncFolderEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NotifyWorkServiceTest-begin NotifyWorkService_NotifySyncFolderEvent_001";
    SyncFolderExt syncFolderExt;
    syncFolderExt.bundleName_ = "com.example.simulatecloud";
    syncFolderExt.path_ = "data/storage";
    syncFolderExt.state_ = State::ACTIVE;
    NotifyWorkService& service = NotifyWorkService::GetInstance();
    NotifyWorkService::EventType inValidEventType =
        static_cast<NotifyWorkService::EventType>(-1); // -1: invalid EventType
    bool result = service.NotifySyncFolderEvent(syncFolderExt, inValidEventType);
    EXPECT_EQ(result, false);

    inValidEventType = NotifyWorkService::EventType::MAX_EVENT_TYPE;
    result = service.NotifySyncFolderEvent(syncFolderExt, inValidEventType);
    EXPECT_EQ(result, false);

    inValidEventType =
        static_cast<NotifyWorkService::EventType>(5); // 5: invalid EventType
    result = service.NotifySyncFolderEvent(syncFolderExt, inValidEventType);
    EXPECT_EQ(result, false);
    GTEST_LOG_(INFO) << "NotifyWorkServiceTest-end NotifyWorkService_NotifySyncFolderEvent_001";
}

/**
 * @tc.number: NotifyWorkService_NotifySyncFolderEvent_002
 * @tc.name: NotifySyncFolderEvent_002
 * @tc.desc: Verify NotifySyncFolderEvent works
 */
HWTEST_F(NotifyWorkServiceTest, NotifyWorkService_NotifySyncFolderEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NotifyWorkServiceTest-begin NotifyWorkService_NotifySyncFolderEvent_002";
    SyncFolderExt syncFolderExt;
    syncFolderExt.bundleName_ = "com.example.simulatecloud";
    syncFolderExt.path_ = "data/storage";
    syncFolderExt.state_ = State::ACTIVE;
    syncFolderExt.displayNameResId_ = 0; // 0: Valid displayNameResId
    NotifyWorkService& service = NotifyWorkService::GetInstance();
    bool result = service.NotifySyncFolderEvent(syncFolderExt, NotifyWorkService::EventType::REGISTER);
    EXPECT_EQ(result, true);

    syncFolderExt.displayName_ = "";
    result = service.NotifySyncFolderEvent(syncFolderExt, NotifyWorkService::EventType::REGISTER);
    EXPECT_EQ(result, true);

    syncFolderExt.displayName_ = "test";
    result = service.NotifySyncFolderEvent(syncFolderExt, NotifyWorkService::EventType::REGISTER);
    EXPECT_EQ(result, true);
    GTEST_LOG_(INFO) << "NotifyWorkServiceTest-end NotifyWorkService_NotifySyncFolderEvent_002";
}

/**
 * @tc.number: NotifyWorkService_NotifySyncFolderEvent_003
 * @tc.name: NotifySyncFolderEvent_003
 * @tc.desc: Verify NotifySyncFolderEvent works
 */
HWTEST_F(NotifyWorkServiceTest, NotifyWorkService_NotifySyncFolderEvent_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NotifyWorkServiceTest-begin NotifyWorkService_NotifySyncFolderEvent_003";
    SyncFolderExt syncFolderExt;
    syncFolderExt.bundleName_ = "com.example.simulatecloud";
    syncFolderExt.path_ = "data/storage";
    syncFolderExt.state_ = State::ACTIVE;
    syncFolderExt.displayNameResId_ = 1; // 1: test displayNameResId
    NotifyWorkService& service = NotifyWorkService::GetInstance();
    bool result = service.NotifySyncFolderEvent(syncFolderExt, NotifyWorkService::EventType::UNREGISTER);
    EXPECT_EQ(result, true);

    syncFolderExt.displayName_ = "";
    result = service.NotifySyncFolderEvent(syncFolderExt, NotifyWorkService::EventType::REGISTER);
    EXPECT_EQ(result, true);

    syncFolderExt.displayName_ = "test";
    result = service.NotifySyncFolderEvent(syncFolderExt, NotifyWorkService::EventType::REGISTER);
    EXPECT_EQ(result, true);
    GTEST_LOG_(INFO) << "NotifyWorkServiceTest-end NotifyWorkService_NotifySyncFolderEvent_003";
}
} // namespace FileAccessFwk
} // namespace OHOS