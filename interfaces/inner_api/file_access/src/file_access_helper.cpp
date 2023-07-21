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

#include "file_access_helper.h"

#include <nlohmann/json.hpp>
#include "bundle_constants.h"
#include "bundle_mgr_proxy.h"
#include "file_access_framework_errno.h"
#include "file_access_extension_info.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "if_system_ability_manager.h"
#include "ifile_access_ext_base.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "image_source.h"
#include "system_ability_definition.h"
#include "tokenid_kit.h"
#include "n_error.h"

namespace OHOS {
namespace FileAccessFwk {
using namespace Media;
using json = nlohmann::json;
namespace {
    constexpr int COPY_EXCEPTION = -1;
    constexpr int COPY_NOEXCEPTION = -2;
}
std::vector<AAFwk::Want> FileAccessHelper::wants_;
sptr<IFileAccessExtBase> g_sourceExtProxy;
sptr<IFileAccessExtBase> g_destExtProxy;

static int GetUserId()
{
    int uid = IPCSkeleton::GetCallingUid();
    int userId = uid / AppExecFwk::Constants::BASE_USER_RANGE;
    return userId;
}

static bool GetBundleNameFromPath(const std::string &path, std::string &bundleName)
{
    if (path.size() == 0) {
        HILOG_ERROR("Uri path error.");
        return false;
    }

    if (path.front() != '/') {
        HILOG_ERROR("Uri path format error.");
        return false;
    }

    auto tmpPath = path.substr(1);
    auto index = tmpPath.find_first_of("/");
    bundleName = tmpPath.substr(0, index);
    if (bundleName.compare(MEDIA_BNUDLE_NAME_ALIAS) == 0) {
        bundleName = MEDIA_BNUDLE_NAME;
    }
    return true;
}

static bool CheckUri(Uri &uri)
{
    HILOG_DEBUG("Uri : %{private}s.", uri.ToString().c_str());
    std::string schemeStr = std::string(uri.GetScheme());
    if (schemeStr.compare(FILE_SCHEME_NAME) != 0) {
        HILOG_ERROR("Uri scheme error.");
        return false;
    }
    return true;
}

sptr<AppExecFwk::IBundleMgr> FileAccessHelper::GetBundleMgrProxy()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        return nullptr;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        return nullptr;
    }

    return iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
}

FileAccessHelper::FileAccessHelper(const std::shared_ptr<OHOS::AbilityRuntime::Context> &context,
    const std::unordered_map<std::string, std::shared_ptr<ConnectInfo>> &cMap)
{
    token_ = context->GetToken();
    cMap_ = cMap;
}

FileAccessHelper::FileAccessHelper(const sptr<IRemoteObject> &token,
    const std::unordered_map<std::string, std::shared_ptr<ConnectInfo>> &cMap)
{
    token_ = token;
    cMap_ = cMap;
}

void FileAccessHelper::AddFileAccessDeathRecipient(const sptr<IRemoteObject> &token)
{
    if (token != nullptr && callerDeathRecipient_ != nullptr) {
        token->RemoveDeathRecipient(callerDeathRecipient_);
    }
    if (callerDeathRecipient_ == nullptr) {
        callerDeathRecipient_ =
            new FileAccessDeathRecipient(std::bind(&FileAccessHelper::OnSchedulerDied, this, std::placeholders::_1));
    }
    if (token != nullptr) {
        token->AddDeathRecipient(callerDeathRecipient_);
    }
}

void FileAccessHelper::OnSchedulerDied(const wptr<IRemoteObject> &remote)
{
    auto object = remote.promote();
    object = nullptr;
}

std::shared_ptr<ConnectInfo> FileAccessHelper::GetConnectInfo(const std::string &bundleName)
{
    auto iterator = cMap_.find(bundleName);
    if (iterator != cMap_.end()) {
        return iterator->second;
    }
    HILOG_ERROR("GetConnectInfo called with bundleName return nullptr");
    return nullptr;
}

