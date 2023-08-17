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

#include "observer_callback_stub.h"

#include <string>
#include <memory>

#include "accesstoken_kit.h"
#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "ipc_object_stub.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "message_parcel.h"

namespace OHOS {
namespace FileAccessFwk {
ObserverCallbackStub::~ObserverCallbackStub()
{
    stubFuncMap_.clear();
}

void ObserverCallbackStub::InitStubFuncMap()
{
    if (stubFuncMap_.size() == 0) {
        stubFuncMap_[CMD_ONCHANGE] = &ObserverCallbackStub::OnChangeStub;
    }
}

int32_t ObserverCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "OnRemoteRequest");
    InitStubFuncMap();
    std::u16string descriptor = ObserverCallbackStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_INVALID_STATE;
    }

    const auto &itFunc = stubFuncMap_.find(code);
    if (itFunc != stubFuncMap_.end()) {
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return (this->*(itFunc->second))(data, reply);
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t ObserverCallbackStub::OnChangeStub(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "OnChangeStub");
    std::shared_ptr<NotifyMessage> notifyMessage(data.ReadParcelable<NotifyMessage>());
    if (notifyMessage == nullptr) {
        HILOG_ERROR("OnChange uri is nullptr");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_URIS;
    }

    OnChange(*notifyMessage);
    int ret = ERR_OK;
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter OnChangeStub fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}
}
}