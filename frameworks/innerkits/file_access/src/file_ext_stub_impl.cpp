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

#include "file_ext_stub_impl.h"

#include "hilog_wrapper.h"

namespace OHOS {
namespace FileAccessFwk {
std::shared_ptr<FileExtAbility> FileExtStubImpl::GetOwner()
{
    HILOG_INFO("%{public}s begin.", __func__);
    HILOG_INFO("%{public}s end.", __func__);
    return extension_;
}

int FileExtStubImpl::OpenFile(const Uri &uri, int flags)
{
    HILOG_INFO("%{public}s begin.", __func__);
    int ret = -1;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("%{public}s end failed.", __func__);
        return ret;
    }
    ret = extension->OpenFile(uri, flags);
    HILOG_INFO("%{public}s end successfully, return fd:%{public}d", __func__, ret);
    return ret;
}

int FileExtStubImpl::CreateFile(const Uri &parentUri, const std::string &displayName,  Uri &newFileUri)
{
    HILOG_INFO("%{public}s begin.", __func__);
    int ret = -1;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("%{public}s end failed.", __func__);
        return ret;
    }
    ret = extension->CreateFile(parentUri, displayName, newFileUri);
    HILOG_INFO("%{public}s end successfully, return ret:%{public}d, %{public}s",
        __func__, ret, newFileUri.ToString().c_str());
    return ret;
}

int FileExtStubImpl::Mkdir(const Uri &parentUri, const std::string &displayName, Uri &newFileUri)
{
    HILOG_INFO("%{public}s begin.", __func__);
    int ret = -1;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("%{public}s end failed.", __func__);
        return ret;
    }
    ret = extension->Mkdir(parentUri, displayName, newFileUri);
    HILOG_INFO("%{public}s end successfully, return ret:%{public}d, %{public}s",
        __func__, ret, newFileUri.ToString().c_str());
    return ret;
}

int FileExtStubImpl::Delete(const Uri &sourceFileUri)
{
    HILOG_INFO("%{public}s begin.", __func__);
    int ret = -1;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("%{public}s end failed.", __func__);
        return ret;
    }
    ret = extension->Delete(sourceFileUri);
    HILOG_INFO("%{public}s end successfully, return fd:%{public}d", __func__, ret);
    return ret;
}

int FileExtStubImpl::Move(const Uri &sourceFileUri, const Uri &targetParentUri, Uri &newFileUri)
{
    int ret = -1;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("%{public}s end failed.", __func__);
        return ret;
    }
    ret = extension->Move(sourceFileUri, targetParentUri, newFileUri);
    HILOG_INFO("%{public}s end successfully, return ret:%{public}d, %{public}s",
        __func__, ret,newFileUri.ToString().c_str());
    return ret;
}

int FileExtStubImpl::Rename(const Uri &sourceFileUri, const std::string &displayName, Uri &newFileUri)
{
    HILOG_INFO("%{public}s begin.", __func__);
    int ret = -1;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("%{public}s end failed.", __func__);
        return ret;
    }
    ret = extension->Rename(sourceFileUri, displayName, newFileUri);
    HILOG_INFO("%{public}s end successfully, return ret:%{public}d, %{public}s",
        __func__, ret, newFileUri.ToString().c_str());
    return ret;
}

std::vector<FileInfo> FileExtStubImpl::ListFile(const Uri &sourceFileUri)
{
    HILOG_INFO("%{public}s begin.", __func__);
    std::vector<FileInfo> vec;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("%{public}s end failed.", __func__);
        return vec;
    }
    vec = extension->ListFile(sourceFileUri);
    HILOG_INFO("%{public}s end", __func__);
    return vec;
}

std::vector<DeviceInfo> FileExtStubImpl::GetRoots()
{
    HILOG_INFO("%{public}s begin.", __func__);
    std::vector<DeviceInfo> vec;
    auto extension = GetOwner();
    if (extension == nullptr) {
        HILOG_ERROR("%{public}s end failed.", __func__);
        return vec;
    }
    vec = extension->GetRoots();
    HILOG_INFO("%{public}s end.", __func__);
    return vec;
}
} // namespace FileAccessFwk
} // namespace OHOS
