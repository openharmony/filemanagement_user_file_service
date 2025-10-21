/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "fileaccessserviceunregisternotifynoobserver_fuzzer.h"

#include <string>

#include "app_file_access_ext_connection.h"
#include "file_access_observer_stub.h"
#include "file_access_service_client.h"

#include "accesstoken_kit.h"
#include "file_access_helper.h"
#include "iservice_registry.h"
#include "token_setproc.h"
#include "nativetoken_kit.h"
#include "user_file_service_token_mock.h"

namespace OHOS {
using namespace std;
using namespace FileAccessFwk;

shared_ptr<FileAccessHelper> g_fah = nullptr;

bool UnregisterNotifyNoObserverFuzzTest(sptr<IFileAccessServiceBase> proxy, const uint8_t* data, size_t size)
{
    if (data == nullptr || size < sizeof(bool) || g_fah == nullptr) {
        return true;
    }

    int pos = sizeof(bool);
    Uri uri(string(reinterpret_cast<const char *>(data + pos), size - pos));
    AAFwk::Want want;
    want.SetElementName(EXTERNAL_BNUDLE_NAME, "FileExtensionAbility");
    sptr<ConnectExtensionInfo> info = sptr(new (std::nothrow) ConnectExtensionInfo(want, g_fah->token_));
    if (info == nullptr) {
        printf("ConnectExtensionInfo is nullptr");
        return false;
    }
    proxy->UnregisterNotifyNoObserver(uri, *info);
    return true;
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::UserFileServiceTokenMock tokenMock;
    tokenMock.SetFileManagerToken();
    auto proxy = OHOS::FileAccessFwk::FileAccessServiceClient::GetInstance();
    if (proxy == nullptr) {
        printf("service proxy is nullptr");
        return 0;
    }
    /* Run your code on data */
    OHOS::UnregisterNotifyNoObserverFuzzTest(proxy, data, size);
    return 0;
}
