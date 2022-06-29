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

namespace OHOS {
namespace FileAccessFwk {
int FileAccessExtProxy::OpenFile(const Uri &uri, int flags)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("%{public}s WriteInterfaceToken failed", __func__);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("%{public}s fail to WriteParcelable uri", __func__);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteInt32(flags)) {
        HILOG_ERROR("%{public}s fail to WriteString mode", __func__);
        return ERR_IPC_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_OPEN_FILE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("%{public}s fail to SendRequest. err: %d", __func__, err);
        return ERR_IPC_ERROR;
    }

    int fd = reply.ReadFileDescriptor();
    if (fd <= ERR_ERROR) {
        HILOG_ERROR("%{public}s fail to ReadFileDescriptor fd", __func__);
        return ERR_IPC_ERROR;
    }
    return fd;
}

int FileAccessExtProxy::CreateFile(const Uri &parent, const std::string &displayName,  Uri &newFile)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("%{public}s WriteInterfaceToken failed", __func__);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&parent)) {
        HILOG_ERROR("%{public}s fail to WriteParcelable parent", __func__);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("%{public}s fail to WriteString mode", __func__);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&newFile)) {
        HILOG_ERROR("%{public}s fail to WriteParcelable newFile", __func__);
        return ERR_IPC_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_CREATE_FILE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("%{public}s fail to SendRequest. err: %d", __func__, err);
        return ERR_IPC_ERROR;
    }

    int ret = reply.ReadInt32();
    if (ret < ERR_OK) {
        HILOG_ERROR("%{public}s fail to ReadInt32 ret", __func__);
        return ERR_IPC_ERROR;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (!tempUri) {
        HILOG_ERROR("%{public}s ReadParcelable value is nullptr.", __func__);
        return ERR_IPC_ERROR;
    }

    newFile = Uri(*tempUri);
    return ret;
}

int FileAccessExtProxy::Mkdir(const Uri &parent, const std::string &displayName, Uri &newFile)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("%{public}s WriteInterfaceToken failed", __func__);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&parent)) {
        HILOG_ERROR("%{public}s fail to WriteParcelable parent", __func__);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("%{public}s fail to WriteString displayName", __func__);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&newFile)) {
        HILOG_ERROR("%{public}s fail to WriteParcelable newFile", __func__);
        return ERR_IPC_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_MKDIR, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("%{public}s fail to SendRequest. err: %d", __func__, err);
        return ERR_IPC_ERROR;
    }

    int ret = reply.ReadInt32();
    if (ret < ERR_OK) {
        HILOG_ERROR("%{public}s fail to ReadInt32 ret", __func__);
        return ERR_IPC_ERROR;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (!tempUri) {
        HILOG_ERROR("%{public}s ReadParcelable value is nullptr.", __func__);
        ret = ERR_IPC_ERROR;
        return ret;
    }

    newFile = Uri(*tempUri);
    return ret;
}

int FileAccessExtProxy::Delete(const Uri &sourceFile)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("%{public}s WriteInterfaceToken failed", __func__);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR("%{public}s fail to WriteParcelable sourceFile", __func__);
        return ERR_IPC_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_DELETE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("%{public}s fail to SendRequest. err: %d", __func__, err);
        return ERR_IPC_ERROR;
    }

    int ret = reply.ReadInt32();
    if (ret < ERR_OK) {
        HILOG_ERROR("%{public}s fail to ReadInt32 ret", __func__);
        return ERR_IPC_ERROR;
    }

    return ret;
}

int FileAccessExtProxy::Move(const Uri &sourceFile, const Uri &targetParent, Uri &newFile)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR(" %{public}s WriteInterfaceToken failed", __func__);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR(" %{public}s fail to WriteParcelable sourceFile", __func__);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&targetParent)) {
        HILOG_ERROR(" %{public}s fail to WriteParcelable targetParent", __func__);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&newFile)) {
        HILOG_ERROR(" %{public}s fail to WriteParcelable newFile", __func__);
        return ERR_IPC_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_MOVE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR(" %{public}s fail to SendRequest. err: %d", __func__, err);
        return ERR_IPC_ERROR;
    }

    int ret = reply.ReadInt32();
    if (ret < ERR_OK) {
        HILOG_ERROR(" %{public}s fail to ReadInt32 ret", __func__);
        return ret;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (!tempUri) {
        HILOG_ERROR(" %{public}s ReadParcelable value is nullptr.", __func__);
        return ERR_IPC_ERROR;
    }

    newFile = Uri(*tempUri);
    return ret;
}

int FileAccessExtProxy::Rename(const Uri &sourceFile, const std::string &displayName, Uri &newFile)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("%{public}s WriteInterfaceToken failed", __func__);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR("%{public}s fail to WriteParcelable sourceFile", __func__);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("%{public}s fail to WriteString displayName", __func__);
        return ERR_IPC_ERROR;
    }

    if (!data.WriteParcelable(&newFile)) {
        HILOG_ERROR("%{public}s fail to WriteParcelable newFile", __func__);
        return ERR_IPC_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_RENAME, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("%{public}s fail to SendRequest. err: %d", __func__, err);
        return ERR_IPC_ERROR;
    }

    int ret = reply.ReadInt32();
    if (ret < ERR_OK) {
        HILOG_ERROR("%{public}s fail to ReadInt32 ret", __func__);
        return ERR_IPC_ERROR;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (!tempUri) {
        HILOG_ERROR("%{public}s ReadParcelable value is nullptr.", __func__);
        return ERR_IPC_ERROR;
    }

    newFile = Uri(*tempUri);
    return ret;
}

std::vector<FileInfo> FileAccessExtProxy::ListFile(const Uri &sourceFile)
{
    std::vector<FileInfo> vec;
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("%{public}s WriteInterfaceToken failed", __func__);
        return vec;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR("%{public}s fail to WriteParcelable sourceFileUri", __func__);
        return vec;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_LIST_FILE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("%{public}s fail to SendRequest. err: %d", __func__, err);
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

    return vec;
}

std::vector<DeviceInfo> FileAccessExtProxy::GetRoots()
{
    std::vector<DeviceInfo> vec;
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("%{public}s WriteInterfaceToken failed", __func__);
        return vec;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_GET_ROOTS, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("%{public}s fail to SendRequest. err: %d", __func__, err);
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

    return vec;
}
} // namespace FileAccessFwk
} // namespace OHOS
