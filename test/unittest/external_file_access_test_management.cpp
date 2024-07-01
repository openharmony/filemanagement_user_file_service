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
 * @tc.number: user_file_service_external_file_access_CreateFile_0000
 * @tc.name: external_file_access_CreateFile_0000
 * @tc.desc: Test function of CreateFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_CreateFile_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_CreateFile_0000";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            result = fileAccessHelper->CreateFile(parentUri, "external_file_access_CreateFile_0000.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "CreateFile_0000 result:" << result;
            result = fileAccessHelper->Delete(newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_CreateFile_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_CreateFile_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_CreateFile_0001
 * @tc.name: external_file_access_CreateFile_0001
 * @tc.desc: Test function of CreateFile interface for ERROR which parentUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_CreateFile_0001, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_CreateFile_0001";
    try {
        Uri newFileUri("");
        Uri parentUri("");
        int result = fileAccessHelper->CreateFile(parentUri, "external_file_access_CreateFile_0001.txt", newFileUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "CreateFile_0001 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_CreateFile_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_CreateFile_0001";
}

/**
 * @tc.number: user_file_service_external_file_access_CreateFile_0002
 * @tc.name: external_file_access_CreateFile_0002
 * @tc.desc: Test function of CreateFile interface for ERROR which parentUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_CreateFile_0002, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_CreateFile_0002";
    try {
        Uri newFileUri("");
        Uri parentUri("storage/media/100/local/files/Download");
        int result = fileAccessHelper->CreateFile(parentUri, "external_file_access_CreateFile_0002.txt", newFileUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "CreateFile_0002 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_CreateFile_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_CreateFile_0002";
}

/**
 * @tc.number: user_file_service_external_file_access_CreateFile_0003
 * @tc.name: external_file_access_CreateFile_0003
 * @tc.desc: Test function of CreateFile interface for ERROR which parentUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_CreateFile_0003, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_CreateFile_0003";
    try {
        Uri newFileUri("");
        Uri parentUri("~!@#$%^&*()_");
        int result = fileAccessHelper->CreateFile(parentUri, "external_file_access_CreateFile_0003.txt", newFileUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "CreateFile_0003 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_CreateFile_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_CreateFile_0003";
}

/**
 * @tc.number: user_file_service_external_file_access_CreateFile_0004
 * @tc.name: external_file_access_CreateFile_0004
 * @tc.desc: Test function of CreateFile interface for ERROR which displayName is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_CreateFile_0004, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_CreateFile_0004";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            string displayName = "";
            result = fileAccessHelper->CreateFile(parentUri, displayName, newFileUri);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "CreateFile_0004 result:" << result;
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_CreateFile_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_CreateFile_0004";
}

static void CreateFileTdd(shared_ptr<FileAccessHelper> fahs, Uri uri, std::string displayName, Uri newFile)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_CreateFileTdd";
    int ret = fahs->CreateFile(uri, displayName, newFile);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(ERROR) << "CreateFileTdd get result error, code:" << ret;
        return;
    }
    EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_NE(newFile.ToString(), "");
    g_num++;
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_CreateFileTdd";
}

/**
 * @tc.number: user_file_service_external_file_access_CreateFile_0005
 * @tc.name: external_file_access_CreateFile_0005
 * @tc.desc: Test function of CreateFile interface for ERROR which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_CreateFile_0005, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_CreateFile_0005";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            std::string displayName = "external_file_access_CreateFile_0005.txt";
            g_num = 0;
            for (int j = 0; j < INIT_THREADS_NUMBER; j++) {
                std::thread execthread(CreateFileTdd, fileAccessHelper, parentUri, displayName, newFileUri);
                execthread.join();
            }
            EXPECT_EQ(g_num, ACTUAL_SUCCESS_THREADS_NUMBER);
            Uri newDelete(info[i].uri + "/" + displayName);
            result = fileAccessHelper->Delete(newDelete);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_CreateFile_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_CreateFile_0005";
}

/**
 * @tc.number: user_file_service_external_file_access_CreateFile_0006
 * @tc.name: external_file_access_CreateFile_0006
 * @tc.desc: Test function of CreateFile interface for ERROR because of GetProxyByUri failed.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_CreateFile_0006, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_CreateFile_0006";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            std::string str = info[i].uri;
            if (ReplaceBundleName(str, "ohos.com.NotExistBundleName")) {
                Uri parentUri(str);
                Uri newFileUri("");
                string displayName = "test.txt";
                result = fileAccessHelper->CreateFile(parentUri, displayName, newFileUri);
                EXPECT_EQ(result, OHOS::FileAccessFwk::E_IPCS);
                GTEST_LOG_(INFO) << "CreateFile_0006 result:" << result;
            } else {
                EXPECT_TRUE(false);
            }
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_CreateFile_0006 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_CreateFile_0006";
}

/**
 * @tc.number: user_file_service_external_file_access_CreateFile_0007
 * @tc.name: external_file_access_CreateFile_0007
 * @tc.desc: Test function of CreateFile interface for SUCCESS, the file name is chinese.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I70SX9
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_CreateFile_0007, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_CreateFile_0007";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            result = fileAccessHelper->CreateFile(parentUri, "新建文件.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newFileUri.ToString().find("新建文件.txt"), std::string::npos);
            GTEST_LOG_(INFO) << "CreateFile_0007 result:" << result;
            result = fileAccessHelper->Delete(newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_CreateFile_0007 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_CreateFile_0007";
}

/**
* @tc.number: user_file_service_external_file_access_Move_file_0000
* @tc.name: external_file_access_Move_file_0000
* @tc.desc: Test function of Move file interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_file_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_file_0000";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = fileAccessHelper->Mkdir(parentUri, "测试目录1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri(newDirUriTest1.ToString() + "/" + "test");

        Uri newDirUriTest2("");
        result = fileAccessHelper->Mkdir(parentUri, "测试目录3", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newFile("");
        std::string fileName = "a";
        result = fileAccessHelper->MoveFile(sourceUri, newDirUriTest2, fileName, newFile);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_URI);
        result = fileAccessHelper->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = fileAccessHelper->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_file_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_file_0000";
}

/**
* @tc.number: user_file_service_external_file_access_Move_file_0001
* @tc.name: external_file_access_Move_file_0001
* @tc.desc: Test function of Move file interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_file_001, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_file_0001";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = fileAccessHelper->Mkdir(parentUri, "test3", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri("");
        result = fileAccessHelper->CreateFile(newDirUriTest1, "test.txt", sourceUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri newDirUriTest2("");
        result = fileAccessHelper->Mkdir(parentUri, "test4", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri targetUri(newDirUriTest2.ToString() + "/" + "test5");
        std::string fileName("a.txt");
        Uri newFile("");
        result = fileAccessHelper->MoveFile(sourceUri, targetUri, fileName, newFile);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_URI);
        result = fileAccessHelper->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = fileAccessHelper->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_file_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_file_0001";
}

/**
* @tc.number: user_file_service_external_file_access_Move_file_0002
* @tc.name: external_file_access_Move_file_0002
* @tc.desc: Test function of Move file interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_file_002, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_file_0002";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = fileAccessHelper->Mkdir(parentUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri newDirUriTest2("");
        result = fileAccessHelper->Mkdir(parentUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string fileName("a.txt");
        Uri newFile("");
        result = fileAccessHelper->MoveFile(newDirUriTest1, newDirUriTest2, fileName, newFile);
        EXPECT_EQ(result, OHOS::FileAccessFwk::E_URIS);
        result = fileAccessHelper->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = fileAccessHelper->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_file_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_file_0002";
}

/**
* @tc.number: user_file_service_external_file_access_Move_file_0003
* @tc.name: external_file_access_Move_file_0003
* @tc.desc: Test function of Move file interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_file_003, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_file_0003";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = fileAccessHelper->Mkdir(parentUri, "test3", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri("");
        result = fileAccessHelper->CreateFile(newDirUriTest1, "test.txt", sourceUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri newDirUriTest2("");
        result = fileAccessHelper->Mkdir(parentUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string fileName("a.txt");
        Uri newFile("");
        result = fileAccessHelper->MoveFile(sourceUri, newDirUriTest2, fileName, newFile);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(newFile.GetPath(), newDirUriTest2.GetPath() + '/' +"test.txt");
        result = fileAccessHelper->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = fileAccessHelper->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_file_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_file_0003";
}

/**
* @tc.number: user_file_service_external_file_access_Move_file_0004
* @tc.name: external_file_access_Move_file_0004
* @tc.desc: Test function of Move file interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_file_004, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_file_0004";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = fileAccessHelper->Mkdir(parentUri, "test4", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri("");
        result = fileAccessHelper->CreateFile(newDirUriTest1, "test.txt", sourceUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri newDirUriTest2("");
        result = fileAccessHelper->Mkdir(parentUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri targetUri("");
        result = fileAccessHelper->CreateFile(newDirUriTest2, "test.txt", targetUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string fileName("a.txt");
        Uri newFile("");
        result = fileAccessHelper->MoveFile(sourceUri, newDirUriTest2, fileName, newFile);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(newFile.GetPath(), newDirUriTest2.GetPath() + '/' +"a.txt");
        result = fileAccessHelper->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = fileAccessHelper->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_file_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_file_0004";
}

/**
* @tc.number: user_file_service_external_file_access_Move_file_0005
* @tc.name: external_file_access_Move_file_0005
* @tc.desc: Test function of Move file interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_file_005, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_file_0005";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = fileAccessHelper->Mkdir(parentUri, "test5", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri("");
        result = fileAccessHelper->CreateFile(newDirUriTest1, "test1.txt", sourceUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri newDirUriTest2("");
        result = fileAccessHelper->Mkdir(parentUri, "test05", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri targetUri1("");
        result = fileAccessHelper->CreateFile(newDirUriTest2, "test5.txt", targetUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri targetUri2("");
        result = fileAccessHelper->CreateFile(newDirUriTest2, "test05.txt", targetUri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string fileName("test05.txt");
        Uri newFile("");
        result = fileAccessHelper->MoveFile(sourceUri, newDirUriTest2, fileName, newFile);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = fileAccessHelper->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = fileAccessHelper->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_file_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_file_0005";
}

/**
* @tc.number: user_file_service_external_file_access_Copy_file_000
* @tc.name: external_file_access_Copy_file_000
* @tc.desc: Test function of Copy file interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Copy_file_000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Copy_file_000";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = fileAccessHelper->Mkdir(parentUri, "test3", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri(newDirUriTest1.ToString() + "/" + "test");

        Uri newDirUriTest2("");
        result = fileAccessHelper->Mkdir(parentUri, "test5", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newFile("");
        std::string fileName = "a";
        result = fileAccessHelper->CopyFile(sourceUri, newDirUriTest2, fileName, newFile);
        EXPECT_EQ(result, OHOS::FileAccessFwk::E_GETRESULT);
        result = fileAccessHelper->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = fileAccessHelper->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Copy_file_000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Copy_file_000";
}

/**
* @tc.number: user_file_service_external_file_access_Copy_file_001
* @tc.name: external_file_access_Copy_file_001
* @tc.desc: Test function of Copy file interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Copy_file_001, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Copy_file_001";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = fileAccessHelper->Mkdir(parentUri, "test6", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri("");
        result = fileAccessHelper->CreateFile(newDirUriTest1, "test.txt", sourceUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri newDirUriTest2("");
        result = fileAccessHelper->Mkdir(parentUri, "test7", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri targetUri(newDirUriTest2.ToString() + "/" + "test8");
        std::string fileName("a.txt");
        Uri newFile("");
        result = fileAccessHelper->CopyFile(sourceUri, targetUri, fileName, newFile);
        EXPECT_EQ(result, OHOS::FileAccessFwk::E_GETRESULT);
        result = fileAccessHelper->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = fileAccessHelper->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Copy_file_001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Copy_file_001";
}

/**
* @tc.number: user_file_service_external_file_access_Copy_file_002
* @tc.name: external_file_access_Copy_file_002
* @tc.desc: Test function of Copy file interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Copy_file_002, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Copy_file_002";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = fileAccessHelper->Mkdir(parentUri, "test3", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri newDirUriTest2("");
        result = fileAccessHelper->Mkdir(parentUri, "test4", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string fileName("a.txt");
        Uri newFile("");
        result = fileAccessHelper->CopyFile(newDirUriTest1, newDirUriTest2, fileName, newFile);
        EXPECT_EQ(result, OHOS::FileAccessFwk::E_URIS);
        result = fileAccessHelper->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = fileAccessHelper->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Copy_file_002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Copy_file_002";
}

/**
* @tc.number: user_file_service_external_file_access_Copy_file_003
* @tc.name: external_file_access_Copy_file_003
* @tc.desc: Test function of Copy file interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Copy_file_003, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Copy_file_003";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = fileAccessHelper->Mkdir(parentUri, "test7", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri("");
        result = fileAccessHelper->CreateFile(newDirUriTest1, "test.txt", sourceUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri newDirUriTest2("");
        result = fileAccessHelper->Mkdir(parentUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri targetUri(newDirUriTest2.ToString() + "/" + "test.txt");
        std::string fileName("a.txt");
        Uri newFile("");
        result = fileAccessHelper->CopyFile(sourceUri, targetUri, fileName, newFile);
        EXPECT_EQ(result, OHOS::FileAccessFwk::E_GETRESULT);
        result = fileAccessHelper->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = fileAccessHelper->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Copy_file_003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Copy_file_003";
}

/**
* @tc.number: user_file_service_external_file_access_Copy_file_004
* @tc.name: external_file_access_Copy_file_004
* @tc.desc: Test function of Copy file interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Copy_file_004, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Copy_file_004";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = fileAccessHelper->Mkdir(parentUri, "test8", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri("");
        result = fileAccessHelper->CreateFile(newDirUriTest1, "test.txt", sourceUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri newDirUriTest2("");
        result = fileAccessHelper->Mkdir(parentUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri targetUri("");
        result = fileAccessHelper->CreateFile(newDirUriTest2, "test.txt", targetUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string fileName("a.txt");
        Uri newFile("");
        result = fileAccessHelper->CopyFile(sourceUri, targetUri, fileName, newFile);
        EXPECT_EQ(result, OHOS::FileAccessFwk::E_URIS);
        result = fileAccessHelper->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = fileAccessHelper->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Copy_file_004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Copy_file_004";
}

/**
* @tc.number: user_file_service_external_file_access_Copy_file_005
* @tc.name: external_file_access_Copy_file_005
* @tc.desc: Test function of Copy file interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Copy_file_005, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Copy_file_005";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = fileAccessHelper->Mkdir(parentUri, "test9", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri("");
        result = fileAccessHelper->CreateFile(newDirUriTest1, "test.txt", sourceUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri newDirUriTest10("");
        result = fileAccessHelper->Mkdir(parentUri, "test10", newDirUriTest10);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string fileName("a.txt");
        Uri newFile("");
        result = fileAccessHelper->CopyFile(sourceUri, newDirUriTest10, fileName, newFile);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(newFile.GetPath(), newDirUriTest10.GetPath() + '/' +"test.txt");
        result = fileAccessHelper->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = fileAccessHelper->Delete(newDirUriTest10);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Copy_file_005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Copy_file_005";
}

/**
* @tc.number: user_file_service_external_file_access_Copy_file_006
* @tc.name: external_file_access_Copy_file_006
* @tc.desc: Test function of Copy file interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Copy_file_006, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Copy_file_006";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = fileAccessHelper->Mkdir(parentUri, "test10", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri("");
        result = fileAccessHelper->CreateFile(newDirUriTest1, "test.txt", sourceUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri newDirUriTest2("");
        result = fileAccessHelper->Mkdir(parentUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri targetUri("");
        result = fileAccessHelper->CreateFile(newDirUriTest2, "test.txt", targetUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string fileName("a.txt");
        Uri newFile("");
        result = fileAccessHelper->CopyFile(sourceUri, newDirUriTest2, fileName, newFile);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(newFile.GetPath(), newDirUriTest2.GetPath() + '/' +"a.txt");
        result = fileAccessHelper->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = fileAccessHelper->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Copy_file_006 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Copy_file_006";
}

/**
* @tc.number: user_file_service_external_file_access_Copy_file_007
* @tc.name: external_file_access_Copy_file_007
* @tc.desc: Test function of Copy file interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Copy_file_007, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Copy_file_007";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = fileAccessHelper->Mkdir(parentUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri("");
        result = fileAccessHelper->CreateFile(newDirUriTest1, "test1.txt", sourceUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri newDirUriTest2("");
        result = fileAccessHelper->Mkdir(parentUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri targetUri1("");
        result = fileAccessHelper->CreateFile(newDirUriTest2, "test1.txt", targetUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri targetUri2("");
        result = fileAccessHelper->CreateFile(newDirUriTest2, "test2.txt", targetUri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string fileName("test2.txt");
        Uri newFile("");
        result = fileAccessHelper->CopyFile(sourceUri, newDirUriTest2, fileName, newFile);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_EXIST);
        result = fileAccessHelper->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = fileAccessHelper->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Copy_file_007 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Copy_file_007";
}

/**
* @tc.number: user_file_service_external_file_access_Copy_file_008
* @tc.name: external_file_access_Copy_file_008
* @tc.desc: Test function of Copy file interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Copy_file_008, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Copy_file_008";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = fileAccessHelper->Mkdir(parentUri, "测试目录1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri("");
        result = fileAccessHelper->CreateFile(newDirUriTest1, "test.txt", sourceUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri newDirUriTest2("");
        result = fileAccessHelper->Mkdir(parentUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string fileName("a.txt");
        Uri newFile("");
        result = fileAccessHelper->CopyFile(sourceUri, newDirUriTest2, fileName, newFile);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(newFile.GetPath(), newDirUriTest2.GetPath() + '/' +"test.txt");
        result = fileAccessHelper->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = fileAccessHelper->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Copy_file_008 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Copy_file_008";
}

/**
* @tc.number: user_file_service_external_file_access_Copy_file_009
* @tc.name: external_file_access_Copy_file_009
* @tc.desc: Test function of Copy file interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Copy_file_009, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> fileAccessHelper = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Copy_file_009";
    try {
        vector<RootInfo> info;
        int result = fileAccessHelper->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = fileAccessHelper->Mkdir(parentUri, "测试1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri("");
        result = fileAccessHelper->CreateFile(newDirUriTest1, "test.txt", sourceUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri newDirUriTest2("");
        result = fileAccessHelper->Mkdir(parentUri, "测试2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri targetUri("");
        result = fileAccessHelper->CreateFile(newDirUriTest2, "test.txt", targetUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string fileName("测试.txt");
        Uri newFile("");
        result = fileAccessHelper->CopyFile(sourceUri, newDirUriTest2, fileName, newFile);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(newDirUriTest1.ToString().find("测试.txt"), std::string::npos);
        result = fileAccessHelper->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = fileAccessHelper->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Copy_file_009 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Copy_file_009";
}
} // namespace
