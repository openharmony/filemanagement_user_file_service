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
void FileAccessExtConnection::OnAbilityConnectDone(
    const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    if (remoteObject == nullptr) {
        HILOG_ERROR("OnAbilityConnectDone failed, remote is nullptr");
        return;
    }

    fileExtProxy_ = iface_cast<FileAccessExtProxy>(remoteObject);
    if (fileExtProxy_ == nullptr) {
        HILOG_ERROR("OnAbilityConnectDone fileExtProxy_ is nullptr");
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
    HILOG_INFO("ConnectFileExtAbility ret=%{public}d", ret);
}

void FileAccessExtConnection::DisconnectFileExtAbility()
{
    fileExtProxy_ = nullptr;
    isConnected_.store(false);
    ErrCode ret = AAFwk::AbilityManagerClient::GetInstance()->DisconnectAbility(this);
    HILOG_INFO("DisconnectFileExtAbility ret=%{public}d", ret);
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