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
 * @tc.number: user_file_service_external_file_access_OpenFile_0000
 * @tc.name: external_file_access_OpenFile_0000
 * @tc.desc: Test function of OpenFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_OpenFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_OpenFile_0000";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        vector<RootInfo> info;
        EXPECT_NE(fileAccessHelper, nullptr);
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            GTEST_LOG_(INFO) << parentUri.ToString();
            Uri newDirUriTest1("");
            result = fileAccessHelper->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri newFileUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest1, "external_file_access_OpenFile_0000.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            int fd;
            result = fileAccessHelper->OpenFile(newFileUri, WRITE_READ, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "OpenFile_0000 result:" << result;
            close(fd);
            result = fileAccessHelper->Delete(newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_OpenFile_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_OpenFile_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_OpenFile_0001
 * @tc.name: external_file_access_OpenFile_0001
 * @tc.desc: Test function of OpenFile interface for ERROR which Uri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_OpenFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_OpenFile_0001";
    try {
        Uri uri("");
        int fd;
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        int result = fileAccessHelper->OpenFile(uri, WRITE_READ, fd);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0001 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_OpenFile_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_OpenFile_0001";
}

/**
 * @tc.number: user_file_service_external_file_access_OpenFile_0002
 * @tc.name: external_file_access_OpenFile_0002
 * @tc.desc: Test function of OpenFile interface for ERROR which Uri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_OpenFile_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_OpenFile_0002";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        vector<RootInfo> info;
        EXPECT_NE(fileAccessHelper, nullptr);
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            result = fileAccessHelper->CreateFile(parentUri, "external_file_access_OpenFile_0002.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri uri("storage/media/100/local/files/Download/external_file_access_OpenFile_0002.txt");
            int fd;
            result = fileAccessHelper->OpenFile(uri, WRITE_READ, fd);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "OpenFile_0002 result:" << result;
            result = fileAccessHelper->Delete(newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_OpenFile_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_OpenFile_0002";
}

/**
 * @tc.number: user_file_service_external_file_access_OpenFile_0003
 * @tc.name: external_file_access_OpenFile_0003
 * @tc.desc: Test function of OpenFile interface for ERROR which Uri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_OpenFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_OpenFile_0003";
    try {
        Uri uri("~!@#$%^&*()_");
        int fd;
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        int result = fileAccessHelper->OpenFile(uri, WRITE_READ, fd);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0003 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_OpenFile_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_OpenFile_0003";
}

/**
 * @tc.number: user_file_service_external_file_access_OpenFile_0004
 * @tc.name: external_file_access_OpenFile_0004
 * @tc.desc: Test function of OpenFile interface for ERROR which flag is -1.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_OpenFile_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_OpenFile_0004";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            result = fileAccessHelper->CreateFile(parentUri, "external_file_access_OpenFile_0004.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            int fd;
            int flag = -1;
            result = fileAccessHelper->OpenFile(newFileUri, flag, fd);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "OpenFile_0004 result:" << result;
            result = fileAccessHelper->Delete(newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_OpenFile_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_OpenFile_0004";
}

/**
 * @tc.number: user_file_service_external_file_access_OpenFile_0005
 * @tc.name: external_file_access_OpenFile_0005
 * @tc.desc: Test function of OpenFile interface for SUCCESS which flag is 0.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_OpenFile_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_OpenFile_0005";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            result = fileAccessHelper->CreateFile(parentUri, "external_file_access_OpenFile_0005.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            int fd;
            result = fileAccessHelper->OpenFile(newFileUri, READ, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "OpenFile_0005 result:" << result;
            close(fd);
            result = fileAccessHelper->Delete(newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_OpenFile_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_OpenFile_0005";
}

/**
 * @tc.number: user_file_service_external_file_access_OpenFile_0006
 * @tc.name: external_file_access_OpenFile_0006
 * @tc.desc: Test function of OpenFile interface for SUCCESS which flag is 1.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_OpenFile_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_OpenFile_0006";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            result = fileAccessHelper->CreateFile(parentUri, "external_file_access_OpenFile_0006.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            int fd;
            result = fileAccessHelper->OpenFile(newFileUri, WRITE, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "OpenFile_0006 result:" << result;
            close(fd);
            result = fileAccessHelper->Delete(newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_OpenFile_0006 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_OpenFile_0006";
}

/**
 * @tc.number: user_file_service_external_file_access_OpenFile_0007
 * @tc.name: external_file_access_OpenFile_0007
 * @tc.desc: Test function of OpenFile interface for SUCCESS which flag is 2.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_OpenFile_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_OpenFile_0007";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            result = fileAccessHelper->CreateFile(parentUri, "external_file_access_OpenFile_0007.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            int fd;
            result = fileAccessHelper->OpenFile(newFileUri, WRITE_READ, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "OpenFile_0007 result:" << result;
            close(fd);
            result = fileAccessHelper->Delete(newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_OpenFile_0007 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_OpenFile_0007";
}

static void OpenFileTdd(shared_ptr<FileAccessHelper> fahs, Uri uri, int flag, int fd)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_OpenFileTdd";
    int ret = fahs->OpenFile(uri, flag, fd);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(ERROR) << "OpenFile get result error, code:" << ret;
        return;
    }
    EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_GE(fd, OHOS::FileAccessFwk::ERR_OK);
    g_num++;
    close(fd);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_OpenFileTdd";
}

/**
 * @tc.number: user_file_service_external_file_access_OpenFile_0008
 * @tc.name: external_file_access_OpenFile_0008
 * @tc.desc: Test function of OpenFile interface for SUCCESS which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_OpenFile_0008, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_OpenFile_0008";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            int fd;
            std::string displayName = "test1.txt";
            g_num = 0;
            result = fileAccessHelper->CreateFile(parentUri, displayName, newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            for (size_t j = 0; j < INIT_THREADS_NUMBER; j++) {
                std::thread execthread(OpenFileTdd, fileAccessHelper, newFileUri, WRITE_READ, fd);
                execthread.join();
            }
            EXPECT_EQ(g_num, INIT_THREADS_NUMBER);
            result = fileAccessHelper->Delete(newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_OpenFile_0008 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_OpenFile_0008";
}

/**
 * @tc.number: user_file_service_external_file_access_OpenFile_0009
 * @tc.name: external_file_access_OpenFile_0009
 * @tc.desc: Test function of OpenFile interface for FAILED because of GetProxyByUri failed.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_OpenFile_0009, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_OpenFile_0009";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            std::string str = info[i].uri;
            if (ReplaceBundleName(str, "ohos.com.NotExistBundleName")) {
                int fd;
                Uri newFileUri(str + "/NotExistFile.txt");
                result = fileAccessHelper->OpenFile(newFileUri, WRITE_READ, fd);
                EXPECT_EQ(result, OHOS::FileAccessFwk::E_IPCS);
                GTEST_LOG_(INFO) << "OpenFile_0009 result:" << result;
            } else {
                EXPECT_TRUE(false);
            }
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_OpenFile_0009 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_OpenFile_0009";
}

/**
 * @tc.number: user_file_service_external_file_access_OpenFile_0010
 * @tc.name: external_file_access_OpenFile_0010
 * @tc.desc: Test function of OpenFile interface for SUCCESS, the file name is chinese.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I70SX9
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_OpenFile_0010, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_OpenFile_0010";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            GTEST_LOG_(INFO) << parentUri.ToString();
            Uri newDirUriTest1("");
            result = fileAccessHelper->Mkdir(parentUri, "测试目录", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newDirUriTest1.ToString().find("测试目录"), std::string::npos);
            Uri newFileUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest1, "打开文件.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newFileUri.ToString().find("打开文件.txt"), std::string::npos);
            int fd;
            result = fileAccessHelper->OpenFile(newFileUri, WRITE_READ, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "OpenFile_0010 result:" << result;
            close(fd);
            result = fileAccessHelper->Delete(newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_OpenFile_0010 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_OpenFile_0010";
}

/**
 * @tc.number: user_file_service_external_file_access_Delete_0000
 * @tc.name: external_file_access_Delete_0000
 * @tc.desc: Test function of Delete interface for SUCCESS which delete file.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Delete_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Delete_0000";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri newFileUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest, "external_file_access_Delete_0000.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Delete(newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Delete_0000 result:" << result;
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Delete_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Delete_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_Delete_0001
 * @tc.name: external_file_access_Delete_0001
 * @tc.desc: Test function of Delete interface for SUCCESS which delete folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Delete_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Delete_0001";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Delete_0001 result:" << result;
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Delete_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Delete_0001";
}

/**
 * @tc.number: user_file_service_external_file_access_Delete_0002
 * @tc.name: external_file_access_Delete_0002
 * @tc.desc: Test function of Delete interface for ERROR which selectFileUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Delete_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Delete_0002";
    try {
        Uri selectFileUri("");
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        int result = fileAccessHelper->Delete(selectFileUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Delete_0002 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Delete_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Delete_0002";
}

/**
 * @tc.number: user_file_service_external_file_access_Delete_0003
 * @tc.name: external_file_access_Delete_0003
 * @tc.desc: Test function of Delete interface for ERROR which selectFileUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Delete_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Delete_0003";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri selectFileUri("storage/media/100/local/files/Download/test");
            result = fileAccessHelper->Delete(selectFileUri);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Delete_0003 result:" << result;
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Delete_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Delete_0003";
}

/**
 * @tc.number: user_file_service_external_file_access_Delete_0004
 * @tc.name: external_file_access_Delete_0004
 * @tc.desc: Test function of Delete interface for ERROR which selectFileUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Delete_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Delete_0004";
    try {
        Uri selectFileUri("!@#$%^&*()");
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        int result = fileAccessHelper->Delete(selectFileUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Delete_0004 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Delete_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Delete_0004";
}

static void DeleteTdd(shared_ptr<FileAccessHelper> fahs, Uri selectFile)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_DeleteTdd";
    int ret = fahs->Delete(selectFile);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(ERROR) << "DeleteTdd get result error, code:" << ret;
        return;
    }
    EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
    g_num++;
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_DeleteTdd";
}

/**
 * @tc.number: user_file_service_external_file_access_Delete_0005
 * @tc.name: external_file_access_Delete_0005
 * @tc.desc: Test function of Delete interface for SUCCESS which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Delete_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Delete_0005";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri newFileUri("");
            std::string displayName = "test1.txt";
            result = fileAccessHelper->CreateFile(newDirUriTest, displayName, newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            g_num = 0;
            for (int j = 0; j < INIT_THREADS_NUMBER; j++) {
                std::thread execthread(DeleteTdd, fileAccessHelper, newFileUri);
                execthread.join();
            }
            EXPECT_EQ(g_num, ACTUAL_SUCCESS_THREADS_NUMBER);
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Delete_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Delete_0005";
}

/**
 * @tc.number: user_file_service_external_file_access_Delete_0006
 * @tc.name: external_file_access_Delete_0006
 * @tc.desc: Test function of Delete interface for ERROR because of GetProxyByUri failed.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Delete_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Delete_0006";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            std::string str = info[i].uri;
            if (ReplaceBundleName(str, "ohos.com.NotExistBundleName")) {
                Uri selectFileUri(str);
                int result = fileAccessHelper->Delete(selectFileUri);
                EXPECT_EQ(result, OHOS::FileAccessFwk::E_IPCS);
                GTEST_LOG_(INFO) << "Delete_0006 result:" << result;
            } else {
                EXPECT_TRUE(false);
            }
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Delete_0006 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Delete_0006";
}

/**
 * @tc.number: user_file_service_external_file_access_Delete_0007
 * @tc.name: external_file_access_Delete_0007
 * @tc.desc: Test function of Delete interface for SUCCESS which delete file, the file name is chinese.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I70SX9
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Delete_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Delete_0007";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "测试目录", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newDirUriTest.ToString().find("测试目录"), std::string::npos);
            Uri newFileUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest, "删除文件.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newFileUri.ToString().find("删除文件.txt"), std::string::npos);
            result = fileAccessHelper->Delete(newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Delete_0007 result:" << result;
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Delete_0007 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Delete_0007";
}

/**
 * @tc.number: user_file_service_external_file_access_Move_0000
 * @tc.name: external_file_access_Move_0000
 * @tc.desc: Test function of Move interface for SUCCESS which move file.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_0000";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = fileAccessHelper->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest1, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri2("");
            result = fileAccessHelper->Move(testUri, newDirUriTest2, testUri2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0000 result:" << result;
            result = fileAccessHelper->Delete(newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Delete(newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_Move_0001
 * @tc.name: external_file_access_Move_0001
 * @tc.desc: Test function of Move interface for SUCCESS which move folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_0001";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = fileAccessHelper->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest1, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri2("");
            result = fileAccessHelper->Move(newDirUriTest1, newDirUriTest2, testUri2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0001 result:" << result;
            result = fileAccessHelper->Delete(newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_0001";
}

/**
 * @tc.number: user_file_service_external_file_access_Move_0002
 * @tc.name: external_file_access_Move_0002
 * @tc.desc: Test function of Move interface for ERROR which sourceFileUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_0002";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            Uri sourceFileUri("");
            result = fileAccessHelper->Move(sourceFileUri, newDirUriTest, testUri);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0002 result:" << result;
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_0002";
}

/**
 * @tc.number: user_file_service_external_file_access_Move_0003
 * @tc.name: external_file_access_Move_0003
 * @tc.desc: Test function of Move interface for ERROR which sourceFileUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_0003";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = fileAccessHelper->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest1, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri2("");
            Uri sourceFileUri("storage/media/100/local/files/Download/test1/test.txt");
            result = fileAccessHelper->Move(sourceFileUri, newDirUriTest2, testUri2);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0003 result:" << result;
            result = fileAccessHelper->Delete(newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Delete(newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_0003";
}

/**
 * @tc.number: user_file_service_external_file_access_Move_0004
 * @tc.name: external_file_access_Move_0004
 * @tc.desc: Test function of Move interface for ERROR which sourceFileUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_0004";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            Uri sourceFileUri("~!@#$%^&*()_");
            result = fileAccessHelper->Move(sourceFileUri, newDirUriTest, testUri);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0004 result:" << result;
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_0004";
}

/**
 * @tc.number: user_file_service_external_file_access_Move_0005
 * @tc.name: external_file_access_Move_0005
 * @tc.desc: Test function of Move interface for ERROR which targetParentUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_0005";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = fileAccessHelper->Mkdir(parentUri, "test1", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri2("");
            Uri targetParentUri("");
            result = fileAccessHelper->Move(testUri, targetParentUri, testUri2);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0005 result:" << result;
            result = fileAccessHelper->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_0005";
}

/**
 * @tc.number: user_file_service_external_file_access_Move_0006
 * @tc.name: external_file_access_Move_0006
 * @tc.desc: Test function of Move interface for ERROR which targetParentUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_0006";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = fileAccessHelper->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest1, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri2("");
            Uri targetParentUri("storage/media/100/local/files/Download/test2");
            result = fileAccessHelper->Move(testUri, targetParentUri, testUri2);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0006 result:" << result;
            result = fileAccessHelper->Delete(newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Delete(newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_0006 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_0006";
}

/**
 * @tc.number: user_file_service_external_file_access_Move_0007
 * @tc.name: external_file_access_Move_0007
 * @tc.desc: Test function of Move interface for ERROR which targetParentUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_0007";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = fileAccessHelper->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest1, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri2("");
            Uri targetParentUri("~!@#$^%&*()_");
            result = fileAccessHelper->Move(testUri, targetParentUri, testUri2);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0007 result:" << result;
            result = fileAccessHelper->Delete(newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Delete(newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_0007 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_0007";
}

/**
 * @tc.number: user_file_service_external_file_access_Move_0008
 * @tc.name: external_file_access_Move_0008
 * @tc.desc: Test function of Move interface for SUCCESS which move empty folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_0008, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_0008";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = fileAccessHelper->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            result = fileAccessHelper->Move(newDirUriTest1, newDirUriTest2, testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0008 result:" << result;
            result = fileAccessHelper->Delete(newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_0008 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_0008";
}

/**
 * @tc.number: user_file_service_external_file_access_Move_0009
 * @tc.name: external_file_access_Move_0009
 * @tc.desc: Test function of Move interface for SUCCESS which move more file in folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_0009, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_0009";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = fileAccessHelper->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            size_t fileNumbers = 2000;
            for (size_t j = 0; j < fileNumbers; j++) {
                string fileName = "test" + ToString(j) + ".txt";
                result = fileAccessHelper->CreateFile(newDirUriTest1, fileName, testUri);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            }
            Uri testUri2("");
            result = fileAccessHelper->Move(newDirUriTest1, newDirUriTest2, testUri2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0009 result:" << result;
            result = fileAccessHelper->Delete(newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_0009 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_0009";
}

/**
 * @tc.number: user_file_service_external_file_access_Move_0010
 * @tc.name: external_file_access_Move_0010
 * @tc.desc: Test function of Move interface for SUCCESS which move Multilevel directory folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_0010, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_0010";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = fileAccessHelper->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            result = fileAccessHelper->Mkdir(newDirUriTest1, "test", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            size_t directoryNumbers = 64;
            for (size_t j = 0; j < directoryNumbers; j++) {
                result = fileAccessHelper->Mkdir(testUri, "test", testUri);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            }
            Uri testUri2("");
            result = fileAccessHelper->Move(newDirUriTest1, newDirUriTest2, testUri2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0010 result:" << result;
            result = fileAccessHelper->Delete(newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_0010 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_0010";
}

/**
 * @tc.number: user_file_service_external_file_access_Move_0011
 * @tc.name: external_file_access_Move_0011
 * @tc.desc: Test function of Move interface for SUCCESS which move other equipment file.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_0011, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_0011";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = fileAccessHelper->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest1, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            for (size_t j = i + 1; j < info.size(); j++) {
                Uri otherUri(info[j].uri);
                result = fileAccessHelper->Mkdir(otherUri, "test2", newDirUriTest2);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
                result = fileAccessHelper->Move(testUri, newDirUriTest2, testUri);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
                result = fileAccessHelper->Move(testUri, newDirUriTest1, testUri);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
                GTEST_LOG_(INFO) << "Move_0011 result:" << result;
                result = fileAccessHelper->Delete(newDirUriTest2);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            }
            result = fileAccessHelper->Delete(newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_0011 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_0011";
}

static void MoveTdd(shared_ptr<FileAccessHelper> fahs, Uri sourceFile, Uri targetParent, Uri newFile)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_MoveTdd";
    int ret = fahs->Move(sourceFile, targetParent, newFile);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(ERROR) << "MoveTdd get result error, code:" << ret;
        return;
    }
    EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
    g_num++;
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_MoveTdd";
}

/**
 * @tc.number: user_file_service_external_file_access_Move_0012
 * @tc.name: external_file_access_Move_0012
 * @tc.desc: Test function of Move interface for SUCCESS which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_0012, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_0012";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = fileAccessHelper->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri("");
            std::string displayName = "test3.txt";
            result = fileAccessHelper->CreateFile(newDirUriTest1, displayName, testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri2("");
            g_num = 0;
            for (int j = 0; j < INIT_THREADS_NUMBER; j++) {
                std::thread execthread(MoveTdd, fileAccessHelper, testUri, newDirUriTest2, testUri2);
                execthread.join();
            }
            EXPECT_EQ(g_num, ACTUAL_SUCCESS_THREADS_NUMBER);
            result = fileAccessHelper->Delete(newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Delete(newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_0012 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_0012";
}

/**
 * @tc.number: user_file_service_external_file_access_Move_0013
 * @tc.name: external_file_access_Move_0013
 * @tc.desc: Test function of Move interface for FAILED because of GetProxyByUri failed.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_0013, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_0013";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        result = fileAccessHelper->Mkdir(parentUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = fileAccessHelper->Mkdir(parentUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        result = fileAccessHelper->Mkdir(newDirUriTest1, "test", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string str = newDirUriTest1.ToString();
        if (!ReplaceBundleNameFromPath(str, "ohos.com.NotExistBundleName")) {
            GTEST_LOG_(ERROR) << "replace BundleName failed.";
            EXPECT_TRUE(false);
        }
        Uri uri(str);
        Uri testUri2("");
        result = fileAccessHelper->Move(uri, newDirUriTest2, testUri2);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0013 result:" << result;
        result = fileAccessHelper->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = fileAccessHelper->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_0013 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_0013";
}

/**
 * @tc.number: user_file_service_external_file_access_Move_0014
 * @tc.name: external_file_access_Move_0014
 * @tc.desc: Test function of Move interface for SUCCESS which move file, the file name is chinese.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I70SX9
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_0014, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_0014";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = fileAccessHelper->Mkdir(parentUri, "测试目录1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newDirUriTest1.ToString().find("测试目录1"), std::string::npos);
            result = fileAccessHelper->Mkdir(parentUri, "测试目录2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newDirUriTest2.ToString().find("测试目录2"), std::string::npos);
            Uri testUri("");
            result = fileAccessHelper->CreateFile(newDirUriTest1, "移动文件.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(testUri.ToString().find("移动文件.txt"), std::string::npos);
            Uri testUri2("");
            result = fileAccessHelper->Move(testUri, newDirUriTest2, testUri2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0014 result:" << result;
            result = fileAccessHelper->Delete(newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Delete(newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_0014 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_0014";
}

/**
 * @tc.number: user_file_service_external_file_access_Move_0015
 * @tc.name: external_file_access_Move_0015
 * @tc.desc: Test function of Move interface for SUCCESS which move Multilevel directory folder，
 *           the folder name is chinese.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I70SX9
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_0015, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_0015";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = fileAccessHelper->Mkdir(parentUri, "移动目录", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newDirUriTest1.ToString().find("移动目录"), std::string::npos);
            result = fileAccessHelper->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri2("");
            result = fileAccessHelper->Move(newDirUriTest1, newDirUriTest2, testUri2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0015 result:" << result;
            result = fileAccessHelper->Delete(newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_0015 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_0015";
}

/**
 * @tc.number: user_file_service_external_file_access_Move_0016
 * @tc.name: external_file_access_Move_0016
 * @tc.desc: Test function of Move interface for SUCCESS which move Multilevel directory folder，
 *           the folder name is chinese.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I70SX9
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_0016, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_0016";
    try {
        shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
        EXPECT_NE(fileAccessHelper, nullptr);
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = fileAccessHelper->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = fileAccessHelper->Mkdir(parentUri, "移动目录", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri2("");
            result = fileAccessHelper->Move(newDirUriTest1, newDirUriTest2, testUri2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0016 result:" << result;
            result = fileAccessHelper->Delete(newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_0016 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_0016";
}

/**
 * @tc.number: user_file_service_external_file_access_creator_0000
 * @tc.name: external_file_access_creator_0000
 * @tc.desc: Test function of creator interface.
 * @tc.desc: create file access helper for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_creator_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_creator_0000";
    try {
        setuid(UID_TRANSFORM_TMP);
        std::pair<shared_ptr<FileAccessHelper>, int> helper =
            FileAccessHelper::Creator(FileExtensionHelperTest::GetContext());
        setuid(UID_DEFAULT);
        ASSERT_TRUE(helper.first != nullptr);
        bool succ = helper.first->Release();
        EXPECT_TRUE(succ);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_creator_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_creator_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_creator_0001
 * @tc.name: external_file_access_creator_0001
 * @tc.desc: Test function of creator interface.
 * @tc.desc: create file access helper for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_creator_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_creator_0001";
    try {
        AAFwk::Want want;
        vector<AAFwk::Want> wantVec;
        setuid(UID_TRANSFORM_TMP);
        int ret = FileAccessHelper::GetRegisteredFileAccessExtAbilityInfo(wantVec);
        EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
        bool sus = false;
        for (size_t i = 0; i < wantVec.size(); i++) {
            auto element = wantVec[i].GetElement();
            if (element.GetBundleName() == "com.ohos.UserFile.ExternalFileManager" &&
                element.GetAbilityName() == "FileExtensionAbility") {
                want = wantVec[i];
                sus = true;
                break;
            }
        }
        EXPECT_TRUE(sus);
        vector<AAFwk::Want> wants{want};
        std::pair<shared_ptr<FileAccessHelper>, int> helper =
            FileAccessHelper::Creator(FileExtensionHelperTest::GetContext(), wants);
        setuid(UID_DEFAULT);
        ASSERT_TRUE(helper.first != nullptr);
        bool succ = helper.first->Release();
        EXPECT_TRUE(succ);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_creator_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_creator_0001";
}

/**
 * @tc.number: user_file_service_external_file_access_creator_0002
 * @tc.name: external_file_access_creator_0002
 * @tc.desc: Test function of creator interface.
 * @tc.desc: create file access helper for FAILED.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_creator_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_creator_0002";
    try {
        std::shared_ptr<OHOS::AbilityRuntime::Context> context = nullptr;
        std::pair<shared_ptr<FileAccessHelper>, int> helper = FileAccessHelper::Creator(context);
        ASSERT_TRUE(helper.first == nullptr);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_creator_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_creator_0002";
}

/**
 * @tc.number: user_file_service_external_file_access_creator_0003
 * @tc.name: external_file_access_creator_0003
 * @tc.desc: Test function of creator interface.
 * @tc.desc: create file access helper for FAILED.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_creator_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_creator_0003";
    try {
        std::vector<AAFwk::Want> wants;
        std::shared_ptr<OHOS::AbilityRuntime::Context> context = nullptr;
        std::pair<shared_ptr<FileAccessHelper>, int> helper = FileAccessHelper::Creator(context, wants);
        ASSERT_TRUE(helper.first == nullptr);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_creator_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_creator_0003";
}

/**
 * @tc.number: user_file_service_external_file_access_creator_0004
 * @tc.name: external_file_access_creator_0004
 * @tc.desc: Test function of creator interface.
 * @tc.desc: create file access helper for FAILED.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_creator_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_creator_0004";
    try {
        std::vector<AAFwk::Want> wants;
        std::pair<shared_ptr<FileAccessHelper>, int> helper =
            FileAccessHelper::Creator(FileExtensionHelperTest::GetContext(), wants);
        ASSERT_TRUE(helper.first == nullptr);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_creator_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_creator_0004";
}

/**
 * @tc.number: user_file_service_external_file_access_creator_0005
 * @tc.name: external_file_access_creator_0005
 * @tc.desc: Test function of creator interface.
 * @tc.desc: create file access helper for FAILED.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_creator_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_creator_0005";
    try {
        AAFwk::Want want;
        want.SetElementName("NotExistBundleName", "NotExistAbilityName");
        std::vector<AAFwk::Want> wants{want};
        std::pair<shared_ptr<FileAccessHelper>, int> helper =
            FileAccessHelper::Creator(FileExtensionHelperTest::GetContext(), wants);
        ASSERT_TRUE(helper.first == nullptr);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_creator_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_creator_0005";
}

/**
 * @tc.number: user_file_service_external_file_access_creator_0006
 * @tc.name: external_file_access_creator_0006
 * @tc.desc: Test function of creator interface.
 * @tc.desc: create file access helper for FAILED.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_creator_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_creator_0006";
    try {
        std::vector<AAFwk::Want> wants;
        std::shared_ptr<OHOS::AbilityRuntime::Context> context = nullptr;
        std::pair<shared_ptr<FileAccessHelper>, int> helper = FileAccessHelper::Creator(context, wants);
        ASSERT_TRUE(helper.first == nullptr);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_creator_0006 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_creator_0006";
}

/**
 * @tc.number: user_file_service_external_file_access_creator_0007
 * @tc.name: external_file_access_creator_0007
 * @tc.desc: Test function of creator interface.
 * @tc.desc: create file access helper for FAILED.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_creator_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_creator_0007";
    try {
        std::vector<AAFwk::Want> wants;
        shared_ptr<FileAccessHelper> helper =
            FileAccessHelper::Creator(FileExtensionHelperTest::GetContext()->GetToken(), wants);
        ASSERT_TRUE(helper == nullptr);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_creator_0007 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_creator_0007";
}

/**
 * @tc.number: user_file_service_external_file_access_creator_0008
 * @tc.name: external_file_access_creator_0008
 * @tc.desc: Test function of creator interface.
 * @tc.desc: create file access helper for FAILED.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_creator_0008, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_creator_0008";
    try {
        AAFwk::Want want;
        want.SetElementName("NotExistBundleName", "NotExistAbilityName");
        std::vector<AAFwk::Want> wants{want};
        shared_ptr<FileAccessHelper> helper =
            FileAccessHelper::Creator(FileExtensionHelperTest::GetContext()->GetToken(), wants);
        ASSERT_TRUE(helper == nullptr);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_creator_0008 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_creator_0008";
}
}
