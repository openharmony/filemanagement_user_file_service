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

#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"

namespace OHOS {
namespace FileAccessFwk {
std::shared_ptr<FileAccessExtAbility> FileAccessExtStubImpl::GetOwner()
{
    return extension_;
}

int FileAccessExtStubImpl::OpenFile(const Uri &uri, int flags)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "OpenFile");
    int ret = ERR_ERROR;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    ret = extension->OpenFile(uri, flags);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtStubImpl::CreateFile(const Uri &parent, const std::string &displayName,  Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CreateFile");
    int ret = ERR_ERROR;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    ret = extension->CreateFile(parent, displayName, newFile);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtStubImpl::Mkdir(const Uri &parent, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Mkdir");
    int ret = ERR_ERROR;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    ret = extension->Mkdir(parent, displayName, newFile);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtStubImpl::Delete(const Uri &sourceFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Delete");
    int ret = ERR_ERROR;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    ret = extension->Delete(sourceFile);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtStubImpl::Move(const Uri &sourceFile, const Uri &targetParent, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Move");
    int ret = ERR_ERROR;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    ret = extension->Move(sourceFile, targetParent, newFile);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtStubImpl::Rename(const Uri &sourceFile, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Rename");
    int ret = ERR_ERROR;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ret;
    }

    ret = extension->Rename(sourceFile, displayName, newFile);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

std::vector<FileInfo> FileAccessExtStubImpl::ListFile(const Uri &sourceFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "ListFile");
    std::vector<FileInfo> vec;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return vec;
    }

    vec = extension->ListFile(sourceFile);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return vec;
}

std::vector<DeviceInfo> FileAccessExtStubImpl::GetRoots()
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetRoots");
    std::vector<DeviceInfo> vec;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("get extension failed.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return vec;
    }

    vec = extension->GetRoots();
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return vec;
}
} // namespace FileAccessFwk
} // namespace OHOS