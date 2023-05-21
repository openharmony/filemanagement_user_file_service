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

#ifndef OBSERVER_CALLBACK_STUB_H
#define OBSERVER_CALLBACK_STUB_H

#include <errors.h>
#include <cstdint>

#include "iobserver_callback.h"
#include "iremote_stub.h"
#include "message_option.h"
#include "message_parcel.h"
#include "refbase.h"

namespace OHOS {
namespace FileAccessFwk {
class ObserverCallbackStub : public IRemoteStub<IFileAccessObserver> {
public:
    ObserverCallbackStub() = default;
    virtual ~ObserverCallbackStub() = default;
    int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;
    void OnChange(NotifyMessage &notifyMessage) override {}

private:
    int32_t OnChangeStub(MessageParcel &data, MessageParcel &reply);
    int32_t ChooseCodeStub(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // OBSERVER_CALLBACK_STUB_H