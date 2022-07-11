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

#include "file_access_ext_connection.h"

#include "ability_manager_client.h"
#include "file_access_ext_proxy.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace FileAccessFwk {
sptr<FileAccessExtConnection> FileAccessExtConnection::instance_ = nullptr;
std::mutex FileAccessExtConnection::mutex_;

sptr<FileAccessExtConnection> FileAccessExtConnection::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (instance_ == nullptr) {
            instance_ = sptr<FileAccessExtConnection>(new (std::nothrow) FileAccessExtConnection());
        }
    }
    return instance_;
}

void FileAccessExtConnection::OnAbilityConnectDone(
    const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    if (remoteObject == nullptr) {
        HILOG_ERROR("%{public}s failed, remote is nullptr", __func__);
        return;
    }
    fileExtProxy_ = iface_cast<FileAccessExtProxy>(remoteObject);
    if (fileExtProxy_ == nullptr) {
        HILOG_ERROR("%{public}s failed, fileExtProxy_ is nullptr", __func__);
        return;
    }
    isConnected_.store(true);
}

void FileAccessExtConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    fileExtProxy_ = nullptr;
    isConnected_.store(false);
}

void FileAccessExtConnection::ConnectFileExtAbility(const AAFwk::Want &want, const sptr<IRemoteObject> &token)
{
    ErrCode ret = AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want, this, token);
    HILOG_INFO("%{public}s called end, ret=%{public}d", __func__, ret);
}

void FileAccessExtConnection::DisconnectFileExtAbility()
{
    fileExtProxy_ = nullptr;
    isConnected_.store(false);
    ErrCode ret = AAFwk::AbilityManagerClient::GetInstance()->DisconnectAbility(this);
    HILOG_INFO("%{public}s called end, ret=%{public}d", __func__, ret);
}

bool FileAccessExtConnection::IsExtAbilityConnected()
{
    return isConnected_.load();
}

sptr<IFileAccessExtBase> FileAccessExtConnection::GetFileExtProxy()
{
    return fileExtProxy_;
}
} // namespace FileAccessFwk
} // namespace OHOS