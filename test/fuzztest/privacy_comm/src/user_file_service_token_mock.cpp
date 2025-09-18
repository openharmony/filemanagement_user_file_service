/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "user_file_service_token_mock.h"

#include <cinttypes>
#include <sstream>
#include "hilog_wrapper.h"

using namespace OHOS::Security::AccessToken;

namespace OHOS {
namespace FileAccessFwk {
const std::string FILE_ACCESS_PERMISSION = "ohos.permission.FILE_ACCESS_MANAGER";
const std::string GET_BUNDLE_INFO_PRIVILEGED = "ohos.permission.GET_BUNDLE_INFO_PRIVILEGED";
const std::string ACCESS_CLOUD_DISK_INFO_PERMISSION = "ohos.permission.ACCESS_CLOUD_DISK_INFO";
const unsigned char FILES[] = {0x63, 0x6f, 0x6d, 0x2e, 0x68, 0x75, 0x61, 0x77, 0x65, 0x69,
    0x2e, 0x68, 0x6d, 0x6f, 0x73, 0x2e, 0x66, 0x69, 0x6c, 0x65,
    0x6d, 0x61, 0x6e, 0x61, 0x67, 0x65, 0x72, 0x00};

const unsigned char FILEMANAGER[] = {0x63, 0x6f, 0x6d, 0x2e, 0x68, 0x75, 0x61, 0x77, 0x65, 0x69,
    0x2e, 0x68, 0x6d, 0x6f, 0x73, 0x2e, 0x66, 0x69, 0x6c, 0x65,
    0x73, 0x00};

std::vector<std::string> requiredPermissions = { FILE_ACCESS_PERMISSION,
    GET_BUNDLE_INFO_PRIVILEGED, ACCESS_CLOUD_DISK_INFO_PERMISSION };
// 获取Native进程tokenID
static AccessTokenID GetNativeTokenIdFromProcess(const std::string &process)
{
    std::string dumpInfo;
    AtmToolsParamInfo info;
    info.processName = process;
    AccessTokenKit::DumpTokenInfo(info, dumpInfo);
    size_t pos = dumpInfo.find("\"tokenID\": ");
    if (pos == std::string::npos) {
        HILOG_ERROR("GetNativeTokenIdFromProcess: dumpInfo is invalid.");
        return 0;
    }
    pos += std::string("\"tokenID\": ").length();
    std::string numStr;
    while (pos < dumpInfo.length() && std::isdigit(dumpInfo[pos])) {
        numStr += dumpInfo[pos];
        ++pos;
    }
    std::istringstream iss(numStr);
    AccessTokenID tokenID;
    iss >> tokenID;
    return tokenID;
}

UserFileServiceTokenMock::UserFileServiceTokenMock()
{
    selfTokenId_ = GetSelfTokenID();
    HILOG_INFO("UserFileServiceTokenMock: selfTokenId_ = 0x%{public}" PRIx64 "", selfTokenId_);
}

UserFileServiceTokenMock::~UserFileServiceTokenMock()
{
    if (SetSelfTokenID(selfTokenId_) != 0) {
        HILOG_ERROR("~UserFileServiceTokenMock SetSelfTokenID: 0x%{public}" PRIx64 " failed", selfTokenId_);
    }
    HILOG_INFO("~UserFileServiceTokenMock, Current tokenId = 0x%{public}" PRIx64 "", GetSelfTokenID());
}

void UserFileServiceTokenMock::SetFileManagerToken()
{
    std::string filesBundle(reinterpret_cast<const char *>(FILES));
    std::string fileManagerBundle(reinterpret_cast<const char *>(FILEMANAGER));
    if (!SetHapToken(filesBundle, requiredPermissions, true)) {
        HILOG_ERROR("SetFileManagerToken files failed.");
        if (!SetHapToken(fileManagerBundle, requiredPermissions, true)) {
            HILOG_ERROR("SetFileManagerToken filemanager failed.");
        }
    }
}

void UserFileServiceTokenMock::SetSaToken(const std::string& process)
{
    SetSelfTokenID(selfTokenId_);
    uint32_t tokenId = GetNativeTokenIdFromProcess(process);
    HILOG_INFO("SetSaToken, tokenId = 0x%{public}x", tokenId);
    if (SetSelfTokenID(tokenId) != 0) {
        HILOG_ERROR("SetSaToken SetSelfTokenID failed");
    }
    HILOG_INFO("SetSaToken Current tokenId = 0x%{public}" PRIx64 "", GetSelfTokenID());
}

int32_t UserFileServiceTokenMock::AllocTestHapToken(
    const HapInfoParams &hapInfo, HapPolicyParams &hapPolicy, AccessTokenIDEx &tokenIdEx)
{
    uint64_t selfTokenId = GetSelfTokenID();
    int ret = RET_SUCCESS;
    for (auto& permissionStateFull : hapPolicy.permStateList) {
        PermissionDef permDefResult;
        if (AccessTokenKit::GetDefPermission(permissionStateFull.permissionName, permDefResult) != RET_SUCCESS) {
            continue;
        }
        if (permDefResult.availableLevel > hapPolicy.apl) {
            hapPolicy.aclRequestedList.emplace_back(permissionStateFull.permissionName);
        }
    }
    if (GetNativeTokenIdFromProcess("foundation") == selfTokenId) {
        HILOG_INFO("GetNativeTokenIdFromProcess called");
        ret = AccessTokenKit::InitHapToken(hapInfo, hapPolicy, tokenIdEx);
    } else {
        HILOG_INFO("SetSaToken called");
        // set sh token for self
        SetSaToken("foundation");
        ret = AccessTokenKit::InitHapToken(hapInfo, hapPolicy, tokenIdEx);
        if (ret != 0) {
            HILOG_ERROR("InitHapToken failed");
        }
        // restore
        if (SetSelfTokenID(selfTokenId) != 0) {
            HILOG_ERROR("AllocTestHapToken SetSelfTokenID failed");
        }
    }
    return ret;
}

bool UserFileServiceTokenMock::SetHapToken(
    const std::string& bundle, const std::vector<std::string>& reqPerm, bool isSystemApp)
{
    SetSelfTokenID(selfTokenId_);
    HapInfoParams infoParams = {
        .userID = 0,
        .bundleName = bundle,
        .instIndex = 0,
        .appIDDesc = "AccessTokenTestAppID",
        .apiVersion = DEFAULT_API_VERSION,
        .isSystemApp = isSystemApp,
        .appDistributionType = "",
    };

    HapPolicyParams policyParams = {
        .apl = APL_NORMAL,
        .domain = "accesstoken_test_domain",
    };
    for (size_t i = 0; i < reqPerm.size(); ++i) {
        PermissionDef permDefResult;
        if (AccessTokenKit::GetDefPermission(reqPerm[i], permDefResult) != RET_SUCCESS) {
            continue;
        }
        PermissionStateFull permState = {
            .permissionName = reqPerm[i],
            .isGeneral = true,
            .resDeviceID = {"local3"},
            .grantStatus = {PermissionState::PERMISSION_DENIED},
            .grantFlags = {PermissionFlag::PERMISSION_DEFAULT_FLAG}
        };
        policyParams.permStateList.emplace_back(permState);
        if (permDefResult.availableLevel > policyParams.apl) {
            policyParams.aclRequestedList.emplace_back(reqPerm[i]);
        }
    }

    AccessTokenIDEx tokenIdEx = { 0 };
    AllocTestHapToken(infoParams, policyParams, tokenIdEx);
    mockTokenId_ = tokenIdEx.tokenIdExStruct.tokenID;
    if (mockTokenId_ == INVALID_TOKENID) {
        HILOG_ERROR("SetHapToken AllocTestHapToken failed");
        return false;
    }
    if (SetSelfTokenID(tokenIdEx.tokenIDEx) != 0) {
        HILOG_ERROR("SetHapToken SetSelfTokenID failed");
        return false;
    }
    return true;
}
} // namespace FileAccessFwk
} // namespace OHOS