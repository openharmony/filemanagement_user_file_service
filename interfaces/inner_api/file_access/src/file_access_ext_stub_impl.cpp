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

#include "file_access_ext_stub_impl.h"

#include <cinttypes>
#include "user_access_tracer.h"
#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"

namespace OHOS {
namespace FileAccessFwk {
const int64_t FILEFILTER_DEFAULT_COUNTS = 2000;
const int64_t FILEFILTER_MAX_COUNTS = 20000;
std::shared_ptr<FileAccessExtAbility> FileAccessExtStubImpl::GetOwner()
{
    return extension_;
}

int FileAccessExtStubImpl::OpenFile(const Uri &uri, const int flags, int &fd)
{
    UserAccessTracer trace;
    trace.Start("OpenFile");
    if (extension_ == nullptr) {
        HILOG_ERROR("OpenFile get extension failed.");
        return E_IPCS;
    }

    int ret = extension_->OpenFile(uri, flags, fd);
    return ret;
}

int FileAccessExtStubImpl::CreateFile(const Uri &parent, const std::string &displayName, Uri &newFile)
{
    UserAccessTracer trace;
    trace.Start("CreateFile");
    if (extension_ == nullptr) {
        HILOG_ERROR("CreateFile get extension failed.");
        return E_IPCS;
    }

    int ret = extension_->CreateFile(parent, displayName, newFile);
    return ret;
}

int FileAccessExtStubImpl::Mkdir(const Uri &parent, const std::string &displayName, Uri &newFile)
{
    UserAccessTracer trace;
    trace.Start("Mkdir");
    if (extension_ == nullptr) {
        HILOG_ERROR("Mkdir get extension failed.");
        return E_IPCS;
    }

    int ret = extension_->Mkdir(parent, displayName, newFile);
    return ret;
}

int FileAccessExtStubImpl::Delete(const Uri &sourceFile)
{
    UserAccessTracer trace;
    trace.Start("Delete");
    if (extension_ == nullptr) {
        HILOG_ERROR("Delete get extension failed.");
        return E_IPCS;
    }

    int ret = extension_->Delete(sourceFile);
    return ret;
}

int FileAccessExtStubImpl::Move(const Uri &sourceFile, const Uri &targetParent, Uri &newFile)
{
    UserAccessTracer trace;
    trace.Start("Move");
    if (extension_ == nullptr) {
        HILOG_ERROR("Move get extension failed.");
        return E_IPCS;
    }

    int ret = extension_->Move(sourceFile, targetParent, newFile);
    return ret;
}

int FileAccessExtStubImpl::Copy(const Uri &sourceUri, const Uri &destUri, std::vector<Result> &copyResult,
    bool force)
{
    UserAccessTracer trace;
    trace.Start("Copy");
    if (extension_ == nullptr) {
        HILOG_ERROR("Copy get extension failed.");
        return E_IPCS;
    }
    int ret = extension_->Copy(sourceUri, destUri, copyResult, force);
    return ret;
}

int FileAccessExtStubImpl::CopyFile(const Uri &sourceUri, const Uri &destUri, const std::string &fileName,
    Uri &newFileUri)
{
    UserAccessTracer trace;
    trace.Start("CopyFile");
    if (extension_ == nullptr) {
        HILOG_ERROR("Copy file get extension failed.");
        return E_IPCS;
    }
    int ret = extension_->CopyFile(sourceUri, destUri, fileName, newFileUri);
    return ret;
}

int FileAccessExtStubImpl::Rename(const Uri &sourceFile, const std::string &displayName, Uri &newFile)
{
    UserAccessTracer trace;
    trace.Start("Rename");
    if (extension_ == nullptr) {
        HILOG_ERROR("Rename get extension failed.");
        return E_IPCS;
    }

    int ret = extension_->Rename(sourceFile, displayName, newFile);
    return ret;
}

int FileAccessExtStubImpl::ListFile(const FileInfo &fileInfo, const int64_t offset, const FileFilter &filter,
        SharedMemoryInfo &memInfo)
{
    UserAccessTracer trace;
    trace.Start("ListFile");
    if (extension_ == nullptr) {
        HILOG_ERROR("ListFile get extension failed.");
        return E_IPCS;
    }

    std::vector<FileInfo> fileInfoVec;
    int ret = ERR_OK;
    memInfo.isOver = false;
    int64_t currentOffset = offset;
    while (true) {
        int64_t maxCounts =
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
        if (fileInfoVec.empty() ||(maxCounts > static_cast<int64_t>(fileInfoVec.size())
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

    int ret = extension_->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    return ret;
}

int FileAccessExtStubImpl::Query(const Uri &uri, std::vector<std::string> &columns, std::vector<std::string> &results)
{
    UserAccessTracer trace;
    trace.Start("Query");
    if (extension_ == nullptr) {
        HILOG_ERROR("Query get extension failed.");
        return E_IPCS;
    }

    int ret = extension_->Query(uri, columns, results);
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

    int ret = extension_->GetRoots(rootInfoVec);
    return ret;
}

int FileAccessExtStubImpl::GetFileInfoFromUri(const Uri &selectFile, FileInfo &fileInfo)
{
    UserAccessTracer trace;
    trace.Start("GetFileInfoFromUri");
    if (extension_ == nullptr) {
        HILOG_ERROR("GetFileInfoFromUri get extension failed.");
        return E_IPCS;
    }

    int ret = extension_->GetFileInfoFromUri(selectFile, fileInfo);
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

    int ret = extension_->GetFileInfoFromRelativePath(selectFile, fileInfo);
    return ret;
}

int FileAccessExtStubImpl::Access(const Uri &uri, bool &isExist)
{
    UserAccessTracer trace;
    trace.Start("Access");
    if (extension_ == nullptr) {
        HILOG_ERROR("Access get extension failed.");
        return E_IPCS;
    }

    int ret = extension_->Access(uri, isExist);
    return ret;
}

int FileAccessExtStubImpl::StartWatcher(const Uri &uri)
{
    UserAccessTracer trace;
    trace.Start("StartWatcher");
    if (extension_ == nullptr) {
        HILOG_ERROR("StartWatcher get extension failed.");
        return E_IPCS;
    }

    int ret = extension_->StartWatcher(uri);
    return ret;
}

int FileAccessExtStubImpl::StopWatcher(const Uri &uri, bool isUnregisterAll)
{
    UserAccessTracer trace;
    trace.Start("StopWatcher");
    if (extension_ == nullptr) {
        HILOG_ERROR("StopWatcher get extension failed.");
        return E_IPCS;
    }

    int ret = extension_->StopWatcher(uri, isUnregisterAll);
    return ret;
}

int FileAccessExtStubImpl::MoveItem(const Uri &sourceFile, const Uri &targetParent, std::vector<Result> &moveResult,
                                    bool force)
{
    UserAccessTracer trace;
    trace.Start("MoveItem");
    if (extension_ == nullptr) {
        HILOG_ERROR("Move get extension failed.");
        return E_IPCS;
    }

    return extension_->MoveItem(sourceFile, targetParent, moveResult, force);
}

int FileAccessExtStubImpl::MoveFile(const Uri &sourceFile, const Uri &targetParent, std::string &fileName, Uri &newFile)
{
    UserAccessTracer trace;
    trace.Start("MoveFile");
    if (extension_ == nullptr) {
        HILOG_ERROR("Move get extension failed.");
        return E_IPCS;
    }

    return extension_->MoveFile(sourceFile, targetParent, fileName, newFile);
}
} // namespace FileAccessFwk
} // namespace OHOS
