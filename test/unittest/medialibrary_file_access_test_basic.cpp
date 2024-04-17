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
const int ABILITY_ID = 5003;
const int INIT_THREADS_NUMBER = 4;
const int ACTUAL_SUCCESS_THREADS_NUMBER = 1;
int g_num = 0;
shared_ptr<FileAccessHelper> g_fah = nullptr;
Uri g_newDirUri("");
const int UID_TRANSFORM_TMP = 20000000;
const int UID_DEFAULT = 0;

void SetNativeToken()
{
    uint64_t tokenId;
    const char **perms = new const char *[2];
    perms[0] = "ohos.permission.FILE_ACCESS_MANAGER";
    perms[1] = "ohos.permission.GET_BUNDLE_INFO_PRIVILEGED";
    NativeTokenInfoParams infoInstance = {
        .aplStr = "system_core",
        .aclsNum = 0,
        .dcaps = nullptr,
        .dcapsNum = 0,
        .permsNum = 2,
        .perms = perms,
        .acls = nullptr,
    };

    infoInstance.processName = "SetUpTestCase";
    const uint64_t systemAppMask = (static_cast<uint64_t>(1) << 32);
    tokenId = GetAccessTokenId(&infoInstance);
    tokenId |= systemAppMask;
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    delete[] perms;
}

