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
#include "fileaccessextconnection_fuzzer.h"

#include <string>
#include <memory>

#include "hilog_wrapper.h"
#include "file_access_ext_connection.h"

namespace OHOS {
using namespace std;
using namespace FileAccessFwk;

bool OnAbilityConnectDoneFuzzTest(sptr<FileAccessExtConnection> conn, const uint8_t *data, size_t size)
{
    AppExecFwk::ElementName element;
    std::string name(reinterpret_cast<const char*>(data), size);
    element.SetBundleName(name);
    sptr<IRemoteObject> remoteObject = nullptr;
    conn->OnAbilityConnectDone(element, remoteObject, 0);
    return true;
}

bool OnAbilityDisconnectDoneFuzzTest(sptr<FileAccessExtConnection> conn, const uint8_t *data, size_t size)
{
    AppExecFwk::ElementName element;
    std::string name(reinterpret_cast<const char*>(data), size);
    element.SetBundleName(name);
    conn->OnAbilityDisconnectDone(element, 0);
    return true;
}

bool IsExtAbilityConnectedFuzzTest(sptr<FileAccessExtConnection> conn)
{
    conn->IsExtAbilityConnected();
    return true;
}

bool GetFileExtProxyFuzzTest(sptr<FileAccessExtConnection> conn)
{
    conn->GetFileExtProxy();
    return true;
}

bool ConnectFileExtAbility(sptr<FileAccessExtConnection> conn, const uint8_t *data, size_t size)
{
    int len = size >> 1;
    AAFwk::Want want;
    want.SetElementName(std::string(reinterpret_cast<const char*>(data), len),
        std::string(reinterpret_cast<const char*>(data + len), size - len));
    sptr<IRemoteObject> remoteObject = nullptr;
    conn->ConnectFileExtAbility(want, remoteObject);
    return true;
}

bool DisconnectFileExtAbility(sptr<FileAccessExtConnection> conn)
{
    conn->DisconnectFileExtAbility();
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    auto conn = OHOS::sptr<OHOS::FileAccessFwk::FileAccessExtConnection>();
    if (conn == nullptr) {
        return 0;
    }

    OHOS::OnAbilityConnectDoneFuzzTest(conn, data, size);
    OHOS::OnAbilityDisconnectDoneFuzzTest(conn, data, size);
    OHOS::IsExtAbilityConnectedFuzzTest(conn);
    OHOS::GetFileExtProxyFuzzTest(conn);
    OHOS::ConnectFileExtAbility(conn, data, size);
    OHOS::DisconnectFileExtAbility(conn);

    return 0;
}
