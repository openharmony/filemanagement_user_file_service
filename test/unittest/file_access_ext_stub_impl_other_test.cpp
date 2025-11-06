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

 /**
 * @tc.number: user_file_service_file_access_ext_stub_impl_OpenFile_0004
 * @tc.name: file_access_ext_stub_impl_OpenFile_0004
 * @tc.desc: Test function of OpenFile interface for ERROR because of OpenFile_0004
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8WZ9U
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_OpenFile_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_OpenFile_0004";
    try {
        int fd;
        Urie uri("/test/test../../test");
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.OpenFile(uri, WRITE_READ, fd);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_OpenFile_0004";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_CreateFile_0003
 * @tc.name: file_access_ext_stub_impl_CreateFile_0003
 * @tc.desc: Test function of CreateFile interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8WZ9U
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_CreateFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_CreateFile_0003";
    try {
        Urie uri("/test/test../../test");
        Urie newUri("");
        string displayName = "";
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.CreateFile(uri, displayName, newUri);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_CreateFile_0003";
}


/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Mkdir_0003
 * @tc.name: file_access_ext_stub_impl_Mkdir_0003
 * @tc.desc: Test function of Mkdir interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8WZ9U
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Mkdir_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Mkdir_0003";
    try {
        Urie uri("/test/test../../test");
        Urie newUri("");
        string displayName = "";
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.Mkdir(uri, displayName, newUri);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Mkdir_0003";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Delete_0003
 * @tc.name: file_access_ext_stub_impl_Delete_0003
 * @tc.desc: Test function of Delete interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8WZ9U
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Delete_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Delete_0003";
    try {
        Urie sourceFile("/test../test../../test");
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.Delete(sourceFile);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Delete_0003";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Move_0003
 * @tc.name: file_access_ext_stub_impl_Move_0003
 * @tc.desc: Test function of Move interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8WZ9U
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Move_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Move_0003";
    try {
        Urie sourceFile("/test/test../../test");
        Urie targetParent("");
        Urie newFile("");
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.Move(sourceFile, targetParent, newFile);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Move_0003";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Move_0004
 * @tc.name: file_access_ext_stub_impl_Move_0004
 * @tc.desc: Test function of Move interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8WZ9U
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Move_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Move_0004";
    try {
        Urie sourceFile("");
        Urie targetParent("/test/test../../test");
        Urie newFile("");
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.Move(sourceFile, targetParent, newFile);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Move_0004";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Copy_0003
 * @tc.name: file_access_ext_stub_impl_Copy_0003
 * @tc.desc: Test function of Copy interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Copy_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Copy_0003";
    try {
        Urie sourceFile("/test/test../../test");
        Urie destUri("");
        vector<Result> copyResult;
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int retCode;
        int result = impl.Copy(sourceFile, destUri, copyResult, retCode);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Copy_0003";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Copy_0004
 * @tc.name: file_access_ext_stub_impl_Copy_0004
 * @tc.desc: Test function of Copy interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Copy_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Copy_0004";
    try {
        Urie sourceFile("");
        Urie destUri("/test/test../../test");
        vector<Result> copyResult;
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int retCode;
        int result = impl.Copy(sourceFile, destUri, copyResult, retCode);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Copy_0004";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_CopyFile_0003
 * @tc.name: file_access_ext_stub_impl_CopyFile_0003
 * @tc.desc: Test function of CopyFile interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_CopyFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_CopyFile_0003";
    try {
        Urie sourceFile("/test/test../../test");
        Urie destUri("");
        string fileName;
        Urie newFileUri("");
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.CopyFile(sourceFile, destUri, fileName, newFileUri);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_CopyFile_0003";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_CopyFile_0004
 * @tc.name: file_access_ext_stub_impl_CopyFile_0004
 * @tc.desc: Test function of CopyFile interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_CopyFile_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_CopyFile_0004";
    try {
        Urie sourceFile("");
        Urie destUri("/test/test../../test");
        string fileName;
        Urie newFileUri("");
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.CopyFile(sourceFile, destUri, fileName, newFileUri);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_CopyFile_0004";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Rename_0003
 * @tc.name: file_access_ext_stub_impl_Rename_0003
 * @tc.desc: Test function of Rename interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Rename_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Rename_0003";
    try {
        Urie sourceFile("/test/test../../test");
        Urie newUri("");
        string displayName = "";
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.Rename(sourceFile, displayName, newUri);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Rename_0003";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_ListFile_0005
 * @tc.name: file_access_ext_stub_impl_ListFile_0005
 * @tc.desc: Test function of ListFile interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_ListFile_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_ListFile_0005";
    try {
        FileInfo fileInfo;
        fileInfo.uri = "/test/test../../test";
        int64_t offset = 0;
        const FileFilter filter;
        SharedMemoryInfo memInfo;
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.ListFile(fileInfo, offset, filter, memInfo);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_ListFile_0005";
}


/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_ScanFile_0003
 * @tc.name: file_access_ext_stub_impl_ScanFile_0003
 * @tc.desc: Test function of ScanFile interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_ScanFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_ScanFile_0003";
    try {
        FileInfo fileInfo;
        fileInfo.uri = "/test/test../../test";
        int64_t offset = 0;
        int64_t maxCount = 0;
        const FileFilter filter;
        vector<FileInfo> fileInfoVec;
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_ScanFile_0003";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Query_0003
 * @tc.name: file_access_ext_stub_impl_Query_0003
 * @tc.desc: Test function of Query interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Query_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Query_0003";
    try {
        Urie uri("/test/test../../test");
        vector<string> columns;
        vector<string> results;
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.Query(uri, columns, results);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Query_0003";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_GetFileInfoFromUri_0003
 * @tc.name: file_access_ext_stub_impl_GetFileInfoFromUri_0003
 * @tc.desc: Test function of GetFileInfoFromUri interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XFLP
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_GetFileInfoFromUri_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_GetFileInfoFromUri_0003";
    try {
        Urie selectFile("/test/test../../test");
        FileInfo fileInfo;
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.GetFileInfoFromUri(selectFile, fileInfo);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_GetFileInfoFromUri_0003";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_GetFileInfoFromRelativePath_0003
 * @tc.name: file_access_ext_stub_impl_GetFileInfoFromRelativePath_0003
 * @tc.desc: Test function of GetFileInfoFromRelativePath interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_GetFileInfoFromRelativePath_0003,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_GetFileInfoFromRelativePath_0003";
    try {
        std::string selectFile = "/test/test../../test";
        FileInfo fileInfo;
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.GetFileInfoFromRelativePath(selectFile, fileInfo);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_GetFileInfoFromRelativePath_0003";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_Access_0003
 * @tc.name: file_access_ext_stub_impl_Access_0003
 * @tc.desc: Test function of Access interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_Access_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_Access_0003";
    try {
        Urie uri("/test/test../../test");
        bool isExist;
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.Access(uri, isExist);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_Access_0003";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_StartWatcher_0003
 * @tc.name: file_access_ext_stub_impl_StartWatcher_0003
 * @tc.desc: Test function of StartWatcher interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_StartWatcher_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_StartWatcher_0003";
    try {
        Urie uri("/test/test../../test");
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.StartWatcher(uri);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_StartWatcher_0003";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_StopWatcher_0003
 * @tc.name: file_access_ext_stub_impl_StopWatcher_0003
 * @tc.desc: Test function of StopWatcher interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_StopWatcher_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_StopWatcher_0003";
    try {
        Urie uri("/test/test../../test");
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.StopWatcher(uri);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_StopWatcher_0003";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_MoveItem_0003
 * @tc.name: file_access_ext_stub_impl_MoveItem_0003
 * @tc.desc: Test function of MoveItem interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_MoveItem_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_MoveItem_0003";
    try {
        EXPECT_CALL(*accesstokenMock_, VerifyAccessToken(_, _)).WillOnce(Return(0));

        Urie sourceFile("/test/test../../test");
        Urie targetParent("");
        vector<Result> moveResult;
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int retCode;
        int result = impl.MoveItem(sourceFile, targetParent, moveResult, retCode);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_MoveItem_0003";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_MoveItem_0004
 * @tc.name: file_access_ext_stub_impl_MoveItem_0004
 * @tc.desc: Test function of MoveItem interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_MoveItem_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_MoveItem_0004";
    try {
        EXPECT_CALL(*accesstokenMock_, VerifyAccessToken(_, _)).WillOnce(Return(0));

        Urie sourceFile("");
        Urie targetParent("/test/test../../test");
        vector<Result> moveResult;
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int retCode;
        int result = impl.MoveItem(sourceFile, targetParent, moveResult, retCode);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_MoveItem_0004";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_MoveFile_0003
 * @tc.name: file_access_ext_stub_impl_MoveFile_0003
 * @tc.desc: Test function of MoveFile interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_MoveFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_MoveFile_0003";
    try {
        Urie sourceFile("/test/test../../test");
        Urie targetParent("");
        string fileName;
        Urie newFile("");
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.MoveFile(sourceFile, targetParent, fileName, newFile);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_MoveFile_0003";
}

/**
 * @tc.number: user_file_service_file_access_ext_stub_impl_MoveFile_0004
 * @tc.name: file_access_ext_stub_impl_MoveFile_0004
 * @tc.desc: Test function of MoveFile interface for ERROR because of uri is invalid
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8XN2E
 */
HWTEST_F(FileAccessExtStubImplTest, file_access_ext_stub_impl_MoveFile_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-begin file_access_ext_stub_impl_MoveFile_0004";
    try {
        Urie sourceFile("");
        Urie targetParent("/test/test../../test");
        string fileName;
        Urie newFile("");
        FileAccessExtStubImpl impl(nullptr, nullptr);
        int result = impl.MoveFile(sourceFile, targetParent, fileName, newFile);
        EXPECT_EQ(result, E_URIS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessExtStubImplTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessExtStubImplTest-end file_access_ext_stub_impl_MoveFile_0004";
}
