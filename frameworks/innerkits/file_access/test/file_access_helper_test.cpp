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
#include "file_access_framework_errno.h"
#include "file_access_helper.h"
#include "iservice_registry.h"


namespace {
using namespace std;
using namespace OHOS;
using namespace FileAccessFwk;
int ABILITY_ID = 5003;
shared_ptr<FileAccessHelper> fah = nullptr;
Uri newDirUri("");

class FileAccessHelperTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        cout << "FileAccessHelperTest code test" << endl;

        auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        auto remoteObj = saManager->GetSystemAbility(ABILITY_ID);
        AAFwk::Want want;
        want.SetElementName("com.ohos.medialibrary.medialibrarydata", "FileExtensionAbility");
        vector<AAFwk::Want> wants {want};
        vector<AAFwk::Want> wantVec;
        FileAccessHelper::GetRegisterFileAccessExtAbilityInfo(wantVec);
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
        .bundleName = "fileAccessHelperTest",
        .instIndex = 0,
        .appIDDesc = "testtesttesttest"
    };
};

/**
 * @tc.number: SUB_user_file_service_file_access_helper_OpenFile_0000
 * @tc.name: file_access_helper_OpenFile_0000
 * @tc.desc: Test function of OpenFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_OpenFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_OpenFile_0000";
    try {
        OHOS::Security::AccessToken::AccessTokenIDEx tokenIdEx = {0};
        tokenIdEx = OHOS::Security::AccessToken::AccessTokenKit::AllocHapToken
            (g_infoManagerTestInfoParms, g_infoManagerTestPolicyPrams);
        OHOS::Security::AccessToken::AccessTokenID tokenId = tokenIdEx.tokenIdExStruct.tokenID;
        SetSelfTokenID(tokenId);

        vector<DeviceInfo> info = fah->GetRoots();
        Uri parentUri("");
        if (info.size() > 0) {
            parentUri = info[0].uri;
            GTEST_LOG_(INFO) << parentUri.ToString();
        }
        
        int result = fah->Mkdir(parentUri, "Download", newDirUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        
        Uri newDirUriTest("");
        result = fah->Mkdir(newDirUri, "test1", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri newFileUri("");
        result = fah->CreateFile(newDirUriTest, "file_access_helper_OpenFile_0000.txt", newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = fah->OpenFile(newFileUri, 0);
        EXPECT_GT(result, OHOS::FileAccessFwk::ERR_OK);

        GTEST_LOG_(INFO) << "OpenFile_0000 result:" << result << endl;
        
        result = fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_OpenFile_0000";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_OpenFile_0001
 * @tc.name: file_access_helper_OpenFile_0001
 * @tc.desc: Test function of OpenFile interface for ERROR which Uri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_OpenFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_OpenFile_0001";
    try {
        Uri uri("");
        int result = fah->OpenFile(uri, 0);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0001 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_OpenFile_0001";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_OpenFile_0002
 * @tc.name: file_access_helper_OpenFile_0002
 * @tc.desc: Test function of OpenFile interface for ERROR which Uri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_OpenFile_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_OpenFile_0002";
    try {
        Uri newFileUri("");
        int result = fah->CreateFile(newDirUri, "file_access_helper_OpenFile_0002.txt", newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri uri("storage/media/100/local/files/Download/file_access_helper_OpenFile_0002.txt");
        result = fah->OpenFile(uri, 0);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0002 result:" << result << endl;

        result = fah->Delete(newFileUri);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_OpenFile_0002";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_OpenFile_0003
 * @tc.name: file_access_helper_OpenFile_0003
 * @tc.desc: Test function of OpenFile interface for ERROR which Uri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_OpenFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_OpenFile_0003";
    try {
        Uri uri("~!@#$%^&*()_");
        int result = fah->OpenFile(uri, 0);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0003 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_OpenFile_0003";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_OpenFile_0004
 * @tc.name: file_access_helper_OpenFile_0004
 * @tc.desc: Test function of OpenFile interface for ERROR which flag is -1.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_OpenFile_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_OpenFile_0004";
    try {
        Uri newFileUri("");
        int result = fah->CreateFile(newDirUri, "file_access_helper_OpenFile_0004.txt", newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = fah->OpenFile(newFileUri, -1);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0004 result:" << result << endl;

        result = fah->Delete(newFileUri);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_OpenFile_0004";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_OpenFile_0005
 * @tc.name: file_access_helper_OpenFile_0005
 * @tc.desc: Test function of OpenFile interface for SUCCESS which flag is 1.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_OpenFile_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_OpenFile_0005";
    try {
        Uri newFileUri("");
        int result = fah->CreateFile(newDirUri, "file_access_helper_OpenFile_0005.txt", newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = fah->OpenFile(newFileUri, 1);
        EXPECT_GT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0005 result:" << result << endl;

        result = fah->Delete(newFileUri);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_OpenFile_0005";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_OpenFile_0006
 * @tc.name: file_access_helper_OpenFile_0006
 * @tc.desc: Test function of OpenFile interface for SUCCESS which flag is 2.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_OpenFile_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_OpenFile_0006";
    try {
        Uri newFileUri("");
        int result = fah->CreateFile(newDirUri, "file_access_helper_OpenFile_0006.txt", newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = fah->OpenFile(newFileUri, 2);
        EXPECT_GT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0006 result:" << result << endl;

        result = fah->Delete(newFileUri);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_OpenFile_0006";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_CreateFile_0000
 * @tc.name: file_access_helper_CreateFile_0000
 * @tc.desc: Test function of CreateFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_CreateFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_CreateFile_0000";
    try {
        Uri newFileUri("");
        int result = fah->CreateFile(newDirUri, "file_access_helper_CreateFile_0000.txt", newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "CreateFile_0000 result:" << result << endl;

        result = fah->Delete(newFileUri);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_CreateFile_0000";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_CreateFile_0001
 * @tc.name: file_access_helper_CreateFile_0001
 * @tc.desc: Test function of CreateFile interface for ERROR which parentUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_CreateFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_CreateFile_0001";
    try {
        Uri newFileUri("");
        Uri parentUri("");
        int result = fah->CreateFile(parentUri, "file_access_helper_CreateFile_0001.txt", newFileUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "CreateFile_0001 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_CreateFile_0001";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_CreateFile_0002
 * @tc.name: file_access_helper_CreateFile_0002
 * @tc.desc: Test function of CreateFile interface for ERROR which parentUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_CreateFile_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_CreateFile_0002";
    try {
        Uri newFileUri("");
        Uri parentUri("storage/media/100/local/files/Download");
        int result = fah->CreateFile(parentUri, "file_access_helper_CreateFile_0002.txt", newFileUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "CreateFile_0002 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_CreateFile_0002";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_CreateFile_0003
 * @tc.name: file_access_helper_CreateFile_0003
 * @tc.desc: Test function of CreateFile interface for ERROR which parentUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_CreateFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_CreateFile_0002";
    try {
        Uri newFileUri("");
        Uri parentUri("~!@#$%^&*()_");
        int result = fah->CreateFile(parentUri, "file_access_helper_CreateFile_0003.txt", newFileUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "CreateFile_0003 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_CreateFile_0003";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_CreateFile_0004
 * @tc.name: file_access_helper_CreateFile_0004
 * @tc.desc: Test function of CreateFile interface for ERROR which displayName is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_CreateFile_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_CreateFile_0004";
    try {
        Uri newFileUri("");
        string displayName = "";
        int result = fah->CreateFile(newDirUri, displayName, newFileUri);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "CreateFile_0004 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_CreateFile_0004";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Mkdir_0000
 * @tc.name: file_access_helper_Mkdir_0000
 * @tc.desc: Test function of Mkdir interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Mkdir_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Mkdir_0000";
    try {
        Uri newDirUriTest("");
        int result = fah->Mkdir(newDirUri, "file_access_helper_Mkdir_0000", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Mkdir_0000 result:" << result << endl;

        result = fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Mkdir_0000";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Mkdir_0001
 * @tc.name: file_access_helper_Mkdir_0001
 * @tc.desc: Test function of Mkdir interface for ERROR which parentUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Mkdir_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Mkdir_0001";
    try {
        Uri newDirUriTest("");
        Uri parentUri("");
        int result = fah->Mkdir(parentUri, "file_access_helper_Mkdir_0001", newDirUriTest);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Mkdir_0001 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Mkdir_0001";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Mkdir_0002
 * @tc.name: file_access_helper_Mkdir_0002
 * @tc.desc: Test function of Mkdir interface for ERROR which parentUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Mkdir_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Mkdir_0002";
    try {
        Uri newDirUriTest("");
        Uri parentUri("storage/media/100/local/files/Download");
        int result = fah->Mkdir(parentUri, "file_access_helper_Mkdir_0002", newDirUriTest);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Mkdir_0002 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Mkdir_0002";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Mkdir_0003
 * @tc.name: file_access_helper_Mkdir_0003
 * @tc.desc: Test function of Mkdir interface for ERROR which parentUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Mkdir_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Mkdir_0002";
    try {
        Uri newDirUriTest("");
        Uri parentUri("~!@#$%^&*()_");
        int result = fah->Mkdir(parentUri, "file_access_helper_Mkdir_0003", newDirUriTest);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Mkdir_0003 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Mkdir_0003";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Mkdir_0004
 * @tc.name: file_access_helper_Mkdir_0004
 * @tc.desc: Test function of Mkdir interface for ERROR which displayName is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Mkdir_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Mkdir_0004";
    try {
        Uri newDirUriTest("");
        string displayName = "";
        int result = fah->Mkdir(newDirUri, displayName, newDirUriTest);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Mkdir_0004 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Mkdir_0004";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Delete_0000
 * @tc.name: file_access_helper_Delete_0000
 * @tc.desc: Test function of Delete interface for SUCCESS which delete file.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Delete_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Delete_0000";
    try {
        Uri newDirUriTest("");
        int result = fah->Mkdir(newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri newFileUri("");
        result = fah->CreateFile(newDirUriTest, "file_access_helper_Delete_0000.txt", newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = fah->Delete(newFileUri);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Delete_0000 result:" << result << endl;
        
        result = fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Delete_0000";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Delete_0001
 * @tc.name: file_access_helper_Delete_0001
 * @tc.desc: Test function of Delete interface for SUCCESS which delete folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Delete_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Delete_0001";
    try {
        Uri newDirUriTest("");
        int result = fah->Mkdir(newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        
        result = fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Delete_0001 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Delete_0001";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Delete_0002
 * @tc.name: file_access_helper_Delete_0002
 * @tc.desc: Test function of Delete interface for ERROR which selectFileUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Delete_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Delete_0002";
    try {
        Uri selectFileUri("");
        int result = fah->Delete(selectFileUri);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Delete_0002 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Delete_0002";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Delete_0003
 * @tc.name: file_access_helper_Delete_0003
 * @tc.desc: Test function of Delete interface for ERROR which selectFileUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Delete_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Delete_0003";
    try {
        Uri newDirUriTest("");
        int result = fah->Mkdir(newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        
        Uri selectFileUri("storage/media/100/local/files/Download/test");
        result = fah->Delete(selectFileUri);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        
        result = fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Delete_0003 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Delete_0003";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Delete_0004
 * @tc.name: file_access_helper_Delete_0004
 * @tc.desc: Test function of Delete interface for ERROR which selectFileUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Delete_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Delete_0004";
    try {
        Uri selectFileUri("!@#$%^&*()");
        int result = fah->Delete(selectFileUri);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Delete_0004 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Delete_0004";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Move_0000
 * @tc.name: file_access_helper_Move_0000
 * @tc.desc: Test function of Move interface for SUCCESS which move file.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Move_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Move_0000";
    try {
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        int result = fah->Mkdir(newDirUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = fah->Mkdir(newDirUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri testUri("");
        result = fah->CreateFile(newDirUriTest1, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri testUri2("");
        result = fah->Move(testUri, newDirUriTest2, testUri2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0000 result:" << result << endl;
        
        result = fah->Delete(newDirUriTest1);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);

        result = fah->Delete(newDirUriTest2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Move_0000";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Move_0001
 * @tc.name: file_access_helper_Move_0001
 * @tc.desc: Test function of Move interface for SUCCESS which move folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Move_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Move_0001";
    try {
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        int result = fah->Mkdir(newDirUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = fah->Mkdir(newDirUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri testUri("");
        result = fah->CreateFile(newDirUriTest1, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri testUri2("");
        result = fah->Move(newDirUriTest1, newDirUriTest2, testUri2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0001 result:" << result << endl;

        result = fah->Delete(newDirUriTest2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Move_0001";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Move_0002
 * @tc.name: file_access_helper_Move_0002
 * @tc.desc: Test function of Move interface for ERROR which sourceFileUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Move_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Move_0002";
    try {
        Uri newDirUriTest("");
        int result = fah->Mkdir(newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri testUri("");
        Uri sourceFileUri("");
        result = fah->Move(sourceFileUri, newDirUriTest, testUri);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0002 result:" << result << endl;
        
        result = fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Move_0002";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Move_0003
 * @tc.name: file_access_helper_Move_0003
 * @tc.desc: Test function of Move interface for ERROR which sourceFileUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Move_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Move_0003";
    try {
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        int result = fah->Mkdir(newDirUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = fah->Mkdir(newDirUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri testUri("");
        result = fah->CreateFile(newDirUriTest1, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri testUri2("");
        Uri sourceFileUri("storage/media/100/local/files/Download/test1/test.txt");
        result = fah->Move(sourceFileUri, newDirUriTest2, testUri2);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0003 result:" << result << endl;
        
        result = fah->Delete(newDirUriTest1);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);

        result = fah->Delete(newDirUriTest2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Move_0003";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Move_0004
 * @tc.name: file_access_helper_Move_0004
 * @tc.desc: Test function of Move interface for ERROR which sourceFileUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Move_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Move_0004";
    try {
        Uri newDirUriTest("");
        int result = fah->Mkdir(newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri testUri("");
        Uri sourceFileUri("~!@#$%^&*()_");
        result = fah->Move(sourceFileUri, newDirUriTest, testUri);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0004 result:" << result << endl;
        
        result = fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Move_0004";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Move_0005
 * @tc.name: file_access_helper_Move_0005
 * @tc.desc: Test function of Move interface for ERROR which targetParentUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Move_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Move_0005";
    try {
        Uri newDirUriTest("");
        int result = fah->Mkdir(newDirUri, "test1", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri testUri("");
        result = fah->CreateFile(newDirUriTest, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri testUri2("");
        Uri targetParentUri("");
        result = fah->Move(testUri, targetParentUri, testUri2);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0005 result:" << result << endl;
        
        result = fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Move_0005";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Move_0006
 * @tc.name: file_access_helper_Move_0006
 * @tc.desc: Test function of Move interface for ERROR which targetParentUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Move_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Move_0006";
    try {
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        int result = fah->Mkdir(newDirUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = fah->Mkdir(newDirUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri testUri("");
        result = fah->CreateFile(newDirUriTest1, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri testUri2("");
        Uri targetParentUri("storage/media/100/local/files/Download/test2");
        result = fah->Move(testUri, targetParentUri, testUri2);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0006 result:" << result << endl;
        
        result = fah->Delete(newDirUriTest1);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);

        result = fah->Delete(newDirUriTest2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Move_0006";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Move_0007
 * @tc.name: file_access_helper_Move_0007
 * @tc.desc: Test function of Move interface for ERROR which targetParentUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Move_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Move_0007";
    try {
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        int result = fah->Mkdir(newDirUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = fah->Mkdir(newDirUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri testUri("");
        result = fah->CreateFile(newDirUriTest1, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri testUri2("");
        Uri targetParentUri("~!@#$^%&*()_");
        result = fah->Move(testUri, targetParentUri, testUri2);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0007 result:" << result << endl;
        
        result = fah->Delete(newDirUriTest1);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);

        result = fah->Delete(newDirUriTest2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Move_0007";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Move_0008
 * @tc.name: file_access_helper_Move_0008
 * @tc.desc: Test function of Move interface for SUCCESS which move empty folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Move_0008, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Move_0008";
    try {
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        int result = fah->Mkdir(newDirUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = fah->Mkdir(newDirUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri testUri2("");
        result = fah->Move(newDirUriTest1, newDirUriTest2, testUri2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0008 result:" << result << endl;

        result = fah->Delete(newDirUriTest2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Move_0008";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Move_0009
 * @tc.name: file_access_helper_Move_0009
 * @tc.desc: Test function of Move interface for SUCCESS which move more file in folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Move_0009, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Move_0009";
    try {
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        int result = fah->Mkdir(newDirUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = fah->Mkdir(newDirUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        for (size_t i = 0; i < 2000; i++) {
            Uri testUri("");
            string fileName = "test" + ToString(i) + ".txt";
            fah->CreateFile(newDirUriTest1, fileName, testUri);
        }

        Uri testUri2("");
        result = fah->Move(newDirUriTest1, newDirUriTest2, testUri2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0009 result:" << result << endl;

        result = fah->Delete(newDirUriTest2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Move_0009";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Move_0010
 * @tc.name: file_access_helper_Move_0010
 * @tc.desc: Test function of Move interface for SUCCESS which move Multilevel directory folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Move_0010, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Move_0010";
    try {
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        int result = fah->Mkdir(newDirUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = fah->Mkdir(newDirUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri testUri("");
        fah->Mkdir(newDirUriTest1, "test", testUri);
        for (size_t i = 0; i < 50; i++) {
            fah->Mkdir(testUri, "test", testUri);
        }

        Uri testUri2("");
        result = fah->Move(newDirUriTest1, newDirUriTest2, testUri2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0010 result:" << result << endl;

        result = fah->Delete(newDirUriTest2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Move_0010";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Rename_0000
 * @tc.name: file_access_helper_Rename_0000
 * @tc.desc: Test function of Rename interface for SUCCESS which rename file.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Rename_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Rename_0000";
    try {
        Uri newDirUriTest("");
        int result = fah->Mkdir(newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri testUri("");
        result = fah->CreateFile(newDirUriTest, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri renameUri("");
        result = fah->Rename(testUri, "test2.txt", renameUri);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Rename_0000 result:" << result << endl;

        result = fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Rename_0000";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Rename_0001
 * @tc.name: file_access_helper_Rename_0001
 * @tc.desc: Test function of Rename interface for SUCCESS which rename folder.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Rename_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Rename_0001";
    try {
        Uri newDirUriTest("");
        int result = fah->Mkdir(newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri renameUri("");
        result = fah->Rename(newDirUriTest, "testRename", renameUri);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Rename_0001 result:" << result << endl;

        result = fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Rename_0001";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Rename_0002
 * @tc.name: file_access_helper_Rename_0002
 * @tc.desc: Test function of Rename interface for ERROR which sourceFileUri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Rename_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Rename_0002";
    try {
        Uri renameUri("");
        Uri sourceFileUri("");
        int result = fah->Rename(sourceFileUri, "testRename.txt", renameUri);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Rename_0002 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Rename_0002";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Rename_0003
 * @tc.name: file_access_helper_Rename_0003
 * @tc.desc: Test function of Rename interface for ERROR which sourceFileUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Rename_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Rename_0003";
    try {
        Uri newDirUriTest("");
        int result = fah->Mkdir(newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri testUri("");
        result = fah->CreateFile(newDirUriTest, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri renameUri("");
        Uri sourceFileUri("storage/media/100/local/files/Download/test/test.txt");
        result = fah->Rename(sourceFileUri, "testRename.txt", renameUri);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Rename_0003 result:" << result << endl;
        
        result = fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Rename_0003";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Rename_0004
 * @tc.name: file_access_helper_Rename_0004
 * @tc.desc: Test function of Rename interface for ERROR which sourceFileUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Rename_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Rename_0004";
    try {
        Uri renameUri("");
        Uri sourceFileUri("~!@#$%^&*()_");
        int result = fah->Rename(sourceFileUri, "testRename.txt", renameUri);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Rename_0004 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Rename_0004";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_Rename_0005
 * @tc.name: file_access_helper_Rename_0005
 * @tc.desc: Test function of Rename interface for ERROR which displayName is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_Rename_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_Rename_0005";
    try {
        Uri newDirUriTest("");
        int result = fah->Mkdir(newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri testUri("");
        result = fah->CreateFile(newDirUriTest, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri renameUri("");
        result = fah->Rename(testUri, "", renameUri);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Rename_0005 result:" << result << endl;

        result = fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_Rename_0005";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_ListFile_0000
 * @tc.name: file_access_helper_ListFile_0000
 * @tc.desc: Test function of ListFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_ListFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_ListFile_0000";
    try {
        Uri newDirUriTest("");
        int result = fah->Mkdir(newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri testUri("");
        result = fah->CreateFile(newDirUriTest, "file_access_helper_ListFile_0000.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        std::vector<FileInfo> fileInfo = fah->ListFile(newDirUriTest);
        EXPECT_GT(fileInfo.size(), 0);
        GTEST_LOG_(INFO) << "ListFile_0000 result:" << fileInfo.size() << endl;

        result = fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_ListFile_0000";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_ListFile_0001
 * @tc.name: file_access_helper_ListFile_0001
 * @tc.desc: Test function of ListFile interface for ERROR which Uri is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_ListFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_ListFile_0001";
    try {
        Uri sourceFileUri("");
        std::vector<FileInfo> fileInfo = fah->ListFile(sourceFileUri);
        EXPECT_EQ(fileInfo.size(), 0);
        GTEST_LOG_(INFO) << "ListFile_0001 result:" << fileInfo.size() << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_ListFile_0001";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_ListFile_0002
 * @tc.name: file_access_helper_ListFile_0002
 * @tc.desc: Test function of ListFile interface for ERROR which sourceFileUri is absolute path.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_ListFile_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_ListFile_0002";
    try {
        Uri newDirUriTest("");
        int result = fah->Mkdir(newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri testUri("");
        result = fah->CreateFile(newDirUriTest, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        Uri sourceFileUri("storage/media/100/local/files/Download/test/test.txt");
        std::vector<FileInfo> fileInfo = fah->ListFile(sourceFileUri);
        EXPECT_EQ(fileInfo.size(), 0);
        GTEST_LOG_(INFO) << "ListFile_0002 result:" << fileInfo.size() << endl;

        result = fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_ListFile_0002";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_ListFile_0003
 * @tc.name: file_access_helper_ListFile_0003
 * @tc.desc: Test function of ListFile interface for ERROR which sourceFileUri is special symbols.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_ListFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_ListFile_0003";
    try {
        Uri sourceFileUri("~!@#$%^&*()_");
        std::vector<FileInfo> fileInfo = fah->ListFile(sourceFileUri);
        EXPECT_EQ(fileInfo.size(), 0);
        GTEST_LOG_(INFO) << "ListFile_0003 result:" << fileInfo.size() << endl;
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_ListFile_0003";
}

/**
 * @tc.number: SUB_user_file_service_file_access_helper_GetRoots_0000
 * @tc.name: file_access_helper_GetRoots_0000
 * @tc.desc: Test function of GetRoots interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileAccessHelperTest, file_access_helper_GetRoots_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin file_access_helper_GetRoots_0000";
    try {
        uint64_t selfTokenId_ = GetSelfTokenID();
        
        vector<DeviceInfo> info = fah->GetRoots();
        EXPECT_GT(info.size(), 0);

        if (info.size() > 0) {
            GTEST_LOG_(INFO) << info[0].uri.ToString();
            GTEST_LOG_(INFO) << info[0].displayName;
            GTEST_LOG_(INFO) << info[0].deviceId;
            GTEST_LOG_(INFO) << info[0].flags;
            GTEST_LOG_(INFO) << info[0].type;
        }

        SetSelfTokenID(selfTokenId_);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileAccessHelperTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end file_access_helper_GetRoots_0000";
}
} // namespace