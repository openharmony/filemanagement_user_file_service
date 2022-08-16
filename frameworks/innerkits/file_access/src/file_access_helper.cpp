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

#include "bundle_mgr_proxy.h"
#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "if_system_ability_manager.h"
#include "ifile_access_ext_base.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace FileAccessFwk {
namespace {
    static const int32_t UID_TRANSFORM_DIVISOR = 200000;
    static const std::string SCHEME_NAME = "datashare";
    static const std::string MEDIA_BNUDLE_NAME_ALIAS = "media";
    static const std::string MEDIA_BNUDLE_NAME = "com.ohos.medialibrary.medialibrarydata";
}
std::unordered_map<std::string, AAFwk::Want> FileAccessHelper::wantsMap_;

static bool GetBundleNameFromPath(const std::string &path, std::string &bundleName)
{
    if (path.size() == 0) {
        return false;
    }

    if (path.front() != '/') {
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

std::shared_ptr<ConnectInfo> FileAccessHelper::GetConnectInfo(const AAFwk::Want &want)
{
    for (auto iter = cMap_.begin(); iter != cMap_.end(); ++iter) {
        auto element = iter->second->want.GetElement();
        auto elementTmp = want.GetElement();
        if (element.GetBundleName() == elementTmp.GetBundleName() &&
            element.GetAbilityName() == elementTmp.GetAbilityName()) {
            return iter->second;
        }
    }
    HILOG_ERROR("GetConnectInfo called with want obj return nullptr");
    return nullptr;
}

int FileAccessHelper::getUserId()
{
    int uid = IPCSkeleton::GetCallingUid();
    int userId = uid / UID_TRANSFORM_DIVISOR;
    return userId;
}

std::string FileAccessHelper::GetKeyOfWantsMap(const AAFwk::Want &want)
{
    for (auto iter = FileAccessHelper::wantsMap_.begin(); iter != FileAccessHelper::wantsMap_.end(); ++iter) {
        auto element = iter->second.GetElement();
        auto elementTmp = want.GetElement();
        if (element.GetBundleName() == elementTmp.GetBundleName() &&
            element.GetAbilityName() == elementTmp.GetAbilityName()) {
            return iter->first;
        }
    }
    HILOG_ERROR("GetKeyOfWantsMap called return nullptr");
    return nullptr;
}

void FileAccessHelper::InsertConnectInfo(const std::string &key,
                                         const AAFwk::Want &want,
                                         const sptr<IFileAccessExtBase> &fileAccessExtProxy,
                                         sptr<FileAccessExtConnection> fileAccessExtConnection)
{
    std::shared_ptr<ConnectInfo> connectInfo = GetConnectInfo(key);
    if (connectInfo == nullptr) {
        std::shared_ptr<ConnectInfo> connectInfo = std::make_shared<ConnectInfo>();
        if (connectInfo == nullptr) {
            HILOG_ERROR("InsertConnectInfo called with connectInfo == nullptr");
            return ;
        }
        connectInfo->want = want;
        connectInfo->fileAccessExtConnection = fileAccessExtConnection;
        cMap_.insert(std::pair<std::string, std::shared_ptr<ConnectInfo>>(key, connectInfo));
    } else {
        connectInfo->want = want;
        connectInfo->fileAccessExtConnection = fileAccessExtConnection;
    }
}

std::shared_ptr<FileAccessHelper> FileAccessHelper::Creator(
    const std::shared_ptr<OHOS::AbilityRuntime::Context> &context)
{
    if (context == nullptr) {
        HILOG_ERROR("FileAccessHelper::Creator failed, context == nullptr");
        return nullptr;
    }

    sptr<AppExecFwk::IBundleMgr> bm = FileAccessHelper::GetBundleMgrProxy();
    FileAccessHelper::wantsMap_.clear();
    std::unordered_map<std::string, std::shared_ptr<ConnectInfo>> cMap;
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    int userId = FileAccessHelper::getUserId();
    bool ret = bm->QueryExtensionAbilityInfos(
        AppExecFwk::ExtensionAbilityType::FILEACCESS_EXTENSION, userId, extensionInfos);
    if (!ret) {
        HILOG_ERROR("FileAccessHelper::Creator QueryExtensionAbilityInfos failed");
        return nullptr;
    }

    for (size_t i = 0; i < extensionInfos.size(); i++) {
        AAFwk::Want wantTem;
        wantTem.SetElementName(extensionInfos[i].bundleName, extensionInfos[i].name);
        sptr<FileAccessExtConnection> fileAccessExtConnection = new(std::nothrow) FileAccessExtConnection();
        if (fileAccessExtConnection == nullptr) {
            HILOG_ERROR("new fileAccessExtConnection fail");
            return nullptr;
        }

        if (!fileAccessExtConnection->IsExtAbilityConnected()) {
            fileAccessExtConnection->ConnectFileExtAbility(wantTem, context->GetToken());
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
        FileAccessHelper::wantsMap_.insert(std::pair<std::string, AAFwk::Want>(extensionInfos[i].bundleName, wantTem));

        connectInfo->want = wantTem;
        connectInfo->fileAccessExtConnection = fileAccessExtConnection;
        cMap.insert(std::pair<std::string, std::shared_ptr<ConnectInfo>>(extensionInfos[i].bundleName, connectInfo));
    }
    FileAccessHelper *ptrFileAccessHelper = new (std::nothrow) FileAccessHelper(context, cMap);
    if (ptrFileAccessHelper == nullptr) {
        HILOG_ERROR("FileAccessHelper::Creator failed, create FileAccessHelper failed");
        return nullptr;
    }

    return std::shared_ptr<FileAccessHelper>(ptrFileAccessHelper);
}

std::shared_ptr<FileAccessHelper> FileAccessHelper::Creator(
    const std::shared_ptr<OHOS::AbilityRuntime::Context> &context, const std::vector<AAFwk::Want> &wants)
{
    if (context == nullptr) {
        HILOG_ERROR("FileAccessHelper::Creator failed, context == nullptr");
        return nullptr;
    }

    if (wants.size() == 0) {
        HILOG_ERROR("FileAccessHelper::Creator failed, wants is empty");
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
            fileAccessExtConnection->ConnectFileExtAbility(wants[i], context->GetToken());
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
        string bundleName = FileAccessHelper::GetKeyOfWantsMap(wants[i]);
        cMap.insert(std::pair<std::string, std::shared_ptr<ConnectInfo>>(bundleName, connectInfo));
    }
    FileAccessHelper *ptrFileAccessHelper = new (std::nothrow) FileAccessHelper(context, cMap);
    if (ptrFileAccessHelper == nullptr) {
        HILOG_ERROR("Creator failed, create FileAccessHelper failed");
        return nullptr;
    }

    return std::shared_ptr<FileAccessHelper>(ptrFileAccessHelper);
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
        string bundleName = FileAccessHelper::GetKeyOfWantsMap(wants[i]);
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
    FileAccessHelper::wantsMap_.clear();
    return true;
}

sptr<IFileAccessExtBase> FileAccessHelper::GetProxyByUri(Uri &uri)
{
    std::string bundleName;
    if (!GetBundleNameFromPath(uri.GetPath(), bundleName)) {
        HILOG_ERROR("Get BundleName failed.");
        return nullptr;
    }

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

int FileAccessHelper::OpenFile(Uri &uri, int flags)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "OpenFile");
    int fd = ERR_ERROR;
    if (!CheckUri(uri)) {
        HILOG_ERROR("Uri format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return fd;
    }

    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(uri);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return fd;
    }

    fd = fileExtProxy->OpenFile(uri, flags);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return fd;
}

int FileAccessHelper::CreateFile(Uri &parent, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CreateFile");
    int index = ERR_ERROR;
    if (!CheckUri(parent)) {
        HILOG_ERROR("Uri format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return index;
    }

    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(parent);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return index;
    }

    index = fileExtProxy->CreateFile(parent, displayName, newFile);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return index;
}

int FileAccessHelper::Mkdir(Uri &parent, const std::string &displayName, Uri &newDir)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Mkdir");
    int index = ERR_ERROR;
    if (!CheckUri(parent)) {
        HILOG_ERROR("Uri format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return index;
    }

    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(parent);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return index;
    }

    index = fileExtProxy->Mkdir(parent, displayName, newDir);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return index;
}

int FileAccessHelper::Delete(Uri &selectFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Delete");
    int index = ERR_ERROR;
    if (!CheckUri(selectFile)) {
        HILOG_ERROR("Uri format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return index;
    }

    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(selectFile);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return index;
    }

    index = fileExtProxy->Delete(selectFile);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return index;
}

int FileAccessHelper::Move(Uri &sourceFile, Uri &targetParent, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Move");
    Uri sourceFileUri(sourceFile.ToString());
    Uri targetParentUri(targetParent.ToString());
    if (!CheckUri(sourceFile)) {
        HILOG_ERROR("sourceFile format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_INVALID_URI;
    }

    if (!CheckUri(targetParent)) {
        HILOG_ERROR("targetParent format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_INVALID_URI;
    }

    if (sourceFileUri.GetScheme() != targetParentUri.GetScheme()) {
        HILOG_WARN("Operation failed, move not supported");
        return ERR_OPERATION_NOT_PERMITTED;
    }

    int index = ERR_ERROR;
    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(sourceFile);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return index;
    }

    index = fileExtProxy->Move(sourceFile, targetParent, newFile);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return index;
}

int FileAccessHelper::Rename(Uri &sourceFile, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Rename");
    int index = ERR_ERROR;
    if (!CheckUri(sourceFile)) {
        HILOG_ERROR("sourceFile format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return index;
    }

    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(sourceFile);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return index;
    }

    index = fileExtProxy->Rename(sourceFile, displayName, newFile);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return index;
}

std::vector<FileInfo> FileAccessHelper::ListFile(Uri &sourceFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "ListFile");
    std::vector<FileInfo> results;
    if (!CheckUri(sourceFile)) {
        HILOG_ERROR("sourceFile format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return results;
    }

    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(sourceFile);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return results;
    }

    results = fileExtProxy->ListFile(sourceFile);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return results;
}

std::vector<DeviceInfo> FileAccessHelper::GetRoots()
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetRoots");
    std::vector<DeviceInfo> rootsInfo;
    if (!GetProxy()) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return rootsInfo;
    }

    for (auto iter = cMap_.begin(); iter != cMap_.end(); ++iter) {
        auto connectInfo = iter->second;
        auto fileAccessExtProxy = connectInfo->fileAccessExtConnection->GetFileExtProxy();
        std::vector<DeviceInfo> results;
        if (fileAccessExtProxy) {
            AddFileAccessDeathRecipient(fileAccessExtProxy->AsObject());
        }
        results = fileAccessExtProxy->GetRoots();
        rootsInfo.insert(rootsInfo.end(), results.begin(), results.end());
    }

    return rootsInfo;
}

