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

#include "cloud_disk_comm.h"
#include "message_parcel_mock.h"

namespace OHOS {
namespace FileManagement {
using namespace testing;
using namespace testing::ext;
using namespace OHOS::FileAccessFwk;

class CloudDiskCommTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
public:
    static inline std::shared_ptr<MessageParcelMock> messageParcelMock_ = nullptr;
};

void CloudDiskCommTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase";
    messageParcelMock_ = std::make_shared<MessageParcelMock>();
    MessageParcelMock::messageParcel = messageParcelMock_;
}

void CloudDiskCommTest::TearDownTestCase(void)
{
    GTEST_LOG_(INFO) << "TearDownTestCase";
    MessageParcelMock::messageParcel = nullptr;
    messageParcelMock_ = nullptr;
}

void CloudDiskCommTest::SetUp(void)
{
    GTEST_LOG_(INFO) << "SetUp";
}

void CloudDiskCommTest::TearDown(void)
{
    GTEST_LOG_(INFO) << "TearDown";
}

/**
 * @tc.name: SyncFolder_Marshalling_001
 * @tc.desc: Test SyncFolder Marshalling success
 * @tc.type: FUNC
 */
HWTEST_F(CloudDiskCommTest, SyncFolder_Marshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SyncFolder_Marshalling_001 start";

    SyncFolder syncFolder("/test/path", State::ACTIVE, 100, "testDisplayName");
    MessageParcel parcel;
    // Mock failed writes
    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
    bool result = syncFolder.Marshalling(parcel);
    EXPECT_FALSE(result);

    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(false));
    result = syncFolder.Marshalling(parcel);
    EXPECT_FALSE(result);

    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint32(_)).WillOnce(Return(false));
    result = syncFolder.Marshalling(parcel);
    EXPECT_FALSE(result);

    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(false));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint32(_)).WillOnce(Return(true));
    result = syncFolder.Marshalling(parcel);
    EXPECT_FALSE(result);

    // Mock successful writes
    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint32(_)).WillOnce(Return(true));
    result = syncFolder.Marshalling(parcel);
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "SyncFolder_Marshalling_001 end";
}

/**
 * @tc.name: SyncFolder_ReadFromParcel_001
 * @tc.desc: Test SyncFolder ReadFromParcel success
 * @tc.type: FUNC
 */
HWTEST_F(CloudDiskCommTest, SyncFolder_ReadFromParcel_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SyncFolder_ReadFromParcel_001 start";
    SyncFolder syncFolder;
    MessageParcel parcel;

    // Mock failure when reading path
    EXPECT_CALL(*messageParcelMock_, ReadString(_)).WillOnce(Return(false));
    bool result = syncFolder.ReadFromParcel(parcel);
    EXPECT_FALSE(result);

    EXPECT_CALL(*messageParcelMock_, ReadString(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, ReadInt32(_)).WillOnce(Return(false));
    result = syncFolder.ReadFromParcel(parcel);
    EXPECT_FALSE(result);

    EXPECT_CALL(*messageParcelMock_, ReadString(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, ReadInt32(_)).WillOnce(
        DoAll(SetArgReferee<0>(-1), Return(true))); // -1: invalid state
    result = syncFolder.ReadFromParcel(parcel);
    EXPECT_FALSE(result);

    EXPECT_CALL(*messageParcelMock_, ReadString(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, ReadInt32(_)).WillOnce(
        DoAll(SetArgReferee<0>(static_cast<int32_t>(State::MAX_VALUE)), Return(true)));
    result = syncFolder.ReadFromParcel(parcel);
    EXPECT_FALSE(result);
    GTEST_LOG_(INFO) << "SyncFolder_ReadFromParcel_001 end";
}

/**
 * @tc.name: SyncFolder_ReadFromParcel_002
 * @tc.desc: Test SyncFolder ReadFromParcel failure when reading path fails
 * @tc.type: FUNC
 */
HWTEST_F(CloudDiskCommTest, SyncFolder_ReadFromParcel_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SyncFolder_ReadFromParcel_002 start";
    SyncFolder syncFolder;
    MessageParcel parcel;

    EXPECT_CALL(*messageParcelMock_, ReadString(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, ReadInt32(_)).WillOnce(
        DoAll(SetArgReferee<0>(static_cast<int32_t>(State::INACTIVE)), Return(true)));
    EXPECT_CALL(*messageParcelMock_, ReadUint32(_)).WillOnce(Return(false));
    bool result = syncFolder.ReadFromParcel(parcel);
    EXPECT_FALSE(result);

    EXPECT_CALL(*messageParcelMock_, ReadString(_)).WillOnce(Return(true)).WillOnce(Return(false));
    EXPECT_CALL(*messageParcelMock_, ReadInt32(_)).WillOnce(
        DoAll(SetArgReferee<0>(static_cast<int32_t>(State::INACTIVE)), Return(true)));
    EXPECT_CALL(*messageParcelMock_, ReadUint32(_)).WillOnce(Return(true));
    result = syncFolder.ReadFromParcel(parcel);
    EXPECT_FALSE(result);

    EXPECT_CALL(*messageParcelMock_, ReadString(_)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, ReadInt32(_)).WillOnce(
        DoAll(SetArgReferee<0>(static_cast<int32_t>(State::INACTIVE)), Return(true)));
    EXPECT_CALL(*messageParcelMock_, ReadUint32(_)).WillOnce(Return(true));
    result = syncFolder.ReadFromParcel(parcel);
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "SyncFolder_ReadFromParcel_002 end";
}

/**
 * @tc.name: SyncFolder_Unmarshalling_001
 * @tc.desc: Test SyncFolder Unmarshalling
 * @tc.type: FUNC
 */
HWTEST_F(CloudDiskCommTest, SyncFolder_Unmarshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SyncFolder_Unmarshalling_001 start";
    MessageParcel parcel;

    EXPECT_CALL(*messageParcelMock_, ReadString(_)).WillOnce(Return(false));
    SyncFolder *syncFolder = SyncFolder::Unmarshalling(parcel);
    EXPECT_EQ(syncFolder, nullptr);

    EXPECT_CALL(*messageParcelMock_, ReadString(_)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, ReadInt32(_)).WillOnce(
        DoAll(SetArgReferee<0>(static_cast<int32_t>(State::INACTIVE)), Return(true)));
    EXPECT_CALL(*messageParcelMock_, ReadUint32(_)).WillOnce(Return(true));
    syncFolder = SyncFolder::Unmarshalling(parcel);
    EXPECT_NE(syncFolder, nullptr);
    EXPECT_EQ(syncFolder->state_, State::INACTIVE);
    if (syncFolder) {
        delete syncFolder;
        syncFolder = nullptr;
    }
    GTEST_LOG_(INFO) << "SyncFolder_Unmarshalling_001 end";
}

/**
 * @tc.name: SyncFolderExt_Marshalling_001
 * @tc.desc: Test SyncFolderExt Marshalling
 * @tc.type: FUNC
 */
HWTEST_F(CloudDiskCommTest, SyncFolderExt_Marshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SyncFolderExt_Marshalling_001 start";

    SyncFolderExt syncFolderExt("/test/path", State::ACTIVE, 100, "testDisplayName", "testBundleName");
    MessageParcel parcel;

    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(false));
    bool result = syncFolderExt.Marshalling(parcel);
    EXPECT_FALSE(result);

    // Mock failure when writing bundle name
    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true))
        .WillOnce(Return(true)).WillOnce(Return(false));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint32(_)).WillOnce(Return(true));
    result = syncFolderExt.Marshalling(parcel);
    EXPECT_FALSE(result);

    EXPECT_CALL(*messageParcelMock_, WriteString(_)).WillOnce(Return(true))
        .WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteInt32(_)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, WriteUint32(_)).WillOnce(Return(true));
    result = syncFolderExt.Marshalling(parcel);
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "SyncFolderExt_Marshalling_001 end";
}

