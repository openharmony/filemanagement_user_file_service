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

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Move_0001
 * @tc.name: file_access_ext_stub_impl_Move_0001
 * @tc.desc: Test function of Move interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Move_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Move_0001";
    try {
        EXPECT_CALL(*ability, Move(_, _, _)).WillOnce(Return(OHOS::FileAccessFwk::ERR_OK));

        Uri sourceFile("");
        Uri targetParent("");
        Uri newFile("");
        FileAccessExtStubImpl impl(ability, nullptr);
        int result = impl.Move(sourceFile, targetParent, newFile);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Move_0001";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Copy_0000
 * @tc.name: file_access_ext_stub_impl_Copy_0000
 * @tc.desc: Test function of Copy interface for ERROR because of extension is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Copy_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Copy_0000";
    try {
        Uri sourceFile("");
        Uri destUri("");
        vector<Result> copyResult;
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.Copy(sourceFile, destUri, copyResult);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Copy_0000";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Copy_0001
 * @tc.name: file_access_ext_stub_impl_Copy_0001
 * @tc.desc: Test function of Copy interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Copy_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Copy_0001";
    try {
        EXPECT_CALL(*ability, Copy(_, _, _, _)).WillOnce(Return(OHOS::FileAccessFwk::ERR_OK));

        Uri sourceFile("");
        Uri destUri("");
        vector<Result> copyResult;
        FileAccessExtStubImpl impl(ability, nullptr);
        int result = impl.Copy(sourceFile, destUri, copyResult);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Copy_0001";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_CopyFile_0000
 * @tc.name: file_access_ext_stub_impl_CopyFile_0000
 * @tc.desc: Test function of CopyFile interface for ERROR because of extension is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_CopyFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_CopyFile_0000";
    try {
        Uri sourceFile("");
        Uri destUri("");
        string fileName;
        Uri newFileUri("");
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.CopyFile(sourceFile, destUri, fileName, newFileUri);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_CopyFile_0000";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_CopyFile_0001
 * @tc.name: file_access_ext_stub_impl_CopyFile_0001
 * @tc.desc: Test function of CopyFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_CopyFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_CopyFile_0001";
    try {
        EXPECT_CALL(*ability, CopyFile(_, _, _, _)).WillOnce(Return(OHOS::FileAccessFwk::ERR_OK));

        Uri sourceFile("");
        Uri destUri("");
        string fileName;
        Uri newFileUri("");
        FileAccessExtStubImpl impl(ability, nullptr);
        int result = impl.CopyFile(sourceFile, destUri, fileName, newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_CopyFile_0001";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Rename_0000
 * @tc.name: file_access_ext_stub_impl_Rename_0000
 * @tc.desc: Test function of Rename interface for ERROR because of extension is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Rename_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Rename_0000";
    try {
        Uri sourceFile("");
        Uri newUri("");
        string displayName = "";
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.Rename(sourceFile, displayName, newUri);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Rename_0000";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Rename_0001
 * @tc.name: file_access_ext_stub_impl_Rename_0001
 * @tc.desc: Test function of Rename interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Rename_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Rename_0001";
    try {
        EXPECT_CALL(*ability, Rename(_, _, _)).WillOnce(Return(OHOS::FileAccessFwk::ERR_OK));

        Uri sourceFile("");
        Uri newUri("");
        string displayName = "";
        FileAccessExtStubImpl impl(ability, nullptr);
        int result = impl.Rename(sourceFile, displayName, newUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Rename_0001";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_ListFile_0000
 * @tc.name: file_access_ext_stub_impl_ListFile_0000
 * @tc.desc: Test function of ListFile interface for ERROR because of extension is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_ListFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_ListFile_0000";
    try {
        FileInfo fileInfo;
        int64_t offset = 0;
        const FileFilter filter;
        SharedMemoryInfo memInfo;
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.ListFile(fileInfo, offset, filter, memInfo);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_ListFile_0000";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_ListFile_0001
 * @tc.name: file_access_ext_stub_impl_ListFile_0001
 * @tc.desc: Test function of ListFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_ListFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_ListFile_0001";
    try {
        EXPECT_CALL(*ability, ListFile(_, _, _, _, _)).WillOnce(Return(EPERM));

        FileInfo fileInfo;
        int64_t offset = 0;
        const FileFilter filter;
        SharedMemoryInfo memInfo;
        FileAccessExtStubImpl impl(ability, nullptr);
        int result = impl.ListFile(fileInfo, offset, filter, memInfo);
        EXPECT_EQ(result, EPERM);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_ListFile_0001";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_ListFile_0002
 * @tc.name: file_access_ext_stub_impl_ListFile_0002
 * @tc.desc: Test function of ListFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_ListFile_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_ListFile_0002";
    try {
        EXPECT_CALL(*ability, ListFile(_, _, _, _, _)).WillOnce(Return(OHOS::FileAccessFwk::ERR_OK));

        FileInfo fileInfo;
        int64_t offset = 0;
        const FileFilter filter;
        SharedMemoryInfo memInfo;
        FileAccessExtStubImpl impl(ability, nullptr);
        int result = impl.ListFile(fileInfo, offset, filter, memInfo);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_ListFile_0002";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_ScanFile_0000
 * @tc.name: file_access_ext_stub_impl_ScanFile_0000
 * @tc.desc: Test function of ScanFile interface for ERROR because of extension is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_ScanFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_ScanFile_0000";
    try {
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        const FileFilter filter;
        vector<FileInfo> fileInfoVec;
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_ScanFile_0000";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_ScanFile_0001
 * @tc.name: file_access_ext_stub_impl_ScanFile_0001
 * @tc.desc: Test function of ScanFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_ScanFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_ScanFile_0001";
    try {
        EXPECT_CALL(*ability, ScanFile(_, _, _, _, _)).WillOnce(Return(OHOS::FileAccessFwk::ERR_OK));

        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        const FileFilter filter;
        vector<FileInfo> fileInfoVec;
        FileAccessExtStubImpl impl(ability, nullptr);
        int result = impl.ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_ScanFile_0001";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Query_0000
 * @tc.name: file_access_ext_stub_impl_Query_0000
 * @tc.desc: Test function of Query interface for ERROR because of extension is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Query_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Query_0000";
    try {
        Uri uri("");
        vector<string> columns;
        vector<string> results;
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.Query(uri, columns, results);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Query_0000";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Query_0001
 * @tc.name: file_access_ext_stub_impl_Query_0001
 * @tc.desc: Test function of Query interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Query_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Query_0001";
    try {
        EXPECT_CALL(*ability, Query(_, _, _)).WillOnce(Return(OHOS::FileAccessFwk::ERR_OK));

        Uri uri("");
        vector<string> columns;
        vector<string> results;
        FileAccessExtStubImpl impl(ability, nullptr);
        int result = impl.Query(uri, columns, results);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Query_0001";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_GetFileInfoFromUri_0000
 * @tc.name: file_access_ext_stub_impl_GetFileInfoFromUri_0000
 * @tc.desc: Test function of GetFileInfoFromUri interface for ERROR because of extension is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_GetFileInfoFromUri_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_GetFileInfoFromUri_0000";
    try {
        Uri selectFile("");
        FileInfo fileInfo;
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.GetFileInfoFromUri(selectFile, fileInfo);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_GetFileInfoFromUri_0000";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_GetFileInfoFromUri_0001
 * @tc.name: file_access_ext_stub_impl_GetFileInfoFromUri_0001
 * @tc.desc: Test function of GetFileInfoFromUri interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_GetFileInfoFromUri_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_GetFileInfoFromUri_0001";
    try {
        EXPECT_CALL(*ability, GetFileInfoFromUri(_, _)).WillOnce(Return(OHOS::FileAccessFwk::ERR_OK));

        Uri selectFile("");
        FileInfo fileInfo;
        FileAccessExtStubImpl impl(ability, nullptr);
        int result = impl.GetFileInfoFromUri(selectFile, fileInfo);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_GetFileInfoFromUri_0001";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_GetFileInfoFromRelativePath_0000
 * @tc.name: file_access_ext_stub_impl_GetFileInfoFromRelativePath_0000
 * @tc.desc: Test function of GetFileInfoFromRelativePath interface for ERROR because of extension is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_GetFileInfoFromRelativePath_0000,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_GetFileInfoFromRelativePath_0000";
    try {
        string selectFile;
        FileInfo fileInfo;
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.GetFileInfoFromRelativePath(selectFile, fileInfo);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_GetFileInfoFromRelativePath_0000";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_GetFileInfoFromRelativePath_0001
 * @tc.name: file_access_ext_stub_impl_GetFileInfoFromRelativePath_0001
 * @tc.desc: Test function of GetFileInfoFromRelativePath interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_GetFileInfoFromRelativePath_0001,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_GetFileInfoFromRelativePath_0001";
    try {
        EXPECT_CALL(*ability, GetFileInfoFromRelativePath(_, _)).WillOnce(Return(OHOS::FileAccessFwk::ERR_OK));

        string selectFile;
        FileInfo fileInfo;
        FileAccessExtStubImpl impl(ability, nullptr);
        int result = impl.GetFileInfoFromRelativePath(selectFile, fileInfo);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_GetFileInfoFromRelativePath_0001";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_GetRoots_0000
 * @tc.name: file_access_ext_stub_impl_GetRoots_0000
 * @tc.desc: Test function of GetRoots interface for ERROR because of extension is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_GetRoots_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_GetRoots_0000";
    try {
        vector<RootInfo> rootInfoVec;
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.GetRoots(rootInfoVec);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_GetRoots_0000";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_GetRoots_0001
 * @tc.name: file_access_ext_stub_impl_GetRoots_0001
 * @tc.desc: Test function of GetRoots interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_GetRoots_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_GetRoots_0001";
    try {
        EXPECT_CALL(*ability, GetRoots(_)).WillOnce(Return(OHOS::FileAccessFwk::ERR_OK));

        vector<RootInfo> rootInfoVec;
        FileAccessExtStubImpl impl(ability, nullptr);
        int result = impl.GetRoots(rootInfoVec);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_GetRoots_0001";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Access_0000
 * @tc.name: file_access_ext_stub_impl_Access_0000
 * @tc.desc: Test function of Access interface for ERROR because of extension is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Access_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Access_0000";
    try {
        Uri uri("");
        bool isExist;
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.Access(uri, isExist);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Access_0000";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Access_0001
 * @tc.name: file_access_ext_stub_impl_Access_0001
 * @tc.desc: Test function of Access interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Access_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Access_0001";
    try {
        EXPECT_CALL(*ability, Access(_, _)).WillOnce(Return(OHOS::FileAccessFwk::ERR_OK));

        Uri uri("");
        bool isExist;
        FileAccessExtStubImpl impl(ability, nullptr);
        int result = impl.Access(uri, isExist);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Access_0001";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_StartWatcher_0000
 * @tc.name: file_access_ext_stub_impl_StartWatcher_0000
 * @tc.desc: Test function of StartWatcher interface for ERROR because of extension is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_StartWatcher_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_StartWatcher_0000";
    try {
        Uri uri("");
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.StartWatcher(uri);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_StartWatcher_0000";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_StartWatcher_0001
 * @tc.name: file_access_ext_stub_impl_StartWatcher_0001
 * @tc.desc: Test function of StartWatcher interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_StartWatcher_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_StartWatcher_0001";
    try {
        EXPECT_CALL(*ability, StartWatcher(_)).WillOnce(Return(OHOS::FileAccessFwk::ERR_OK));

        Uri uri("");
        FileAccessExtStubImpl impl(ability, nullptr);
        int result = impl.StartWatcher(uri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_StartWatcher_0001";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_StopWatcher_0000
 * @tc.name: file_access_ext_stub_impl_StopWatcher_0000
 * @tc.desc: Test function of StopWatcher interface for ERROR because of extension is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_StopWatcher_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_StopWatcher_0000";
    try {
        Uri uri("");
        bool isUnregisterAll = false;
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.StopWatcher(uri, isUnregisterAll);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_StopWatcher_0000";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_StopWatcher_0001
 * @tc.name: file_access_ext_stub_impl_StopWatcher_0001
 * @tc.desc: Test function of StopWatcher interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_StopWatcher_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_StopWatcher_0001";
    try {
        EXPECT_CALL(*ability, StopWatcher(_, _)).WillOnce(Return(OHOS::FileAccessFwk::ERR_OK));

        Uri uri("");
        bool isUnregisterAll = false;
        FileAccessExtStubImpl impl(ability, nullptr);
        int result = impl.StopWatcher(uri, isUnregisterAll);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_StopWatcher_0001";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_MoveItem_0000
 * @tc.name: file_access_ext_stub_impl_MoveItem_0000
 * @tc.desc: Test function of MoveItem interface for ERROR because of extension is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_MoveItem_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_MoveItem_0000";
    try {
        Uri sourceFile("");
        Uri targetParent("");
        vector<Result> moveResult;
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.MoveItem(sourceFile, targetParent, moveResult);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_MoveItem_0000";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_MoveItem_0001
 * @tc.name: file_access_ext_stub_impl_MoveItem_0001
 * @tc.desc: Test function of MoveItem interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_MoveItem_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_MoveItem_0001";
    try {
        EXPECT_CALL(*ability, MoveItem(_, _, _, _)).WillOnce(Return(OHOS::FileAccessFwk::ERR_OK));

        Uri sourceFile("");
        Uri targetParent("");
        vector<Result> moveResult;
        FileAccessExtStubImpl impl(ability, nullptr);
        int result = impl.MoveItem(sourceFile, targetParent, moveResult);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_MoveItem_0001";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_MoveFile_0000
 * @tc.name: file_access_ext_stub_impl_MoveFile_0000
 * @tc.desc: Test function of MoveFile interface for ERROR because of extension is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_MoveFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_MoveFile_0000";
    try {
        Uri sourceFile("");
        Uri targetParent("");
        string fileName;
        Uri newFile("");
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.MoveFile(sourceFile, targetParent, fileName, newFile);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_MoveFile_0000";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_MoveFile_0001
 * @tc.name: file_access_ext_stub_impl_MoveFile_0001
 * @tc.desc: Test function of MoveFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_MoveFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_MoveFile_0001";
    try {
        EXPECT_CALL(*ability, MoveFile(_, _, _, _)).WillOnce(Return(OHOS::FileAccessFwk::ERR_OK));

        Uri sourceFile("");
        Uri targetParent("");
        string fileName;
        Uri newFile("");
        FileAccessExtStubImpl impl(ability, nullptr);
        int result = impl.MoveFile(sourceFile, targetParent, fileName, newFile);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_MoveFile_0001";
}
}