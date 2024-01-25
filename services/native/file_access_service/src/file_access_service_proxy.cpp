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

#include "file_access_service_proxy.h"

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
sptr<FileAccessServiceProxy> FileAccessServiceProxy::GetInstance()
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

void FileAccessServiceProxy::ServiceProxyLoadCallback::OnLoadSystemAbilitySuccess(
    int32_t systemAbilityId,
    const sptr<IRemoteObject> &remoteObject)
{
    HILOG_INFO("Load FileAccessService SA success,systemAbilityId:%{public}d, remoteObj result:%{private}s",
        systemAbilityId, (remoteObject == nullptr ? "false" : "true"));
    if (systemAbilityId != FILE_ACCESS_SERVICE_ID || remoteObject == nullptr) {
        isLoadSuccess_.store(false);
        proxyConVar_.notify_one();
        return;
    }
    std::unique_lock<std::mutex> lock(proxyMutex_);
    serviceProxy_ = iface_cast<FileAccessServiceProxy>(remoteObject);
    auto remoteObj = serviceProxy_->AsObject();
    if (!remoteObj) {
        HILOG_ERROR("Failed to get remote object");
        serviceProxy_ = nullptr;
        isLoadSuccess_.store(false);
        proxyConVar_.notify_one();
        return;
    }

    auto callback = [](const wptr<IRemoteObject> &obj) {
        FileAccessServiceProxy::InvaildInstance();
        HILOG_ERROR("service died");
    };
    sptr<ProxyDeathRecipient> deathRecipient = sptr(new ProxyDeathRecipient(callback));
    remoteObj->AddDeathRecipient(deathRecipient);
    isLoadSuccess_.store(true);
    proxyConVar_.notify_one();
}

void FileAccessServiceProxy::ServiceProxyLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    HILOG_INFO("Load FileAccessService SA failed,systemAbilityId:%{public}d", systemAbilityId);
    std::unique_lock<std::mutex> lock(proxyMutex_);
    serviceProxy_ = nullptr;
    isLoadSuccess_.store(false);
    proxyConVar_.notify_one();
}

void FileAccessServiceProxy::InvaildInstance()
{
    HILOG_INFO("invalid instance");
    std::unique_lock<std::mutex> lock(proxyMutex_);
    serviceProxy_ = nullptr;
}

int32_t FileAccessServiceProxy::OnChange(Uri uri, NotifyType notifyType)
{
    UserAccessTracer trace;
    trace.Start("OnChange");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessServiceProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        return E_IPCS;
    }

    if (!data.WriteInt32(static_cast<int32_t>(notifyType))) {
        HILOG_ERROR("fail to WriteParcelable notifyType");
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(static_cast<uint32_t>(FileAccessServiceInterfaceCode::CMD_ONCHANGE), data, reply,
        option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret) || ret != ERR_OK) {
        HILOG_ERROR("OnChange operation failed ret : %{public}d", ret);
        return ret;
    }

    return ERR_OK;
}

int32_t FileAccessServiceProxy::RegisterNotify(Uri uri, bool notifyForDescendants,
    const sptr<IFileAccessObserver> &observer, const std::shared_ptr<ConnectExtensionInfo> &info)
{
    UserAccessTracer trace;
    trace.Start("RegisterNotify");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessServiceProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        return E_IPCS;
    }

    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOG_ERROR("fail to WriteRemoteObject observer");
        return E_IPCS;
    }

    if (!data.WriteBool(notifyForDescendants)) {
        HILOG_ERROR("fail to WriteBool notifyForDescendants");
        return E_IPCS;
    }

    if (info == nullptr) {
        HILOG_ERROR("ExtensionInfo is nullptr");
        return E_GETINFO;
    }

    if (!info->WriteToParcel(data)) {
        HILOG_ERROR("fail to WriteParcelable Info");
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(static_cast<uint32_t>(FileAccessServiceInterfaceCode::CMD_REGISTER_NOTIFY), data,
        reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret) || ret != ERR_OK) {
        HILOG_ERROR("RegisterNotify operation failed ret : %{public}d", ret);
        return ret;
    }

    return ERR_OK;
}

int32_t FileAccessServiceProxy::UnregisterNotifyInternal(MessageParcel &data)
{
    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(static_cast<uint32_t>(FileAccessServiceInterfaceCode::CMD_UNREGISTER_NOTIFY), data,
        reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret) || ret != ERR_OK) {
        HILOG_ERROR("UnregisterNotify operation failed ret : %{public}d", ret);
        return ret;
    }

    return ERR_OK;
}

int32_t FileAccessServiceProxy::UnregisterNotify(Uri uri, const sptr<IFileAccessObserver> &observer,
    const std::shared_ptr<ConnectExtensionInfo> &info)
{
    UserAccessTracer trace;
    trace.Start("UnregisterNotify");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessServiceProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        return E_IPCS;
    }
    bool observerNotNull = true;
    if (observer != nullptr) {
        if (!data.WriteBool(observerNotNull)) {
            HILOG_ERROR("fail to WriteBool observerNotNull");
            return E_IPCS;
        }
        if (!data.WriteRemoteObject(observer->AsObject())) {
            HILOG_ERROR("fail to WriteRemoteObject observer");
            return E_IPCS;
        }
    } else {
        observerNotNull = false;
        if (!data.WriteBool(observerNotNull)) {
            HILOG_ERROR("fail to WriteBool observerNotNull");
            return E_IPCS;
        }
    }

    if (info == nullptr) {
        HILOG_ERROR("ExtensionInfo is nullptr");
        return E_GETINFO;
    }

    if (!info->WriteToParcel(data)) {
        HILOG_ERROR("fail to WriteParcelable Info");
        return E_IPCS;
    }

    // Split the code into two functions for better readability
    int32_t result = UnregisterNotifyInternal(data);

    return result;
}


int32_t FileAccessServiceProxy::GetExensionProxy(const std::shared_ptr<ConnectExtensionInfo> &info,
    sptr<IFileAccessExtBase> &extensionProxy)
{
    UserAccessTracer trace;
    trace.Start("GetExensionProxy");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessServiceProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    if (info == nullptr) {
        HILOG_ERROR("ExtensionInfo is nullptr");
        return E_GETINFO;
    }

    if (!info->WriteToParcel(data)) {
        HILOG_ERROR("fail to WriteParcelable Info");
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(static_cast<uint32_t>(FileAccessServiceInterfaceCode::CMD_GET_EXTENSION_PROXY),
        data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret) || ret != ERR_OK) {
        HILOG_ERROR("GetExensionProxy operation failed ret : %{public}d", ret);
        return ret;
    }

    sptr<IRemoteObject> obj = reply.ReadRemoteObject();
    if (obj == nullptr) {
        HILOG_INFO("get remote obj fail.");
        return E_IPCS;
    }

    extensionProxy = iface_cast<IFileAccessExtBase>(obj);
    if (extensionProxy == nullptr) {
        HILOG_INFO("get observer fail");
        return E_IPCS;
    }

    return ERR_OK;
}
} // namespace FileAccessFwk
} // namespace OHOS
