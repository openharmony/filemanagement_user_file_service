/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include <cstddef>
#include <unistd.h>

#include "user_access_tracer.h"
#include "file_access_framework_errno.h"
#include "file_access_extension_info.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "ipc_skeleton.h"
#include "system_ability_definition.h"
#include "iservice_registry.h"
#include "uri_ext.h"
#include "access_token.h"
#include "accesstoken_kit.h"

using namespace std;
namespace OHOS {
namespace FileAccessFwk {
namespace {
    auto pms = FileAccessService::GetInstance();
    const bool G_REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(pms.GetRefPtr());
    const std::string FILE_SCHEME = "file://";
    const std::string FILE_ACCESS_PERMISSION = "ohos.permission.FILE_ACCESS_MANAGER";
}
sptr<FileAccessService> FileAccessService::instance_;
mutex FileAccessService::mutex_;

constexpr int32_t NOTIFY_MAX_NUM = 32;
constexpr int32_t NOTIFY_TIME_INTERVAL = 500;
constexpr int32_t MAX_WAIT_TIME = 20;
constexpr int32_t ONE_SECOND = 1 * 1000;
constexpr int32_t UNLOAD_SA_WAIT_TIME = 30;
std::vector<Uri> deviceUris(DEVICE_ROOTS.begin(), DEVICE_ROOTS.end());

bool FileAccessService::CheckCallingPermission(const std::string &permission)
{
    UserAccessTracer trace;
    trace.Start("CheckCallingPermission");
    Security::AccessToken::AccessTokenID tokenCaller = IPCSkeleton::GetCallingTokenID();
    int res = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller, permission);
    if (res != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        HILOG_ERROR("FileAccessExtStub::CheckCallingPermission have no fileAccess permission");
        return false;
    }

