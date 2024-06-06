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

#ifndef FILE_ACCESS_EXT_CONNECTION_H
#define FILE_ACCESS_EXT_CONNECTION_H

#include <atomic>
#include <mutex>

#include "ability_connect_callback_stub.h"
#include "element_name.h"
#include "ifile_access_ext_base.h"
#include "iremote_object.h"
#include "refbase.h"
#include "want.h"

namespace OHOS {
namespace FileAccessFwk {
class FileAccessExtConnection : public AAFwk::AbilityConnectionStub {
public:
    FileAccessExtConnection() = default;
    virtual ~FileAccessExtConnection();

    void OnAbilityConnectDone(
        const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode) override;
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override;
    void ConnectFileExtAbility(const AAFwk::Want &want, const sptr<IRemoteObject> &token);
    void DisconnectFileExtAbility();
    bool IsExtAbilityConnected();
    void OnSchedulerDied(const wptr<IRemoteObject> &remote);
    sptr<IFileAccessExtBase> GetFileExtProxy();

private:
    void AddFileAccessDeathRecipient(const sptr<IRemoteObject> &token);
    void RemoveFileAccessDeathRecipient(const sptr<IRemoteObject> &token);
    struct ThreadLockInfo {
        std::condition_variable condition;
        std::mutex mutex;
        bool isReady = false;
    };
    ThreadLockInfo connectLockInfo_;

    static std::mutex mutex_;
    std::atomic<bool> isConnected_ = {false};
    sptr<IFileAccessExtBase> fileExtProxy_;
    std::mutex deathRecipientMutex_;
    sptr<IRemoteObject::DeathRecipient> callerDeathRecipient_ = nullptr;
};

class FileAccessDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    using RemoteDiedHandler = std::function<void(const wptr<IRemoteObject> &)>;
    explicit FileAccessDeathRecipient(RemoteDiedHandler handler);
    virtual ~FileAccessDeathRecipient();
    virtual void OnRemoteDied(const wptr<IRemoteObject> &remote);

private:
    RemoteDiedHandler handler_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_EXT_CONNECTION_H
