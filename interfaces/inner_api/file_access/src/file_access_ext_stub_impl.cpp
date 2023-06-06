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

#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"

namespace OHOS {
namespace FileAccessFwk {
std::shared_ptr<FileAccessExtAbility> FileAccessExtStubImpl::GetOwner()
{
    return extension_;
}

int FileAccessExtStubImpl::OpenFile(const Uri &uri, const int flags, int &fd)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "OpenFile");
    if (extension_ == nullptr) {
        HILOG_ERROR("OpenFile get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = extension_->OpenFile(uri, flags, fd);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtStubImpl::CreateFile(const Uri &parent, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CreateFile");
    if (extension_ == nullptr) {
        HILOG_ERROR("CreateFile get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = extension_->CreateFile(parent, displayName, newFile);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtStubImpl::Mkdir(const Uri &parent, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Mkdir");
    if (extension_ == nullptr) {
        HILOG_ERROR("Mkdir get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = extension_->Mkdir(parent, displayName, newFile);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtStubImpl::Delete(const Uri &sourceFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Delete");
    if (extension_ == nullptr) {
        HILOG_ERROR("Delete get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = extension_->Delete(sourceFile);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtStubImpl::Move(const Uri &sourceFile, const Uri &targetParent, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Move");
    if (extension_ == nullptr) {
        HILOG_ERROR("Move get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = extension_->Move(sourceFile, targetParent, newFile);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtStubImpl::Copy(const Uri &sourceUri, const Uri &destUri, std::vector<CopyResult> &copyResult,
    bool force)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Copy");
    if (extension_ == nullptr) {
        HILOG_ERROR("Copy get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }
    int ret = extension_->Copy(sourceUri, destUri, copyResult, force);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtStubImpl::Rename(const Uri &sourceFile, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Rename");
    if (extension_ == nullptr) {
        HILOG_ERROR("Rename get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = extension_->Rename(sourceFile, displayName, newFile);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtStubImpl::ListFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
    const FileFilter &filter, std::vector<FileInfo> &fileInfoVec)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "ListFile");
    if (extension_ == nullptr) {
        HILOG_ERROR("ListFile get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = extension_->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtStubImpl::ScanFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
    const FileFilter &filter, std::vector<FileInfo> &fileInfoVec)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "ScanFile");
    if (extension_ == nullptr) {
        HILOG_ERROR("ScanFile get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = extension_->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtStubImpl::Query(const Uri &uri, std::vector<std::string> &columns, std::vector<std::string> &results)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Query");
    if (extension_ == nullptr) {
        HILOG_ERROR("Query get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = extension_->Query(uri, columns, results);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtStubImpl::GetRoots(std::vector<RootInfo> &rootInfoVec)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetRoots");
    if (extension_ == nullptr) {
        HILOG_ERROR("GetRoots get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = extension_->GetRoots(rootInfoVec);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtStubImpl::GetThumbnail(const Uri &uri, const ThumbnailSize &thumbnailSize,
    std::shared_ptr<PixelMap> &pixelMap)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetThumbnail");
    if (extension_ == nullptr) {
        HILOG_ERROR("GetThumbnail get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }
    std::unique_ptr<PixelMap> tempPtr;
    Size size{ thumbnailSize.width, thumbnailSize.height };
    int ret = extension_->GetThumbnail(uri, size, tempPtr);
    if (ret != ERR_OK) {
        HILOG_ERROR("GetThumbnail failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }
    pixelMap = std::move(tempPtr);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtStubImpl::GetFileInfoFromUri(const Uri &selectFile, FileInfo &fileInfo)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetFileInfoFromUri");
    if (extension_ == nullptr) {
        HILOG_ERROR("GetFileInfoFromUri get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = extension_->GetFileInfoFromUri(selectFile, fileInfo);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtStubImpl::GetFileInfoFromRelativePath(const std::string &selectFile, FileInfo &fileInfo)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetFileInfoFromRelativePath");
    if (extension_ == nullptr) {
        HILOG_ERROR("GetFileInfoFromRelativePath get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = extension_->GetFileInfoFromRelativePath(selectFile, fileInfo);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtStubImpl::Access(const Uri &uri, bool &isExist)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Access");
    if (extension_ == nullptr) {
        HILOG_ERROR("Access get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = extension_->Access(uri, isExist);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtStubImpl::StartWatcher(const Uri &uri)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "StartWatcher");
    if (extension_ == nullptr) {
        HILOG_ERROR("StartWatcher get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = extension_->StartWatcher(uri);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtStubImpl::StopWatcher(const Uri &uri)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "StopWatcher");
    if (extension_ == nullptr) {
        HILOG_ERROR("StopWatcher get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_IPCS;
    }

    int ret = extension_->StopWatcher(uri);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}
} // namespace FileAccessFwk
} // namespace OHOS