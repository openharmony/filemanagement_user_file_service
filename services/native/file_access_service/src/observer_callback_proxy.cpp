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

#include "observer_callback_proxy.h"
#include "user_access_tracer.h"
#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "ipc_types.h"
#include "message_option.h"
#include "message_parcel.h"

namespace OHOS {
namespace FileAccessFwk {
void ObserverCallbackProxy::OnChange(NotifyMessage &notifyMessage)
{
    UserAccessTracer trace;
    trace.Start("OnChange");
    MessageParcel data;
    if (!data.WriteInterfaceToken(ObserverCallbackProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteParcelable(&notifyMessage)) {
        HILOG_ERROR("fail to WriteParcelable notifyMessage");
        return;
    }

    MessageParcel reply;
    MessageOption option;
    auto remote = Remote();
    if (!remote) {
        HILOG_ERROR("failed to get remote");
        return;
    }
    int err = remote->SendRequest(CMD_ONCHANGE, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("OnChange operation failed ret : %{public}d", ret);
        return;
    }
    return;
}
} // namespace FileAccessFwk
} // namespace OHOS
