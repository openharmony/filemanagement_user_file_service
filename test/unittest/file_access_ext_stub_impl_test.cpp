/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include "bindable.h"
#include "context.h"
#include "extension_context.h"
#include "ability_context.h"
#include "file_access_ext_stub_impl.h"
#include "file_access_extension_info.h"
#include "file_access_framework_errno.h"
#include "file_access_ext_ability_mock.h"
#include "uri.h"

namespace OHOS::FileAccessFwk {
using namespace std;
using namespace testing;
using namespace testing::ext;
using namespace OHOS::AbilityRuntime;

FileAccessExtStub::FileAccessExtStub() {}

FileAccessExtStub::~FileAccessExtStub() {}

int FileAccessExtStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    return 0;
}

class FileAccessExtStubImplTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
    shared_ptr<FileAccessExtAbilityMock> ability = make_shared<FileAccessExtAbilityMock>();
};

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_OpenFile_0000
 * @tc.name: file_access_ext_stub_impl_OpenFile_0000
 * @tc.desc: Test function of OpenFile interface for ERROR because of extension is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8WZ9U
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_OpenFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_OpenFile_0000";
    try {
        int fd;
        Uri uri("");
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.OpenFile(uri, WRITE_READ, fd);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_OpenFile_0000";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_OpenFile_0001
 * @tc.name: file_access_ext_stub_impl_OpenFile_0001
 * @tc.desc: Test function of OpenFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8WZ9U
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_OpenFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_OpenFile_0001";
    try {
        EXPECT_CALL(*ability, OpenFile(_, _, _)).WillOnce(Return(OHOS::FileAccessFwk::ERR_OK));

        int fd;
        Uri uri("");
        FileAccessExtStubImpl impl(ability, nullptr);
        int result = impl.OpenFile(uri, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_OpenFile_0001";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_CreateFile_0000
 * @tc.name: file_access_ext_stub_impl_CreateFile_0000
 * @tc.desc: Test function of CreateFile interface for ERROR because of extension is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8WZ9U
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_CreateFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_CreateFile_0000";
    try {
        Uri uri("");
        Uri newUri("");
        string displayName = "";
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.CreateFile(uri, displayName, newUri);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_CreateFile_0000";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_CreateFile_0001
 * @tc.name: file_access_ext_stub_impl_CreateFile_0001
 * @tc.desc: Test function of CreateFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8WZ9U
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_CreateFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_CreateFile_0001";
    try {
        EXPECT_CALL(*ability, CreateFile(_, _, _)).WillOnce(Return(OHOS::FileAccessFwk::ERR_OK));

        Uri uri("");
        Uri newUri("");
        string displayName = "";
        FileAccessExtStubImpl impl(ability, nullptr);
        int result = impl.CreateFile(uri, displayName, newUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_CreateFile_0001";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Mkdir_0000
 * @tc.name: file_access_ext_stub_impl_Mkdir_0000
 * @tc.desc: Test function of Mkdir interface for ERROR because of extension is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8WZ9U
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Mkdir_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Mkdir_0000";
    try {
        Uri uri("");
        Uri newUri("");
        string displayName = "";
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.Mkdir(uri, displayName, newUri);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Mkdir_0000";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Mkdir_0001
 * @tc.name: file_access_ext_stub_impl_Mkdir_0001
 * @tc.desc: Test function of Mkdir interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8WZ9U
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Mkdir_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Mkdir_0001";
    try {
        EXPECT_CALL(*ability, Mkdir(_, _, _)).WillOnce(Return(OHOS::FileAccessFwk::ERR_OK));

        Uri uri("");
        Uri newUri("");
        string displayName = "";
        FileAccessExtStubImpl impl(ability, nullptr);
        int result = impl.Mkdir(uri, displayName, newUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Mkdir_0001";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Delete_0000
 * @tc.name: file_access_ext_stub_impl_Delete_0000
 * @tc.desc: Test function of Delete interface for ERROR because of extension is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8WZ9U
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Delete_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Delete_0000";
    try {
        Uri sourceFile("");
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.Delete(sourceFile);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Delete_0000";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Delete_0001
 * @tc.name: file_access_ext_stub_impl_Delete_0001
 * @tc.desc: Test function of Delete interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8WZ9U
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Delete_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Delete_0001";
    try {
        EXPECT_CALL(*ability, Delete(_)).WillOnce(Return(OHOS::FileAccessFwk::ERR_OK));

        Uri sourceFile("");
        FileAccessExtStubImpl impl(ability, nullptr);
        int result = impl.Delete(sourceFile);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Delete_0001";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Move_0000
 * @tc.name: file_access_ext_stub_impl_Move_0000
 * @tc.desc: Test function of Move interface for ERROR because of extension is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8WZ9U
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Move_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Move_0000";
    try {
        Uri sourceFile("");
        Uri targetParent("");
        Uri newFile("");
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.Move(sourceFile, targetParent, newFile);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Move_0000";
}
}