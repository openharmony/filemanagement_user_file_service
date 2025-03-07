/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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
#include "file_access_ext_base_proxy.h"
#include "file_access_service.h"
#include "iremote_object.h"
#include "refbase.h"
#include "want.h"

namespace OHOS {
namespace FileAccessFwk {
class FileAccessService;

class FileAccessExtConnection : public AAFwk::AbilityConnectionStub {
public:
    FileAccessExtConnection() = default;
    virtual ~FileAccessExtConnection() = default;

    void OnAbilityConnectDone(
        const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode) override;
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override;
    void ConnectFileExtAbility(const AAFwk::Want &want, const sptr<IRemoteObject> &token);
    void DisconnectFileExtAbility();
    bool IsExtAbilityConnected();
    sptr<IFileAccessExtBase> GetFileExtProxy();

private:
    struct ThreadLockInfo {
        std::condition_variable condition;
        std::mutex mutex;
        bool isReady = false;
    };
    ThreadLockInfo connectLockInfo_;

    static std::mutex mutex_;
    std::atomic<bool> isConnected_ = {false};
    sptr<IFileAccessExtBase> fileExtProxy_;
};


class AgentFileAccessExtConnection : public AAFwk::AbilityConnectionStub {
public:
    AgentFileAccessExtConnection(const sptr<AAFwk::IAbilityConnection>& connection) : connection_(connection){};
    virtual ~AgentFileAccessExtConnection();
    void OnAbilityConnectDone(
        const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode) override;
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override;
    void ConnectFileExtAbility(const AAFwk::Want &want);
    void DisconnectFileExtAbility();
private:
    const sptr<AAFwk::IAbilityConnection> connection_ = nullptr;
};

} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_EXT_CONNECTION_H
