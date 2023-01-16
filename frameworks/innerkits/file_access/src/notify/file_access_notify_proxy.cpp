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

#include "file_access_notify_proxy.h"

#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "message_parcel.h"

namespace OHOS {
namespace FileAccessFwk {
FileAccessNotifyProxy::FileAccessNotifyProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IFileAccessNotify>(impl)
{
}

int FileAccessNotifyProxy::Notify(const NotifyMessage& message)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(FileAccessNotifyProxy::GetDescriptor())) {
        HILOG_ERROR("write descriptor failed");
        return E_IPCS;
    }

    if (!data.WriteParcelable(&message)) {
        HILOG_ERROR("write parcel message failed");
        return E_IPCS;
    }

    int error = Remote()->SendRequest(CMD_NOTIFY, data, reply, option);
    if (error != 0) {
        HILOG_ERROR("SendRequest failed, error %{public}d", error);
    }
    return error;
}
}  // namespace FileAccessFwk
}  // namespace OHOS