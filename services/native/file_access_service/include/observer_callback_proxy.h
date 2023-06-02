/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OBSERVER_CALLBACK_PROXY_H
#define OBSERVER_CALLBACK_PROXY_H

#include "iobserver_callback.h"
#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace FileAccessFwk {
class ObserverCallbackProxy : public IRemoteProxy<IFileAccessObserver> {
public:
    explicit ObserverCallbackProxy(const sptr<IRemoteObject>& remote) : IRemoteProxy<IFileAccessObserver>(remote) {}
    ~ObserverCallbackProxy() = default;
    void OnChange(NotifyMessage &notifyMessage) {} override;

private:
    static inline BrokerDelegator<ObserverCallbackProxy> delegator_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // OBSERVER_CALLBACK_PROXY_H