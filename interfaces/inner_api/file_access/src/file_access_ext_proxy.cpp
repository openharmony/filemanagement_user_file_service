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
#include "file_access_extension_info.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "ipc_types.h"
#include "message_option.h"
#include "message_parcel.h"
#include "user_access_tracer.h"

namespace OHOS {
namespace FileAccessFwk {
namespace {
    constexpr int COPY_EXCEPTION = -1;
    constexpr uint32_t MAX_COPY_ERROR_COUNT = 1000;
};
int FileAccessExtProxy::OpenFile(const Uri &uri, const int flags, int &fd)
{
    UserAccessTracer trace;
    trace.Start("OpenFile");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        return E_IPCS;
    }

    if (!data.WriteInt32(flags)) {
        HILOG_ERROR("fail to WriteInt32 flags");
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_OPEN_FILE, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return E_IPCS;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("OpenFile operation failed ret : %{public}d", ret);
        return ret;
    }

    fd = reply.ReadFileDescriptor();
    if (fd < ERR_OK) {
        HILOG_ERROR("fail to ReadFileDescriptor fd: %{public}d", fd);
        return E_GETRESULT;
    }

    return ERR_OK;
}

int FileAccessExtProxy::CreateFile(const Uri &parent, const std::string &displayName, Uri &newFile)
{
    UserAccessTracer trace;
    trace.Start("CreateFile");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    if (!data.WriteParcelable(&parent)) {
        HILOG_ERROR("fail to WriteParcelable parent");
        return E_IPCS;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("fail to WriteString displayName");
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_CREATE_FILE, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return E_IPCS;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("CreateFile operation failed ret : %{public}d", ret);
        return ret;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (tempUri == nullptr) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        return E_IPCS;
    }

    newFile = Uri(*tempUri);
    if (newFile.ToString().empty()) {
        HILOG_ERROR("get uri is empty.");
        return E_GETRESULT;
    }

    return ERR_OK;
}

int FileAccessExtProxy::Mkdir(const Uri &parent, const std::string &displayName, Uri &newFile)
{
    UserAccessTracer trace;
    trace.Start("Mkdir");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    if (!data.WriteParcelable(&parent)) {
        HILOG_ERROR("fail to WriteParcelable parent");
        return E_IPCS;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("fail to WriteString displayName");
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_MKDIR, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return E_IPCS;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("Mkdir operation failed ret : %{public}d", ret);
        return ret;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (tempUri == nullptr) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        return E_IPCS;
    }

    newFile = Uri(*tempUri);
    if (newFile.ToString().empty()) {
        HILOG_ERROR("get uri is empty.");
        return E_GETRESULT;
    }

    return ERR_OK;
}

int FileAccessExtProxy::Delete(const Uri &sourceFile)
{
    UserAccessTracer trace;
    trace.Start("Delete");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR("fail to WriteParcelable sourceFile");
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_DELETE, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return E_IPCS;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("Delete operation failed ret : %{public}d", ret);
        return ret;
    }

    return ERR_OK;
}

int FileAccessExtProxy::Move(const Uri &sourceFile, const Uri &targetParent, Uri &newFile)
{
    UserAccessTracer trace;
    trace.Start("Move");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR("fail to WriteParcelable sourceFile");
        return E_IPCS;
    }

    if (!data.WriteParcelable(&targetParent)) {
        HILOG_ERROR("fail to WriteParcelable targetParent");
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_MOVE, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return E_IPCS;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("Move operation failed ret : %{public}d", ret);
        return ret;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (tempUri == nullptr) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        return E_IPCS;
    }

    newFile = Uri(*tempUri);
    if (newFile.ToString().empty()) {
        HILOG_ERROR("get uri is empty.");
        return E_GETRESULT;
    }

    return ERR_OK;
}

