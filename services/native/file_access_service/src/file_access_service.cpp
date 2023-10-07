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

#include "file_access_service.h"

#include <unistd.h>
#include "user_access_tracer.h"
#include "file_access_framework_errno.h"
#include "file_access_ext_connection.h"
#include "file_access_extension_info.h"
#include "file_access_ext_connection.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "system_ability_definition.h"
#include "iservice_registry.h"

using namespace std;
namespace OHOS {
namespace FileAccessFwk {
namespace {
    auto pms = FileAccessService::GetInstance();
    const bool G_REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(pms.GetRefPtr());
}
sptr<FileAccessService> FileAccessService::instance_;
mutex FileAccessService::mutex_;

constexpr int32_t NOTIFY_MAX_NUM = 32;
constexpr int32_t NOTIFY_TIME_INTERVAL = 500;
constexpr int32_t MAX_WAIT_TIME = 20;

sptr<FileAccessService> FileAccessService::GetInstance()
{
    if (instance_ != nullptr) {
        return instance_;
    }

    lock_guard<mutex> lock(mutex_);
    if (instance_ == nullptr) {
        instance_ = new FileAccessService();
        if (instance_ == nullptr) {
            HILOG_ERROR("GetInstance nullptr");
            return instance_;
        }
    }
    return instance_;
}

FileAccessService::FileAccessService() : SystemAbility(FILE_ACCESS_SERVICE_ID, false)
{
    InitTimer();
    if (extensionDeathRecipient_ == nullptr) {
        extensionDeathRecipient_ = new ExtensionDeathRecipient();
    }
    if (observerDeathRecipient_  == nullptr) {
        observerDeathRecipient_ = new ObserverDeathRecipient();
    }
}

void FileAccessService::OnStart()
{
    UserAccessTracer trace;
    trace.Start("OnStart");
    if (!Publish(FileAccessService::GetInstance())) {
        HILOG_ERROR("OnStart register to system ability manager failed");
        return;
    }
    ready_ = true;
    return;
}

void FileAccessService::OnStop()
{
    UserAccessTracer trace;
    trace.Start("OnStop");
    if (!ready_) {
        HILOG_ERROR("OnStop is not ready, nothing to do");
        return;
    }
    ready_ = false;
}

int32_t FileAccessService::Dump(int32_t fd, const vector<u16string> &args)
{
    return ERR_OK;
}

bool FileAccessService::IsServiceReady() const
{
    return ready_;
}

static bool IsParentUri(const string &comparedUriStr, string &srcUriStr)
{
    if ((comparedUriStr.empty()) || (srcUriStr.empty())) {
        HILOG_ERROR("Uri is empty");
        return false;
    }
    size_t slashIndex = srcUriStr.rfind("/");
    if (slashIndex != string::npos) {
        if (comparedUriStr.compare(srcUriStr.substr(0, slashIndex)) == 0) {
            return true;
        }
    }
    return false;
}

static bool IsChildUri(const string &comparedUriStr, string &srcUriStr)
{
    if ((comparedUriStr.empty()) || (srcUriStr.empty())) {
        HILOG_ERROR("Uri is empty");
        return false;
    }
    size_t slashIndex = comparedUriStr.rfind("/");
    if (slashIndex != string::npos) {
        if (srcUriStr.compare(comparedUriStr.substr(0, slashIndex)) == 0) {
            return true;
        }
    }
    return false;
}

int32_t FileAccessService::ConnectExtension()
{
    HILOG_ERROR("cjw enter ConnectExtension");
    lock_guard<mutex> lock(mutex_);
    if (extensionProxy_ != nullptr) {
        return ERR_OK;
    }
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        HILOG_ERROR("Samgr is nullptr");
        return E_CONNECT;
    }

    auto remote = samgr->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID);
    if (remote == nullptr) {
        HILOG_ERROR("get remote failed");
        return E_CONNECT;
    }

