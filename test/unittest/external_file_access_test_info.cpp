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

#include "external_file_access_test_basic.h"
#include "accesstoken_kit.h"
#include "context_impl.h"
#include "file_access_framework_errno.h"
#include "file_info_shared_memory.h"
#include "iservice_registry.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

#define private public
#include "file_access_helper.h"
#undef private

namespace OHOS::FileAccessFwk {
using json = nlohmann::json;
static int g_num = 0;

/**
 * @tc.number: user_file_service_external_file_access_GetRoots_0000
 * @tc.name: external_file_access_GetRoots_0000
 * @tc.desc: Test function of GetRoots interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0387
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_GetRoots_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_GetRoots_0000";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_GT(info.size(), OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "GetRoots_0000 result:" << info.size() << endl;
        for (size_t i = 0; i < info.size(); i++) {
            GTEST_LOG_(INFO) << info[i].uri;
            GTEST_LOG_(INFO) << info[i].displayName;
            GTEST_LOG_(INFO) << info[i].deviceFlags;
            GTEST_LOG_(INFO) << info[i].deviceType;
            GTEST_LOG_(INFO) << info[i].relativePath;
        }
        EXPECT_GE(info.size(), 2);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_GetRoots_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_GetRoots_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_Access_0000
 * @tc.name: external_file_access_Access_0000
 * @tc.desc: Test function of Access interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Access_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Access_0000";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            GTEST_LOG_(INFO) << parentUri.ToString();
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "test1", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri newFileUri("");
            result = g_fah->CreateFile(newDirUriTest, "external_file_access_Access_0000.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            bool isExist = false;
            result = g_fah->Access(newDirUriTest, isExist);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_TRUE(isExist);
            result = g_fah->Access(newFileUri, isExist);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_TRUE(isExist);
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = g_fah->Access(newDirUriTest, isExist);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_FALSE(isExist);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Access_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Access_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_Access_0001
 * @tc.name: external_file_access_Access_0001
 * @tc.desc: Test function of Access interface FAILED because of GetProxyByUri failed.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Access_0001, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Access_0001";
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
            Uri newDirUriTest(str + "/test.txt");
            bool isExist = false;
            result = g_fah->Access(newDirUriTest, isExist);
            EXPECT_EQ(result, OHOS::FileAccessFwk::E_IPCS);
            EXPECT_FALSE(isExist);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Access_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Access_0001";
}

/**
 * @tc.number: user_file_service_external_file_access_Access_0002
 * @tc.name: external_file_access_Access_0002
 * @tc.desc: Test function of Access interface for SUCCESS, the file and folder name is chinese.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I70SX9
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Access_0002, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Access_0002";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            GTEST_LOG_(INFO) << parentUri.ToString();
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "访问目录", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newDirUriTest.ToString().find("访问目录"), std::string::npos);
            Uri newFileUri("");
            result = g_fah->CreateFile(newDirUriTest, "访问文件.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newFileUri.ToString().find("访问文件.txt"), std::string::npos);
            bool isExist = false;
            result = g_fah->Access(newDirUriTest, isExist);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_TRUE(isExist);
            result = g_fah->Access(newFileUri, isExist);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_TRUE(isExist);
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = g_fah->Access(newDirUriTest, isExist);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_FALSE(isExist);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Access_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Access_0002";
}

/**
 * @tc.number: user_file_service_external_file_access_GetFileInfoFromUri_0000
 * @tc.name: external_file_access_GetFileInfoFromUri_0000
 * @tc.desc: Test function of GetFileInfoFromUri interface.
 * @tc.desc: convert the root directory uri to fileinfo and call listfile for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_GetFileInfoFromUri_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_GetFileInfoFromUri_0000";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        SharedMemoryInfo memInfo;
        result = FileAccessFwk::SharedMemoryOperation::CreateSharedMemory("FileInfo List", DEFAULT_CAPACITY_200KB,
            memInfo);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            FileInfo fileinfo;
            result = g_fah->GetFileInfoFromUri(parentUri, fileinfo);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            int64_t offset = 0;
            FileFilter filter;
            result = g_fah->ListFile(fileinfo, offset, filter, memInfo);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_GE(memInfo.Size(), OHOS::FileAccessFwk::ERR_OK);
        }
        FileAccessFwk::SharedMemoryOperation::DestroySharedMemory(memInfo);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_GetFileInfoFromUri_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_GetFileInfoFromUri_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_GetFileInfoFromUri_0001
 * @tc.name: external_file_access_GetFileInfoFromUri_0001
 * @tc.desc: Test function of GetFileInfoFromUri interface.
 * @tc.desc: convert the general directory uri to fileinfo and call listfile for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_GetFileInfoFromUri_0001, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_GetFileInfoFromUri_0001";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        SharedMemoryInfo memInfo;
        result = FileAccessFwk::SharedMemoryOperation::CreateSharedMemory("FileInfo List", DEFAULT_CAPACITY_200KB,
            memInfo);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "testDir", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            FileInfo dirInfo;
            result = g_fah->GetFileInfoFromUri(newDirUriTest, dirInfo);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            int64_t offset = 0;
            FileFilter filter;
            result = g_fah->ListFile(dirInfo, offset, filter, memInfo);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_GE(memInfo.Size(), OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
        FileAccessFwk::SharedMemoryOperation::DestroySharedMemory(memInfo);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_GetFileInfoFromUri_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_GetFileInfoFromUri_0001";
}

/**
 * @tc.number: user_file_service_external_file_access_GetFileInfoFromUri_0002
 * @tc.name: external_file_access_GetFileInfoFromUri_0002
 * @tc.desc: Test function of GetFileInfoFromUri interface.
 * @tc.desc: convert the general filepath uri to fileinfo and call listfile for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_GetFileInfoFromUri_0002, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_GetFileInfoFromUri_0002";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        SharedMemoryInfo memInfo;
        result = FileAccessFwk::SharedMemoryOperation::CreateSharedMemory("FileInfo List", DEFAULT_CAPACITY_200KB,
            memInfo);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "testDir", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri newFileUri("");
            result = g_fah->CreateFile(newDirUriTest, "external_file_access_GetFileInfoFromUri_0002.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            FileInfo fileinfo;
            result = g_fah->GetFileInfoFromUri(newFileUri, fileinfo);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            int64_t offset = 0;
            FileFilter filter;
            result = g_fah->ListFile(fileinfo, offset, filter, memInfo);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(memInfo.Size(), OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
        FileAccessFwk::SharedMemoryOperation::DestroySharedMemory(memInfo);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_GetFileInfoFromUri_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_GetFileInfoFromUri_0002";
}

/**
 * @tc.number: user_file_service_external_file_access_GetFileInfoFromUri_0003
 * @tc.name: external_file_access_GetFileInfoFromUri_0003
 * @tc.desc: Test function of GetFileInfoFromUri interface.
 * @tc.desc: convert the root directory uri to fileinfo for CheckUri failed.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_GetFileInfoFromUri_0003, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_GetFileInfoFromUri_0003";
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
        GTEST_LOG_(ERROR) << "external_file_access_GetFileInfoFromUri_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_GetFileInfoFromUri_0003";
}

/**
 * @tc.number: user_file_service_external_file_access_GetFileInfoFromUri_0004
 * @tc.name: external_file_access_GetFileInfoFromUri_0004
 * @tc.desc: Test function of GetFileInfoFromUri interface.
 * @tc.desc: convert the root directory uri to fileinfo failed because of GetProxyByUri failed.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_GetFileInfoFromUri_0004, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_GetFileInfoFromUri_0004";
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
        GTEST_LOG_(ERROR) << "external_file_access_GetFileInfoFromUri_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_GetFileInfoFromUri_0004";
}

/**
 * @tc.number: user_file_service_external_file_access_GetFileInfoFromUri_0005
 * @tc.name: external_file_access_GetFileInfoFromUri_0005
 * @tc.desc: Test function of GetFileInfoFromUri interface.
 * @tc.desc: convert the invalid uri to fileinfo failed.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_GetFileInfoFromUri_0005, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_GetFileInfoFromUri_0005";
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
        GTEST_LOG_(ERROR) << "external_file_access_GetFileInfoFromUri_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_GetFileInfoFromUri_0005";
}

/**
 * @tc.number: user_file_service_external_file_access_GetFileInfoFromUri_0006
 * @tc.name: external_file_access_GetFileInfoFromUri_0006
 * @tc.desc: Test function of GetFileInfoFromUri interface.
 * @tc.desc: convert the general directory uri to fileinfo and call listfile for SUCCESS, the folder name is chinese.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I70SX9
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_GetFileInfoFromUri_0006, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_GetFileInfoFromUri_0006";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        SharedMemoryInfo memInfo;
        result = FileAccessFwk::SharedMemoryOperation::CreateSharedMemory("FileInfo List", DEFAULT_CAPACITY_200KB,
            memInfo);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "测试目录", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newDirUriTest.ToString().find("测试目录"), std::string::npos);

            FileInfo dirInfo;
            result = g_fah->GetFileInfoFromUri(newDirUriTest, dirInfo);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            int64_t offset = 0;
            FileFilter filter;
            result = g_fah->ListFile(dirInfo, offset, filter, memInfo);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_GE(memInfo.Size(), OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
        FileAccessFwk::SharedMemoryOperation::DestroySharedMemory(memInfo);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_GetFileInfoFromUri_0006 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_GetFileInfoFromUri_0006";
}

/**
 * @tc.number: user_file_service_external_file_access_GetFileInfoFromUri_0007
 * @tc.name: external_file_access_GetFileInfoFromUri_0007
 * @tc.desc: Test function of GetFileInfoFromUri interface.
 * @tc.desc: convert the general filepath uri to fileinfo and call listfile for ERROR, the file name is chinese.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I70SX9
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_GetFileInfoFromUri_0007, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_GetFileInfoFromUri_0007";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        SharedMemoryInfo memInfo;
        result = FileAccessFwk::SharedMemoryOperation::CreateSharedMemory("FileInfo List", DEFAULT_CAPACITY_200KB,
            memInfo);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "测试目录", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newDirUriTest.ToString().find("测试目录"), std::string::npos);
            Uri newFileUri("");
            result = g_fah->CreateFile(newDirUriTest, "测试文件.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newFileUri.ToString().find("测试文件.txt"), std::string::npos);

            FileInfo fileinfo;
            result = g_fah->GetFileInfoFromUri(newFileUri, fileinfo);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            int64_t offset = 0;
            FileFilter filter;
            std::vector<FileInfo> fileInfoVecTemp;
            result = g_fah->ListFile(fileinfo, offset, filter, memInfo);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(memInfo.Size(), OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
        FileAccessFwk::SharedMemoryOperation::DestroySharedMemory(memInfo);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_GetFileInfoFromUri_0007 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_GetFileInfoFromUri_0007";
}

/**
 * @tc.number: user_file_service_external_file_access_Mkdir_0000
 * @tc.name: external_file_access_Mkdir_0000
 * @tc.desc: Test function of Mkdir interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Mkdir_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Mkdir_0000";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "external_file_access_Mkdir_0000", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Mkdir_0000 result:" << result;
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Mkdir_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Mkdir_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_Mkdir_0001
 * @tc.name: external_file_access_Mkdir_0001
 * @tc.desc: Test function of Mkdir interface for ERROR which parentUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Mkdir_0001, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Mkdir_0001";
    try {
        Uri newDirUriTest("");
        Uri parentUri("");
        int result = g_fah->Mkdir(parentUri, "external_file_access_Mkdir_0001", newDirUriTest);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Mkdir_0001 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Mkdir_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Mkdir_0001";
}

/**
 * @tc.number: user_file_service_external_file_access_Mkdir_0002
 * @tc.name: external_file_access_Mkdir_0002
 * @tc.desc: Test function of Mkdir interface for ERROR which parentUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Mkdir_0002, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Mkdir_0002";
    try {
        Uri newDirUriTest("");
        Uri parentUri("storage/media/100/local/files/Download");
        int result = g_fah->Mkdir(parentUri, "external_file_access_Mkdir_0002", newDirUriTest);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Mkdir_0002 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Mkdir_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Mkdir_0002";
}

/**
 * @tc.number: user_file_service_external_file_access_Mkdir_0003
 * @tc.name: external_file_access_Mkdir_0003
 * @tc.desc: Test function of Mkdir interface for ERROR which parentUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Mkdir_0003, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Mkdir_0002";
    try {
        Uri newDirUriTest("");
        Uri parentUri("~!@#$%^&*()_");
        int result = g_fah->Mkdir(parentUri, "external_file_access_Mkdir_0003", newDirUriTest);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Mkdir_0003 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Mkdir_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Mkdir_0003";
}

/**
 * @tc.number: user_file_service_external_file_access_Mkdir_0004
 * @tc.name: external_file_access_Mkdir_0004
 * @tc.desc: Test function of Mkdir interface for ERROR which displayName is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Mkdir_0004, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Mkdir_0004";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            string displayName = "";
            result = g_fah->Mkdir(parentUri, displayName, newDirUriTest);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Mkdir_0004 result:" << result;
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Mkdir_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Mkdir_0004";
}

static void MkdirTdd(shared_ptr<FileAccessHelper> fahs, Uri uri, std::string displayName, Uri newFile)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_MkdirTdd";
    int ret = fahs->Mkdir(uri, displayName, newFile);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(ERROR) << "MkdirTdd get result error, code:" << ret;
        return;
    }
    EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_NE(newFile.ToString(), "");
    g_num++;
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_MkdirTdd";
}

/**
 * @tc.number: user_file_service_external_file_access_Mkdir_0005
 * @tc.name: external_file_access_Mkdir_0005
 * @tc.desc: Test function of Mkdir interface for SUCCESS which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Mkdir_0005, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Mkdir_0005";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            std::string displayName = "test1";
            g_num = 0;
            for (int j = 0; j < INIT_THREADS_NUMBER; j++) {
                std::thread execthread(MkdirTdd, g_fah, parentUri, displayName, newDirUriTest);
                execthread.join();
            }
            EXPECT_EQ(g_num, ACTUAL_SUCCESS_THREADS_NUMBER);
            Uri newDelete(info[i].uri + "/" + displayName);
            result = g_fah->Delete(newDelete);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Mkdir_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Mkdir_0005";
}

/**
 * @tc.number: user_file_service_external_file_access_Mkdir_0006
 * @tc.name: external_file_access_Mkdir_0006
 * @tc.desc: Test function of Mkdir interface for ERROR because of GetProxyByUri failed.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Mkdir_0006, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Mkdir_0006";
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
            Uri newDirUriTest("");
            string displayName = "";
            result = g_fah->Mkdir(parentUri, displayName, newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::E_IPCS);
            GTEST_LOG_(INFO) << "Mkdir_0006 result:" << result;
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Mkdir_0006 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Mkdir_0006";
}

/**
 * @tc.number: user_file_service_external_file_access_Mkdir_0007
 * @tc.name: external_file_access_Mkdir_0007
 * @tc.desc: Test function of Mkdir interface for SUCCESS，the folder name is chinese.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I70SX9
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Mkdir_0007, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Mkdir_0007";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "新建目录", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newDirUriTest.ToString().find("新建目录"), std::string::npos);
            GTEST_LOG_(INFO) << "Mkdir_0007 result:" << result;
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Mkdir_0007 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Mkdir_0007";
}

/**
 * @tc.number: user_file_service_external_file_access_Mkdir_0008
 * @tc.name: external_file_access_Mkdir_0008
 * @tc.desc: Test function of Mkdir interface for FAIL，the folder name is Documents.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7MQMD
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Mkdir_0008, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Mkdir_0008";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_GE(info.size(), 2);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest("");
        result = g_fah->Mkdir(parentUri, "Documents", newDirUriTest);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(parentUri, "Download", newDirUriTest);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(parentUri, "Desktop", newDirUriTest);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Mkdir_0008 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Mkdir_0008";
}

/**
 * @tc.number: user_file_service_external_file_access_GetConnectInfo_0000
 * @tc.name: external_file_access_GetConnectInfo_0000
 * @tc.desc: Test function of GetConnectInfo interface.
 * @tc.desc: helper invoke GetConnectInfo for FAILED.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_GetConnectInfo_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_GetConnectInfo_0000";
    try {
        std::shared_ptr<ConnectInfo> connectInfo = g_fah->GetConnectInfo("NotExistBundleName");
        ASSERT_TRUE(connectInfo == nullptr);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_GetConnectInfo_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_GetConnectInfo_0000";
}


/**
 * @tc.number: user_file_service_external_file_access_GetProxyByUri_0000
 * @tc.name: external_file_access_GetProxyByUri_0000
 * @tc.desc: Test function of GetProxyByUri interface.
 * @tc.desc: helper invoke GetProxyByUri for FAILED.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_GetProxyByUri_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_GetProxyByUri_0000";
    try {
        Uri uri("");
        sptr<IFileAccessExtBase> proxy = g_fah->GetProxyByUri(uri);
        ASSERT_TRUE(proxy == nullptr);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_GetProxyByUri_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_GetProxyByUri_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_GetProxyByUri_0001
 * @tc.name: external_file_access_GetProxyByUri_0001
 * @tc.desc: Test function of GetProxyByUri interface.
 * @tc.desc: helper invoke GetProxyByUri for FAILED.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_GetProxyByUri_0001, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_GetProxyByUri_0001";
    try {
        Uri uri("file://com.ohos.UserFile.NotExistBundleName/data/storage/el1/bundle/storage_daemon");
        sptr<IFileAccessExtBase> proxy = g_fah->GetProxyByUri(uri);
        ASSERT_TRUE(proxy == nullptr);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_GetProxyByUri_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_GetProxyByUri_0001";
}

static void CheckJson(Uri& newDirUriTest, const std::string displayName, const std::string relativePath,
    int len, shared_ptr<FileAccessHelper> g_fah)
{
    json testJson = {
        {RELATIVE_PATH, " "},
        {DISPLAY_NAME, " "},
        {FILE_SIZE, " "},
        {DATE_MODIFIED, " "},
        {DATE_ADDED, " "},
        {HEIGHT, " "},
        {WIDTH, " "},
        {DURATION, " "}
    };
    auto testJsonString = testJson.dump();
    int result = g_fah->Query(newDirUriTest, testJsonString);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    auto jsonObject = json::parse(testJsonString);
    EXPECT_EQ(jsonObject.at(DISPLAY_NAME), displayName);
    EXPECT_EQ(jsonObject.at(FILE_SIZE), len);
    EXPECT_EQ(jsonObject.at(RELATIVE_PATH), relativePath);
    ASSERT_TRUE(jsonObject.at(DATE_MODIFIED) > 0);
    ASSERT_TRUE(jsonObject.at(DATE_ADDED) > 0);
    GTEST_LOG_(INFO) << " result" << testJsonString;
}

/**
 * @tc.number: user_file_service_external_file_access_Query_0000
 * @tc.name: external_file_access_Query_0000
 * @tc.desc: Test function of Query directory for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6S4VV
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Query_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Query_0000";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        GTEST_LOG_(INFO) << parentUri.ToString();
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        std::string displayName = "QueryTest1";
        std::string relativePath = "/storage/Users/currentUser/QueryTest1";
        result = g_fah->Mkdir(parentUri, "QueryTest1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(newDirUriTest1, "QueryTest2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newFileUri1("");
        Uri newFileUri2("");
        std::string fileName = "external_file_access_Query_00001.txt";
        result = g_fah->CreateFile(newDirUriTest1, fileName, newFileUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->CreateFile(newDirUriTest2, fileName, newFileUri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int fd = -1;
        std::string buff = "query test";
        result = g_fah->OpenFile(newFileUri1, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        ssize_t fileSize = write(fd, buff.c_str(), buff.size());
        close(fd);
        EXPECT_EQ(fileSize, buff.size());
        result = g_fah->OpenFile(newFileUri2, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        fileSize = write(fd, buff.c_str(), buff.size());
        close(fd);
        EXPECT_EQ(fileSize, buff.size());
        int len = buff.size() + buff.size();
        CheckJson(newDirUriTest1, displayName, relativePath, len, g_fah);
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Query_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Query_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_Query_0001
 * @tc.name: external_file_access_Query_0001
 * @tc.desc: Test function of Query file for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6S4VV
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Query_0001, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Query_0001";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        GTEST_LOG_(INFO) << parentUri.ToString();
        Uri newDirUriTest("");
        result = g_fah->Mkdir(parentUri, "QueryTest3", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newFileUri("");
        std::string displayName = "external_file_access_Query_0001.txt";
        std::string relativePath = "/storage/Users/currentUser/QueryTest3/external_file_access_Query_0001.txt";
        result = g_fah->CreateFile(newDirUriTest, displayName, newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int fd = -1;
        result = g_fah->OpenFile(newFileUri, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string buff = "query test";
        ssize_t fileSize = write(fd, buff.c_str(), buff.size());
        close(fd);
        EXPECT_EQ(fileSize, buff.size());
        CheckJson(newFileUri, displayName, relativePath, buff.size(), g_fah);
        result = g_fah->Delete(newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Query_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Query_0001";
}

/**
 * @tc.number: user_file_service_external_file_access_Query_0002
 * @tc.name: external_file_access_Query_0002
 * @tc.desc: Test function of Query directory for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6S4VV
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Query_0002, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Query_0002";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        GTEST_LOG_(INFO) << parentUri.ToString();
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        result = g_fah->Mkdir(parentUri, "QueryTest4", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(newDirUriTest1, "QueryTest5", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newFileUri1("");
        Uri newFileUri2("");
        std::string fileName = "external_file_access_Query_00001.txt";
        result = g_fah->CreateFile(newDirUriTest1, fileName, newFileUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->CreateFile(newDirUriTest2, fileName, newFileUri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int fd = -1;
        std::string buff = "query test";
        result = g_fah->OpenFile(newFileUri1, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        ssize_t fileSize = write(fd, buff.c_str(), buff.size());
        close(fd);
        EXPECT_EQ(fileSize, buff.size());
        result = g_fah->OpenFile(newFileUri2, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        fileSize = write(fd, buff.c_str(), buff.size());
        close(fd);
        EXPECT_EQ(fileSize, buff.size());
        json testJson = {
            {FILE_SIZE, " "}
        };
        auto testJsonString = testJson.dump();
        result = g_fah->Query(newDirUriTest1, testJsonString);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        auto jsonObject = json::parse(testJsonString);
        EXPECT_EQ(jsonObject.at(FILE_SIZE), buff.size() * 2);
        GTEST_LOG_(INFO) << " result" << testJsonString;
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Query_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Query_0002";
}

/**
 * @tc.number: user_file_service_external_file_access_Query_0003
 * @tc.name: external_file_access_Query_0003
 * @tc.desc: Test function of Query interface for which is unreadable code.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6S4VV
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Query_0003, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Query_0003";
    try {
        Uri testUri("&*()*/?");
        json testJson = {
            {RELATIVE_PATH, " "},
            {DISPLAY_NAME, " "}
        };
        auto testJsonString = testJson.dump();
        int result = g_fah->Query(testUri, testJsonString);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << " result" << testJsonString;
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Query_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Query_0003";
}

