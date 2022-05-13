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
namespace AppExecFwk {
int FileExtProxy::OpenFile(const Uri &uri, const std::string &mode)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    int fd = -1;
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileExtProxy::GetDescriptor())) {
        HILOG_ERROR("tag dsa %{public}s WriteInterfaceToken failed", __func__);
        return fd;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("tag dsa fail to WriteParcelable uri");
        return fd;
    }

    if (!data.WriteString(mode)) {
        HILOG_ERROR("tag dsa fail to WriteString mode");
        return fd;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_OPEN_FILE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("tag dsa OpenFile fail to SendRequest. err: %d", err);
        return fd;
    }

    fd = reply.ReadFileDescriptor();
    if (fd == -1) {
        HILOG_ERROR("tag dsa fail to ReadFileDescriptor fd");
        return fd;
    }

    HILOG_INFO("tag dsa %{public}s end successfully, return fd=%{public}d", __func__, fd);
    return fd;
}

int FileExtProxy::CloseFile(int fd, const std::string &uri)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    int ret = -1;
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileExtProxy::GetDescriptor())) {
        HILOG_ERROR("tag dsa %{public}s WriteInterfaceToken failed", __func__);
        return ret;
    }

    if (!data.WriteInt32(fd)) {
        HILOG_ERROR("tag dsa fail to WriteParcelable sourceFileUri");
        return ret;
    }

    if (!data.WriteString(uri)) {
        HILOG_ERROR("tag dsa fail to WriteString uri");
        return ret;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_CLOSE_FILE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("tag dsa CloseFile fail to SendRequest. err: %d", err);
        return ret;
    }

    ret = reply.ReadInt32();
    if (ret < 0) {
        HILOG_ERROR("tag dsa fail to ReadInt32 ret");
        return ret;
    }

    HILOG_INFO("tag dsa %{public}s end successfully, return ret=%{public}d", __func__, ret);
    return ret;
}

int FileExtProxy::CreateFile(const Uri &parentUri, const std::string &displayName,  Uri &newFileUri)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    int ret = -1;
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileExtProxy::GetDescriptor())) {
        HILOG_ERROR("tag dsa %{public}s WriteInterfaceToken failed", __func__);
        return ret;
    }

    if (!data.WriteParcelable(&parentUri)) {
        HILOG_ERROR("tag dsa fail to WriteParcelable parentUri");
        return ret;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("tag dsa fail to WriteString mode");
        return ret;
    }

    if (!data.WriteParcelable(&newFileUri)) {
        HILOG_ERROR("tag dsa fail to WriteParcelable newFileUri");
        return ret;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_CREATE_FILE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("tag dsa OpenFile fail to SendRequest. err: %d", err);
        return ret;
    }

    ret = reply.ReadInt32();
    if (ret < 0) {
        HILOG_ERROR("tag dsa fail to ReadInt32 ret");
        return ret;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (!tempUri) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        ret = -1;
        return ret;
    }

    HILOG_INFO("tag dsa %{public}s end successfully, return ret=%{public}d, newFileUri=%{public}s", __func__, ret, newFileUri.ToString().c_str());
    newFileUri = Uri(*tempUri);
    HILOG_INFO("tag dsa %{public}s end successfully, return ret=%{public}d, newFileUri=%{public}s", __func__, ret, newFileUri.ToString().c_str());
    HILOG_INFO("tag dsa %{public}s end successfully, tempUri=%{public}s", __func__, tempUri->ToString().c_str());
    return ret;
}

int FileExtProxy::Mkdir(const Uri &parentUri, const std::string &displayName, Uri &newFileUri)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    int ret = -1;
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileExtProxy::GetDescriptor())) {
        HILOG_ERROR("tag dsa %{public}s WriteInterfaceToken failed", __func__);
        return ret;
    }

    if (!data.WriteParcelable(&parentUri)) {
        HILOG_ERROR("tag dsa fail to WriteParcelable parentUri");
        return ret;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("tag dsa fail to WriteString displayName");
        return ret;
    }

    if (!data.WriteParcelable(&newFileUri)) {
        HILOG_ERROR("tag dsa fail to WriteParcelable newFileUri");
        return ret;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_MKDIR, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("tag dsa OpenFile fail to SendRequest. err: %d", err);
        return ret;
    }

    ret = reply.ReadInt32();
    if (ret < 0) {
        HILOG_ERROR("tag dsa fail to ReadInt32 ret");
        return ret;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (!tempUri) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        ret = -1;
        return ret;
    }

    HILOG_INFO("tag dsa %{public}s end successfully, return ret=%{public}d, newFileUri=%{public}s", __func__, ret, newFileUri.ToString().c_str());
    newFileUri = Uri(*tempUri);
    HILOG_INFO("tag dsa %{public}s end successfully, return ret=%{public}d, newFileUri=%{public}s", __func__, ret, newFileUri.ToString().c_str());
    HILOG_INFO("tag dsa %{public}s end successfully, tempUri=%{public}s", __func__, tempUri->ToString().c_str());
    return ret;
}

