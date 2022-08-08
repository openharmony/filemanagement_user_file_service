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

#include "file_access_notify_stub.h"

#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "message_parcel.h"

namespace OHOS {
namespace FileAccessFwk {
int32_t FileAccessNotifyStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    auto descriptor = FileAccessNotifyStub::GetDescriptor();
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_ERROR("local descriptor is not equal to remote");
        return ERR_ERROR;
    }
    HILOG_INFO("FileAccessNotifyStub::OnRemoteRequest, code:%{public}u", code);
    switch (code) {
        case CMD_NOTIFY: {
            std::shared_ptr<NotifyMessage> message(data.ReadParcelable<NotifyMessage>());
            if (message == nullptr) {
                HILOG_ERROR("read parcelable message fail");
                return ERR_IPC_ERROR;
            }
            auto ret = Notify(*message);
            if (ret != ERR_OK) {
                HILOG_ERROR("FileAccessNotifyStub::OnRemoteRequest, Notify error, ret:%{public}d", ret);
            }
            return ret;
        }
        default:
            HILOG_DEBUG("FileAccessNotifyStub::OnRemoteRequest error code:%{public}u", code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}
}  // namespace FileAccessFwk
}  // namespace OHOS