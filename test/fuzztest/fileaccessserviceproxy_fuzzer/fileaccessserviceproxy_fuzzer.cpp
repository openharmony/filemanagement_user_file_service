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
#include "fileaccessserviceproxy_fuzzer.h"

#include <cstring>
#include <memory>

#include "file_access_service_proxy.h"
#include "fileaccessservicemock.h"
#include "iservice_registry.h"
#include "refbase.h"

namespace OHOS {
using namespace std;
using namespace FileAccessFwk;

SystemAbilityManagerClient& SystemAbilityManagerClient::GetInstance()
{
    static auto instance = std::make_shared<SystemAbilityManagerClient>();
    return *instance;
}

sptr<ISystemAbilityManager> SystemAbilityManagerClient::GetSystemAbilityManager()
{
    return nullptr;
}

void SystemAbilityManagerClient::DestroySystemAbilityManagerObject()
{}

template<class T>
T TypeCast(const uint8_t *data, int *pos = nullptr)
{
    if (pos) {
        *pos += sizeof(T);
    }
    return *(reinterpret_cast<const T*>(data));
}

bool OnChangeFuzzTest(shared_ptr<FileAccessServiceProxy> proxy, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(NotifyType)) {
        return true;
    }

    int pos = 0;
    NotifyType notifyType = TypeCast<NotifyType>(data, &pos);
    Uri uri(string(reinterpret_cast<const char*>(data + pos), size - pos));

    proxy->OnChange(uri, notifyType);
    return true;
}

bool RegisterNotifyFuzzTest(shared_ptr<FileAccessServiceProxy> proxy, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(bool)) {
        return true;
    }

    int pos = 0;
    auto info = make_shared<ConnectExtensionInfo>();
    bool notifyForDescendants = TypeCast<bool>(data, &pos);
    Uri uri(string(reinterpret_cast<const char*>(data + pos), size - pos));

    proxy->RegisterNotify(uri, notifyForDescendants, nullptr, info);
    return true;
}

bool UnregisterNotifyFuzzTest(shared_ptr<FileAccessServiceProxy> proxy, const uint8_t *data, size_t size)
{
    auto info = make_shared<ConnectExtensionInfo>();
    Uri uri(string(reinterpret_cast<const char*>(data), size));

    proxy->UnregisterNotify(uri, nullptr, info);
    return true;
}

bool GetExensionProxyFuzzTest(shared_ptr<FileAccessServiceProxy> proxy, const uint8_t *data, size_t size)
{
    int len = size >> 1;
    AAFwk::Want want;
    want.SetElementName(std::string(reinterpret_cast<const char*>(data), len),
        std::string(reinterpret_cast<const char*>(data + len), size - len));
    auto info = make_shared<ConnectExtensionInfo>(want, nullptr);
    sptr<IFileAccessExtBase> extensionProxy = nullptr;

    proxy->GetExtensionProxy(info, extensionProxy);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    auto impl = std::make_unique<OHOS::FileAccessFwk::FileAccessServiceMock>();
    auto proxy = std::make_shared<OHOS::FileAccessFwk::FileAccessServiceProxy>(OHOS::sptr(impl.release()));
    if (proxy == nullptr || impl == nullptr) {
        return 0;
    }

    OHOS::OnChangeFuzzTest(proxy, data, size);
    OHOS::RegisterNotifyFuzzTest(proxy, data, size);
    OHOS::UnregisterNotifyFuzzTest(proxy, data, size);
    OHOS::GetExensionProxyFuzzTest(proxy, data, size);

    return 0;
}