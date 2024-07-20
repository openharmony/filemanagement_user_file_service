/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "file_access_ext_stub.h"

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "user_access_tracer.h"
#include "access_token.h"
#include "accesstoken_kit.h"
#include "file_access_extension_info.h"
#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "ipc_object_stub.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "unique_fd.h"
#include "uri.h"

namespace OHOS {
namespace FileAccessFwk {
namespace {
    const std::string FILE_ACCESS_PERMISSION = "ohos.permission.FILE_ACCESS_MANAGER";
}
FileAccessExtStub::FileAccessExtStub()
{
    stubFuncMap_[CMD_OPEN_FILE] = &FileAccessExtStub::CmdOpenFile;
    stubFuncMap_[CMD_CREATE_FILE] = &FileAccessExtStub::CmdCreateFile;
    stubFuncMap_[CMD_START_WATCHER] = &FileAccessExtStub::CmdStartWatcher;
    stubFuncMap_[CMD_STOP_WATCHER] = &FileAccessExtStub::CmdStopWatcher;
    stubFuncMap_[CMD_MKDIR] = &FileAccessExtStub::CmdMkdir;
    stubFuncMap_[CMD_DELETE] = &FileAccessExtStub::CmdDelete;
    stubFuncMap_[CMD_MOVE] = &FileAccessExtStub::CmdMove;
    stubFuncMap_[CMD_COPY] = &FileAccessExtStub::CmdCopy;
    stubFuncMap_[CMD_RENAME] = &FileAccessExtStub::CmdRename;
    stubFuncMap_[CMD_LIST_FILE] = &FileAccessExtStub::CmdListFile;
    stubFuncMap_[CMD_SCAN_FILE] = &FileAccessExtStub::CmdScanFile;
    stubFuncMap_[CMD_QUERY] = &FileAccessExtStub::CmdQuery;
    stubFuncMap_[CMD_GET_ROOTS] = &FileAccessExtStub::CmdGetRoots;
    stubFuncMap_[CMD_ACCESS] = &FileAccessExtStub::CmdAccess;
    stubFuncMap_[CMD_GET_FILEINFO_FROM_URI] = &FileAccessExtStub::CmdGetFileInfoFromUri;
    stubFuncMap_[CMD_GET_FILEINFO_FROM_RELATIVE_PATH] = &FileAccessExtStub::CmdGetFileInfoFromRelativePath;
    stubFuncMap_[CMD_COPY_FILE] = &FileAccessExtStub::CmdCopyFile;
    stubFuncMap_[CMD_MOVE_ITEM] = &FileAccessExtStub::CmdMoveItem;
    stubFuncMap_[CMD_MOVE_FILE] = &FileAccessExtStub::CmdMoveFile;
}

FileAccessExtStub::~FileAccessExtStub()
{
    stubFuncMap_.clear();
}

int FileAccessExtStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    UserAccessTracer trace;
    trace.Start("OnRemoteRequest");
    std::u16string descriptor = FileAccessExtStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        return ERR_INVALID_STATE;
    }