void FileAccessHelperTest::SetUpTestCase()
{
    cout << "FileAccessHelperTest code test" << endl;
    SetNativeToken();
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObj = saManager->GetSystemAbility(ABILITY_ID);
    AAFwk::Want want;
    vector<AAFwk::Want> wantVec;
    setuid(UID_TRANSFORM_TMP);
    int ret = FileAccessHelper::GetRegisteredFileAccessExtAbilityInfo(wantVec);
    EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
    bool found = false;
    for (size_t i = 0; i < wantVec.size(); i++) {
        auto element = wantVec[i].GetElement();
        if (element.GetBundleName() == "com.ohos.medialibrary.medialibrarydata" &&
            element.GetAbilityName() == "FileExtensionAbility") {
            want = wantVec[i];
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
    vector<AAFwk::Want> wants{want};
    g_fah = FileAccessHelper::Creator(remoteObj, wants);
    if (g_fah == nullptr) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_test g_fah is nullptr";
        exit(1);
    }
    setuid(UID_DEFAULT);
}

void FileAccessHelperTest::TearDownTestCase()
{
    g_fah->Release();
    g_fah = nullptr;
}

void FileAccessHelperTest::TearDown()
{
}

void FileAccessHelperTest::SetUp()
{
}

shared_ptr<FileAccessHelper> FileExtensionHelperTest::GetFileAccessHelper()
{
    return g_fah;
}

static Uri GetParentUri()
{
    vector<RootInfo> info;
    int result = g_fah->GetRoots(info);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri parentUri("");
    if (info.size() > OHOS::FileAccessFwk::ERR_OK) {
        parentUri = Uri(info[0].uri + "/file");
        GTEST_LOG_(ERROR) << parentUri.ToString();
    }
    return parentUri;
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_CreateFile_0000
 * @tc.name: medialibrary_file_access_CreateFile_0000
 * @tc.desc: Test function of CreateFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_CreateFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_CreateFile_0000";
    try {
        Uri newFileUri("");
        int result = g_fah->CreateFile(g_newDirUri, "medialibrary_file_access_CreateFile_0000.txt", newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "CreateFile_0000 result:" << result;
        result = g_fah->Delete(newFileUri);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_CreateFile_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_CreateFile_0000";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_CreateFile_0001
 * @tc.name: medialibrary_file_access_CreateFile_0001
 * @tc.desc: Test function of CreateFile interface for ERROR which parentUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_CreateFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_CreateFile_0001";
    try {
        Uri newFileUri("");
        Uri parentUri("");
        int result = g_fah->CreateFile(parentUri, "medialibrary_file_access_CreateFile_0001.txt", newFileUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "CreateFile_0001 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_CreateFile_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_CreateFile_0001";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_CreateFile_0002
 * @tc.name: medialibrary_file_access_CreateFile_0002
 * @tc.desc: Test function of CreateFile interface for ERROR which parentUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_CreateFile_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_CreateFile_0002";
    try {
        Uri newFileUri("");
        Uri parentUri("storage/media/100/local/files/Download");
        int result = g_fah->CreateFile(parentUri, "medialibrary_file_access_CreateFile_0002.txt", newFileUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "CreateFile_0002 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_CreateFile_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_CreateFile_0002";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_CreateFile_0003
 * @tc.name: medialibrary_file_access_CreateFile_0003
 * @tc.desc: Test function of CreateFile interface for ERROR which parentUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_CreateFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_CreateFile_0002";
    try {
        Uri newFileUri("");
        Uri parentUri("~!@#$%^&*()_");
        int result = g_fah->CreateFile(parentUri, "medialibrary_file_access_CreateFile_0003.txt", newFileUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "CreateFile_0003 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_CreateFile_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_CreateFile_0003";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_CreateFile_0004
 * @tc.name: medialibrary_file_access_CreateFile_0004
 * @tc.desc: Test function of CreateFile interface for ERROR which displayName is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_CreateFile_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_CreateFile_0004";
    try {
        Uri newFileUri("");
        string displayName = "";
        int result = g_fah->CreateFile(g_newDirUri, displayName, newFileUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "CreateFile_0004 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_CreateFile_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_CreateFile_0004";
}

static void CreateFileTdd(shared_ptr<FileAccessHelper> fahs, Uri parent, std::string displayName, Uri newDir)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_CreateFileTdd";
    int ret = fahs->CreateFile(parent, displayName, newDir);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(ERROR) << "CreateFileTdd get result error, code:" << ret;
        return;
    }
    EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_NE(newDir.ToString(), "");
    g_num++;
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_CreateFileTdd";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_CreateFile_0005
 * @tc.name: medialibrary_file_access_CreateFile_0005
 * @tc.desc: Test function of CreateFile interface for SUCCESS while Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_CreateFile_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_CreateFile_0005";
    try {
        Uri newFileUri1("");
        Uri newFileUri2("");
        Uri newFileUri3("");
        std::string displayName1 = "test1";
        std::string displayName2 = "test2";
        std::string displayName3 = "test3.txt";
        int result = g_fah->Mkdir(g_newDirUri, displayName1, newFileUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(newFileUri1, displayName2, newFileUri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        g_num = 0;
        for (int j = 0; j < INIT_THREADS_NUMBER; j++) {
            std::thread execthread(CreateFileTdd, g_fah, newFileUri2, displayName3, newFileUri3);
            execthread.join();
        }
        EXPECT_GE(g_num, ACTUAL_SUCCESS_THREADS_NUMBER);
        GTEST_LOG_(INFO) << "g_newDirUri.ToString() =" << g_newDirUri.ToString();
        result = g_fah->Delete(newFileUri1);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_CreateFile_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_CreateFile_0005";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Mkdir_0000
 * @tc.name: medialibrary_file_access_Mkdir_0000
 * @tc.desc: Test function of Mkdir interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Mkdir_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Mkdir_0000";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "medialibrary_file_access_Mkdir_0000", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Mkdir_0000 result:" << result;
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Mkdir_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Mkdir_0000";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Mkdir_0001
 * @tc.name: medialibrary_file_access_Mkdir_0001
 * @tc.desc: Test function of Mkdir interface for ERROR which parentUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Mkdir_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Mkdir_0001";
    try {
        Uri newDirUriTest("");
        Uri parentUri("");
        int result = g_fah->Mkdir(parentUri, "medialibrary_file_access_Mkdir_0001", newDirUriTest);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Mkdir_0001 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Mkdir_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Mkdir_0001";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Mkdir_0002
 * @tc.name: medialibrary_file_access_Mkdir_0002
 * @tc.desc: Test function of Mkdir interface for ERROR which parentUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Mkdir_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Mkdir_0002";
    try {
        Uri newDirUriTest("");
        Uri parentUri("storage/media/100/local/files/Download");
        int result = g_fah->Mkdir(parentUri, "medialibrary_file_access_Mkdir_0002", newDirUriTest);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Mkdir_0002 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Mkdir_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Mkdir_0002";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Mkdir_0003
 * @tc.name: medialibrary_file_access_Mkdir_0003
 * @tc.desc: Test function of Mkdir interface for ERROR which parentUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Mkdir_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Mkdir_0002";
    try {
        Uri newDirUriTest("");
        Uri parentUri("~!@#$%^&*()_");
        int result = g_fah->Mkdir(parentUri, "medialibrary_file_access_Mkdir_0003", newDirUriTest);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Mkdir_0003 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Mkdir_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Mkdir_0003";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Mkdir_0004
 * @tc.name: medialibrary_file_access_Mkdir_0004
 * @tc.desc: Test function of Mkdir interface for ERROR which displayName is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Mkdir_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Mkdir_0004";
    try {
        Uri newDirUriTest("");
        string displayName = "";
        int result = g_fah->Mkdir(g_newDirUri, displayName, newDirUriTest);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Mkdir_0004 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Mkdir_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Mkdir_0004";
}

static void MkdirTdd(shared_ptr<FileAccessHelper> fahs, Uri parent, std::string displayName, Uri newDir)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_MkdirTdd";
    int ret = fahs->Mkdir(parent, displayName, newDir);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(ERROR) << "MkdirTdd get result error, code:" << ret;
        return;
    }
    EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_NE(newDir.ToString(), "");
    g_num++;
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_MkdirTdd";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Mkdir_0005
 * @tc.name: medialibrary_file_access_Mkdir_0005
 * @tc.desc: Test function of Mkdir interface for SUCCESS which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Mkdir_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Mkdir_0005";
    try {
        Uri newFileUri1("");
        Uri newFileUri2("");
        Uri newFileUri3("");
        std::string displayName1 = "test1";
        std::string displayName2 = "test2";
        std::string displayName3 = "test3";
        int result = g_fah->Mkdir(g_newDirUri, displayName1, newFileUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(newFileUri1, displayName2, newFileUri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        g_num = 0;
        for (int j = 0; j < INIT_THREADS_NUMBER; j++) {
            std::thread execthread(MkdirTdd, g_fah, newFileUri2, displayName3, newFileUri3);
            execthread.join();
        }
        EXPECT_GE(g_num, ACTUAL_SUCCESS_THREADS_NUMBER);
        result = g_fah->Delete(newFileUri1);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Mkdir_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Mkdir_0005";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Delete_0000
 * @tc.name: medialibrary_file_access_Delete_0000
 * @tc.desc: Test function of Delete interface for SUCCESS which delete file.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Delete_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Delete_0000";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newFileUri("");
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_Delete_0000.txt", newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newFileUri);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Delete_0000 result:" << result;
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Delete_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Delete_0000";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Delete_0001
 * @tc.name: medialibrary_file_access_Delete_0001
 * @tc.desc: Test function of Delete interface for SUCCESS which delete folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Delete_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Delete_0001";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Delete_0001 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Delete_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Delete_0001";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Delete_0002
 * @tc.name: medialibrary_file_access_Delete_0002
 * @tc.desc: Test function of Delete interface for ERROR which selectFileUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Delete_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Delete_0002";
    try {
        Uri selectFileUri("");
        int result = g_fah->Delete(selectFileUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Delete_0002 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Delete_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Delete_0002";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Delete_0003
 * @tc.name: medialibrary_file_access_Delete_0003
 * @tc.desc: Test function of Delete interface for ERROR which selectFileUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Delete_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Delete_0003";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri selectFileUri("storage/media/100/local/files/Download/test");
        result = g_fah->Delete(selectFileUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Delete_0003 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Delete_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Delete_0003";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Delete_0004
 * @tc.name: medialibrary_file_access_Delete_0004
 * @tc.desc: Test function of Delete interface for ERROR which selectFileUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Delete_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Delete_0004";
    try {
        Uri selectFileUri("!@#$%^&*()");
        int result = g_fah->Delete(selectFileUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Delete_0004 result:" << result;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Delete_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Delete_0004";
}

static void DeleteTdd(shared_ptr<FileAccessHelper> fahs, Uri selectFile)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_DeleteTdd";
    int ret = fahs->Delete(selectFile);
    if (ret < OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(ERROR) << "DeleteTdd get result error, code:" << ret;
        return;
    }
    EXPECT_GE(ret, OHOS::FileAccessFwk::ERR_OK);
    g_num++;
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_DeleteTdd";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Delete_0005
 * @tc.name: medialibrary_file_access_Delete_0005
 * @tc.desc: Test function of Delete interface for SUCCESS which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Delete_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Delete_0005";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newFileUri("");
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_Delete_0005.txt", newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        std::string displayName = "test1.txt";
        Uri testUri2("");
        result = g_fah->CreateFile(newDirUriTest, displayName, testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        g_num = 0;
        for (int j = 0; j < INIT_THREADS_NUMBER; j++) {
            std::thread execthread(DeleteTdd, g_fah, testUri);
            execthread.join();
        }
        EXPECT_GE(g_num, ACTUAL_SUCCESS_THREADS_NUMBER);
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Delete_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Delete_0005";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Move_0000
 * @tc.name: medialibrary_file_access_Move_0000
 * @tc.desc: Test function of Move interface for SUCCESS which move file.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Move_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Move_0000";
    try {
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        int result = g_fah->Mkdir(g_newDirUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(g_newDirUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        result = g_fah->CreateFile(newDirUriTest1, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri2("");
        result = g_fah->Move(testUri, newDirUriTest2, testUri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0000 result:" << result;
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Move_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Move_0000";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Move_0001
 * @tc.name: medialibrary_file_access_Move_0001
 * @tc.desc: Test function of Move interface for SUCCESS which move folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Move_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Move_0001";
    try {
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        int result = g_fah->Mkdir(g_newDirUri, "test3", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(g_newDirUri, "test4", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        result = g_fah->CreateFile(newDirUriTest1, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri2("");
        result = g_fah->Move(newDirUriTest1, newDirUriTest2, testUri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0001 result:" << result;
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Move_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Move_0001";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Move_0002
 * @tc.name: medialibrary_file_access_Move_0002
 * @tc.desc: Test function of Move interface for ERROR which sourceFileUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Move_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Move_0002";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        Uri sourceFileUri("");
        result = g_fah->Move(sourceFileUri, newDirUriTest, testUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0002 result:" << result;
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Move_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Move_0002";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Move_0003
 * @tc.name: medialibrary_file_access_Move_0003
 * @tc.desc: Test function of Move interface for ERROR which sourceFileUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Move_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Move_0003";
    try {
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        int result = g_fah->Mkdir(g_newDirUri, "test5", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(g_newDirUri, "test6", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        result = g_fah->CreateFile(newDirUriTest1, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri2("");
        Uri sourceFileUri("storage/media/100/local/files/Download/test5/test.txt");
        result = g_fah->Move(sourceFileUri, newDirUriTest2, testUri2);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0003 result:" << result;
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Move_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Move_0003";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Move_0004
 * @tc.name: medialibrary_file_access_Move_0004
 * @tc.desc: Test function of Move interface for ERROR which sourceFileUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Move_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Move_0004";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        Uri sourceFileUri("~!@#$%^&*()_");
        result = g_fah->Move(sourceFileUri, newDirUriTest, testUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0004 result:" << result;
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Move_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Move_0004";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Move_0005
 * @tc.name: medialibrary_file_access_Move_0005
 * @tc.desc: Test function of Move interface for ERROR which targetParentUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Move_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Move_0005";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test1", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        result = g_fah->CreateFile(newDirUriTest, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri2("");
        Uri targetParentUri("");
        result = g_fah->Move(testUri, targetParentUri, testUri2);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0005 result:" << result;
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Move_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Move_0005";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Move_0006
 * @tc.name: medialibrary_file_access_Move_0006
 * @tc.desc: Test function of Move interface for ERROR which targetParentUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Move_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Move_0006";
    try {
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        int result = g_fah->Mkdir(g_newDirUri, "test7", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(g_newDirUri, "test8", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        result = g_fah->CreateFile(newDirUriTest1, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri2("");
        Uri targetParentUri("storage/media/100/local/files/Download/test8");
        result = g_fah->Move(testUri, targetParentUri, testUri2);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0006 result:" << result;
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Move_0006 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Move_0006";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Move_0007
 * @tc.name: medialibrary_file_access_Move_0007
 * @tc.desc: Test function of Move interface for ERROR which targetParentUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Move_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Move_0007";
    try {
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        int result = g_fah->Mkdir(g_newDirUri, "test9", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(g_newDirUri, "test10", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        result = g_fah->CreateFile(newDirUriTest1, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri2("");
        Uri targetParentUri("~!@#$^%&*()_");
        result = g_fah->Move(testUri, targetParentUri, testUri2);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0007 result:" << result;
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Move_0007 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Move_0007";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Move_0008
 * @tc.name: medialibrary_file_access_Move_0008
 * @tc.desc: Test function of Move interface for SUCCESS which move empty folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Move_0008, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Move_0008";
    try {
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        int result = g_fah->Mkdir(g_newDirUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(g_newDirUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri2("");
        result = g_fah->Move(newDirUriTest1, newDirUriTest2, testUri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0008 result:" << result;
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Move_0008 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Move_0008";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Move_0009
 * @tc.name: medialibrary_file_access_Move_0009
 * @tc.desc: Test function of Move interface for SUCCESS which move more file in folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Move_0009, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Move_0009";
    try {
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        int result = g_fah->Mkdir(g_newDirUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(g_newDirUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        size_t fileNumbers = 2000;
        for (size_t i = 0; i < fileNumbers; i++) {
            string fileName = "test" + ToString(i) + ".txt";
            result = g_fah->CreateFile(newDirUriTest1, fileName, testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
        Uri testUri2("");
        result = g_fah->Move(newDirUriTest1, newDirUriTest2, testUri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0009 result:" << result;
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Move_0009 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Move_0009";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Move_0010
 * @tc.name: medialibrary_file_access_Move_0010
 * @tc.desc: Test function of Move interface for SUCCESS which move Multilevel directory folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Move_0010, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Move_0010";
    try {
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        int result = g_fah->Mkdir(g_newDirUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(g_newDirUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        result = g_fah->Mkdir(newDirUriTest1, "test", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        size_t directoryNumbers = 50;
        for (size_t i = 0; i < directoryNumbers; i++) {
            result = g_fah->Mkdir(testUri, "test", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
        Uri testUri2("");
        result = g_fah->Move(newDirUriTest1, newDirUriTest2, testUri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0010 result:" << result;
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Move_0010 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Move_0010";
}

static void MoveTdd(shared_ptr<FileAccessHelper> fahs, Uri sourceFile, Uri targetParent, Uri newFile)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_MoveTdd";
    int ret = fahs->Move(sourceFile, targetParent, newFile);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(ERROR) << "MoveTdd get result error, code:" << ret;
        return;
    }
    EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_NE(newFile.ToString(), "");
    g_num++;
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_MoveTdd";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Move_0011
 * @tc.name: medialibrary_file_access_Move_0011
 * @tc.desc: Test function of Move interface for SUCCESS which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Move_0011, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Move_0011";
    try {
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        int result = g_fah->Mkdir(g_newDirUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(g_newDirUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri{""};
        Uri testUri2("");
        std::string displayName = "test1.txt";
        result = g_fah->CreateFile(newDirUriTest1, displayName, testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (int j = 0; j < INIT_THREADS_NUMBER; j++) {
            std::thread execthread(MoveTdd, g_fah, testUri, newDirUriTest2, testUri2);
            execthread.join();
        }
        EXPECT_GE(g_num, ACTUAL_SUCCESS_THREADS_NUMBER);
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Move_0011 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Move_0011";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Copy_0000
 * @tc.name: medialibrary_file_access_Copy_0000
 * @tc.desc: Test function of Copy interface, copy a file and argument of force is false
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6UI3H
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Copy_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Copy_0000";
    try {
        Uri srcUri("");
        int result = g_fah->Mkdir(g_newDirUri, "Copy_0000_src", srcUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri aFileUri("");
        result = g_fah->CreateFile(srcUri, "a.txt", aFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int fd;
        result = g_fah->OpenFile(aFileUri, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string aFileBuff = "Copy test content for a.txt";
        ssize_t aFileSize = write(fd, aFileBuff.c_str(), aFileBuff.size());
        close(fd);
        EXPECT_EQ(aFileSize, aFileBuff.size());

        Uri destUri("");
        result = g_fah->Mkdir(g_newDirUri, "Copy_0000_dest", destUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        vector<CopyResult> copyResult;
        result = g_fah->Copy(aFileUri, destUri, copyResult, false);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = g_fah->Delete(srcUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(destUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Copy_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Copy_0000";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Copy_0001
 * @tc.name: medialibrary_file_access_Copy_0001
 * @tc.desc: Test function of Copy interface, copy a directory and argument of force is false
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6UI3H
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Copy_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Copy_0001";
    try {
        Uri srcUri("");
        int result = g_fah->Mkdir(g_newDirUri, "Copy_0001_src", srcUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri aFileUri("");
        result = g_fah->CreateFile(srcUri, "b.txt", aFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int fd;
        result = g_fah->OpenFile(aFileUri, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string aFileBuff = "Copy test content for b.txt";
        ssize_t aFileSize = write(fd, aFileBuff.c_str(), aFileBuff.size());
        close(fd);
        EXPECT_EQ(aFileSize, aFileBuff.size());

        Uri bFileUri("");
        result = g_fah->CreateFile(srcUri, "c.txt", bFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->OpenFile(bFileUri, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string bFileBuff = "Copy test content for c.txt";
        ssize_t bFileSize = write(fd, bFileBuff.c_str(), bFileBuff.size());
        close(fd);
        EXPECT_EQ(bFileSize, bFileBuff.size());

        Uri destUri("");
        result = g_fah->Mkdir(g_newDirUri, "Copy_0001_dest", destUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        vector<CopyResult> copyResult;
        result = g_fah->Copy(srcUri, destUri, copyResult, false);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = g_fah->Delete(srcUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(destUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Copy_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Copy_0001";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Copy_0002
 * @tc.name: medialibrary_file_access_Copy_0002
 * @tc.desc: Test function of Copy interface, copy a file with the same name and argument of force is false
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6UI3H
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Copy_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Copy_0002";
    try {
        Uri srcUri("");
        int result = g_fah->Mkdir(g_newDirUri, "Copy_0002_src", srcUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri srcFileUri("");
        result = g_fah->CreateFile(srcUri, "a.txt", srcFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int fd;
        result = g_fah->OpenFile(srcFileUri, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string aFileBuff = "Copy test content for a.txt";
        ssize_t aFileSize = write(fd, aFileBuff.c_str(), aFileBuff.size());
        close(fd);
        EXPECT_EQ(aFileSize, aFileBuff.size());

        Uri destUri("");
        result = g_fah->Mkdir(g_newDirUri, "Copy_0002_dest_false", destUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri destFileUri("");
        result = g_fah->CreateFile(destUri, "a.txt", destFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        vector<CopyResult> copyResult;
        result = g_fah->Copy(srcFileUri, destUri, copyResult, false);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_GT(copyResult.size(), 0);

        result = g_fah->Delete(srcUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(destUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Copy_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Copy_0002";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Copy_0003
 * @tc.name: medialibrary_file_access_Copy_0003
 * @tc.desc: Test function of Copy interface, copy a file with the same name and argument of force is true
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6UI3H
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Copy_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Copy_0003";
    try {
        Uri srcUri("");
        int result = g_fah->Mkdir(g_newDirUri, "Copy_0003_src", srcUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri srcFileUri("");
        result = g_fah->CreateFile(srcUri, "d.txt", srcFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int fd;
        result = g_fah->OpenFile(srcFileUri, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string aFileBuff = "Copy test content for d.txt";
        ssize_t aFileSize = write(fd, aFileBuff.c_str(), aFileBuff.size());
        close(fd);
        EXPECT_EQ(aFileSize, aFileBuff.size());

        Uri destUri("");
        result = g_fah->Mkdir(g_newDirUri, "Copy_0003_dest_true", destUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri destFileUri("");
        result = g_fah->CreateFile(destUri, "d.txt", destFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        vector<CopyResult> copyResult;
        result = g_fah->Copy(srcFileUri, destUri, copyResult, true);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = g_fah->Delete(srcUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(destUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Copy_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Copy_0003";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Copy_0004
 * @tc.name: medialibrary_file_access_Copy_0004
 * @tc.desc: Test function of Copy interface, copy a directory with the same name and argument of force is false
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6UI3H
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Copy_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Copy_0004";
    try {
        Uri srcUri("");
        int result = g_fah->Mkdir(g_newDirUri, "Copy_0004_src", srcUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri aFileUri("");
        result = g_fah->CreateFile(srcUri, "e.txt", aFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int fd;
        result = g_fah->OpenFile(aFileUri, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string aFileBuff = "Copy test content for e.txt";
        ssize_t aFileSize = write(fd, aFileBuff.c_str(), aFileBuff.size());
        close(fd);
        EXPECT_EQ(aFileSize, aFileBuff.size());

        Uri bFileUri("");
        result = g_fah->CreateFile(srcUri, "f.txt", bFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->OpenFile(bFileUri, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string bFileBuff = "Copy test content for f.txt";
        ssize_t bFileSize = write(fd, bFileBuff.c_str(), bFileBuff.size());
        close(fd);
        EXPECT_EQ(bFileSize, bFileBuff.size());

        Uri destUri("");
        result = g_fah->Mkdir(g_newDirUri, "Copy_0004_dest_false", destUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri destSrcUri("");
        result = g_fah->Mkdir(destUri, "Copy_0004_src", destSrcUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri destSrcAUri("");
        result = g_fah->CreateFile(destSrcUri, "e.txt", destSrcAUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        vector<CopyResult> copyResult;
        result = g_fah->Copy(srcUri, destUri, copyResult, false);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_GT(copyResult.size(), 0);

        result = g_fah->Delete(srcUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(destUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Copy_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Copy_0004";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Copy_0005
 * @tc.name: medialibrary_file_access_Copy_0005
 * @tc.desc: Test function of Copy interface, copy a directory with the same name and argument of force is true
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6UI3H
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Copy_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Copy_0005";
    try {
        Uri srcUri("");
        int result = g_fah->Mkdir(g_newDirUri, "Copy_0005_src", srcUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri aFileUri("");
        result = g_fah->CreateFile(srcUri, "g.txt", aFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int fd;
        result = g_fah->OpenFile(aFileUri, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string aFileBuff = "Copy test content for g.txt";
        ssize_t aFileSize = write(fd, aFileBuff.c_str(), aFileBuff.size());
        close(fd);
        EXPECT_EQ(aFileSize, aFileBuff.size());

        Uri bFileUri("");
        result = g_fah->CreateFile(srcUri, "h.txt", bFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->OpenFile(bFileUri, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string bFileBuff = "Copy test content for h.txt";
        ssize_t bFileSize = write(fd, bFileBuff.c_str(), bFileBuff.size());
        close(fd);
        EXPECT_EQ(bFileSize, bFileBuff.size());

        Uri destUri("");
        result = g_fah->Mkdir(g_newDirUri, "Copy_0005_dest_true", destUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri destSrcUri("");
        result = g_fah->Mkdir(destUri, "Copy_0005_src", destSrcUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri destSrcAUri("");
        result = g_fah->CreateFile(destSrcUri, "g.txt", destSrcAUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        vector<CopyResult> copyResult;
        result = g_fah->Copy(srcUri, destUri, copyResult, true);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = g_fah->Delete(srcUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(destUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Copy_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Copy_0005";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Copy_0006
 * @tc.name: medialibrary_file_access_Copy_0006
 * @tc.desc: Test function of Copy interface, copy a file with the same name
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6UI3H
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Copy_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Copy_0006";
    try {
        Uri srcUri("");
        int result = g_fah->Mkdir(g_newDirUri, "Copy_0006_src", srcUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri srcFileUri("");
        result = g_fah->CreateFile(srcUri, "i.txt", srcFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int fd;
        result = g_fah->OpenFile(srcFileUri, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string aFileBuff = "Copy test content for i.txt";
        ssize_t aFileSize = write(fd, aFileBuff.c_str(), aFileBuff.size());
        close(fd);
        EXPECT_EQ(aFileSize, aFileBuff.size());

        Uri destUri("");
        result = g_fah->Mkdir(g_newDirUri, "Copy_0006_dest_false", destUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri destFileUri("");
        result = g_fah->CreateFile(destUri, "i.txt", destFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        vector<CopyResult> copyResult;
        result = g_fah->Copy(srcFileUri, destUri, copyResult);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_GT(copyResult.size(), 0);

        result = g_fah->Delete(srcUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(destUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Copy_0006 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Copy_0006";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_Copy_0007
 * @tc.name: medialibrary_file_access_Copy_0007
 * @tc.desc: Test function of Copy interface, copy a directory with the same name
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6UI3H
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_Copy_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Copy_0007";
    try {
        Uri srcUri("");
        int result = g_fah->Mkdir(g_newDirUri, "Copy_0007_src", srcUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri aFileUri("");
        result = g_fah->CreateFile(srcUri, "j.txt", aFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int fd;
        result = g_fah->OpenFile(aFileUri, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string aFileBuff = "Copy test content for j.txt";
        ssize_t aFileSize = write(fd, aFileBuff.c_str(), aFileBuff.size());
        close(fd);
        EXPECT_EQ(aFileSize, aFileBuff.size());

        Uri bFileUri("");
        result = g_fah->CreateFile(srcUri, "k.txt", bFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->OpenFile(bFileUri, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string bFileBuff = "Copy test content for k.txt";
        ssize_t bFileSize = write(fd, bFileBuff.c_str(), bFileBuff.size());
        close(fd);
        EXPECT_EQ(bFileSize, bFileBuff.size());

        Uri destUri("");
        result = g_fah->Mkdir(g_newDirUri, "Copy_0007_dest_false", destUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri destSrcUri("");
        result = g_fah->Mkdir(destUri, "Copy_0007_src", destSrcUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri destSrcAUri("");
        result = g_fah->CreateFile(destSrcUri, "j.txt", destSrcAUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        vector<CopyResult> copyResult;
        result = g_fah->Copy(srcUri, destUri, copyResult);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_GT(copyResult.size(), 0);

        result = g_fah->Delete(srcUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(destUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Copy_0007 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Copy_0007";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_ListFile_0000
 * @tc.name: medialibrary_file_access_ListFile_0000
 * @tc.desc: Test function of ListFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_ListFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_ListFile_0000";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_ListFile_0000.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        FileInfo fileInfo;
        fileInfo.uri = newDirUriTest.ToString();
        int64_t offset = 0;
        int64_t maxCount = 1000;
        std::vector<FileInfo> fileInfoVec;
        FileFilter filter;
        result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_GT(fileInfoVec.size(), OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "ListFile_0000 result:" << fileInfoVec.size() << endl;
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_ListFile_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_ListFile_0000";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_ListFile_0001
 * @tc.name: medialibrary_file_access_ListFile_0001
 * @tc.desc: Test function of ListFile interface for ERROR which Uri is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_ListFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_ListFile_0001";
    try {
        Uri sourceFileUri("");
        FileInfo fileInfo;
        fileInfo.uri = sourceFileUri.ToString();
        int64_t offset = 0;
        int64_t maxCount = 1000;
        vector<FileAccessFwk::FileInfo> fileInfoVec;
        FileFilter filter({}, {}, {}, -1, -1, false, false);
        int result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(fileInfoVec.size(), OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "ListFile_0001 result:" << fileInfoVec.size() << endl;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_ListFile_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_ListFile_0001";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_ListFile_0002
 * @tc.name: medialibrary_file_access_ListFile_0002
 * @tc.desc: Test function of ListFile interface for ERROR which sourceFileUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_ListFile_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_ListFile_0002";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        result = g_fah->CreateFile(newDirUriTest, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri sourceFileUri("storage/media/100/local/files/Download/test/test.txt");
        FileInfo fileInfo;
        fileInfo.uri = sourceFileUri.ToString();
        int64_t offset = 0;
        int64_t maxCount = 1000;
        vector<FileAccessFwk::FileInfo> fileInfoVec;
        FileFilter filter({}, {}, {}, -1, -1, false, false);
        result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(fileInfoVec.size(), OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "ListFile_0002 result:" << fileInfoVec.size() << endl;
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_ListFile_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_ListFile_0002";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_ListFile_0003
 * @tc.name: medialibrary_file_access_ListFile_0003
 * @tc.desc: Test function of ListFile interface for ERROR which sourceFileUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_ListFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_ListFile_0003";
    try {
        Uri sourceFileUri("~!@#$%^&*()_");
        FileInfo fileInfo;
        fileInfo.uri = sourceFileUri.ToString();
        int64_t offset = 0;
        int64_t maxCount = 1000;
        vector<FileAccessFwk::FileInfo> fileInfoVec;
        FileFilter filter;
        int result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(fileInfoVec.size(), OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "ListFile_0003 result:" << fileInfoVec.size() << endl;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_ListFile_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_ListFile_0003";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_ListFile_0004
 * @tc.name: medialibrary_file_access_ListFile_0004
 * @tc.desc: Test function of ListFile interface for ERROR which add filter.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000HB855
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_ListFile_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_ListFile_0004";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri1("");
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_ListFile_0004.txt", testUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri2("");
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_ListFile_0004.docx", testUri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        FileInfo fileInfo;
        fileInfo.uri = newDirUriTest.ToString();
        int64_t offset = 0;
        int64_t maxCount = 1000;
        std::vector<FileInfo> fileInfoVec;
        FileFilter filter({".txt"}, {}, {}, -1, 1, false, true);
        result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(fileInfoVec.size(), 1);
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_ListFile_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_ListFile_0004";
}

static void ListFileTdd(shared_ptr<FileAccessHelper> fahs, FileInfo fileInfo, FileFilter filter,
    std::vector<FileInfo> fileInfoVec)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_ListFileTdd";
    int64_t offset = 0;
    int64_t maxCount = 1000;
    int ret = fahs->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(ERROR) << "ListFileTdd get result error, code:" << ret;
        return;
    }
    EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), 1);
    g_num++;
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_ListFileTdd";
}

/**
 * @tc.number: user_file_service_medialibrary_file_access_ListFile_0005
 * @tc.name: medialibrary_file_access_ListFile_0005
 * @tc.desc: Test function of ListFile interface for ERROR which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, medialibrary_file_access_ListFile_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_ListFile_0005";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri1("");
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_ListFile_0005.txt", testUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri2("");
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_ListFile_0005.docx", testUri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        FileInfo fileInfo;
        fileInfo.uri = newDirUriTest.ToString();
        std::vector<FileInfo> fileInfoVec;
        g_num = 0;
        FileFilter filter({".txt"}, {}, {}, -1, -1, false, true);
        for (int j = 0; j < INIT_THREADS_NUMBER; j++) {
            std::thread execthread(ListFileTdd, g_fah, fileInfo, filter, fileInfoVec);
            execthread.join();
        }
        EXPECT_GE(g_num, ACTUAL_SUCCESS_THREADS_NUMBER);
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_ListFile_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_ListFile_0005";
}
} // namespace
