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

#include "file_access_ext_proxy.h"

#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "ipc_types.h"
#include "message_option.h"
#include "message_parcel.h"

namespace OHOS {
namespace FileAccessFwk {
int FileAccessExtProxy::OpenFile(const Uri &uri, const int flags)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "OpenFile");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteInt32(flags)) {
        HILOG_ERROR("fail to WriteString mode");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_OPEN_FILE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int fd = reply.ReadFileDescriptor();
    if (fd <= ERR_ERROR) {
        HILOG_ERROR("fail to ReadFileDescriptor fd");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_INVALID_FD;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return fd;
}

int FileAccessExtProxy::CreateFile(const Uri &parent, const std::string &displayName,  Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CreateFile");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&parent)) {
        HILOG_ERROR("fail to WriteParcelable parent");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("fail to WriteString mode");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&newFile)) {
        HILOG_ERROR("fail to WriteParcelable newFile");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_CREATE_FILE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = reply.ReadInt32();
    if (ret < ERR_OK) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (!tempUri) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    newFile = Uri(*tempUri);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtProxy::Mkdir(const Uri &parent, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Mkdir");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&parent)) {
        HILOG_ERROR("fail to WriteParcelable parent");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("fail to WriteString displayName");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&newFile)) {
        HILOG_ERROR("fail to WriteParcelable newFile");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_MKDIR, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = reply.ReadInt32();
    if (ret < ERR_OK) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (!tempUri) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        ret = ERR_IPC_ERROR;
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    newFile = Uri(*tempUri);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtProxy::Delete(const Uri &sourceFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Delete");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR("fail to WriteParcelable sourceFile");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_DELETE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = reply.ReadInt32();
    if (ret < ERR_OK) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtProxy::Move(const Uri &sourceFile, const Uri &targetParent, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Move");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR("fail to WriteParcelable sourceFile");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&targetParent)) {
        HILOG_ERROR("fail to WriteParcelable targetParent");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&newFile)) {
        HILOG_ERROR("fail to WriteParcelable newFile");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_MOVE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = reply.ReadInt32();
    if (ret < ERR_OK) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (!tempUri) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    newFile = Uri(*tempUri);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtProxy::Rename(const Uri &sourceFile, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Rename");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR("fail to WriteParcelable sourceFile");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("fail to WriteString displayName");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&newFile)) {
        HILOG_ERROR("fail to WriteParcelable newFile");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_RENAME, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = reply.ReadInt32();
    if (ret < ERR_OK) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (!tempUri) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    newFile = Uri(*tempUri);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

std::vector<FileInfo> FileAccessExtProxy::ListFile(const Uri &sourceFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "ListFile");
    std::vector<FileInfo> vec;
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return vec;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR("fail to WriteParcelable sourceFileUri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return vec;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_LIST_FILE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return vec;
    }

    vec.clear();
    int64_t count = reply.ReadInt64();
    for (int32_t i = 0; i < count; i++) {
        std::unique_ptr<FileInfo> fileInfo(reply.ReadParcelable<FileInfo>());
        if (fileInfo != nullptr) {
            vec.push_back(*fileInfo);
        }
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return vec;
}

std::vector<DeviceInfo> FileAccessExtProxy::GetRoots()
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetRoots");
    std::vector<DeviceInfo> vec;
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return vec;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_GET_ROOTS, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return vec;
    }

    vec.clear();
    uint64_t count = reply.ReadUint64();
    for (uint64_t i = 0; i < count; i++) {
        std::unique_ptr<DeviceInfo> deviceInfo(reply.ReadParcelable<DeviceInfo>());
        if (deviceInfo != nullptr) {
            vec.push_back(*deviceInfo);
        }
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return vec;
}

int FileAccessExtProxy::IsFileExist(const Uri &uri, bool &isExist)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "IsFileExist");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteBool(isExist)) {
        HILOG_ERROR("fail to WriteBool isExist");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_IS_FILE_EXIST, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = reply.ReadInt32();
    if (ret < ERR_OK) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    isExist = reply.ReadBool();
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtProxy::RegisterNotify(sptr<IFileAccessNotify> &notify)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "RegisterNotify");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteRemoteObject(notify->AsObject())) {
        HILOG_ERROR("write subscribe type or parcel failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_REGISTER_NOTIFY, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    err = reply.ReadInt32();
    if (err != ERR_OK) {
        HILOG_ERROR("fail to RegisterNotify. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return err;
}

int FileAccessExtProxy::UnregisterNotify(sptr<IFileAccessNotify> &notify)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "UnregisterNotify");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteRemoteObject(notify->AsObject())) {
        HILOG_ERROR("write subscribe type or parcel failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_IPC_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_UNREGISTER_NOTIFY, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    err = reply.ReadInt32();
    if (err != ERR_OK) {
        HILOG_ERROR("fail to UnregisterNotify. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return err;
}
} // namespace FileAccessFwk
} // namespace OHOS
