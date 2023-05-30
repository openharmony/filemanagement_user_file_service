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

#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"

namespace OHOS {
namespace FileAccessFwk {
int32_t FileAccessServiceProxy::OnChange(Uri uri, NotifyType notifyType)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "OnChange");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessObserverProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteInt32(static_cast<int32_t>(notifyType))) {
        HILOG_ERROR("fail to WriteParcelable notifyType");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_ONCHANGE, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret) || ret != ERR_OK) {
        HILOG_ERROR("OnChange operation failed ret : %{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int32_t FileAccessServiceProxy::RegisterNotify(Uri uri, const sptr<IFileAccessObserver> &observer,
    bool notifyForDescendants)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "RegisterNotify");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessServiceProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOG_ERROR("fail to WriteRemoteObject observer");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteBool(notifyForDescendants)) {
        HILOG_ERROR("fail to WriteBool notifyForDescendants");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_REGISTER_NOTIFY, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret) || ret != ERR_OK) {
        HILOG_ERROR("RegisterNotify operation failed ret : %{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int32_t FileAccessServiceProxy::UnregisterNotify(Uri uri, const sptr<IFileAccessObserver> &observer)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "UnregisterNotify");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessServiceProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOG_ERROR("fail to WriteRemoteObject observer");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_UNREGISTER_NOTIFY, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret) || ret != ERR_OK) {
        HILOG_ERROR("UnregisterNotify operation failed ret : %{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}
} // namespace FileAccessFwk
} // namespace OHOS