/**
 * @tc.number: user_file_service_external_file_access_Query_0004
 * @tc.name: external_file_access_Query_0004
 * @tc.desc: Test function of Query interface for which all column nonexistence.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6S4VV
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Query_0004, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Query_0004";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        GTEST_LOG_(INFO) << parentUri.ToString();
        Uri newDirUriTest("");
        result = g_fah->Mkdir(parentUri, "QueryTest6", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        json testJson = {
            {"001", " "},
            {"#", " "},
            {"test", " "},
            {"target", " "}
        };
        auto testJsonString = testJson.dump();
        result = g_fah->Query(newDirUriTest, testJsonString);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << " result" << testJsonString;
        result = g_fah->Delete(newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Query_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Query_0004";
}

/**
 * @tc.number: user_file_service_external_file_access_Query_0005
 * @tc.name: external_file_access_Query_0005
 * @tc.desc: Test function of Query interface for which part of column nonexistence.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6S4VV
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Query_0005, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Query_0005";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        GTEST_LOG_(INFO) << parentUri.ToString();
        Uri newDirUriTest("");
        result = g_fah->Mkdir(parentUri, "QueryTest7", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        json testJson = {
            {RELATIVE_PATH, " "},
            {DISPLAY_NAME, " "},
            {"test", " "}
        };
        auto testJsonString = testJson.dump();
        result = g_fah->Query(newDirUriTest, testJsonString);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << " result" << testJsonString;
        result = g_fah->Delete(newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Query_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Query_0005";
}

/**
 * @tc.number: user_file_service_external_file_access_Query_0006
 * @tc.name: external_file_access_Query_0006
 * @tc.desc: Test function of Query interface for which column is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6S4VV
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Query_0006, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Query_0006";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        GTEST_LOG_(INFO) << parentUri.ToString();
        Uri newDirUriTest("");
        result = g_fah->Mkdir(parentUri, "QueryTest8", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        json testJson;
        auto testJsonString = testJson.dump();
        result = g_fah->Query(newDirUriTest, testJsonString);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << " result" << testJsonString;
        result = g_fah->Delete(newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Query_0006 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Query_0006";
}

/**
 * @tc.number: user_file_service_external_file_access_Query_0007
 * @tc.name: external_file_access_Query_0007
 * @tc.desc: Test function of Query directory for SUCCESS, the folder and file name is chinese.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I70SX9
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Query_0007, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Query_0007";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        GTEST_LOG_(INFO) << parentUri.ToString();
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        std::string displayName = "查询目录1";
        std::string relativePath = "/storage/Users/currentUser/查询目录1";
        result = g_fah->Mkdir(parentUri, displayName, newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(newDirUriTest1.ToString().find(displayName), std::string::npos);
        result = g_fah->Mkdir(newDirUriTest1, "查询目录2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(newDirUriTest2.ToString().find("查询目录2"), std::string::npos);
        Uri newFileUri1("");
        Uri newFileUri2("");
        std::string fileName = "查询文件.txt";
        result = g_fah->CreateFile(newDirUriTest1, fileName, newFileUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(newFileUri1.ToString().find(fileName), std::string::npos);
        result = g_fah->CreateFile(newDirUriTest2, fileName, newFileUri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(newFileUri2.ToString().find(fileName), std::string::npos);
        int fd = -1;
        std::string buff = "query test";
        result = g_fah->OpenFile(newFileUri1, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        ssize_t fileSize = write(fd, buff.c_str(), buff.size());
        close(fd);
        EXPECT_EQ(fileSize, buff.size());
        result = g_fah->OpenFile(newFileUri2, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        fileSize = write(fd, buff.c_str(), buff.size());
        close(fd);
        EXPECT_EQ(fileSize, buff.size());
        int len = buff.size() + buff.size();
        CheckJson(newDirUriTest1, displayName, relativePath, len, g_fah);
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Query_0007 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Query_0007";
}

/**
 * @tc.number: user_file_service_external_file_access_GetBundleNameFromPath_0000
 * @tc.name: external_file_access_GetBundleNameFromPath_0000
 * @tc.desc: Test function of GetBundleNameFromPath interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_GetBundleNameFromPath_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_GetBundleNameFromPath_0000";
    try {
        Uri uri("file://media/some/path");
        auto result = g_fah->GetProxyByUri(uri);
        EXPECT_EQ(result, nullptr);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_GetBundleNameFromPath_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_GetBundleNameFromPath_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_Move_item_0000
 * @tc.name: external_file_access_Move_item_0000
 * @tc.desc: Test function of Move item interface.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_item_0000, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_item_0000";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(parentUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        result = g_fah->CreateFile(newDirUriTest1, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::vector<Result> moveResult;
        result = g_fah->MoveItem(testUri, newDirUriTest2, moveResult, false);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_item_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_item_0000";
}

/**
* @tc.number: user_file_service_external_file_access_Move_item_0001
* @tc.name: external_file_access_Move_item_0001
* @tc.desc: Test function of Move item interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_item_0001, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_item_0001";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(parentUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri("");
        result = g_fah->CreateFile(newDirUriTest1, "test.txt", sourceUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri targetUri("");
        result = g_fah->CreateFile(newDirUriTest2, "test.txt", targetUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::vector<Result> moveResult;
        result = g_fah->MoveItem(sourceUri, newDirUriTest2, moveResult, false);
        EXPECT_EQ(result, -2);
        EXPECT_EQ(moveResult[0].errCode, OHOS::FileAccessFwk::ERR_EXIST);
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_item_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_item_0001";
}

/**
* @tc.number: user_file_service_external_file_access_Move_item_0002
* @tc.name: external_file_access_Move_item_0002
* @tc.desc: Test function of Move item interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_item_0002, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_item_0002";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(parentUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri("");
        result = g_fah->CreateFile(newDirUriTest1, "test.txt", sourceUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri targetUri("");
        result = g_fah->CreateFile(newDirUriTest2, "test.txt", targetUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::vector<Result> moveResult;
        result = g_fah->MoveItem(sourceUri, newDirUriTest2, moveResult, true);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_item_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_item_0002";
}

/**
* @tc.number: user_file_service_external_file_access_Move_item_0003
* @tc.name: external_file_access_Move_item_0003
* @tc.desc: Test function of Move item interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_item_0003, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_item_0003";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(parentUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri("");
        result = g_fah->CreateFile(newDirUriTest1, "test.txt", sourceUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri targetUri("");
        result = g_fah->CreateFile(newDirUriTest2, "test.txt", targetUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::vector<Result> moveResult;
        result = g_fah->MoveItem(newDirUriTest1, newDirUriTest2, moveResult, false);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0000 result:" << result;
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_item_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_item_0003";
}

/**
* @tc.number: user_file_service_external_file_access_Move_item_0004
* @tc.name: external_file_access_Move_item_0004
* @tc.desc: Test function of Move item interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_item_0004, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_item_0004";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        Uri newDirUriTest3("");
        result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(parentUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(newDirUriTest2, "test1", newDirUriTest3);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri("");
        result = g_fah->CreateFile(newDirUriTest1, "test.txt", sourceUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri targetUri("");
        result = g_fah->CreateFile(newDirUriTest3, "test.txt", targetUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::vector<Result> moveResult;
        result = g_fah->MoveItem(newDirUriTest1, newDirUriTest2, moveResult, false);
        EXPECT_EQ(result, -2);
        EXPECT_EQ(moveResult[0].errCode, OHOS::FileAccessFwk::ERR_EXIST);
        GTEST_LOG_(INFO) << "Move_0000 result:" << result;
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_item_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_item_0004";
}

/**
* @tc.number: user_file_service_external_file_access_Move_item_0005
* @tc.name: external_file_access_Move_item_0005
* @tc.desc: Test function of Move item interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_item_0005, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_item_0005";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        Uri newDirUriTest3("");
        result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(parentUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(newDirUriTest2, "test1", newDirUriTest3);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri("");
        result = g_fah->CreateFile(newDirUriTest1, "test.txt", sourceUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri targetUri("");
        result = g_fah->CreateFile(newDirUriTest3, "test.txt", targetUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::vector<Result> moveResult;
        result = g_fah->MoveItem(newDirUriTest1, newDirUriTest2, moveResult, true);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0000 result:" << result;
        result = g_fah->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_item_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_item_0005";
}

/**
* @tc.number: user_file_service_external_file_access_Move_item_0006
* @tc.name: external_file_access_Move_item_0006
* @tc.desc: Test function of Move item interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_item_0006, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_item_0006";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newDirUriTest2("");
        result = g_fah->Mkdir(newDirUriTest1, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri1("");
        result = g_fah->CreateFile(newDirUriTest2, "test.txt", sourceUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri2("");
        result = g_fah->CreateFile(newDirUriTest2, "test1.txt", sourceUri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newDirUriTest3("");
        result = g_fah->Mkdir(parentUri, "test3", newDirUriTest3);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newDirUriTest4("");
        result = g_fah->Mkdir(newDirUriTest3, "test1", newDirUriTest4);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newDirUriTest5("");
        result = g_fah->Mkdir(newDirUriTest4, "test2", newDirUriTest5);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri targetUri1("");
        result = g_fah->CreateFile(newDirUriTest5, "test.txt", targetUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::vector<Result> moveResult;
        result = g_fah->MoveItem(newDirUriTest1, newDirUriTest3, moveResult, false);
        EXPECT_EQ(result, -2);
        EXPECT_EQ(moveResult[0].errCode, OHOS::FileAccessFwk::ERR_EXIST);
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest3);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_item_0006 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_item_0006";
}

/**
* @tc.number: user_file_service_external_file_access_Move_item_0007
* @tc.name: external_file_access_Move_item_0007
* @tc.desc: Test function of Move item interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_item_0007, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_item_0007";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newDirUriTest2("");
        result = g_fah->Mkdir(newDirUriTest1, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri1("");
        result = g_fah->CreateFile(newDirUriTest2, "test.txt", sourceUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri2("");
        result = g_fah->CreateFile(newDirUriTest2, "test1.txt", sourceUri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newDirUriTest3("");
        result = g_fah->Mkdir(parentUri, "test3", newDirUriTest3);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newDirUriTest4("");
        result = g_fah->Mkdir(newDirUriTest3, "test1", newDirUriTest4);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newDirUriTest5("");
        result = g_fah->Mkdir(newDirUriTest4, "test2", newDirUriTest5);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri targetUri1("");
        result = g_fah->CreateFile(newDirUriTest5, "test.txt", targetUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::vector<Result> moveResult;
        result = g_fah->MoveItem(newDirUriTest1, newDirUriTest3, moveResult, true);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(moveResult.size(), 0);
        result = g_fah->Delete(newDirUriTest3);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_item_0007 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_item_0007";
}

/**
* @tc.number: user_file_service_external_file_access_Move_item_0008
* @tc.name: external_file_access_Move_item_0008
* @tc.desc: Test function of Move item interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_item_0008, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_item_0008";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri("");
        result = g_fah->CreateFile(newDirUriTest1, "test", sourceUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri newDirUriTest2("");
        result = g_fah->Mkdir(parentUri, "test3", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newDirUriTest3("");
        result = g_fah->Mkdir(newDirUriTest2, "test", newDirUriTest3);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::vector<Result> moveResult;
        result = g_fah->MoveItem(sourceUri, newDirUriTest2, moveResult, true);
        EXPECT_EQ(result, -2);
        EXPECT_EQ(moveResult.size(), 1);
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_item_0008 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_item_0008";
}

/**
* @tc.number: user_file_service_external_file_access_Move_item_0009
* @tc.name: external_file_access_Move_item_0009
* @tc.desc: Test function of Move item interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_item_0009, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_item_0009";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri("");
        result = g_fah->Mkdir(newDirUriTest1, "test", sourceUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri newDirUriTest2("");
        result = g_fah->Mkdir(parentUri, "test3", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri targetUri("");
        result = g_fah->CreateFile(newDirUriTest2, "test", targetUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::vector<Result> moveResult;
        result = g_fah->MoveItem(sourceUri, newDirUriTest2, moveResult, true);
        EXPECT_EQ(result, -2);
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_item_0009 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_item_0009";
}

/**
* @tc.number: user_file_service_external_file_access_Move_item_0010
* @tc.name: external_file_access_Move_0007
* @tc.desc: Test function of Move item interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_item_0010, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_item_0010";
    try {
        std::vector<Result> moveResult;
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri(newDirUriTest1.ToString() + "/" + "test");

        Uri newDirUriTest2("");
        result = g_fah->Mkdir(parentUri, "test3", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->MoveItem(sourceUri, newDirUriTest2, moveResult, true);
        EXPECT_EQ(result, -1);
        EXPECT_EQ(moveResult[0].errCode, OHOS::FileAccessFwk::ERR_URI);
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_item_0010 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_item_0010";
}

/**
* @tc.number: user_file_service_external_file_access_Move_item_0011
* @tc.name: external_file_access_Move_item_0011
* @tc.desc: Test function of Move item interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_item_0011, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_item_0011";
    try {
        std::vector<Result> moveResult;
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceUri("");
        result = g_fah->CreateFile(newDirUriTest1, "test.txt", sourceUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri newDirUriTest2("");
        result = g_fah->Mkdir(parentUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri targetUri(newDirUriTest2.ToString() + "/" + "test3");
        result = g_fah->MoveItem(sourceUri, targetUri, moveResult, true);
        EXPECT_EQ(result, -1);
        EXPECT_EQ(moveResult[0].errCode, OHOS::FileAccessFwk::ERR_URI);
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_item_0011 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_item_0011";
}

/**
* @tc.number: user_file_service_external_file_access_Move_item_0012
* @tc.name: external_file_access_Move_item_0012
* @tc.desc: Test function of Move item interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_item_0012, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_item_0012";
    try {
        std::vector<Result> moveResult;
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri uri1("");
        result = g_fah->CreateFile(newDirUriTest1, "test.txt", uri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri uri2("");
        result = g_fah->CreateFile(newDirUriTest1, "test4", uri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri newDirUriTest2("");
        result = g_fah->Mkdir(parentUri, "test3", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newDirUriTest3("");
        result = g_fah->Mkdir(newDirUriTest2, "test1", newDirUriTest3);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newDirUriTest4("");
        result = g_fah->Mkdir(newDirUriTest3, "test4", newDirUriTest4);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newDirUriTest5("");
        result = g_fah->CreateFile(newDirUriTest3, "test.txt", newDirUriTest5);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = g_fah->MoveItem(newDirUriTest1, newDirUriTest2, moveResult, false);
        EXPECT_EQ(result, -2);
        EXPECT_EQ(moveResult[0].errCode, OHOS::FileAccessFwk::ERR_IS_DIR);
        EXPECT_EQ(moveResult[1].errCode, OHOS::FileAccessFwk::ERR_EXIST);
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_item_0012 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_item_0012";
}

/**
* @tc.number: user_file_service_external_file_access_Move_item_0013
* @tc.name: external_file_access_Move_item_0013
* @tc.desc: Test function of Move item interface.
* @tc.size: MEDIUM
* @tc.type: FUNC
* @tc.level Level 1
*/
HWTEST_F(FileExtensionHelperTest, external_file_access_Move_item_0013, testing::ext::TestSize.Level1)
{
    shared_ptr<FileAccessHelper> g_fah = FileExtensionHelperTest::GetFileAccessHelper();
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Move_item_0013";
    try {
        std::vector<Result> moveResult;
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[0].uri);
        Uri newDirUriTest1("");
        result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri uri1("");
        result = g_fah->CreateFile(newDirUriTest1, "test.txt", uri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri uri2("");
        result = g_fah->CreateFile(newDirUriTest1, "test4", uri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri newDirUriTest2("");
        result = g_fah->Mkdir(parentUri, "test3", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newDirUriTest3("");
        result = g_fah->Mkdir(newDirUriTest2, "test1", newDirUriTest3);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newDirUriTest4("");
        result = g_fah->Mkdir(newDirUriTest3, "test4", newDirUriTest4);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newDirUriTest5("");
        result = g_fah->Mkdir(newDirUriTest3, "test.txt", newDirUriTest5);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = g_fah->MoveItem(newDirUriTest1, newDirUriTest2, moveResult, true);
        EXPECT_EQ(result, -2);
        EXPECT_EQ(moveResult[0].errCode, OHOS::FileAccessFwk::ERR_IS_DIR);
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_item_0013 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_item_0013";
}
} // namespace
