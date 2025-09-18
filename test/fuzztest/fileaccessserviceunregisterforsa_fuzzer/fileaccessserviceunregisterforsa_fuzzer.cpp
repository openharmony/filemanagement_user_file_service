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

#include "fileaccessserviceunregisterforsa_fuzzer.h"
#include <string>
#include <vector>
#include <securec.h>
#include "file_access_service_client.h"
#include "user_file_service_token_mock.h"

namespace OHOS {
using namespace std;
using namespace FileAccessFwk;

bool DoSomethingUnregisterForSaFuzzTest(sptr<IFileAccessServiceBase> proxy, const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return true;
    }

    std::string path(reinterpret_cast<const char*>(data), size);

    proxy->UnregisterForSa(path);

    return true;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::UserFileServiceTokenMock tokenMock;
    tokenMock.SetFileManagerToken();
    auto proxy = OHOS::FileAccessFwk::FileAccessServiceClient::GetInstance();
    if (proxy == nullptr) {
        printf("service proxy is nullptr\n");
        return 0;
    }

    OHOS::DoSomethingUnregisterForSaFuzzTest(proxy, data, size);
    return 0;
}
}