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

#include "file_access_ext_stub.h"

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "access_token.h"
#include "accesstoken_kit.h"
#include "file_access_extension_info.h"
#include "file_access_framework_errno.h"
#include "image_source.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "ipc_object_stub.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "unique_fd.h"
#include "uri.h"

namespace OHOS {
namespace FileAccessFwk {
using namespace Media;
namespace {
    const std::string FILE_ACCESS_PERMISSION = "ohos.permission.FILE_ACCESS_MANAGER";
}
FileAccessExtStub::FileAccessExtStub()
{
    stubFuncMap_[CMD_OPEN_FILE] = &FileAccessExtStub::CmdOpenFile;
    stubFuncMap_[CMD_CREATE_FILE] = &FileAccessExtStub::CmdCreateFile;
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
    stubFuncMap_[CMD_GET_THUMBNAIL] = &FileAccessExtStub::CmdGetThumbnail;
    stubFuncMap_[CMD_GET_FILEINFO_FROM_URI] = &FileAccessExtStub::CmdGetFileInfoFromUri;
    stubFuncMap_[CMD_GET_FILEINFO_FROM_RELATIVE_PATH] = &FileAccessExtStub::CmdGetFileInfoFromRelativePath;
}

FileAccessExtStub::~FileAccessExtStub()
{
    stubFuncMap_.clear();
}

int FileAccessExtStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "OnRemoteRequest");
    if (!CheckCallingPermission(FILE_ACCESS_PERMISSION)) {
        HILOG_ERROR("permission error");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_PERMISSION;
    }

