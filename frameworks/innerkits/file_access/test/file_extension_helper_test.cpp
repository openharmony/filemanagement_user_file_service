/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
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

#include "base/security/access_token/interfaces/innerkits/accesstoken/include/accesstoken_kit.h"
#include "base/security/access_token/interfaces/innerkits/token_setproc/include/token_setproc.h"
#include "file_access_framework_errno.h"
#include "file_access_helper.h"
#include "iservice_registry.h"

namespace {
using namespace std;
using namespace OHOS;
using namespace FileAccessFwk;
int ABILITY_ID = 5003;
shared_ptr<FileAccessHelper> g_fah = nullptr;
OHOS::Security::AccessToken::AccessTokenID g_tokenId;

// permission state
OHOS::Security::AccessToken::PermissionStateFull g_infoManagerTestState = {
    .permissionName = "ohos.permission.FILE_ACCESS_MANAGER",
    .isGeneral = true,
    .resDeviceID = {"local"},
    .grantStatus = {OHOS::Security::AccessToken::PermissionState::PERMISSION_GRANTED},
    .grantFlags = {1}
};

OHOS::Security::AccessToken::PermissionStateFull g_infoManagerTestState1 = {
    .permissionName = "ohos.permission.READ_MEDIA",
    .isGeneral = true,
    .resDeviceID = {"local"},
    .grantStatus = {OHOS::Security::AccessToken::PermissionState::PERMISSION_GRANTED},
    .grantFlags = {1}
};

OHOS::Security::AccessToken::PermissionStateFull g_infoManagerTestState2 = {
    .permissionName = "ohos.permission.WRITE_MEDIA",
    .isGeneral = true,
    .resDeviceID = {"local"},
    .grantStatus = {OHOS::Security::AccessToken::PermissionState::PERMISSION_GRANTED},
    .grantFlags = {1}
};

// permission define
OHOS::Security::AccessToken::PermissionDef g_infoManagerTestPermDef = {
    .permissionName = "ohos.permission.FILE_ACCESS_MANAGER",
    .bundleName = "ohos.acts.multimedia.mediaLibrary",
    .grantMode = 1,
    .availableLevel = OHOS::Security::AccessToken::APL_NORMAL,
    .label = "label",
    .labelId = 1,
    .description = "FILE_ACCESS_MANAGER",
    .descriptionId = 1
};

OHOS::Security::AccessToken::PermissionDef g_infoManagerTestPermDef1 = {
    .permissionName = "ohos.permission.READ_MEDIA",
    .bundleName = "ohos.acts.multimedia.mediaLibrary",
    .grantMode = 1,
    .availableLevel = OHOS::Security::AccessToken::APL_NORMAL,
    .label = "label",
    .labelId = 1,
    .description = "READ_MEDIA",
    .descriptionId = 1
};

OHOS::Security::AccessToken::PermissionDef g_infoManagerTestPermDef2 = {
    .permissionName = "ohos.permission.WRITE_MEDIA",
    .bundleName = "ohos.acts.multimedia.mediaLibrary",
    .grantMode = 1,
    .availableLevel = OHOS::Security::AccessToken::APL_NORMAL,
    .label = "label",
    .labelId = 1,
    .description = "WRITE_MEDIA",
    .descriptionId = 1
};

// permission info
OHOS::Security::AccessToken::HapPolicyParams g_infoManagerTestPolicyPrams = {
    .apl = OHOS::Security::AccessToken::APL_NORMAL,
    .domain = "test.domain",
    .permList = {g_infoManagerTestPermDef, g_infoManagerTestPermDef1, g_infoManagerTestPermDef2},
    .permStateList = {g_infoManagerTestState, g_infoManagerTestState1, g_infoManagerTestState2}
};

// hap info
OHOS::Security::AccessToken::HapInfoParams g_infoManagerTestInfoParms = {
    .userID = 1,
    .bundleName = "FileExtensionHelperTest",
    .instIndex = 0,
    .appIDDesc = "testtesttesttest"
};

class FileExtensionHelperTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        cout << "FileExtensionHelperTest code test" << endl;

        auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        auto remoteObj = saManager->GetSystemAbility(ABILITY_ID);
        AAFwk::Want want;
        vector<AAFwk::Want> wantVec;
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
        vector<AAFwk::Want> wants {want};
        g_fah = FileAccessHelper::Creator(remoteObj, wants);

