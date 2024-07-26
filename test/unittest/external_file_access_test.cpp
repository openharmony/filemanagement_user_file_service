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
static shared_ptr<FileAccessHelper> g_fah = nullptr;
static shared_ptr<OHOS::AbilityRuntime::Context> g_context = nullptr;

void SetNativeToken()
{
    uint64_t tokenId;
    const char *perms[] = {
        "ohos.permission.FILE_ACCESS_MANAGER",
        "ohos.permission.GET_BUNDLE_INFO_PRIVILEGED",
        "ohos.permission.CONNECT_FILE_ACCESS_EXTENSION"
    };
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 3,
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
}

void FileExtensionHelperTest::SetUpTestCase()
{
    cout << "FileExtensionHelperTest code test" << endl;
    SetNativeToken();
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        return;
    }
    auto systemAbilityObj = saManager->GetSystemAbility(ABILITY_ID);
    g_context = make_shared<OHOS::AbilityRuntime::ContextImpl>();
    g_context->SetToken(systemAbilityObj);
    AAFwk::Want want;
    vector<AAFwk::Want> wantVec;
    setuid(UID_TRANSFORM_TMP);
    int ret = FileAccessHelper::GetRegisteredFileAccessExtAbilityInfo(wantVec);
    EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
    bool isFound = false;
    for (size_t i = 0; i < wantVec.size(); i++) {
        auto element = wantVec[i].GetElement();
        if (element.GetBundleName() == "com.ohos.UserFile.ExternalFileManager" &&
            element.GetAbilityName() == "FileExtensionAbility") {
            want = wantVec[i];
            isFound = true;
            break;
        }
    }
    EXPECT_TRUE(isFound);
    vector<AAFwk::Want> wants{want};
    g_fah = FileAccessHelper::Creator(systemAbilityObj, wants);
    if (g_fah == nullptr) {
        GTEST_LOG_(ERROR) << "external_file_access_test g_fah is nullptr";
        exit(1);
    }
    setuid(UID_DEFAULT);
}
void FileExtensionHelperTest::TearDownTestCase()
{
    g_fah = nullptr;
    g_context = nullptr;
}

void FileExtensionHelperTest::SetUp()
{
}

void FileExtensionHelperTest::TearDown()
{
}

shared_ptr<FileAccessHelper> FileExtensionHelperTest::GetFileAccessHelper()
{
    return g_fah;
}

shared_ptr<OHOS::AbilityRuntime::Context> FileExtensionHelperTest::GetContext()
{
    return g_context;
}

bool FileExtensionHelperTest::ReplaceBundleNameFromPath(std::string &path, const std::string &newName)
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

bool FileExtensionHelperTest::ReplaceBundleName(std::string& str, const std::string& newBundleName)
{
    if (!ReplaceBundleNameFromPath(str, newBundleName)) {
        GTEST_LOG_(ERROR) << "replace BundleName failed.";
        return false;
    }
    return true;
}
}