/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "external_file_access_test.h"
#include "accesstoken_kit.h"
#include "context_impl.h"
#include "file_access_framework_errno.h"
#include "file_info_shared_memory.h"
#include "iservice_registry.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

namespace OHOS::FileAccessFwk {
static int g_num = 0;

/**
 * @tc.number: user_file_service_external_file_access_Copy_0000
 * @tc.name: external_file_access_Copy_0000
 * @tc.desc: Test function of Copy interface, copy a file and argument of force is false
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7QXVD
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Copy_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Copy_0000";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri srcDir("");
            result = fileAccessHelper->Mkdir(parentUri, "Copy_0000_src", srcDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri srcFile("");
            result = fileAccessHelper->CreateFile(srcDir, "a.txt", srcFile);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            int fd = -1;
            result = fileAccessHelper->OpenFile(srcFile, WRITE_READ, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            char buff[] = "Copy test content for a.txt";
            ssize_t srcFileSize = write(fd, buff, sizeof(buff));
            EXPECT_EQ(srcFileSize, sizeof(buff));
            close(fd);

            Uri destDir("");
            result = fileAccessHelper->Mkdir(parentUri, "Copy_0000_dest", destDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            std::vector<Result> copyResult;
            result = fileAccessHelper->Copy(srcFile, destDir, copyResult, false);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(copyResult.size(), 0);

            Uri destFileUri(destDir.ToString() + "/" + "a.txt");
            result = fileAccessHelper->OpenFile(destFileUri, WRITE_READ, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            ssize_t destFileSize = read(fd, buff, sizeof(buff));
            EXPECT_EQ(srcFileSize, destFileSize);
            close(fd);

            result = fileAccessHelper->Delete(srcDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Delete(destDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Copy_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Copy_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_Copy_0001
 * @tc.name: external_file_access_Copy_0001
 * @tc.desc: Test function of Copy interface, copy a directory and argument of force is false
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7QXVD
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Copy_0001, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Copy_0001";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri srcDir("");
            Uri destDir("");
            result = fileAccessHelper->Mkdir(parentUri, "Copy_0001_src", srcDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri aFileUri("");
            result = fileAccessHelper->CreateFile(srcDir, "a.txt", aFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            int fd = -1;
            result = fileAccessHelper->OpenFile(aFileUri, WRITE_READ, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            char buff[] = "Copy test content for a.txt";
            ssize_t aFileSize = write(fd, buff, sizeof(buff));
            EXPECT_EQ(aFileSize, sizeof(buff));
            close(fd);
            Uri bFileUri("");
            result = fileAccessHelper->CreateFile(srcDir, "b.txt", bFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Mkdir(parentUri, "Copy_0001_dest", destDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            std::vector<Result> copyResult;
            result = fileAccessHelper->Copy(srcDir, destDir, copyResult, false);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(copyResult.size(), 0);

            result = fileAccessHelper->Delete(srcDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Delete(destDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Copy_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Copy_0001";
}

/**
 * @tc.number: user_file_service_external_file_access_Copy_0002
 * @tc.name: external_file_access_Copy_0002
 * @tc.desc: Test function of Copy interface, copy a empty directory
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7QXVD
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Copy_0002, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Copy_0002";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri srcDir("");
            result = fileAccessHelper->Mkdir(parentUri, "Copy_0002_src", srcDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri destDir("");
            result = fileAccessHelper->Mkdir(parentUri, "Copy_0002_dest", destDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            std::vector<Result> copyResult;
            result = fileAccessHelper->Copy(srcDir, destDir, copyResult, false);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(copyResult.size(), 0);

            result = fileAccessHelper->Delete(srcDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Delete(destDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Copy_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Copy_0002";
}

/**
 * @tc.number: user_file_service_external_file_access_Copy_0003
 * @tc.name: external_file_access_Copy_0003
 * @tc.desc: Test function of Copy interface, copy a file with the same name and argument of force is true
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7QXVD
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Copy_0003, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Copy_0003";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri srcDir("");
            result = fileAccessHelper->Mkdir(parentUri, "Copy_0003_src", srcDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri srcFile("");
            result = fileAccessHelper->CreateFile(srcDir, "b.txt", srcFile);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            int fd = -1;
            result = fileAccessHelper->OpenFile(srcFile, WRITE_READ, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            char buff[] = "Copy test content for b.txt";
            ssize_t srcFileSize = write(fd, buff, sizeof(buff));
            EXPECT_EQ(srcFileSize, sizeof(buff));
            close(fd);

            Uri destDir("");
            result = fileAccessHelper->Mkdir(parentUri, "Copy_0003_dest", destDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri existFile("");
            result = fileAccessHelper->CreateFile(destDir, "b.txt", existFile);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            std::vector<Result> copyResult;
            result = fileAccessHelper->Copy(srcFile, destDir, copyResult, true);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(copyResult.size(), 0);

            result = fileAccessHelper->OpenFile(existFile, WRITE_READ, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            ssize_t destFileSize = read(fd, buff, sizeof(buff));
            EXPECT_EQ(srcFileSize, destFileSize);
            close(fd);

            result = fileAccessHelper->Delete(srcDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Delete(destDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Copy_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Copy_0003";
}

/**
 * @tc.number: user_file_service_external_file_access_Copy_0004
 * @tc.name: external_file_access_Copy_0004
 * @tc.desc: Test function of Copy interface, copy a file with the same name and argument of force is false
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7QXVD
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Copy_0004, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Copy_0004";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri srcDir("");
            result = fileAccessHelper->Mkdir(parentUri, "Copy_0004_src", srcDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri srcFile("");
            result = fileAccessHelper->CreateFile(srcDir, "c.txt", srcFile);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            int fd = -1;
            result = fileAccessHelper->OpenFile(srcFile, WRITE_READ, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            char buff[] = "Copy test content for c.txt";
            ssize_t srcFileSize = write(fd, buff, sizeof(buff));
            EXPECT_EQ(srcFileSize, sizeof(buff));
            close(fd);

            Uri destDir("");
            result = fileAccessHelper->Mkdir(parentUri, "Copy_0004_dest", destDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri existFile("");
            result = fileAccessHelper->CreateFile(destDir, "c.txt", existFile);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            std::vector<Result> copyResult;
            result = fileAccessHelper->Copy(srcFile, destDir, copyResult, false);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);

            result = fileAccessHelper->OpenFile(existFile, WRITE_READ, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            ssize_t destFileSize = read(fd, buff, sizeof(buff));
            EXPECT_EQ(destFileSize, 0);
            close(fd);

            result = fileAccessHelper->Delete(srcDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Delete(destDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Copy_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Copy_0004";
}

/**
 * @tc.number: user_file_service_external_file_access_Copy_0005
 * @tc.name: external_file_access_Copy_0005
 * @tc.desc: Test function of Copy interface, copy a file with the same name and no force argument
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7QXVD
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Copy_0005, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Copy_0005";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri srcDir("");
            result = fileAccessHelper->Mkdir(parentUri, "Copy_0005_src", srcDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri srcFile("");
            result = fileAccessHelper->CreateFile(srcDir, "d.txt", srcFile);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            int fd = -1;
            result = fileAccessHelper->OpenFile(srcFile, WRITE_READ, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            char buff[] = "Copy test content for d.txt";
            ssize_t srcFileSize = write(fd, buff, sizeof(buff));
            EXPECT_EQ(srcFileSize, sizeof(buff));
            close(fd);

            Uri destDir("");
            result = fileAccessHelper->Mkdir(parentUri, "Copy_0005_dest", destDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri existFile("");
            result = fileAccessHelper->CreateFile(destDir, "d.txt", existFile);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            std::vector<Result> copyResult;
            result = fileAccessHelper->Copy(srcFile, destDir, copyResult);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);

            result = fileAccessHelper->OpenFile(existFile, WRITE_READ, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            ssize_t destFileSize = read(fd, buff, sizeof(buff));
            EXPECT_EQ(destFileSize, 0);
            close(fd);

            result = fileAccessHelper->Delete(srcDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Delete(destDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Copy_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Copy_0005";
}

static int ReadyToCopy06(Uri& parentUri, Uri& srcDir, const char* buff, int len,
    shared_ptr<FileAccessHelper> fileAccessHelper)
{
    int result = fileAccessHelper->Mkdir(parentUri, "Copy_0006_src", srcDir);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri aFileUri("");
    result = fileAccessHelper->CreateFile(srcDir, "a.txt", aFileUri);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri bFileUri("");
    result = fileAccessHelper->CreateFile(srcDir, "b.txt", bFileUri);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    int fd = -1;
    result = fileAccessHelper->OpenFile(bFileUri, WRITE_READ, fd);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    
    ssize_t srcFileSize = write(fd, buff, len);
    EXPECT_EQ(srcFileSize, len);
    close(fd);
    return srcFileSize;
}

/**
 * @tc.number: user_file_service_external_file_access_Copy_0006
 * @tc.name: external_file_access_Copy_0006
 * @tc.desc: Test function of Copy interface, copy a directory with the same name and argument of force is true
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7QXVD
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Copy_0006, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Copy_0006";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        char buff[] = "Copy test content for b.txt";
        for (size_t i = 0; i < info.size(); i++) {
            Uri srcDir("");
            Uri parentUri(info[i].uri);
            int srcFileSize = ReadyToCopy06(parentUri, srcDir, buff, sizeof(buff), fileAccessHelper);

            Uri bFileUri("");
            Uri destDir("");
            result = fileAccessHelper->Mkdir(parentUri, "Copy_0006_dest", destDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri destSrcDir("");
            result = fileAccessHelper->Mkdir(destDir, "Copy_0006_src", destSrcDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->CreateFile(destSrcDir, "b.txt", bFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            std::vector<Result> copyResult;
            result = fileAccessHelper->Copy(srcDir, destDir, copyResult, true);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(copyResult.size(), 0);

            int fd = -1;
            result = fileAccessHelper->OpenFile(bFileUri, WRITE_READ, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            ssize_t destFileSize = read(fd, buff, sizeof(buff));
            EXPECT_EQ(destFileSize, srcFileSize);
            close(fd);

            result = fileAccessHelper->Delete(srcDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Delete(destDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Copy_0006 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Copy_0006";
}

/**
 * @tc.number: user_file_service_external_file_access_Copy_0007
 * @tc.name: external_file_access_Copy_0007
 * @tc.desc: Test function of Copy interface, copy a directory with the same name and argument of force is false
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7QXVD
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Copy_0007, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Copy_0007";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri srcDir("");
            result = fileAccessHelper->Mkdir(parentUri, "Copy_0007_src", srcDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri bFileUri("");
            result = fileAccessHelper->CreateFile(srcDir, "a.txt", bFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->CreateFile(srcDir, "b.txt", bFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            int fd = -1;
            result = fileAccessHelper->OpenFile(bFileUri, WRITE_READ, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            char buff[] = "Copy test content for b.txt";
            ssize_t bFileSize = write(fd, buff, sizeof(buff));
            EXPECT_EQ(bFileSize, sizeof(buff));
            close(fd);

            Uri destDir("");
            result = fileAccessHelper->Mkdir(parentUri, "Copy_0007_dest", destDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri destSrcDir("");
            result = fileAccessHelper->Mkdir(destDir, "Copy_0007_src", destSrcDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri destFileUri("");
            result = fileAccessHelper->CreateFile(destSrcDir, "b.txt", destFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            std::vector<Result> copyResult;
            result = fileAccessHelper->Copy(srcDir, destDir, copyResult, false);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);

            result = fileAccessHelper->Delete(srcDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Delete(destDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Copy_0007 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Copy_0007";
}

/**
 * @tc.number: user_file_service_external_file_access_Copy_0008
 * @tc.name: external_file_access_Copy_0008
 * @tc.desc: Test function of Copy interface, copy a directory with the same name and no force argument
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7QXVD
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Copy_0008, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Copy_0008";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri srcDir("");
            result = fileAccessHelper->Mkdir(parentUri, "Copy_0008_src", srcDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri aFileUri("");
            result = fileAccessHelper->CreateFile(srcDir, "a.txt", aFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri bFileUri("");
            result = fileAccessHelper->CreateFile(srcDir, "b.txt", bFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri destDir("");
            result = fileAccessHelper->Mkdir(parentUri, "Copy_0008_dest", destDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri destSrcDir("");
            result = fileAccessHelper->Mkdir(destDir, "Copy_0008_src", destSrcDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->CreateFile(destSrcDir, "b.txt", bFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            std::vector<Result> copyResult;
            result = fileAccessHelper->Copy(srcDir, destDir, copyResult);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);

            result = fileAccessHelper->Delete(srcDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Delete(destDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Copy_0008 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Copy_0008";
}

static int ReadyToCopy09(Uri& parentUri, Uri& srcDir, const char* buff, int len,
    shared_ptr<FileAccessHelper> fileAccessHelper)
{
    int result = fileAccessHelper->Mkdir(parentUri, "Copy_0009_src", srcDir);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri aFileUri("");
    result = fileAccessHelper->CreateFile(srcDir, "c.txt", aFileUri);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri bFileUri("");
    result = fileAccessHelper->CreateFile(srcDir, "d.txt", bFileUri);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    int fd = -1;
    result = fileAccessHelper->OpenFile(bFileUri, WRITE_READ, fd);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    ssize_t srcFileSize = write(fd, buff, len);
    EXPECT_EQ(srcFileSize, len);
    close(fd);

    return srcFileSize;
}

/**
 * @tc.number: user_file_service_external_file_access_Copy_0009
 * @tc.name: external_file_access_Copy_0009
 * @tc.desc: Test function of Copy interface, copy directory and file between different disks
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7QXVD
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Copy_0009, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Copy_0009";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        char buff[] = "Copy test content for b.txt";
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri srcDir("");
            int srcFileSize = ReadyToCopy09(parentUri, srcDir, buff, sizeof(buff), fileAccessHelper);

            for (size_t j = i + 1; j < info.size(); j++) {
                Uri targetUri(info[j].uri);
                Uri destDir("");
                Uri bFileUri("");
                result = fileAccessHelper->Mkdir(targetUri, "Copy_0009_dest", destDir);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
                Uri destSrcDir("");
                result = fileAccessHelper->Mkdir(destDir, "Copy_0009_src", destSrcDir);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
                result = fileAccessHelper->CreateFile(destSrcDir, "d.txt", bFileUri);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

                std::vector<Result> copyResult;
                result = fileAccessHelper->Copy(srcDir, destDir, copyResult, true);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
                EXPECT_EQ(copyResult.size(), 0);

                int fd = -1;
                result = fileAccessHelper->OpenFile(bFileUri, WRITE_READ, fd);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
                ssize_t destFileSize = read(fd, buff, sizeof(buff));
                EXPECT_EQ(destFileSize, srcFileSize);
                close(fd);

                result = fileAccessHelper->Delete(destDir);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            }
            result = fileAccessHelper->Delete(srcDir);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Copy_0009 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Copy_0009";
}

/**
 * @tc.number: user_file_service_external_file_access_Rename_0000
 * @tc.name: external_file_access_Rename_0000
 * @tc.desc: Test function of Rename interface for SUCCESS which rename file.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Rename_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Rename_0000";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "test7", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri renameUri("");
            result = fileAccessHelper->Rename(testUri, "test2.txt", renameUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Rename_0000 result:" << result;
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Rename_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Rename_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_Rename_0001
 * @tc.name: external_file_access_Rename_0001
 * @tc.desc: Test function of Rename interface for SUCCESS which rename folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Rename_0001, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Rename_0001";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "test8", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri renameUri("");
            result = fileAccessHelper->Rename(newDirUriTest, "testRename", renameUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Rename_0001 result:" << result;
            result = fileAccessHelper->Delete(renameUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Rename_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Rename_0001";
}

/**
 * @tc.number: user_file_service_external_file_access_Rename_0002
 * @tc.name: external_file_access_Rename_0002
 * @tc.desc: Test function of Rename interface for ERROR which sourceFileUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Rename_0002, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Rename_0002";
    try {
        Uri renameUri("");
        Uri sourceFileUri("");
        int result = fileAccessHelper->Rename(sourceFileUri, "testRename.txt", renameUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Rename_0002 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Rename_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Rename_0002";
}

/**
 * @tc.number: user_file_service_external_file_access_Rename_0003
 * @tc.name: external_file_access_Rename_0003
 * @tc.desc: Test function of Rename interface for ERROR which sourceFileUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Rename_0003, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Rename_0003";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "test9", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri renameUri("");
            Uri sourceFileUri("storage/media/100/local/files/Download/test9/test.txt");
            result = fileAccessHelper->Rename(sourceFileUri, "testRename.txt", renameUri);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Rename_0003 result:" << result;
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Rename_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Rename_0003";
}

/**
 * @tc.number: user_file_service_external_file_access_Rename_0004
 * @tc.name: external_file_access_Rename_0004
 * @tc.desc: Test function of Rename interface for ERROR which sourceFileUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Rename_0004, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Rename_0004";
    try {
        Uri renameUri("");
        Uri sourceFileUri("~!@#$%^&*()_");
        int result = fileAccessHelper->Rename(sourceFileUri, "testRename.txt", renameUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Rename_0004 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Rename_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Rename_0004";
}

/**
 * @tc.number: user_file_service_external_file_access_Rename_0005
 * @tc.name: external_file_access_Rename_0005
 * @tc.desc: Test function of Rename interface for ERROR which displayName is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Rename_0005, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Rename_0005";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "test10", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri renameUri("");
            result = fileAccessHelper->Rename(testUri, "", renameUri);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Rename_0005 result:" << result;
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Rename_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Rename_0005";
}

static void RenameTdd(shared_ptr<FileAccessHelper> fahs, Uri sourceFile, std::string displayName, Uri newFile)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_RenameTdd";
    int ret = fahs->Rename(sourceFile, displayName, newFile);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(ERROR) << "RenameTdd get result error, code:" << ret;
        return;
    }
    EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
    g_num++;
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_RenameTdd";
}

/**
 * @tc.number: user_file_service_external_file_access_Rename_0006
 * @tc.name: external_file_access_Rename_0006
 * @tc.desc: Test function of Rename interface for SUCCESS which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Rename_0006, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Rename_0006";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "test11", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            std::string displayName1 = "test1.txt";
            std::string displayName2 = "test2.txt";
            Uri renameUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest, displayName1, testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            g_num = 0;
            for (int j = 0; j < INIT_THREADS_NUMBER; j++) {
                std::thread execthread(RenameTdd, fileAccessHelper, testUri, displayName2, renameUri);
                execthread.join();
            }
            EXPECT_EQ(g_num, ACTUAL_SUCCESS_THREADS_NUMBER);
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Rename_0006 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Rename_0006";
}

/**
 * @tc.number: user_file_service_external_file_access_Rename_0007
 * @tc.name: external_file_access_Rename_0007
 * @tc.desc: Test function of Rename interface for ERROR because of GetProxyByUri failed.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Rename_0007, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Rename_0007";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "test12", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            std::string str = newDirUriTest.ToString();
            if (ReplaceBundleName(str, "ohos.com.NotExistBundleName")) {
                Uri testUri(str);
                Uri renameUri("");
                result = fileAccessHelper->Rename(testUri, "test.txt", renameUri);
                EXPECT_EQ(result, OHOS::FileAccessFwk::E_IPCS);
                GTEST_LOG_(INFO) << "Rename_0007 result:" << result;
                result = fileAccessHelper->Delete(newDirUriTest);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            } else {
                EXPECT_TRUE(false);
            }
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Rename_0007 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Rename_0007";
}

/**
 * @tc.number: user_file_service_external_file_access_Rename_0008
 * @tc.name: external_file_access_Rename_0008
 * @tc.desc: Test function of Rename interface for SUCCESS which rename file, the display name is chinese.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I70SX9
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Rename_0008, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Rename_0008";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "测试目录2", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newDirUriTest.ToString().find("测试目录2"), std::string::npos);
            Uri testUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri renameUri("");
            result = fileAccessHelper->Rename(testUri, "测试文件.txt", renameUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(renameUri.ToString().find("测试文件.txt"), std::string::npos);
            GTEST_LOG_(INFO) << "Rename_0008 result:" << result;
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Rename_0008 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Rename_0008";
}

/**
 * @tc.number: user_file_service_external_file_access_Rename_0009
 * @tc.name: external_file_access_Rename_0009
 * @tc.desc: Test function of Rename interface for SUCCESS which rename folder, the display name is chinese.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I70SX9
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Rename_0009, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Rename_0009";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "test13", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri renameUri("");
            result = fileAccessHelper->Rename(newDirUriTest, "重命名目录", renameUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(renameUri.ToString().find("重命名目录"), std::string::npos);
            GTEST_LOG_(INFO) << "Rename_0009 result:" << result;
            result = fileAccessHelper->Delete(renameUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Rename_0009 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Rename_0009";
}

/**
 * @tc.number: user_file_service_external_file_access_ListFile_0000
 * @tc.name: external_file_access_ListFile_0000
 * @tc.desc: Test function of ListFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_ListFile_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0000";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "test14", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest, "external_file_access_ListFile_0000.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            FileInfo fileInfo;
            fileInfo.uri = newDirUriTest.ToString();
            int64_t offset = 0;
            FileFilter filter;
            SharedMemoryInfo memInfo;
            result = FileAccessFwk::SharedMemoryOperation::CreateSharedMemory("FileInfo List", DEFAULT_CAPACITY_200KB,
                memInfo);
            result = fileAccessHelper->ListFile(fileInfo, offset, filter, memInfo);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_GT(memInfo.Size(), OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            FileAccessFwk::SharedMemoryOperation::DestroySharedMemory(memInfo);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ListFile_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ListFile_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_ListFile_0001
 * @tc.name: external_file_access_ListFile_0001
 * @tc.desc: Test function of ListFile interface for ERROR which Uri is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_ListFile_0001, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0001";
    try {
        Uri sourceFileUri("");
        FileInfo fileInfo;
        fileInfo.uri = sourceFileUri.ToString();
        int64_t offset = 0;
        FileFilter filter;
        SharedMemoryInfo memInfo;
        int result = FileAccessFwk::SharedMemoryOperation::CreateSharedMemory("FileInfo List", DEFAULT_CAPACITY_200KB,
                memInfo);
        result = fileAccessHelper->ListFile(fileInfo, offset, filter, memInfo);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(memInfo.Size(), OHOS::FileAccessFwk::ERR_OK);
        FileAccessFwk::SharedMemoryOperation::DestroySharedMemory(memInfo);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ListFile_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ListFile_0001";
}

/**
 * @tc.number: user_file_service_external_file_access_ListFile_0002
 * @tc.name: external_file_access_ListFile_0002
 * @tc.desc: Test function of ListFile interface for ERROR which sourceFileUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_ListFile_0002, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0002";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "test15", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri sourceFileUri("storage/media/100/local/files/Download/test15/test.txt");
            FileInfo fileInfo;
            fileInfo.uri = sourceFileUri.ToString();
            Uri sourceFile(fileInfo.uri);
            int64_t offset = 0;
            FileFilter filter;
            SharedMemoryInfo memInfo;
            int result = FileAccessFwk::SharedMemoryOperation::CreateSharedMemory("FileInfo List",
                DEFAULT_CAPACITY_200KB, memInfo);
            result = fileAccessHelper->ListFile(fileInfo, offset, filter, memInfo);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(memInfo.Size(), OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            FileAccessFwk::SharedMemoryOperation::DestroySharedMemory(memInfo);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ListFile_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ListFile_0002";
}

/**
 * @tc.number: user_file_service_external_file_access_ListFile_0003
 * @tc.name: external_file_access_ListFile_0003
 * @tc.desc: Test function of ListFile interface for ERROR which sourceFileUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_ListFile_0003, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0003";
    try {
        Uri sourceFileUri("~!@#$%^&*()_");
        FileInfo fileInfo;
        fileInfo.uri = sourceFileUri.ToString();
        Uri sourceFile(fileInfo.uri);
        int64_t offset = 0;
        FileFilter filter;
        SharedMemoryInfo memInfo;
        int result = FileAccessFwk::SharedMemoryOperation::CreateSharedMemory("FileInfo List", DEFAULT_CAPACITY_200KB,
            memInfo);
        result = fileAccessHelper->ListFile(fileInfo, offset, filter, memInfo);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(memInfo.Size(), OHOS::FileAccessFwk::ERR_OK);
        FileAccessFwk::SharedMemoryOperation::DestroySharedMemory(memInfo);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ListFile_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ListFile_0003";
}

static void ListFileTdd(FileInfo fileInfo, int offset, FileFilter filter,
    SharedMemoryInfo &memInfo)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFileTdd";
    int ret = fileAccessHelper->ListFile(fileInfo, offset, filter, memInfo);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(ERROR) << "ListFile get result error, code:" << ret;
        return;
    }
    EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
    g_num++;
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ListFileTdd";
}

/**
 * @tc.number: user_file_service_external_file_access_ListFile_0004
 * @tc.name: external_file_access_ListFile_0004
 * @tc.desc: Test function of ListFile interface for SUCCESS which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_ListFile_0004, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0004";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "test16", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest, "external_file_access_ListFile_0004.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            FileInfo fileInfo;
            fileInfo.uri = newDirUriTest.ToString();
            int offset = 0;
            FileFilter filter;
            g_num = 0;
            SharedMemoryInfo memInfo;
            result = FileAccessFwk::SharedMemoryOperation::CreateSharedMemory("FileInfo List", DEFAULT_CAPACITY_200KB,
                memInfo);
            for (int j = 0; j < INIT_THREADS_NUMBER; j++) {
                std::thread execthread(ListFileTdd, fileInfo, offset, filter, std::ref(memInfo));
                execthread.join();
            }
            EXPECT_EQ(g_num, INIT_THREADS_NUMBER);
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            FileAccessFwk::SharedMemoryOperation::DestroySharedMemory(memInfo);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ListFile_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ListFile_0004";
}

/**
 * @tc.number: user_file_service_external_file_access_ListFile_0005
 * @tc.name: external_file_access_ListFile_0005
 * @tc.desc: Test function of ListFile interface for ERROR because of GetProxyByUri failed.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_ListFile_0005, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0005";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "test17", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            std::string str = testUri.ToString();
            if (ReplaceBundleName(str, "ohos.com.NotExistBundleName")) {
                FileInfo fileInfo;
                fileInfo.uri = str;
                int64_t offset = 0;
                FileFilter filter;
                SharedMemoryInfo memInfo;
                int result = FileAccessFwk::SharedMemoryOperation::CreateSharedMemory("FileInfo List",
                    DEFAULT_CAPACITY_200KB, memInfo);
                result = fileAccessHelper->ListFile(fileInfo, offset, filter, memInfo);
                EXPECT_EQ(result, OHOS::FileAccessFwk::E_IPCS);
                result = fileAccessHelper->Delete(newDirUriTest);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
                FileAccessFwk::SharedMemoryOperation::DestroySharedMemory(memInfo);
            } else {
                EXPECT_TRUE(false);
            }
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ListFile_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ListFile_0005";
}

/**
 * @tc.number: user_file_service_external_file_access_ListFile_0006
 * @tc.name: external_file_access_ListFile_0006
 * @tc.desc: Test function of ListFile interface for SUCCESS, the folder and file name is chinese.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I70SX9
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_ListFile_0006, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0006";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "测试目录0006", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newDirUriTest.ToString().find("测试目录0006"), std::string::npos);
            Uri testUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest, "测试文件.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(testUri.ToString().find("测试文件.txt"), std::string::npos);
            FileInfo fileInfo;
            fileInfo.uri = newDirUriTest.ToString();
            int64_t offset = 0;
            FileFilter filter;
            SharedMemoryInfo memInfo;
            result = FileAccessFwk::SharedMemoryOperation::CreateSharedMemory("FileInfo List",
                DEFAULT_CAPACITY_200KB, memInfo);
            result = fileAccessHelper->ListFile(fileInfo, offset, filter, memInfo);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_GT(memInfo.Size(), OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            FileAccessFwk::SharedMemoryOperation::DestroySharedMemory(memInfo);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ListFile_0006 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ListFile_0006";
}

static void WriteData(Uri &uri, shared_ptr<FileAccessHelper> fileAccessHelper)
{
    int fd = -1;
    std::string buff = "extenal_file_access_test";
    int result = fileAccessHelper->OpenFile(uri, WRITE_READ, fd);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    ssize_t fileSize = write(fd, buff.c_str(), buff.size());
    close(fd);
    EXPECT_EQ(fileSize, buff.size());
}

static double GetTime()
{
    struct timespec t{};
    t.tv_sec = 0;
    t.tv_nsec = 0;
    clock_gettime(CLOCK_REALTIME, &t);
    return static_cast<double>(t.tv_sec);
}

static double InitListFile(Uri newDirUriTest, const std::string &caseNumber,
    shared_ptr<FileAccessHelper> fileAccessHelper, const bool &needSleep = false)
{
    Uri testUri1("");
    int result = fileAccessHelper->CreateFile(newDirUriTest,
        "external_file_access_ListFile_" + caseNumber + ".txt", testUri1);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri testUri2("");
    result = fileAccessHelper->CreateFile(newDirUriTest,
        "external_file_access_ListFile_" + caseNumber + ".docx", testUri2);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri testUri3("");
    double time = GetTime();
    if (needSleep) {
        sleep(SLEEP_TIME);
    }
    result = fileAccessHelper->CreateFile(newDirUriTest,
        "external_file_access_ListFile_01_" + caseNumber + ".txt", testUri3);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri testUri4("");
    result = fileAccessHelper->CreateFile(newDirUriTest,
        "external_file_access_ListFile_01_" + caseNumber +  ".docx", testUri4);
    WriteData(testUri4, fileAccessHelper);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri testUri5("");
    result = fileAccessHelper->CreateFile(newDirUriTest,
        "external_file_access_ListFile_01_" + caseNumber + "_01.docx", testUri5);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    return time;
}

static void ListFileFilter7(Uri newDirUriTest, shared_ptr<FileAccessHelper> fileAccessHelper)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 4;
    SharedMemoryInfo memInfo;
    FileFilter filter({".txt", ".docx"}, {}, {}, -1, -1, false, true);
    int result = FileAccessFwk::SharedMemoryOperation::CreateSharedMemory("FileInfo List", DEFAULT_CAPACITY_200KB,
        memInfo);
    result = fileAccessHelper->ListFile(fileInfo, offset, filter, memInfo);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(memInfo.Size(), FILE_COUNT_1);
    FileAccessFwk::SharedMemoryOperation::DestroySharedMemory(memInfo);
}

/**
 * @tc.number: user_file_service_external_file_access_ListFile_0007
 * @tc.name: external_file_access_ListFile_0007
 * @tc.desc: Test function of ListFile for Success, filter is File Extensions.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I79CSX
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_ListFile_0007, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0007";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "listfile007", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            InitListFile(newDirUriTest, "0007", fileAccessHelper);
            ListFileFilter7(newDirUriTest, fileAccessHelper);
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ListFile_0007 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ListFile_0007";
}

static double InitListFileFolder(Uri newDirUriTest, const std::string &caseNumber,
    shared_ptr<FileAccessHelper> fileAccessHelper, const bool &needSleep = false)
{
    double time = InitListFile(newDirUriTest, caseNumber, fileAccessHelper, needSleep);
    Uri folderUri("");
    int result = fileAccessHelper->Mkdir(newDirUriTest, "test" + caseNumber, folderUri);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri testUri6("");
    result = fileAccessHelper->CreateFile(folderUri,
        "external_file_access_ListFile_01_" + caseNumber + "_02.docx", testUri6);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    return time;
}

static void ShowInfo(SharedMemoryInfo &memInfo, const std::string &caseNumber)
{
    FileAccessFwk::FileInfo fileInfo;
    for (size_t i = 0; i < memInfo.Size(); i++) {
        FileAccessFwk::SharedMemoryOperation::ReadFileInfo(fileInfo, memInfo);
        GTEST_LOG_(INFO) << caseNumber << ", uri:" << fileInfo.uri << endl;
    }
}

static void ListFileFilter8(Uri newDirUriTest, shared_ptr<FileAccessHelper> fileAccessHelper)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 0;
    SharedMemoryInfo memInfo;
    FileFilter filter({}, {}, {}, -1, 0, false, true);
    int result = FileAccessFwk::SharedMemoryOperation::CreateSharedMemory("FileInfo List", DEFAULT_CAPACITY_200KB,
        memInfo);
    result = fileAccessHelper->ListFile(fileInfo, offset, filter, memInfo);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(memInfo.Size(), FILE_COUNT_6);
    ShowInfo(memInfo, "external_file_access_ListFile_0008");
    FileAccessFwk::SharedMemoryOperation::DestroySharedMemory(memInfo);
}

/**
 * @tc.number: user_file_service_external_file_access_ListFile_0008
 * @tc.name: external_file_access_ListFile_0008
 * @tc.desc: Test function of ListFile for Success, filter is filesize >= 0
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I79CSX
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_ListFile_0008, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0008";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "listfile008", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            InitListFileFolder(newDirUriTest, "0008", fileAccessHelper);
            ListFileFilter8(newDirUriTest, fileAccessHelper);
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ListFile_0008 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ListFile_0008";
}

static void ListFileFilter9(Uri newDirUriTest, shared_ptr<FileAccessHelper> fileAccessHelper)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 0;
    SharedMemoryInfo memInfo;
    FileFilter filter;
    int result = FileAccessFwk::SharedMemoryOperation::CreateSharedMemory("FileInfo List", DEFAULT_CAPACITY_200KB,
        memInfo);
    result = fileAccessHelper->ListFile(fileInfo, offset, filter, memInfo);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(memInfo.Size(), FILE_COUNT_6);
    FileAccessFwk::SharedMemoryOperation::DestroySharedMemory(memInfo);
}

/**
 * @tc.number: user_file_service_external_file_access_ListFile_0009
 * @tc.name: external_file_access_ListFile_0009
 * @tc.desc: Test function of ListFile for Success, filter is offset from 0 and maxCount is 1000
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I79CSX
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_ListFile_0009, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0008";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "listfile009", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            InitListFileFolder(newDirUriTest, "0009", fileAccessHelper);
            ListFileFilter9(newDirUriTest, fileAccessHelper);
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ListFile_0009 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ListFile_0009";
}

static void ListFileFilter10(Uri newDirUriTest, const double &time, shared_ptr<FileAccessHelper> fileAccessHelper)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 0;
    SharedMemoryInfo memInfo;
    FileFilter filter({".txt", ".docx"}, {}, {}, -1, -1, false, true);
    int result = FileAccessFwk::SharedMemoryOperation::CreateSharedMemory("FileInfo List", DEFAULT_CAPACITY_200KB,
        memInfo);
    result = fileAccessHelper->ListFile(fileInfo, offset, filter, memInfo);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(memInfo.Size(), FILE_COUNT_5);

    FileFilter filter1({".txt", ".docx"}, {"0010.txt", "0010.docx"}, {}, -1, -1, false, true);
    result = fileAccessHelper->ListFile(fileInfo, offset, filter1, memInfo);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(memInfo.Size(), FILE_COUNT_4);

    FileFilter filter2({".txt", ".docx"}, {"0010.txt", "0010.docx"}, {}, 0, 0, false, true);
    result = fileAccessHelper->ListFile(fileInfo, offset, filter2, memInfo);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(memInfo.Size(), FILE_COUNT_1);

    FileFilter filter3({".txt", ".docx"}, {"0010.txt", "0010.docx"}, {}, -1, time, false, true);
    result = fileAccessHelper->ListFile(fileInfo, offset, filter3, memInfo);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(memInfo.Size(), FILE_COUNT_2);

    double nowTime = GetTime();
    FileFilter filter4({".txt", ".docx"}, {"0010.txt", "0010.docx"}, {}, -1, nowTime, false, true);
    result = fileAccessHelper->ListFile(fileInfo, offset, filter4, memInfo);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(memInfo.Size(), 0);
    FileAccessFwk::SharedMemoryOperation::DestroySharedMemory(memInfo);
}

/**
 * @tc.number: user_file_service_external_file_access_ListFile_0010
 * @tc.name: external_file_access_ListFile_0010
 * @tc.desc: Test function of ListFile interface for SUCCESS, filter is filename, extension, file size, modify time
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I79CSX
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_ListFile_0010, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0010";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "listfile0010", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            double time = InitListFile(newDirUriTest, "0010", fileAccessHelper, true);
            ListFileFilter10(newDirUriTest, time, fileAccessHelper);
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ListFile_0010 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ListFile_0010";
}

static void ListFileFilter11(Uri newDirUriTest, const double &time, shared_ptr<FileAccessHelper> fileAccessHelper)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 0;
    SharedMemoryInfo memInfo;
    int result = FileAccessFwk::SharedMemoryOperation::CreateSharedMemory("FileInfo List", DEFAULT_CAPACITY_200KB,
        memInfo);
    FileFilter filter({".txt", ".docx"}, {}, {}, -1, -1, false, true);
    result = fileAccessHelper->ListFile(fileInfo, offset, filter, memInfo);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(memInfo.Size(), FILE_COUNT_6);

    FileFilter filter1({".txt", ".docx"}, {"测试.txt", "测试.docx"}, {}, -1, -1, false, true);
    result = fileAccessHelper->ListFile(fileInfo, offset, filter1, memInfo);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(memInfo.Size(), FILE_COUNT_4);

    FileFilter filter2({".txt", ".docx"}, {"测试.txt", "测试.docx"}, {}, 0, 0, false, true);
    result = fileAccessHelper->ListFile(fileInfo, offset, filter2, memInfo);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(memInfo.Size(), FILE_COUNT_1);

    FileFilter filter3({".txt", ".docx"}, {"测试.txt", "测试.docx"}, {}, -1, time, false, true);
    result = fileAccessHelper->ListFile(fileInfo, offset, filter3, memInfo);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(memInfo.Size(), FILE_COUNT_2);

    double nowTime = GetTime();
    FileFilter filter4({".txt", ".docx"}, {"测试.txt", "测试.docx"}, {}, -1, nowTime, false, true);
    result = fileAccessHelper->ListFile(fileInfo, offset, filter4, memInfo);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(memInfo.Size(), 0);
    FileAccessFwk::SharedMemoryOperation::DestroySharedMemory(memInfo);
}

/**
 * @tc.number: user_file_service_external_file_access_ListFile_0011
 * @tc.name: external_file_access_ListFile_0011
 * @tc.desc: Test function of ListFile interface for SUCCESS, for filename is Chinese
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I79CSX
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_ListFile_0011, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0011";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "listfile测试", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            double time = InitListFileFolder(newDirUriTest, "测试", fileAccessHelper, true);
            ListFileFilter11(newDirUriTest, time, fileAccessHelper);
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ListFile_0011 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ListFile_0011";
}

static double InitScanFile(Uri newDirUriTest, const std::string &caseNumber,
    shared_ptr<FileAccessHelper> fileAccessHelper, const bool &needSleep = false)
{
    Uri forlderUriTest("");
    int result = fileAccessHelper->Mkdir(newDirUriTest, "test" + caseNumber, forlderUriTest);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

    Uri testUri1("");
    result = fileAccessHelper->CreateFile(newDirUriTest,
        "external_file_access_ScanFile_" + caseNumber + ".txt", testUri1);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri testUri2("");
    result = fileAccessHelper->CreateFile(newDirUriTest,
        "external_file_access_ScanFile_" + caseNumber + ".docx", testUri2);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    double time = GetTime();
    if (needSleep) {
        sleep(SLEEP_TIME);
    }
    Uri testUri3("");
    result = fileAccessHelper->CreateFile(forlderUriTest,
        "external_file_access_ScanFile_01_" + caseNumber + ".txt", testUri3);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri testUri4("");
    result = fileAccessHelper->CreateFile(forlderUriTest,
        "external_file_access_ScanFile_01_" + caseNumber + ".docx", testUri4);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    WriteData(testUri4, fileAccessHelper);
    Uri testUri5("");
    result = fileAccessHelper->CreateFile(forlderUriTest,
        "external_file_access_ScanFile_01_" + caseNumber + "_01.docx", testUri5);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    return time;
}

static void ScanFileFilter0(Uri newDirUriTest, const double &time, shared_ptr<FileAccessHelper> fileAccessHelper)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 0;
    int64_t maxCount = 1000;
    std::vector<FileInfo> fileInfoVec;
    FileFilter filter({".txt", ".docx"}, {}, {}, -1, -1, false, true);
    int result = fileAccessHelper->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_5);

    FileFilter filter1({".txt", ".docx"}, {"0000.txt", "0000.docx"}, {}, -1, -1, false, true);
    result = fileAccessHelper->ScanFile(fileInfo, offset, maxCount, filter1, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_4);

    FileFilter filter2({".txt", ".docx"}, {"0000.txt", "0000.docx"}, {}, 0, 0, false, true);
    result = fileAccessHelper->ScanFile(fileInfo, offset, maxCount, filter2, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_1);

    FileFilter filter3({".txt", ".docx"}, {"0000.txt", "0000.docx"}, {}, -1, time, false, true);
    result = fileAccessHelper->ScanFile(fileInfo, offset, maxCount, filter3, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_2);

    double nowTime = GetTime();
    FileFilter filter4({".txt", ".docx"}, {"0000.txt", "0000.docx"}, {}, -1, nowTime, false, true);
    result = fileAccessHelper->ScanFile(fileInfo, offset, maxCount, filter4, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), 0);
}

/**
 * @tc.number: user_file_service_external_file_access_ScanFile_0000
 * @tc.name: external_file_access_ScanFile_0000
 * @tc.desc: Test function of ScanFile interface for SUCCESS, filter is filename, extension, file size, modify time
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I79CSX
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_ScanFile_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ScanFile_0000";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "scanfile0000", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            double time = InitScanFile(newDirUriTest, "0000", fileAccessHelper, true);
            ScanFileFilter0(newDirUriTest, time, fileAccessHelper);
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ScanFile_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ScanFile_0000";
}

static void ScanFileFilter1(Uri newDirUriTest, shared_ptr<FileAccessHelper> fileAccessHelper)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 0;
    int64_t maxCount = 1000;
    std::vector<FileInfo> fileInfoVec;
    FileFilter filter;
    int result = fileAccessHelper->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_5);
}

/**
 * @tc.number: user_file_service_external_file_access_ScanFile_0001
 * @tc.name: external_file_access_ScanFile_0001
 * @tc.desc: Test function of ScanFile interface for SUCCESS, the filter is offset from 0 to maxcount
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I79CSX
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_ScanFile_0001, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ScanFile_0001";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "scanfile0001", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            InitScanFile(newDirUriTest, "0001", fileAccessHelper);
            ScanFileFilter1(newDirUriTest, fileAccessHelper);
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ScanFile_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ScanFile_0001";
}

static void ScanFileFilter2(Uri newDirUriTest, shared_ptr<FileAccessHelper> fileAccessHelper)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 3;
    int64_t maxCount = 3;
    std::vector<FileInfo> fileInfoVec;
    FileFilter filter({".txt", ".docx"}, {}, {}, -1, -1, false, true);
    int result = fileAccessHelper->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_2);
}

/**
 * @tc.number: user_file_service_external_file_access_ScanFile_0002
 * @tc.name: external_file_access_ScanFile_0002
 * @tc.desc: Test function of ScanFile interface for SUCCESS, the filter is extenstion offset maxCount
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I79CSX
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_ScanFile_0002, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ScanFile_0002";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "scanfile0002", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            InitScanFile(newDirUriTest, "0002", fileAccessHelper);
            ScanFileFilter2(newDirUriTest, fileAccessHelper);
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ScanFile_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ScanFile_0002";
}

static void ScanFileFilter3(Uri newDirUriTest, const double &time, shared_ptr<FileAccessHelper> fileAccessHelper)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 0;
    int64_t maxCount = 1000;
    std::vector<FileInfo> fileInfoVec;
    FileFilter filter({}, {}, {}, -1, time, false, true);
    int result = fileAccessHelper->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_3);
}

/**
 * @tc.number: user_file_service_external_file_access_ScanFile_0003
 * @tc.name: external_file_access_ScanFile_0003
 * @tc.desc: Test function of ScanFile interface for SUCCESS, the filter is modify time
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I79CSX
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_ScanFile_0003, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ScanFile_0003";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "scanfile0003", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            double time = InitScanFile(newDirUriTest, "0003", fileAccessHelper, true);
            ScanFileFilter3(newDirUriTest, time, fileAccessHelper);
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ScanFile_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ScanFile_0003";
}

static void ScanFileFilter4(Uri newDirUriTest, shared_ptr<FileAccessHelper> fileAccessHelper)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 0;
    int64_t maxCount = 1000;
    std::vector<FileInfo> fileInfoVec;
    FileFilter filter;
    int result = fileAccessHelper->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_5);
}

/**
 * @tc.number: user_file_service_external_file_access_ScanFile_0004
 * @tc.name: external_file_access_ScanFile_0004
 * @tc.desc: Test function of ScanFile interface for SUCCESS, the filter is offset from 0 to maxCount
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I79CSX
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_ScanFile_0004, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin user_file_service_external_file_access_ScanFile_0004";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "scanfile0004", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            InitScanFile(newDirUriTest, "0004", fileAccessHelper);
            ScanFileFilter4(newDirUriTest, fileAccessHelper);
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "user_file_service_external_file_access_ScanFile_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end user_file_service_external_file_access_ScanFile_0004";
}

static void ScanFileFilter5(Uri newDirUriTest, shared_ptr<FileAccessHelper> fileAccessHelper)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 0;
    int64_t maxCount = 1000;
    std::vector<FileInfo> fileInfoVec;
    FileFilter filter({".txt", ".docx"}, {"测试.txt", "测试.docx"}, {}, -1, -1, false, true);
    int result = fileAccessHelper->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_4);
}

/**
 * @tc.number: user_file_service_external_file_access_ScanFile_0005
 * @tc.name: external_file_access_ScanFile_0005
 * @tc.desc: Test function of ScanFile interface for SUCCESS, the filter is Chinese filename
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I79CSX
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_ScanFile_0005, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    EXPECT_NE(fileAccessHelper, nullptr);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin user_file_service_external_file_access_ScanFile_0005";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "scanfile测试", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            InitScanFile(newDirUriTest, "测试", fileAccessHelper);
            ScanFileFilter5(newDirUriTest, fileAccessHelper);
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "user_file_service_external_file_access_ScanFile_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end user_file_service_external_file_access_ScanFile_0005";
}
} // namespace
