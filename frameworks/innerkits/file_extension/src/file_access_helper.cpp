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
namespace AppExecFwk {
namespace {
constexpr int INVALID_VALUE = -1;
}  // namespace

std::mutex FileAccessHelper::oplock_;

FileAccessHelper::FileAccessHelper(const std::shared_ptr<OHOS::AbilityRuntime::Context> &context,
    const AAFwk::Want &want, const sptr<IFileExtBase> &fileExtProxy)
{
    HILOG_INFO("tag dsa FileAccessHelper::FileAccessHelper start");
    token_ = context->GetToken();
    want_ = want;
    fileExtProxy_ = fileExtProxy;
    fileExtConnection_ = FileExtConnection::GetInstance();
    HILOG_INFO("tag dsa FileAccessHelper::FileAccessHelper end");
}

void FileAccessHelper::AddFileAccessDeathRecipient(const sptr<IRemoteObject> &token)
{
    HILOG_INFO("tag dsa %{public}s called begin", __func__);
    if (token != nullptr && callerDeathRecipient_ != nullptr) {
        HILOG_INFO("tag dsa token RemoveDeathRecipient.");
        token->RemoveDeathRecipient(callerDeathRecipient_);
    }
    if (callerDeathRecipient_ == nullptr) {
        callerDeathRecipient_ =
            new FileAccessDeathRecipient(std::bind(&FileAccessHelper::OnSchedulerDied, this, std::placeholders::_1));
    }
    if (token != nullptr) {
        HILOG_INFO("tag dsa token AddDeathRecipient.");
        token->AddDeathRecipient(callerDeathRecipient_);
    }
    HILOG_INFO("tag dsa %{public}s called end", __func__);
}

void FileAccessHelper::OnSchedulerDied(const wptr<IRemoteObject> &remote)
{
    HILOG_INFO("tag dsa %{public}s called begin", __func__);
    auto object = remote.promote();
    object = nullptr;
    fileExtProxy_ = nullptr;
    HILOG_INFO("tag dsa %{public}s called end", __func__);
}

std::shared_ptr<FileAccessHelper> FileAccessHelper::Creator(
    const std::shared_ptr<OHOS::AbilityRuntime::Context> &context, const AAFwk::Want &want)
{
    HILOG_INFO("tag dsa FileAccessHelper::Creator with runtime context, want and uri called start.");
    if (context == nullptr) {
        HILOG_ERROR("tag dsa ileAccessHelper::Creator failed, context == nullptr");
        return nullptr;
    }

    HILOG_INFO("tag dsa FileAccessHelper::Creator before ConnectFileExtAbility.");
    sptr<IFileExtBase> fileExtProxy = nullptr;

    sptr<FileExtConnection> fileExtConnection = FileExtConnection::GetInstance();
    if (!fileExtConnection->IsExtAbilityConnected()) {
        fileExtConnection->ConnectFileExtAbility(want, context->GetToken());
    }
    fileExtProxy = fileExtConnection->GetFileExtProxy();
    if (fileExtProxy == nullptr) {
        HILOG_WARN("tag dsa FileAccessHelper::Creator get invalid fileExtProxy");
    }
    HILOG_INFO("tag dsa FileAccessHelper::Creator after ConnectFileExtAbility.");

    FileAccessHelper *ptrDataShareHelper = new (std::nothrow) FileAccessHelper(context, want, fileExtProxy);
    if (ptrDataShareHelper == nullptr) {
        HILOG_ERROR("tag dsa FileAccessHelper::Creator failed, create FileAccessHelper failed");
        return nullptr;
    }

    HILOG_INFO("tag dsa FileAccessHelper::Creator with runtime context, want and uri called end.");
    return std::shared_ptr<FileAccessHelper>(ptrDataShareHelper);
}

bool FileAccessHelper::Release()
{
    HILOG_INFO("tag dsa %{public}s called begin", __func__);

    HILOG_INFO("tag dsa FileAccessHelper::Release before DisconnectFileExtAbility.");
    if (fileExtConnection_->IsExtAbilityConnected()) {
        fileExtConnection_->DisconnectFileExtAbility();
    }
    HILOG_INFO("tag dsa FileAccessHelper::Release after DisconnectFileExtAbility.");
    fileExtProxy_ = nullptr;
    HILOG_INFO("tag dsa %{public}s called end", __func__);
    return true;
}

int FileAccessHelper::OpenFile(Uri &uri, const std::string &mode)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    int fd = INVALID_VALUE;

