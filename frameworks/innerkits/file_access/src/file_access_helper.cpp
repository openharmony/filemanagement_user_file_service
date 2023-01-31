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

#include "bundle_constants.h"
#include "bundle_mgr_proxy.h"
#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "if_system_ability_manager.h"
#include "ifile_access_ext_base.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "tokenid_kit.h"

namespace OHOS {
namespace FileAccessFwk {
std::vector<AAFwk::Want> FileAccessHelper::wants_;

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
    HILOG_DEBUG("Uri : %{public}s.", uri.ToString().c_str());
    std::string schemeStr = std::string(uri.GetScheme());
    if (schemeStr.compare(SCHEME_NAME) != 0) {
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
    if (context == nullptr) {
        HILOG_ERROR("FileAccessHelper::Creator failed, context == nullptr");
        return {nullptr, EINVAL};
    }

    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::Creator check IsSystemAppByFullTokenID failed");
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
        return {nullptr, E_GETINFO};
    }

    for (size_t i = 0; i < extensionInfos.size(); i++) {
        AAFwk::Want wantTem;
        wantTem.SetElementName(extensionInfos[i].bundleName, extensionInfos[i].name);
        sptr<FileAccessExtConnection> fileAccessExtConnection = new(std::nothrow) FileAccessExtConnection();
        if (fileAccessExtConnection == nullptr) {
            HILOG_ERROR("new fileAccessExtConnection fail");
            return {nullptr, E_GETRESULT};
        }

        if (!fileAccessExtConnection->IsExtAbilityConnected()) {
            fileAccessExtConnection->ConnectFileExtAbility(wantTem, context->GetToken());
        }

        sptr<IFileAccessExtBase> fileExtProxy = fileAccessExtConnection->GetFileExtProxy();
        if (fileExtProxy == nullptr) {
            HILOG_ERROR("Creator get invalid fileExtProxy");
            return {nullptr, E_CONNECT};
        }

        std::shared_ptr<ConnectInfo> connectInfo = std::make_shared<ConnectInfo>();
        if (connectInfo == nullptr) {
            HILOG_ERROR("Creator, connectInfo == nullptr");
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
        return {nullptr, E_GETRESULT};
    }

    return {std::shared_ptr<FileAccessHelper>(ptrFileAccessHelper), ERR_OK};
}

std::pair<std::shared_ptr<FileAccessHelper>, int> FileAccessHelper::Creator(
    const std::shared_ptr<OHOS::AbilityRuntime::Context> &context, const std::vector<AAFwk::Want> &wants)
{
    if (context == nullptr) {
        HILOG_ERROR("FileAccessHelper::Creator failed, context == nullptr");
        return {nullptr, EINVAL};
    }

    if (wants.size() == 0) {
        HILOG_ERROR("FileAccessHelper::Creator failed, wants is empty");
        return {nullptr, EINVAL};
    }

    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::Creator check IsSystemAppByFullTokenID failed");
        return {nullptr, E_PERMISSION_SYS};
    }

    if (GetRegisteredFileAccessExtAbilityInfo(FileAccessHelper::wants_) != ERR_OK) {
        HILOG_ERROR("GetRegisteredFileAccessExtAbilityInfo failed");
        return {nullptr, E_GETINFO};
    }

    std::unordered_map<std::string, std::shared_ptr<ConnectInfo>> cMap;
    for (size_t i = 0; i < wants.size(); i++) {
        sptr<FileAccessExtConnection> fileAccessExtConnection = new(std::nothrow) FileAccessExtConnection();
        if (fileAccessExtConnection == nullptr) {
            HILOG_ERROR("new fileAccessExtConnection fail");
            return {nullptr, E_GETRESULT};
        }

        if (!fileAccessExtConnection->IsExtAbilityConnected()) {
            fileAccessExtConnection->ConnectFileExtAbility(wants[i], context->GetToken());
        }

        sptr<IFileAccessExtBase> fileExtProxy = fileAccessExtConnection->GetFileExtProxy();
        if (fileExtProxy == nullptr) {
            HILOG_ERROR("Creator get invalid fileExtProxy");
            return {nullptr, E_CONNECT};
        }

        std::shared_ptr<ConnectInfo> connectInfo = std::make_shared<ConnectInfo>();
        if (connectInfo == nullptr) {
            HILOG_ERROR("Creator, connectInfo == nullptr");
            return {nullptr, E_GETRESULT};
        }

        connectInfo->want = wants[i];
        connectInfo->fileAccessExtConnection = fileAccessExtConnection;
        string bundleName = FileAccessHelper::GetKeyOfWants(wants[i]);
        if (bundleName.length() == 0) {
            HILOG_ERROR("Creator GetKeyOfWants bundleName not found");
            return {nullptr, E_GETRESULT};
        }
        cMap.insert(std::pair<std::string, std::shared_ptr<ConnectInfo>>(bundleName, connectInfo));
    }
    FileAccessHelper *ptrFileAccessHelper = new (std::nothrow) FileAccessHelper(context, cMap);
    if (ptrFileAccessHelper == nullptr) {
        HILOG_ERROR("Creator failed, create FileAccessHelper failed");
        return {nullptr, E_GETRESULT};
    }

    return {std::shared_ptr<FileAccessHelper>(ptrFileAccessHelper), ERR_OK};
}

std::shared_ptr<FileAccessHelper> FileAccessHelper::Creator(const sptr<IRemoteObject> &token,
    const std::vector<AAFwk::Want> &wants)
{
    if (token == nullptr) {
        HILOG_ERROR("FileAccessHelper::Creator failed, token == nullptr");
        return nullptr;
    }

    if (wants.size() == 0) {
        HILOG_ERROR("FileAccessHelper::Creator failed, wants is empty");
        return nullptr;
    }

    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::Creator check IsSystemAppByFullTokenID failed");
        return nullptr;
    }

