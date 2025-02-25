/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef FILE_ACCESS_SERVICE_CLIENT_H
#define FILE_ACCESS_SERVICE_CLIENT_H

#include "ifile_access_service_base.h"
#include "iremote_proxy.h"
#include "system_ability_load_callback_stub.h"
#include "file_access_service_base_proxy.h"

namespace OHOS {
namespace FileAccessFwk {
class FileAccessServiceClient  {
public:

    ~FileAccessServiceClient() = default;
    static sptr<IFileAccessServiceBase> GetInstance();
    static void InvaildInstance();
    
    class ServiceProxyLoadCallback : public SystemAbilityLoadCallbackStub {
    public:
        void OnLoadSystemAbilitySuccess(int32_t systemAbilityId, const sptr<IRemoteObject> &remoteObject) override;
        void OnLoadSystemAbilityFail(int32_t systemAbilityId) override;
        std::condition_variable proxyConVar_;
        std::atomic<bool> isLoadSuccess_{false};
    };

private:
    class ProxyDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit ProxyDeathRecipient(std::function<void(const wptr<IRemoteObject> &)> functor) : functor_(functor) {};
        virtual ~ProxyDeathRecipient() = default;
    public:
        void OnRemoteDied(const wptr<IRemoteObject> &object) override
        {
            auto ptr = object.promote();
            if (!ptr) {
                HILOG_ERROR("remote object is nullptr");
                return;
            }
            ptr->RemoveDeathRecipient(this);
            functor_(ptr);
        };
    private:
        std::function<void(const wptr<IRemoteObject> &)> functor_;
    };
    int32_t UnregisterNotifyInternal(MessageParcel &data);
    static inline std::mutex proxyMutex_;
    static inline sptr<IFileAccessServiceBase> serviceProxy_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_SERVICE_CLIENT_H