static int WriteCopyFuncArguments(OHOS::MessageParcel &data, const Uri &sourceUri, const Uri &destUri, bool force)
{
    UserAccessTracer trace;
    trace.Start("WriteCopyFuncArguments");
    if (!data.WriteParcelable(&sourceUri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        return E_IPCS;
    }

    if (!data.WriteParcelable(&destUri)) {
        HILOG_ERROR("fail to WriteParcelable targetParent");
        return E_IPCS;
    }

    if (!data.WriteBool(force)) {
        HILOG_ERROR("fail to WriteBool force");
        return E_IPCS;
    }
    return ERR_OK;
}

static int WriteCopyFileFuncArguments(OHOS::MessageParcel &data, const Uri &sourceUri, const Uri &destUri,
    const std::string &fileName)
{
    UserAccessTracer trace;
    trace.Start("WriteCopyFuncArguments");
    if (!data.WriteString(sourceUri.ToString())) {
        HILOG_ERROR("fail to WriteParcelable sourceUri");
        return E_IPCS;
    }
    if (!data.WriteString(destUri.ToString())) {
        HILOG_ERROR("fail to WriteParcelable destUri");
        return E_IPCS;
    }
    if (!data.WriteString(fileName)) {
        HILOG_ERROR("fail to WriteString fileName");
        return E_IPCS;
    }
    return ERR_OK;
}

static int ReadCopyFuncResults(OHOS::MessageParcel &reply, std::vector<Result> &copyResult)
{
    UserAccessTracer trace;
    trace.Start("ReadCopyFuncResults");
    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return E_IPCS;
    }
    if (ret == ERR_OK) {
        HILOG_ERROR("Copy operation success");
        return ret;
    }

    uint32_t count = 0;
    if (!reply.ReadUint32(count)) {
        HILOG_ERROR("Copy operation failed to Read count");
        return E_IPCS;
    }
    if (count > MAX_COPY_ERROR_COUNT) {
        HILOG_ERROR("Copy operation failed, count value greater than max count");
        Result result { "", "", E_COUNT, "Count value greater than max count"};
        copyResult.clear();
        copyResult.push_back(result);
        return COPY_EXCEPTION;
    }

    copyResult.clear();
    for (uint32_t i = 0; i < count; i++) {
        std::unique_ptr<Result> copyResultPtr(reply.ReadParcelable<Result>());
        if (copyResultPtr != nullptr) {
            copyResult.push_back(*copyResultPtr);
        }
    }
    return ret;
}

int FileAccessExtProxy::Copy(const Uri &sourceUri, const Uri &destUri, std::vector<Result> &copyResult, bool force)
{
    UserAccessTracer trace;
    trace.Start("Copy");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    int ret = WriteCopyFuncArguments(data, sourceUri, destUri, force);
    if (ret != ERR_OK) {
        HILOG_ERROR("Write copy function arguments error, code: %{public}d", ret);
        return ret;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_COPY, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest, err: %{public}d", err);
        return err;
    }

    ret = ReadCopyFuncResults(reply, copyResult);
    if (ret != ERR_OK) {
        HILOG_ERROR("Read copy function result error, code: %{public}d", ret);
        return ret;
    }

    return ret;
}

int FileAccessExtProxy::CopyFile(const Uri &sourceUri, const Uri &destUri, const std::string &fileName, Uri &newFileUri)
{
    UserAccessTracer trace;
    trace.Start("CopyFile");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    int ret = WriteCopyFileFuncArguments(data, sourceUri, destUri, fileName);
    if (ret != ERR_OK) {
        HILOG_ERROR("Write copy file function arguments error, code: %{public}d", ret);
        return ret;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_COPY_FILE, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest, err: %{public}d", err);
        return err;
    }

    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return E_IPCS;
    }
    if (ret != ERR_OK) {
        HILOG_ERROR("Copy file error, code:%{public}d", ret);
        return ret;
    }
    std::string tempUri = "";
    if (!reply.ReadString(tempUri)) {
        HILOG_ERROR("fail to ReadString copyResult");
        return E_IPCS;
    }
    if (tempUri.empty()) {
        HILOG_ERROR("get uri is empty");
        return E_GETRESULT;
    }
    newFileUri = Uri(tempUri);
    return ERR_OK;
}

int FileAccessExtProxy::Rename(const Uri &sourceFile, const std::string &displayName, Uri &newFile)
{
    UserAccessTracer trace;
    trace.Start("Rename");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    if (!data.WriteParcelable(&sourceFile)) {
        HILOG_ERROR("fail to WriteParcelable sourceFile");
        return E_IPCS;
    }

    if (!data.WriteString(displayName)) {
        HILOG_ERROR("fail to WriteString displayName");
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_RENAME, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return E_IPCS;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("Rename operation failed ret : %{public}d", ret);
        return ret;
    }

    std::unique_ptr<Uri> tempUri(reply.ReadParcelable<Uri>());
    if (tempUri == nullptr) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        return E_IPCS;
    }

    newFile = Uri(*tempUri);
    if (newFile.ToString().empty()) {
        HILOG_ERROR("get uri is empty.");
        return E_GETRESULT;
    }

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

