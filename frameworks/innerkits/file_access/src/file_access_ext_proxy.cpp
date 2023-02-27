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
        return E_IPCS;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteInt32(flags)) {
        HILOG_ERROR("fail to WriteInt32 flags");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_OPEN_FILE, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
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
        return E_GETRESULT;
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
        return E_IPCS;
    }

    if (!data.WriteParcelable(&parent)) {
        HILOG_ERROR("fail to WriteParcelable parent");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("fail to WriteString displayName");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_CREATE_FILE, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
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
        return E_IPCS;
    }

    newFile = Uri(*tempUri);
    if (newFile.ToString().empty()) {
        HILOG_ERROR("get uri is empty.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_GETRESULT;
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
        return E_IPCS;
    }

    if (!data.WriteParcelable(&parent)) {
        HILOG_ERROR("fail to WriteParcelable parent");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("fail to WriteString displayName");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_MKDIR, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
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
        return E_IPCS;
    }

    newFile = Uri(*tempUri);
    if (newFile.ToString().empty()) {
        HILOG_ERROR("get uri is empty.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_GETRESULT;
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
        return E_IPCS;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR("fail to WriteParcelable sourceFile");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_DELETE, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
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
        return E_IPCS;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR("fail to WriteParcelable sourceFile");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteParcelable(&targetParent)) {
        HILOG_ERROR("fail to WriteParcelable targetParent");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_MOVE, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
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
        return E_IPCS;
    }

    newFile = Uri(*tempUri);
    if (newFile.ToString().empty()) {
        HILOG_ERROR("get uri is empty.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_GETRESULT;
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
        return E_IPCS;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR("fail to WriteParcelable sourceFile");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("fail to WriteString displayName");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_RENAME, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
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
        return E_IPCS;
    }

    newFile = Uri(*tempUri);
    if (newFile.ToString().empty()) {
        HILOG_ERROR("get uri is empty.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_GETRESULT;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

static int GetListFileResult(MessageParcel &reply, std::vector<FileInfo> &fileInfoVec)
{
    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return E_IPCS;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("ListFile operation failed ret : %{public}d", ret);
        return ret;
    }

    int64_t count = 0;
    if (!reply.ReadInt64(count)) {
        HILOG_ERROR("ListFile operation failed to Read count");
        return E_IPCS;
    }

    fileInfoVec.clear();
    for (int64_t i = 0; i < count; i++) {
        std::unique_ptr<FileInfo> fileInfoPtr(reply.ReadParcelable<FileInfo>());
        if (fileInfoPtr != nullptr) {
            fileInfoVec.push_back(*fileInfoPtr);
        }
    }
    return ERR_OK;
}

int FileAccessExtProxy::ListFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
    const FileFilter &filter, std::vector<FileInfo> &fileInfoVec)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "ListFile");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteParcelable(&fileInfo)) {
        HILOG_ERROR("fail to WriteParcelable fileInfo");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteInt64(offset)) {
        HILOG_ERROR("fail to WriteInt64 offset");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteInt64(maxCount)) {
        HILOG_ERROR("fail to WriteInt64 maxCount");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteParcelable(&filter)) {
        HILOG_ERROR("fail to WriteParcelable filter");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_LIST_FILE, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    err = GetListFileResult(reply, fileInfoVec);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to GetListFileResult. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessExtProxy::ScanFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
    const FileFilter &filter, std::vector<FileInfo> &fileInfoVec)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "ScanFile");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteParcelable(&fileInfo)) {
        HILOG_ERROR("fail to WriteParcelable fileInfo");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteInt64(offset)) {
        HILOG_ERROR("fail to WriteInt64 offset");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteInt64(maxCount)) {
        HILOG_ERROR("fail to WriteInt64 maxCount");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteParcelable(&filter)) {
        HILOG_ERROR("fail to WriteParcelable filter");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_SCAN_FILE, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    err = GetListFileResult(reply, fileInfoVec);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to GetListFileResult. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessExtProxy::GetRoots(std::vector<RootInfo> &rootInfoVec)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetRoots");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_GET_ROOTS, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("GetRoots operation failed ret : %{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    uint64_t count = 0;
    if (!reply.ReadUint64(count)) {
        HILOG_ERROR("GetRoots operation failed to Read count");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    rootInfoVec.clear();
    for (uint64_t i = 0; i < count; i++) {
        std::unique_ptr<RootInfo> rootInfoPtr(reply.ReadParcelable<RootInfo>());
        if (rootInfoPtr != nullptr) {
            rootInfoVec.push_back(*rootInfoPtr);
        }
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessExtProxy::GetThumbnail(const Uri &uri, Size &size, std::shared_ptr<PixelMap> &pixelMap)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetThumbnail");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable selectfile");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }
    if (!data.WriteInt32(size.width)) {
        HILOG_ERROR("fail to WriteParcelable sourceFile");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }
    if (!data.WriteInt32(size.height)) {
        HILOG_ERROR("fail to WriteParcelable sourceFile");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }
    MessageParcel reply;
    MessageOption option;
    std::string uriString = uri.ToString();
    int err = Remote()->SendRequest(CMD_GET_THUMBNAIL, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }
    int ret = 0;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("Parameter GetThumbnail fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }
    std::shared_ptr<PixelMap> tempPixelMap = std::shared_ptr<PixelMap>(reply.ReadParcelable<PixelMap>());
    if (tempPixelMap == nullptr) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }
    pixelMap = tempPixelMap;
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessExtProxy::GetFileInfoFromUri(const Uri &selectFile, FileInfo &fileInfo)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetFileInfoFromUri");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteParcelable(&selectFile)) {
        HILOG_ERROR("fail to WriteParcelable selectFile");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_GET_FILEINFO_FROM_URI, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("GetFileInfoFromUri operation failed ret : %{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    std::unique_ptr<FileInfo> fileInfoTemp(reply.ReadParcelable<FileInfo>());
    if (fileInfoTemp == nullptr) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    fileInfo = *fileInfoTemp;
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int FileAccessExtProxy::GetFileInfoFromRelativePath(const std::string &selectFile, FileInfo &fileInfo)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetFileInfoFromRelativePath");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!data.WriteString(selectFile)) {
        HILOG_ERROR("fail to WriteParcelable selectFile");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_GET_FILEINFO_FROM_RELATIVE_PATH, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("GetFileInfoFromRelativePath operation failed ret : %{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    std::unique_ptr<FileInfo> fileInfoTemp(reply.ReadParcelable<FileInfo>());
    if (fileInfoTemp == nullptr) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    fileInfo = *fileInfoTemp;
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
        return E_IPCS;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_ACCESS, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("Access operation failed ret : %{public}d", ret);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    if (!reply.ReadBool(isExist)) {
        HILOG_ERROR("fail to ReadInt32 isExist");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

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
        return E_IPCS;
    }

    if (!data.WriteRemoteObject(notify->AsObject())) {
        HILOG_ERROR("write subscribe type or parcel failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_REGISTER_NOTIFY, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
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
        return E_IPCS;
    }

    if (!data.WriteRemoteObject(notify->AsObject())) {
        HILOG_ERROR("write subscribe type or parcel failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_UNREGISTER_NOTIFY, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
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