std::vector<AAFwk::Want> FileAccessHelper::GetRegisterFileAccessExtAbilityInfo()
{
    std::vector<AAFwk::Want> wants;
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    sptr<AppExecFwk::IBundleMgr> bm = FileAccessHelper::GetBundleMgrProxy();
    int userId = FileAccessHelper::getUserId();
    bool ret = bm->QueryExtensionAbilityInfos(
        AppExecFwk::ExtensionAbilityType::FILEACCESS_EXTENSION, userId, extensionInfos);
    if (!ret) {
        HILOG_ERROR("FileAccessHelper::GetRegisterFileAccessExtAbilityInfo QueryExtensionAbilityInfos error");
        return wants;
    }

    FileAccessHelper::wantsMap_.clear();
    for (size_t i = 0; i < extensionInfos.size(); i++) {
        AAFwk::Want want;
        want.SetElementName(extensionInfos[i].bundleName, extensionInfos[i].name);
        FileAccessHelper::wantsMap_.insert(std::pair<std::string, AAFwk::Want>(extensionInfos[i].bundleName, want));
        wants.push_back(want);
    }

    return wants;
}

int FileAccessHelper::IsFileExist(Uri &uri, bool &isExist)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "IsFileExist");
    int ret = ERR_ERROR;
    if (!CheckUri(uri)) {
        HILOG_ERROR("uri format check error.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    sptr<IFileAccessExtBase> fileExtProxy = GetProxyByUri(uri);
    if (fileExtProxy == nullptr) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    ret = fileExtProxy->IsFileExist(uri, isExist);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessHelper::On(std::shared_ptr<INotifyCallback> &callback)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "On");
    if (callback == nullptr) {
        HILOG_ERROR("failed with invalid callback");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_ERROR;
    }

    if (!GetProxy()) {
        HILOG_ERROR("failed with invalid fileAccessExtProxy");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    std::lock_guard<std::mutex> lock(notifyAgentMutex_);
    if (notifyAgent_ == nullptr) {
        notifyAgent_ = new(std::nothrow) FileAccessNotifyAgent(callback);
        if (notifyAgent_ == nullptr) {
            HILOG_ERROR("new FileAccessNotifyAgent fail");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return ERR_ERROR;
        }
    }

    int errorCode = ERR_OK;
    for (auto iter = cMap_.begin(); iter != cMap_.end(); ++iter) {
        auto connectInfo = iter->second;
        auto fileAccessExtProxy = connectInfo->fileAccessExtConnection->GetFileExtProxy();
        if (fileAccessExtProxy == nullptr) {
            HILOG_ERROR("fileAccessExtProxy RegisterNotify fail");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return ERR_IPC_ERROR;
        }

        auto ret = fileAccessExtProxy->RegisterNotify(notifyAgent_);
        if (ret != ERR_OK) {
            HILOG_ERROR("fileAccessExtProxy RegisterNotify fail, bundleName:%{public}s, ret:%{public}d.",
                connectInfo->want.GetElement().GetBundleName().c_str(), ret);
            errorCode = ret;
        }
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return errorCode;
}

int FileAccessHelper::Off()
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Off");
    std::lock_guard<std::mutex> lock(notifyAgentMutex_);
    if (notifyAgent_ == nullptr) {
        HILOG_ERROR("not registered notify");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_NOTIFY_NOT_EXIST;
    }

    int errorCode = ERR_OK;
    for (auto iter = cMap_.begin(); iter != cMap_.end(); ++iter) {
        auto connectInfo = iter->second;
        auto fileAccessExtProxy = connectInfo->fileAccessExtConnection->GetFileExtProxy();
        if (fileAccessExtProxy == nullptr) {
            HILOG_INFO("fileAccessExtProxy UnregisterNotify fail, bundleName:%{public}s",
                connectInfo->want.GetElement().GetBundleName().c_str());
            continue;
        }

        auto ret = fileAccessExtProxy->UnregisterNotify(notifyAgent_);
        if (ret != ERR_OK) {
            HILOG_ERROR("fileAccessExtProxy UnregisterNotify fail, bundleName:%{public}s, ret:%{public}d.",
                connectInfo->want.GetElement().GetBundleName().c_str(), ret);
            errorCode = ret;
        }
    }

    notifyAgent_.clear();
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return errorCode;
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