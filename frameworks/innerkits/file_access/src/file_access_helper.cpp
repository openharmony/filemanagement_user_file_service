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

#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "ifile_access_ext_base.h"

namespace OHOS {
namespace FileAccessFwk {
std::unordered_map<std::string, AAFwk::Want> FileAccessHelper::wantsMap_;

FileAccessHelper::FileAccessHelper(const std::shared_ptr<OHOS::AbilityRuntime::Context> &context,
    const std::unordered_map<std::string, std::shared_ptr<ConnectInfo>> &cMap)
{
    token_ = context->GetToken();
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

std::shared_ptr<ConnectInfo> FileAccessHelper::GetConnectInfo(const std::string &key)
{
    auto iterator = cMap_.find(key);
    if (iterator != cMap_.end()) {
        return iterator->second;
    }
    HILOG_ERROR("GetConnectInfo called with key return nullptr");
    return nullptr;
}

std::shared_ptr<ConnectInfo> FileAccessHelper::GetConnectInfo(Uri &uri)
{
    for (auto iter = cMap_.begin(); iter != cMap_.end(); ++iter) {
        Uri key(iter->first);
        if (key.GetScheme().compare(uri.GetScheme()) == 0) {
            return iter->second;
        }
    }

    HILOG_ERROR("GetConnectInfo called with uri return nullptr");
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
                                         std::shared_ptr<FileAccessExtConnection> fileAccessExtConnection)
{
    Uri uri(key);
    std::shared_ptr<ConnectInfo> connectInfo = GetConnectInfo(uri);
    if (connectInfo == nullptr) {
        std::shared_ptr<ConnectInfo> connectInfo = std::make_shared<ConnectInfo>();
        if (connectInfo == nullptr) {
            HILOG_ERROR("InsertConnectInfo called with connectInfo == nullptr");
            return ;
        }
        connectInfo->want = want;
        connectInfo->fileAccessExtProxy = fileAccessExtProxy;
        connectInfo->fileAccessExtConnection = fileAccessExtConnection;
        cMap_.insert(std::pair<std::string, std::shared_ptr<ConnectInfo>>(key, connectInfo));
    } else {
        connectInfo->want = want;
        connectInfo->fileAccessExtProxy = fileAccessExtProxy;
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

    sptr<AppExecFwk::IBundleMgr> bm = FileExtInfo::GetBundleMgrProxy();
    FileAccessHelper::wantsMap_.clear();
    std::unordered_map<std::string, std::shared_ptr<ConnectInfo>> cMap;
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    bool ret = bm->QueryExtensionAbilityInfos(
        AppExecFwk::ExtensionAbilityType::FILEACCESS_EXTENSION, 100, extensionInfos);
    if (!ret) {
        HILOG_ERROR("FileAccessHelper::Creator QueryExtensionAbilityInfos failed");
        return nullptr;
    }

    for (size_t i = 0; i < extensionInfos.size(); i++) {
        AAFwk::Want wantTem;
        wantTem.SetElementName(extensionInfos[i].bundleName, extensionInfos[i].name);
        sptr<IFileAccessExtBase> fileExtProxy = nullptr;
        std::shared_ptr<FileAccessExtConnection> fileAccessExtConnection =  std::make_shared<FileAccessExtConnection>();
        if (!fileAccessExtConnection->IsExtAbilityConnected()) {
            fileAccessExtConnection->ConnectFileExtAbility(wantTem, context->GetToken());
        }

        fileExtProxy = fileAccessExtConnection->GetFileExtProxy();
        if (fileExtProxy == nullptr) {
            HILOG_WARN("Creator get invalid fileExtProxy");
        }

        std::shared_ptr<ConnectInfo> connectInfo = std::make_shared<ConnectInfo>();
        if (connectInfo == nullptr) {
            HILOG_ERROR("Creator, connectInfo == nullptr");
            return nullptr;
        }
        FileAccessHelper::wantsMap_.insert(std::pair<std::string, AAFwk::Want>(extensionInfos[i].uri, wantTem));

        connectInfo->want = wantTem;
        connectInfo->fileAccessExtProxy = fileExtProxy;
        connectInfo->fileAccessExtConnection = fileAccessExtConnection;
        cMap.insert(std::pair<std::string, std::shared_ptr<ConnectInfo>>(extensionInfos[i].uri, connectInfo));
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
        sptr<IFileAccessExtBase> fileExtProxy = nullptr;
        std::shared_ptr<FileAccessExtConnection> fileAccessExtConnection =  std::make_shared<FileAccessExtConnection>();
        if (!fileAccessExtConnection->IsExtAbilityConnected()) {
            fileAccessExtConnection->ConnectFileExtAbility(wants[i], context->GetToken());
        }

        fileExtProxy = fileAccessExtConnection->GetFileExtProxy();
        if (fileExtProxy == nullptr) {
            HILOG_WARN("Creator get invalid fileExtProxy");
        }

        std::shared_ptr<ConnectInfo> connectInfo = std::make_shared<ConnectInfo>();
        if (connectInfo == nullptr) {
            HILOG_ERROR("Creator, connectInfo == nullptr");
            return nullptr;
        }

        connectInfo->want = wants[i];
        connectInfo->fileAccessExtProxy = fileExtProxy;
        connectInfo->fileAccessExtConnection = fileAccessExtConnection;
        string uriTmp = FileAccessHelper::GetKeyOfWantsMap(wants[i]);
        cMap.insert(std::pair<std::string, std::shared_ptr<ConnectInfo>>(uriTmp, connectInfo));
    }
    FileAccessHelper *ptrFileAccessHelper = new (std::nothrow) FileAccessHelper(context, cMap);
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
        iter->second->fileAccessExtProxy = nullptr;
    }
    cMap_.clear();
    token_ = nullptr;
    FileAccessHelper::wantsMap_.clear();
    return true;
}

sptr<IFileAccessExtBase> FileAccessHelper::GetProxy(Uri &uri)
{
    auto connectInfo = GetConnectInfo(uri);
    if (connectInfo == nullptr) {
        HILOG_ERROR("GetProxy failed with invalid connectInfo");
        return nullptr;
    }

    if (!connectInfo->fileAccessExtConnection->IsExtAbilityConnected()) {
        connectInfo->fileAccessExtConnection->ConnectFileExtAbility(connectInfo->want, token_);
    }

    connectInfo->fileAccessExtProxy = connectInfo->fileAccessExtConnection->GetFileExtProxy();
    if (isSystemCaller_ && connectInfo->fileAccessExtProxy) {
        AddFileAccessDeathRecipient(connectInfo->fileAccessExtProxy->AsObject());
    }

    if (connectInfo->fileAccessExtProxy == nullptr) {
        HILOG_ERROR("GetProxy failed with invalid fileAccessExtProxy");
        return nullptr;
    }

    return connectInfo->fileAccessExtProxy;
}

bool FileAccessHelper::GetProxy()
{
    for (auto iter = cMap_.begin(); iter != cMap_.end(); ++iter) {
        auto connectInfo = iter->second;
        std::vector<DeviceInfo> results;
        if (!connectInfo->fileAccessExtConnection->IsExtAbilityConnected()) {
            connectInfo->fileAccessExtConnection->ConnectFileExtAbility(connectInfo->want, token_);
        }
        connectInfo->fileAccessExtProxy = connectInfo->fileAccessExtConnection->GetFileExtProxy();
        if (isSystemCaller_ && connectInfo->fileAccessExtProxy) {
            AddFileAccessDeathRecipient(connectInfo->fileAccessExtProxy->AsObject());
        }

        if (connectInfo->fileAccessExtProxy == nullptr) {
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
    sptr<IFileAccessExtBase> fileExtProxy = GetProxy(uri);
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
    sptr<IFileAccessExtBase> fileExtProxy = GetProxy(parent);
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
    sptr<IFileAccessExtBase> fileExtProxy = GetProxy(parent);
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
    sptr<IFileAccessExtBase> fileExtProxy = GetProxy(selectFile);
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
    int index = ERR_ERROR;
    sptr<IFileAccessExtBase> fileExtProxy = GetProxy(sourceFile);
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
    sptr<IFileAccessExtBase> fileExtProxy = GetProxy(sourceFile);
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
    sptr<IFileAccessExtBase> fileExtProxy = GetProxy(sourceFile);
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
        std::vector<DeviceInfo> results;
        if (isSystemCaller_ && connectInfo->fileAccessExtProxy) {
            AddFileAccessDeathRecipient(connectInfo->fileAccessExtProxy->AsObject());
        }
        results = connectInfo->fileAccessExtProxy->GetRoots();
        rootsInfo.insert(rootsInfo.end(), results.begin(), results.end());
    }

    return rootsInfo;
}

std::vector<AAFwk::Want> FileAccessHelper::GetRegisterFileAccessExtAbilityInfo()
{
    std::vector<AAFwk::Want> wants;
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    sptr<AppExecFwk::IBundleMgr> bm = FileExtInfo::GetBundleMgrProxy();
    bool ret = bm->QueryExtensionAbilityInfos(
        AppExecFwk::ExtensionAbilityType::FILEACCESS_EXTENSION, 100, extensionInfos);
    if (!ret) {
        HILOG_ERROR("FileAccessHelper::GetRegisterFileAccessExtAbilityInfo QueryExtensionAbilityInfos error");
        return wants;
    }

    FileAccessHelper::wantsMap_.clear();
    for (size_t i = 0; i < extensionInfos.size(); i++) {
        AAFwk::Want want;
        want.SetElementName(extensionInfos[i].bundleName, extensionInfos[i].name);
        FileAccessHelper::wantsMap_.insert(std::pair<std::string, AAFwk::Want>(extensionInfos[i].uri, want));
        wants.push_back (want);
    }

    return wants;
}

bool FileAccessHelper::AddService(AAFwk::Want &want)
{
    sptr<IFileAccessExtBase> fileExtProxy = nullptr;
    std::shared_ptr<FileAccessExtConnection> fileAccessExtConnection =  std::make_shared<FileAccessExtConnection>();
    if (!fileAccessExtConnection->IsExtAbilityConnected()) {
        fileAccessExtConnection->ConnectFileExtAbility(want, token_);
    }
    fileExtProxy = fileAccessExtConnection->GetFileExtProxy();
    if (fileExtProxy == nullptr) {
        HILOG_WARN("AddService get invalid fileExtProxy");
    }

    std::shared_ptr<ConnectInfo> connectInfo = std::make_shared<ConnectInfo>();
    if (connectInfo == nullptr) {
        HILOG_ERROR("AddService, connectInfo == nullptr");
        return false;
    }

    string uriTmp = FileAccessHelper::GetKeyOfWantsMap(want);
    InsertConnectInfo(uriTmp, want, fileExtProxy, fileAccessExtConnection);
    return true;
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