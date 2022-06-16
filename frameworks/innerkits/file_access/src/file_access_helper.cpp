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

#include "file_access_helper.h"

#include "hilog_wrapper.h"
#include "ifile_ext_base.h"

namespace OHOS {
namespace FileAccessFwk {
FileAccessHelper::FileAccessHelper(const std::shared_ptr<OHOS::AbilityRuntime::Context> &context,
    const AAFwk::Want &want, const sptr<IFileExtBase> &fileExtProxy)
{
    HILOG_INFO("%{public}s start", __func__);
    token_ = context->GetToken();
    want_ = want;
    fileExtProxy_ = fileExtProxy;
    fileExtConnection_ = FileExtConnection::GetInstance();
    HILOG_INFO("%{public}s end", __func__);
}

void FileAccessHelper::AddFileAccessDeathRecipient(const sptr<IRemoteObject> &token)
{
    HILOG_INFO("%{public}s called begin", __func__);
    if (token != nullptr && callerDeathRecipient_ != nullptr) {
        HILOG_INFO("token RemoveDeathRecipient.");
        token->RemoveDeathRecipient(callerDeathRecipient_);
    }
    if (callerDeathRecipient_ == nullptr) {
        callerDeathRecipient_ =
            new FileAccessDeathRecipient(std::bind(&FileAccessHelper::OnSchedulerDied, this, std::placeholders::_1));
    }
    if (token != nullptr) {
        HILOG_INFO("token AddDeathRecipient.");
        token->AddDeathRecipient(callerDeathRecipient_);
    }
    HILOG_INFO("%{public}s called end", __func__);
}

void FileAccessHelper::OnSchedulerDied(const wptr<IRemoteObject> &remote)
{
    HILOG_INFO("%{public}s called begin", __func__);
    auto object = remote.promote();
    object = nullptr;
    fileExtProxy_ = nullptr;
    HILOG_INFO("%{public}s called end", __func__);
}

std::shared_ptr<FileAccessHelper> FileAccessHelper::Creator(
    const std::shared_ptr<OHOS::AbilityRuntime::Context> &context, const AAFwk::Want &want)
{
    HILOG_INFO("%{public}s with runtime context, want and uri called start.", __func__);
    if (context == nullptr) {
        HILOG_ERROR("%{public}s failed, context == nullptr", __func__);
        return nullptr;
    }

    HILOG_INFO("%{public}s before ConnectFileExtAbility.", __func__);
    sptr<IFileExtBase> fileExtProxy = nullptr;

    sptr<FileExtConnection> fileExtConnection = FileExtConnection::GetInstance();
    if (!fileExtConnection->IsExtAbilityConnected()) {
        fileExtConnection->ConnectFileExtAbility(want, context->GetToken());
    }
    fileExtProxy = fileExtConnection->GetFileExtProxy();
    if (fileExtProxy == nullptr) {
        HILOG_WARN("%{public}s get invalid fileExtProxy", __func__);
    }
    HILOG_INFO("%{public}s after ConnectFileExtAbility.", __func__);

    FileAccessHelper *ptrFileAccessHelper = new (std::nothrow) FileAccessHelper(context, want, fileExtProxy);
    if (ptrFileAccessHelper == nullptr) {
        HILOG_ERROR("%{public}s failed, create FileAccessHelper failed", __func__);
        return nullptr;
    }

    HILOG_INFO("%{public}s with runtime context, want and uri called end.", __func__);
    return std::shared_ptr<FileAccessHelper>(ptrFileAccessHelper);
}

void FileAccessDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    HILOG_INFO("%{public}s begin.", __func__);
    if (handler_) {
        handler_(remote);
    }
    HILOG_INFO("%{public}s end.", __func__);
}

FileAccessDeathRecipient::FileAccessDeathRecipient(RemoteDiedHandler handler) : handler_(handler)
{
    HILOG_INFO("%{public}s .", __func__);
}

FileAccessDeathRecipient::~FileAccessDeathRecipient()
{
    HILOG_INFO("%{public}s .", __func__);
}
} // namespace FileAccessFwk
} // namespace OHOS