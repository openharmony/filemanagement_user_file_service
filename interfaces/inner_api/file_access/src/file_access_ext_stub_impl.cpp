/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "file_access_ext_stub_impl.h"

#include <cinttypes>
#include "user_access_tracer.h"
#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "access_token.h"
#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "file_uri_check.h"

namespace OHOS {
namespace FileAccessFwk {
const uint64_t FILEFILTER_DEFAULT_COUNTS = 2000;
const uint64_t FILEFILTER_MAX_COUNTS = 20000;
const std::string FILE_ACCESS_MANAGER_PERMISSION = "ohos.permission.FILE_ACCESS_MANAGER";
std::shared_ptr<FileAccessExtAbility> FileAccessExtStubImpl::GetOwner()
{
    return extension_;
}
    
bool FileAccessExtStubImpl::CheckCallingPermission(const std::string &permission)
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

int FileAccessExtStubImpl::OpenFile(const Urie &uri, const int flags, int &fd)
{
    UserAccessTracer trace;
    trace.Start("OpenFile");
    if (!IsFilePathValid(uri.ToString().c_str())) {
        HILOG_ERROR ("uri is invalid");
        return E_URIS;
    }
    if (extension_ == nullptr) {
        HILOG_ERROR("OpenFile get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }
    if (flags < 0) {
        HILOG_ERROR("Parameter OpenFile flags is invalid");
        return EINVAL;
    }

    fd = -1;
    Uri uriConvert(uri.ToString());
    int ret = extension_->OpenFile(uriConvert, flags, fd);
    return ret;
}

int FileAccessExtStubImpl::CreateFile(const Urie &parent, const std::string &displayName, Urie &newFile)
{
    UserAccessTracer trace;
    trace.Start("CreateFile");
    if (!IsFilePathValid(parent.ToString().c_str())) {
        HILOG_ERROR("parent uri is invalid");
        return E_URIS;
    }
    if (extension_ == nullptr) {
        HILOG_ERROR("CreateFile get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri parentConvert(parent.ToString());
    Uri newFileConvert("");
    int ret = extension_->CreateFile(parentConvert, displayName, newFileConvert);
    newFile = Urie(newFileConvert.ToString());
    return ret;
}

int FileAccessExtStubImpl::Mkdir(const Urie &parent, const std::string &displayName, Urie &newFile)
{
    UserAccessTracer trace;
    trace.Start("Mkdir");
    if (!IsFilePathValid(parent.ToString().c_str())) {
        HILOG_ERROR("parent uri is invalid");
        return E_URIS;
    }
    if (extension_ == nullptr) {
        HILOG_ERROR("Mkdir get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri parentConvert(parent.ToString());
    Uri newFileConvert("");
    int ret = extension_->Mkdir(parentConvert, displayName, newFileConvert);
    newFile = Urie(newFileConvert.ToString());
    return ret;
}

int FileAccessExtStubImpl::Delete(const Urie &sourceFile)
{
    UserAccessTracer trace;
    trace.Start("Delete");
    if (!IsFilePathValid(sourceFile.ToString().c_str())) {
        HILOG_ERROR("sourceFile uri is invalid");
        return E_URIS;
    }
    if (extension_ == nullptr) {
        HILOG_ERROR("Delete get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri sourceFileConvert(sourceFile.ToString());
    int ret = extension_->Delete(sourceFileConvert);
    return ret;
}

int FileAccessExtStubImpl::Move(const Urie &sourceFile, const Urie &targetParent, Urie &newFile)
{
    UserAccessTracer trace;
    trace.Start("Move");
    if (!IsFilePathValid(sourceFile.ToString().c_str())) {
        HILOG_ERROR("sourceFile uri is invalid");
        return E_URIS;
    }
    if (!IsFilePathValid(targetParent.ToString().c_str())) {
        HILOG_ERROR("targetParent uri is invalid");
        return E_URIS;
    }
    if (extension_ == nullptr) {
        HILOG_ERROR("Move get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri sourceFileConvert(sourceFile.ToString());
    Uri targetParentConvert(targetParent.ToString());
    Uri newFileConvert("");
    int ret = extension_->Move(sourceFileConvert, targetParentConvert, newFileConvert);
    newFile = Urie(newFileConvert.ToString());
    return ret;
}

int FileAccessExtStubImpl::Copy(const Urie &sourceUri, const Urie &destUri, std::vector<Result> &copyResult,
    int32_t& retCode, bool force)
{
    UserAccessTracer trace;
    trace.Start("Copy");
    if (!IsFilePathValid(sourceUri.ToString().c_str())) {
        HILOG_ERROR("sourceUri is invalid");
        return E_URIS;
    }
    if (!IsFilePathValid(destUri.ToString().c_str())) {
        HILOG_ERROR("destUri is invalid");
        return E_URIS;
    }
    if (extension_ == nullptr) {
        HILOG_ERROR("Copy get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri sourceUriConvert(sourceUri.ToString());
    Uri destUriConvert(destUri.ToString());
    int ret = extension_->Copy(sourceUriConvert, destUriConvert, copyResult, force);
    retCode = ret;
    return ERR_OK;
}

int FileAccessExtStubImpl::CopyFile(const Urie &sourceUri, const Urie &destUri, const std::string &fileName,
    Urie &newFileUri)
{
    UserAccessTracer trace;
    trace.Start("CopyFile");
    if (!IsFilePathValid(sourceUri.ToString().c_str())) {
        HILOG_ERROR("sourceUri is invalid");
        return E_URIS;
    }
    if (!IsFilePathValid(destUri.ToString().c_str())) {
        HILOG_ERROR("destUri is invalid");
        return E_URIS;
    }
    if (extension_ == nullptr) {
        HILOG_ERROR("Copy file get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri sourceUriConvert(sourceUri.ToString());
    Uri destUriConvert(destUri.ToString());
    Uri newFileUriConvert("");
    int ret = extension_->CopyFile(sourceUriConvert, destUriConvert, fileName, newFileUriConvert);
    newFileUri = Urie(newFileUriConvert.ToString());
    return ret;
}

int FileAccessExtStubImpl::Rename(const Urie &sourceFile, const std::string &displayName, Urie &newFile)
{
    UserAccessTracer trace;
    trace.Start("Rename");
    if (!IsFilePathValid(sourceFile.ToString().c_str())) {
        HILOG_ERROR("sourceFile uri is invalid");
        return E_URIS;
    }
    if (extension_ == nullptr) {
        HILOG_ERROR("Rename get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri sourceFileConvert(sourceFile.ToString());
    Uri newFileConvert("");
    int ret = extension_->Rename(sourceFileConvert, displayName, newFileConvert);
    newFile = Urie(newFileConvert.ToString());
    return ret;
}

int FileAccessExtStubImpl::ListFile(const FileInfo &fileInfo, const int64_t offset,
    const FileFilter &filter, SharedMemoryInfo &memInfo)
{
    UserAccessTracer trace;
    trace.Start("ListFile");
    if (!IsFilePathValid(fileInfo.uri.c_str())) {
        HILOG_ERROR("fileInfo.uri is invalid");
        return E_URIS;
    }
    if (extension_ == nullptr) {
        HILOG_ERROR("ListFile get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    int ret = SharedMemoryOperation::MapSharedMemory(memInfo);
    if (ret != ERR_OK) {
        HILOG_ERROR("failed to MapSharedMemory");
        return ret;
    }

    std::vector<FileInfo> fileInfoVec;
    memInfo.isOver = false;
    int64_t currentOffset = offset;
    while (true) {
        uint64_t maxCounts =
            memInfo.memSize > DEFAULT_CAPACITY_200KB ? FILEFILTER_MAX_COUNTS : FILEFILTER_DEFAULT_COUNTS;
        fileInfoVec.clear();
        ret = extension_->ListFile(fileInfo, currentOffset, maxCounts, filter, fileInfoVec);
        if (ret != ERR_OK) {
            HILOG_ERROR("Extension ability ListFile error, code:%{public}d", ret);
            break;
        }

        uint32_t currentWriteCounts = SharedMemoryOperation::WriteFileInfos(fileInfoVec, memInfo);
        if (currentWriteCounts < fileInfoVec.size()) {
            if (memInfo.memSize == DEFAULT_CAPACITY_200KB) {
                uint32_t counts = 0;
                extension_->GetFileInfoNum(fileInfo.uri, filter, false, counts);
                memInfo.leftDataCounts = counts - memInfo.dataCounts;
            }
            break;
        }
        if (fileInfoVec.empty() ||(maxCounts > static_cast<uint64_t>(fileInfoVec.size())
            && currentWriteCounts == fileInfoVec.size())) {
            memInfo.isOver = true;
            break;
        }
        currentOffset += currentWriteCounts;
    }

    return ret;
}

int FileAccessExtStubImpl::ScanFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
    const FileFilter &filter, std::vector<FileInfo> &fileInfoVec)
{
    UserAccessTracer trace;
    trace.Start("ScanFile");
    if (!IsFilePathValid(fileInfo.uri.c_str())) {
        HILOG_ERROR("fileInfo.uri is invalid");
        return E_URIS;
    }
    if (extension_ == nullptr) {
        HILOG_ERROR("ScanFile get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    int ret = extension_->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    return ret;
}

int FileAccessExtStubImpl::Query(const Urie &uri, const std::vector<std::string> &columns,
                                 std::vector<std::string> &results)
{
    UserAccessTracer trace;
    trace.Start("Query");
    if (!IsFilePathValid(uri.ToString().c_str())) {
        HILOG_ERROR("uri is invalid");
        return E_URIS;
    }
    if (extension_ == nullptr) {
        HILOG_ERROR("Query get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    std::vector<std::string> newColumns = const_cast<std::vector<std::string>&>(columns);
    Uri uriConvert(uri.ToString());
    int ret = extension_->Query(uriConvert, newColumns, results);
    return ret;
}

int FileAccessExtStubImpl::GetRoots(std::vector<RootInfo> &rootInfoVec)
{
    UserAccessTracer trace;
    trace.Start("GetRoots");
    if (extension_ == nullptr) {
        HILOG_ERROR("GetRoots get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    int ret = extension_->GetRoots(rootInfoVec);
    return ret;
}

int FileAccessExtStubImpl::GetFileInfoFromUri(const Urie &selectFile, FileInfo &fileInfo)
{
    UserAccessTracer trace;
    trace.Start("GetFileInfoFromUri");
    if (!IsFilePathValid(selectFile.ToString().c_str())) {
        HILOG_ERROR("selectFile uri is invalid");
        return E_URIS;
    }
    if (extension_ == nullptr) {
        HILOG_ERROR("GetFileInfoFromUri get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri selectFileConvert(selectFile.ToString());
    int ret = extension_->GetFileInfoFromUri(selectFileConvert, fileInfo);
    return ret;
}

int FileAccessExtStubImpl::GetFileInfoFromRelativePath(const std::string &selectFile, FileInfo &fileInfo)
{
    UserAccessTracer trace;
    trace.Start("GetFileInfoFromRelativePath");
    if (!IsFilePathValid(selectFile.c_str())) {
        HILOG_ERROR("selectFile path is invalid");
        return E_URIS;
    }
    if (extension_ == nullptr) {
        HILOG_ERROR("GetFileInfoFromRelativePath get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    int ret = extension_->GetFileInfoFromRelativePath(selectFile, fileInfo);
    return ret;
}

int FileAccessExtStubImpl::Access(const Urie &uri, bool &isExist)
{
    UserAccessTracer trace;
    trace.Start("Access");
    if (!IsFilePathValid(uri.ToString().c_str())) {
        HILOG_ERROR("uri is invalid");
        return E_URIS;
    }
    if (extension_ == nullptr) {
        HILOG_ERROR("Access get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri uriConvert(uri.ToString());
    int ret = extension_->Access(uriConvert, isExist);
    return ret;
}

int FileAccessExtStubImpl::StartWatcher(const Urie &uri)
{
    UserAccessTracer trace;
    trace.Start("StartWatcher");
    if (!IsFilePathValid(uri.ToString().c_str())) {
        HILOG_ERROR("uri is invalid");
        return E_URIS;
    }
    if (extension_ == nullptr) {
        HILOG_ERROR("StartWatcher get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri uriConvert(uri.ToString());
    int ret = extension_->StartWatcher(uriConvert);
    return ret;
}

int FileAccessExtStubImpl::StopWatcher(const Urie &uri)
{
    UserAccessTracer trace;
    trace.Start("StopWatcher");
    if (!IsFilePathValid(uri.ToString().c_str())) {
        HILOG_ERROR("uri is invalid");
        return E_URIS;
    }
    if (extension_ == nullptr) {
        HILOG_ERROR("StopWatcher get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri uriConvert(uri.ToString());
    int ret = extension_->StopWatcher(uriConvert);
    return ret;
}

int FileAccessExtStubImpl::MoveItem(const Urie &sourceFile, const Urie &targetParent, std::vector<Result> &moveResult,
                                    int32_t& retCode, bool force)
{
    UserAccessTracer trace;
    trace.Start("MoveItem");
    if (!IsFilePathValid(sourceFile.ToString().c_str())) {
        HILOG_ERROR("sourceFile uri is invalid");
        return E_URIS;
    }
    if (!IsFilePathValid(targetParent.ToString().c_str())) {
        HILOG_ERROR("targetParent uri is invalid");
        return E_URIS;
    }
    if (extension_ == nullptr) {
        HILOG_ERROR("Move get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri sourceFileConvert(sourceFile.ToString());
    Uri targetParentConvert(targetParent.ToString());
    int ret = extension_->MoveItem(sourceFileConvert, targetParentConvert, moveResult, force);
    retCode = ret;
    return ERR_OK;
}

int FileAccessExtStubImpl::MoveFile(const Urie &sourceFile, const Urie &targetParent, const std::string &fileName,
                                    Urie &newFile)
{
    UserAccessTracer trace;
    trace.Start("MoveFile");
    if (!IsFilePathValid(sourceFile.ToString().c_str())) {
        HILOG_ERROR("sourceFile uri is invalid");
        return E_URIS;
    }
    if (!IsFilePathValid(targetParent.ToString().c_str())) {
        HILOG_ERROR("targetParent uri is invalid");
        return E_URIS;
    }
    if (extension_ == nullptr) {
        HILOG_ERROR("Move get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    std::string newFileName = const_cast<std::string&>(fileName);
    Uri sourceFileConvert(sourceFile.ToString());
    Uri targetParentConvert(targetParent.ToString());
    Uri newFileConvert("");
    int ret =  extension_->MoveFile(sourceFileConvert, targetParentConvert, newFileName, newFileConvert);
    newFile = Urie(newFileConvert.ToString());
    return ret;
}
} // namespace FileAccessFwk
} // namespace OHOS
