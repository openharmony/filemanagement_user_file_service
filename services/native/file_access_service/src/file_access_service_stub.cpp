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

#include "file_access_service_stub.h"

#include <memory>
#include <string>

#include "access_token.h"
#include "accesstoken_kit.h"
#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "ipc_object_stub.h"
#include "ipc_skeleton.h"

#include "message_parcel.h"
#include "uri.h"

namespace OHOS {
namespace FileAccessFwk {
namespace {
    const std::string FILE_ACCESS_PERMISSION = "ohos.permission.FILE_ACCESS_MANAGER";
}
FileAccessServiceStub::FileAccessServiceStub()
{
    stubFuncMap_[CMD_REGISTER_NOTIFY] = &FileAccessServiceStub::CmdRegisterNotify;
    stubFuncMap_[CMD_UNREGISTER_NOTIFY] = &FileAccessServiceStub::CmdUnregisterNotify;
    stubFuncMap_[CMD_ONCHANGE] = &FileAccessServiceStub::CmdOnChange;
}

FileAccessServiceStub::~FileAccessServiceStub()
{
    stubFuncMap_.clear();
}

int32_t FileAccessServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "OnRemoteRequest");
    if (!CheckCallingPermission(FILE_ACCESS_PERMISSION)) {
        HILOG_ERROR("permission error");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_PERMISSION;
    }

    std::u16string descriptor = FileAccessServiceStub::GetDescriptor();
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

ErrCode FileAccessServiceStub::CmdOnChange(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdOnChange");
    std::shared_ptr<Uri> uri(data.ReadParcelable<Uri>());
    if (uri == nullptr) {
        HILOG_ERROR("UnregisterNotify uri is nullptr");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    NotifyType notifyType = static_cast<NotifyType>(data.ReadInt32());
    int ret = OnChange(*uri, notifyType);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter OnChange fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

ErrCode FileAccessServiceStub::CmdRegisterNotify(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdRegisterNotify");
    std::shared_ptr<Uri> uri(data.ReadParcelable<Uri>());
    if (uri == nullptr) {
        HILOG_ERROR("RegisterNotify uri is nullptr");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    if (obj == nullptr) {
        HILOG_INFO("get remote obj fail.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    sptr<IFileAccessObserver> observer = iface_cast<IFileAccessObserver>(obj);
    if (observer == nullptr) {
        HILOG_INFO("get observer fail");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    bool notifyForDescendants = data.ReadBool();
    int ret = RegisterNotify(*uri, observer, notifyForDescendants);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter RegisterNotify fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessServiceStub::CmdUnregisterNotify(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdUnregisterNotify");
    std::shared_ptr<Uri> uri(data.ReadParcelable<Uri>());
    if (uri == nullptr) {
        HILOG_ERROR("UnregisterNotify uri is nullptr");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    if (obj == nullptr) {
        HILOG_INFO("get remote obj fail.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    sptr<IFileAccessObserver> observer = iface_cast<IFileAccessObserver>(obj);
    if (observer == nullptr) {
        HILOG_INFO("get observer fail");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = UnregisterNotify(*uri, observer);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter UnregisterNotify fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

bool FileAccessServiceStub::CheckCallingPermission(const std::string &permission)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CheckCallingPermission");
    Security::AccessToken::AccessTokenID tokenCaller = IPCSkeleton::GetCallingTokenID();
    int res = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller, permission);
    if (res != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        HILOG_ERROR("FileAccessExtStub::CheckCallingPermission have no fileAccess permission");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return false;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return true;
}
} // namespace FileAccessFwk
} // namespace OHOS