    if (!CheckCallingPermission(FILE_ACCESS_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    const auto &itFunc = stubFuncMap_.find(code);
    if (itFunc != stubFuncMap_.end()) {
        return (this->*(itFunc->second))(data, reply);
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode FileAccessExtStub::CmdOpenFile(MessageParcel &data, MessageParcel &reply)
{
    UserAccessTracer trace;
    trace.Start("CmdOpenFile");
    std::shared_ptr<Uri> uri(data.ReadParcelable<Uri>());
    if (uri == nullptr) {
        HILOG_ERROR("Parameter OpenFile fail to ReadParcelable uri");
        return E_IPCS;
    }

    int flags = E_IPCS;
    if (!data.ReadInt32(flags)) {
        HILOG_ERROR("Parameter OpenFile fail to ReadInt32 flags");
        return E_IPCS;
    }

    if (flags < 0) {
        HILOG_ERROR("Parameter OpenFile flags is invalid");
        return EINVAL;
    }

    int fd = -1;
    int ret = OpenFile(*uri, flags, fd);
    UniqueFd uniqueFd(fd);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter OpenFile fail to WriteInt32 ret");
        return E_IPCS;
    }

    if (!reply.WriteFileDescriptor(fd)) {
        HILOG_ERROR("Parameter OpenFile fail to WriteFileDescriptor fd");
        return E_IPCS;
    }

    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdCreateFile(MessageParcel &data, MessageParcel &reply)
{
    UserAccessTracer trace;
    trace.Start("CmdCreateFile");
    std::shared_ptr<Uri> parent(data.ReadParcelable<Uri>());
    if (parent == nullptr) {
        HILOG_ERROR("Parameter CreateFile fail to ReadParcelable parent");
        return E_IPCS;
    }

    std::string displayName = "";
    if (!data.ReadString(displayName)) {
        HILOG_ERROR("Parameter CreateFile fail to ReadString displayName");
        return E_IPCS;
    }

    if (displayName.empty()) {
        HILOG_ERROR("Parameter CreateFile displayName is empty");
        return EINVAL;
    }

    std::string newFile = "";
    OHOS::Uri newFileUri(newFile);
    int ret = CreateFile(*parent, displayName, newFileUri);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter CreateFile fail to WriteInt32 ret");
        return E_IPCS;
    }

    if (!reply.WriteParcelable(&newFileUri)) {
        HILOG_ERROR("Parameter CreateFile fail to WriteParcelable newFileUri");
        return E_IPCS;
    }

    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdMkdir(MessageParcel &data, MessageParcel &reply)
{
    UserAccessTracer trace;
    trace.Start("CmdMkdir");
    std::shared_ptr<Uri> parent(data.ReadParcelable<Uri>());
    if (parent == nullptr) {
        HILOG_ERROR("Parameter Mkdir fail to ReadParcelable parent");
        return E_IPCS;
    }

    std::string displayName = "";
    if (!data.ReadString(displayName)) {
        HILOG_ERROR("Parameter Mkdir fail to ReadString displayName");
        return E_IPCS;
    }

    if (displayName.empty()) {
        HILOG_ERROR("Parameter Mkdir displayName is empty");
        return EINVAL;
    }

    std::string newFile = "";
    OHOS::Uri newFileUri(newFile);
    int ret = Mkdir(*parent, displayName, newFileUri);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Mkdir fail to WriteInt32 ret");
        return E_IPCS;
    }

    if (!reply.WriteParcelable(&newFileUri)) {
        HILOG_ERROR("Parameter Mkdir fail to WriteParcelable newFileUri");
        return E_IPCS;
    }

    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdDelete(MessageParcel &data, MessageParcel &reply)
{
    UserAccessTracer trace;
    trace.Start("CmdDelete");
    std::shared_ptr<Uri> uri(data.ReadParcelable<Uri>());
    if (uri == nullptr) {
        HILOG_ERROR("Parameter Delete fail to ReadParcelable uri");
        return E_IPCS;
    }

    int ret = Delete(*uri);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Delete fail to WriteInt32 ret");
        return E_IPCS;
    }

    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdMove(MessageParcel &data, MessageParcel &reply)
{
    UserAccessTracer trace;
    trace.Start("CmdMove");
    std::shared_ptr<Uri> sourceFile(data.ReadParcelable<Uri>());
    if (sourceFile == nullptr) {
        HILOG_ERROR("Parameter Move fail to ReadParcelable sourceFile");
        return E_IPCS;
    }

    std::shared_ptr<Uri> targetParent(data.ReadParcelable<Uri>());
    if (targetParent == nullptr) {
        HILOG_ERROR("Parameter Move fail to ReadParcelable targetParent");
        return E_IPCS;
    }

    std::string newFile = "";
    OHOS::Uri newFileUri(newFile);
    int ret = Move(*sourceFile, *targetParent, newFileUri);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Move fail to WriteInt32 ret");
        return E_IPCS;
    }

    if (!reply.WriteParcelable(&newFileUri)) {
        HILOG_ERROR("Parameter Move fail to WriteParcelable newFileUri");
        return E_IPCS;
    }

    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdCopy(MessageParcel &data, MessageParcel &reply)
{
    UserAccessTracer trace;
    trace.Start("CmdCopy");
    std::shared_ptr<Uri> sourceUri(data.ReadParcelable<Uri>());
    if (sourceUri == nullptr) {
        HILOG_ERROR("Parameter Copy fail to ReadParcelable sourceUri");
        return E_IPCS;
    }

    std::shared_ptr<Uri> destUri(data.ReadParcelable<Uri>());
    if (destUri == nullptr) {
        HILOG_ERROR("Parameter Copy fail to ReadParcelable destUri");
        return E_IPCS;
    }

    bool force = false;
    if (!data.ReadBool(force)) {
        HILOG_ERROR("Parameter Copy fail to ReadBool force");
        return E_IPCS;
    }

    std::vector<Result> copyResult;
    int ret = Copy(*sourceUri, *destUri, copyResult, force);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Copy fail to WriteInt32 ret");
        return E_IPCS;
    }

    if (!reply.WriteUint32(copyResult.size())) {
        HILOG_ERROR("Parameter Copy fail to WriteInt32 size of copyResult");
        return E_IPCS;
    }

    for (auto &result : copyResult) {
        if (!reply.WriteParcelable(&result)) {
            HILOG_ERROR("Parameter Copy fail to WriteParcelable copyResult");
            return E_IPCS;
        }
    }

    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdCopyFile(MessageParcel &data, MessageParcel &reply)
{
    UserAccessTracer trace;
    trace.Start("CmdCopyFile");
    std::string sourceUri = "";
    if (!data.ReadString(sourceUri)) {
        HILOG_ERROR("Parameter Copy file fail to ReadParcelable sourceUri");
        return E_IPCS;
    }

    std::string destUri = "";
    if (!data.ReadString(destUri)) {
        HILOG_ERROR("Parameter Copy file fail to ReadParcelable destUri");
        return E_IPCS;
    }

    std::string fileName = "";
    if (!data.ReadString(fileName)) {
        HILOG_ERROR("Parameter Copy file fail to ReadString fileName");
        return E_IPCS;
    }

    OHOS::Uri newFileUri("");
    Uri source(sourceUri);
    Uri dest(destUri);
    int ret = CopyFile(source, dest, fileName, newFileUri);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Copy file fail to WriteInt32 ret");
        return E_IPCS;
    }
    if (!reply.WriteString(newFileUri.ToString())) {
        HILOG_ERROR("Parameter Copy file fail to WriteString newFileUri");
        return E_IPCS;
    }

    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdRename(MessageParcel &data, MessageParcel &reply)
{
    UserAccessTracer trace;
    trace.Start("CmdRename");
    std::shared_ptr<Uri> sourceFile(data.ReadParcelable<Uri>());
    if (sourceFile == nullptr) {
        HILOG_ERROR("Parameter Rename fail to ReadParcelable sourceFile");
        return E_IPCS;
    }

    std::string displayName = "";
    if (!data.ReadString(displayName)) {
        HILOG_ERROR("Parameter Rename fail to ReadString displayName");
        return E_IPCS;
    }

    if (displayName.empty()) {
        HILOG_ERROR("Parameter Rename displayName is empty");
        return EINVAL;
    }

    std::string newFile = "";
    OHOS::Uri newFileUri(newFile);
    int ret = Rename(*sourceFile, displayName, newFileUri);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Rename fail to WriteInt32 ret");
        return E_IPCS;
    }

    if (!reply.WriteParcelable(&newFileUri)) {
        HILOG_ERROR("Parameter Rename fail to WriteParcelable newFileUri");
        return E_IPCS;
    }

    return ERR_OK;
}

std::tuple<int, std::shared_ptr<FileInfo>, int64_t, std::shared_ptr<FileFilter>, std::shared_ptr<SharedMemoryInfo>>
    ReadFileFilterFuncArguments(MessageParcel &data)
{
    std::shared_ptr<FileInfo> fileInfo(data.ReadParcelable<FileInfo>());
    if (fileInfo == nullptr) {
        HILOG_ERROR("Parameter ListFile fail to ReadParcelable fileInfo");
        return std::make_tuple(E_IPCS, nullptr, 0, nullptr, nullptr);
    }

    int64_t offset = 0;
    if (!data.ReadInt64(offset)) {
        HILOG_ERROR("parameter ListFile offset is invalid");
        return std::make_tuple(E_IPCS, nullptr, 0, nullptr, nullptr);
    }

    std::shared_ptr<FileFilter> filter(data.ReadParcelable<FileFilter>());
    if (filter == nullptr) {
        HILOG_ERROR("parameter ListFile FileFilter is invalid");
        return std::make_tuple(E_IPCS, nullptr, 0, nullptr, nullptr);
    }

    std::shared_ptr<SharedMemoryInfo> memInfo(data.ReadParcelable<SharedMemoryInfo>());
    if (memInfo == nullptr) {
        HILOG_ERROR("parameter ListFile SharedMemoryInfo is invalid");
        return std::make_tuple(E_IPCS, nullptr, 0, nullptr, nullptr);
    }

    return std::make_tuple(ERR_OK, fileInfo, offset, filter, memInfo);
}

int WriteFileFilterResults(MessageParcel &reply, SharedMemoryInfo &memInfo)
{
    if (!reply.WriteUint32(memInfo.dataCounts)) {
        HILOG_ERROR("fail to WriteUint32 dataCounts");
        return E_IPCS;
    }

    if (!reply.WriteUint64(memInfo.dataSize)) {
        HILOG_ERROR("fail to WriteUint32 dataSize");
        return E_IPCS;
    }

    if (!reply.WriteUint32(memInfo.leftDataCounts)) {
        HILOG_ERROR("fail to WriteUint32 leftDataCounts");
        return E_IPCS;
    }

    if (!reply.WriteBool(memInfo.isOver)) {
        HILOG_ERROR("fail to WriteBool isOver");
        return E_IPCS;
    }

    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdListFile(MessageParcel &data, MessageParcel &reply)
{
    UserAccessTracer trace;
    trace.Start("CmdListFile");
    int ret = E_IPCS;
    std::shared_ptr<FileInfo> fileInfo = nullptr;
    int64_t offset = 0;
    std::shared_ptr<FileFilter> filter = nullptr;
    std::shared_ptr<SharedMemoryInfo> memInfo = nullptr;

    std::tie(ret, fileInfo, offset, filter, memInfo) = ReadFileFilterFuncArguments(data);
    if (ret != ERR_OK) {
        return ret;
    }
    ret = SharedMemoryOperation::MapSharedMemory(*memInfo);
    if (ret != ERR_OK) {
        return ret;
    }

    ret = ListFile(*fileInfo, offset, *filter, *memInfo);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("ListFile fail to WriteInt32 ret");
        SharedMemoryOperation::DestroySharedMemory(*memInfo);
        return E_IPCS;
    }
 
    ret = WriteFileFilterResults(reply, *memInfo);
    SharedMemoryOperation::DestroySharedMemory(*memInfo);
    return ret;
}

ErrCode FileAccessExtStub::CmdScanFile(MessageParcel &data, MessageParcel &reply)
{
    UserAccessTracer trace;
    trace.Start("CmdScanFile");
    std::shared_ptr<FileInfo> fileInfo(data.ReadParcelable<FileInfo>());
    if (fileInfo == nullptr) {
        HILOG_ERROR("Parameter ScanFile fail to ReadParcelable fileInfo");
        return E_IPCS;
    }

    int64_t offset = 0;
    if (!data.ReadInt64(offset)) {
        HILOG_ERROR("parameter ScanFile offset is invalid");
        return E_IPCS;
    }

    int64_t maxCount = 0;
    if (!data.ReadInt64(maxCount)) {
        HILOG_ERROR("parameter ScanFile maxCount is invalid");
        return E_IPCS;
    }

    std::shared_ptr<FileFilter> filter(data.ReadParcelable<FileFilter>());
    if (filter == nullptr) {
        HILOG_ERROR("parameter ScanFile FileFilter is invalid");
        return E_IPCS;
    }

    std::vector<FileInfo> fileInfoVec;
    int ret = ScanFile(*fileInfo, offset, maxCount, *filter, fileInfoVec);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter ScanFile fail to WriteInt32 ret");
        return E_IPCS;
    }

    int64_t count {fileInfoVec.size()};
    if (!reply.WriteInt64(count)) {
        HILOG_ERROR("Parameter ScanFile fail to WriteInt64 count");
        return E_IPCS;
    }

    for (const auto &fileInfo : fileInfoVec) {
        if (!reply.WriteParcelable(&fileInfo)) {
            HILOG_ERROR("parameter ScanFile fail to WriteParcelable fileInfoVec");
            return E_IPCS;
        }
    }

    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdGetRoots(MessageParcel &data, MessageParcel &reply)
{
    UserAccessTracer trace;
    trace.Start("CmdGetRoots");

    std::vector<RootInfo> rootInfoVec;
    int ret = GetRoots(rootInfoVec);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter GetRoots fail to WriteInt32 ret");
        return E_IPCS;
    }

    int64_t count {rootInfoVec.size()};
    if (!reply.WriteInt64(count)) {
        HILOG_ERROR("Parameter GetRoots fail to WriteInt64 count");
        return E_IPCS;
    }

    for (const auto &rootInfo : rootInfoVec) {
        if (!reply.WriteParcelable(&rootInfo)) {
            HILOG_ERROR("parameter ListFile fail to WriteParcelable rootInfo");
            return E_IPCS;
        }
    }

    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdQuery(MessageParcel &data, MessageParcel &reply)
{
    UserAccessTracer trace;
    trace.Start("CmdQuery");
    std::shared_ptr<Uri> uri(data.ReadParcelable<Uri>());
    if (uri == nullptr) {
        HILOG_ERROR("Parameter Query fail to ReadParcelable uri");
        return E_IPCS;
    }

    int64_t count = 0;
    if (!data.ReadInt64(count)) {
        HILOG_ERROR("Query operation failed to Read count");
        return E_IPCS;
    }
    if (count > static_cast<int64_t>(FILE_RESULT_TYPE.size())) {
        HILOG_ERROR(" The number of query operations exceeds %{public}zu ", FILE_RESULT_TYPE.size());
        return EINVAL;
    }
    std::vector<std::string> columns;
    for (int64_t i = 0; i < count; i++) {
        columns.push_back(data.ReadString());
    }
    std::vector<std::string> results;
    int ret = Query(*uri, columns, results);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Query fail to WriteInt32 ret");
        return E_IPCS;
    }

    int64_t resCount {results.size()};
    if (!reply.WriteInt64(resCount)) {
        HILOG_ERROR("Parameter Query fail to WriteInt64 count");
        return E_IPCS;
    }

    for (const auto &result : results) {
        if (!reply.WriteString(result)) {
            HILOG_ERROR("parameter Query fail to WriteParcelable column");
            return E_IPCS;
        }
    }

    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdGetFileInfoFromUri(MessageParcel &data, MessageParcel &reply)
{
    UserAccessTracer trace;
    trace.Start("CmdGetFileInfoFromUri");
    std::shared_ptr<Uri> uri(data.ReadParcelable<Uri>());
    if (uri == nullptr) {
        HILOG_ERROR("SelectFile uri is nullptr");
        return E_URIS;
    }

    FileInfo fileInfoTemp;
    int ret = GetFileInfoFromUri(*uri, fileInfoTemp);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter GetFileInfoFromUri fail to WriteInt32 ret");
        return E_IPCS;
    }

    if (!reply.WriteParcelable(&fileInfoTemp)) {
        HILOG_ERROR("Parameter GetFileInfoFromUri fail to WriteParcelable fileInfoTemp");
        return E_IPCS;
    }

    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdGetFileInfoFromRelativePath(MessageParcel &data, MessageParcel &reply)
{
    UserAccessTracer trace;
    trace.Start("CmdGetFileInfoFromRelativePath");
    std::string relativePath(data.ReadString());

    FileInfo fileInfoTemp;
    int ret = GetFileInfoFromRelativePath(relativePath, fileInfoTemp);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter CmdGetFileInfoFromRelativePath fail to WriteInt32 ret");
        return E_IPCS;
    }

    if (!reply.WriteParcelable(&fileInfoTemp)) {
        HILOG_ERROR("Parameter CmdGetFileInfoFromRelativePath fail to WriteParcelable fileInfoTemp");
        return E_IPCS;
    }

    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdAccess(MessageParcel &data, MessageParcel &reply)
{
    UserAccessTracer trace;
    trace.Start("CmdAccess");
    std::shared_ptr<Uri> uri(data.ReadParcelable<Uri>());
    if (uri == nullptr) {
        HILOG_ERROR("Access uri is nullptr");
        return E_URIS;
    }

    bool isExist = false;
    int ret = Access(*uri, isExist);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Access fail to WriteInt32 ret");
        return E_IPCS;
    }

    if (!reply.WriteBool(isExist)) {
        HILOG_ERROR("Parameter Access fail to WriteBool isExist");
        return E_IPCS;
    }

    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdStartWatcher(MessageParcel &data, MessageParcel &reply)
{
    UserAccessTracer trace;
    trace.Start("CmdStartWatcher");
    std::string uriString;
    if (!data.ReadString(uriString)) {
        HILOG_ERROR("Parameter StartWatcher fail to ReadParcelable uri");
        return E_IPCS;
    }

    if (uriString.empty()) {
        HILOG_ERROR("Parameter StartWatcher insideInputUri is empty");
        return EINVAL;
    }

    Uri uri(uriString);
    int ret = StartWatcher(uri);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter StartWatcher fail to WriteInt32 ret");
        return E_IPCS;
    }

    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdStopWatcher(MessageParcel &data, MessageParcel &reply)
{
    UserAccessTracer trace;
    trace.Start("CmdStopWatcher");
    std::string uriString;
    if (!data.ReadString(uriString)) {
        HILOG_ERROR("Parameter StopWatcher fail to ReadParcelable uri");
        return E_IPCS;
    }

    if (uriString.empty()) {
        HILOG_ERROR("Parameter StopWatcher insideInputUri is empty");
        return EINVAL;
    }

    Uri uri(uriString);
    int ret = StopWatcher(uri);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter StopWatcher fail to WriteInt32 ret");
        return E_IPCS;
    }

    return ERR_OK;
}
bool FileAccessExtStub::CheckCallingPermission(const std::string &permission)
{
    UserAccessTracer trace;
    trace.Start("CheckCallingPermission");
    Security::AccessToken::AccessTokenID tokenCaller = IPCSkeleton::GetCallingTokenID();
    int res = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller, permission);
    if (res != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        HILOG_ERROR("FileAccessExtStub::CheckCallingPermission have no fileAccess permission");
        return false;
    }

    return true;
}

ErrCode FileAccessExtStub::CmdMoveItem(MessageParcel &data, MessageParcel &reply)
{
    UserAccessTracer trace;
    trace.Start("CmdMoveItem");
    std::string sourceFile;
    if (!data.ReadString(sourceFile)) {
        HILOG_ERROR("Parameter Move fail to ReadParcelable uri");
        return E_IPCS;
    }

    std::string targetParent;
    if (!data.ReadString(targetParent)) {
        HILOG_ERROR("Parameter Move fail to ReadParcelable uri");
        return E_IPCS;
    }

    bool force = false;
    if (!data.ReadBool(force)) {
        HILOG_ERROR("Parameter Copy fail to ReadBool force");
        return E_IPCS;
    }

    std::vector<Result> moveResult;
    Uri source(sourceFile);
    Uri dest(targetParent);
    int ret = MoveItem(source, dest, moveResult, force);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Copy fail to WriteInt32 ret");
        return E_IPCS;
    }

