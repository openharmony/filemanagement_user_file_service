/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <cstdio>
#include <thread>
#include <unistd.h>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "medialibrary_file_access_test_basic.h"
#include "accesstoken_kit.h"
#include "file_access_extension_info.h"
#include "file_access_framework_errno.h"
#include "file_access_helper.h"
#include "iservice_registry.h"
#include "image_source.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

namespace OHOS::FileAccessFwk {
using json = nlohmann::json;
const int INIT_THREADS_NUMBER = 4;
const int ACTUAL_SUCCESS_THREADS_NUMBER = 1;
int g_num = 0;
shared_ptr<FileAccessHelper> g_fah = nullptr;
Uri g_newDirUri("");

/**
 * @tc.number: user_file_service_medialibrary_file_access_OpenFile_0000
 * @tc.name: medialibrary_file_access_OpenFile_0000
 * @tc.desc: Test function of OpenFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_OpenFile_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_OpenFile_0000";
    try {
        Uri parentUri = GetParentUri();
        bool isExist = false;
        int result = g_fah->Access(g_newDirUri, isExist);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        if (!isExist) {
            result = g_fah->Mkdir(parentUri, "Download", g_newDirUri);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        }
        Uri newDirUriTest("file://media/root/file");
        FileInfo fileInfo;
        fileInfo.uri = newDirUriTest.ToString();
        int64_t offset = 0;
        int64_t maxCount = 1000;
        std::vector<FileInfo> fileInfoVec;
        FileFilter filter;
        result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_GE(fileInfoVec.size(), OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < fileInfoVec.size(); i++) {
            if (fileInfoVec[i].fileName.compare("Download") == 0) {
                g_newDirUri = Uri(fileInfoVec[i].uri);
                break;
            }
        }
        result = g_fah->Mkdir(g_newDirUri, "test1", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newFileUri("");
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_OpenFile_0000.txt", newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int fd;
        result = g_fah->OpenFile(newFileUri, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0000 result:" << result;
        close(fd);
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_accsess_OpenFile_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_OpenFile_0000";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_OpenFile_0001
 * @tc.name: medialibrary_file_access_OpenFile_0001
 * @tc.desc: Test function of OpenFile interface for ERROR which Uri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_OpenFile_0001, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_OpenFile_0001";
    try {
        Uri uri("");
        int fd;
        int result = g_fah->OpenFile(uri, WRITE_READ, fd);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0001 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_OpenFile_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_OpenFile_0001";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_OpenFile_0002
 * @tc.name: medialibrary_file_access_OpenFile_0002
 * @tc.desc: Test function of OpenFile interface for ERROR which Uri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_OpenFile_0002, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_OpenFile_0002";
    try {
        Uri uri("storage/media/100/local/files/Download/medialibrary_file_access_OpenFile_0002.txt");
        int fd;
        int result = g_fah->OpenFile(uri, WRITE_READ, fd);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0002 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_OpenFile_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_OpenFile_0002";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_OpenFile_0003
 * @tc.name: medialibrary_file_access_OpenFile_0003
 * @tc.desc: Test function of OpenFile interface for ERROR which Uri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_OpenFile_0003, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_OpenFile_0003";
    try {
        Uri uri("~!@#$%^&*()_");
        int fd;
        int result = g_fah->OpenFile(uri, WRITE_READ, fd);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0003 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_OpenFile_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_OpenFile_0003";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_OpenFile_0004
 * @tc.name: medialibrary_file_access_OpenFile_0004
 * @tc.desc: Test function of OpenFile interface for ERROR which flag is -1.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_OpenFile_0004, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_OpenFile_0004";
    try {
        Uri newFileUri("");
        int result = g_fah->CreateFile(g_newDirUri, "medialibrary_file_access_OpenFile_0004.txt", newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int fd;
        int flag = -1;
        result = g_fah->OpenFile(newFileUri, flag, fd);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0004 result:" << result;
        result = g_fah->Delete(newFileUri);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_OpenFile_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_OpenFile_0004";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_OpenFile_0005
 * @tc.name: medialibrary_file_access_OpenFile_0005
 * @tc.desc: Test function of OpenFile interface for SUCCESS which flag is 0.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_OpenFile_0005, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_OpenFile_0005";
    try {
        Uri newFileUri("");
        int result = g_fah->CreateFile(g_newDirUri, "medialibrary_file_access_OpenFile_0005.txt", newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int fd;
        result = g_fah->OpenFile(newFileUri, READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0005 result:" << result;
        close(fd);
        result = g_fah->Delete(newFileUri);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_OpenFile_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_OpenFile_0005";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_OpenFile_0006
 * @tc.name: medialibrary_file_access_OpenFile_0006
 * @tc.desc: Test function of OpenFile interface for SUCCESS which flag is 1.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_OpenFile_0006, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_OpenFile_0006";
    try {
        Uri newFileUri("");
        int result = g_fah->CreateFile(g_newDirUri, "medialibrary_file_access_OpenFile_0006.txt", newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int fd;
        result = g_fah->OpenFile(newFileUri, WRITE, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0006 result:" << result;
        close(fd);
        result = g_fah->Delete(newFileUri);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_OpenFile_0006 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_OpenFile_0006";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_OpenFile_0007
 * @tc.name: medialibrary_file_access_OpenFile_0007
 * @tc.desc: Test function of OpenFile interface for SUCCESS which flag is 2.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_OpenFile_0007, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_OpenFile_0007";
    try {
        Uri newFileUri("");
        int result = g_fah->CreateFile(g_newDirUri, "medialibrary_file_access_OpenFile_0007.txt", newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int fd;
        result = g_fah->OpenFile(newFileUri, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0007 result:" << result;
        close(fd);
        result = g_fah->Delete(newFileUri);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_OpenFile_0007 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_OpenFile_0007";
}

static void OpenFileTdd(shared_ptr<FileAccessHelper> fahs, Uri uri, int flag, int fd)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_OpenFileTdd";
    int ret = fahs->OpenFile(uri, flag, fd);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(ERROR) << "OpenFileTdd get result error, code:" << ret;
        return;
    }
    EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_GE(fd, OHOS::FileAccessFwk::ERR_OK);
    g_num++;
    close(fd);
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_OpenFileTdd";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_OpenFile_0008
 * @tc.name: medialibrary_file_access_OpenFile_0008
 * @tc.desc: Test function of OpenFile interface for SUCCESS which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_OpenFile_0008, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_OpenFile_0008";
    try {
        Uri newFileUri("");
        int fd;
        g_num = 0;
        std::string displayName = "test1.txt";
        int result = g_fah->CreateFile(g_newDirUri, displayName, newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (int j = 0; j < INIT_THREADS_NUMBER; j++) {
            std::thread execthread(OpenFileTdd, g_fah, newFileUri, WRITE_READ, fd);
            execthread.join();
        }
        EXPECT_EQ(g_num, INIT_THREADS_NUMBER);
        result = g_fah->Delete(newFileUri);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_OpenFile_0008 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_OpenFile_0008";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Rename_0000
 * @tc.name: medialibrary_file_access_Rename_0000
 * @tc.desc: Test function of Rename interface for SUCCESS which rename file.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Rename_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Rename_0000";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        result = g_fah->CreateFile(newDirUriTest, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri renameUri("");
        result = g_fah->Rename(testUri, "test2.txt", renameUri);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Rename_0000 result:" << result;
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Rename_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Rename_0000";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Rename_0001
 * @tc.name: medialibrary_file_access_Rename_0001
 * @tc.desc: Test function of Rename interface for SUCCESS which rename folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Rename_0001, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Rename_0001";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri renameUri("");
        result = g_fah->Rename(newDirUriTest, "testRename", renameUri);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Rename_0001 result:" << result;
        result = g_fah->Delete(renameUri);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Rename_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Rename_0001";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Rename_0002
 * @tc.name: medialibrary_file_access_Rename_0002
 * @tc.desc: Test function of Rename interface for ERROR which sourceFileUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Rename_0002, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Rename_0002";
    try {
        Uri renameUri("");
        Uri sourceFileUri("");
        int result = g_fah->Rename(sourceFileUri, "testRename.txt", renameUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Rename_0002 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Rename_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Rename_0002";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Rename_0003
 * @tc.name: medialibrary_file_access_Rename_0003
 * @tc.desc: Test function of Rename interface for ERROR which sourceFileUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Rename_0003, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Rename_0003";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        result = g_fah->CreateFile(newDirUriTest, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri renameUri("");
        Uri sourceFileUri("storage/media/100/local/files/Download/test/test.txt");
        result = g_fah->Rename(sourceFileUri, "testRename.txt", renameUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Rename_0003 result:" << result;
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Rename_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Rename_0003";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Rename_0004
 * @tc.name: medialibrary_file_access_Rename_0004
 * @tc.desc: Test function of Rename interface for ERROR which sourceFileUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Rename_0004, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Rename_0004";
    try {
        Uri renameUri("");
        Uri sourceFileUri("~!@#$%^&*()_");
        int result = g_fah->Rename(sourceFileUri, "testRename.txt", renameUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Rename_0004 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Rename_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Rename_0004";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Rename_0005
 * @tc.name: medialibrary_file_access_Rename_0005
 * @tc.desc: Test function of Rename interface for ERROR which displayName is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Rename_0005, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Rename_0005";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        result = g_fah->CreateFile(newDirUriTest, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri renameUri("");
        result = g_fah->Rename(testUri, "", renameUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Rename_0005 result:" << result;
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Rename_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Rename_0005";
}

static void RenameTdd(shared_ptr<FileAccessHelper> fahs, Uri sourceFile, std::string displayName, Uri newFile)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_RenameTdd";
    int ret = fahs->Rename(sourceFile, displayName, newFile);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(ERROR) << "RenameTdd get result error, code:" << ret;
        return;
    }
    EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_NE(newFile.ToString(), "");
    g_num++;
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_RenameTdd";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Rename_0006
 * @tc.name: medialibrary_file_access_Rename_0006
 * @tc.desc: Test function of Rename interface for SUCCESS which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Rename_0006, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Rename_0006";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri{""};
        std::string displayName1 = "test1.txt";
        std::string displayName2 = "test2.txt";
        Uri renameUri("");
        result = g_fah->CreateFile(newDirUriTest, displayName1, testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (int j = 0; j < INIT_THREADS_NUMBER; j++) {
            std::thread execthread(RenameTdd, g_fah, testUri, displayName2, renameUri);
            execthread.join();
        }
        EXPECT_GE(g_num, ACTUAL_SUCCESS_THREADS_NUMBER);
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Rename_0006 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Rename_0006";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_ScanFile_0000
 * @tc.name: medialibrary_file_access_ScanFile_0000
 * @tc.desc: Test function of ScanFile interface for SUCCESS which scan root directory with filter.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000HB866
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_ScanFile_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_ScanFile_0000";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        FileInfo fileInfo;
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_ScanFile_0000.q1w2e3r4", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_ScanFile_0000.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        fileInfo.uri = "file://media/root";
        int64_t offset = 0;
        int64_t maxCount = 1000;
        std::vector<FileInfo> fileInfoVec;
        FileFilter filter({".q1w2e3r4"}, {}, {}, -1, -1, false, true);
        result = g_fah->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_GE(fileInfoVec.size(), 1);
        GTEST_LOG_(INFO) << "ScanFile_0000 result:" << fileInfoVec.size() << endl;
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_ScanFile_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_ScanFile_0000";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_ScanFile_0001
 * @tc.name: medialibrary_file_access_ScanFile_0001
 * @tc.desc: Test function of ScanFile interface for SUCCESS which scan root directory with no filter.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000HB866
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_ScanFile_0001, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_ScanFile_0001";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        FileInfo fileInfo;
        fileInfo.uri = "file://media/root";
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_ScanFile_0001.q1w2e3r4", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_ScanFile_0001.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int64_t offset = 0;
        int64_t maxCount = 1000;
        std::vector<FileInfo> fileInfoVec;
        FileFilter filter({}, {}, {}, -1, -1, false, false);
        result = g_fah->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_GE(fileInfoVec.size(), 2);
        GTEST_LOG_(INFO) << "ScanFile_0000 result:" << fileInfoVec.size() << endl;
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_ScanFile_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_ScanFile_0001";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_ScanFile_0002
 * @tc.name: medialibrary_file_access_ScanFile_0002
 * @tc.desc: Test function of ScanFile interface for SUCCESS which self created directory with filter.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000HB866
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_ScanFile_0002, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_ScanFile_0002";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_ScanFile_0002.q1w2e3r4", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_ScanFile_0000.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_ScanFile_0000.docx", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        FileInfo fileInfo;
        fileInfo.uri = newDirUriTest.ToString();
        int64_t offset = 0;
        int64_t maxCount = 1000;
        std::vector<FileInfo> fileInfoVec;
        FileFilter filter({".q1w2e3r4"}, {}, {}, -1, -1, false, true);
        result = g_fah->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(fileInfoVec.size(), 1);
        FileFilter filter1({".q1w2e3r4", ".txt"}, {}, {}, -1, -1, false, true);
        result = g_fah->ScanFile(fileInfo, offset, maxCount, filter1, fileInfoVec);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(fileInfoVec.size(), 2);
        GTEST_LOG_(INFO) << "ScanFile_0002 result:" << fileInfoVec.size() << endl;
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_ScanFile_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_ScanFile_0002";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_ScanFile_0003
 * @tc.name: medialibrary_file_access_ScanFile_0003
 * @tc.desc: Test function of ScanFile interface for SUCCESS which self created directory with filter.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000HB866
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_ScanFile_0003, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_ScanFile_0003";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_ScanFile_0003.q1w2e3r4", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        FileInfo fileInfo;
        fileInfo.uri = newDirUriTest.ToString();
        int64_t offset = 0;
        int64_t maxCount = 1000;
        std::vector<FileInfo> fileInfoVec;
        FileFilter filter({".q1w2e3r4"}, {}, {}, -1, -1, false, true);
        result = g_fah->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(fileInfoVec.size(), 1);
        GTEST_LOG_(INFO) << "ScanFile_0003 result:" << fileInfoVec.size() << endl;
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_ScanFile_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_ScanFile_0003";
}

static void ScanFileTdd(FileInfo fileInfo, FileFilter filter, std::vector<FileInfo> fileInfoVec,
    shared_ptr<FileAccessHelper> g_fah)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_ScanFileTdd";
    int64_t offset = 0;
    int64_t maxCount = 1000;
    int ret = g_fah->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(ERROR) << "ScanFileTdd get result error, code:" << ret;
        return;
    }
    EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), 1);
    g_num++;
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_ScanFileTdd";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_ScanFile_0004
 * @tc.name: medialibrary_file_access_ScanFile_0004
 * @tc.desc: Test function of ScanFile interface for SUCCESS which scan root directory with Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000HB866
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_ScanFile_0004, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_ScanFile_0004";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        FileInfo fileInfo;
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_ScanFile_0000.q1w2e3r4", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_ScanFile_0000.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        fileInfo.uri = "file://media/root";
        std::vector<FileInfo> fileInfoVec;
        FileFilter filter({".q1w2e3r4"}, {}, {}, -1, -1, false, true);
        g_num = 0;
        for (int j = 0; j < INIT_THREADS_NUMBER; j++) {
            std::thread execthread(ScanFileTdd, fileInfo, filter, fileInfoVec, g_fah);
            execthread.join();
        }
        EXPECT_GE(g_num, ACTUAL_SUCCESS_THREADS_NUMBER);
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_ScanFile_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_ScanFile_0004";
}

static bool ReplaceBundleNameFromPath(std::string &path, const std::string &newName)
{
    Uri uri(path);
    std::string scheme = uri.GetScheme();
    if (scheme == FILE_SCHEME_NAME) {
        std::string curName = uri.GetAuthority();
        if (curName.empty()) {
            return false;
        }
        path.replace(path.find(curName), curName.length(), newName);
        return true;
    }

    std::string tPath = Uri(path).GetPath();
    if (tPath.empty()) {
        GTEST_LOG_(INFO) << "Uri path error.";
        return false;
    }

    if (tPath.front() != '/') {
        GTEST_LOG_(INFO) << "Uri path format error.";
        return false;
    }

    auto index = tPath.substr(1).find_first_of("/");
    auto bundleName = tPath.substr(1, index);
    if (bundleName.empty()) {
        GTEST_LOG_(INFO) << "bundleName empty.";
        return false;
    }

    path.replace(path.find(bundleName), bundleName.length(), newName);
    return true;
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_ScanFile_0005
 * @tc.name: medialibrary_file_access_ScanFile_0005
 * @tc.desc: Test function of ScanFile interface for FAILED because of GetProxyByUri failed.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000HB866
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_ScanFile_0005, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_ScanFile_0005";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_ScanFile_0005.q1w2e3r4", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        std::string str = testUri.ToString();
        if (!ReplaceBundleNameFromPath(str, "ohos.com.NotExistBundleName")) {
            GTEST_LOG_(INFO) << "replace BundleName failed.";
            EXPECT_TRUE(false);
        }
        FileInfo fileInfo;
        fileInfo.uri = str;
        int64_t offset = 0;
        int64_t maxCount = 1000;
        std::vector<FileInfo> fileInfoVec;
        FileFilter filter({".q1w2e3r4"}, {}, {}, -1, -1, false, true);
        result = g_fah->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, OHOS::FileAccessFwk::E_IPCS);
        EXPECT_EQ(fileInfoVec.size(), 0);
        GTEST_LOG_(INFO) << "ScanFile_0005 result:" << fileInfoVec.size() << endl;
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_ScanFile_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_ScanFile_0005";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_GetRoots_0000
 * @tc.name: medialibrary_file_access_GetRoots_0000
 * @tc.desc: Test function of GetRoots interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_GetRoots_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_GetRoots_0000";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_GT(info.size(), OHOS::FileAccessFwk::ERR_OK);
        if (info.size() > OHOS::FileAccessFwk::ERR_OK) {
            GTEST_LOG_(INFO) << info[0].uri;
            GTEST_LOG_(INFO) << info[0].displayName;
            GTEST_LOG_(INFO) << info[0].deviceType;
            GTEST_LOG_(INFO) << info[0].deviceFlags;
        }
        string uri = "file://media/root";
        string displayName = "LOCAL";
        EXPECT_EQ(info[0].uri, uri);
        EXPECT_EQ(info[0].displayName, displayName);
        EXPECT_EQ(info[0].deviceType, DEVICE_LOCAL_DISK);
        EXPECT_EQ(info[0].deviceFlags, DEVICE_FLAG_SUPPORTS_READ | DEVICE_FLAG_SUPPORTS_WRITE);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_GetRoots_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_GetRoots_0000";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_GetFileInfoFromUri_0000
 * @tc.name: medialibrary_file_access_GetFileInfoFromUri_0000
 * @tc.desc: Test function of GetFileInfoFromUri interface.
 * @tc.desc: convert the root directory uri to fileinfo and call listfile for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000HRLBS
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_GetFileInfoFromUri_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin medialibrary_file_access_GetFileInfoFromUri_0000";
    try {
        vector<RootInfo> infos;
        int result = g_fah->GetRoots(infos);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < infos.size(); i++) {
            Uri parentUri(infos[i].uri);
            FileInfo fileinfo;
            result = g_fah->GetFileInfoFromUri(parentUri, fileinfo);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            int64_t offset = 0;
            int64_t maxCount = 1000;
            FileFilter filter;
            std::vector<FileInfo> fileInfoVecTemp;
            result = g_fah->ListFile(fileinfo, offset, maxCount, filter, fileInfoVecTemp);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_GE(fileInfoVecTemp.size(), OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_GetFileInfoFromUri_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end medialibrary_file_access_GetFileInfoFromUri_0000";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_GetFileInfoFromUri_0001
 * @tc.name: medialibrary_file_access_GetFileInfoFromUri_0001
 * @tc.desc: Test function of GetFileInfoFromUri interface.
 * @tc.desc: convert the general directory uri to fileinfo and call listfile for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000HRLBS
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_GetFileInfoFromUri_0001, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin medialibrary_file_access_GetFileInfoFromUri_0001";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "testDir", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        FileInfo dirInfo;
        result = g_fah->GetFileInfoFromUri(newDirUriTest, dirInfo);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        int64_t offset = 0;
        int64_t maxCount = 1000;
        FileFilter filter;
        std::vector<FileInfo> fileInfoVec;
        result = g_fah->ListFile(dirInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_GE(fileInfoVec.size(), OHOS::FileAccessFwk::ERR_OK);

        result = g_fah->Delete(newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_GetFileInfoFromUri_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end medialibrary_file_access_GetFileInfoFromUri_0001";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_GetFileInfoFromUri_0002
 * @tc.name: medialibrary_file_access_GetFileInfoFromUri_0002
 * @tc.desc: Test function of GetFileInfoFromUri interface.
 * @tc.desc: convert the regular file uri to fileinfo and call listfile for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000HRLBS
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_GetFileInfoFromUri_0002, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin medialibrary_file_access_GetFileInfoFromUri_0002";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "testDir", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newFileUri("");
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_GetFileInfoFromUri_0002.txt", newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        FileInfo fileinfo;
        result = g_fah->GetFileInfoFromUri(newFileUri, fileinfo);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        int64_t offset = 0;
        int64_t maxCount = 1000;
        FileFilter filter;
        std::vector<FileInfo> fileInfoVecTemp;
        result = g_fah->ListFile(fileinfo, offset, maxCount, filter, fileInfoVecTemp);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(fileInfoVecTemp.size(), OHOS::FileAccessFwk::ERR_OK);

        result = g_fah->Delete(newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_GetFileInfoFromUri_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end medialibrary_file_access_GetFileInfoFromUri_0002";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_GetFileInfoFromUri_0003
 * @tc.name: medialibrary_file_access_GetFileInfoFromUri_0003
 * @tc.desc: Test function of GetFileInfoFromUri interface.
 * @tc.desc: convert the root directory uri to fileinfo for CheckUri failed.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000HRLBS
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_GetFileInfoFromUri_0003, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin medialibrary_file_access_GetFileInfoFromUri_0003";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(std::string("\?\?\?\?/") + info[i].uri);
            FileInfo fileinfo;
            result = g_fah->GetFileInfoFromUri(parentUri, fileinfo);
            EXPECT_EQ(result, OHOS::FileAccessFwk::E_URIS);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_GetFileInfoFromUri_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end medialibrary_file_access_GetFileInfoFromUri_0003";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_GetFileInfoFromUri_0004
 * @tc.name: medialibrary_file_access_GetFileInfoFromUri_0004
 * @tc.desc: Test function of GetFileInfoFromUri interface.
 * @tc.desc: convert the root directory uri to fileinfo failed because of GetProxyByUri failed.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000HRLBS
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_GetFileInfoFromUri_0004, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin medialibrary_file_access_GetFileInfoFromUri_0004";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            std::string str = info[i].uri;
            if (!ReplaceBundleNameFromPath(str, "ohos.com.NotExistBundleName")) {
                GTEST_LOG_(ERROR) << "replace BundleName failed.";
                EXPECT_TRUE(false);
            }
            Uri parentUri(str);
            FileInfo fileinfo;
            result = g_fah->GetFileInfoFromUri(parentUri, fileinfo);
            EXPECT_EQ(result, OHOS::FileAccessFwk::E_IPCS);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_GetFileInfoFromUri_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end medialibrary_file_access_GetFileInfoFromUri_0004";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_GetFileInfoFromUri_0005
 * @tc.name: medialibrary_file_access_GetFileInfoFromUri_0005
 * @tc.desc: Test function of GetFileInfoFromUri interface.
 * @tc.desc: convert the invalid uri to fileinfo failed.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000HRLBS
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_GetFileInfoFromUri_0005, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin medialibrary_file_access_GetFileInfoFromUri_0005";
    try {
        Uri uri("~!@#$%^&*()_");
        FileInfo fileInfo;
        int result = g_fah->GetFileInfoFromUri(uri, fileInfo);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);

        uri = Uri("/");
        result = g_fah->GetFileInfoFromUri(uri, fileInfo);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);

        uri = Uri("");
        result = g_fah->GetFileInfoFromUri(uri, fileInfo);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_GetFileInfoFromUri_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end medialibrary_file_access_GetFileInfoFromUri_0005";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_GetFileInfoFromRelativePath_0000
 * @tc.name: medialibrary_file_access_GetFileInfoFromRelativePath_0000
 * @tc.desc: Test function of GetFileInfoFromRelativePath interface.
 * @tc.desc: convert the general directory relativePath to fileinfo for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000HRLBS
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_GetFileInfoFromRelativePath_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin medialibrary_file_access_GetFileInfoFromRelativePath_0000";
    try {
        FileInfo fileInfo;
        string relativePath = "";
        int result = g_fah->GetFileInfoFromRelativePath(relativePath, fileInfo);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        relativePath = "Audios/";
        result = g_fah->GetFileInfoFromRelativePath(relativePath, fileInfo);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        relativePath = "Camera/";
        result = g_fah->GetFileInfoFromRelativePath(relativePath, fileInfo);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        relativePath = "Documents/";
        result = g_fah->GetFileInfoFromRelativePath(relativePath, fileInfo);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        relativePath = "Download";
        result = g_fah->GetFileInfoFromRelativePath(relativePath, fileInfo);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        relativePath = "Pictures";
        result = g_fah->GetFileInfoFromRelativePath(relativePath, fileInfo);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        relativePath = "Videos";
        result = g_fah->GetFileInfoFromRelativePath(relativePath, fileInfo);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_GetFileInfoFromRelativePath_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_GetFileInfoFromRelativePath_0000";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_GetFileInfoFromRelativePath_0001
 * @tc.name: medialibrary_file_access_GetFileInfoFromRelativePath_0001
 * @tc.desc: Test function of GetFileInfoFromRelativePath interface.
 * @tc.desc: convert the general directory relativePath to fileinfo for failed.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000HRLBS
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_GetFileInfoFromRelativePath_0001, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin medialibrary_file_access_GetFileInfoFromRelativePath_0001";
    try {
        FileInfo fileInfo;
        string relativePath = "test/";
        int result = g_fah->GetFileInfoFromRelativePath(relativePath, fileInfo);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);

        relativePath = "/";
        result = g_fah->GetFileInfoFromRelativePath(relativePath, fileInfo);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);

        relativePath = "~!@#$%^&*()_";
        result = g_fah->GetFileInfoFromRelativePath(relativePath, fileInfo);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);

        relativePath = "/d";
        result = g_fah->GetFileInfoFromRelativePath(relativePath, fileInfo);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_GetFileInfoFromRelativePath_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_GetFileInfoFromRelativePath_0001";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_GetFileInfoFromRelativePath_0002
 * @tc.name: medialibrary_file_access_GetFileInfoFromRelativePath_0002
 * @tc.desc: Test function of GetFileInfoFromRelativePath interface.
 * @tc.desc: convert the general directory relativePath to fileinfo and call listfile for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000HRLBS
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_GetFileInfoFromRelativePath_0002, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin medialibrary_file_access_GetFileInfoFromRelativePath_0002";
    try {
        FileInfo fileInfo;
        string relativePath = "Download/";
        int result = g_fah->GetFileInfoFromRelativePath(relativePath, fileInfo);
        ASSERT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri parentUri(fileInfo.uri);
        Uri newFile("");
        result = g_fah->CreateFile(parentUri, "GetFileInfoFromRelativePath_0002.jpg", newFile);
        ASSERT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        int64_t offset = 0;
        int64_t maxCount = 1000;
        FileFilter filter;
        std::vector<FileInfo> fileInfoVec;
        result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_GT(fileInfoVec.size(), OHOS::FileAccessFwk::ERR_OK);

        result = g_fah->Delete(newFile);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_GetFileInfoFromRelativePath_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_GetFileInfoFromRelativePath_0002";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_GetFileInfoFromRelativePath_0003
 * @tc.name: medialibrary_file_access_GetFileInfoFromRelativePath_0003
 * @tc.desc: Test function of GetFileInfoFromRelativePath interface.
 * @tc.desc: convert the relative file path to fileinfo and call listfile for failed.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000HRLBS
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_GetFileInfoFromRelativePath_0003, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin medialibrary_file_access_GetFileInfoFromRelativePath_0003";
    try {
        FileInfo fileInfo;
        string relativePath = "Download/";
        int result = g_fah->GetFileInfoFromRelativePath(relativePath, fileInfo);
        ASSERT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri parentUri(fileInfo.uri);
        Uri newFile("");
        result = g_fah->CreateFile(parentUri, "GetFileInfoFromRelativePath_0003.jpg", newFile);
        ASSERT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        relativePath = "Download/GetFileInfoFromRelativePath_0003.jpg";
        result = g_fah->GetFileInfoFromRelativePath(relativePath, fileInfo);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        int64_t offset = 0;
        int64_t maxCount = 1000;
        FileFilter filter;
        std::vector<FileInfo> fileInfoVec;
        result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);

        result = g_fah->Delete(newFile);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_GetFileInfoFromRelativePath_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_GetFileInfoFromRelativePath_0003";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_GetFileInfoFromRelativePath_0004
 * @tc.name: medialibrary_file_access_GetFileInfoFromRelativePath_0004
 * @tc.desc: Test function of GetFileInfoFromRelativePath interface.
 * @tc.desc: convert the relative directory path to fileinfo and call listfile for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000HRLBS
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_GetFileInfoFromRelativePath_0004, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin medialibrary_file_access_GetFileInfoFromRelativePath_0004";
    try {
        FileInfo fileInfo;
        string relativePath = "Download/";
        int result = g_fah->GetFileInfoFromRelativePath(relativePath, fileInfo);
        ASSERT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri parentUri(fileInfo.uri);
        Uri newDir("");
        result = g_fah->Mkdir(parentUri, "DirGetFileInfoFromRelativePath_0004", newDir);
        ASSERT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri fileUri("");
        result = g_fah->CreateFile(newDir, "file1.jpg", fileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = g_fah->CreateFile(newDir, "file2.jpg", fileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        relativePath = "Download/DirGetFileInfoFromRelativePath_0004";
        result = g_fah->GetFileInfoFromRelativePath(relativePath, fileInfo);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        int64_t offset = 0;
        int64_t maxCount = 1000;
        FileFilter filter;
        std::vector<FileInfo> fileInfoVec;
        result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_GT(fileInfoVec.size(), OHOS::FileAccessFwk::ERR_OK);

        result = g_fah->Delete(newDir);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_GetFileInfoFromRelativePath_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_GetFileInfoFromRelativePath_0004";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Query_0000
 * @tc.name: medialibrary_file_access_Query_0000
 * @tc.desc: Test function of Query directory for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6S4VV
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Query_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Query_0000";
    try {
        FileAccessFwk::FileInfo fileInfo;
        std::string relativePath = "Documents/";
        std::string displayName = "Documents";
        int targetSize = 46;
        int result = g_fah->GetFileInfoFromRelativePath(relativePath, fileInfo);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri dirUriTest(fileInfo.uri);

        json testJson = {
            {RELATIVE_PATH, " "},
            {DISPLAY_NAME, " "},
            {FILE_SIZE, " "},
            {DATE_MODIFIED, " "},
            {DATE_ADDED, " "}
        };
        auto testJsonString = testJson.dump();
        int ret = g_fah->Query(dirUriTest, testJsonString);
        EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
        auto jsonObject = json::parse(testJsonString);
        EXPECT_EQ(jsonObject.at(DISPLAY_NAME), displayName);
        EXPECT_EQ(jsonObject.at(FILE_SIZE), targetSize);
        ASSERT_TRUE(jsonObject.at(DATE_MODIFIED) > 0);
        ASSERT_TRUE(jsonObject.at(DATE_ADDED) > 0);
        GTEST_LOG_(INFO) << testJsonString;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Query_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Query_0000";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Query_0001
 * @tc.name: medialibrary_file_access_Query_0001
 * @tc.desc: Test function of Query file for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6S4VV
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Query_0001, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Query_0001";
    try {
        FileAccessFwk::FileInfo fileInfo;
        std::string relativePath = "Documents/Test/";
        std::string displayName = "CreateQueryTest_002.txt";
        int targetSize = 23;
        std::string filePath = "Documents/Test/CreateQueryTest_002.txt";
        int ret = g_fah->GetFileInfoFromRelativePath(filePath, fileInfo);
        EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri(fileInfo.uri);

        json testJson = {
            {RELATIVE_PATH, " "},
            {DISPLAY_NAME, " "},
            {FILE_SIZE, " "},
            {DATE_MODIFIED, " "},
            {DATE_ADDED, " "}
        };
        auto testJsonString = testJson.dump();
        ret = g_fah->Query(testUri, testJsonString);
        EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << testJsonString;
        auto jsonObject = json::parse(testJsonString);
        EXPECT_EQ(jsonObject.at(RELATIVE_PATH), relativePath);
        EXPECT_EQ(jsonObject.at(DISPLAY_NAME), displayName);
        EXPECT_EQ(jsonObject.at(FILE_SIZE), targetSize);
        ASSERT_TRUE(jsonObject.at(DATE_MODIFIED) > 0);
        ASSERT_TRUE(jsonObject.at(DATE_ADDED) > 0);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Query_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Query_0001";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Query_0002
 * @tc.name: medialibrary_file_access_Query_0002
 * @tc.desc: Test function of Query directory size for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Query_0002, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Query_0002";
    try {
        FileAccessFwk::FileInfo fileInfo;
        std::string relativePath = "Documents/";
        std::string displayName = "Documents";
        int targetSize = 46;
        int result = g_fah->GetFileInfoFromRelativePath(relativePath, fileInfo);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri dirUriTest(fileInfo.uri);

        json testJson = {
            {FILE_SIZE, " "}
        };
        auto testJsonString = testJson.dump();
        int ret = g_fah->Query(dirUriTest, testJsonString);
        EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
        auto jsonObject = json::parse(testJsonString);
        EXPECT_EQ(jsonObject.at(FILE_SIZE), targetSize);
        GTEST_LOG_(INFO) << testJsonString;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Query_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Query_0002";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Query_0003
 * @tc.name: medialibrary_file_access_Query_0003
 * @tc.desc: Test function of Query interface for ERROR which Uri is unreadable code.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6S4VV
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Query_0003, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Query_0003";
    try {
        Uri testUri("&*()*/?");
        json testJson = {
            {RELATIVE_PATH, " "},
            {DISPLAY_NAME, " "}
        };
        auto testJsonString = testJson.dump();
        int ret = g_fah->Query(testUri, testJsonString);
        EXPECT_NE(ret, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Query_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Query_0003";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Query_0004
 * @tc.name: medialibrary_file_access_Query_0004
 * @tc.desc: Test function of Query interface for which all column nonexistence.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6S4VV
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Query_0004, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Query_0004";
    try {
        Uri newDirUriTest("");
        std::string fileName = "test.txt";
        int ret = g_fah->Mkdir(g_newDirUri, "Query004", newDirUriTest);
        EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        ret = g_fah->CreateFile(newDirUriTest, fileName, testUri);
        EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
        json testJson = {
            {"001", " "},
            {"#", " "},
            {"test", " "},
            {"target", " "}
        };
        auto testJsonString = testJson.dump();
        ret = g_fah->Query(testUri, testJsonString);
        EXPECT_NE(ret, OHOS::FileAccessFwk::ERR_OK);
        ret = g_fah->Delete(newDirUriTest);
        EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Query_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Query_0004";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Query_0005
 * @tc.name: medialibrary_file_access_Query_0005
 * @tc.desc: Test function of Query interface for which part of column nonexistence.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6S4VV
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Query_0005, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Query_0005";
    try {
        Uri newDirUriTest("");
        std::string fileName = "test.txt";
        int ret = g_fah->Mkdir(g_newDirUri, "Query005", newDirUriTest);
        EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        ret = g_fah->CreateFile(newDirUriTest, fileName, testUri);
        EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
        json testJson = {
            {RELATIVE_PATH, " "},
            {DISPLAY_NAME, " "},
            {"test", " "}
        };
        auto testJsonString = testJson.dump();
        ret = g_fah->Query(testUri, testJsonString);
        EXPECT_NE(ret, OHOS::FileAccessFwk::ERR_OK);
        ret = g_fah->Delete(newDirUriTest);
        EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Query_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Query_0005";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Query_0006
 * @tc.name: medialibrary_file_access_Query_0006
 * @tc.desc: Test function of Query interface for which column is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6S4VV
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Query_0006, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Query_0006";
    try {
        Uri newDirUriTest("");
        std::string fileName = "test.txt";
        std::string relativePath = "test/test.txt";
        int ret = g_fah->Mkdir(g_newDirUri, "Query006", newDirUriTest);
        EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        ret = g_fah->CreateFile(newDirUriTest, fileName, testUri);
        EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
        json testJson;
        auto testJsonString = testJson.dump();
        ret = g_fah->Query(testUri, testJsonString);
        EXPECT_NE(ret, OHOS::FileAccessFwk::ERR_OK);
        ret = g_fah->Delete(newDirUriTest);
        EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Query_0006 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Query_0006";
}
} // namespace
