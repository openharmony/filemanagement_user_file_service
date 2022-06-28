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

#include "hilog_wrapper.h"
#include "file_access_framework_errno.h"

namespace OHOS {
namespace FileAccessFwk {
std::shared_ptr<FileAccessExtAbility> FileAccessExtStubImpl::GetOwner()
{
    return extension_;
}

int FileAccessExtStubImpl::OpenFile(const Uri &uri, int flags)
{
    int ret = ERR_ERROR;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("get extension failed.");
        return ret;
    }
    ret = extension->OpenFile(uri, flags);
    return ret;
}

int FileAccessExtStubImpl::CreateFile(const Uri &parent, const std::string &displayName,  Uri &newFile)
{
    int ret = ERR_ERROR;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("get extension failed.");
        return ret;
    }
    ret = extension->CreateFile(parent, displayName, newFile);
    return ret;
}

int FileAccessExtStubImpl::Mkdir(const Uri &parent, const std::string &displayName, Uri &newFile)
{
    int ret = ERR_ERROR;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("get extension failed.");
        return ret;
    }
    ret = extension->Mkdir(parent, displayName, newFile);
    return ret;
}

int FileAccessExtStubImpl::Delete(const Uri &sourceFile)
{
    int ret = ERR_ERROR;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("get extension failed.");
        return ret;
    }
    ret = extension->Delete(sourceFile);
    return ret;
}

int FileAccessExtStubImpl::Move(const Uri &sourceFile, const Uri &targetParent, Uri &newFile)
{
    int ret = ERR_ERROR;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("get extension failed.");
        return ret;
    }
    ret = extension->Move(sourceFile, targetParent, newFile);
    return ret;
}

int FileAccessExtStubImpl::Rename(const Uri &sourceFile, const std::string &displayName, Uri &newFile)
{
    int ret = ERR_ERROR;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("get extension failed.");
        return ret;
    }
    ret = extension->Rename(sourceFile, displayName, newFile);
    return ret;
}

std::vector<FileInfo> FileAccessExtStubImpl::ListFile(const Uri &sourceFile)
{
    std::vector<FileInfo> vec;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("get extension failed.");
        return vec;
    }

    vec = extension->ListFile(sourceFile);
    return vec;
}

std::vector<DeviceInfo> FileAccessExtStubImpl::GetRoots()
{
    std::vector<DeviceInfo> vec;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("get extension failed.");
        return vec;
    }

    vec = extension->GetRoots();
    return vec;
}
} // namespace FileAccessFwk
} // namespace OHOS