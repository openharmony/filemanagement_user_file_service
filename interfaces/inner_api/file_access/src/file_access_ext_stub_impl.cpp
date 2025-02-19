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

#include "file_access_ext_stub_impl.h"

#include <cinttypes>
#include "user_access_tracer.h"
#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "access_token.h"
#include "accesstoken_kit.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace FileAccessFwk {
const uint64_t FILEFILTER_DEFAULT_COUNTS = 2000;
const uint64_t FILEFILTER_MAX_COUNTS = 20000;
std::shared_ptr<FileAccessExtAbility> FileAccessExtStubImpl::GetOwner()
{
    return extension_;
}

namespace {
    const std::string FILE_ACCESS_MANAGER_PERMISSION = "ohos.permission.FILE_ACCESS_MANAGER";
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
    Uri uri_(uri.ToString());
    int ret = extension_->OpenFile(uri_, flags, fd);
    return ret;
}

int FileAccessExtStubImpl::CreateFile(const Urie &parent, const std::string &displayName, Urie &newFile)
{
    UserAccessTracer trace;
    trace.Start("CreateFile");
    if (extension_ == nullptr) {
        HILOG_ERROR("CreateFile get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri parent_(parent.ToString());
    Uri newFile_("");
    int ret = extension_->CreateFile(parent_, displayName, newFile_);
    newFile = Urie(newFile_.ToString());
    return ret;
}

int FileAccessExtStubImpl::Mkdir(const Urie &parent, const std::string &displayName, Urie &newFile)
{
    UserAccessTracer trace;
    trace.Start("Mkdir");
    if (extension_ == nullptr) {
        HILOG_ERROR("Mkdir get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri parent_(parent.ToString());
    Uri newFile_("");
    int ret = extension_->Mkdir(parent_, displayName, newFile_);
    newFile = Urie(newFile_.ToString());
    return ret;
}

int FileAccessExtStubImpl::Delete(const Urie &sourceFile)
{
    UserAccessTracer trace;
    trace.Start("Delete");
    if (extension_ == nullptr) {
        HILOG_ERROR("Delete get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri sourceFile_(sourceFile.ToString());
    int ret = extension_->Delete(sourceFile_);
    return ret;
}

int FileAccessExtStubImpl::Move(const Urie &sourceFile, const Urie &targetParent, Urie &newFile)
{
    UserAccessTracer trace;
    trace.Start("Move");
    if (extension_ == nullptr) {
        HILOG_ERROR("Move get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri sourceFile_(sourceFile.ToString());
    Uri targetParent_(targetParent.ToString());
    Uri newFile_("");
    int ret = extension_->Move(sourceFile_, targetParent_, newFile_);
    newFile = Urie(newFile_.ToString());
    return ret;
}

int FileAccessExtStubImpl::Copy(const Urie &sourceUri, const Urie &destUri, std::vector<Result> &copyResult,
    bool force)
{
    UserAccessTracer trace;
    trace.Start("Copy");
    if (extension_ == nullptr) {
        HILOG_ERROR("Copy get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri sourceUri_(sourceUri.ToString());
    Uri destUri_(destUri.ToString());
    int ret = extension_->Copy(sourceUri_, destUri_, copyResult, force);
    return ret;
}

int FileAccessExtStubImpl::CopyFile(const Urie &sourceUri, const Urie &destUri, const std::string &fileName,
    Urie &newFileUri)
{
    UserAccessTracer trace;
    trace.Start("CopyFile");
    if (extension_ == nullptr) {
        HILOG_ERROR("Copy file get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri sourceUri_(sourceUri.ToString());
    Uri destUri_(destUri.ToString());
    Uri newFileUri_("");
    int ret = extension_->CopyFile(sourceUri_, destUri_, fileName, newFileUri_);
    newFileUri = Urie(newFileUri_.ToString());
    return ret;
}

int FileAccessExtStubImpl::Rename(const Urie &sourceFile, const std::string &displayName, Urie &newFile)
{
    UserAccessTracer trace;
    trace.Start("Rename");
    if (extension_ == nullptr) {
        HILOG_ERROR("Rename get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri sourceFile_(sourceFile.ToString());
    Uri newFile_("");
    int ret = extension_->Rename(sourceFile_, displayName, newFile_);
    newFile = Urie(newFile_.ToString());
    return ret;
}

int FileAccessExtStubImpl::ListFile(const FileInfo &fileInfo, const int64_t offset,
    const FileFilter &filter, SharedMemoryInfo &memInfo)
{
    UserAccessTracer trace;
    trace.Start("ListFile");
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
    if (extension_ == nullptr) {
        HILOG_ERROR("Query get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    std::vector<std::string> newColumns = const_cast<std::vector<std::string>&>(columns);
    Uri uri_(uri.ToString());
    int ret = extension_->Query(uri_, newColumns, results);
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
    if (extension_ == nullptr) {
        HILOG_ERROR("GetFileInfoFromUri get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri selectFile_(selectFile.ToString());
    int ret = extension_->GetFileInfoFromUri(selectFile_, fileInfo);
    return ret;
}

int FileAccessExtStubImpl::GetFileInfoFromRelativePath(const std::string &selectFile, FileInfo &fileInfo)
{
    UserAccessTracer trace;
    trace.Start("GetFileInfoFromRelativePath");
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
    if (extension_ == nullptr) {
        HILOG_ERROR("Access get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri uri_(uri.ToString());
    int ret = extension_->Access(uri_, isExist);
    return ret;
}

int FileAccessExtStubImpl::StartWatcher(const Urie &uri)
{
    UserAccessTracer trace;
    trace.Start("StartWatcher");
    if (extension_ == nullptr) {
        HILOG_ERROR("StartWatcher get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri uri_(uri.ToString());
    int ret = extension_->StartWatcher(uri_);
    return ret;
}

int FileAccessExtStubImpl::StopWatcher(const Urie &uri)
{
    UserAccessTracer trace;
    trace.Start("StopWatcher");
    if (extension_ == nullptr) {
        HILOG_ERROR("StopWatcher get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri uri_(uri.ToString());
    int ret = extension_->StopWatcher(uri_);
    return ret;
}

int FileAccessExtStubImpl::MoveItem(const Urie &sourceFile, const Urie &targetParent, std::vector<Result> &moveResult,
                                    bool force)
{
    UserAccessTracer trace;
    trace.Start("MoveItem");
    if (extension_ == nullptr) {
        HILOG_ERROR("Move get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    Uri sourceFile_(sourceFile.ToString());
    Uri targetParent_(targetParent.ToString());
    return extension_->MoveItem(sourceFile_, targetParent_, moveResult, force);
}

int FileAccessExtStubImpl::MoveFile(const Urie &sourceFile, const Urie &targetParent, const std::string &fileName,
                                    Urie &newFile)
{
    UserAccessTracer trace;
    trace.Start("MoveFile");
    if (extension_ == nullptr) {
        HILOG_ERROR("Move get extension failed.");
        return E_IPCS;
    }
    if (!CheckCallingPermission(FILE_ACCESS_MANAGER_PERMISSION)) {
        HILOG_ERROR("permission error");
        return E_PERMISSION;
    }

    std::string newFileName = const_cast<std::string&>(fileName);
    Uri sourceFile_(sourceFile.ToString());
    Uri targetParent_(targetParent.ToString());
    Uri newFile_("");
    int ret =  extension_->MoveFile(sourceFile_, targetParent_, newFileName, newFile_);
    newFile = Urie(newFile_.ToString());
    return ret;
}
} // namespace FileAccessFwk
} // namespace OHOS