    HILOG_INFO("tag dsa FileAccessHelper::OpenFile before ConnectFileExtAbility.");
    if (!fileExtConnection_->IsExtAbilityConnected()) {
        fileExtConnection_->ConnectFileExtAbility(want_, token_);
    }
    fileExtProxy_ = fileExtConnection_->GetFileExtProxy();
    HILOG_INFO("tag dsa FileAccessHelper::OpenFile after ConnectFileExtAbility.");
    if (isSystemCaller_ && fileExtProxy_) {
        AddFileAccessDeathRecipient(fileExtProxy_->AsObject());
    }

    if (fileExtProxy_ == nullptr) {
        HILOG_ERROR("tag dsa %{public}s failed with invalid fileExtProxy_", __func__);
        return fd;
    }

    HILOG_INFO("tag dsa FileAccessHelper::OpenFile before fileExtProxy_->OpenFile.");
    fd = fileExtProxy_->OpenFile(uri, mode);
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    return fd;
}

int FileAccessHelper::CreateFile(Uri &parentUri, const std::string &displayName, Uri &newFileUri)
{
    HILOG_INFO("tag dsa FileAccessHelper::CreateFile start.");
    int index = INVALID_VALUE;

    HILOG_INFO("tag dsa FileAccessHelper::CreateFile before ConnectDataShareExtAbility.");
    if (!fileExtConnection_->IsExtAbilityConnected()) {
        fileExtConnection_->ConnectFileExtAbility(want_, token_);
    }
    fileExtProxy_ = fileExtConnection_->GetFileExtProxy();
    HILOG_INFO("tag dsa FileAccessHelper::CreateFile after ConnectDataShareExtAbility.");
    if (isSystemCaller_ && fileExtProxy_) {
        AddFileAccessDeathRecipient(fileExtProxy_->AsObject());
    }

    if (fileExtProxy_ == nullptr) {
        HILOG_ERROR("tag dsa %{public}s failed with invalid fileExtProxy_", __func__);
        return index;
    }

    HILOG_INFO("tag dsa FileAccessHelper::CreateFile before fileExtProxy_->Mkdir.");
    index = fileExtProxy_->CreateFile(parentUri, displayName, newFileUri);
    HILOG_INFO("tag dsa FileAccessHelper::CreateFile end. index = %{public}d", index);
    HILOG_INFO("tag dsa FileAccessHelper::Mkdir end. newDirUri = %{public}s", newFileUri.ToString().c_str());
    return index;
}

int FileAccessHelper::Mkdir(Uri &parentUri, const std::string &displayName, Uri &newDirUri)
{
    HILOG_INFO("tag dsa FileAccessHelper::Mkdir start.");
    int index = INVALID_VALUE;

    HILOG_INFO("tag dsa FileAccessHelper::Mkdir before ConnectDataShareExtAbility.");
    if (!fileExtConnection_->IsExtAbilityConnected()) {
        fileExtConnection_->ConnectFileExtAbility(want_, token_);
    }
    fileExtProxy_ = fileExtConnection_->GetFileExtProxy();
    HILOG_INFO("tag dsa FileAccessHelper::Mkdir after ConnectDataShareExtAbility.");
    if (isSystemCaller_ && fileExtProxy_) {
        AddFileAccessDeathRecipient(fileExtProxy_->AsObject());
    }

    if (fileExtProxy_ == nullptr) {
        HILOG_ERROR("tag dsa %{public}s failed with invalid fileExtProxy_", __func__);
        return index;
    }

    HILOG_INFO("tag dsa FileAccessHelper::Mkdir before fileExtProxy_->Mkdir.");
    index = fileExtProxy_->Mkdir(parentUri, displayName, newDirUri);
    HILOG_INFO("tag dsa FileAccessHelper::Mkdir end. index = %{public}d", index);
    HILOG_INFO("tag dsa FileAccessHelper::Mkdir end. newDirUri = %{public}s", newDirUri.ToString().c_str());
    return index;
}

int FileAccessHelper::Delete(Uri &selectFileUri)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    int index = INVALID_VALUE;

    HILOG_INFO("tag dsa FileAccessHelper::Delete before ConnectFileExtAbility.");
    if (!fileExtConnection_->IsExtAbilityConnected()) {
        fileExtConnection_->ConnectFileExtAbility(want_, token_);
    }
    fileExtProxy_ = fileExtConnection_->GetFileExtProxy();
    HILOG_INFO("tag dsa FileAccessHelper::Delete after ConnectFileExtAbility.");
    if (isSystemCaller_ && fileExtProxy_) {
        AddFileAccessDeathRecipient(fileExtProxy_->AsObject());
    }

    if (fileExtProxy_ == nullptr) {
        HILOG_ERROR("tag dsa %{public}s failed with invalid fileExtProxy_", __func__);
        return index;
    }

    HILOG_INFO("tag dsa FileAccessHelper::OpenFile before fileExtProxy_->OpenFile.");
    index = fileExtProxy_->Delete(selectFileUri);
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    return index;
}

