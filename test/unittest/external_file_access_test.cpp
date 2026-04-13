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
static std::string g_skipReason;

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
    g_skipReason.clear();
    SetNativeToken();
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        g_skipReason = "SystemAbilityManager is nullptr";
        GTEST_LOG_(WARNING) << g_skipReason;
        return;
    }
    auto systemAbilityObj = saManager->GetSystemAbility(ABILITY_ID);
    g_context = make_shared<OHOS::AbilityRuntime::ContextImpl>();
    g_context->SetToken(systemAbilityObj);
    AAFwk::Want want;
    vector<AAFwk::Want> wantVec;
    int ret = FileAccessHelper::GetRegisteredFileAccessExtAbilityInfo(wantVec);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        g_skipReason = "GetRegisteredFileAccessExtAbilityInfo failed, ret=" + std::to_string(ret);
        GTEST_LOG_(WARNING) << g_skipReason;
        g_context = nullptr;
        return;
    }
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
    if (!isFound) {
        g_skipReason = "ExternalFileManager FileExtensionAbility is not registered";
        GTEST_LOG_(WARNING) << g_skipReason;
        g_context = nullptr;
        return;
    }
    vector<AAFwk::Want> wants{want};
    g_fah = FileAccessHelper::Creator(systemAbilityObj, wants);
    if (g_fah == nullptr) {
        g_skipReason = "FileAccessHelper::Creator returned nullptr";
        GTEST_LOG_(WARNING) << g_skipReason;
        g_context = nullptr;
    }
}
void FileExtensionHelperTest::TearDownTestCase()
{
    g_fah = nullptr;
    g_context = nullptr;
    g_skipReason.clear();
}

void FileExtensionHelperTest::SetUp()
{
    if (g_fah == nullptr || g_context == nullptr) {
        GTEST_SKIP() << (g_skipReason.empty() ? "External file access extension is unavailable" : g_skipReason);
    }
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
