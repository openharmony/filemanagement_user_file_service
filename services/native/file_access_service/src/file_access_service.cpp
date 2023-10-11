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
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "system_ability_definition.h"

using namespace std;
namespace OHOS {
namespace FileAccessFwk {
namespace {
    auto pms = FileAccessService::GetInstance();
    const bool G_REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(pms.GetRefPtr());
}
sptr<FileAccessService> FileAccessService::instance_;
mutex FileAccessService::mutex_;
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

FileAccessService::FileAccessService() : SystemAbility(FILE_ACCESS_SERVICE_ID, false) {}

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
    RemoveRelations(uriStr, obsNode);
    return ERR_OK;
}

void FileAccessService::SendListNotify(const vector<uint32_t> list, NotifyMessage &notifyMessage)
{
    for (uint32_t code : list) {
        if (obsManager_.get(code) == nullptr) {
            HILOG_ERROR("Failed to get obs code = %{private}ud", code);
        } else {
            obsManager_.get(code)->obs_->OnChange(notifyMessage);
        }
    }
}

int32_t FileAccessService::OnChange(Uri uri, NotifyType notifyType)
{
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
        SendListNotify(node->obsCodeList_, notifyMessage);
        return ERR_OK;
    }
    SendListNotify(node->obsCodeList_, notifyMessage);
    if ((node->parent_ == nullptr) || (!node->parent_->needChildNote_)) {
        HILOG_DEBUG("Do not need notify parent");
        return ERR_OK;
    }
    SendListNotify(node->parent_->obsCodeList_, notifyMessage);
    return ERR_OK;
}
} // namespace FileAccessFwk
} // namespace OHOS