static int WriteFileFilterFuncArguments(MessageParcel &data,
    std::tuple<const FileInfo *, int64_t, const FileFilter *, SharedMemoryInfo *> args)
{
    const FileInfo *fileInfo;
    int64_t offset;
    const FileFilter *filter;
    SharedMemoryInfo *memInfo;
    std::tie(fileInfo, offset, filter, memInfo) = args;

    if (!data.WriteParcelable(fileInfo)) {
        HILOG_ERROR("fail to WriteParcelable fileInfo");
        return E_IPCS;
    }

    if (!data.WriteInt64(offset)) {
        HILOG_ERROR("fail to WriteInt64 offset");
        return E_IPCS;
    }

    if (!data.WriteParcelable(filter)) {
        HILOG_ERROR("fail to WriteParcelable filter");
        return E_IPCS;
    }

    if (!data.WriteParcelable(memInfo)) {
        HILOG_ERROR("fail to WriteParcelable memInfo");
        return E_IPCS;
    }
    return ERR_OK;
}

static int ReadFileFilterResults(MessageParcel &reply, SharedMemoryInfo &memInfo)
{
    if (!reply.ReadUint32(memInfo.dataCounts)) {
        HILOG_ERROR("fail to ReadUnt64 dataCounts");
        return E_IPCS;
    }
    memInfo.totalDataCounts = memInfo.dataCounts;

    if (!reply.ReadUint64(memInfo.dataSize)) {
        HILOG_ERROR("fail to ReadUnt64 dataSize");
        return E_IPCS;
    }

    if (!reply.ReadUint32(memInfo.leftDataCounts)) {
        HILOG_ERROR("fail to ReadUnt64 leftDataCounts");
        return E_IPCS;
    }

    if (!reply.ReadBool(memInfo.isOver)) {
        HILOG_ERROR("fail to ReadBool isOver");
        return E_IPCS;
    }
    return ERR_OK;
}

int FileAccessExtProxy::ListFile(const FileInfo &fileInfo, const int64_t offset,
    const FileFilter &filter, SharedMemoryInfo &memInfo)
{
    UserAccessTracer trace;
    trace.Start("ListFile");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    int ret = WriteFileFilterFuncArguments(data, std::make_tuple(&fileInfo, offset, &filter, &memInfo));
    if (ret != ERR_OK) {
        return ret;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_LIST_FILE, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err;
    }
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return E_IPCS;
    }
    if (ret != ERR_OK) {
        HILOG_ERROR("ListFile operation failed ret : %{public}d", ret);
        return ret;
    }

    ret = ReadFileFilterResults(reply, memInfo);
    if (ret != ERR_OK) {
        HILOG_ERROR("fail to read server return results. ret: %{public}d", ret);
        return ret;
    }
    return ERR_OK;
}

int FileAccessExtProxy::ScanFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
    const FileFilter &filter, std::vector<FileInfo> &fileInfoVec)
{
    UserAccessTracer trace;
    trace.Start("ScanFile");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    if (!data.WriteParcelable(&fileInfo)) {
        HILOG_ERROR("fail to WriteParcelable fileInfo");
        return E_IPCS;
    }

    if (!data.WriteInt64(offset)) {
        HILOG_ERROR("fail to WriteInt64 offset");
        return E_IPCS;
    }

    if (!data.WriteInt64(maxCount)) {
        HILOG_ERROR("fail to WriteInt64 maxCount");
        return E_IPCS;
    }

    if (!data.WriteParcelable(&filter)) {
        HILOG_ERROR("fail to WriteParcelable filter");
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_SCAN_FILE, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err;
    }

    err = GetListFileResult(reply, fileInfoVec);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to GetListFileResult. err: %{public}d", err);
        return err;
    }

    return ERR_OK;
}

static int GetQueryResult(MessageParcel &reply, std::vector<std::string> &results)
{
    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return E_IPCS;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("Query operation failed ret : %{public}d", ret);
        return ret;
    }

    int64_t count = 0;
    if (!reply.ReadInt64(count)) {
        HILOG_ERROR("Query operation failed to Read count");
        return E_IPCS;
    }

    results.clear();
    for (int64_t i = 0; i < count; i++) {
        results.push_back(reply.ReadString());
    }
    return ERR_OK;
}