    AAFwk::Want want;
    want.SetElementName("com.ohos.UserFile.ExternalFileManager", "FileExtensionAbility");
    sptr<FileAccessExtConnection> fileAccessExtConnection(new(std::nothrow) FileAccessExtConnection());
    if (fileAccessExtConnection == nullptr) {
        HILOG_ERROR("new fileAccessExtConnection fail");
        return E_CONNECT;
    }

    if (!fileAccessExtConnection->IsExtAbilityConnected()) {
        fileAccessExtConnection->ConnectFileExtAbility(want, remote);
    }
    extensionProxy_ = fileAccessExtConnection->GetFileExtProxy();
    auto object = extensionProxy_->AsObject();
    object->AddDeathRecipient(extensionDeathRecipient_);
    return ERR_OK;
}

void FileAccessService::ResetProxy()
{
    if (extensionProxy_ != nullptr && extensionDeathRecipient_ != nullptr) {
        extensionProxy_->AsObject()->RemoveDeathRecipient(extensionDeathRecipient_);
    }
    if (ConnectExtension() != ERR_OK) {
        HILOG_ERROR("ResetProxy failed");
    }
}

void FileAccessService::ExtensionDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    HILOG_ERROR("FileAccessService::ExtensionDeathRecipient::OnRemoteDied, remote obj died.");
    FileAccessService::GetInstance()->ResetProxy();
}

void FileAccessService::ObserverDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    HILOG_ERROR("FileAccessService::ObserverDeathRecipient::OnRemoteDied, remote obj died.");
    if (remote == nullptr || remote.promote() == nullptr) {
        return;
    }
    sptr<IFileAccessObserver> observer = iface_cast<IFileAccessObserver>(remote.promote());
    FileAccessService::GetInstance()->CleanRelativeObserver(observer);
}

void FileAccessService::CleanRelativeObserver(const sptr<IFileAccessObserver> &observer)
{
    shared_ptr<ObserverContext> obsContext = make_shared<ObserverContext>(observer);
    uint32_t code = obsManager_.getId([obsContext](const shared_ptr<ObserverContext>  &afterContext) {
        return obsContext->EqualTo(afterContext);
    });
    for (auto pair : relationshipMap_) {
        auto codeList = pair.second->obsCodeList_;
        auto haveCodeIter = find_if(codeList.begin(), codeList.end(),
            [code](const uint32_t &listCode) { return code == listCode; });
        if (haveCodeIter != codeList.end()) {
            Uri uri(pair.first);
            UnregisterNotify(uri, observer);
        }
    }
}