    return true;
}

sptr<FileAccessService> FileAccessService::GetInstance()
{
    if (instance_ != nullptr) {
        return instance_;
    }

    lock_guard<mutex> lock(mutex_);
    if (instance_ == nullptr) {
        instance_ = sptr(new FileAccessService());
        if (instance_ == nullptr) {
            HILOG_ERROR("GetInstance nullptr");
            return instance_;
        }
    }
    return instance_;
}

FileAccessService::FileAccessService() : SystemAbility(FILE_ACCESS_SERVICE_ID, false)
{
}

void FileAccessService::OnStart()
{
    UserAccessTracer trace;
    trace.Start("OnStart");
    sptr<FileAccessService> service = FileAccessService::GetInstance();
    if (service == nullptr) {
        HILOG_ERROR("service is nullptr");
        return;
    }
    service->Init();
    if (!Publish(service)) {
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

void FileAccessService::Init()
{
    InitTimer();
    if (extensionDeathRecipient_ == nullptr) {
        extensionDeathRecipient_ = sptr(new ExtensionDeathRecipient());
    }
    if (observerDeathRecipient_  == nullptr) {
        observerDeathRecipient_ = sptr(new ObserverDeathRecipient());
    }
    if (appDeathRecipient_ == nullptr) {
        appDeathRecipient_ = sptr(new AppDeathRecipient());
    }
}

static bool GetBundleNameFromUri(Uri &uri, string &bundleName)
{
    string scheme = uri.GetScheme();
    if (scheme != FILE_SCHEME_NAME) {
        return false;
    }
    string path = "/" + uri.GetAuthority() + uri.GetPath();
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
        return true;
    }
    if (bundleName.compare(EXTERNAL_BNUDLE_NAME_ALIAS) == 0) {
        bundleName = EXTERNAL_BNUDLE_NAME;
        return true;
    }
    HILOG_ERROR("Uri-authority error.");
    return false;
}

sptr<IFileAccessExtBase> FileAccessService::ConnectExtension(Uri &uri, const shared_ptr<ConnectExtensionInfo> &info)
{
    string bundleName;
    GetBundleNameFromUri(uri, bundleName);
    auto curBundleExtProxy = FindExtProxyByBundleName(bundleName);
    if (curBundleExtProxy != nullptr) {
        return curBundleExtProxy;
    }
    sptr<IFileAccessExtBase> extensionProxy;
    {
        lock_guard<mutex> lock(mutex_);
        int32_t ret = GetExtensionProxy(*info, extensionProxy);
        if (ret != ERR_OK || extensionProxy == nullptr) {
            return nullptr;
        }
        auto object = extensionProxy->AsObject();
        if (object) {
            object->AddDeathRecipient(extensionDeathRecipient_);
        }
    }
    AddExtProxyInfo(bundleName, extensionProxy);
    return extensionProxy;
}

void FileAccessService::ResetProxy(const wptr<IRemoteObject> &remote)
{
    HILOG_DEBUG("FileAccessService::ResetProxy start");
    lock_guard<mutex> lock(mapMutex_);
    if (remote != nullptr && extensionDeathRecipient_ != nullptr) {
        for (auto iter = cMap_.begin(); iter != cMap_.end();) {
            if (iter->second == nullptr) {
                HILOG_ERROR("iter->second is null or extensionDeathRecipient_ is null.");
                ++iter;
                continue;
            }
            auto proxyRemote = iter->second->AsObject();
            if (proxyRemote != nullptr && proxyRemote == remote.promote()) {
                proxyRemote->RemoveDeathRecipient(extensionDeathRecipient_);
                iter = cMap_.erase(iter);
                continue;
            }
            ++iter;
        }
    } else {
        HILOG_ERROR("FileAccessService::ResetProxy, proxy is null or extensionDeathRecipient_ is null.");
    }
}

void FileAccessService::ExtensionDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    HILOG_ERROR("FileAccessService::ExtensionDeathRecipient::OnRemoteDied, remote obj died.");
    if (remote == nullptr) {
        HILOG_ERROR("remote is nullptr");
        return;
    }
    FileAccessService::GetInstance()->ResetProxy(remote);
}

void FileAccessService::ObserverDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    HILOG_ERROR("FileAccessService::ObserverDeathRecipient::OnRemoteDied, remote obj died.");
    if (remote == nullptr || remote.promote() == nullptr) {
        return;
    }
    sptr<IFileAccessObserver> observer = iface_cast<IFileAccessObserver>(remote.promote());
    if (observer == nullptr) {
        HILOG_ERROR("convert promote failed");
        return;
    }
    FileAccessService::GetInstance()->CleanRelativeObserver(observer);
}

void FileAccessService::CleanRelativeObserver(const sptr<IFileAccessObserver> &observer)
{
    shared_ptr<ObserverContext> obsContext = make_shared<ObserverContext>(observer);
    if (obsContext == nullptr) {
        HILOG_ERROR("obsContext is nullptr");
        return;
    }
    uint32_t code = obsManager_.getId([obsContext](const shared_ptr<ObserverContext>  &afterContext) {
        return obsContext->EqualTo(afterContext);
    });
    ConnectExtensionInfo info = ConnectExtensionInfo();
    std::vector<Uri> uriLists = GetUriList(code);
    for (size_t i = 0; i < uriLists.size(); ++i) {
        UnregisterNotify(uriLists[i], observer, info);
    }
}

std::vector<Uri> FileAccessService::GetUriList(uint32_t code)
{
    lock_guard<mutex> lock(nodeMutex_);
    std::vector<Uri> uriList;
    for (auto pair : relationshipMap_) {
        auto codeList = pair.second->obsCodeList_;
        auto haveCodeIter = find_if(codeList.begin(), codeList.end(),
            [code](const uint32_t &listCode) { return code == listCode; });
        if (haveCodeIter != codeList.end()) {
            Uri uri(pair.first);
            uriList.push_back(uri);
        }
    }
    return uriList;
}