    if (GetRegisteredFileAccessExtAbilityInfo(FileAccessHelper::wants_) != ERR_OK) {
        HILOG_ERROR("GetRegisteredFileAccessExtAbilityInfo failed");
        return nullptr;
    }

    std::unordered_map<std::string, std::shared_ptr<ConnectInfo>> cMap;
    for (size_t i = 0; i < wants.size(); i++) {
        sptr<FileAccessExtConnection> fileAccessExtConnection = new(std::nothrow) FileAccessExtConnection();
        if (fileAccessExtConnection == nullptr) {
            HILOG_ERROR("new fileAccessExtConnection fail");
            return nullptr;
        }

        if (!fileAccessExtConnection->IsExtAbilityConnected()) {
            fileAccessExtConnection->ConnectFileExtAbility(wants[i], token);
        }

        sptr<IFileAccessExtBase> fileExtProxy = fileAccessExtConnection->GetFileExtProxy();
        if (fileExtProxy == nullptr) {
            HILOG_ERROR("Creator get invalid fileExtProxy");
            return nullptr;
        }

        std::shared_ptr<ConnectInfo> connectInfo = std::make_shared<ConnectInfo>();
        if (connectInfo == nullptr) {
            HILOG_ERROR("Creator, connectInfo == nullptr");
            return nullptr;
        }

        connectInfo->want = wants[i];
        connectInfo->fileAccessExtConnection = fileAccessExtConnection;
        string bundleName = FileAccessHelper::GetKeyOfWants(wants[i]);
        if (bundleName.length() == 0) {
            HILOG_ERROR("Creator GetKeyOfWants bundleName not found");
            return nullptr;
        }
        cMap.insert(std::pair<std::string, std::shared_ptr<ConnectInfo>>(bundleName, connectInfo));
    }
    FileAccessHelper *ptrFileAccessHelper = new (std::nothrow) FileAccessHelper(token, cMap);
    if (ptrFileAccessHelper == nullptr) {
        HILOG_ERROR("Creator failed, create FileAccessHelper failed");
        return nullptr;
    }

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
    std::string bundleName;
    if (!GetBundleNameFromPath(uri.GetPath(), bundleName)) {
        HILOG_ERROR("Get BundleName failed.");
        return nullptr;
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

int FileAccessHelper::Rename(Uri &sourceFile, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Rename");
    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::Rename check IsSystemAppByFullTokenID failed");
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

int FileAccessHelper::GetRoots(std::vector<RootInfo> &rootInfoVec)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetRoots");
    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::GetRoots check IsSystemAppByFullTokenID failed");
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

int FileAccessHelper::GetFileInfoFromUri(Uri &selectFile, FileInfo &fileInfo)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetFileInfoFromUri");
    if (!IsSystemApp()) {
        HILOG_ERROR("FileAccessHelper::GetFileInfoFromUri check IsSystemAppByFullTokenID failed");
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

int FileAccessHelper::On(std::shared_ptr<INotifyCallback> &callback)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "On");
    if (callback == nullptr) {
        HILOG_ERROR("failed with invalid callback");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return EINVAL;
    }

    if (!GetProxy()) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    std::lock_guard<std::mutex> lock(notifyAgentMutex_);
    if (notifyAgent_ == nullptr) {
        notifyAgent_ = new(std::nothrow) FileAccessNotifyAgent(callback);
        if (notifyAgent_ == nullptr) {
            HILOG_ERROR("new FileAccessNotifyAgent fail");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return EINVAL;
        }
    }

    int errorCode = ERR_OK;
    for (auto iter = cMap_.begin(); iter != cMap_.end(); ++iter) {
        auto connectInfo = iter->second;
        auto fileAccessExtProxy = connectInfo->fileAccessExtConnection->GetFileExtProxy();
        if (fileAccessExtProxy == nullptr) {
            HILOG_ERROR("fileAccessExtProxy RegisterNotify fail");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return E_IPCS;
        }

        errorCode = fileAccessExtProxy->RegisterNotify(notifyAgent_);
        if (errorCode != ERR_OK) {
            HILOG_ERROR("fileAccessExtProxy RegisterNotify fail, bundleName:%{public}s, ret:%{public}d.",
                connectInfo->want.GetElement().GetBundleName().c_str(), errorCode);
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return errorCode;
        }
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessHelper::Off()
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Off");
    std::lock_guard<std::mutex> lock(notifyAgentMutex_);
    if (notifyAgent_ == nullptr) {
        HILOG_ERROR("not registered notify");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_NOTIFY;
    }

    int errorCode = ERR_OK;
    for (auto [key, value] : cMap_) {
        auto connectInfo = value;
        auto fileAccessExtProxy = connectInfo->fileAccessExtConnection->GetFileExtProxy();
        if (fileAccessExtProxy == nullptr) {
            HILOG_INFO("fileAccessExtProxy UnregisterNotify fail, bundleName:%{public}s",
                connectInfo->want.GetElement().GetBundleName().c_str());
            continue;
        }

        errorCode = fileAccessExtProxy->UnregisterNotify(notifyAgent_);
        if (errorCode != ERR_OK) {
            HILOG_ERROR("fileAccessExtProxy UnregisterNotify fail, bundleName:%{public}s, ret:%{public}d.",
                connectInfo->want.GetElement().GetBundleName().c_str(), errorCode);
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return errorCode;
        }
    }

    notifyAgent_.clear();
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
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