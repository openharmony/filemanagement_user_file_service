/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "app_file_access_ext_connection.h"

#include <errors.h>
#include <memory>

#include "ability_connect_callback_interface.h"
#include "ability_manager_client.h"
#include "file_access_service_proxy.h"
#include "hilog_wrapper.h"
#include "iremote_broker.h"

namespace OHOS {

namespace FileAccessFwk {
std::mutex AppFileAccessExtConnection::mutex_;

void AppFileAccessExtConnection::OnAbilityConnectDone(
    const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    std::lock_guard<std::mutex> lock(proxyMutex_);
    if (remoteObject == nullptr) {
        HILOG_ERROR("remote is nullptr");
        return;
    }
    fileExtProxy_ = iface_cast<IFileAccessExtBase>(remoteObject);
    if (fileExtProxy_ == nullptr) {
        HILOG_ERROR("fileExtProxy_ is nullptr");
        return;
    }
    AddFileAccessDeathRecipient(fileExtProxy_->AsObject());
    HILOG_INFO("OnAbilityConnectDone set connected info");
    isConnected_.store(true);
    connectLockInfo_.isReady = true;
    connectLockInfo_.condition.notify_all();
}

void AppFileAccessExtConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    std::lock_guard<std::mutex> lock(proxyMutex_);
    if (fileExtProxy_) {
        fileExtProxy_ = nullptr;
    }
    isConnected_.store(false);
    HILOG_INFO("OnAbilityDisconnectDone resultCode=%{public}d", resultCode);
}

void AppFileAccessExtConnection::ConnectFileExtAbility(const AAFwk::Want &want)
{
    std::unique_lock<std::mutex> lock(proxyMutex_);
    if (fileExtProxy_) {
        fileExtProxy_ = nullptr;
    }
    isConnected_.store(false);
    auto proxy = FileAccessServiceProxy::GetInstance();
    if (proxy == nullptr) {
        HILOG_ERROR("ConnectFileExtAbility FileAccessServiceProxy GetInstance fail");
        return;
    }
    
    ErrCode ret = proxy->ConnectFileExtAbility(want, this);
    if (ret != ERR_OK) {
        HILOG_ERROR("ConnectAbility failed, ret=%{public}d", ret);
        return;
    }
    const int WAIT_TIME = 3;  // second
    if (!connectLockInfo_.condition.wait_for(lock, std::chrono::seconds(WAIT_TIME),
        [this] { return fileExtProxy_ != nullptr && connectLockInfo_.isReady; })) {
        HILOG_ERROR("Wait connect timeout.");
        return;
    }
    HILOG_INFO("ConnectFileExtAbility success");
}

void AppFileAccessExtConnection::DisconnectFileExtAbility()
{
    std::lock_guard<std::mutex> lock(proxyMutex_);
    if (fileExtProxy_ != nullptr) {
        RemoveFileAccessDeathRecipient(fileExtProxy_->AsObject());
    }
    auto proxy = FileAccessServiceProxy::GetInstance();
    if (proxy == nullptr) {
        HILOG_ERROR("DisconnectFileExtAbility FileAccessServiceProxy GetInstance fail");
        return;
    }
    ErrCode ret = proxy->DisConnectFileExtAbility(this);
    if (ret != ERR_OK) {
        HILOG_ERROR("DisconnectAbility failed, ret=%{public}d", ret);
        return;
    }
    if (fileExtProxy_) {
        fileExtProxy_ = nullptr;
    }
    isConnected_.store(false);
    HILOG_INFO("DisconnectFileExtAbility done");
}

bool AppFileAccessExtConnection::IsExtAbilityConnected()
{
    return isConnected_.load();
}

sptr<IFileAccessExtBase> AppFileAccessExtConnection::GetFileExtProxy()
{
    return fileExtProxy_;
}

void AppFileAccessExtConnection::AddFileAccessDeathRecipient(const sptr<IRemoteObject> &token)
{
    std::lock_guard<std::mutex> lock(deathRecipientMutex_);
    if (token != nullptr && callerDeathRecipient_ != nullptr) {
        token->RemoveDeathRecipient(callerDeathRecipient_);
    }
    if (callerDeathRecipient_ == nullptr) {
        callerDeathRecipient_ =
            new FileAccessDeathRecipient(
                std::bind(&AppFileAccessExtConnection::OnSchedulerDied, this, std::placeholders::_1));
    }
    if (token != nullptr) {
        token->AddDeathRecipient(callerDeathRecipient_);
    }
}

void AppFileAccessExtConnection::RemoveFileAccessDeathRecipient(const sptr<IRemoteObject> &token)
{
    std::lock_guard<std::mutex> lock(deathRecipientMutex_);
    if (token != nullptr && callerDeathRecipient_ != nullptr) {
        token->RemoveDeathRecipient(callerDeathRecipient_);
    }
}

void AppFileAccessExtConnection::OnSchedulerDied(const wptr<IRemoteObject> &remote)
{
    std::lock_guard<std::mutex> lock(proxyMutex_);
    HILOG_ERROR("OnSchedulerDied");
    auto object = remote.promote();
    if (object) {
        object = nullptr;
    }
    isConnected_.store(false);
    if (fileExtProxy_) {
        fileExtProxy_ = nullptr;
    }
}

AppFileAccessExtConnection::~AppFileAccessExtConnection()
{
    HILOG_INFO("~AppFileAccessExtConnection");
    if (isConnected_.load()) {
        DisconnectFileExtAbility();
    }
}

void FileAccessDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    HILOG_ERROR("OnRemoteDied");
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