    if (!reply.WriteUint32(moveResult.size())) {
        HILOG_ERROR("Parameter Copy fail to WriteInt32 size of copyResult");
        return E_IPCS;
    }

    for (auto &result : moveResult) {
        if (!reply.WriteParcelable(&result)) {
            HILOG_ERROR("Parameter Copy fail to WriteParcelable copyResult");
            return E_IPCS;
        }
    }

    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdMoveFile(MessageParcel &data, MessageParcel &reply)
{
    UserAccessTracer trace;
    trace.Start("CmdMoveFile");
    std::string sourceFile;
    if (!data.ReadString(sourceFile)) {
        HILOG_ERROR("Parameter Move fail to ReadParcelable sourceUri");
        return E_IPCS;
    }

    std::string targetParent;
    if (!data.ReadString(targetParent)) {
        HILOG_ERROR("Parameter Move fail to ReadParcelable targetParentUri");
        return E_IPCS;
    }

    std::string fileName;
    if (!data.ReadString(fileName)) {
        HILOG_ERROR("Parameter Move fail to ReadParcelable fileName");
        return E_IPCS;
    }
    std::string newFile;
    OHOS::Uri newFileUri(newFile);
    Uri source(sourceFile);
    Uri target(targetParent);
    int ret = MoveFile(source, target, fileName, newFileUri);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Move fail to WriteInt32 ret");
        return E_IPCS;
    }

    std::string insideOutputUri = newFileUri.ToString();
    if (!reply.WriteString(insideOutputUri)) {
        HILOG_ERROR("Parameter Move fail to WriteParcelable newFileUri");
        return E_IPCS;
    }

    return ERR_OK;
}
} // namespace FileAccessFwk
} // namespace OHOS
