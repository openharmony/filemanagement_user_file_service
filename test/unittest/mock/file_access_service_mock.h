/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef FILE_ACCESS_SERVICE_MOCK_H
#define FILE_ACCESS_SERVICE_MOCK_H

#include "file_access_service_stub.h"

namespace OHOS {
namespace FileAccessFwk {
using Uri = OHOS::Uri;
class FileAccessServiceMock : public FileAccessServiceStub {
public:
    MOCK_METHOD2(OnChange, int32_t(Uri uri, NotifyType notifyType));
    MOCK_METHOD4(RegisterNotify, int32_t(Uri uri, bool notifyForDescendants,
        const sptr<IFileAccessObserver> &observer, const std::shared_ptr<ConnectExtensionInfo> &info));
    MOCK_METHOD3(UnregisterNotify, int32_t(Uri uri, const sptr<IFileAccessObserver> &observer,
        const std::shared_ptr<ConnectExtensionInfo> &info));
    MOCK_METHOD2(GetExensionProxy, int32_t(const std::shared_ptr<ConnectExtensionInfo> &info,
        sptr<IFileAccessExtBase> &extensionProxy));
    MOCK_METHOD0(AsObject, sptr<IRemoteObject>());
    MOCK_METHOD2(CleanAllNotify, int32_t(Uri uri, const std::shared_ptr<ConnectExtensionInfo> &info));
    MOCK_METHOD4(OnRemoteRequest, int32_t(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option));
    MOCK_METHOD4(SendRequest, int(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option));
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_SERVICE_MOCK_H