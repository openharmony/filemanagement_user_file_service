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

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "accesstoken_kit.h"
#include "context_impl.h"
#include "file_access_framework_errno.h"
#include "iservice_registry.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

#include "file_access_helper.h"

namespace {
using namespace std;
using namespace OHOS;
using namespace FileAccessFwk;
using json = nlohmann::json;
const int ABILITY_ID = 5003;
shared_ptr<FileAccessHelper> g_fah = nullptr;
const int UID_TRANSFORM_TMP = 20000000;
const int UID_DEFAULT = 0;
const int COPY_EXCEPTION = -1;
shared_ptr<OHOS::AbilityRuntime::Context> g_context = nullptr;

static void SetNativeToken(bool isSystemApp)
{
    uint64_t tokenId;
    const char **perms = new const char *[1];
    perms[0] = "ohos.permission.FILE_ACCESS_MANAGER";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .aplStr = "system_core",
    };

    infoInstance.processName = "SetUpTestCase";
    tokenId = GetAccessTokenId(&infoInstance);
    if (isSystemApp) {
        const uint64_t systemAppMask = (static_cast<uint64_t>(1) << 32);
        tokenId |= systemAppMask;
    }
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    delete[] perms;
}

class AbnormalFileExtensionHelperTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        cout << "AbnormalFileExtensionHelperTest code test" << endl;
        SetNativeToken(true);
        auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        auto remoteObj = saManager->GetSystemAbility(ABILITY_ID);
        g_context = make_shared<OHOS::AbilityRuntime::ContextImpl>();
        g_context->SetToken(remoteObj);
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
        g_fah = FileAccessHelper::Creator(remoteObj, wants);
        if (g_fah == nullptr) {
            GTEST_LOG_(ERROR) << "AbnormalFileExtensionHelperTest g_fah is nullptr";
            exit(1);
        }
        setuid(UID_DEFAULT);
        SetNativeToken(false);
    }
    static void TearDownTestCase()
    {
        g_fah->Release();
        g_fah = nullptr;
    };
    void SetUp(){};
    void TearDown(){};
};

/**
 * @tc.number: user_file_service_external_file_access_OpenFile_0000
 * @tc.name: abnormal_external_file_access_OpenFile_0000
 * @tc.desc: Test function of OpenFile interface for ERROR because of set not system app flag.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I76YA0
 */