int32_t FileAccessService::RegisterNotify(Uri uri, bool notifyForDescendants, const sptr<IFileAccessObserver> &observer)
{
    UserAccessTracer trace;
    trace.Start("RegisterNotify");
    shared_ptr<ObserverContext> obsContext = make_shared<ObserverContext>(observer);
    // find if obsManager_ has this callback.
    uint32_t code = obsManager_.getId([obsContext](const shared_ptr<ObserverContext> &afterContext) {
        return obsContext->EqualTo(afterContext);
    });
    if (code == HolderManager<shared_ptr<ObserverContext>>::CODE_CAN_NOT_FIND) {
        // this is new callback, save this context
        obsContext->Ref();
        code = obsManager_.save(obsContext);
        auto object = obsContext->obs_->AsObject();
        object->AddDeathRecipient(observerDeathRecipient_);
    } else {
        // this callback is already in manager, add ref.
        obsManager_.get(code)->Ref();
    }
    string uriStr = uri.ToString();
    lock_guard<mutex> lock(nodeMutex_);
    auto iter = relationshipMap_.find(uriStr);
    shared_ptr<ObserverNode> obsNode;
    if (iter != relationshipMap_.end()) {
        obsNode = iter->second;
        // this node has this callback or not, if has this, unref manager.
        auto haveCodeIter = find_if(obsNode->obsCodeList_.begin(), obsNode->obsCodeList_.end(),
            [code](const uint32_t &listCode) { return code == listCode; });
        if (haveCodeIter != obsNode->obsCodeList_.end()) {
            obsManager_.get(code)->UnRef();
            if (obsNode->needChildNote_ == notifyForDescendants) {
                HILOG_DEBUG("Register same uri and same callback and same notifyForDescendants");
                return ERR_OK;
            }
            // need modify obsNode notifyForDescendants
            obsNode->needChildNote_ = notifyForDescendants;
            HILOG_DEBUG("Register same uri and same callback but need modify notifyForDescendants");
            return ERR_OK;
        }
        obsNode->obsCodeList_.push_back(code);
        return ERR_OK;
    } else {
        if (ConnectExtension() != ERR_OK) {
            HILOG_ERROR("cjw Creator get invalid fileExtProxy");
            return E_CONNECT;
        }

        size_t uriIndex = uriStr.find("file://");
        Uri tempUri(uriStr.substr(uriIndex));
        HILOG_ERROR("startWatcher tempUri = %{public}s", tempUri.ToString().c_str());
        if(extensionProxy_ == nullptr) {
            HILOG_ERROR("Creator get invalid fileExtProxy");
            return E_CONNECT;
        }
        extensionProxy_->StartWatcher(tempUri);
    }
    obsNode = make_shared<ObserverNode>(notifyForDescendants);
    // add new node relations.
    for (auto &[comUri, node] : relationshipMap_) {
        if (IsParentUri(comUri, uriStr)) {
            obsNode->parent_ = node;
            node->children_.push_back(obsNode);
        }
        if (IsChildUri(comUri, uriStr)) {
            obsNode->children_.push_back(node);
            node->parent_ = obsNode;
        }
    }
    // obsCodeList_ is to save callback number
    obsNode->obsCodeList_.push_back(code);
    relationshipMap_.insert(make_pair(uriStr, obsNode));
    return ERR_OK;
}

void FileAccessService::RemoveRelations(string &uriStr, shared_ptr<ObserverNode> obsNode)
{
    lock_guard<mutex> lock(nodeMutex_);
    for (auto &[comUri, node] : relationshipMap_) {
        auto childrenIter = find_if(node->children_.begin(), node->children_.end(),
            [obsNode](const shared_ptr<ObserverNode> &obsListNode) { return obsNode == obsListNode; });
        if (childrenIter != node->children_.end()) {
            node->children_.erase(childrenIter);
        }
        if (IsChildUri(comUri, uriStr)) {
            node->parent_ = nullptr;
        }
    }
    relationshipMap_.erase(uriStr);
}

int FileAccessService::FindUri(const string &uriStr, shared_ptr<ObserverNode> &outObsNode)
{
    lock_guard<mutex> lock(nodeMutex_);
    auto iter = relationshipMap_.find(uriStr);
    if (iter == relationshipMap_.end()) {
        HILOG_ERROR("Can not find uri");
        return E_CAN_NOT_FIND_URI;
    }
    outObsNode = iter->second;
    return ERR_OK;
}

int32_t FileAccessService::CleanAllNotify(Uri uri)
{
    UserAccessTracer trace;
    trace.Start("CleanAllNotify");
    string uriStr = uri.ToString();
    shared_ptr<ObserverNode> obsNode;
    if (FindUri(uriStr, obsNode) != ERR_OK) {
        HILOG_ERROR("Can not find unregisterNotify uri");
        return E_CAN_NOT_FIND_URI;
    }
    for (auto code : obsNode->obsCodeList_) {
        auto context = obsManager_.get(code);
        context->UnRef();
        if (!context->IsValid()) {
            obsManager_.release(code);
        }
    }
    if (ConnectExtension() != ERR_OK) {
        HILOG_ERROR("cjw Creator get invalid fileExtProxy");
        return E_CONNECT;
    }

    size_t uriIndex = uriStr.find("file://");
    Uri tempUri(uriStr.substr(uriIndex));
    HILOG_ERROR("StopWatcher tempUri = %{public}s", tempUri.ToString().c_str());
    if(extensionProxy_ == nullptr) {
        HILOG_ERROR("Creator get invalid fileExtProxy");
        return E_CONNECT;
    }
    extensionProxy_->StopWatcher(tempUri);
    RemoveRelations(uriStr, obsNode);
    return ERR_OK;
}