int FileExtProxy::Delete(const Uri &sourceFileUri)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    int ret = -1;
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileExtProxy::GetDescriptor())) {
        HILOG_ERROR("tag dsa %{public}s WriteInterfaceToken failed", __func__);
        return ret;
    }

    if (!data.WriteParcelable(&sourceFileUri)) {
        HILOG_ERROR("tag dsa fail to WriteParcelable sourceFileUri");
        return ret;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_DELETE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("tag dsa OpenFile fail to SendRequest. err: %d", err);
        return ret;
    }

    ret = reply.ReadInt32();
    if (ret < 0) {
        HILOG_ERROR("tag dsa fail to ReadInt32 ret");
        return ret;
    }

    HILOG_INFO("tag dsa %{public}s end successfully, return ret=%{public}d", __func__, ret);
    return ret;
}

int FileExtProxy::Move(const Uri &sourceFileUri, const Uri &targetParentUri, Uri &newFileUri)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    int ret = -1;
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileExtProxy::GetDescriptor())) {
        HILOG_ERROR("tag dsa %{public}s WriteInterfaceToken failed", __func__);
        return ret;
    }

    if (!data.WriteParcelable(&sourceFileUri)) {
        HILOG_ERROR("tag dsa fail to WriteParcelable sourceFileUri");
        return ret;
    }

    if (!data.WriteParcelable(&targetParentUri)) {
        HILOG_ERROR("tag dsa fail to WriteParcelable targetParentUri");
        return ret;
    }

    if (!data.WriteParcelable(&newFileUri)) {
        HILOG_ERROR("tag dsa fail to WriteParcelable newFileUri");
        return ret;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_MOVE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("tag dsa OpenFile fail to SendRequest. err: %d", err);
        return ret;
    }

    ret = reply.ReadInt32();
    if (ret < 0) {
        HILOG_ERROR("tag dsa fail to ReadInt32 ret");
        return ret;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (!tempUri) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        ret = -1;
        return ret;
    }

    HILOG_INFO("tag dsa %{public}s end successfully, return ret=%{public}d, newFileUri=%{public}s", __func__, ret, newFileUri.ToString().c_str());
    newFileUri = Uri(*tempUri);
    HILOG_INFO("tag dsa %{public}s end successfully, return ret=%{public}d, newFileUri=%{public}s", __func__, ret, newFileUri.ToString().c_str());
    HILOG_INFO("tag dsa %{public}s end successfully, tempUri=%{public}s", __func__, tempUri->ToString().c_str());
    return ret;
}

int FileExtProxy::Rename(const Uri &sourceFileUri, const std::string &displayName, Uri &newFileUri)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    int ret = -1;
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileExtProxy::GetDescriptor())) {
        HILOG_ERROR("tag dsa %{public}s WriteInterfaceToken failed", __func__);
        return ret;
    }

    if (!data.WriteParcelable(&sourceFileUri)) {
        HILOG_ERROR("tag dsa fail to WriteParcelable sourceFileUri");
        return ret;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("tag dsa fail to WriteString displayName");
        return ret;
    }

    if (!data.WriteParcelable(&newFileUri)) {
        HILOG_ERROR("tag dsa fail to WriteParcelable newFileUri");
        return ret;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_RENAME, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("tag dsa OpenFile fail to SendRequest. err: %d", err);
        return ret;
    }

    ret = reply.ReadInt32();
    if (ret < 0) {
        HILOG_ERROR("tag dsa fail to ReadInt32 ret");
        return ret;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (!tempUri) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        return ret;
    }

    HILOG_INFO("tag dsa %{public}s end successfully, return ret=%{public}d, newFileUri=%{public}s", __func__, ret, newFileUri.ToString().c_str());
    newFileUri = Uri(*tempUri);
    HILOG_INFO("tag dsa %{public}s end successfully, return ret=%{public}d, newFileUri=%{public}s", __func__, ret, newFileUri.ToString().c_str());
    HILOG_INFO("tag dsa %{public}s end successfully, tempUri=%{public}s", __func__, tempUri->ToString().c_str());
    return ret;
}
} // namespace AAFwk
} // namespace OHOS
