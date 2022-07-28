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
#include <unistd.h>

#include <gtest/gtest.h>

#include "base/security/access_token/interfaces/innerkits/accesstoken/include/accesstoken_kit.h"
#include "base/security/access_token/interfaces/innerkits/token_setproc/include/token_setproc.h"
#include "file_access_helper.h"
#include "iservice_registry.h"

namespace {
using namespace std;
using namespace OHOS;
using namespace FileAccessFwk;
int uid = 5003;
int ok = 0;
int error = 102825986;
shared_ptr<FileAccessHelper> fah = nullptr;

class FileExtensionHelperTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        cout << "FileExtensionHelperTest code test" << endl;

        auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        auto remoteObj = saManager->GetSystemAbility(uid);
        AAFwk::Want want;
        want.SetElementName("com.ohos.UserFile.ExternalFileManager", "FileExtensionAbility");
        vector<AAFwk::Want> wants {want};
        FileAccessHelper::GetRegisterFileAccessExtAbilityInfo();
        fah = FileAccessHelper::Creator(remoteObj, wants);
    }
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};

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
};

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_OpenFile_0000
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
        OHOS::Security::AccessToken::AccessTokenIDEx tokenIdEx = {0};
        tokenIdEx = OHOS::Security::AccessToken::AccessTokenKit::AllocHapToken
            (g_infoManagerTestInfoParms, g_infoManagerTestPolicyPrams);
        OHOS::Security::AccessToken::AccessTokenID tokenId = tokenIdEx.tokenIdExStruct.tokenID;
        SetSelfTokenID(tokenId);
        sleep(2); // sleep 2 seconds

        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            GTEST_LOG_(INFO) << parentUri.ToString();
            
            Uri newDirUriTest1("");
            int result = fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, ok);

            Uri newFileUri("");
            result = fah->CreateFile(newDirUriTest1, "file_extension_helper_OpenFile_0000.txt", newFileUri);
            EXPECT_EQ(result, ok);

            result = fah->OpenFile(newFileUri, 0);
            EXPECT_GT(result, ok);

            GTEST_LOG_(INFO) << "OpenFile_0000 result:" << result << endl;
            
            result = fah->Delete(newDirUriTest1);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_OpenFile_0000";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_OpenFile_0001
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
        int result = fah->OpenFile(uri, 0);
        EXPECT_EQ(result, error);
        GTEST_LOG_(INFO) << "OpenFile_0001 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_OpenFile_0001";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_OpenFile_0002
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newFileUri("");
            int result = fah->CreateFile(parentUri, "file_extension_helper_OpenFile_0002.txt", newFileUri);
            EXPECT_EQ(result, ok);

            Uri uri("storage/media/100/local/files/Download/file_extension_helper_OpenFile_0002.txt");
            result = fah->OpenFile(uri, 0);
            EXPECT_EQ(result, error);
            GTEST_LOG_(INFO) << "OpenFile_0002 result:" << result << endl;

            result = fah->Delete(newFileUri);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_OpenFile_0002";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_OpenFile_0003
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
        int result = fah->OpenFile(uri, 0);
        EXPECT_EQ(result, error);
        GTEST_LOG_(INFO) << "OpenFile_0003 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_OpenFile_0003";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_OpenFile_0004
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newFileUri("");
            int result = fah->CreateFile(parentUri, "file_extension_helper_OpenFile_0004.txt", newFileUri);
            EXPECT_EQ(result, ok);

            result = fah->OpenFile(newFileUri, -1);
            EXPECT_LT(result, ok);
            GTEST_LOG_(INFO) << "OpenFile_0004 result:" << result << endl;

            result = fah->Delete(newFileUri);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_OpenFile_0004";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_OpenFile_0005
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newFileUri("");
            int result = fah->CreateFile(parentUri, "file_extension_helper_OpenFile_0005.txt", newFileUri);
            EXPECT_EQ(result, ok);

            result = fah->OpenFile(newFileUri, 1);
            EXPECT_GT(result, ok);
            GTEST_LOG_(INFO) << "OpenFile_0005 result:" << result << endl;

            result = fah->Delete(newFileUri);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_OpenFile_0005";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_OpenFile_0006
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newFileUri("");
            int result = fah->CreateFile(parentUri, "file_extension_helper_OpenFile_0006.txt", newFileUri);
            EXPECT_EQ(result, ok);

            result = fah->OpenFile(newFileUri, 2);
            EXPECT_GT(result, ok);
            GTEST_LOG_(INFO) << "OpenFile_0006 result:" << result << endl;

            result = fah->Delete(newFileUri);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_OpenFile_0006";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_CreateFile_0000
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newFileUri("");
            int result = fah->CreateFile(parentUri, "file_extension_helper_CreateFile_0000.txt", newFileUri);
            EXPECT_EQ(result, ok);
            GTEST_LOG_(INFO) << "CreateFile_0000 result:" << result << endl;

            result = fah->Delete(newFileUri);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_CreateFile_0000";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_CreateFile_0001
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
        int result = fah->CreateFile(parentUri, "file_extension_helper_CreateFile_0001.txt", newFileUri);
        EXPECT_NE(result, ok);
        GTEST_LOG_(INFO) << "CreateFile_0001 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_CreateFile_0001";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_CreateFile_0002
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
        int result = fah->CreateFile(parentUri, "file_extension_helper_CreateFile_0002.txt", newFileUri);
        EXPECT_NE(result, ok);
        GTEST_LOG_(INFO) << "CreateFile_0002 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_CreateFile_0002";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_CreateFile_0003
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
        int result = fah->CreateFile(parentUri, "file_extension_helper_CreateFile_0003.txt", newFileUri);
        EXPECT_NE(result, ok);
        GTEST_LOG_(INFO) << "CreateFile_0003 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_CreateFile_0003";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_CreateFile_0004
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newFileUri("");
            string displayName = "";
            int result = fah->CreateFile(parentUri, displayName, newFileUri);
            EXPECT_NE(result, ok);
            GTEST_LOG_(INFO) << "CreateFile_0004 result:" << result << endl;
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_CreateFile_0004";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Mkdir_0000
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest("");
            int result = fah->Mkdir(parentUri, "file_extension_helper_Mkdir_0000", newDirUriTest);
            EXPECT_EQ(result, ok);
            GTEST_LOG_(INFO) << "Mkdir_0000 result:" << result << endl;

            result = fah->Delete(newDirUriTest);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Mkdir_0000";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Mkdir_0001
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
        int result = fah->Mkdir(parentUri, "file_extension_helper_Mkdir_0001", newDirUriTest);
        EXPECT_NE(result, ok);
        GTEST_LOG_(INFO) << "Mkdir_0001 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Mkdir_0001";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Mkdir_0002
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
        int result = fah->Mkdir(parentUri, "file_extension_helper_Mkdir_0002", newDirUriTest);
        EXPECT_NE(result, ok);
        GTEST_LOG_(INFO) << "Mkdir_0002 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Mkdir_0002";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Mkdir_0003
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
        int result = fah->Mkdir(parentUri, "file_extension_helper_Mkdir_0003", newDirUriTest);
        EXPECT_NE(result, ok);
        GTEST_LOG_(INFO) << "Mkdir_0003 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Mkdir_0003";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Mkdir_0004
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest("");
            string displayName = "";
            int result = fah->Mkdir(parentUri, displayName, newDirUriTest);
            EXPECT_NE(result, ok);
            GTEST_LOG_(INFO) << "Mkdir_0004 result:" << result << endl;
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Mkdir_0004";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Delete_0000
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest("");
            int result = fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, ok);

            Uri newFileUri("");
            result = fah->CreateFile(newDirUriTest, "file_extension_helper_Delete_0000.txt", newFileUri);
            EXPECT_EQ(result, ok);

            result = fah->Delete(newFileUri);
            EXPECT_GE(result, ok);
            GTEST_LOG_(INFO) << "Delete_0000 result:" << result << endl;
            
            result = fah->Delete(newDirUriTest);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Delete_0000";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Delete_0001
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest("");
            int result = fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, ok);
            
            result = fah->Delete(newDirUriTest);
            EXPECT_GE(result, ok);
            GTEST_LOG_(INFO) << "Delete_0001 result:" << result << endl;
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Delete_0001";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Delete_0002
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
        int result = fah->Delete(selectFileUri);
        EXPECT_LT(result, ok);
        GTEST_LOG_(INFO) << "Delete_0002 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Delete_0002";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Delete_0003
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest("");
            int result = fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, ok);
            
            Uri selectFileUri("storage/media/100/local/files/Download/test");
            result = fah->Delete(selectFileUri);
            EXPECT_LT(result, ok);
            
            result = fah->Delete(newDirUriTest);
            EXPECT_GE(result, ok);
            GTEST_LOG_(INFO) << "Delete_0003 result:" << result << endl;
        }
        
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Delete_0003";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Delete_0004
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
        int result = fah->Delete(selectFileUri);
        EXPECT_LT(result, ok);
        GTEST_LOG_(INFO) << "Delete_0004 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Delete_0004";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Move_0000
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            int result = fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, ok);

            result = fah->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, ok);

            Uri testUri("");
            result = fah->CreateFile(newDirUriTest1, "test.txt", testUri);
            EXPECT_EQ(result, ok);

            Uri testUri2("");
            result = fah->Move(testUri, newDirUriTest2, testUri2);
            EXPECT_GE(result, ok);
            GTEST_LOG_(INFO) << "Move_0000 result:" << result << endl;
            
            result = fah->Delete(newDirUriTest1);
            EXPECT_GE(result, ok);

            result = fah->Delete(newDirUriTest2);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0000";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Move_0001
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            int result = fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, ok);

            result = fah->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, ok);

            Uri testUri("");
            result = fah->CreateFile(newDirUriTest1, "test.txt", testUri);
            EXPECT_EQ(result, ok);

            Uri testUri2("");
            result = fah->Move(newDirUriTest1, newDirUriTest2, testUri2);
            EXPECT_GE(result, ok);
            GTEST_LOG_(INFO) << "Move_0001 result:" << result << endl;

            result = fah->Delete(newDirUriTest2);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0001";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Move_0002
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest("");
            int result = fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, ok);

            Uri testUri("");
            Uri sourceFileUri("");
            result = fah->Move(sourceFileUri, newDirUriTest, testUri);
            EXPECT_LT(result, ok);
            GTEST_LOG_(INFO) << "Move_0002 result:" << result << endl;
            
            result = fah->Delete(newDirUriTest);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0002";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Move_0003
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            int result = fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, ok);

            result = fah->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, ok);

            Uri testUri("");
            result = fah->CreateFile(newDirUriTest1, "test.txt", testUri);
            EXPECT_EQ(result, ok);

            Uri testUri2("");
            Uri sourceFileUri("storage/media/100/local/files/Download/test1/test.txt");
            result = fah->Move(sourceFileUri, newDirUriTest2, testUri2);
            EXPECT_LT(result, ok);
            GTEST_LOG_(INFO) << "Move_0003 result:" << result << endl;
            
            result = fah->Delete(newDirUriTest1);
            EXPECT_GE(result, ok);

            result = fah->Delete(newDirUriTest2);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0003";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Move_0004
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest("");
            int result = fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, ok);

            Uri testUri("");
            Uri sourceFileUri("~!@#$%^&*()_");
            result = fah->Move(sourceFileUri, newDirUriTest, testUri);
            EXPECT_LT(result, ok);
            GTEST_LOG_(INFO) << "Move_0004 result:" << result << endl;
            
            result = fah->Delete(newDirUriTest);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0004";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Move_0005
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest("");
            int result = fah->Mkdir(parentUri, "test1", newDirUriTest);
            EXPECT_EQ(result, ok);

            Uri testUri("");
            result = fah->CreateFile(newDirUriTest, "test.txt", testUri);
            EXPECT_EQ(result, ok);

            Uri testUri2("");
            Uri targetParentUri("");
            result = fah->Move(testUri, targetParentUri, testUri2);
            EXPECT_LT(result, ok);
            GTEST_LOG_(INFO) << "Move_0005 result:" << result << endl;
            
            result = fah->Delete(newDirUriTest);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0005";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Move_0006
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            int result = fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, ok);

            result = fah->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, ok);

            Uri testUri("");
            result = fah->CreateFile(newDirUriTest1, "test.txt", testUri);
            EXPECT_EQ(result, ok);

            Uri testUri2("");
            Uri targetParentUri("storage/media/100/local/files/Download/test2");
            result = fah->Move(testUri, targetParentUri, testUri2);
            EXPECT_LT(result, ok);
            GTEST_LOG_(INFO) << "Move_0006 result:" << result << endl;
            
            result = fah->Delete(newDirUriTest1);
            EXPECT_GE(result, ok);

            result = fah->Delete(newDirUriTest2);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0006";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Move_0007
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            int result = fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, ok);

            result = fah->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, ok);

            Uri testUri("");
            result = fah->CreateFile(newDirUriTest1, "test.txt", testUri);
            EXPECT_EQ(result, ok);

            Uri testUri2("");
            Uri targetParentUri("~!@#$^%&*()_");
            result = fah->Move(testUri, targetParentUri, testUri2);
            EXPECT_LT(result, ok);
            GTEST_LOG_(INFO) << "Move_0007 result:" << result << endl;
            
            result = fah->Delete(newDirUriTest1);
            EXPECT_GE(result, ok);

            result = fah->Delete(newDirUriTest2);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0007";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Move_0008
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            int result = fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, ok);

            result = fah->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, ok);

            Uri testUri("");
            result = fah->Move(newDirUriTest1, newDirUriTest2, testUri);
            EXPECT_GE(result, ok);
            GTEST_LOG_(INFO) << "Move_0008 result:" << result << endl;

            result = fah->Delete(newDirUriTest2);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0008";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Move_0009
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            int result = fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, ok);

            result = fah->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, ok);

            for (size_t j = 0; j < 2000; j++)
            {
                Uri testUri("");
                string fileName = "test" + ToString(j) + ".txt";
                fah->CreateFile(newDirUriTest1, fileName, testUri);
            }

            Uri testUri2("");
            result = fah->Move(newDirUriTest1, newDirUriTest2, testUri2);
            EXPECT_GE(result, ok);
            GTEST_LOG_(INFO) << "Move_0009 result:" << result << endl;

            result = fah->Delete(newDirUriTest2);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0009";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Move_0010
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            int result = fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, ok);

            result = fah->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, ok);

            Uri testUri("");
            fah->Mkdir(newDirUriTest1, "test", testUri);
            for (size_t j = 0; j < 64; j++)
            {
                fah->Mkdir(testUri, "test", testUri);
            }

            Uri testUri2("");
            result = fah->Move(newDirUriTest1, newDirUriTest2, testUri2);
            EXPECT_GE(result, ok);
            GTEST_LOG_(INFO) << "Move_0010 result:" << result << endl;

            result = fah->Delete(newDirUriTest2);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0010";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Move_0011
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            int result = fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, ok);
            
            Uri testUri("");
            fah->CreateFile(newDirUriTest1, "test.txt", testUri);

            for (size_t j = i + 1; j < info.size(); j++) {
                Uri otherUri = info[j].uri;
                result = fah->Mkdir(otherUri, "test2", newDirUriTest2);
                EXPECT_EQ(result, ok);

                result = fah->Move(testUri, newDirUriTest2, testUri);
                EXPECT_GE(result, ok);

                result = fah->Move(testUri, newDirUriTest1, testUri);
                EXPECT_GE(result, ok);

                GTEST_LOG_(INFO) << "Move_0011 result:" << result << endl;

                result = fah->Delete(newDirUriTest2);
                EXPECT_GE(result, ok);
            }

            result = fah->Delete(newDirUriTest1);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Move_0011";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Rename_0000
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest("");
            int result = fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, ok);

            Uri testUri("");
            result = fah->CreateFile(newDirUriTest, "test.txt", testUri);
            EXPECT_EQ(result, ok);

            Uri renameUri("");
            result = fah->Rename(testUri, "test2.txt", renameUri);
            EXPECT_GE(result, ok);
            GTEST_LOG_(INFO) << "Rename_0000 result:" << result << endl;

            result = fah->Delete(newDirUriTest);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Rename_0000";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Rename_0001
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest("");
            int result = fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, ok);

            Uri renameUri("");
            result = fah->Rename(newDirUriTest, "testRename", renameUri);
            EXPECT_GE(result, ok);
            GTEST_LOG_(INFO) << "Rename_0001 result:" << result << endl;

            result = fah->Delete(renameUri);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Rename_0001";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Rename_0002
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
        int result = fah->Rename(sourceFileUri, "testRename.txt", renameUri);
        EXPECT_LT(result, ok);
        GTEST_LOG_(INFO) << "Rename_0002 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Rename_0002";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Rename_0003
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest("");
            int result = fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, ok);

            Uri testUri("");
            result = fah->CreateFile(newDirUriTest, "test.txt", testUri);
            EXPECT_EQ(result, ok);

            Uri renameUri("");
            Uri sourceFileUri("storage/media/100/local/files/Download/test/test.txt");
            result = fah->Rename(sourceFileUri, "testRename.txt", renameUri);
            EXPECT_LT(result, ok);
            GTEST_LOG_(INFO) << "Rename_0003 result:" << result << endl;
            
            result = fah->Delete(newDirUriTest);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Rename_0003";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Rename_0004
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
        int result = fah->Rename(sourceFileUri, "testRename.txt", renameUri);
        EXPECT_LT(result, ok);
        GTEST_LOG_(INFO) << "Rename_0004 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Rename_0004";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_Rename_0005
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest("");
            int result = fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, ok);

            Uri testUri("");
            result = fah->CreateFile(newDirUriTest, "test.txt", testUri);
            EXPECT_EQ(result, ok);

            Uri renameUri("");
            result = fah->Rename(testUri, "", renameUri);

            int errorCode = 102825990;
            EXPECT_EQ(result, errorCode);
            GTEST_LOG_(INFO) << "Rename_0005 result:" << result << endl;

            result = fah->Delete(newDirUriTest);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_Rename_0005";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_ListFile_0000
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest("");
            int result = fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, ok);

            Uri testUri("");
            result = fah->CreateFile(newDirUriTest, "file_extension_helper_ListFile_0000.txt", testUri);
            EXPECT_EQ(result, ok);

            std::vector<FileInfo> fileInfo = fah->ListFile(newDirUriTest);
            EXPECT_GT(fileInfo.size(), 0);
            GTEST_LOG_(INFO) << "ListFile_0000 result:" << fileInfo.size() << endl;

            result = fah->Delete(newDirUriTest);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_ListFile_0000";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_ListFile_0001
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
        std::vector<FileInfo> fileInfo = fah->ListFile(sourceFileUri);
        EXPECT_EQ(fileInfo.size(), 0);
        GTEST_LOG_(INFO) << "ListFile_0001 result:" << fileInfo.size() << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_ListFile_0001";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_ListFile_0002
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
        vector<DeviceInfo> info = fah->GetRoots();
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri = info[i].uri;
            Uri newDirUriTest("");
            int result = fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, ok);

            Uri testUri("");
            result = fah->CreateFile(newDirUriTest, "test.txt", testUri);
            EXPECT_EQ(result, ok);

            Uri sourceFileUri("storage/media/100/local/files/Download/test/test.txt");
            std::vector<FileInfo> fileInfo = fah->ListFile(sourceFileUri);
            EXPECT_EQ(fileInfo.size(), 0);
            GTEST_LOG_(INFO) << "ListFile_0002 result:" << fileInfo.size() << endl;

            result = fah->Delete(newDirUriTest);
            EXPECT_GE(result, ok);
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_ListFile_0002";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_ListFile_0003
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
        std::vector<FileInfo> fileInfo = fah->ListFile(sourceFileUri);
        EXPECT_EQ(fileInfo.size(), 0);
        GTEST_LOG_(INFO) << "ListFile_0003 result:" << fileInfo.size() << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_ListFile_0003";
}

/**
 * @tc.number: SUB_user_file_service_file_extension_helper_GetRoots_0000
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
        vector<DeviceInfo> info = fah->GetRoots();
        EXPECT_GT(info.size(), 0);
        GTEST_LOG_(INFO) << "GetRoots_0000 result:" << info.size() << endl;

        for (size_t i = 0; i < info.size(); i++) {
            GTEST_LOG_(INFO) << info[i].uri.ToString();
            GTEST_LOG_(INFO) << info[i].displayName;
            GTEST_LOG_(INFO) << info[i].deviceId;
            GTEST_LOG_(INFO) << info[i].flags;
            GTEST_LOG_(INFO) << info[i].type;
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileExtensionHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end file_extension_helper_GetRoots_0000";
}
} // namespace