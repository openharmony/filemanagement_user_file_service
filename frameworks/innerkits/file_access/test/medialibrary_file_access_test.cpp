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

#include "base/security/access_token/interfaces/innerkits/accesstoken/include/accesstoken_kit.h"
#include "base/security/access_token/interfaces/innerkits/token_setproc/include/token_setproc.h"
#include "file_access_framework_errno.h"
#include "file_access_helper.h"
#include "iservice_registry.h"

namespace {
using namespace std;
using namespace OHOS;
using namespace FileAccessFwk;
const int32_t READ = 0;
const int32_t WRITE = 1;
const int32_t WRITE_READ = 2;
const int ABILITY_ID = 5003;
const int INIT_THREADS_NUMBER = 4;
const int ACTUAL_SUCCESS_THREADS_NUMBER = 1;
int g_num = 0;
shared_ptr<FileAccessHelper> g_fah = nullptr;
OHOS::Security::AccessToken::AccessTokenID g_tokenId;
Uri g_newDirUri("");
const int UID_TRANSFORM_TMP = 20000000;
const int UID_DEFAULT = 0;

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

OHOS::Security::AccessToken::PermissionStateFull g_infoManagerTestState3 = {
    .permissionName = "ohos.permission.GET_BUNDLE_INFO_PRIVILEGED",
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

OHOS::Security::AccessToken::PermissionDef g_infoManagerTestPermDef3 = {
    .permissionName = "ohos.permission.GET_BUNDLE_INFO_PRIVILEGED",
    .bundleName = "ohos.acts.multimedia.mediaLibrary",
    .grantMode = 1,
    .availableLevel = OHOS::Security::AccessToken::APL_NORMAL,
    .label = "label",
    .labelId = 1,
    .description = "GET_BUNDLE_INFO_PRIVILEGED",
    .descriptionId = 1
};

// permission info
OHOS::Security::AccessToken::HapPolicyParams g_infoManagerTestPolicyPrams = {
    .apl = OHOS::Security::AccessToken::APL_NORMAL,
    .domain = "test.domain",
    .permList = {g_infoManagerTestPermDef, g_infoManagerTestPermDef1, g_infoManagerTestPermDef2,
        g_infoManagerTestPermDef3},
    .permStateList = {g_infoManagerTestState, g_infoManagerTestState1, g_infoManagerTestState2,
        g_infoManagerTestState3}
};

// hap info
OHOS::Security::AccessToken::HapInfoParams const g_infoManagerTestInfoParms = {
    .userID = 1,
    .bundleName = "fileAccessHelperTest",
    .instIndex = 0,
    .appIDDesc = "testtesttesttest"
};

class FileAccessHelperTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        cout << "FileAccessHelperTest code test" << endl;
        auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        auto remoteObj = saManager->GetSystemAbility(ABILITY_ID);
        AAFwk::Want want;
        vector<AAFwk::Want> wantVec;
        setuid(UID_TRANSFORM_TMP);
        int ret = FileAccessHelper::GetRegisteredFileAccessExtAbilityInfo(wantVec);
        EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
        bool sus = false;
        for (size_t i = 0; i < wantVec.size(); i++) {
            auto element = wantVec[i].GetElement();
            if (element.GetBundleName() == "com.ohos.medialibrary.medialibrarydata" &&
                element.GetAbilityName() == "FileExtensionAbility") {
                want = wantVec[i];
                sus = true;
                break;
            }
        }
        EXPECT_TRUE(sus);
        vector<AAFwk::Want> wants {want};
        bool temp = false;
        g_fah = FileAccessHelper::Creator(remoteObj, wants);
        if (g_fah == nullptr) {
            GTEST_LOG_(ERROR) << "external_file_access_test g_fah is nullptr";
            EXPECT_TRUE(temp);
        }
        setuid(UID_DEFAULT);
        OHOS::Security::AccessToken::AccessTokenIDEx tokenIdEx = {0};
        tokenIdEx = OHOS::Security::AccessToken::AccessTokenKit::AllocHapToken(
            g_infoManagerTestInfoParms, g_infoManagerTestPolicyPrams);
        g_tokenId = tokenIdEx.tokenIdExStruct.tokenID;
        SetSelfTokenID(g_tokenId);
    }
    static void TearDownTestCase()
    {
        g_fah->Release();
        g_fah = nullptr;
        OHOS::Security::AccessToken::AccessTokenKit::DeleteToken(g_tokenId);
    };
    void SetUp() {};
    void TearDown() {};
};

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
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_OpenFile_0000";
    try {
        Uri parentUri = GetParentUri();
        bool isExist = false;
        int result = g_fah->Access(g_newDirUri, isExist);
        EXPECT_LE(result, OHOS::FileAccessFwk::ERR_OK);
        if (!isExist) {
            result = g_fah->Mkdir(parentUri, "Download", g_newDirUri);
            EXPECT_LE(result, OHOS::FileAccessFwk::ERR_OK);
        }
        Uri newDirUriTest("datashare:///media/root/file");
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
        GTEST_LOG_(INFO) << "OpenFile_0000 result:" << result << endl;
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
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_OpenFile_0001";
    try {
        Uri uri("");
        int fd;
        int result = g_fah->OpenFile(uri, WRITE_READ, fd);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0001 result:" << result << endl;
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
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_OpenFile_0002";
    try {
        Uri uri("storage/media/100/local/files/Download/medialibrary_file_access_OpenFile_0002.txt");
        int fd;
        int result = g_fah->OpenFile(uri, WRITE_READ, fd);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0002 result:" << result << endl;
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
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_OpenFile_0003";
    try {
        Uri uri("~!@#$%^&*()_");
        int fd;
        int result = g_fah->OpenFile(uri, WRITE_READ, fd);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0003 result:" << result << endl;
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
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_OpenFile_0004";
    try {
        Uri newFileUri("");
        int result = g_fah->CreateFile(g_newDirUri, "medialibrary_file_access_OpenFile_0004.txt", newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int fd;
        int flag = -1;
        result = g_fah->OpenFile(newFileUri, flag, fd);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0004 result:" << result << endl;
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
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_OpenFile_0005";
    try {
        Uri newFileUri("");
        int result = g_fah->CreateFile(g_newDirUri, "medialibrary_file_access_OpenFile_0005.txt", newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int fd;
        result = g_fah->OpenFile(newFileUri, READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0005 result:" << result << endl;
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
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_OpenFile_0006";
    try {
        Uri newFileUri("");
        int result = g_fah->CreateFile(g_newDirUri, "medialibrary_file_access_OpenFile_0006.txt", newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int fd;
        result = g_fah->OpenFile(newFileUri, WRITE, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0006 result:" << result << endl;
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
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_OpenFile_0007";
    try {
        Uri newFileUri("");
        int result = g_fah->CreateFile(g_newDirUri, "medialibrary_file_access_OpenFile_0007.txt", newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int fd;
        result = g_fah->OpenFile(newFileUri, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "OpenFile_0007 result:" << result << endl;
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
        GTEST_LOG_(INFO) << "CreateFile_0000 result:" << result << endl;
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
        GTEST_LOG_(INFO) << "CreateFile_0001 result:" << result << endl;
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
        GTEST_LOG_(INFO) << "CreateFile_0002 result:" << result << endl;
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
        GTEST_LOG_(INFO) << "CreateFile_0003 result:" << result << endl;
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
        GTEST_LOG_(INFO) << "CreateFile_0004 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_CreateFile_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_CreateFile_0004";
}

static void CreateFileTdd(shared_ptr<FileAccessHelper> fahs, Uri parent, std::string displayName, Uri newDir)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_CreateFileTdd";
    int ret = fahs->CreateFile(parent, displayName, newDir);
    if (ret < OHOS::FileAccessFwk::ERR_OK) {
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
        GTEST_LOG_(INFO) << "g_newDirUri.ToString() =" << g_newDirUri.ToString() ;
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
        GTEST_LOG_(INFO) << "Mkdir_0000 result:" << result << endl;
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
        GTEST_LOG_(INFO) << "Mkdir_0001 result:" << result << endl;
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
        GTEST_LOG_(INFO) << "Mkdir_0002 result:" << result << endl;
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
        GTEST_LOG_(INFO) << "Mkdir_0003 result:" << result << endl;
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
        GTEST_LOG_(INFO) << "Mkdir_0004 result:" << result << endl;
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_Mkdir_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_Mkdir_0004";
}

static void MkdirTdd(shared_ptr<FileAccessHelper> fahs, Uri parent, std::string displayName, Uri newDir)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_MkdirTdd";
    int ret = fahs->Mkdir(parent, displayName, newDir);
    if (ret < OHOS::FileAccessFwk::ERR_OK) {
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
        GTEST_LOG_(INFO) << "Delete_0000 result:" << result << endl;
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
        GTEST_LOG_(INFO) << "Delete_0001 result:" << result << endl;
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
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Delete_0002 result:" << result << endl;
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
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Delete_0003 result:" << result << endl;
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
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Delete_0004 result:" << result << endl;
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
        GTEST_LOG_(INFO) << "Move_0000 result:" << result << endl;
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
        int result = g_fah->Mkdir(g_newDirUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(g_newDirUri, "test2", newDirUriTest2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        result = g_fah->CreateFile(newDirUriTest1, "test.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri2("");
        result = g_fah->Move(newDirUriTest1, newDirUriTest2, testUri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0001 result:" << result << endl;
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
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0002 result:" << result << endl;
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
        int result = g_fah->Mkdir(g_newDirUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(g_newDirUri, "test2", newDirUriTest2);
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
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0004 result:" << result << endl;
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
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0005 result:" << result << endl;
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
        int result = g_fah->Mkdir(g_newDirUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(g_newDirUri, "test2", newDirUriTest2);
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
        int result = g_fah->Mkdir(g_newDirUri, "test1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Mkdir(g_newDirUri, "test2", newDirUriTest2);
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
        GTEST_LOG_(INFO) << "Move_0008 result:" << result << endl;
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
        GTEST_LOG_(INFO) << "Move_0009 result:" << result << endl;
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
        GTEST_LOG_(INFO) << "Move_0010 result:" << result << endl;
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
        GTEST_LOG_(INFO) << "Rename_0000 result:" << result << endl;
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
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Rename_0001";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri renameUri("");
        result = g_fah->Rename(newDirUriTest, "testRename", renameUri);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Rename_0001 result:" << result << endl;
        result = g_fah->Delete(newDirUriTest);
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
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Rename_0002";
    try {
        Uri renameUri("");
        Uri sourceFileUri("");
        int result = g_fah->Rename(sourceFileUri, "testRename.txt", renameUri);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Rename_0002 result:" << result << endl;
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
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Rename_0003 result:" << result << endl;
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
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_Rename_0004";
    try {
        Uri renameUri("");
        Uri sourceFileUri("~!@#$%^&*()_");
        int result = g_fah->Rename(sourceFileUri, "testRename.txt", renameUri);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Rename_0004 result:" << result << endl;
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
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Rename_0005 result:" << result << endl;
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
        FileFilter filter({}, {}, {}, 0, 0, false, false);
        int result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
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
        FileFilter filter({}, {}, {}, 0, 0, false, false);
        result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_LT(result, OHOS::FileAccessFwk::ERR_OK);
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
        FileFilter filter({".txt"}, {}, {}, 0, 0, false, true);
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

static void ListFileTdd(shared_ptr<FileAccessHelper> fahs, FileInfo fileInfo, int offset, int maxCount,
    FileFilter filter, std::vector<FileInfo> fileInfoVec)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_ListFileTdd";
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
        int64_t offset = 0;
        int64_t maxCount = 1000;
        std::vector<FileInfo> fileInfoVec;
        g_num = 0;
        FileFilter filter({".txt"}, {}, {}, 0, 0, false, true);
        for (int j = 0; j < INIT_THREADS_NUMBER; j++) {
            std::thread execthread(ListFileTdd, g_fah, fileInfo, offset, maxCount, filter, fileInfoVec);
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
        fileInfo.uri = "datashare:///media/root";
        int64_t offset = 0;
        int64_t maxCount = 1000;
        std::vector<FileInfo> fileInfoVec;
        FileFilter filter({".q1w2e3r4"}, {}, {}, 0, 0, false, true);
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
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_ScanFile_0001";
    try {
        Uri newDirUriTest("");
        int result = g_fah->Mkdir(g_newDirUri, "test", newDirUriTest);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri testUri("");
        FileInfo fileInfo;
        fileInfo.uri = "datashare:///media/root";
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_ScanFile_0001.q1w2e3r4", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->CreateFile(newDirUriTest, "medialibrary_file_access_ScanFile_0001.txt", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int64_t offset = 0;
        int64_t maxCount = 1000;
        std::vector<FileInfo> fileInfoVec;
        FileFilter filter({}, {}, {}, 0, 0, false, false);
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
        FileFilter filter({".q1w2e3r4"}, {}, {}, 0, 0, false, true);
        result = g_fah->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        EXPECT_EQ(fileInfoVec.size(), 1);
        FileFilter filter1({".q1w2e3r4", ".txt"}, {}, {}, 0, 0, false, true);
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
        FileFilter filter({".q1w2e3r4"}, {}, {}, 0, 0, false, true);
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

static void ScanFileTdd(FileInfo fileInfo, int offset, int maxCount, FileFilter filter,
    std::vector<FileInfo> fileInfoVec)
{
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_ScanFileTdd";
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
        fileInfo.uri = "datashare:///media/root";
        int64_t offset = 0;
        int64_t maxCount = 1000;
        std::vector<FileInfo> fileInfoVec;
        FileFilter filter({".q1w2e3r4"}, {}, {}, 0, 0, false, true);
        g_num = 0;
        for (int j = 0; j < INIT_THREADS_NUMBER; j++) {
            std::thread execthread(ScanFileTdd, fileInfo, offset, maxCount, filter, fileInfoVec);
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
    GTEST_LOG_(INFO) << "FileAccessHelperTest-begin medialibrary_file_access_GetRoots_0000";
    try {
        uint64_t selfTokenId = GetSelfTokenID();
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
        string uri = "datashare:///media/root";
        string displayName = "LOCAL";
        EXPECT_EQ(info[0].uri, uri);
        EXPECT_EQ(info[0].displayName, displayName);
        EXPECT_EQ(info[0].deviceType, DEVICE_LOCAL_DISK);
        EXPECT_EQ(info[0].deviceFlags, DEVICE_FLAG_SUPPORTS_READ | DEVICE_FLAG_SUPPORTS_WRITE);
        SetSelfTokenID(selfTokenId);
    } catch (...) {
        GTEST_LOG_(ERROR) << "medialibrary_file_access_GetRoots_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessHelperTest-end medialibrary_file_access_GetRoots_0000";
}
} // namespace