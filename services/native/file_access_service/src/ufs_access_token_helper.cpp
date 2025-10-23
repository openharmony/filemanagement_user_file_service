/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "accesstoken_kit.h"
#include "ufs_access_token_helper.h"
#include "ipc_skeleton.h"
#include "tokenid_kit.h"
#include "uri.h"
#include "hilog_wrapper.h"
#include "want.h"
#include "file_access_helper.h"
#ifdef SANDBOX_MANAGER
#include "sandbox_manager_kit.h"
#endif

namespace OHOS::FileAccessFwk {
using namespace std;
using namespace Security::AccessToken;
#ifdef SANDBOX_MANAGER
using namespace AccessControl::SandboxManager;
#endif
constexpr int32_t BASE_USER_RANGE = 200000;
const std::unordered_map<std::string, std::string> g_sandboxPathMap = {
    {"/storage/Users/currentUser", "/data/service/el2/<currentUserId>/hmdfs/account/files/Docs"},
};

typedef enum OperationMode {
    READ_MODE = 1 << 0,
    WRITE_MODE = 1 << 1,
    CREATE_MODE = 1 << 2,
    DELETE_MODE = 1 << 3,
    RENAME_MODE = 1 << 4,
} OperationMode;

bool UfsAccessTokenHelper::CheckCallerPermission(const std::string &permissionName)
{
    auto tokenId = IPCSkeleton::GetCallingTokenID();
    auto tokenType = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (tokenType == TOKEN_HAP || tokenType == TOKEN_NATIVE) {
        bool isGranted = CheckPermission(tokenId, permissionName);
        if (!isGranted) {
            HILOG_ERROR("Token Type is %{public}d", tokenType);
        }
        return isGranted;
    } else {
        HILOG_ERROR("Unsupported token type:%{public}d", tokenType);
        return false;
    }
}

bool UfsAccessTokenHelper::CheckPermission(uint32_t tokenId, const std::string &permissionName)
{
    int32_t ret = AccessTokenKit::VerifyAccessToken(tokenId, permissionName);
    if (ret == PermissionState::PERMISSION_DENIED) {
        HILOG_ERROR("permission %{private}s: PERMISSION_DENIED", permissionName.c_str());
        return false;
    }
    return true;
}

int32_t UfsAccessTokenHelper::GetCallerBundleNameAndIndex(std::string &bundleName, int &index)
{
    auto tokenId = IPCSkeleton::GetCallingTokenID();
    HILOG_INFO("UfsAccessTokenHelper::GetCallerBundleName");
    return GetBundleNameByToken(tokenId, bundleName, index);
}

int32_t UfsAccessTokenHelper::GetBundleNameByToken(uint32_t tokenId, std::string &bundleName, int &index)
{
    HILOG_INFO("UfsAccessTokenHelper::GetBundleNameByToken");
    int32_t tokenType = AccessTokenKit::GetTokenTypeFlag(tokenId);
    HILOG_INFO("UfsAccessTokenHelper::GetTokenTypeFlag");
    switch (tokenType) {
        case TOKEN_HAP: {
            HapTokenInfo hapInfo;
            if (AccessTokenKit::GetHapTokenInfo(tokenId, hapInfo) != 0) {
                HILOG_ERROR("UfsAccessTokenHelper::[Permission Check] get hap token info fail");
                return -1;
            }
            if (hapInfo.instIndex != 0) {
                HILOG_ERROR("UfsAccessTokenHelper::[Permission Check] APP twin is not supported.");
                break;
            }
            bundleName = hapInfo.bundleName;
            index = hapInfo.instIndex;
            break;
        }
        case TOKEN_NATIVE:
        // fall-through
        case TOKEN_SHELL: {
            NativeTokenInfo tokenInfo;
            if (AccessTokenKit::GetNativeTokenInfo(tokenId, tokenInfo) != 0) {
                HILOG_ERROR("UfsAccessTokenHelper::[Permission Check] get native token info fail");
                return -1;
            }
            bundleName = tokenInfo.processName;
            break;
        }
        default: {
            HILOG_ERROR("UfsAccessTokenHelper::[Permission Check] token type not match");
            return -1;
        }
    }
    if (bundleName.empty()) {
        HILOG_ERROR("UfsAccessTokenHelper::[Permission Check] package name is empty");
        return -1;
    }
    return ERR_OK;
}

int32_t UfsAccessTokenHelper::GetUserId()
{
    auto uid = IPCSkeleton::GetCallingUid();
    return uid / BASE_USER_RANGE;
}
std::string UfsAccessTokenHelper::ReplacePlaceholders(const std::string& path, const std::string& userId)
{
    std::string result = path;

    size_t pos;
    int replaceCount = 15;
    while ((pos = result.find("<currentUserId>")) != std::string::npos) {
        result.replace(pos, replaceCount, userId);
    }
    return result;
}
bool UfsAccessTokenHelper::PathToPhysicalPath(const std::string& path, const std::string& userId,
    std::string& physicalPath)
{
    std::string sandboxPath = path;
    std::unordered_map<std::string, std::string> pathMap = g_sandboxPathMap;
    std::string matchedPrefix;
    std::string remainingPath;
    for (const auto& [prefix, srcPath] : pathMap) {
        if (sandboxPath.find(prefix) == 0 && prefix.length() > matchedPrefix.length()) {
            matchedPrefix = prefix;
            remainingPath = sandboxPath.substr(prefix.length());
        }
    }
    if (matchedPrefix.empty()) {
        return false;
    }
    std::string srcPath = pathMap[matchedPrefix];
    srcPath = ReplacePlaceholders(srcPath, userId);
    physicalPath = srcPath + remainingPath;
    return true;
}

bool UfsAccessTokenHelper::CheckUriPersistentPermission(const std::string& path)
{
#ifdef SANDBOX_MANAGER
    std::vector<AccessControl::SandboxManager::PolicyInfo> uriPolicies;
    auto tokenId = IPCSkeleton::GetCallingTokenID();
    AccessControl::SandboxManager::PolicyInfo uriPolicy { .path = path, .mode =
        (OperationMode::READ_MODE | OperationMode::WRITE_MODE) };
    uriPolicies.emplace_back(uriPolicy);
    std::vector<bool> persistErrorResults;
    std::vector<bool> errorResults;
    auto persistCheckRet = SandboxManagerKit::CheckPersistPolicy(tokenId, uriPolicies, persistErrorResults);
    if (persistCheckRet == ERR_OK && persistErrorResults[0] == true) {
        HILOG_INFO("Check path persist permission success");
        return true;
    }
    auto tmpCheckRet = SandboxManagerKit::CheckPolicy(tokenId, uriPolicies, errorResults);
    if (tmpCheckRet == ERR_OK && errorResults[0] == true) {
        HILOG_INFO("Check path tmp permission success");
        return true;
    }
    HILOG_ERROR("Check path Permission failed ,persistCheckRet : %{public}d,
        tmpCheckRet : %{public}d" , persistCheckRet, tmpCheckRet);
    return false;
#endif
    return false;
}
} // namespace OHOS::FileManagement