HWTEST_F(AbnormalFileExtensionHelperTest, abnormal_external_file_access_OpenFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-begin abnormal_external_file_access_OpenFile_0000";
    try {
        Uri uri("");
        int fd;
        int result = g_fah->OpenFile(uri, WRITE_READ, fd);
        EXPECT_EQ(result, E_PERMISSION_SYS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "abnormal_external_file_access_OpenFile_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-end abnormal_external_file_access_OpenFile_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_CreateFile_0000
 * @tc.name: abnormal_external_file_access_CreateFile_0000
 * @tc.desc: Test function of CreateFile interface for ERROR because of set not system app flag.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I76YA0
 */
HWTEST_F(AbnormalFileExtensionHelperTest, abnormal_external_file_access_CreateFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-begin abnormal_external_file_access_CreateFile_0000";
    try {
        Uri parent("");
        string displayName("");
        Uri newFile("");
        int result = g_fah->CreateFile(parent, displayName, newFile);
        EXPECT_EQ(result, E_PERMISSION_SYS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "abnormal_external_file_access_CreateFile_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-end abnormal_external_file_access_CreateFile_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_Mkdir_0000
 * @tc.name: abnormal_external_file_access_Mkdir_0000
 * @tc.desc: Test function of Mkdir interface for ERROR because of set not system app flag.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I76YA0
 */
HWTEST_F(AbnormalFileExtensionHelperTest, abnormal_external_file_access_Mkdir_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-begin abnormal_external_file_access_Mkdir_0000";
    try {
        Uri parent("");
        string displayName("");
        Uri newDir("");
        int result = g_fah->Mkdir(parent, displayName, newDir);
        EXPECT_EQ(result, E_PERMISSION_SYS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "abnormal_external_file_access_Mkdir_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-end abnormal_external_file_access_Mkdir_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_Delete_0000
 * @tc.name: abnormal_external_file_access_Delete_0000
 * @tc.desc: Test function of Delete interface for ERROR because of set not system app flag.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I76YA0
 */
HWTEST_F(AbnormalFileExtensionHelperTest, abnormal_external_file_access_Delete_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-begin abnormal_external_file_access_Delete_0000";
    try {
        Uri uri("");
        int result = g_fah->Delete(uri);
        EXPECT_EQ(result, E_PERMISSION_SYS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "abnormal_external_file_access_Delete_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-end abnormal_external_file_access_Delete_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_Move_0000
 * @tc.name: abnormal_external_file_access_Move_0000
 * @tc.desc: Test function of Move interface for ERROR because of set not system app flag.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I76YA0
 */
HWTEST_F(AbnormalFileExtensionHelperTest, abnormal_external_file_access_Move_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-begin abnormal_external_file_access_Move_0000";
    try {
        Uri sourceFile("");
        Uri targetParent("");
        Uri newFile("");
        int result = g_fah->Move(sourceFile, targetParent, newFile);
        EXPECT_EQ(result, E_PERMISSION_SYS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "abnormal_external_file_access_Move_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-end abnormal_external_file_access_Move_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_Copy_0000
 * @tc.name: abnormal_external_file_access_Copy_0000
 * @tc.desc: Test function of Copy interface for ERROR because of set not system app flag.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I76YA0
 */
HWTEST_F(AbnormalFileExtensionHelperTest, abnormal_external_file_access_Copy_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-begin abnormal_external_file_access_Copy_0000";
    try {
        Uri sourceUri("");
        Uri destUri("");
        vector<CopyResult> copyResult;
        bool force = false;
        int result = g_fah->Copy(sourceUri, destUri, copyResult, force);
        EXPECT_EQ(result, COPY_EXCEPTION);
    } catch (...) {
        GTEST_LOG_(ERROR) << "abnormal_external_file_access_Copy_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-end abnormal_external_file_access_Copy_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_Rename_0000
 * @tc.name: abnormal_external_file_access_Rename_0000
 * @tc.desc: Test function of Rename interface for ERROR because of set not system app flag.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I76YA0
 */
HWTEST_F(AbnormalFileExtensionHelperTest, abnormal_external_file_access_Rename_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-begin abnormal_external_file_access_Rename_0000";
    try {
        Uri sourceFile("");
        string displayName("");
        Uri newFile("");
        int result = g_fah->Rename(sourceFile, displayName, newFile);
        EXPECT_EQ(result, E_PERMISSION_SYS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "abnormal_external_file_access_Rename_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-end abnormal_external_file_access_Rename_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_ListFile_0000
 * @tc.name: abnormal_external_file_access_ListFile_0000
 * @tc.desc: Test function of ListFile interface for ERROR because of set not system app flag.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I76YA0
 */
HWTEST_F(AbnormalFileExtensionHelperTest, abnormal_external_file_access_ListFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-begin abnormal_external_file_access_ListFile_0000";
    try {
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        vector<FileInfo> fileInfoVec;
        int result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, E_PERMISSION_SYS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "abnormal_external_file_access_ListFile_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-end abnormal_external_file_access_ListFile_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_ScanFile_0000
 * @tc.name: abnormal_external_file_access_ScanFile_0000
 * @tc.desc: Test function of ScanFile interface for ERROR because of set not system app flag.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I76YA0
 */
HWTEST_F(AbnormalFileExtensionHelperTest, abnormal_external_file_access_ScanFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-begin abnormal_external_file_access_ScanFile_0000";
    try {
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        vector<FileInfo> fileInfoVec;
        int result = g_fah->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, E_PERMISSION_SYS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "abnormal_external_file_access_ScanFile_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-end abnormal_external_file_access_ScanFile_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_Query_0000
 * @tc.name: abnormal_external_file_access_Query_0000
 * @tc.desc: Test function of Query interface for ERROR because of set not system app flag.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I76YA0
 */
HWTEST_F(AbnormalFileExtensionHelperTest, abnormal_external_file_access_Query_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-begin abnormal_external_file_access_Query_0000";
    try {
        Uri uri("");
        string metaJson("");
        int result = g_fah->Query(uri, metaJson);
        EXPECT_EQ(result, E_PERMISSION_SYS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "abnormal_external_file_access_Query_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-end abnormal_external_file_access_Query_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_GetRoots_0000
 * @tc.name: abnormal_external_file_access_GetRoots_0000
 * @tc.desc: Test function of GetRoots interface for ERROR because of set not system app flag.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I76YA0
 */
HWTEST_F(AbnormalFileExtensionHelperTest, abnormal_external_file_access_GetRoots_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-begin abnormal_external_file_access_GetRoots_0000";
    try {
        vector<RootInfo> rootInfoVec;
        int result = g_fah->GetRoots(rootInfoVec);
        EXPECT_EQ(result, E_PERMISSION_SYS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "abnormal_external_file_access_GetRoots_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-end abnormal_external_file_access_GetRoots_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_GetRegisteredFileAccessExtAbilityInfo_0000
 * @tc.name: abnormal_external_file_access_GetRegisteredFileAccessExtAbilityInfo_0000
 * @tc.desc: Test function of GetRegisteredFileAccessExtAbilityInfo interface for ERROR because of set not system app
 *           flag.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I76YA0
 */
HWTEST_F(AbnormalFileExtensionHelperTest, abnormal_external_file_access_GetRegisteredFileAccessExtAbilityInfo_0000,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-begin "
        "abnormal_external_file_access_GetRegisteredFileAccessExtAbilityInfo_0000";
    try {
        vector<AAFwk::Want> wantVec;
        int result = g_fah->GetRegisteredFileAccessExtAbilityInfo(wantVec);
        EXPECT_EQ(result, E_PERMISSION_SYS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "abnormal_external_file_access_GetRegisteredFileAccessExtAbilityInfo_0000 occurs an "
        "exception.";
    }
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-end "
        "abnormal_external_file_access_GetRegisteredFileAccessExtAbilityInfo_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_Access_0000
 * @tc.name: abnormal_external_file_access_Access_0000
 * @tc.desc: Test function of Access interface for ERROR because of set not system app flag.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I76YA0
 */
HWTEST_F(AbnormalFileExtensionHelperTest, abnormal_external_file_access_Access_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-begin abnormal_external_file_access_Access_0000";
    try {
        Uri uri("");
        bool isExist = true;
        int result = g_fah->Access(uri, isExist);
        EXPECT_EQ(result, E_PERMISSION_SYS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "abnormal_external_file_access_Access_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-end abnormal_external_file_access_Access_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_GetThumbnail_0000
 * @tc.name: abnormal_external_file_access_GetThumbnail_0000
 * @tc.desc: Test function of GetThumbnail interface for ERROR because of set not system app flag.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I76YA0
 */
HWTEST_F(AbnormalFileExtensionHelperTest, abnormal_external_file_access_GetThumbnail_0000,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-begin abnormal_external_file_access_GetThumbnail_0000";
    try {
        Uri uri("");
        ThumbnailSize thumbnailSize;
        shared_ptr<PixelMap> pixelMap;
        int result = g_fah->GetThumbnail(uri, thumbnailSize, pixelMap);
        EXPECT_EQ(result, E_PERMISSION_SYS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "abnormal_external_file_access_GetThumbnail_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-end abnormal_external_file_access_GetThumbnail_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_GetFileInfoFromUri_0000
 * @tc.name: abnormal_external_file_access_GetFileInfoFromUri_0000
 * @tc.desc: Test function of GetFileInfoFromUri interface for ERROR because of set not system app flag.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I76YA0
 */
HWTEST_F(AbnormalFileExtensionHelperTest, abnormal_external_file_access_GetFileInfoFromUri_0000,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-begin abnormal_external_file_access_GetFileInfoFromUri_0000";
    try {
        Uri selectFile("");
        FileInfo fileInfo;
        int result = g_fah->GetFileInfoFromUri(selectFile, fileInfo);
        EXPECT_EQ(result, E_PERMISSION_SYS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "abnormal_external_file_access_GetFileInfoFromUri_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-end abnormal_external_file_access_GetFileInfoFromUri_0000";
}

/**
 * @tc.number: user_file_service_external_file_access_GetFileInfoFromRelativePath_0000
 * @tc.name: abnormal_external_file_access_GetFileInfoFromRelativePath_0000
 * @tc.desc: Test function of GetFileInfoFromRelativePath interface for ERROR because of set not system app flag.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I76YA0
 */
HWTEST_F(AbnormalFileExtensionHelperTest, abnormal_external_file_access_GetFileInfoFromRelativePath_0000,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-begin"
        "abnormal_external_file_access_GetFileInfoFromRelativePath_0000";
    try {
        string selectFile("");
        FileInfo fileInfo;
        int result = g_fah->GetFileInfoFromRelativePath(selectFile, fileInfo);
        EXPECT_EQ(result, E_PERMISSION_SYS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "abnormal_external_file_access_GetFileInfoFromRelativePath_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "AbnormalFileExtensionHelperTest-end"
        "abnormal_external_file_access_GetFileInfoFromRelativePath_0000";
}
} // namespace