int32_t FileAccessService::UnregisterNotify(Uri uri, const sptr<IFileAccessObserver> &observer)
{
    UserAccessTracer trace;
    trace.Start("UnregisterNotify");
    if (observer->AsObject() == nullptr) {
        HILOG_ERROR("UnregisterNotify failed with invalid observer");
        return E_IPCS;
    }
    string uriStr = uri.ToString();
    shared_ptr<ObserverNode> obsNode;
    if (FindUri(uriStr, obsNode) != ERR_OK) {
        HILOG_ERROR("Can not find unregisterNotify uri");
        return E_CAN_NOT_FIND_URI;
    }
    // find if obsManager_ has this callback.
    shared_ptr<ObserverContext> obsContext = make_shared<ObserverContext>(observer);
    uint32_t code = obsManager_.getId([obsContext](const shared_ptr<ObserverContext>  &afterContext) {
        return obsContext->EqualTo(afterContext);
    });
    if (code == HolderManager<shared_ptr<ObserverContext>>::CODE_CAN_NOT_FIND) {
        HILOG_ERROR("Can not find observer");
        return E_CALLBACK_IS_NOT_REGISTER;
    }
    // find if this node has this callback.
    auto haveCodeIter = find_if(obsNode->obsCodeList_.begin(), obsNode->obsCodeList_.end(),
        [code](const uint32_t &listCode) { return code == listCode; });
    if (haveCodeIter == obsNode->obsCodeList_.end()) {
        HILOG_ERROR("Uri node observer list don not has this observer");
        return E_CALLBACK_AND_URI_HAS_NOT_RELATIONS;
    }
    obsNode->obsCodeList_.erase(haveCodeIter);
    obsManager_.get(code)->UnRef();
    // node has other observers, do not need remove.
    if (obsNode->obsCodeList_.size() != 0) {
        HILOG_DEBUG("Has code do not stopWatcher");
        return ERR_OK;
    }
    // if data refcount is invalid, release this code.
    if (!obsManager_.get(code)->IsValid()) {
        obsManager_.release(code);
    }
    if (ConnectExtension() != ERR_OK) {
        HILOG_ERROR("cjw Creator get invalid fileExtProxy");
        return E_CONNECT;
    }

    size_t uriIndex = uriStr.find("file://");
    Uri tempUri(uriStr.substr(uriIndex));
    HILOG_ERROR("StopWatcher tempUri = %{public}s", tempUri.ToString().c_str());
    if(extensionProxy_ == nullptr) {
        HILOG_ERROR("Creator get invalid fileExtProxy");
        return E_CONNECT;
    }
    extensionProxy_->StopWatcher(tempUri);
    RemoveRelations(uriStr, obsNode);
    return ERR_OK;
}

void FileAccessService::SendListNotify(string uriStr, NotifyType notifyType, const std::vector<uint32_t> &list)
{
    if (onDemandTimer_ == nullptr) {
        HILOG_ERROR("onDemandTimer_ is nullptr");
        return;
    }
    onDemandTimer_->start();
    for (uint32_t code : list) {
        if (obsManager_.get(code) == nullptr) {
            HILOG_ERROR("Failed to get obs code = %{private}ud", code);
            continue;
        }
        auto context = obsManager_.get(code);
        if (uriStr == EXTERNAL_ROOT) {
            vector<string> uris = {uriStr};
            NotifyMessage notifyMessage{notifyType, uris};
            context->obs_->OnChange(notifyMessage);
            continue;
        }
        lock_guard<mutex> lock(context->mapMutex_);
        if (context->notifyMap_.find(notifyType) != context->notifyMap_.end()) {
            context->notifyMap_[notifyType].push_back(uriStr);
        } else {
            vector<string> uris{uriStr};
            context->notifyMap_.emplace(notifyType, uris);
        }
        if (context->notifyMap_[notifyType].size() >= NOTIFY_MAX_NUM) {
            NotifyMessage notifyMessage;
            notifyMessage.notifyType_ = notifyType;
            notifyMessage.uris_ = context->notifyMap_[notifyType];
            context->obs_->OnChange(notifyMessage);
            context->notifyMap_.erase(notifyType);
        }
    }
}