int FileAccessExtProxy::Query(const Uri &uri, std::vector<std::string> &columns, std::vector<std::string> &results)
{
    UserAccessTracer trace;
    trace.Start("Query");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable sourceFile");
        return E_IPCS;
    }
    size_t count = columns.size();
    if (!data.WriteInt64(count)) {
        HILOG_ERROR("Parameter Query fail to WriteInt64 count");
        return E_IPCS;
    }
    if (count > FILE_RESULT_TYPE.size()) {
        HILOG_ERROR(" The number of query operations exceeds %{public}zu ", FILE_RESULT_TYPE.size());
        return EINVAL;
    }

    for (const auto &column : columns) {
        if (!data.WriteString(column)) {
            HILOG_ERROR("parameter Query fail to WriteParcelable column");
            return E_IPCS;
        }
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_QUERY, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err;
    }

    err = GetQueryResult(reply, results);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to GetQueryResult. err: %{public}d", err);
        return err;
    }
    return ERR_OK;
}

int FileAccessExtProxy::GetRoots(std::vector<RootInfo> &rootInfoVec)
{
    UserAccessTracer trace;
    trace.Start("GetRoots");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_GET_ROOTS, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return E_IPCS;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("GetRoots operation failed ret : %{public}d", ret);
        return ret;
    }

    uint64_t count = 0;
    if (!reply.ReadUint64(count)) {
        HILOG_ERROR("GetRoots operation failed to Read count");
        return E_IPCS;
    }

    rootInfoVec.clear();
    for (uint64_t i = 0; i < count; i++) {
        std::unique_ptr<RootInfo> rootInfoPtr(reply.ReadParcelable<RootInfo>());
        if (rootInfoPtr != nullptr) {
            rootInfoVec.push_back(*rootInfoPtr);
        }
    }

    return ERR_OK;
}

int FileAccessExtProxy::GetFileInfoFromUri(const Uri &selectFile, FileInfo &fileInfo)
{
    UserAccessTracer trace;
    trace.Start("GetFileInfoFromUri");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    if (!data.WriteParcelable(&selectFile)) {
        HILOG_ERROR("fail to WriteParcelable selectFile");
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_GET_FILEINFO_FROM_URI, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return E_IPCS;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("GetFileInfoFromUri operation failed ret : %{public}d", ret);
        return ret;
    }

    std::unique_ptr<FileInfo> fileInfoTemp(reply.ReadParcelable<FileInfo>());
    if (fileInfoTemp == nullptr) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        return E_IPCS;
    }

    fileInfo = *fileInfoTemp;
    return ERR_OK;
}

int FileAccessExtProxy::GetFileInfoFromRelativePath(const std::string &selectFile, FileInfo &fileInfo)
{
    UserAccessTracer trace;
    trace.Start("GetFileInfoFromRelativePath");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    if (!data.WriteString(selectFile)) {
        HILOG_ERROR("fail to WriteParcelable selectFile");
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_GET_FILEINFO_FROM_RELATIVE_PATH, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return E_IPCS;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("GetFileInfoFromRelativePath operation failed ret : %{public}d", ret);
        return ret;
    }

    std::unique_ptr<FileInfo> fileInfoTemp(reply.ReadParcelable<FileInfo>());
    if (fileInfoTemp == nullptr) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        return E_IPCS;
    }

    fileInfo = *fileInfoTemp;
    return ERR_OK;
}

int FileAccessExtProxy::Access(const Uri &uri, bool &isExist)
{
    UserAccessTracer trace;
    trace.Start("Access");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    if (!data.WriteParcelable(&uri)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_ACCESS, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return E_IPCS;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("Access operation failed ret : %{public}d", ret);
        return ret;
    }

    if (!reply.ReadBool(isExist)) {
        HILOG_ERROR("fail to ReadInt32 isExist");
        return E_IPCS;
    }

    return ERR_OK;
}

int FileAccessExtProxy::StartWatcher(const Uri &uri)
{
    UserAccessTracer trace;
    trace.Start("StartWatcher");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    std::string uriString = uri.ToString();
    if (!data.WriteString(uriString)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_START_WATCHER, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return E_IPCS;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("StartWatcher operation failed ret : %{public}d", ret);
        return ret;
    }

    return ERR_OK;
}