int FileAccessHelper::Move(Uri &sourceFileUri, Uri &targetParentUri, Uri &newFileUri)
{
    HILOG_INFO("tag dsa FileAccessHelper::Move start.");
    int index = INVALID_VALUE;

    HILOG_INFO("tag dsa FileAccessHelper::Move before ConnectDataShareExtAbility.");
    if (!fileExtConnection_->IsExtAbilityConnected()) {
        fileExtConnection_->ConnectFileExtAbility(want_, token_);
    }
    fileExtProxy_ = fileExtConnection_->GetFileExtProxy();
    HILOG_INFO("tag dsa FileAccessHelper::Move after ConnectDataShareExtAbility.");
    if (isSystemCaller_ && fileExtProxy_) {
        AddFileAccessDeathRecipient(fileExtProxy_->AsObject());
    }

    if (fileExtProxy_ == nullptr) {
        HILOG_ERROR("tag dsa %{public}s failed with invalid fileExtProxy_", __func__);
        return index;
    }

    HILOG_INFO("tag dsa FileAccessHelper::Move before fileExtProxy_->Move.");
    index = fileExtProxy_->Move(sourceFileUri, targetParentUri, newFileUri);
    HILOG_INFO("tag dsa FileAccessHelper::Move end. index = %{public}d", index);
    HILOG_INFO("tag dsa FileAccessHelper::Move end. newFileUri = %{public}s", newFileUri.ToString().c_str());
    return index;
}

int FileAccessHelper::Rename(Uri &sourceFileUri, const std::string &displayName, Uri &newFileUri)
{
    HILOG_INFO("tag dsa FileAccessHelper::Rename start.");
    int index = INVALID_VALUE;

    HILOG_INFO("tag dsa FileAccessHelper::Rename before ConnectDataShareExtAbility.");
    if (!fileExtConnection_->IsExtAbilityConnected()) {
        fileExtConnection_->ConnectFileExtAbility(want_, token_);
    }
    fileExtProxy_ = fileExtConnection_->GetFileExtProxy();
    HILOG_INFO("tag dsa FileAccessHelper::Rename after ConnectDataShareExtAbility.");
    if (isSystemCaller_ && fileExtProxy_) {
        AddFileAccessDeathRecipient(fileExtProxy_->AsObject());
    }

    if (fileExtProxy_ == nullptr) {
        HILOG_ERROR("tag dsa %{public}s failed with invalid fileExtProxy_", __func__);
        return index;
    }

    HILOG_INFO("tag dsa FileAccessHelper::Rename before fileExtProxy_->Rename.");
    index = fileExtProxy_->Rename(sourceFileUri, displayName, newFileUri);
    HILOG_INFO("tag dsa FileAccessHelper::Rename end. index = %{public}d", index);
    HILOG_INFO("tag dsa FileAccessHelper::Rename end. newFileUri = %{public}s", newFileUri.ToString().c_str());
    return index;
}

int FileAccessHelper::CloseFile(int fd, const std::string &uri)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    int index = INVALID_VALUE;

    HILOG_INFO("tag dsa FileAccessHelper::Delete before ConnectFileExtAbility.");
    if (!fileExtConnection_->IsExtAbilityConnected()) {
        fileExtConnection_->ConnectFileExtAbility(want_, token_);
    }
    fileExtProxy_ = fileExtConnection_->GetFileExtProxy();
    HILOG_INFO("tag dsa FileAccessHelper::Delete after ConnectFileExtAbility.");
    if (isSystemCaller_ && fileExtProxy_) {
        AddFileAccessDeathRecipient(fileExtProxy_->AsObject());
    }

    if (fileExtProxy_ == nullptr) {
        HILOG_ERROR("tag dsa %{public}s failed with invalid fileExtProxy_", __func__);
        return index;
    }

    HILOG_INFO("tag dsa FileAccessHelper::OpenFile before fileExtProxy_->OpenFile.");
    index = fileExtProxy_->CloseFile(fd, uri);
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    return index;
}

void FileAccessDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    if (handler_) {
        handler_(remote);
    }
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
}

FileAccessDeathRecipient::FileAccessDeathRecipient(RemoteDiedHandler handler) : handler_(handler)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
}

FileAccessDeathRecipient::~FileAccessDeathRecipient()
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
}
}  // namespace AppExecFwk
}  // namespace OHOS