/**
 * @tc.name: SyncFolderExt_ReadFromParcel_001
 * @tc.desc: Test SyncFolderExt ReadFromParcel success
 * @tc.type: FUNC
 */
HWTEST_F(CloudDiskCommTest, SyncFolderExt_ReadFromParcel_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SyncFolderExt_ReadFromParcel_001 start";

    SyncFolderExt syncFolderExt;
    MessageParcel parcel;

    EXPECT_CALL(*messageParcelMock_, ReadString(_)).WillOnce(Return(false));
    bool result = syncFolderExt.ReadFromParcel(parcel);
    EXPECT_FALSE(result);

    EXPECT_CALL(*messageParcelMock_, ReadString(_)).WillOnce(Return(true))
        .WillOnce(Return(true)).WillOnce(Return(false));
    EXPECT_CALL(*messageParcelMock_, ReadInt32(_)).WillOnce(
        DoAll(SetArgReferee<0>(static_cast<int32_t>(State::INACTIVE)), Return(true)));
    EXPECT_CALL(*messageParcelMock_, ReadUint32(_)).WillOnce(Return(true));
    result = syncFolderExt.ReadFromParcel(parcel);
    EXPECT_FALSE(result);

    EXPECT_CALL(*messageParcelMock_, ReadString(_)).WillOnce(Return(true))
        .WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, ReadInt32(_)).WillOnce(
        DoAll(SetArgReferee<0>(static_cast<int32_t>(State::INACTIVE)), Return(true)));
    EXPECT_CALL(*messageParcelMock_, ReadUint32(_)).WillOnce(Return(true));
    result = syncFolderExt.ReadFromParcel(parcel);
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "SyncFolderExt_ReadFromParcel_001 end";
}

/**
 * @tc.name: SyncFolderExt_Unmarshalling_001
 * @tc.desc: Test SyncFolderExt Unmarshalling success
 * @tc.type: FUNC
 */
HWTEST_F(CloudDiskCommTest, SyncFolderExt_Unmarshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SyncFolderExt_Unmarshalling_001 start";

    MessageParcel parcel;

    EXPECT_CALL(*messageParcelMock_, ReadString(_)).WillOnce(Return(false));
    SyncFolderExt *syncFolderExt = SyncFolderExt::Unmarshalling(parcel);
    EXPECT_EQ(syncFolderExt, nullptr);

    EXPECT_CALL(*messageParcelMock_, ReadString(_)).WillOnce(Return(true))
        .WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*messageParcelMock_, ReadInt32(_)).WillOnce(
        DoAll(SetArgReferee<0>(static_cast<int32_t>(State::INACTIVE)), Return(true)));
    EXPECT_CALL(*messageParcelMock_, ReadUint32(_)).WillOnce(Return(true));
    syncFolderExt = SyncFolderExt::Unmarshalling(parcel);
    EXPECT_NE(syncFolderExt, nullptr);
    EXPECT_EQ(syncFolderExt->state_, State::INACTIVE);
    if (syncFolderExt) {
        delete syncFolderExt;
        syncFolderExt = nullptr;
    }
    GTEST_LOG_(INFO) << "SyncFolderExt_Unmarshalling_001 end";
}
} // namespace FileManagement
} // namespace OHOS