        OHOS::Security::AccessToken::AccessTokenIDEx tokenIdEx = {0};
        tokenIdEx = OHOS::Security::AccessToken::AccessTokenKit::AllocHapToken
            (g_infoManagerTestInfoParms, g_infoManagerTestPolicyPrams);
        g_tokenId = tokenIdEx.tokenIdExStruct.tokenID;
        SetSelfTokenID(g_tokenId);
    }
    static void TearDownTestCase() {
        g_fah->Release();
        g_fah = nullptr;
        OHOS::Security::AccessToken::AccessTokenKit::DeleteToken(g_tokenId);
    };
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: user_file_service_file_extension_helper_OpenFile_0000
 * @tc.name: file_extension_helper_OpenFile_0000
 * @tc.desc: Test function of OpenFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_OpenFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_OpenFile_0000";
    try {

        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            GTEST_LOG_(INFO) << parentUri.ToString();

            Uri newDirUriTest1("");
            result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri newFileUri("");
            result = g_fah->CreateFile(newDirUriTest1, "file_extension_helper_OpenFile_0000.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            int fd;
            result = g_fah->OpenFile(newFileUri, 0, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            GTEST_LOG_(INFO) << "OpenFile_0000 result:" << result << endl;

            result = g_fah->Delete(newDirUriTest1);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_OpenFile_0000";
}

/**
 * @tc.number: user_file_service_file_extension_helper_OpenFile_0001
 * @tc.name: file_extension_helper_OpenFile_0001
 * @tc.desc: Test function of OpenFile interface for ERROR which Uri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_OpenFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_OpenFile_0001";
    try {
        Uri uri("");
        int fd;
        int result = g_fah->OpenFile(uri, 0, fd);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0001 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_OpenFile_0001";
}

/**
 * @tc.number: user_file_service_file_extension_helper_OpenFile_0002
 * @tc.name: file_extension_helper_OpenFile_0002
 * @tc.desc: Test function of OpenFile interface for ERROR which Uri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_OpenFile_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_OpenFile_0002";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            result = g_fah->CreateFile(parentUri, "file_extension_helper_OpenFile_0002.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri uri("storage/media/100/local/files/Download/file_extension_helper_OpenFile_0002.txt");
            int fd;
            result = g_fah->OpenFile(uri, 0, fd);
            EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "OpenFile_0002 result:" << result << endl;

            result = g_fah->Delete(newFileUri);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_OpenFile_0002";
}

/**
 * @tc.number: user_file_service_file_extension_helper_OpenFile_0003
 * @tc.name: file_extension_helper_OpenFile_0003
 * @tc.desc: Test function of OpenFile interface for ERROR which Uri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_OpenFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_OpenFile_0003";
    try {
        Uri uri("~!@#$%^&*()_");
        int fd;
        int result = g_fah->OpenFile(uri, 0, fd);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0003 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_OpenFile_0003";
}

/**
 * @tc.number: user_file_service_file_extension_helper_OpenFile_0004
 * @tc.name: file_extension_helper_OpenFile_0004
 * @tc.desc: Test function of OpenFile interface for ERROR which flag is -1.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_OpenFile_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_OpenFile_0004";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            result = g_fah->CreateFile(parentUri, "file_extension_helper_OpenFile_0004.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            int fd;
            result = g_fah->OpenFile(newFileUri, -1, fd);
            EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "OpenFile_0004 result:" << result << endl;

            result = g_fah->Delete(newFileUri);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_OpenFile_0004";
}

/**
 * @tc.number: user_file_service_file_extension_helper_OpenFile_0005
 * @tc.name: file_extension_helper_OpenFile_0005
 * @tc.desc: Test function of OpenFile interface for SUCCESS which flag is 1.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_OpenFile_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_OpenFile_0005";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            result = g_fah->CreateFile(parentUri, "file_extension_helper_OpenFile_0005.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            int fd;
            result = g_fah->OpenFile(newFileUri, 1, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "OpenFile_0005 result:" << result << endl;

            result = g_fah->Delete(newFileUri);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_OpenFile_0005";
}

/**
 * @tc.number: user_file_service_file_extension_helper_OpenFile_0006
 * @tc.name: file_extension_helper_OpenFile_0006
 * @tc.desc: Test function of OpenFile interface for SUCCESS which flag is 2.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_OpenFile_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_OpenFile_0006";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            result = g_fah->CreateFile(parentUri, "file_extension_helper_OpenFile_0006.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            int fd;
            result = g_fah->OpenFile(newFileUri, 2, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "OpenFile_0006 result:" << result << endl;

            result = g_fah->Delete(newFileUri);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_OpenFile_0006";
}

void OpenFileTdd(shared_ptr<FileAccessHelper> fahs, Uri uri, int flags, int fd)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_OpenFileTdd";
    int ret = fahs->OpenFile(uri, flags, fd);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(INFO) << "OpenFile get result error, code:" << ret;
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_OpenFileTdd";
}

/**
 * @tc.number: user_file_service_file_extension_helper_OpenFile_0007
 * @tc.name: file_extension_helper_OpenFile_0007
 * @tc.desc: Test function of OpenFile interface for SUCCESS which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_OpenFile_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_OpenFile_0007";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            result = g_fah->CreateFile(parentUri, "file_extension_helper_OpenFile_0007.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            int flags;
            int fd[4];
            for (size_t j = 0; j< 4; j++) {
                std::thread OpenFile_Thread(OpenFileTdd, g_fah, newFileUri, flags, fd[j]);
                OpenFile_Thread.detach();
                EXPECT_GE(fd[j], OHOS::FileAccessFwk::ERR_OK);
            }

            result = g_fah->Delete(newFileUri);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_OpenFile_0007";
}

/**
 * @tc.number: user_file_service_file_extension_helper_CreateFile_0000
 * @tc.name: file_extension_helper_CreateFile_0000
 * @tc.desc: Test function of CreateFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_CreateFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_CreateFile_0000";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            result = g_fah->CreateFile(parentUri, "file_extension_helper_CreateFile_0000.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "CreateFile_0000 result:" << result << endl;

            result = g_fah->Delete(newFileUri);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_CreateFile_0000";
}

/**
 * @tc.number: user_file_service_file_extension_helper_CreateFile_0001
 * @tc.name: file_extension_helper_CreateFile_0001
 * @tc.desc: Test function of CreateFile interface for ERROR which parentUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_CreateFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_CreateFile_0001";
    try {
        Uri newFileUri("");
        Uri parentUri("");
        int result = g_fah->CreateFile(parentUri, "file_extension_helper_CreateFile_0001.txt", newFileUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "CreateFile_0001 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_CreateFile_0001";
}

/**
 * @tc.number: user_file_service_file_extension_helper_CreateFile_0002
 * @tc.name: file_extension_helper_CreateFile_0002
 * @tc.desc: Test function of CreateFile interface for ERROR which parentUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_CreateFile_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_CreateFile_0002";
    try {
        Uri newFileUri("");
        Uri parentUri("storage/media/100/local/files/Download");
        int result = g_fah->CreateFile(parentUri, "file_extension_helper_CreateFile_0002.txt", newFileUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "CreateFile_0002 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_CreateFile_0002";
}

/**
 * @tc.number: user_file_service_file_extension_helper_CreateFile_0003
 * @tc.name: file_extension_helper_CreateFile_0003
 * @tc.desc: Test function of CreateFile interface for ERROR which parentUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_CreateFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_CreateFile_0002";
    try {
        Uri newFileUri("");
        Uri parentUri("~!@#$%^&*()_");
        int result = g_fah->CreateFile(parentUri, "file_extension_helper_CreateFile_0003.txt", newFileUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "CreateFile_0003 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_CreateFile_0003";
}

/**
 * @tc.number: user_file_service_file_extension_helper_CreateFile_0004
 * @tc.name: file_extension_helper_CreateFile_0004
 * @tc.desc: Test function of CreateFile interface for ERROR which displayName is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_CreateFile_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_CreateFile_0004";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            string displayName = "";
            result = g_fah->CreateFile(parentUri, displayName, newFileUri);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "CreateFile_0004 result:" << result << endl;
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_CreateFile_0004";
}

void CreateFileTdd(shared_ptr<FileAccessHelper> fahs, Uri uri, std::string displayName, Uri newFile)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_CreateFileTdd";
    int ret = fahs->CreateFile(uri, displayName, newFile);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(INFO) << "CreateFileTdd get result error, code:" << ret;
    }
    EXPECT_NE(newFile.ToString(), "");
    ret = fahs->Delete(newFile);
    EXPECT_GE(ret, OHOS::FileAccessFwk::ERR_OK);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_CreateFileTdd";
}

/**
 * @tc.number: user_file_service_file_extension_helper_CreateFile_0005
 * @tc.name: file_extension_helper_CreateFile_0005
 * @tc.desc: Test function of CreateFile interface for ERROR which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_CreateFile_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_CreateFile_0005";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            std::string displayName[4] = {"test1.txt", "test2.txt", "test3.txt", "test4.txt"};
            for (int j = 0; j < 1; j++) {
                std::thread CreateFile_Thread(CreateFileTdd, g_fah, parentUri, displayName[j], newFileUri);
                CreateFile_Thread.detach();
            }
            GTEST_LOG_(INFO) << "CreateFile_0005 result:" << result << endl;
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_CreateFile_0005";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Mkdir_0000
 * @tc.name: file_extension_helper_Mkdir_0000
 * @tc.desc: Test function of Mkdir interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Mkdir_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Mkdir_0000";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "file_extension_helper_Mkdir_0000", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Mkdir_0000 result:" << result << endl;

            result = g_fah->Delete(newDirUriTest);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Mkdir_0000";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Mkdir_0001
 * @tc.name: file_extension_helper_Mkdir_0001
 * @tc.desc: Test function of Mkdir interface for ERROR which parentUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Mkdir_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Mkdir_0001";
    try {
        Uri newDirUriTest("");
        Uri parentUri("");
        int result = g_fah->Mkdir(parentUri, "file_extension_helper_Mkdir_0001", newDirUriTest);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Mkdir_0001 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Mkdir_0001";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Mkdir_0002
 * @tc.name: file_extension_helper_Mkdir_0002
 * @tc.desc: Test function of Mkdir interface for ERROR which parentUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Mkdir_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Mkdir_0002";
    try {
        Uri newDirUriTest("");
        Uri parentUri("storage/media/100/local/files/Download");
        int result = g_fah->Mkdir(parentUri, "file_extension_helper_Mkdir_0002", newDirUriTest);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Mkdir_0002 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Mkdir_0002";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Mkdir_0003
 * @tc.name: file_extension_helper_Mkdir_0003
 * @tc.desc: Test function of Mkdir interface for ERROR which parentUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Mkdir_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Mkdir_0002";
    try {
        Uri newDirUriTest("");
        Uri parentUri("~!@#$%^&*()_");
        int result = g_fah->Mkdir(parentUri, "file_extension_helper_Mkdir_0003", newDirUriTest);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Mkdir_0003 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Mkdir_0003";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Mkdir_0004
 * @tc.name: file_extension_helper_Mkdir_0004
 * @tc.desc: Test function of Mkdir interface for ERROR which displayName is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Mkdir_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Mkdir_0004";
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
            GTEST_LOG_(INFO) << "Mkdir_0004 result:" << result << endl;
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Mkdir_0004";
}

void MkdirTdd(shared_ptr<FileAccessHelper> fahs, Uri uri, std::string displayName, Uri newFile)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_MkdirTdd";
    int ret = fahs->Mkdir(uri, displayName, newFile);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(INFO) << "MkdirTdd get result error, code:" << ret;
    }
    EXPECT_NE(newFile.ToString(), "");
    ret = fahs->Delete(newFile);
    EXPECT_GE(ret, OHOS::FileAccessFwk::ERR_OK);
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_MkdirTdd";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Mkdir_0005
 * @tc.name: file_extension_helper_Mkdir_0005
 * @tc.desc: Test function of Mkdir interface for SUCCESS which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Mkdir_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Mkdir_0005";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            std::string displayName[4] = {"test1", "test2", "test3", "test4"};
            for (int j = 0; j < 4; j++) {
                std::thread Mkdir_Thread(MkdirTdd, g_fah, parentUri, displayName[j], newDirUriTest);
                Mkdir_Thread.detach();
            }
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Mkdir_0005";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Delete_0000
 * @tc.name: file_extension_helper_Delete_0000
 * @tc.desc: Test function of Delete interface for SUCCESS which delete file.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Delete_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Delete_0000";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri newFileUri("");
            result = g_fah->CreateFile(newDirUriTest, "file_extension_helper_Delete_0000.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Delete(newFileUri);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Delete_0000 result:" << result << endl;

            result = g_fah->Delete(newDirUriTest);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Delete_0000";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Delete_0001
 * @tc.name: file_extension_helper_Delete_0001
 * @tc.desc: Test function of Delete interface for SUCCESS which delete folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Delete_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Delete_0001";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Delete(newDirUriTest);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Delete_0001 result:" << result << endl;
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Delete_0001";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Delete_0002
 * @tc.name: file_extension_helper_Delete_0002
 * @tc.desc: Test function of Delete interface for ERROR which selectFileUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Delete_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Delete_0002";
    try {
        Uri selectFileUri("");
        int result = g_fah->Delete(selectFileUri);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Delete_0002 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Delete_0002";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Delete_0003
 * @tc.name: file_extension_helper_Delete_0003
 * @tc.desc: Test function of Delete interface for ERROR which selectFileUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Delete_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Delete_0003";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri selectFileUri("storage/media/100/local/files/Download/test");
            result = g_fah->Delete(selectFileUri);
            EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Delete(newDirUriTest);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Delete_0003 result:" << result << endl;
        }

    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Delete_0003";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Delete_0004
 * @tc.name: file_extension_helper_Delete_0004
 * @tc.desc: Test function of Delete interface for ERROR which selectFileUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Delete_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Delete_0004";
    try {
        Uri selectFileUri("!@#$%^&*()");
        int result = g_fah->Delete(selectFileUri);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Delete_0004 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Delete_0004";
}

void DeleteTdd(shared_ptr<FileAccessHelper> fahs, Uri selectFile)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_DeleteTdd";
    int ret = fahs->Delete(selectFile);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(INFO) << "DeleteTdd get result error, code:" << ret;
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_DeleteTdd";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Delete_0005
 * @tc.name: file_extension_helper_Delete_0005
 * @tc.desc: Test function of Delete interface for SUCCESS which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Delete_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Delete_0005";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri newFileUri("");
            std::string displayName[4] = {"test1.txt", "test2.txt", "test3.txt", "test4.txt"};
            for (int j = 0; j < 4; j++) {
                result = g_fah->CreateFile(newDirUriTest, displayName[j], newFileUri);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
                std::thread Delete_Thread(DeleteTdd, g_fah, newFileUri);
                Delete_Thread.detach();
            }
            result = g_fah->Delete(newDirUriTest);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Delete_0005";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Move_0000
 * @tc.name: file_extension_helper_Move_0000
 * @tc.desc: Test function of Move interface for SUCCESS which move file.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Move_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Move_0000";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri("");
            result = g_fah->CreateFile(newDirUriTest1, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri2("");
            result = g_fah->Move(testUri, newDirUriTest2, testUri2);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0000 result:" << result << endl;

            result = g_fah->Delete(newDirUriTest1);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Delete(newDirUriTest2);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0000";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Move_0001
 * @tc.name: file_extension_helper_Move_0001
 * @tc.desc: Test function of Move interface for SUCCESS which move folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Move_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Move_0001";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri("");
            result = g_fah->CreateFile(newDirUriTest1, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri2("");
            result = g_fah->Move(newDirUriTest1, newDirUriTest2, testUri2);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0001 result:" << result << endl;

            result = g_fah->Delete(newDirUriTest2);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0001";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Move_0002
 * @tc.name: file_extension_helper_Move_0002
 * @tc.desc: Test function of Move interface for ERROR which sourceFileUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Move_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Move_0002";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri("");
            Uri sourceFileUri("");
            result = g_fah->Move(sourceFileUri, newDirUriTest, testUri);
            EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0002 result:" << result << endl;

            result = g_fah->Delete(newDirUriTest);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0002";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Move_0003
 * @tc.name: file_extension_helper_Move_0003
 * @tc.desc: Test function of Move interface for ERROR which sourceFileUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Move_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Move_0003";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri("");
            result = g_fah->CreateFile(newDirUriTest1, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri2("");
            Uri sourceFileUri("storage/media/100/local/files/Download/test1/test.txt");
            result = g_fah->Move(sourceFileUri, newDirUriTest2, testUri2);
            EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0003 result:" << result << endl;

            result = g_fah->Delete(newDirUriTest1);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Delete(newDirUriTest2);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0003";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Move_0004
 * @tc.name: file_extension_helper_Move_0004
 * @tc.desc: Test function of Move interface for ERROR which sourceFileUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Move_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Move_0004";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri("");
            Uri sourceFileUri("~!@#$%^&*()_");
            result = g_fah->Move(sourceFileUri, newDirUriTest, testUri);
            EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0004 result:" << result << endl;

            result = g_fah->Delete(newDirUriTest);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0004";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Move_0005
 * @tc.name: file_extension_helper_Move_0005
 * @tc.desc: Test function of Move interface for ERROR which targetParentUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Move_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Move_0005";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "test1", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri("");
            result = g_fah->CreateFile(newDirUriTest, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri2("");
            Uri targetParentUri("");
            result = g_fah->Move(testUri, targetParentUri, testUri2);
            EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0005 result:" << result << endl;

            result = g_fah->Delete(newDirUriTest);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0005";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Move_0006
 * @tc.name: file_extension_helper_Move_0006
 * @tc.desc: Test function of Move interface for ERROR which targetParentUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Move_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Move_0006";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri("");
            result = g_fah->CreateFile(newDirUriTest1, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri2("");
            Uri targetParentUri("storage/media/100/local/files/Download/test2");
            result = g_fah->Move(testUri, targetParentUri, testUri2);
            EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0006 result:" << result << endl;

            result = g_fah->Delete(newDirUriTest1);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Delete(newDirUriTest2);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0006";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Move_0007
 * @tc.name: file_extension_helper_Move_0007
 * @tc.desc: Test function of Move interface for ERROR which targetParentUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Move_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Move_0007";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri("");
            result = g_fah->CreateFile(newDirUriTest1, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri2("");
            Uri targetParentUri("~!@#$^%&*()_");
            result = g_fah->Move(testUri, targetParentUri, testUri2);
            EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0007 result:" << result << endl;

            result = g_fah->Delete(newDirUriTest1);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Delete(newDirUriTest2);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0007";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Move_0008
 * @tc.name: file_extension_helper_Move_0008
 * @tc.desc: Test function of Move interface for SUCCESS which move empty folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Move_0008, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Move_0008";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri("");
            result = g_fah->Move(newDirUriTest1, newDirUriTest2, testUri);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0008 result:" << result << endl;

            result = g_fah->Delete(newDirUriTest2);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0008";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Move_0009
 * @tc.name: file_extension_helper_Move_0009
 * @tc.desc: Test function of Move interface for SUCCESS which move more file in folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Move_0009, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Move_0009";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri("");
            for (size_t j = 0; j < 2000; j++)
            {
                string fileName = "test" + ToString(j) + ".txt";
                g_fah->CreateFile(newDirUriTest1, fileName, testUri);
            }

            Uri testUri2("");
            result = g_fah->Move(newDirUriTest1, newDirUriTest2, testUri2);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0009 result:" << result << endl;

            result = g_fah->Delete(newDirUriTest2);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0009";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Move_0010
 * @tc.name: file_extension_helper_Move_0010
 * @tc.desc: Test function of Move interface for SUCCESS which move Multilevel directory folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Move_0010, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Move_0010";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri("");
            g_fah->Mkdir(newDirUriTest1, "test", testUri);
            for (size_t j = 0; j < 64; j++)
            {
                g_fah->Mkdir(testUri, "test", testUri);
            }

            Uri testUri2("");
            result = g_fah->Move(newDirUriTest1, newDirUriTest2, testUri2);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0010 result:" << result << endl;

            result = g_fah->Delete(newDirUriTest2);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0010";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Move_0011
 * @tc.name: file_extension_helper_Move_0011
 * @tc.desc: Test function of Move interface for SUCCESS which move other equipment file.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Move_0011, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Move_0011";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri("");
            g_fah->CreateFile(newDirUriTest1, "test.txt", testUri);

            for (size_t j = i + 1; j < info.size(); j++) {
                Uri otherUri(info[j].uri);
                result = g_fah->Mkdir(otherUri, "test2", newDirUriTest2);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

                result = g_fah->Move(testUri, newDirUriTest2, testUri);
                EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);

                result = g_fah->Move(testUri, newDirUriTest1, testUri);
                EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);

                GTEST_LOG_(INFO) << "Move_0011 result:" << result << endl;

                result = g_fah->Delete(newDirUriTest2);
                EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
            }

            result = g_fah->Delete(newDirUriTest1);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0011";
}

void MoveTdd(shared_ptr<FileAccessHelper> fahs, Uri sourceFile, Uri targetParent, Uri newFile)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_MoveTdd";
    int ret = fahs->Move(sourceFile, targetParent, newFile);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(INFO) << "MoveTdd get result error, code:" << ret;
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_MoveTdd";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Move_0012
 * @tc.name: file_extension_helper_Move_0012
 * @tc.desc: Test function of Move interface for SUCCESS which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Move_0012, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Move_0012";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri[4] = {Uri(""), Uri(""), Uri(""), Uri("")};
            std::string displayName[4] = {"test1.txt", "test2.txt", "test3.txt", "test4.txt"};
            for (int j = 0; j < 4; j++) {
                Uri testUri2("");
                result = g_fah->CreateFile(newDirUriTest1, displayName[j], testUri[j]);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
                std::thread Move_Thread(MoveTdd, g_fah, testUri[j], newDirUriTest2, testUri2);
                Move_Thread.detach();
            }

            result = g_fah->Delete(newDirUriTest1);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Delete(newDirUriTest2);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0012";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Rename_0000
 * @tc.name: file_extension_helper_Rename_0000
 * @tc.desc: Test function of Rename interface for SUCCESS which rename file.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Rename_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Rename_0000";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri("");
            result = g_fah->CreateFile(newDirUriTest, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri renameUri("");
            result = g_fah->Rename(testUri, "test2.txt", renameUri);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Rename_0000 result:" << result << endl;

            result = g_fah->Delete(newDirUriTest);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Rename_0000";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Rename_0001
 * @tc.name: file_extension_helper_Rename_0001
 * @tc.desc: Test function of Rename interface for SUCCESS which rename folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Rename_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Rename_0001";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri renameUri("");
            result = g_fah->Rename(newDirUriTest, "testRename", renameUri);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Rename_0001 result:" << result << endl;

            result = g_fah->Delete(renameUri);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Rename_0001";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Rename_0002
 * @tc.name: file_extension_helper_Rename_0002
 * @tc.desc: Test function of Rename interface for ERROR which sourceFileUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Rename_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Rename_0002";
    try {
        Uri renameUri("");
        Uri sourceFileUri("");
        int result = g_fah->Rename(sourceFileUri, "testRename.txt", renameUri);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Rename_0002 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Rename_0002";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Rename_0003
 * @tc.name: file_extension_helper_Rename_0003
 * @tc.desc: Test function of Rename interface for ERROR which sourceFileUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Rename_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Rename_0003";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri("");
            result = g_fah->CreateFile(newDirUriTest, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri renameUri("");
            Uri sourceFileUri("storage/media/100/local/files/Download/test/test.txt");
            result = g_fah->Rename(sourceFileUri, "testRename.txt", renameUri);
            EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Rename_0003 result:" << result << endl;

            result = g_fah->Delete(newDirUriTest);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Rename_0003";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Rename_0004
 * @tc.name: file_extension_helper_Rename_0004
 * @tc.desc: Test function of Rename interface for ERROR which sourceFileUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Rename_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Rename_0004";
    try {
        Uri renameUri("");
        Uri sourceFileUri("~!@#$%^&*()_");
        int result = g_fah->Rename(sourceFileUri, "testRename.txt", renameUri);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Rename_0004 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Rename_0004";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Rename_0005
 * @tc.name: file_extension_helper_Rename_0005
 * @tc.desc: Test function of Rename interface for ERROR which displayName is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Rename_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Rename_0005";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri("");
            result = g_fah->CreateFile(newDirUriTest, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri renameUri("");
            result = g_fah->Rename(testUri, "", renameUri);
            EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Rename_0005 result:" << result << endl;

            result = g_fah->Delete(newDirUriTest);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Rename_0005";
}

void RenameTdd(shared_ptr<FileAccessHelper> fahs, Uri sourceFile, std::string displayName, Uri newFile)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_RenameTdd";
    int ret = fahs->Rename(sourceFile, displayName, newFile);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(INFO) << "RenameTdd get result error, code:" << ret;
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_RenameTdd";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Rename_0006
 * @tc.name: file_extension_helper_Rename_0006
 * @tc.desc: Test function of Rename interface for SUCCESS which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Rename_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Rename_0006";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri("");
            std::string displayName[4] = {"test1.txt", "test2.txt", "test3.txt", "test4.txt"};
            Uri renameUri("");
            for (int j = 0; j < 4; j++) {
                result = g_fah->CreateFile(newDirUriTest, displayName[j], testUri);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
                std::thread Rename_Thread(RenameTdd, g_fah, testUri, displayName[j], renameUri);
                Rename_Thread.detach();
            }

            result = g_fah->Delete(newDirUriTest);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Rename_0006";
}

/**
 * @tc.number: user_file_service_file_extension_helper_ListFile_0000
 * @tc.name: file_extension_helper_ListFile_0000
 * @tc.desc: Test function of ListFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_ListFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_ListFile_0000";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri("");
            result = g_fah->CreateFile(newDirUriTest, "file_extension_helper_ListFile_0000.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            FileInfo fileInfo;
            fileInfo.uri = newDirUriTest.ToString();
            int64_t offset = 0;
            int64_t maxCount = 1000;
            std::vector<FileInfo> fileInfoVec;
            FileFilter filter;
            result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_GT(fileInfoVec.size(), 0);

            result = g_fah->Delete(newDirUriTest);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_ListFile_0000";
}

/**
 * @tc.number: user_file_service_file_extension_helper_ListFile_0001
 * @tc.name: file_extension_helper_ListFile_0001
 * @tc.desc: Test function of ListFile interface for ERROR which Uri is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_ListFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_ListFile_0001";
    try {
        Uri sourceFileUri("");
        FileInfo fileInfo;
        fileInfo.uri = sourceFileUri.ToString();
        int64_t offset = 0;
        int64_t maxCount = 1000;
        std::vector<FileInfo> fileInfoVec;
        FileFilter filter;
        int result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(fileInfoVec.size(), 0);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_ListFile_0001";
}

/**
 * @tc.number: user_file_service_file_extension_helper_ListFile_0002
 * @tc.name: file_extension_helper_ListFile_0002
 * @tc.desc: Test function of ListFile interface for ERROR which sourceFileUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_ListFile_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_ListFile_0002";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri("");
            result = g_fah->CreateFile(newDirUriTest, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri sourceFileUri("storage/media/100/local/files/Download/test/test.txt");
            FileInfo fileInfo;
            fileInfo.uri = sourceFileUri.ToString();
            Uri sourceFile(fileInfo.uri);
            int64_t offset = 0;
            int64_t maxCount = 1000;
            std::vector<FileInfo> fileInfoVec;
            FileFilter filter;
            result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(fileInfoVec.size(), 0);

            result = g_fah->Delete(newDirUriTest);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_ListFile_0002";
}

/**
 * @tc.number: user_file_service_file_extension_helper_ListFile_0003
 * @tc.name: file_extension_helper_ListFile_0003
 * @tc.desc: Test function of ListFile interface for ERROR which sourceFileUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_ListFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_ListFile_0003";
    try {
        Uri sourceFileUri("~!@#$%^&*()_");
        FileInfo fileInfo;
        fileInfo.uri = sourceFileUri.ToString();
        Uri sourceFile(fileInfo.uri);
        int64_t offset = 0;
        int64_t maxCount = 1000;
        std::vector<FileInfo> fileInfoVec;
        FileFilter filter;
        int result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(fileInfoVec.size(), 0);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_ListFile_0003";
}

void ListFileTdd(shared_ptr<FileAccessHelper> fahs, FileInfo fileInfo, int offset, int maxCount,
     FileFilter filter, std::vector<FileInfo> fileInfoVec)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_ListFileTdd";
    int ret = fahs->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        GTEST_LOG_(INFO) << "ListFile get result error, code:" << ret;
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_ListFileTdd";
}

/**
 * @tc.number: user_file_service_file_extension_helper_ListFile_0004
 * @tc.name: file_extension_helper_ListFile_0004
 * @tc.desc: Test function of ListFile interface for SUCCESS which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_ListFile_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_ListFile_0004";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri testUri("");
            result = g_fah->CreateFile(newDirUriTest, "file_extension_helper_ListFile_0004.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            FileInfo fileInfo;
            fileInfo.uri = newDirUriTest.ToString();
            int offset = 0;
            int maxCount = 1000;
            std::vector<FileInfo> fileInfoVec;
            FileFilter filter;
            for (int j = 0; j < 4; j++) {
                std::thread ListFile_Thread(ListFileTdd, g_fah, fileInfo, offset, maxCount, filter, fileInfoVec);
                ListFile_Thread.detach();
            }

            result = g_fah->Delete(newDirUriTest);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_ListFile_0004";
}

/**
 * @tc.number: user_file_service_file_extension_helper_GetRoots_0000
 * @tc.name: file_extension_helper_GetRoots_0000
 * @tc.desc: Test function of GetRoots interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_GetRoots_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_GetRoots_0000";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_GT(info.size(), 0);
        GTEST_LOG_(INFO) << "GetRoots_0000 result:" << info.size() << endl;

        for (size_t i = 0; i < info.size(); i++) {
            GTEST_LOG_(INFO) << info[i].uri;
            GTEST_LOG_(INFO) << info[i].displayName;
            GTEST_LOG_(INFO) << info[i].deviceFlags;
            GTEST_LOG_(INFO) << info[i].deviceType;
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_GetRoots_0000";
}

/**
 * @tc.number: user_file_service_file_extension_helper_allInterface_0000
 * @tc.name: file_extension_helper_allInterface_0000
 * @tc.desc: Test function of allInterface interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_allInterface_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_allInterface_0000";
    try {
        auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        auto remoteObj = saManager->GetSystemAbility(ABILITY_ID);
        vector<AAFwk::Want> wants;
        int ret = FileAccessHelper::GetRegisteredFileAccessExtAbilityInfo(wants);
        EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
        shared_ptr<FileAccessHelper> fahs = FileAccessHelper::Creator(remoteObj, wants);

        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            string uri = parentUri.ToString();
            GTEST_LOG_(INFO) << uri;

            string document = uri + "/Documents";
            Uri documentUri(document);
            bool isExist = false;
            result = fahs->Access(documentUri, isExist);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            if (!isExist) {
                result = fahs->Mkdir(parentUri, "Documents", documentUri);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            }

            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = fahs->Mkdir(documentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            result = fahs->Mkdir(documentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri newFileUri("");
            result = fahs->CreateFile(newDirUriTest1, "file_extension_helper_allInterface_0000.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            result = fahs->Rename(newFileUri, "test1.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            Uri moveUri("");
            result = fahs->Move(newFileUri, newDirUriTest2, moveUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            int fd;
            result = g_fah->OpenFile(moveUri, 0, fd);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);

            GTEST_LOG_(INFO) << "OpenFile_0000 result:" << result << endl;

            result = fahs->Delete(newDirUriTest1);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);

            result = fahs->Delete(newDirUriTest2);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_allInterface_0000";
}

/**
 * @tc.number: user_file_service_file_extension_helper_Access_0000
 * @tc.name: file_extension_helper_Access_0000
 * @tc.desc: Test function of Access interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, file_extension_helper_Access_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin file_extension_helper_Access_0000";
    try {
        uint64_t selfTokenId_ = GetSelfTokenID();

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
            result = g_fah->CreateFile(newDirUriTest, "file_extension_helper_Access_0000.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            bool isExist = false;
            result = g_fah->Access(newDirUriTest, isExist);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_TRUE(isExist);

            result = g_fah->Access(newFileUri, isExist);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_TRUE(isExist);

            result = g_fah->Delete(newDirUriTest);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Access(newDirUriTest, isExist);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_FALSE(isExist);
        }

        SetSelfTokenID(selfTokenId_);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Access_0000";
}
} // namespace