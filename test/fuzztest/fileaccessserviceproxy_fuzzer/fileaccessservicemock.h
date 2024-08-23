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
class FileAccessServiceMock final : public FileAccessServiceStub {
public:
    virtual ~FileAccessServiceMock() = default;

    int32_t OnChange(Uri uri, NotifyType notifyType) override { return 0; }
    int32_t RegisterNotify(Uri uri, bool notifyForDescendants, const sptr<IFileAccessObserver> &observer,
        const std::shared_ptr<ConnectExtensionInfo> &info) override { return 0; }
    int32_t UnregisterNotify(Uri uri, const sptr<IFileAccessObserver> &observer,
        const std::shared_ptr<ConnectExtensionInfo> &info) override { return 0; }
    int32_t GetExtensionProxy(const std::shared_ptr<ConnectExtensionInfo> &info,
        sptr<IFileAccessExtBase> &extensionProxy) override { return 0; }
    int32_t CleanAllNotify(Uri uri, const std::shared_ptr<ConnectExtensionInfo> &info) override { return 0; }
    int32_t ConnectFileExtAbility(const AAFwk::Want &want,
        const sptr<AAFwk::IAbilityConnection>& connection) override { return 0; };
    int32_t DisConnectFileExtAbility(const sptr<AAFwk::IAbilityConnection>& connection) override { return 0; };
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_SERVICE_MOCK_H