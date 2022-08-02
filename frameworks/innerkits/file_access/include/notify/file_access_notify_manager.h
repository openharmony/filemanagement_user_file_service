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

#ifndef FILE_ACCESS_NOTIFY_MANAGER_H
#define FILE_ACCESS_NOTIFY_MANAGER_H

#include <memory>
#include <mutex>
#include <vector>

#include "event_handler.h"
#include "ifile_access_notify.h"
#include "iremote_object.h"

namespace OHOS {
namespace FileAccessFwk {
using EventHandler = OHOS::AppExecFwk::EventHandler;
class FileAccessNotifyManager final : public std::enable_shared_from_this<FileAccessNotifyManager> {
public:
    using NotifyMapType = std::map<sptr<IFileAccessNotify>, sptr<IRemoteObject::DeathRecipient>>;

    FileAccessNotifyManager();
    ~FileAccessNotifyManager();

    int RegisterNotify(sptr<IFileAccessNotify> &notify);
    int UnregisterNotify(sptr<IFileAccessNotify> &notify);
    int Notify(const NotifyMessage &message);

private:
    void OnCallBackDied(const wptr<IRemoteObject> &remote);
    bool AddNotifyToMap(const sptr<IFileAccessNotify> &notify);
    bool RemoveNotifyFromMap(const sptr<IRemoteObject> &remote);
    NotifyMapType notifyMap_;
    static std::mutex notifyMapMutex_;
};

class FileAccessNotifyCallbackRecipient : public IRemoteObject::DeathRecipient {
public:
    using RemoteDiedHandler = std::function<void(const wptr<IRemoteObject> &)>;
    FileAccessNotifyCallbackRecipient(RemoteDiedHandler handler);
    virtual ~FileAccessNotifyCallbackRecipient();
    virtual void OnRemoteDied(const wptr<IRemoteObject> &remote);

private:
    RemoteDiedHandler handler_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_NOTIFY_MANAGER_H