int32_t FileAccessService::OnChange(Uri uri, NotifyType notifyType)
{
    HILOG_ERROR("enter onchange uri = %{public}s", uri.ToString().c_str());
    UserAccessTracer trace;
    trace.Start("OnChange");
    string uriStr = uri.ToString();
    shared_ptr<ObserverNode> node;
    NotifyMessage notifyMessage;
    notifyMessage.notifyType_ = notifyType;
    vector<string> uris {uriStr};
    notifyMessage.uris_ = uris;
    //When the path is not found, search for its parent path
    if (FindUri(uriStr, node) != ERR_OK) {
        size_t slashIndex = uriStr.rfind("/");
        if (slashIndex == string::npos) {
            HILOG_ERROR("Do not have parent");
            return E_DO_NOT_HAVE_PARENT;
        }
        string parentUri = uriStr.substr(0, slashIndex);
        if (FindUri(parentUri, node) != ERR_OK) {
            HILOG_DEBUG("Can not find onChange parent uri");
            return ERR_OK;
        }
        if (!node->needChildNote_) {
            HILOG_DEBUG("Do not need send onChange message");
            return ERR_OK;
        }
        SendListNotify(uriStr, notifyType, node->obsCodeList_);
        return ERR_OK;
    }
    SendListNotify(uriStr, notifyType, node->obsCodeList_);
    if ((node->parent_ == nullptr) || (!node->parent_->needChildNote_)) {
        HILOG_DEBUG("Do not need notify parent");
        return ERR_OK;
    }
    SendListNotify(uriStr, notifyType, node->parent_->obsCodeList_);
    return ERR_OK;
}

void FileAccessService::InitTimer()
{
    onDemandTimer_ = std::make_shared<OnDemandTimer>([this] {
        lock_guard<mutex> lock(mutex_);
        vector<shared_ptr<ObserverContext>> contexts;
        FileAccessService::GetInstance()->obsManager_.getAll(contexts);
        bool isMessage = false;
        for (auto context : contexts) {
            if (!context->notifyMap_.size()) {
                continue;
            }
            for (auto message : context->notifyMap_) {
                NotifyMessage notifyMessage;
                notifyMessage.notifyType_ = message.first;
                notifyMessage.uris_ = message.second;
                context->obs_->OnChange(notifyMessage);
            }
            context->notifyMap_.clear();
            isMessage = true;
        }
        return isMessage;
            }, NOTIFY_TIME_INTERVAL, MAX_WAIT_TIME);
}

int32_t FileAccessService::GetExensionProxy(const std::shared_ptr<ConnectExtensionInfo> &info,
    sptr<IFileAccessExtBase> &extensionProxy)
{
    sptr<FileAccessExtConnection> fileAccessExtConnection(new(std::nothrow) FileAccessExtConnection());
    if (fileAccessExtConnection == nullptr) {
        HILOG_ERROR("new fileAccessExtConnection fail");
        return E_CONNECT;
    }
    if (info == nullptr || info->token == nullptr) {
        HILOG_ERROR("ConnectExtensionInfo is invalid");
        return E_CONNECT;
    }
    fileAccessExtConnection->ConnectFileExtAbility(info->want, info->token);
    extensionProxy = fileAccessExtConnection->GetFileExtProxy();
    if (extensionProxy == nullptr) {
        HILOG_ERROR("extensionProxy is nullptr");
        return E_CONNECT;
    }
    return ERR_OK;
}
} // namespace FileAccessFwk
} // namespace OHOS