int FileAccessExtProxy::StopWatcher(const Uri &uri)
{
    UserAccessTracer trace;
    trace.Start("StopWatcher");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    std::string uriString = uri.ToString();
    if (!data.WriteString(uriString)) {
        HILOG_ERROR("fail to WriteParcelable uri");
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_STOP_WATCHER, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return E_IPCS;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("StopWatcher operation failed ret : %{public}d", ret);
        return ret;
    }

    return ERR_OK;
}

static int ReadMoveItemFuncResults(OHOS::MessageParcel &reply, std::vector<Result> &moveResult)
{
    UserAccessTracer trace;
    trace.Start("ReadMoveItemFuncResults");
    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return E_IPCS;
    }
    if (ret == ERR_OK) {
        HILOG_ERROR("Move operation success");
        return ret;
    }

    uint32_t count = 0;
    if (!reply.ReadUint32(count)) {
        HILOG_ERROR("Move operation failed to Read count");
        return E_IPCS;
    }
    if (count > MAX_COPY_ERROR_COUNT) {
        HILOG_ERROR("Move operation failed, count value greater than max count");
        Result result { "", "", E_COUNT, "Count value greater than max count"};
        moveResult.clear();
        moveResult.push_back(result);
        return COPY_EXCEPTION;
    }

    moveResult.clear();
    for (uint32_t i = 0; i < count; i++) {
        std::unique_ptr<Result> moveResultPtr(reply.ReadParcelable<Result>());
        if (moveResultPtr != nullptr) {
            moveResult.push_back(*moveResultPtr);
        }
    }
    return ret;
}

int FileAccessExtProxy::MoveItem(const Uri &sourceFile, const Uri &targetParent, std::vector<Result> &moveResult,
                                 bool force)
{
    UserAccessTracer trace;
    trace.Start("MoveItem");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    std::string insideInputSourceUri = sourceFile.ToString();
    if (!data.WriteString(insideInputSourceUri)) {
        HILOG_ERROR("fail to WriteParcelable insideInputSourceUri");
        return E_IPCS;
    }

    std::string insideInputTargetUri = targetParent.ToString();
    if (!data.WriteString(insideInputTargetUri)) {
        HILOG_ERROR("fail to WriteParcelable insideInputTargetUri");
        return E_IPCS;
    }

    if (!data.WriteBool(force)) {
        HILOG_ERROR("fail to WriteBool force");
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_MOVE_ITEM, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest, err: %{public}d", err);
        return err;
    }

    auto ret = ReadMoveItemFuncResults(reply, moveResult);
    if (ret != ERR_OK) {
        HILOG_ERROR("Read moveItem function result error, code: %{public}d", ret);
        return ret;
    }

    return ret;
}

int FileAccessExtProxy::MoveFile(const Uri &sourceFile, const Uri &targetParent, std::string &fileName, Uri &newFile)
{
    UserAccessTracer trace;
    trace.Start("MoveFile");
    MessageParcel data;
    if (!data.WriteInterfaceToken(FileAccessExtProxy::GetDescriptor())) {
        HILOG_ERROR("WriteInterfaceToken failed");
        return E_IPCS;
    }

    std::string insideInputSourceUri = sourceFile.ToString();
    if (!data.WriteString(insideInputSourceUri)) {
        HILOG_ERROR("fail to WriteParcelable sourceFile");
        return E_IPCS;
    }

    std::string insideInputTargetUri = targetParent.ToString();
    if (!data.WriteString(insideInputTargetUri)) {
        HILOG_ERROR("fail to WriteParcelable targetParent");
        return E_IPCS;
    }

    if (!data.WriteString(fileName)) {
        HILOG_ERROR("fail to WriteParcelable fileName");
        return E_IPCS;
    }

    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(CMD_MOVE_FILE, data, reply, option);
    if (err != ERR_OK) {
        HILOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err;
    }

    int ret = E_IPCS;
    if (!reply.ReadInt32(ret)) {
        HILOG_ERROR("fail to ReadInt32 ret");
        return E_IPCS;
    }

    if (ret != ERR_OK) {
        HILOG_ERROR("Move file operation failed ret : %{public}d", ret);
        return ret;
    }

    std::string tempUri;
    if (!reply.ReadString(tempUri)) {
        HILOG_ERROR("ReadParcelable value is nullptr.");
        return E_IPCS;
    };

    if (tempUri.empty()) {
        HILOG_ERROR("get uri is empty.");
        return E_GETRESULT;
    }
    newFile = Uri(tempUri);

    return ERR_OK;
}
} // namespace FileAccessFwk
} // namespace OHOS
