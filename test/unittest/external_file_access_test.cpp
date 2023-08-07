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

#include "accesstoken_kit.h"
#include "context_impl.h"
#include "file_access_framework_errno.h"
#include "iservice_registry.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

#define private public
#include "file_access_helper.h"
#undef private

namespace {
using namespace std;
using namespace OHOS;
using namespace FileAccessFwk;
using json = nlohmann::json;
const int ABILITY_ID = 5003;
const int INIT_THREADS_NUMBER = 4;
const int ACTUAL_SUCCESS_THREADS_NUMBER = 1;
shared_ptr<FileAccessHelper> g_fah = nullptr;
int g_num = 0;
const int UID_TRANSFORM_TMP = 20000000;
const int UID_DEFAULT = 0;
shared_ptr<OHOS::AbilityRuntime::Context> g_context = nullptr;
const int FILE_COUNT_1 = 1;
const int FILE_COUNT_2 = 2;
const int FILE_COUNT_3 = 3;
const int FILE_COUNT_4 = 4;
const int FILE_COUNT_5 = 5;
const int FILE_COUNT_6 = 6;

void SetNativeToken()
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
    const uint64_t systemAppMask = (static_cast<uint64_t>(1) << 32);
    tokenId |= systemAppMask;
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    delete[] perms;
}

class FileExtensionHelperTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        cout << "FileExtensionHelperTest code test" << endl;
        SetNativeToken();
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
            GTEST_LOG_(ERROR) << "external_file_access_test g_fah is nullptr";
            exit(1);
        }
        setuid(UID_DEFAULT);
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
            result = g_fah->CreateFile(newDirUriTest1, "external_file_access_OpenFile_0000.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            int fd;
            result = g_fah->OpenFile(newFileUri, WRITE_READ, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "OpenFile_0000 result:" << result;
            close(fd);
            result = g_fah->Delete(newDirUriTest1);
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
        int result = g_fah->OpenFile(uri, WRITE_READ, fd);
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
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            result = g_fah->CreateFile(parentUri, "external_file_access_OpenFile_0002.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri uri("storage/media/100/local/files/Download/external_file_access_OpenFile_0002.txt");
            int fd;
            result = g_fah->OpenFile(uri, WRITE_READ, fd);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "OpenFile_0002 result:" << result;
            result = g_fah->Delete(newFileUri);
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
        int result = g_fah->OpenFile(uri, WRITE_READ, fd);
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
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            result = g_fah->CreateFile(parentUri, "external_file_access_OpenFile_0004.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            int fd;
            int flag = -1;
            result = g_fah->OpenFile(newFileUri, flag, fd);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "OpenFile_0004 result:" << result;
            result = g_fah->Delete(newFileUri);
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
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            result = g_fah->CreateFile(parentUri, "external_file_access_OpenFile_0005.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            int fd;
            result = g_fah->OpenFile(newFileUri, READ, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "OpenFile_0005 result:" << result;
            close(fd);
            result = g_fah->Delete(newFileUri);
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
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            result = g_fah->CreateFile(parentUri, "external_file_access_OpenFile_0006.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            int fd;
            result = g_fah->OpenFile(newFileUri, WRITE, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "OpenFile_0006 result:" << result;
            close(fd);
            result = g_fah->Delete(newFileUri);
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
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            result = g_fah->CreateFile(parentUri, "external_file_access_OpenFile_0007.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            int fd;
            result = g_fah->OpenFile(newFileUri, WRITE_READ, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "OpenFile_0007 result:" << result;
            close(fd);
            result = g_fah->Delete(newFileUri);
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
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            int fd;
            std::string displayName = "test1.txt";
            g_num = 0;
            result = g_fah->CreateFile(parentUri, displayName, newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            for (size_t j = 0; j < INIT_THREADS_NUMBER; j++) {
                std::thread execthread(OpenFileTdd, g_fah, newFileUri, WRITE_READ, fd);
                execthread.join();
            }
            EXPECT_EQ(g_num, INIT_THREADS_NUMBER);
            result = g_fah->Delete(newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_OpenFile_0008 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_OpenFile_0008";
}

static bool ReplaceBundleNameFromPath(std::string &path, const std::string &newName)
{
    Uri uri(path);
    std::string scheme = uri.GetScheme();
    if (scheme == FILE_SCHEME_NAME) {
        std::string curName = uri.GetAuthority();
        if (curName.empty()) {
            return false;
        }
        path.replace(path.find(curName), curName.length(), newName);
        return true;
    }

    std::string tPath = uri.GetPath();
    if (tPath.empty()) {
        GTEST_LOG_(INFO) << "Uri path error.";
        return false;
    }

    if (tPath.front() != '/') {
        GTEST_LOG_(INFO) << "Uri path format error.";
        return false;
    }

    auto index = tPath.substr(1).find_first_of("/");
    auto bundleName = tPath.substr(1, index);
    if (bundleName.empty()) {
        GTEST_LOG_(INFO) << "bundleName empty.";
        return false;
    }

    path.replace(path.find(bundleName), bundleName.length(), newName);
    return true;
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
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            std::string str = info[i].uri;
            if (!ReplaceBundleNameFromPath(str, "ohos.com.NotExistBundleName")) {
                GTEST_LOG_(ERROR) << "replace BundleName failed.";
                EXPECT_TRUE(false);
            }
            int fd;
            Uri newFileUri(str + "/NotExistFile.txt");
            result = g_fah->OpenFile(newFileUri, WRITE_READ, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::E_IPCS);
            GTEST_LOG_(INFO) << "OpenFile_0009 result:" << result;
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
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            GTEST_LOG_(INFO) << parentUri.ToString();
            Uri newDirUriTest1("");
            result = g_fah->Mkdir(parentUri, "测试目录", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newDirUriTest1.ToString().find("测试目录"), std::string::npos);
            Uri newFileUri("");
            result = g_fah->CreateFile(newDirUriTest1, "打开文件.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newFileUri.ToString().find("打开文件.txt"), std::string::npos);
            int fd;
            result = g_fah->OpenFile(newFileUri, WRITE_READ, fd);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "OpenFile_0010 result:" << result;
            close(fd);
            result = g_fah->Delete(newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_OpenFile_0010 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_OpenFile_0010";
}

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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_CreateFile_0000";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            result = g_fah->CreateFile(parentUri, "external_file_access_CreateFile_0000.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "CreateFile_0000 result:" << result;
            result = g_fah->Delete(newFileUri);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_CreateFile_0001";
    try {
        Uri newFileUri("");
        Uri parentUri("");
        int result = g_fah->CreateFile(parentUri, "external_file_access_CreateFile_0001.txt", newFileUri);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_CreateFile_0002";
    try {
        Uri newFileUri("");
        Uri parentUri("storage/media/100/local/files/Download");
        int result = g_fah->CreateFile(parentUri, "external_file_access_CreateFile_0002.txt", newFileUri);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_CreateFile_0002";
    try {
        Uri newFileUri("");
        Uri parentUri("~!@#$%^&*()_");
        int result = g_fah->CreateFile(parentUri, "external_file_access_CreateFile_0003.txt", newFileUri);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_CreateFile_0004";
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
            GTEST_LOG_(INFO) << "CreateFile_0004 result:" << result;
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_CreateFile_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_CreateFile_0004";
}

static void CreateFileTdd(shared_ptr<FileAccessHelper> fahs, Uri uri, std::string displayName, Uri newFile)
{
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_CreateFile_0005";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            std::string displayName = "test1.txt";
            g_num = 0;
            for (int j = 0; j < INIT_THREADS_NUMBER; j++) {
                std::thread execthread(CreateFileTdd, g_fah, parentUri, displayName, newFileUri);
                execthread.join();
            }
            EXPECT_EQ(g_num, ACTUAL_SUCCESS_THREADS_NUMBER);
            Uri newDelete(info[i].uri + "/" + displayName);
            result = g_fah->Delete(newDelete);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_CreateFile_0006";
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
            Uri newFileUri("");
            string displayName = "test.txt";
            result = g_fah->CreateFile(parentUri, displayName, newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::E_IPCS);
            GTEST_LOG_(INFO) << "CreateFile_0006 result:" << result;
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_CreateFile_0007";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newFileUri("");
            result = g_fah->CreateFile(parentUri, "新建文件.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newFileUri.ToString().find("新建文件.txt"), std::string::npos);
            GTEST_LOG_(INFO) << "CreateFile_0007 result:" << result;
            result = g_fah->Delete(newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_CreateFile_0007 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_CreateFile_0007";
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
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri newFileUri("");
            result = g_fah->CreateFile(newDirUriTest, "external_file_access_Delete_0000.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = g_fah->Delete(newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Delete_0000 result:" << result;
            result = g_fah->Delete(newDirUriTest);
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
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = g_fah->Delete(newDirUriTest);
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
        int result = g_fah->Delete(selectFileUri);
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
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            result = g_fah->Delete(newDirUriTest);
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
        int result = g_fah->Delete(selectFileUri);
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
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri newFileUri("");
            std::string displayName = "test1.txt";
            result = g_fah->CreateFile(newDirUriTest, displayName, newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            g_num = 0;
            for (int j = 0; j < INIT_THREADS_NUMBER; j++) {
                std::thread execthread(DeleteTdd, g_fah, newFileUri);
                execthread.join();
            }
            EXPECT_EQ(g_num, ACTUAL_SUCCESS_THREADS_NUMBER);
            result = g_fah->Delete(newDirUriTest);
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
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            std::string str = info[i].uri;
            if (!ReplaceBundleNameFromPath(str, "ohos.com.NotExistBundleName")) {
                GTEST_LOG_(ERROR) << "replace BundleName failed.";
                EXPECT_TRUE(false);
            }
            Uri selectFileUri(str);
            int result = g_fah->Delete(selectFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::E_IPCS);
            GTEST_LOG_(INFO) << "Delete_0006 result:" << result;
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
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "测试目录", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newDirUriTest.ToString().find("测试目录"), std::string::npos);
            Uri newFileUri("");
            result = g_fah->CreateFile(newDirUriTest, "删除文件.txt", newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newFileUri.ToString().find("删除文件.txt"), std::string::npos);
            result = g_fah->Delete(newFileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Delete_0007 result:" << result;
            result = g_fah->Delete(newDirUriTest);
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
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0000 result:" << result;
            result = g_fah->Delete(newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = g_fah->Delete(newDirUriTest2);
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
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0001 result:" << result;
            result = g_fah->Delete(newDirUriTest2);
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
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0002 result:" << result;
            result = g_fah->Delete(newDirUriTest);
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
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0003 result:" << result;
            result = g_fah->Delete(newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = g_fah->Delete(newDirUriTest2);
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
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0004 result:" << result;
            result = g_fah->Delete(newDirUriTest);
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
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0005 result:" << result;
            result = g_fah->Delete(newDirUriTest);
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
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0006 result:" << result;
            result = g_fah->Delete(newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = g_fah->Delete(newDirUriTest2);
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
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0007 result:" << result;
            result = g_fah->Delete(newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = g_fah->Delete(newDirUriTest2);
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
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0008 result:" << result;
            result = g_fah->Delete(newDirUriTest2);
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
            size_t fileNumbers = 2000;
            for (size_t j = 0; j < fileNumbers; j++) {
                string fileName = "test" + ToString(j) + ".txt";
                result = g_fah->CreateFile(newDirUriTest1, fileName, testUri);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            }
            Uri testUri2("");
            result = g_fah->Move(newDirUriTest1, newDirUriTest2, testUri2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0009 result:" << result;
            result = g_fah->Delete(newDirUriTest2);
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
            result = g_fah->Mkdir(newDirUriTest1, "test", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            size_t directoryNumbers = 64;
            for (size_t j = 0; j < directoryNumbers; j++) {
                result = g_fah->Mkdir(testUri, "test", testUri);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            }
            Uri testUri2("");
            result = g_fah->Move(newDirUriTest1, newDirUriTest2, testUri2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0010 result:" << result;
            result = g_fah->Delete(newDirUriTest2);
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
            result = g_fah->CreateFile(newDirUriTest1, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            for (size_t j = i + 1; j < info.size(); j++) {
                Uri otherUri(info[j].uri);
                result = g_fah->Mkdir(otherUri, "test2", newDirUriTest2);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
                result = g_fah->Move(testUri, newDirUriTest2, testUri);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
                result = g_fah->Move(testUri, newDirUriTest1, testUri);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
                GTEST_LOG_(INFO) << "Move_0011 result:" << result;
                result = g_fah->Delete(newDirUriTest2);
                EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            }
            result = g_fah->Delete(newDirUriTest1);
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
            std::string displayName = "test3.txt";
            result = g_fah->CreateFile(newDirUriTest1, displayName, testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri2("");
            g_num = 0;
            for (int j = 0; j < INIT_THREADS_NUMBER; j++) {
                std::thread execthread(MoveTdd, g_fah, testUri, newDirUriTest2, testUri2);
                execthread.join();
            }
            EXPECT_EQ(g_num, ACTUAL_SUCCESS_THREADS_NUMBER);
            result = g_fah->Delete(newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = g_fah->Delete(newDirUriTest2);
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
        result = g_fah->Mkdir(newDirUriTest1, "test", testUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string str = newDirUriTest1.ToString();
        if (!ReplaceBundleNameFromPath(str, "ohos.com.NotExistBundleName")) {
            GTEST_LOG_(ERROR) << "replace BundleName failed.";
            EXPECT_TRUE(false);
        }
        Uri uri(str);
        Uri testUri2("");
        result = g_fah->Move(uri, newDirUriTest2, testUri2);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        GTEST_LOG_(INFO) << "Move_0013 result:" << result;
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest2);
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
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = g_fah->Mkdir(parentUri, "测试目录1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newDirUriTest1.ToString().find("测试目录1"), std::string::npos);
            result = g_fah->Mkdir(parentUri, "测试目录2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newDirUriTest2.ToString().find("测试目录2"), std::string::npos);
            Uri testUri("");
            result = g_fah->CreateFile(newDirUriTest1, "移动文件.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(testUri.ToString().find("移动文件.txt"), std::string::npos);
            Uri testUri2("");
            result = g_fah->Move(testUri, newDirUriTest2, testUri2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0014 result:" << result;
            result = g_fah->Delete(newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = g_fah->Delete(newDirUriTest2);
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
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = g_fah->Mkdir(parentUri, "移动目录", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newDirUriTest1.ToString().find("移动目录"), std::string::npos);
            result = g_fah->Mkdir(parentUri, "test2", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri2("");
            result = g_fah->Move(newDirUriTest1, newDirUriTest2, testUri2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0015 result:" << result;
            result = g_fah->Delete(newDirUriTest2);
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
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest1("");
            Uri newDirUriTest2("");
            result = g_fah->Mkdir(parentUri, "test1", newDirUriTest1);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            result = g_fah->Mkdir(parentUri, "移动目录", newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri testUri2("");
            result = g_fah->Move(newDirUriTest1, newDirUriTest2, testUri2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Move_0016 result:" << result;
            result = g_fah->Delete(newDirUriTest2);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Move_0016 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Move_0016";
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Rename_0000";
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
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Rename_0000 result:" << result;
            result = g_fah->Delete(newDirUriTest);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Rename_0001";
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
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Rename_0001 result:" << result;
            result = g_fah->Delete(renameUri);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Rename_0002";
    try {
        Uri renameUri("");
        Uri sourceFileUri("");
        int result = g_fah->Rename(sourceFileUri, "testRename.txt", renameUri);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Rename_0003";
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
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Rename_0003 result:" << result;
            result = g_fah->Delete(newDirUriTest);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Rename_0004";
    try {
        Uri renameUri("");
        Uri sourceFileUri("~!@#$%^&*()_");
        int result = g_fah->Rename(sourceFileUri, "testRename.txt", renameUri);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Rename_0005";
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
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            GTEST_LOG_(INFO) << "Rename_0005 result:" << result;
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Rename_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Rename_0005";
}

static void RenameTdd(shared_ptr<FileAccessHelper> fahs, Uri sourceFile, std::string displayName, Uri newFile)
{
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Rename_0006";
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
            std::string displayName1 = "test1.txt";
            std::string displayName2 = "test2.txt";
            Uri renameUri("");
            result = g_fah->CreateFile(newDirUriTest, displayName1, testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            g_num = 0;
            for (int j = 0; j < INIT_THREADS_NUMBER; j++) {
                std::thread execthread(RenameTdd, g_fah, testUri, displayName2, renameUri);
                execthread.join();
            }
            EXPECT_EQ(g_num, ACTUAL_SUCCESS_THREADS_NUMBER);
            result = g_fah->Delete(newDirUriTest);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Rename_0007";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "test", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            std::string str = newDirUriTest.ToString();
            if (!ReplaceBundleNameFromPath(str, "ohos.com.NotExistBundleName")) {
                GTEST_LOG_(ERROR) << "replace BundleName failed.";
                EXPECT_TRUE(false);
            }
            Uri testUri(str);
            Uri renameUri("");
            result = g_fah->Rename(testUri, "test.txt", renameUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::E_IPCS);
            GTEST_LOG_(INFO) << "Rename_0007 result:" << result;
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Rename_0008";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "测试目录", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newDirUriTest.ToString().find("测试目录"), std::string::npos);
            Uri testUri("");
            result = g_fah->CreateFile(newDirUriTest, "test.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            Uri renameUri("");
            result = g_fah->Rename(testUri, "测试文件.txt", renameUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(renameUri.ToString().find("测试文件.txt"), std::string::npos);
            GTEST_LOG_(INFO) << "Rename_0008 result:" << result;
            result = g_fah->Delete(newDirUriTest);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_Rename_0009";
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
            result = g_fah->Rename(newDirUriTest, "重命名目录", renameUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(renameUri.ToString().find("重命名目录"), std::string::npos);
            GTEST_LOG_(INFO) << "Rename_0009 result:" << result;
            result = g_fah->Delete(renameUri);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0000";
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
            result = g_fah->CreateFile(newDirUriTest, "external_file_access_ListFile_0000.txt", testUri);
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
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0001";
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
        EXPECT_EQ(fileInfoVec.size(), OHOS::FileAccessFwk::ERR_OK);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0002";
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
            EXPECT_EQ(fileInfoVec.size(), OHOS::FileAccessFwk::ERR_OK);
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0003";
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
        EXPECT_EQ(fileInfoVec.size(), OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ListFile_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ListFile_0003";
}

static void ListFileTdd(FileInfo fileInfo, int offset, int maxCount, FileFilter filter,
    std::vector<FileInfo> fileInfoVec)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFileTdd";
    int ret = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0004";
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
            result = g_fah->CreateFile(newDirUriTest, "external_file_access_ListFile_0004.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            FileInfo fileInfo;
            fileInfo.uri = newDirUriTest.ToString();
            int offset = 0;
            int maxCount = 1000;
            std::vector<FileInfo> fileInfoVec;
            FileFilter filter;
            g_num = 0;
            for (int j = 0; j < INIT_THREADS_NUMBER; j++) {
                std::thread execthread(ListFileTdd, fileInfo, offset, maxCount, filter, fileInfoVec);
                execthread.join();
            }
            EXPECT_EQ(g_num, INIT_THREADS_NUMBER);
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0005";
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

            std::string str = testUri.ToString();
            if (!ReplaceBundleNameFromPath(str, "ohos.com.NotExistBundleName")) {
                GTEST_LOG_(ERROR) << "replace BundleName failed.";
                EXPECT_TRUE(false);
            }
            FileInfo fileInfo;
            fileInfo.uri = str;
            int64_t offset = 0;
            int64_t maxCount = 1000;
            std::vector<FileInfo> fileInfoVec;
            FileFilter filter;
            result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
            EXPECT_EQ(result, OHOS::FileAccessFwk::E_IPCS);
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0006";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "测试目录", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(newDirUriTest.ToString().find("测试目录"), std::string::npos);
            Uri testUri("");
            result = g_fah->CreateFile(newDirUriTest, "测试文件.txt", testUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(testUri.ToString().find("测试文件.txt"), std::string::npos);
            FileInfo fileInfo;
            fileInfo.uri = newDirUriTest.ToString();
            int64_t offset = 0;
            int64_t maxCount = 1000;
            std::vector<FileInfo> fileInfoVec;
            FileFilter filter;
            result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_GT(fileInfoVec.size(), OHOS::FileAccessFwk::ERR_OK);
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ListFile_0006 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ListFile_0006";
}


static void WriteData(Uri &uri)
{
    int fd = -1;
    std::string buff = "extenal_file_access_test";
    int result = g_fah->OpenFile(uri, WRITE_READ, fd);
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

static double InitListFile(Uri newDirUriTest, const std::string &caseNumber, const bool &needSleep = false)
{
    Uri testUri1("");
    int result = g_fah->CreateFile(newDirUriTest, "external_file_access_ListFile_" + caseNumber + ".txt", testUri1);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri testUri2("");
    result = g_fah->CreateFile(newDirUriTest, "external_file_access_ListFile_" + caseNumber + ".docx", testUri2);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri testUri3("");
    double time = GetTime();
    if (needSleep) {
      sleep(2);
    }
    result = g_fah->CreateFile(newDirUriTest, "external_file_access_ListFile_01_" + caseNumber + ".txt", testUri3);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri testUri4("");
    result = g_fah->CreateFile(newDirUriTest, "external_file_access_ListFile_01_" + caseNumber +  ".docx", testUri4);
    WriteData(testUri4);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri testUri5("");
    result = g_fah->CreateFile(newDirUriTest, "external_file_access_ListFile_01_" + caseNumber + "_01.docx", testUri5);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    return time;
}

static void ListFileFilter7(Uri newDirUriTest)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 4;
    int64_t maxCount = 1;
    std::vector<FileInfo> fileInfoVec;
    FileFilter filter({".txt", ".docx"}, {}, {}, -1, -1, false, true);
    int result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_1);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0007";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "listfile007", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            InitListFile(newDirUriTest, "0007");
            ListFileFilter7(newDirUriTest);
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ListFile_0007 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ListFile_0007";
}

static double InitListFileFolder(Uri newDirUriTest, const std::string &caseNumber, const bool &needSleep = false)
{
    double time = InitListFile(newDirUriTest, caseNumber, needSleep);
    Uri folderUri("");
    int result = g_fah->Mkdir(newDirUriTest, "test" + caseNumber, folderUri);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri testUri6("");
    result = g_fah->CreateFile(folderUri, "external_file_access_ListFile_01_" + caseNumber + "_02.docx", testUri6);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    return time;
}

static void ShowInfo(const std::vector<FileInfo> &fileInfoVec, const std::string &caseNumber)
{
    for (auto fileInfo : fileInfoVec) {
        GTEST_LOG_(INFO) << caseNumber << ", uri:" << fileInfo.uri << endl;
    }
}

static void ListFileFilter8(Uri newDirUriTest)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 0;
    int64_t maxCount = 1000;
    std::vector<FileInfo> fileInfoVec;
    FileFilter filter({}, {}, {}, -1, 0, false, true);
    int result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_6);
    ShowInfo(fileInfoVec, "external_file_access_ListFile_0008");
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0008";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "listfile008", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            InitListFileFolder(newDirUriTest, "0008");
            ListFileFilter8(newDirUriTest);
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ListFile_0008 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ListFile_0008";
}

static void ListFileFilter9(Uri newDirUriTest)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 0;
    int64_t maxCount = 1000;
    std::vector<FileInfo> fileInfoVec;
    FileFilter filter;
    int result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_6);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0008";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "listfile009", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            InitListFileFolder(newDirUriTest, "0009");
            ListFileFilter9(newDirUriTest);
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ListFile_0009 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ListFile_0009";
}

static void ListFileFilter10(Uri newDirUriTest, const double &time)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 0;
    int64_t maxCount = 1000;
    std::vector<FileInfo> fileInfoVec;
    FileFilter filter({".txt", ".docx"}, {}, {}, -1, -1, false, true);
    int result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_5);

    FileFilter filter1({".txt", ".docx"}, {"0010.txt", "0010.docx"}, {}, -1, -1, false, true);
    result = g_fah->ListFile(fileInfo, offset, maxCount, filter1, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_4);

    FileFilter filter2({".txt", ".docx"}, {"0010.txt", "0010.docx"}, {}, 0, 0, false, true);
    result = g_fah->ListFile(fileInfo, offset, maxCount, filter2, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_1);

    FileFilter filter3({".txt", ".docx"}, {"0010.txt", "0010.docx"}, {}, -1, time, false, true);
    result = g_fah->ListFile(fileInfo, offset, maxCount, filter3, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_2);

    double nowTime = GetTime();
    FileFilter filter4({".txt", ".docx"}, {"0010.txt", "0010.docx"}, {}, -1, nowTime, false, true);
    result = g_fah->ListFile(fileInfo, offset, maxCount, filter4, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), 0);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0010";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "listfile0010", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            double time = InitListFile(newDirUriTest, "0010", true);
            ListFileFilter10(newDirUriTest, time);
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ListFile_0010 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ListFile_0010";
}

static void ListFileFilter11(Uri newDirUriTest, const double &time)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 0;
    int64_t maxCount = 1000;
    std::vector<FileInfo> fileInfoVec;
    FileFilter filter({".txt", ".docx"}, {}, {}, -1, -1, false, true);
    int result = g_fah->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_6);

    FileFilter filter1({".txt", ".docx"}, {"测试.txt", "测试.docx"}, {}, -1, -1, false, true);
    result = g_fah->ListFile(fileInfo, offset, maxCount, filter1, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_4);

    FileFilter filter2({".txt", ".docx"}, {"测试.txt", "测试.docx"}, {}, 0, 0, false, true);
    result = g_fah->ListFile(fileInfo, offset, maxCount, filter2, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_1);

    FileFilter filter3({".txt", ".docx"}, {"测试.txt", "测试.docx"}, {}, -1, time, false, true);
    result = g_fah->ListFile(fileInfo, offset, maxCount, filter3, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_2);

    double nowTime = GetTime();
    FileFilter filter4({".txt", ".docx"}, {"测试.txt", "测试.docx"}, {}, -1, nowTime, false, true);
    result = g_fah->ListFile(fileInfo, offset, maxCount, filter4, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), 0);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ListFile_0011";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "listfile测试", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            double time = InitListFileFolder(newDirUriTest, "测试", true);
            ListFileFilter11(newDirUriTest, time);
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ListFile_0011 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ListFile_0011";
}

static double InitScanFile(Uri newDirUriTest, const std::string &caseNumber, const bool &needSleep = false)
{
    Uri forlderUriTest("");
    int result = g_fah->Mkdir(newDirUriTest, "test" + caseNumber, forlderUriTest);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

    Uri testUri1("");
    result = g_fah->CreateFile(newDirUriTest, "external_file_access_ScanFile_" + caseNumber + ".txt", testUri1);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri testUri2("");
    result = g_fah->CreateFile(newDirUriTest, "external_file_access_ScanFile_" + caseNumber + ".docx", testUri2);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    double time = GetTime();
    if (needSleep) {
      sleep(2);
    }
    Uri testUri3("");
    result = g_fah->CreateFile(forlderUriTest, "external_file_access_ScanFile_01_" + caseNumber + ".txt", testUri3);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri testUri4("");
    result = g_fah->CreateFile(forlderUriTest, "external_file_access_ScanFile_01_" + caseNumber + ".docx", testUri4);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    WriteData(testUri4);
    Uri testUri5("");
    result = g_fah->CreateFile(forlderUriTest, "external_file_access_ScanFile_01_" + caseNumber + "_01.docx", testUri5);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    return time;
}

static void ScanFileFilter0(Uri newDirUriTest, const double &time)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 0;
    int64_t maxCount = 1000;
    std::vector<FileInfo> fileInfoVec;
    FileFilter filter({".txt", ".docx"}, {}, {}, -1, -1, false, true);
    int result = g_fah->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_5);

    FileFilter filter1({".txt", ".docx"}, {"0000.txt", "0000.docx"}, {}, -1, -1, false, true);
    result = g_fah->ScanFile(fileInfo, offset, maxCount, filter1, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_4);

    FileFilter filter2({".txt", ".docx"}, {"0000.txt", "0000.docx"}, {}, 0, 0, false, true);
    result = g_fah->ScanFile(fileInfo, offset, maxCount, filter2, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_1);

    FileFilter filter3({".txt", ".docx"}, {"0000.txt", "0000.docx"}, {}, -1, time, false, true);
    result = g_fah->ScanFile(fileInfo, offset, maxCount, filter3, fileInfoVec);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    EXPECT_EQ(fileInfoVec.size(), FILE_COUNT_2);

    double nowTime = GetTime();
    FileFilter filter4({".txt", ".docx"}, {"0000.txt", "0000.docx"}, {}, -1, nowTime, false, true);
    result = g_fah->ScanFile(fileInfo, offset, maxCount, filter4, fileInfoVec);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ScanFile_0000";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "scanfile0000", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            double time = InitScanFile(newDirUriTest, "0000", true);
            ScanFileFilter0(newDirUriTest, time);
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ScanFile_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ScanFile_0000";
}

static void ScanFileFilter1(Uri newDirUriTest)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 0;
    int64_t maxCount = 1000;
    std::vector<FileInfo> fileInfoVec;
    FileFilter filter;
    int result = g_fah->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ScanFile_0001";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "scanfile0001", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            InitScanFile(newDirUriTest, "0001");
            ScanFileFilter1(newDirUriTest);
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ScanFile_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ScanFile_0001";
}

static void ScanFileFilter2(Uri newDirUriTest)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 3;
    int64_t maxCount = 3;
    std::vector<FileInfo> fileInfoVec;
    FileFilter filter({".txt", ".docx"}, {}, {}, -1, -1, false, true);
    int result = g_fah->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ScanFile_0002";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "scanfile0002", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            InitScanFile(newDirUriTest, "0002");
            ScanFileFilter2(newDirUriTest);
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ScanFile_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ScanFile_0002";
}

static void ScanFileFilter3(Uri newDirUriTest, const double &time)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 0;
    int64_t maxCount = 1000;
    std::vector<FileInfo> fileInfoVec;
    FileFilter filter({}, {}, {}, -1, time, false, true);
    int result = g_fah->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_ScanFile_0003";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "scanfile0003", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            double time = InitScanFile(newDirUriTest, "0003", true);
            ScanFileFilter3(newDirUriTest, time);
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_ScanFile_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_ScanFile_0003";
}

static void ScanFileFilter4(Uri newDirUriTest)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 0;
    int64_t maxCount = 1000;
    std::vector<FileInfo> fileInfoVec;
    FileFilter filter;
    int result = g_fah->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin user_file_service_external_file_access_ScanFile_0004";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "scanfile0004", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            InitScanFile(newDirUriTest, "0004");
            ScanFileFilter4(newDirUriTest);
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "user_file_service_external_file_access_ScanFile_0004 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end user_file_service_external_file_access_ScanFile_0004";
}

static void ScanFileFilter5(Uri newDirUriTest)
{
    FileInfo fileInfo;
    fileInfo.uri = newDirUriTest.ToString();
    int64_t offset = 0;
    int64_t maxCount = 1000;
    std::vector<FileInfo> fileInfoVec;
    FileFilter filter({".txt", ".docx"}, {"测试.txt", "测试.docx"}, {}, -1, -1, false, true);
    int result = g_fah->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin user_file_service_external_file_access_ScanFile_0005";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "scanfile测试", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            InitScanFile(newDirUriTest, "测试");
            ScanFileFilter5(newDirUriTest);
            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "user_file_service_external_file_access_ScanFile_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end user_file_service_external_file_access_ScanFile_0005";
}

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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_GetFileInfoFromUri_0000";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            FileInfo fileinfo;
            result = g_fah->GetFileInfoFromUri(parentUri, fileinfo);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            int64_t offset = 0;
            int64_t maxCount = 1000;
            FileFilter filter;
            std::vector<FileInfo> fileInfoVecTemp;
            result = g_fah->ListFile(fileinfo, offset, maxCount, filter, fileInfoVecTemp);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_GE(fileInfoVecTemp.size(), OHOS::FileAccessFwk::ERR_OK);
        }
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_GetFileInfoFromUri_0001";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        for (size_t i = 0; i < info.size(); i++) {
            Uri parentUri(info[i].uri);
            Uri newDirUriTest("");
            result = g_fah->Mkdir(parentUri, "testDir", newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            FileInfo dirInfo;
            result = g_fah->GetFileInfoFromUri(newDirUriTest, dirInfo);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

            int64_t offset = 0;
            int64_t maxCount = 1000;
            FileFilter filter;
            std::vector<FileInfo> fileInfoVec;
            result = g_fah->ListFile(dirInfo, offset, maxCount, filter, fileInfoVec);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_GE(fileInfoVec.size(), OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_GetFileInfoFromUri_0002";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
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
            int64_t maxCount = 1000;
            FileFilter filter;
            std::vector<FileInfo> fileInfoVecTemp;
            result = g_fah->ListFile(fileinfo, offset, maxCount, filter, fileInfoVecTemp);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(fileInfoVecTemp.size(), OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_GetFileInfoFromUri_0006";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
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
            int64_t maxCount = 1000;
            FileFilter filter;
            std::vector<FileInfo> fileInfoVec;
            result = g_fah->ListFile(dirInfo, offset, maxCount, filter, fileInfoVec);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_GE(fileInfoVec.size(), OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
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
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_GetFileInfoFromUri_0007";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
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
            int64_t maxCount = 1000;
            FileFilter filter;
            std::vector<FileInfo> fileInfoVecTemp;
            result = g_fah->ListFile(fileinfo, offset, maxCount, filter, fileInfoVecTemp);
            EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
            EXPECT_EQ(fileInfoVecTemp.size(), OHOS::FileAccessFwk::ERR_OK);

            result = g_fah->Delete(newDirUriTest);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_GetFileInfoFromUri_0007 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_GetFileInfoFromUri_0007";
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
        std::pair<shared_ptr<FileAccessHelper>, int> helper = FileAccessHelper::Creator(g_context);
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
        std::pair<shared_ptr<FileAccessHelper>, int> helper = FileAccessHelper::Creator(g_context, wants);
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
        std::pair<shared_ptr<FileAccessHelper>, int> helper = FileAccessHelper::Creator(g_context, wants);
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
        std::pair<shared_ptr<FileAccessHelper>, int> helper = FileAccessHelper::Creator(g_context, wants);
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
        shared_ptr<FileAccessHelper> helper = FileAccessHelper::Creator(g_context->GetToken(), wants);
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
        shared_ptr<FileAccessHelper> helper = FileAccessHelper::Creator(g_context->GetToken(), wants);
        ASSERT_TRUE(helper == nullptr);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_creator_0008 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_creator_0008";
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
 * @tc.number: user_file_service_external_file_access_GetKeyOfWants_0000
 * @tc.name: external_file_access_GetKeyOfWants_0000
 * @tc.desc: Test function of GetKeyOfWants interface.
 * @tc.desc: helper invoke GetKeyOfWants for FAILED.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionHelperTest, external_file_access_GetKeyOfWants_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin external_file_access_GetKeyOfWants_0000";
    try {
        AAFwk::Want want;
        want.SetElementName("NotExistBundleName", "NotExistAbilityName");
        std::string key = g_fah->GetKeyOfWants(want);
        ASSERT_TRUE(key.empty());
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_GetKeyOfWants_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_GetKeyOfWants_0000";
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
        std::string relativePath = "/storage/Users/currentUserId/QueryTest1";
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
        result = g_fah->Query(newDirUriTest1, testJsonString);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        auto jsonObject = json::parse(testJsonString);
        EXPECT_EQ(jsonObject.at(DISPLAY_NAME), displayName);
        EXPECT_EQ(jsonObject.at(FILE_SIZE), buff.size() * 2);
        EXPECT_EQ(jsonObject.at(RELATIVE_PATH), relativePath);
        ASSERT_TRUE(jsonObject.at(DATE_MODIFIED) > 0);
        ASSERT_TRUE(jsonObject.at(DATE_ADDED) > 0);
        GTEST_LOG_(INFO) << " result" << testJsonString;
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
        std::string relativePath = "/storage/Users/currentUserId/QueryTest3/external_file_access_Query_0001.txt";
        result = g_fah->CreateFile(newDirUriTest, displayName, newFileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        int fd = -1;
        result = g_fah->OpenFile(newFileUri, WRITE_READ, fd);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        std::string buff = "query test";
        ssize_t fileSize = write(fd, buff.c_str(), buff.size());
        close(fd);
        EXPECT_EQ(fileSize, buff.size());
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
        result = g_fah->Query(newFileUri, testJsonString);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        auto jsonObject = json::parse(testJsonString);
        EXPECT_EQ(jsonObject.at(DISPLAY_NAME), displayName);
        EXPECT_EQ(jsonObject.at(FILE_SIZE), buff.size());
        EXPECT_EQ(jsonObject.at(RELATIVE_PATH), relativePath);
        ASSERT_TRUE(jsonObject.at(DATE_MODIFIED) > 0);
        ASSERT_TRUE(jsonObject.at(DATE_ADDED) > 0);
        GTEST_LOG_(INFO) << " result" << testJsonString;
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
        std::string relativePath = "/storage/Users/currentUserId/查询目录1";
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
        result = g_fah->Query(newDirUriTest1, testJsonString);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        auto jsonObject = json::parse(testJsonString);
        EXPECT_EQ(jsonObject.at(DISPLAY_NAME), displayName);
        EXPECT_EQ(jsonObject.at(FILE_SIZE), buff.size() * 2);
        EXPECT_EQ(jsonObject.at(RELATIVE_PATH), relativePath);
        ASSERT_TRUE(jsonObject.at(DATE_MODIFIED) > 0);
        ASSERT_TRUE(jsonObject.at(DATE_ADDED) > 0);
        GTEST_LOG_(INFO) << " result" << testJsonString;
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_Query_0007 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-end external_file_access_Query_0007";
}
} // namespace