std::string FileAccessHelper::GetKeyOfWants(const AAFwk::Want &want)
{
    auto elementTmp = want.GetElement();
    for (auto iter = FileAccessHelper::wants_.begin(); iter != FileAccessHelper::wants_.end(); ++iter) {
        auto element = iter->GetElement();
        if (element.GetBundleName() == elementTmp.GetBundleName() &&
            element.GetAbilityName() == elementTmp.GetAbilityName()) {
            return element.GetBundleName();
        }
    }
    HILOG_ERROR("GetKeyOfWants did not find a want message to match");
    return "";
}

static bool IsSystemApp()
{
    uint64_t accessTokenIDEx = OHOS::IPCSkeleton::GetCallingFullTokenID();
    return OHOS::Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(accessTokenIDEx);
}

std::pair<std::shared_ptr<FileAccessHelper>, int> FileAccessHelper::Creator(
    const std::shared_ptr<OHOS::AbilityRuntime::Context> &context)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Creator");
    if (context == nullptr) {
        HILOG_ERROR("FileAccessHelper::Creator failed, context == nullptr");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return {nullptr, EINVAL};
    }

    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::Creator check IsSystemAppByFullTokenID failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return {nullptr, E_PERMISSION_SYS};
    }

    sptr<AppExecFwk::IBundleMgr> bm = FileAccessHelper::GetBundleMgrProxy();
    FileAccessHelper::wants_.clear();
    std::unordered_map<std::string, std::shared_ptr<ConnectInfo>> cMap;
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    bool ret = bm->QueryExtensionAbilityInfos(
        AppExecFwk::ExtensionAbilityType::FILEACCESS_EXTENSION, GetUserId(), extensionInfos);
    if (!ret) {
        HILOG_ERROR("FileAccessHelper::Creator QueryExtensionAbilityInfos failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return {nullptr, E_GETINFO};
    }

    for (size_t i = 0; i < extensionInfos.size(); i++) {
        AAFwk::Want wantTem;
        wantTem.SetElementName(extensionInfos[i].bundleName, extensionInfos[i].name);
        sptr<FileAccessExtConnection> fileAccessExtConnection = new(std::nothrow) FileAccessExtConnection();
        if (fileAccessExtConnection == nullptr) {
            HILOG_ERROR("new fileAccessExtConnection fail");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return {nullptr, E_GETRESULT};
        }

        if (!fileAccessExtConnection->IsExtAbilityConnected()) {
            fileAccessExtConnection->ConnectFileExtAbility(wantTem, context->GetToken());
        }

        sptr<IFileAccessExtBase> fileExtProxy = fileAccessExtConnection->GetFileExtProxy();
        if (fileExtProxy == nullptr) {
            HILOG_ERROR("Creator get invalid fileExtProxy");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return {nullptr, E_CONNECT};
        }

        std::shared_ptr<ConnectInfo> connectInfo = std::make_shared<ConnectInfo>();
        if (!connectInfo) {
            HILOG_ERROR("Creator, connectInfo == nullptr");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return {nullptr, E_GETRESULT};
        }
        FileAccessHelper::wants_.push_back(wantTem);

        connectInfo->want = wantTem;
        connectInfo->fileAccessExtConnection = fileAccessExtConnection;
        cMap.emplace(extensionInfos[i].bundleName, connectInfo);
    }
    FileAccessHelper *ptrFileAccessHelper = new (std::nothrow) FileAccessHelper(context, cMap);
    if (ptrFileAccessHelper == nullptr) {
        HILOG_ERROR("FileAccessHelper::Creator failed, create FileAccessHelper failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return {nullptr, E_GETRESULT};
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return {std::shared_ptr<FileAccessHelper>(ptrFileAccessHelper), ERR_OK};
}

std::pair<std::shared_ptr<FileAccessHelper>, int> FileAccessHelper::Creator(
    const std::shared_ptr<OHOS::AbilityRuntime::Context> &context, const std::vector<AAFwk::Want> &wants)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Creator");
    if (context == nullptr) {
        HILOG_ERROR("FileAccessHelper::Creator failed, context == nullptr");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return {nullptr, EINVAL};
    }

    if (wants.size() == 0) {
        HILOG_ERROR("FileAccessHelper::Creator failed, wants is empty");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return {nullptr, EINVAL};
    }

    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::Creator check IsSystemAppByFullTokenID failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return {nullptr, E_PERMISSION_SYS};
    }

    if (GetRegisteredFileAccessExtAbilityInfo(FileAccessHelper::wants_) != ERR_OK) {
        HILOG_ERROR("GetRegisteredFileAccessExtAbilityInfo failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return {nullptr, E_GETINFO};
    }

    std::unordered_map<std::string, std::shared_ptr<ConnectInfo>> cMap;
    for (size_t i = 0; i < wants.size(); i++) {
        sptr<FileAccessExtConnection> fileAccessExtConnection = new(std::nothrow) FileAccessExtConnection();
        if (fileAccessExtConnection == nullptr) {
            HILOG_ERROR("new fileAccessExtConnection fail");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return {nullptr, E_GETRESULT};
        }

        if (!fileAccessExtConnection->IsExtAbilityConnected()) {
            fileAccessExtConnection->ConnectFileExtAbility(wants[i], context->GetToken());
        }

        sptr<IFileAccessExtBase> fileExtProxy = fileAccessExtConnection->GetFileExtProxy();
        if (fileExtProxy == nullptr) {
            HILOG_ERROR("Creator get invalid fileExtProxy");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return {nullptr, E_CONNECT};
        }

        std::shared_ptr<ConnectInfo> connectInfo = std::make_shared<ConnectInfo>();
        if (connectInfo == nullptr) {
            HILOG_ERROR("Creator, connectInfo == nullptr");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return {nullptr, E_GETRESULT};
        }

        connectInfo->want = wants[i];
        connectInfo->fileAccessExtConnection = fileAccessExtConnection;
        string bundleName = FileAccessHelper::GetKeyOfWants(wants[i]);
        if (bundleName.length() == 0) {
            HILOG_ERROR("Creator GetKeyOfWants bundleName not found");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return {nullptr, E_GETRESULT};
        }
        cMap.insert(std::pair<std::string, std::shared_ptr<ConnectInfo>>(bundleName, connectInfo));
    }
    FileAccessHelper *ptrFileAccessHelper = new (std::nothrow) FileAccessHelper(context, cMap);
    if (ptrFileAccessHelper == nullptr) {
        HILOG_ERROR("Creator failed, create FileAccessHelper failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return {nullptr, E_GETRESULT};
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return {std::shared_ptr<FileAccessHelper>(ptrFileAccessHelper), ERR_OK};
}

std::shared_ptr<FileAccessHelper> FileAccessHelper::Creator(const sptr<IRemoteObject> &token,
    const std::vector<AAFwk::Want> &wants)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Creator");
    if (token == nullptr) {
        HILOG_ERROR("FileAccessHelper::Creator failed, token == nullptr");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return nullptr;
    }

    if (wants.size() == 0) {
        HILOG_ERROR("FileAccessHelper::Creator failed, wants is empty");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return nullptr;
    }

    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::Creator check IsSystemAppByFullTokenID failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return nullptr;
    }

    if (GetRegisteredFileAccessExtAbilityInfo(FileAccessHelper::wants_) != ERR_OK) {
        HILOG_ERROR("GetRegisteredFileAccessExtAbilityInfo failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return nullptr;
    }

    std::unordered_map<std::string, std::shared_ptr<ConnectInfo>> cMap;
    for (size_t i = 0; i < wants.size(); i++) {
        sptr<FileAccessExtConnection> fileAccessExtConnection = new(std::nothrow) FileAccessExtConnection();
        if (fileAccessExtConnection == nullptr) {
            HILOG_ERROR("new fileAccessExtConnection fail");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return nullptr;
        }

        if (!fileAccessExtConnection->IsExtAbilityConnected()) {
            fileAccessExtConnection->ConnectFileExtAbility(wants[i], token);
        }

        sptr<IFileAccessExtBase> fileExtProxy = fileAccessExtConnection->GetFileExtProxy();
        if (fileExtProxy == nullptr) {
            HILOG_ERROR("Creator get invalid fileExtProxy");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return nullptr;
        }

        std::shared_ptr<ConnectInfo> connectInfo = std::make_shared<ConnectInfo>();
        if (!connectInfo) {
            HILOG_ERROR("Creator, connectInfo == nullptr");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return nullptr;
        }

        connectInfo->want = wants[i];
        connectInfo->fileAccessExtConnection = fileAccessExtConnection;
        string bundleName = FileAccessHelper::GetKeyOfWants(wants[i]);
        if (bundleName.length() == 0) {
            HILOG_ERROR("Creator GetKeyOfWants bundleName not found");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return nullptr;
        }
        cMap.insert(std::pair<std::string, std::shared_ptr<ConnectInfo>>(bundleName, connectInfo));
    }
    FileAccessHelper *ptrFileAccessHelper = new (std::nothrow) FileAccessHelper(token, cMap);
    if (ptrFileAccessHelper == nullptr) {
        HILOG_ERROR("Creator failed, create FileAccessHelper failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return nullptr;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return std::shared_ptr<FileAccessHelper>(ptrFileAccessHelper);
}

bool FileAccessHelper::Release()
{
    for (auto iter = cMap_.begin(); iter != cMap_.end(); iter++) {
        if (iter->second->fileAccessExtConnection->IsExtAbilityConnected()) {
            iter->second->fileAccessExtConnection->DisconnectFileExtAbility();
        }
    }
    cMap_.clear();
    token_ = nullptr;
    FileAccessHelper::wants_.clear();
    return true;
}

sptr<IFileAccessExtBase> FileAccessHelper::GetProxyByUri(Uri &uri)
{
    std::string scheme = uri.GetScheme();
    std::string bundleName;
    if (scheme == FILE_SCHEME_NAME) {
        std::string path = "/" + uri.GetAuthority() + uri.GetPath();
        if (!GetBundleNameFromPath(path, bundleName)) {
            HILOG_ERROR("Get BundleName failed.");
            return nullptr;
        }
    }

    auto fileAccessExtProxy = GetProxyByBundleName(bundleName);
    return fileAccessExtProxy;
}

sptr<IFileAccessExtBase> FileAccessHelper::GetProxyByBundleName(const std::string &bundleName)
{
    auto connectInfo = GetConnectInfo(bundleName);
    if (connectInfo == nullptr) {
        HILOG_ERROR("GetProxyByUri failed with invalid connectInfo");
        return nullptr;
    }

    if (!connectInfo->fileAccessExtConnection->IsExtAbilityConnected()) {
        connectInfo->fileAccessExtConnection->ConnectFileExtAbility(connectInfo->want, token_);
    }

    auto fileAccessExtProxy = connectInfo->fileAccessExtConnection->GetFileExtProxy();
    if (fileAccessExtProxy) {
        AddFileAccessDeathRecipient(fileAccessExtProxy->AsObject());
    }

    if (fileAccessExtProxy == nullptr) {
        HILOG_ERROR("GetProxyByUri failed with invalid fileAccessExtProxy");
        return nullptr;
    }

    return fileAccessExtProxy;
}

bool FileAccessHelper::GetProxy()
{
    for (auto iter = cMap_.begin(); iter != cMap_.end(); ++iter) {
        auto connectInfo = iter->second;
        if (!connectInfo->fileAccessExtConnection->IsExtAbilityConnected()) {
            connectInfo->fileAccessExtConnection->ConnectFileExtAbility(connectInfo->want, token_);
        }

        auto fileAccessExtProxy = connectInfo->fileAccessExtConnection->GetFileExtProxy();
        if (fileAccessExtProxy) {
            AddFileAccessDeathRecipient(fileAccessExtProxy->AsObject());
        }

        if (fileAccessExtProxy == nullptr) {
            HILOG_ERROR("GetProxy failed with invalid fileAccessExtProxy");
            return false;
        }
    }
    return true;
}

int FileAccessHelper::OpenFile(Uri &uri, int flags, int &fd)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "OpenFile");
    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::OpenFile check IsSystemAppByFullTokenID failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_PERMISSION_SYS;
    }

    if (!CheckUri(uri)) {
        HILOG_ERROR("Uri format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_URIS;
    }

    if (flags != READ && flags != WRITE && flags != WRITE_READ) {
        HILOG_ERROR("flags type error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return EINVAL;
    }

    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(uri);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = fileExtProxy->OpenFile(uri, flags, fd);
    if (ret != ERR_OK) {
        HILOG_ERROR("OpenFile get result error, code:%{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessHelper::CreateFile(Uri &parent, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CreateFile");
    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::CreateFile check IsSystemAppByFullTokenID failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_PERMISSION_SYS;
    }

    if (!CheckUri(parent)) {
        HILOG_ERROR("Uri format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_URIS;
    }

    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(parent);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = fileExtProxy->CreateFile(parent, displayName, newFile);
    if (ret != ERR_OK) {
        HILOG_ERROR("CreateFile get result error, code:%{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessHelper::Mkdir(Uri &parent, const std::string &displayName, Uri &newDir)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Mkdir");
    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::Mkdir check IsSystemAppByFullTokenID failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_PERMISSION_SYS;
    }

    if (!CheckUri(parent)) {
        HILOG_ERROR("Uri format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_URIS;
    }

    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(parent);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = fileExtProxy->Mkdir(parent, displayName, newDir);
    if (ret != ERR_OK) {
        HILOG_ERROR("Mkdir get result error, code:%{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessHelper::Delete(Uri &selectFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Delete");
    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::Delete check IsSystemAppByFullTokenID failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_PERMISSION_SYS;
    }

    if (!CheckUri(selectFile)) {
        HILOG_ERROR("Uri format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_URIS;
    }

    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(selectFile);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = fileExtProxy->Delete(selectFile);
    if (ret != ERR_OK) {
        HILOG_ERROR("Delete get result error, code:%{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessHelper::Move(Uri &sourceFile, Uri &targetParent, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Move");
    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::Move check IsSystemAppByFullTokenID failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_PERMISSION_SYS;
    }

    Uri sourceFileUri(sourceFile.ToString());
    Uri targetParentUri(targetParent.ToString());
    if (!CheckUri(sourceFile)) {
        HILOG_ERROR("sourceFile format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_URIS;
    }

    if (!CheckUri(targetParent)) {
        HILOG_ERROR("targetParent format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_URIS;
    }

    if (sourceFileUri.GetScheme() != targetParentUri.GetScheme()) {
        HILOG_WARN("Operation failed, move not supported");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return EPERM;
    }

    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(sourceFile);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = fileExtProxy->Move(sourceFile, targetParent, newFile);
    if (ret != ERR_OK) {
        HILOG_ERROR("Move get result error, code:%{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

static bool IsMediaUri(Uri &uri)
{
    string scheme = uri.GetScheme();
    if (scheme == FILE_SCHEME_NAME) {
        return uri.GetAuthority() == MEDIA_BNUDLE_NAME_ALIAS;
    }
    return false;
}

static int ThrowExceptionByErrorCode(int errCode, CopyResult &copyResult)
{
    if (errCode == COPY_EXCEPTION || errCode == ERR_OK) {
        return errCode;
    }

    int ret = ERR_OK;
    switch (errCode) {
        case E_IPCS:
        case E_URIS:
        case ERR_NOMEM:
        case ERR_PERM: {
            HILOG_ERROR("Copy exception, terminate copy");
            ret = COPY_EXCEPTION;
            break;
        }
        default: {
            HILOG_ERROR("Copy exception, continue copy");
            ret = COPY_NOEXCEPTION;
            break;
        }
    }
    if (OHOS::FileManagement::LibN::errCodeTable.find(errCode) !=
        OHOS::FileManagement::LibN::errCodeTable.end()) {
        copyResult.errCode = OHOS::FileManagement::LibN::errCodeTable.at(errCode).first;
        copyResult.errMsg = OHOS::FileManagement::LibN::errCodeTable.at(errCode).second;
    }
    return ret;
}

int FileAccessHelper::CopyInsideService(Uri &sourceUri, Uri &destUri, std::vector<CopyResult> &copyResult, bool force)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CopyInsideService");
    int ret = COPY_EXCEPTION;
    sptr<IFileAccessExtBase> proxy = GetProxyByUri(sourceUri);
    if (proxy == nullptr) {
        HILOG_ERROR("Failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        CopyResult result;
        ret = ThrowExceptionByErrorCode(E_IPCS, result);
        copyResult.push_back(result);
        return ret;
    }

    ret = proxy->Copy(sourceUri, destUri, copyResult, force);
    if (ret != ERR_OK) {
        HILOG_ERROR("Copy error, code:%{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        if ((ret == COPY_EXCEPTION) || (ret == COPY_NOEXCEPTION)) {
            HILOG_ERROR("Copy exception, code:%{public}d", ret);
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return ret;
        }
        CopyResult result { "", "", ret, ""};
        copyResult.push_back(result);
        return COPY_EXCEPTION;
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessHelper::Copy(Uri &sourceUri, Uri &destUri, std::vector<CopyResult> &copyResult, bool force)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Copy");
    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::Copy check IsSystemAppByFullTokenID failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        CopyResult result { sourceUri.ToString(), destUri.ToString(), E_PERMISSION_SYS, "" };
        copyResult.push_back(result);
        return COPY_EXCEPTION;
    }

    if (!CheckUri(sourceUri) || !CheckUri(destUri)) {
        HILOG_ERROR("Uri format check error");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        CopyResult result { sourceUri.ToString(), destUri.ToString(), E_URIS, "" };
        copyResult.push_back(result);
        return COPY_EXCEPTION;
    }

    int ret = COPY_EXCEPTION;
    if (IsMediaUri(sourceUri) && IsMediaUri(destUri)) {
        ret = CopyInsideService(sourceUri, destUri, copyResult, force);
    } else {
        HILOG_ERROR("External storage copy is not supported.");
        CopyResult result { sourceUri.ToString(), destUri.ToString(), EPERM, "" };
        copyResult.push_back(result);
        return COPY_NOEXCEPTION;
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessHelper::Rename(Uri &sourceFile, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Rename");
    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::Rename check IsSystemAppByFullTokenID failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_PERMISSION_SYS;
    }

    if (!CheckUri(sourceFile)) {
        HILOG_ERROR("sourceFile format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_URIS;
    }

    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(sourceFile);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = fileExtProxy->Rename(sourceFile, displayName, newFile);
    if (ret != ERR_OK) {
        HILOG_ERROR("Rename get result error, code:%{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessHelper::ListFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
    const FileFilter &filter, std::vector<FileInfo> &fileInfoVec)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "ListFile");
    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::ListFile check IsSystemAppByFullTokenID failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_PERMISSION_SYS;
    }

    Uri sourceFile(fileInfo.uri);
    if (!CheckUri(sourceFile)) {
        HILOG_ERROR("sourceFile format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_URIS;
    }

    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(sourceFile);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = fileExtProxy->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    if (ret != ERR_OK) {
        HILOG_ERROR("ListFile get result error, code:%{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessHelper::ScanFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
    const FileFilter &filter, std::vector<FileInfo> &fileInfoVec)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "ScanFile");
    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::ScanFile check IsSystemAppByFullTokenID failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_PERMISSION_SYS;
    }

    Uri sourceFile(fileInfo.uri);
    if (!CheckUri(sourceFile)) {
        HILOG_ERROR("sourceFile format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_URIS;
    }

    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(sourceFile);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = fileExtProxy->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    if (ret != ERR_OK) {
        HILOG_ERROR("ScanFile get result error, code:%{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

static int GetQueryColumns(std::string &uri, std::string &metaJson, std::vector<std::string> &columns)
{
    if (!json::accept(metaJson)) {
        HILOG_ERROR("metaJson format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return EINVAL;
    }

    auto jsonObject = json::parse(metaJson);
    for (json::iterator it = jsonObject.begin(); it != jsonObject.end(); ++it) {
        auto iterator = FILE_RESULT_TYPE.find(it.key());
        if (iterator == FILE_RESULT_TYPE.end()) {
            HILOG_ERROR("columns format check error.");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return EINVAL;
        }
        columns.push_back(it.key());
    }

    if (columns.empty()) {
        HILOG_ERROR("columns is empty.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return EINVAL;
    }
    return ERR_OK;
}

static int GetQueryResult(std::string &uri, std::vector<std::string> &columns, std::vector<std::string> &results,
    std::string &metaJson)
{
    json jsonObject;
    for (size_t i = 0; i < columns.size(); i++) {
        auto memberType = FILE_RESULT_TYPE.at(columns.at(i));
        // Assign a default value based on the type, When fileIo obtains an invalid value.
        switch (memberType) {
            case STRING_TYPE:
                if (results[i].empty()) {
                    results[i] = " ";
                }
                jsonObject[columns[i]] = results[i];
                break;
            case INT32_TYPE:
                if (results[i].empty()) {
                    results[i] = "0";
                }
                jsonObject[columns[i]] = std::stoi(results[i]);
                break;
            case INT64_TYPE:
                if (results[i].empty()) {
                    results[i] = "0";
                }
                jsonObject[columns[i]] = std::stol(results[i]);
                break;
            default:
                jsonObject[columns[i]] = " ";
                HILOG_ERROR("not match  memberType %{public}d", memberType);
                break;
        }
    }
    metaJson = jsonObject.dump();
    return ERR_OK;
}

int FileAccessHelper::Query(Uri &uri, std::string &metaJson)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Query");
    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::Query check IsSystemAppByFullTokenID failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_PERMISSION_SYS;
    }

    if (!CheckUri(uri)) {
        HILOG_ERROR("Uri format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_URIS;
    }

    std::string uriString(uri.ToString());
    std::vector<std::string> columns;
    std::vector<std::string> results;
    int ret = GetQueryColumns(uriString, metaJson, columns);
    if (ret != ERR_OK) {
        HILOG_ERROR("Query get columns error, code:%{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(uri);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    ret = fileExtProxy->Query(uri, columns, results);
    if (ret != ERR_OK) {
        HILOG_ERROR("Query get result error, code:%{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    ret = GetQueryResult(uriString, columns, results, metaJson);
    if (ret != ERR_OK) {
        HILOG_ERROR("Query get result error, code:%{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessHelper::GetRoots(std::vector<RootInfo> &rootInfoVec)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetRoots");
    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::GetRoots check IsSystemAppByFullTokenID failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_PERMISSION_SYS;
    }

    if (!GetProxy()) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = ERR_OK;
    for (auto iter = cMap_.begin(); iter != cMap_.end(); ++iter) {
        auto connectInfo = iter->second;
        auto fileAccessExtProxy = connectInfo->fileAccessExtConnection->GetFileExtProxy();
        std::vector<RootInfo> results;
        if (fileAccessExtProxy) {
            AddFileAccessDeathRecipient(fileAccessExtProxy->AsObject());
        } else {
            HILOG_ERROR("GetFileExtProxy return nullptr, bundle name is %{public}s", iter->first.c_str());
            continue;
        }

        ret = fileAccessExtProxy->GetRoots(results);
        if (ret != ERR_OK) {
            HILOG_ERROR("getRoots get fail ret:%{public}d", ret);
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return ret;
        }

        rootInfoVec.insert(rootInfoVec.end(), results.begin(), results.end());
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessHelper::GetRegisteredFileAccessExtAbilityInfo(std::vector<AAFwk::Want> &wantVec)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetRegisteredFileAccessExtAbilityInfo");
    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::GetRoots check IsSystemAppByFullTokenID failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_PERMISSION_SYS;
    }

    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    sptr<AppExecFwk::IBundleMgr> bm = FileAccessHelper::GetBundleMgrProxy();
    if (bm == nullptr) {
        HILOG_ERROR("GetBundleMgrProxy nullptr.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_GETINFO;
    }
    bool ret = bm->QueryExtensionAbilityInfos(
        AppExecFwk::ExtensionAbilityType::FILEACCESS_EXTENSION, GetUserId(), extensionInfos);
    if (!ret) {
        HILOG_ERROR("FileAccessHelper::GetRegisteredFileAccessExtAbilityInfo QueryExtensionAbilityInfos error");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_GETINFO;
    }

    wantVec.clear();
    for (size_t i = 0; i < extensionInfos.size(); i++) {
        AAFwk::Want want;
        want.SetElementName(extensionInfos[i].bundleName, extensionInfos[i].name);
        wantVec.push_back(want);
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessHelper::Access(Uri &uri, bool &isExist)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Access");
    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::Access check IsSystemAppByFullTokenID failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_PERMISSION_SYS;
    }

    if (!CheckUri(uri)) {
        HILOG_ERROR("uri format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_URIS;
    }

    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(uri);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = fileExtProxy->Access(uri, isExist);
    if (ret != ERR_OK) {
        HILOG_ERROR("Access get result error, code:%{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessHelper::GetThumbnail(Uri &uri, ThumbnailSize &thumbnailSize, std::shared_ptr<PixelMap> &pixelMap)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetThumbnail");
    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::GetThumbnail check IsSystemAppByFullTokenID failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_PERMISSION_SYS;
    }

    if (!CheckUri(uri)) {
        HILOG_ERROR("Uri format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_URIS;
    }

    string uriStr = uri.ToString();
    if (thumbnailSize.width <= 0 || thumbnailSize.height <= 0) {
        HILOG_ERROR("Size format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_GETRESULT;
    }

    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(uri);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = fileExtProxy->GetThumbnail(uri, thumbnailSize, pixelMap);
    if (ret != ERR_OK) {
        HILOG_ERROR("GetThumbnail get result error, code:%{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessHelper::GetFileInfoFromUri(Uri &selectFile, FileInfo &fileInfo)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetFileInfoFromUri");
    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::GetFileInfoFromUri check IsSystemAppByFullTokenID failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_PERMISSION_SYS;
    }

    if (!CheckUri(selectFile)) {
        HILOG_ERROR("selectFile uri format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_URIS;
    }

    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(selectFile);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = fileExtProxy->GetFileInfoFromUri(selectFile, fileInfo);
    if (ret != ERR_OK) {
        HILOG_ERROR("GetFileInfoFromUri get result error, code:%{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessHelper::GetFileInfoFromRelativePath(std::string &selectFile, FileInfo &fileInfo)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetFileInfoFromRelativePath");
    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::GetFileInfoFromRelativePath check IsSystemAppByFullTokenID failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_PERMISSION_SYS;
    }

    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByBundleName(MEDIA_BNUDLE_NAME);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = fileExtProxy->GetFileInfoFromRelativePath(selectFile, fileInfo);
    if (ret != ERR_OK) {
        HILOG_ERROR("GetFileInfoFromRelativePath get result error, code:%{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessHelper::StartWatcher(Uri &uri)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "StartWatcher");
    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(uri);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = fileExtProxy->StartWatcher(uri);
    if (ret != ERR_OK) {
        HILOG_ERROR("Delete get result error, code:%{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessHelper::StopWatcher(Uri &uri)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "StopWatcher");
    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(uri);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = fileExtProxy->StopWatcher(uri);
    if (ret != ERR_OK) {
        HILOG_ERROR("Delete get result error, code:%{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessHelper::RegisterNotify(Uri uri, sptr<IFileAccessObserver> &observer, bool notifyForDescendants)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "RegisterNotify");
    return ERR_OK;
}

int FileAccessHelper::UnregisterNotify(Uri uri, sptr<IFileAccessObserver> &observer)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "UnregisterNotify");
    return ERR_OK;
}

void FileAccessDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    if (handler_) {
        handler_(remote);
    }
}

FileAccessDeathRecipient::FileAccessDeathRecipient(RemoteDiedHandler handler) : handler_(handler)
{
}

FileAccessDeathRecipient::~FileAccessDeathRecipient()
{
}
} // namespace FileAccessFwk
} // namespace OHOS
