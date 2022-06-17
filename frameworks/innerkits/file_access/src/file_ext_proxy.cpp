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

#include "file_ext_proxy.h"

#include "hilog_wrapper.h"

namespace OHOS {
namespace FileAccessFwk {
int FileExtProxy::OpenFile(const Uri &uri, int flags)
{
    HILOG_INFO("%{public}s begin.", __func__);
    int fd = -1;
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileExtProxy::GetDescriptor())) {
        HILOG_ERROR("%{public}s WriteInterfaceToken failed", __func__);
        return fd;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("%{public}s fail to WriteParcelable uri", __func__);
        return fd;
    }

    if (!data.WriteInt32(flags)) {
        HILOG_ERROR("%{public}s fail to WriteString mode", __func__);
        return fd;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_OPEN_FILE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("%{public}s fail to SendRequest. err: %d", __func__, err);
        return fd;
    }

    fd = reply.ReadFileDescriptor();
    if (fd == -1) {
        HILOG_ERROR("%{public}s fail to ReadFileDescriptor fd", __func__);
        return fd;
    }

    HILOG_INFO("%{public}s end successfully, return fd=%{public}d", __func__, fd);
    return fd;
}

int FileExtProxy::CreateFile(const Uri &parentUri, const std::string &displayName,  Uri &newFileUri)
{
    HILOG_INFO("%{public}s begin.", __func__);
    int ret = -1;
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileExtProxy::GetDescriptor())) {
        HILOG_ERROR("%{public}s WriteInterfaceToken failed", __func__);
        return ret;
    }

    if (!data.WriteParcelable(&parentUri)) {
        HILOG_ERROR("%{public}s fail to WriteParcelable parentUri", __func__);
        return ret;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("%{public}s fail to WriteString mode", __func__);
        return ret;
    }

    if (!data.WriteParcelable(&newFileUri)) {
        HILOG_ERROR("%{public}s fail to WriteParcelable newFileUri", __func__);
        return ret;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_CREATE_FILE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("%{public}s fail to SendRequest. err: %d", __func__, err);
        return ret;
    }

    ret = reply.ReadInt32();
    if (ret < 0) {
        HILOG_ERROR("%{public}s fail to ReadInt32 ret", __func__);
        return ret;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (!tempUri) {
        HILOG_ERROR("%{public}s ReadParcelable value is nullptr.", __func__);
        ret = -1;
        return ret;
    }

    newFileUri = Uri(*tempUri);
    HILOG_INFO("%{public}s end successfully, tempUri=%{public}s", __func__, tempUri->ToString().c_str());
    return ret;
}

int FileExtProxy::Mkdir(const Uri &parentUri, const std::string &displayName, Uri &newFileUri)
{
    HILOG_INFO("%{public}s begin.", __func__);
    int ret = -1;
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileExtProxy::GetDescriptor())) {
        HILOG_ERROR("%{public}s WriteInterfaceToken failed", __func__);
        return ret;
    }

    if (!data.WriteParcelable(&parentUri)) {
        HILOG_ERROR("%{public}s fail to WriteParcelable parentUri", __func__);
        return ret;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("%{public}s fail to WriteString displayName", __func__);
        return ret;
    }

    if (!data.WriteParcelable(&newFileUri)) {
        HILOG_ERROR("%{public}s fail to WriteParcelable newFileUri", __func__);
        return ret;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_MKDIR, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("%{public}s fail to SendRequest. err: %d", __func__, err);
        return ret;
    }

    ret = reply.ReadInt32();
    if (ret < 0) {
        HILOG_ERROR("%{public}s fail to ReadInt32 ret", __func__);
        return ret;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (!tempUri) {
        HILOG_ERROR("%{public}s ReadParcelable value is nullptr.", __func__);
        ret = -1;
        return ret;
    }

    newFileUri = Uri(*tempUri);
    HILOG_INFO("%{public}s end successfully, tempUri=%{public}s", __func__, tempUri->ToString().c_str());
    return ret;
}

int FileExtProxy::Delete(const Uri &sourceFileUri)
{
    HILOG_INFO("%{public}s begin.", __func__);
    int ret = -1;
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileExtProxy::GetDescriptor())) {
        HILOG_ERROR("%{public}s WriteInterfaceToken failed", __func__);
        return ret;
    }

    if (!data.WriteParcelable(&sourceFileUri)) {
        HILOG_ERROR("%{public}s fail to WriteParcelable sourceFileUri", __func__);
        return ret;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_DELETE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("%{public}s fail to SendRequest. err: %d", __func__, err);
        return ret;
    }

    ret = reply.ReadInt32();
    if (ret < 0) {
        HILOG_ERROR("%{public}s fail to ReadInt32 ret", __func__);
        return ret;
    }

    HILOG_INFO("%{public}s end successfully, return ret=%{public}d", __func__, ret);
    return ret;
}

int FileExtProxy::Rename(const Uri &sourceFileUri, const std::string &displayName, Uri &newFileUri)
{
    HILOG_INFO("%{public}s begin.", __func__);
    int ret = -1;
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileExtProxy::GetDescriptor())) {
        HILOG_ERROR("%{public}s WriteInterfaceToken failed", __func__);
        return ret;
    }

    if (!data.WriteParcelable(&sourceFileUri)) {
        HILOG_ERROR("%{public}s fail to WriteParcelable sourceFileUri", __func__);
        return ret;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("%{public}s fail to WriteString displayName", __func__);
        return ret;
    }

    if (!data.WriteParcelable(&newFileUri)) {
        HILOG_ERROR("%{public}s fail to WriteParcelable newFileUri", __func__);
        return ret;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_RENAME, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("%{public}s fail to SendRequest. err: %d", __func__, err);
        return ret;
    }

    ret = reply.ReadInt32();
    if (ret < 0) {
        HILOG_ERROR("%{public}s fail to ReadInt32 ret", __func__);
        return ret;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (!tempUri) {
        HILOG_ERROR("%{public}s ReadParcelable value is nullptr.", __func__);
        ret = -1;
        return ret;
    }

    newFileUri = Uri(*tempUri);
    HILOG_INFO("%{public}s end successfully, tempUri=%{public}s", __func__, tempUri->ToString().c_str());
    return ret;
}

std::vector<FileInfo> FileExtProxy::ListFile(const Uri &sourceFileUri)
{
    HILOG_INFO("%{public}s begin.", __func__);
    std::vector<FileInfo> vec;
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileExtProxy::GetDescriptor())) {
        HILOG_ERROR("%{public}s WriteInterfaceToken failed", __func__);
        return vec;
    }

    if (!data.WriteParcelable(&sourceFileUri)) {
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

    HILOG_INFO("%{public}s end successfully, return vec.size=%{public}zu", __func__, vec.size());
    return vec;
}

std::vector<DeviceInfo> FileExtProxy::GetRoots()
{
    HILOG_INFO("%{public}s begin.", __func__);
    std::vector<DeviceInfo> vec;
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileExtProxy::GetDescriptor())) {
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

    HILOG_INFO("%{public}s end successfully, return vec.size=%{public}zu", __func__, vec.size());
    return vec;
}
} // namespace FileAccessFwk
} // namespace OHOS
