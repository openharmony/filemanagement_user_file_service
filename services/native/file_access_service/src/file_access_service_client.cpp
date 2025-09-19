/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "file_access_service_client.h"

#include "bundle_constants.h"
#include "user_access_tracer.h"
#include "file_access_framework_errno.h"
#include "file_access_service_ipc_interface_code.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace FileAccessFwk {
constexpr int LOAD_SA_TIMEOUT_MS = 5000;
const std::string UID_TAG = "uid:";
sptr<IFileAccessServiceBase> FileAccessServiceClient::GetInstance()
{
    HILOG_INFO("Getinstance");
    std::unique_lock<std::mutex> lock(proxyMutex_);
    if (serviceProxy_ != nullptr) {
        return serviceProxy_;
    }

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        HILOG_ERROR("Samgr is nullptr");
        return nullptr;
    }
    sptr<ServiceProxyLoadCallback> loadCallback(new ServiceProxyLoadCallback());
    if (loadCallback == nullptr) {
        HILOG_ERROR("loadCallback is nullptr");
        return nullptr;
    }
    int32_t ret = samgr->LoadSystemAbility(FILE_ACCESS_SERVICE_ID, loadCallback);
    if (ret != ERR_OK) {
        HILOG_ERROR("Failed to Load systemAbility, systemAbilityId:%{public}d, ret code:%{public}d",
            FILE_ACCESS_SERVICE_ID, ret);
        return nullptr;
    }

    auto waitStatus = loadCallback->proxyConVar_.wait_for(
        lock, std::chrono::milliseconds(LOAD_SA_TIMEOUT_MS),
        [loadCallback]() { return loadCallback->isLoadSuccess_.load(); });
    if (!waitStatus) {
        HILOG_ERROR("Load FileAccessService SA timeout");
        return nullptr;
    }
    return serviceProxy_;
}

void FileAccessServiceClient::ServiceProxyLoadCallback::OnLoadSystemAbilitySuccess(
    int32_t systemAbilityId,
    const sptr<IRemoteObject> &remoteObject)
{
    HILOG_INFO("Load FileAccessService SA success,systemAbilityId:%{public}d, remoteObj result:%{private}s",
        systemAbilityId, (remoteObject == nullptr ? "false" : "true"));
    std::unique_lock<std::mutex> lock(proxyMutex_);
    if (systemAbilityId != FILE_ACCESS_SERVICE_ID || remoteObject == nullptr) {
        isLoadSuccess_.store(false);
        proxyConVar_.notify_one();
        return;
    }
    serviceProxy_ = iface_cast<IFileAccessServiceBase>(remoteObject);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get remote object");
        isLoadSuccess_.store(false);
        proxyConVar_.notify_one();
        return;
    }
    auto remoteObj = serviceProxy_->AsObject();
    if (!remoteObj) {
        HILOG_ERROR("Failed to get remote object");
        serviceProxy_ = nullptr;
        isLoadSuccess_.store(false);
        proxyConVar_.notify_one();
        return;
    }

    auto callback = [](const wptr<IRemoteObject> &obj) {
        FileAccessServiceClient::InvaildInstance();
        HILOG_ERROR("service died");
    };
    sptr<ProxyDeathRecipient> deathRecipient = sptr(new ProxyDeathRecipient(callback));
    remoteObj->AddDeathRecipient(deathRecipient);
    isLoadSuccess_.store(true);
    proxyConVar_.notify_one();
}

void FileAccessServiceClient::ServiceProxyLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    HILOG_INFO("Load FileAccessService SA failed,systemAbilityId:%{public}d", systemAbilityId);
    std::unique_lock<std::mutex> lock(proxyMutex_);
    serviceProxy_ = nullptr;
    isLoadSuccess_.store(false);
    proxyConVar_.notify_one();
}

void FileAccessServiceClient::InvaildInstance()
{
    HILOG_INFO("invalid instance");
    std::unique_lock<std::mutex> lock(proxyMutex_);
    serviceProxy_ = nullptr;
}
} // namespace FileAccessFwk
} // namespace OHOS