static void convertUris(Uri uri, std::vector<Uri> &uris)
{
    std::string uriString = uri.ToString();
    if (uriString == DEVICES_URI) {
        auto uid = uriString.substr(0, uriString.find("file://"));
        for (auto uirStr : DEVICE_ROOTS) {
            uris.push_back(Uri(uid + uirStr));
        }
    } else {
        uris.push_back(uri);
    }
}

int32_t FileAccessService::RegisterNotify(const Uri &uri, bool notifyForDescendants,
                                          const sptr<IFileAccessObserver> &observer,
                                          const ConnectExtensionInfo& info)
{
    if (!CheckCallingPermission(FILE_ACCESS_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }
    std::shared_ptr<ConnectExtensionInfo> infoPtr =
                std::make_shared<ConnectExtensionInfo>(info);
    std::vector<Uri> uris;
    convertUris(uri, uris);
    for (auto eachUri : uris) {
        int ret = RegisterNotifyImpl(eachUri, notifyForDescendants, observer, infoPtr);
        if (ret != ERR_OK) {
            HILOG_ERROR("RegisterNotify error ret = %{public}d", ret);
            return ret;
        }
    }
    return ERR_OK;
}

int32_t FileAccessService::OperateObsNode(Uri &uri, bool notifyForDescendants, uint32_t code,
    const std::shared_ptr<ConnectExtensionInfo> &info)
{
    string uriStr = uri.ToString();
    {
        lock_guard<mutex> lock(nodeMutex_);
        auto iter = relationshipMap_.find(uriStr);
        if (iter != relationshipMap_.end()) {
            auto obsNode = iter->second;
            auto haveCodeIter = find_if(obsNode->obsCodeList_.begin(), obsNode->obsCodeList_.end(),
                [code](const uint32_t &listCode) { return code == listCode; });
            if (haveCodeIter != obsNode->obsCodeList_.end()) {
                if (obsManager_.get(code) != nullptr) {
                    obsManager_.get(code)->UnRef();
                }
                if (obsNode->needChildNote_ == notifyForDescendants) {
                    HILOG_DEBUG("Register same uri and same callback and same notifyForDescendants");
                    return ERR_OK;
                }
                obsNode->needChildNote_ = notifyForDescendants;
                HILOG_DEBUG("Register same uri and same callback but need modify notifyForDescendants");
                return ERR_OK;
            }
            obsNode->obsCodeList_.push_back(code);
            return ERR_OK;
        }
    }
    auto extensionProxy = ConnectExtension(uri, info);
    if (extensionProxy == nullptr) {
        return E_CONNECT;
    }
    Urie uriConvert(uriStr);
    extensionProxy->StartWatcher(uriConvert);
    {
        lock_guard<mutex> lock(nodeMutex_);
        auto obsNode = make_shared<ObserverNode>(notifyForDescendants);
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
        obsNode->obsCodeList_.push_back(code);
        relationshipMap_.insert(make_pair(uriStr, obsNode));
    }
    return ERR_OK;
}

int32_t FileAccessService::RegisterNotifyImpl(Uri uri, bool notifyForDescendants,
    const sptr<IFileAccessObserver> &observer, const std::shared_ptr<ConnectExtensionInfo> &info)
{
    UserAccessTracer trace;
    trace.Start("RegisterNotifyImpl");
    // std::string token = IPCSkeleton::ResetCallingIdentity();
    shared_ptr<ObserverContext> obsContext = make_shared<ObserverContext>(observer);
    // find if obsManager_ has this callback.
    uint32_t code = obsManager_.getId([obsContext](const shared_ptr<ObserverContext> &afterContext) {
        return obsContext->EqualTo(afterContext);
    });
    if (code == HolderManager<shared_ptr<ObserverContext>>::CODE_CAN_NOT_FIND) {
        // this is new callback, save this context
        obsContext->Ref();
        code = obsManager_.save(obsContext);
        if (obsContext->obs_ == nullptr) {
            return E_GETRESULT;
        }
        auto object = obsContext->obs_->AsObject();
        object->AddDeathRecipient(observerDeathRecipient_);
    } else {
        // this callback is already in manager, add ref.
        auto object = obsManager_.get(code);
        if (object == nullptr) {
            return E_GETRESULT;
        }
        object->Ref();
    }
    return OperateObsNode(uri, notifyForDescendants, code, info);
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
    HILOG_INFO("uriStr: %{private}s", uriStr.c_str());
    lock_guard<mutex> lock(nodeMutex_);
    HILOG_DEBUG("FindUri start");
    auto iter = relationshipMap_.find(uriStr);
    if (iter == relationshipMap_.end()) {
        HILOG_ERROR("Can not find uri");
        return E_CAN_NOT_FIND_URI;
    }
    outObsNode = iter->second;
    return ERR_OK;
}

int32_t FileAccessService::CleanAllNotify(Uri uri, const std::shared_ptr<ConnectExtensionInfo> &info)
{
    std::vector<Uri> uris;
    convertUris(uri, uris);
    for (auto eachUri : uris) {
        int ret = CleanAllNotifyImpl(eachUri, info);
        if (ret != ERR_OK) {
            HILOG_ERROR("CleanAllNotify error ret = %{public}d", ret);
            return ret;
        }
    }
    return ERR_OK;
}

int32_t FileAccessService::CleanAllNotifyImpl(Uri uri, const std::shared_ptr<ConnectExtensionInfo> &info)
{
    UserAccessTracer trace;
    trace.Start("CleanAllNotifyImpl");
    string uriStr = uri.ToString();
    shared_ptr<ObserverNode> obsNode;
    if (FindUri(uriStr, obsNode) != ERR_OK) {
        HILOG_ERROR("Can not find unregisterNotify uri");
        return E_CAN_NOT_FIND_URI;
    }
    for (auto code : obsNode->obsCodeList_) {
        auto context = obsManager_.get(code);
        if (context == nullptr) {
            continue;
        }
        context->UnRef();
        if (!context->IsValid()) {
            obsManager_.release(code);
        }
    }

    size_t uriIndex = uriStr.find(FILE_SCHEME);
    if (uriIndex == string::npos) {
        HILOG_ERROR("Current uriStr can not find targetUri");
        return ERR_URI;
    }
    Uri originalUri(uriStr.substr(uriIndex));
    auto extensionProxy = ConnectExtension(originalUri, info);
    if (extensionProxy == nullptr) {
        HILOG_ERROR("Creator get invalid fileExtProxy");
        return E_CONNECT;
    }
    Urie originalUriConvert(originalUri.ToString());
    extensionProxy->StopWatcher(originalUriConvert);
    RemoveRelations(uriStr, obsNode);
    if (IsUnused() && unLoadTimer_) {
        unLoadTimer_->reset();
    }
    return ERR_OK;
}

int32_t FileAccessService::UnregisterNotify(const Uri &uri, const sptr<IFileAccessObserver> &observer,
    const ConnectExtensionInfo &info)
{
    if (!CheckCallingPermission(FILE_ACCESS_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }
    std::shared_ptr<ConnectExtensionInfo> infoPtr =
                std::make_shared<ConnectExtensionInfo>(info);
    std::vector<Uri> uris;
    convertUris(uri, uris);
    for (auto eachUri : uris) {
        int ret = UnregisterNotifyImpl(eachUri, observer, infoPtr);
        if (ret != ERR_OK) {
            HILOG_ERROR("UnregisterNotify error ret = %{public}d", ret);
            return ret;
        }
    }
    if (IsUnused() && unLoadTimer_) {
        unLoadTimer_->reset();
    }
    return ERR_OK;
}

int32_t FileAccessService::UnregisterNotifyNoObserver(const Uri &uri, const ConnectExtensionInfo &info)
{
    if (!CheckCallingPermission(FILE_ACCESS_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }
    std::shared_ptr<ConnectExtensionInfo> infoPtr =
                std::make_shared<ConnectExtensionInfo>(info);
    int ret = CleanAllNotify(uri, infoPtr);
    return ret;
}

int32_t FileAccessService::UnregisterNotifyImpl(Uri uri, const sptr<IFileAccessObserver> &observer,
    const std::shared_ptr<ConnectExtensionInfo> &info)
{
    UserAccessTracer trace;
    trace.Start("UnregisterNotifyImpl");
    if (observer == nullptr) {
        HILOG_ERROR("UnregisterNotify failed with invalid observer");
        return E_IPCS;
    }
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
    int32_t ret = obsNode->FindAndRmObsCodeByCode(code);
    if (ret != ERR_OK) {
        HILOG_ERROR("Can not find obsNode by code");
        return ret;
    }
    auto object = obsManager_.get(code);
    if (object == nullptr) {
        HILOG_ERROR("Can not find obsNode by code");
        return E_IPCS;
    }
    object->UnRef();
    // if data refcount is invalid, release this code.
    if (!object->IsValid()) {
        obsManager_.release(code);
    }
    // node has other observers, do not need remove.
    if (obsNode->CheckObsCodeListNotEmpty()) {
        HILOG_DEBUG("Has code do not stopWatcher");
        return ERR_OK;
    }
    return RmUriObsNodeRelations(uriStr, obsNode, info);
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
        auto it = std::find(DEVICE_ROOTS.begin(), DEVICE_ROOTS.end(), uriStr);
        if (it != DEVICE_ROOTS.end()) {
            vector<string> uris = {uriStr};
            NotifyMessage notifyMessage{notifyType, uris};
            if (context->obs_) {
                context->obs_->OnChange(notifyMessage);
            }
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
            if (context->obs_) {
                context->obs_->OnChange(notifyMessage);
            }
            context->notifyMap_.erase(notifyType);
        }
    }
}

int32_t FileAccessService::OnChange(const Uri &uri, NotifyType notifyType)
{
    if (!CheckCallingPermission(FILE_ACCESS_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }
    UserAccessTracer trace;
    trace.Start("OnChange");
    string uriStr = uri.ToString();
    shared_ptr<ObserverNode> node;
    size_t uriIndex = uriStr.find(FILE_SCHEME);
    if (uriIndex == string::npos) {
        HILOG_ERROR("Current uriStr can not find targetUri");
        return ERR_URI;
    }
    string uris = uriStr.substr(uriIndex);
    HILOG_DEBUG("OnChange FindUri start");
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
        SendListNotify(uris, notifyType, node->obsCodeList_);
        return ERR_OK;
    }
    HILOG_DEBUG("OnChange FindUri end");
    SendListNotify(uris, notifyType, node->obsCodeList_);
    if ((node->parent_ == nullptr) || (!node->parent_->needChildNote_)) {
        HILOG_DEBUG("Do not need notify parent");
        return ERR_OK;
    }
    SendListNotify(uris, notifyType, node->parent_->obsCodeList_);
    return ERR_OK;
}

bool FileAccessService::IsUnused()
{
    HILOG_INFO("IsUnused: obsManager_: %{public}d, appProxyMap_: %{public}d, calledCount_: %{public}d",
        obsManager_.isEmpty(), appProxyMap_.empty(), calledCount_);
    return obsManager_.isEmpty() && appProxyMap_.empty() && !IsCalledCountValid();
}

void FileAccessService::InitTimer()
{
    onDemandTimer_ = std::make_shared<OnDemandTimer>([this] {
        lock_guard<mutex> lock(mutex_);
        vector<shared_ptr<ObserverContext>> contexts;
        FileAccessService::GetInstance()->obsManager_.getAll(contexts);
        bool isMessage = false;
        for (auto context : contexts) {
            if (context == nullptr) {
                continue;
            }
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

    unLoadTimer_ = std::make_shared<UnloadTimer>([this] {
        if (!IsUnused()) {
            return;
        }
        sptr<ISystemAbilityManager> saManager =
        OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (saManager == nullptr) {
            HILOG_ERROR("UnloadSA, GetSystemAbilityManager is null.");
            return;
        }
        int32_t result = saManager->UnloadSystemAbility(FILE_ACCESS_SERVICE_ID);
        if (result != ERR_OK) {
            HILOG_ERROR("UnloadSA, UnloadSystemAbility result: %{public}d", result);
            return;
        }
    }, ONE_SECOND, UNLOAD_SA_WAIT_TIME);
    unLoadTimer_->start();
}

int32_t FileAccessService::ConnectFileExtAbility(const AAFwk::Want &want,
    const sptr<AAFwk::IAbilityConnection>& connection)
{
    if (!CheckCallingPermission(FILE_ACCESS_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }
    HILOG_INFO("ConnectFileExtAbility start");
    if (connection == nullptr) {
        HILOG_ERROR("connection is nullptr");
        return E_CONNECT;
    }

    sptr<AgentFileAccessExtConnection> fileAccessExtConnection(
        new(std::nothrow) AgentFileAccessExtConnection(connection));
    if (fileAccessExtConnection == nullptr) {
        HILOG_ERROR("new fileAccessExtConnection fail");
        return E_CONNECT;
    }

    fileAccessExtConnection->ConnectFileExtAbility(want);
    AddAppProxy(connection, fileAccessExtConnection);
    return ERR_OK;
}

int32_t FileAccessService::DisConnectFileExtAbility(const sptr<AAFwk::IAbilityConnection>& connection)
{
    if (!CheckCallingPermission(FILE_ACCESS_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }
    HILOG_INFO("ConnectFileExtAbility start");
    if (connection == nullptr) {
        HILOG_ERROR("connection is nullptr");
        return E_CONNECT;
    }
    DisconnectAppProxy(connection);
    RemoveAppProxy(connection);
    return ERR_OK;
}


int32_t FileAccessService::GetExtensionProxy(const ConnectExtensionInfo& info,
    sptr<IFileAccessExtBase> &extensionProxy)
{
    if (!CheckCallingPermission(FILE_ACCESS_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }
    std::shared_ptr<ConnectExtensionInfo> infoPtr =
                std::make_shared<ConnectExtensionInfo>(info);
    sptr<FileAccessExtConnection> fileAccessExtConnection(new(std::nothrow) FileAccessExtConnection());
    if (fileAccessExtConnection == nullptr) {
        HILOG_ERROR("new fileAccessExtConnection fail");
        return E_CONNECT;
    }
    if (infoPtr == nullptr || infoPtr->token == nullptr) {
        HILOG_ERROR("ConnectExtensionInfo is invalid");
        return E_CONNECT;
    }
    fileAccessExtConnection->ConnectFileExtAbility(infoPtr->want, infoPtr->token);
    extensionProxy = fileAccessExtConnection->GetFileExtProxy();
    if (extensionProxy == nullptr) {
        HILOG_ERROR("extensionProxy is nullptr");
        return E_CONNECT;
    }
    return ERR_OK;
}

int32_t FileAccessService::RmUriObsNodeRelations(std::string &uriStr, std::shared_ptr<ObserverNode> &obsNode,
    const std::shared_ptr<ConnectExtensionInfo> &info)
{
    size_t uriIndex = uriStr.find(FILE_SCHEME);
    if (uriIndex == string::npos) {
        HILOG_ERROR("Current uriStr can not find targetUri");
        return ERR_URI;
    }
    Uri originalUri(uriStr.substr(uriIndex));
    auto extensionProxy = ConnectExtension(originalUri, info);
    if (extensionProxy == nullptr) {
        HILOG_ERROR("Creator get invalid fileExtProxy");
        return E_CONNECT;
    }
    Urie originalUriConvert(originalUri.ToString());
    extensionProxy->StopWatcher(originalUriConvert);
    RemoveRelations(uriStr, obsNode);
    return ERR_OK;
}

sptr<IFileAccessExtBase> FileAccessService::FindExtProxyByBundleName(std::string bundleName)
{
    lock_guard<mutex> lock(mapMutex_);
    auto iterator = cMap_.find(bundleName);
    if (iterator != cMap_.end()) {
        return iterator->second;
    }
    return nullptr;
}

void FileAccessService::AddExtProxyInfo(std::string bundleName, sptr<IFileAccessExtBase> extProxy)
{
    lock_guard<mutex> lock(mapMutex_);
    cMap_.emplace(bundleName, extProxy);
}

void FileAccessService::AddAppProxy(const sptr<AAFwk::IAbilityConnection>& connection,
    sptr<AgentFileAccessExtConnection>& value)
{
    if (connection == nullptr || value == nullptr) {
        HILOG_ERROR("key is null or value is null");
        return;
    }
    size_t key = reinterpret_cast<size_t>(connection->AsObject().GetRefPtr());
    HILOG_INFO("sa add key, %{public}zu", key);
    lock_guard<mutex> lock(appProxyMutex_);
    if (appProxyMap_.count(key)) {
        HILOG_INFO("sa had proxy,needn't create connection");
        return;
    }
    if (connection->AsObject() == nullptr) {
        HILOG_ERROR("connection->AsObject() is nullptr");
        return;
    }
    connection->AsObject()->AddDeathRecipient(appDeathRecipient_);
    appProxyMap_[key] = value;
    appConnection_[key] = connection;
    HILOG_INFO("appProxyMap_ size: %{public}zu", appProxyMap_.size());
}

void FileAccessService::RemoveAppProxy(const sptr<AAFwk::IAbilityConnection>& connection)
{
    if (connection == nullptr) {
        HILOG_WARN("key is null");
        return;
    }
    size_t key = reinterpret_cast<size_t>(connection->AsObject().GetRefPtr());
    lock_guard<mutex> lock(appProxyMutex_);
    if (appProxyMap_.find(key) == appProxyMap_.end()) {
        HILOG_INFO("appProxyMap_ not key %{public}zu", key);
        return;
    }

    appProxyMap_.erase(key);
    appConnection_.erase(key);
    HILOG_INFO("appProxyMap_ size: %{public}zu", appProxyMap_.size());
}

void FileAccessService::DisconnectAppProxy(const sptr<AAFwk::IAbilityConnection>& connection)
{
    size_t key = reinterpret_cast<size_t>(connection->AsObject().GetRefPtr());
    lock_guard<mutex> lock(appProxyMutex_);
    if (appProxyMap_.find(key) == appProxyMap_.end()) {
        HILOG_WARN("appProxyMap_ not key: %{public}zu", key);
        return;
    }
    HILOG_INFO("DisconnectAppProxy DisconnectFileExtAbility key: %{public}zu", key);
    if (appProxyMap_[key] != nullptr) {
        appProxyMap_[key]->DisconnectFileExtAbility();
    }
}

void FileAccessService::AppDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    HILOG_INFO("FileAccessService::AppDeathRecipient::OnRemoteDied, remote obj died.");
    if (remote == nullptr) {
        HILOG_ERROR("remote is nullptr");
        return;
    }
    auto remoteBroker = iface_cast<AAFwk::IAbilityConnection>(remote.promote());
    size_t key = reinterpret_cast<size_t>(remoteBroker->AsObject().GetRefPtr());
    HILOG_INFO("remote: %{public}zu", key);
    FileAccessService::GetInstance()->RemoveAppProxy(remoteBroker);
}
} // namespace FileAccessFwk
} // namespace OHOS
