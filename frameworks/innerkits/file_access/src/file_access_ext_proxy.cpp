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
int FileAccessExtProxy::OpenFile(const Uri &uri, const int flags, int &fd)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "OpenFile");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (!data.WriteUint32(flags)) {
        HILOG_ERROR("fail to WriteUint32 flags");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_OPEN_FILE, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = ERR_PARCEL_FAIL;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("OpenFile operation failed ret : %{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    fd = reply.ReadFileDescriptor();
    if (fd < ERR_OK) {
        HILOG_ERROR("fail to ReadFileDescriptor fd: %{public}d", fd);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessExtProxy::CreateFile(const Uri &parent, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CreateFile");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (!data.WriteParcelable(&parent)) {
        HILOG_ERROR("fail to WriteParcelable parent");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("fail to WriteString displayName");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_CREATE_FILE, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = ERR_PARCEL_FAIL;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("CreateFile operation failed ret : %{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (tempUri == nullptr) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    newFile = Uri(*tempUri);
    if (newFile.ToString() == "") {
        HILOG_ERROR("get uri is empty.");
        return ERR_INVALID_RESULT;
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessExtProxy::Mkdir(const Uri &parent, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Mkdir");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (!data.WriteParcelable(&parent)) {
        HILOG_ERROR("fail to WriteParcelable parent");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("fail to WriteString displayName");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_MKDIR, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = ERR_PARCEL_FAIL;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("Mkdir operation failed ret : %{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (tempUri == nullptr) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    newFile = Uri(*tempUri);
    if (newFile.ToString() == "") {
        HILOG_ERROR("get uri is empty.");
        return ERR_INVALID_RESULT;
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessExtProxy::Delete(const Uri &sourceFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Delete");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR("fail to WriteParcelable sourceFile");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_DELETE, data, reply, option);
    if (err != NO_ERROR) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = ERR_PARCEL_FAIL;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("Delete operation failed ret : %{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessExtProxy::Move(const Uri &sourceFile, const Uri &targetParent, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Move");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR("fail to WriteParcelable sourceFile");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (!data.WriteParcelable(&targetParent)) {
        HILOG_ERROR("fail to WriteParcelable targetParent");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_MOVE, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = ERR_PARCEL_FAIL;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("Move operation failed ret : %{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (tempUri == nullptr) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    newFile = Uri(*tempUri);
    if (newFile.ToString() == "") {
        HILOG_ERROR("get uri is empty.");
        return ERR_INVALID_RESULT;
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessExtProxy::Rename(const Uri &sourceFile, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Rename");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR("fail to WriteParcelable sourceFile");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("fail to WriteString displayName");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_RENAME, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = ERR_PARCEL_FAIL;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("Rename operation failed ret : %{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (tempUri == nullptr) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    newFile = Uri(*tempUri);
    if (newFile.ToString() == "") {
        HILOG_ERROR("get uri is empty.");
        return ERR_INVALID_RESULT;
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessExtProxy::ListFile(const Uri &sourceFile, std::vector<FileInfo> &fileInfo)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "ListFile");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR("fail to WriteParcelable sourceFileUri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_LIST_FILE, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = ERR_PARCEL_FAIL;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("ListFile operation failed ret : %{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    fileInfo.clear();
    uint64_t count = reply.ReadUint64();
    for (uint64_t i = 0; i < count; i++) {
        std::unique_ptr<FileInfo> fileInfoPtr(reply.ReadParcelable<FileInfo>());
        if (fileInfoPtr != nullptr) {
            fileInfo.push_back(*fileInfoPtr);
        }
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessExtProxy::GetRoots(std::vector<RootInfo> &rootInfo)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetRoots");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_GET_ROOTS, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = ERR_PARCEL_FAIL;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("GetRoots operation failed ret : %{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    rootInfo.clear();
    uint64_t count = reply.ReadUint64();
    for (uint64_t i = 0; i < count; i++) {
        std::unique_ptr<RootInfo> rootInfoPtr(reply.ReadParcelable<RootInfo>());
        if (rootInfoPtr != nullptr) {
            rootInfo.push_back(*rootInfoPtr);
        }
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessExtProxy::Access(const Uri &uri, bool &isExist)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Access");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(CMD_ACCESS, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = ERR_PARCEL_FAIL;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("Access operation failed ret : %{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    isExist = reply.ReadBool();
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessExtProxy::RegisterNotify(sptr<IFileAccessNotify> &notify)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "RegisterNotify");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (!data.WriteRemoteObject(notify->AsObject())) {
        HILOG_ERROR("write subscribe type or parcel failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_REGISTER_NOTIFY, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = ERR_PARCEL_FAIL;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("RegisterNotify operation failed ret : %{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessExtProxy::UnregisterNotify(sptr<IFileAccessNotify> &notify)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "UnregisterNotify");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (!data.WriteRemoteObject(notify->AsObject())) {
        HILOG_ERROR("write subscribe type or parcel failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_UNREGISTER_NOTIFY, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = ERR_PARCEL_FAIL;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARCEL_FAIL;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("UnregisterNotify operation failed ret : %{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}
} // namespace FileAccessFwk
} // namespace OHOS