    std::u16string descriptor = FileAccessExtStub::GetDescriptor();
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

ErrCode FileAccessExtStub::CmdOpenFile(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdOpenFile");
    std::string insideInputUri;
    if (!data.ReadString(insideInputUri)) {
        HILOG_ERROR("Parameter OpenFile fail to ReadParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (insideInputUri.empty()) {
        HILOG_ERROR("Parameter OpenFile insideInputUri is empty");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return EINVAL;
    }

    int flags = E_IPCS;
    if (!data.ReadInt32(flags)) {
        HILOG_ERROR("Parameter OpenFile fail to ReadInt32 flags");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (flags < 0) {
        HILOG_ERROR("Parameter OpenFile flags is invalid");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return EINVAL;
    }

    int fd = -1;
    Uri uri(insideInputUri);
    int ret = OpenFile(uri, flags, fd);
    UniqueFd uniqueFd(fd);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter OpenFile fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!reply.WriteFileDescriptor(fd)) {
        HILOG_ERROR("Parameter OpenFile fail to WriteFileDescriptor fd");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdCreateFile(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdCreateFile");
    std::string insideInputUri;
    if (!data.ReadString(insideInputUri)) {
        HILOG_ERROR("Parameter OpenFile fail to ReadParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (insideInputUri.empty()) {
        HILOG_ERROR("Parameter CreateFile insideInputUri is empty");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return EINVAL;
    }

    std::string displayName = "";
    if (!data.ReadString(displayName)) {
        HILOG_ERROR("Parameter CreateFile fail to ReadString displayName");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (displayName.empty()) {
        HILOG_ERROR("Parameter CreateFile displayName is empty");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return EINVAL;
    }

    std::string newFile = "";
    OHOS::Uri newFileUri(newFile);
    Uri uri(insideInputUri);
    int ret = CreateFile(uri, displayName, newFileUri);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter CreateFile fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    std::string insideOutputUri = newFileUri.ToString();
    if (!reply.WriteString(insideOutputUri)) {
        HILOG_ERROR("Parameter CreateFile fail to WriteParcelable newFileUri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdMkdir(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdMkdir");
    std::string insideInputUri;
    if (!data.ReadString(insideInputUri)) {
        HILOG_ERROR("Parameter Mkdir fail to ReadParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (insideInputUri.empty()) {
        HILOG_ERROR("Parameter Mkdir insideInputUri is empty");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return EINVAL;
    }

    std::string displayName = "";
    if (!data.ReadString(displayName)) {
        HILOG_ERROR("Parameter Mkdir fail to ReadString displayName");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (displayName.empty()) {
        HILOG_ERROR("Parameter Mkdir displayName is empty");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return EINVAL;
    }

    std::string newFile = "";
    OHOS::Uri newFileUri(newFile);
    Uri uri(insideInputUri);
    int ret = Mkdir(uri, displayName, newFileUri);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Mkdir fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    std::string insideOutputUri = newFileUri.ToString();
    if (!reply.WriteString(insideOutputUri)) {
        HILOG_ERROR("Parameter Mkdir fail to WriteParcelable newFileUri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdDelete(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdDelete");
    std::string insideInputUri;
    if (!data.ReadString(insideInputUri)) {
        HILOG_ERROR("Parameter Delete fail to ReadParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (insideInputUri.empty()) {
        HILOG_ERROR("Parameter Delete insideInputUri is empty");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return EINVAL;
    }

    Uri uri(insideInputUri);
    int ret = Delete(uri);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Delete fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdMove(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdMove");
    std::string sourceFile;
    if (!data.ReadString(sourceFile)) {
        HILOG_ERROR("Parameter Move fail to ReadParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    std::string targetParent;
    if (!data.ReadString(targetParent)) {
        HILOG_ERROR("Parameter Move fail to ReadParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    std::string newFile = "";
    OHOS::Uri newFileUri(newFile);
    Uri source(sourceFile);
    Uri target(targetParent);
    int ret = Move(source, target, newFileUri);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Move fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    std::string insideOutputUri = newFileUri.ToString();
    if (!reply.WriteString(insideOutputUri)) {
        HILOG_ERROR("Parameter Move fail to WriteParcelable newFileUri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdCopy(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdCopy");
    std::string sourceUri;
    if (!data.ReadString(sourceUri)) {
        HILOG_ERROR("Parameter Copy fail to ReadParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    std::string destUri;
    if (!data.ReadString(destUri)) {
        HILOG_ERROR("Parameter Copy fail to ReadParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    bool force = false;
    if (!data.ReadBool(force)) {
        HILOG_ERROR("Parameter Copy fail to ReadBool force");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    std::vector<CopyResult> copyResult;
    Uri source(sourceUri);
    Uri dest(destUri);
    int ret = Copy(source, dest, copyResult, force);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Copy fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!reply.WriteUint32(copyResult.size())) {
        HILOG_ERROR("Parameter Copy fail to WriteInt32 size of copyResult");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    for (auto &result : copyResult) {
        if (!reply.WriteParcelable(&result)) {
            HILOG_ERROR("Parameter Copy fail to WriteParcelable copyResult");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return E_IPCS;
        }
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdRename(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdRename");
    std::string sourceFile;
    if (!data.ReadString(sourceFile)) {
        HILOG_ERROR("Parameter Rename fail to ReadParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    std::string displayName = "";
    if (!data.ReadString(displayName)) {
        HILOG_ERROR("Parameter Rename fail to ReadString displayName");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (displayName.empty()) {
        HILOG_ERROR("Parameter Rename displayName is empty");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return EINVAL;
    }

    std::string newFile = "";
    OHOS::Uri newFileUri(newFile);
    Uri source(sourceFile);
    int ret = Rename(source, displayName, newFileUri);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Rename fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    std::string insideOutputUri = newFileUri.ToString();
    if (!reply.WriteString(insideOutputUri)) {
        HILOG_ERROR("Parameter Rename fail to WriteParcelable newFileUri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdListFile(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdListFile");
    std::shared_ptr<FileInfo> fileInfo(data.ReadParcelable<FileInfo>());
    if (fileInfo == nullptr) {
        HILOG_ERROR("Parameter ListFile fail to ReadParcelable fileInfo");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int64_t offset = 0;
    if (!data.ReadInt64(offset)) {
        HILOG_ERROR("parameter ListFile offset is invalid");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int64_t maxCount = 0;
    if (!data.ReadInt64(maxCount)) {
        HILOG_ERROR("parameter ListFile maxCount is invalid");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    std::shared_ptr<FileFilter> filter(data.ReadParcelable<FileFilter>());
    if (filter == nullptr) {
        HILOG_ERROR("parameter ListFile FileFilter is invalid");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    std::vector<FileInfo> fileInfoVec;
    int ret = ListFile(*fileInfo, offset, maxCount, *filter, fileInfoVec);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter ListFile fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int64_t count {fileInfoVec.size()};
    if (!reply.WriteInt64(count)) {
        HILOG_ERROR("Parameter ListFile fail to WriteInt64 count");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    for (const auto &fileInfo : fileInfoVec) {
        if (!reply.WriteParcelable(&fileInfo)) {
            HILOG_ERROR("parameter ListFile fail to WriteParcelable fileInfoVec");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return E_IPCS;
        }
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdScanFile(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdScanFile");
    std::shared_ptr<FileInfo> fileInfo(data.ReadParcelable<FileInfo>());
    if (fileInfo == nullptr) {
        HILOG_ERROR("Parameter ScanFile fail to ReadParcelable fileInfo");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int64_t offset = 0;
    if (!data.ReadInt64(offset)) {
        HILOG_ERROR("parameter ScanFile offset is invalid");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int64_t maxCount = 0;
    if (!data.ReadInt64(maxCount)) {
        HILOG_ERROR("parameter ScanFile maxCount is invalid");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    std::shared_ptr<FileFilter> filter(data.ReadParcelable<FileFilter>());
    if (filter == nullptr) {
        HILOG_ERROR("parameter ScanFile FileFilter is invalid");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    std::vector<FileInfo> fileInfoVec;
    int ret = ScanFile(*fileInfo, offset, maxCount, *filter, fileInfoVec);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter ScanFile fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int64_t count {fileInfoVec.size()};
    if (!reply.WriteInt64(count)) {
        HILOG_ERROR("Parameter ScanFile fail to WriteInt64 count");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    for (const auto &fileInfo : fileInfoVec) {
        if (!reply.WriteParcelable(&fileInfo)) {
            HILOG_ERROR("parameter ScanFile fail to WriteParcelable fileInfoVec");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return E_IPCS;
        }
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdGetRoots(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdGetRoots");

    std::vector<RootInfo> rootInfoVec;
    int ret = GetRoots(rootInfoVec);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter GetRoots fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int64_t count {rootInfoVec.size()};
    if (!reply.WriteInt64(count)) {
        HILOG_ERROR("Parameter GetRoots fail to WriteInt64 count");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    for (const auto &rootInfo : rootInfoVec) {
        if (!reply.WriteParcelable(&rootInfo)) {
            HILOG_ERROR("parameter ListFile fail to WriteParcelable rootInfo");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return E_IPCS;
        }
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdQuery(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdQuery");
    std::string uri;
    if (!data.ReadString(uri)) {
        HILOG_ERROR("Parameter Query fail to ReadParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int64_t count = 0;
    if (!data.ReadInt64(count)) {
        HILOG_ERROR("Query operation failed to Read count");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }
    std::vector<std::string> columns;
    for (int64_t i = 0; i < count; i++) {
        columns.push_back(data.ReadString());
    }
    std::vector<std::string> results;
    Uri sourceUri(uri);
    int ret = Query(sourceUri, columns, results);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Query fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int64_t resCount {results.size()};
    if (!reply.WriteInt64(resCount)) {
        HILOG_ERROR("Parameter Query fail to WriteInt64 count");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    for (const auto &result : results) {
        if (!reply.WriteString(result)) {
            HILOG_ERROR("parameter Query fail to WriteParcelable column");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return E_IPCS;
        }
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdGetThumbnail(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdGetThumbnail");
    std::string uri;
    if (!data.ReadString(uri)) {
        HILOG_ERROR("Parameter GetThumbnail fail to ReadParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    std::shared_ptr<ThumbnailSize> thumbnailSize(data.ReadParcelable<ThumbnailSize>());
    if (thumbnailSize == nullptr) {
        HILOG_ERROR("Parameter GetThumbnail fail to ReadParcelable thumbnailSize");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_URIS;
    }

    std::shared_ptr<PixelMap> pixelMap;
    Uri sourceUri(uri);
    int ret = GetThumbnail(sourceUri, *thumbnailSize, pixelMap);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter CmdGetThumbnail fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!reply.WriteParcelable(pixelMap.get())) {
        HILOG_ERROR("Parameter CmdGetThumbnail fail to WriteParcelable pixelMap");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdGetFileInfoFromUri(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdGetFileInfoFromUri");
    std::string uri;
    if (!data.ReadString(uri)) {
        HILOG_ERROR("Parameter GetFileInfoFromUri fail to ReadParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    FileInfo fileInfoTemp;
    Uri sourceUri(uri);
    int ret = GetFileInfoFromUri(sourceUri, fileInfoTemp);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter GetFileInfoFromUri fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!reply.WriteParcelable(&fileInfoTemp)) {
        HILOG_ERROR("Parameter GetFileInfoFromUri fail to WriteParcelable fileInfoTemp");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdGetFileInfoFromRelativePath(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdGetFileInfoFromRelativePath");
    std::string relativePath(data.ReadString());

    FileInfo fileInfoTemp;
    int ret = GetFileInfoFromRelativePath(relativePath, fileInfoTemp);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter CmdGetFileInfoFromRelativePath fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!reply.WriteParcelable(&fileInfoTemp)) {
        HILOG_ERROR("Parameter CmdGetFileInfoFromRelativePath fail to WriteParcelable fileInfoTemp");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdAccess(MessageParcel &data, MessageParcel &reply)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CmdAccess");
    std::string uri;
    if (!data.ReadString(uri)) {
        HILOG_ERROR("Parameter Query fail to ReadParcelable uri");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    bool isExist = false;
    Uri sourceUri(uri);
    int ret = Access(sourceUri, isExist);
    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("Parameter Access fail to WriteInt32 ret");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    if (!reply.WriteBool(isExist)) {
        HILOG_ERROR("Parameter Access fail to WriteBool isExist");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

bool FileAccessExtStub::CheckCallingPermission(const std::string &permission)
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
