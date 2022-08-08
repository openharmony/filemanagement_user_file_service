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

#include "file_access_notify_manager.h"

#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace FileAccessFwk {
std::mutex FileAccessNotifyManager::notifyMapMutex_;

FileAccessNotifyManager::FileAccessNotifyManager()
{
}

FileAccessNotifyManager::~FileAccessNotifyManager()
{
}

int FileAccessNotifyManager::RegisterNotify(sptr<IFileAccessNotify> &notify)
{
    std::lock_guard<std::mutex> lock(notifyMapMutex_);
    if (!AddNotifyToMap(notify)) {
        HILOG_ERROR("the remote obj is nullptr.");
        return ERR_INVALID_NOTIFY;
    }
    HILOG_INFO("FileAccessNotifyManager::RegisterNotify success.");
    return ERR_OK;
}

int FileAccessNotifyManager::UnregisterNotify(sptr<IFileAccessNotify> &notify)
{
    if (notify == nullptr || notify->AsObject() == nullptr) {
        HILOG_ERROR("the remote obj is nullptr.");
        return ERR_INVALID_NOTIFY;
    }

    std::lock_guard<std::mutex> lock(notifyMapMutex_);
    if (!RemoveNotifyFromMap(notify->AsObject())) {
        HILOG_ERROR("remove remote obj from map fail.");
        return ERR_REMOVE_NOTIFY_FAIL;
    }
    HILOG_INFO("FileAccessNotifyManager::UnregisterNotify success.");
    return ERR_OK;
}

int FileAccessNotifyManager::Notify(const NotifyMessage& message)
{
    std::lock_guard<std::mutex> lock(notifyMapMutex_);
    for (auto iter = notifyMap_.begin(); iter != notifyMap_.end(); iter++) {
        if (iter->first != nullptr) {
            iter->first->Notify(message);
        }
    }
    return ERR_OK;
}

bool FileAccessNotifyManager::AddNotifyToMap(const sptr<IFileAccessNotify> &notify)
{
    if (notify == nullptr || notify->AsObject() == nullptr) {
        HILOG_ERROR("the death notify obj is nullptr.");
        return false;
    }

    for (auto iter = notifyMap_.begin(); iter != notifyMap_.end(); iter++) {
        if (iter->first == notify) {
            iter->first->AsObject()->RemoveDeathRecipient(iter->second);
            notifyMap_.erase(iter);
        }
    }
    sptr<IRemoteObject::DeathRecipient> deathRecipient = new FileAccessNotifyCallbackRecipient(
        std::bind(&FileAccessNotifyManager::OnCallBackDied, this, std::placeholders::_1));
    notify->AsObject()->AddDeathRecipient(deathRecipient);
    notifyMap_.emplace(notify, deathRecipient);
    return true;
}

bool FileAccessNotifyManager::RemoveNotifyFromMap(const sptr<IRemoteObject> &remote)
{
    if (remote == nullptr) {
        HILOG_ERROR("the death remote obj is nullptr.");
        return false;
    }

    for (auto iter = notifyMap_.begin(); iter != notifyMap_.end(); iter++) {
        if (iter->first->AsObject() == remote) {
            iter->first->AsObject()->RemoveDeathRecipient(iter->second);
            notifyMap_.erase(iter);
            return true;
        }
    }
    HILOG_ERROR("find remote object fail.");
    return false;
}

void FileAccessNotifyManager::OnCallBackDied(const wptr<IRemoteObject> &remote)
{
    auto object = remote.promote();
    if (object == nullptr) {
        HILOG_INFO("the death remote obj is nullptr");
        return;
    }

    std::lock_guard<std::mutex> lock(notifyMapMutex_);
    if (!RemoveNotifyFromMap(object)) {
        HILOG_ERROR("remove remote obj from map fail.");
        return;
    }
}

FileAccessNotifyCallbackRecipient::FileAccessNotifyCallbackRecipient(RemoteDiedHandler handler) : handler_(handler)
{
}

FileAccessNotifyCallbackRecipient::~FileAccessNotifyCallbackRecipient()
{
}

void FileAccessNotifyCallbackRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    if (handler_ != nullptr) {
        handler_(remote);
    }
}
} // namespace FileAccessFwk
} // namespace OHOS