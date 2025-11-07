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
#include "fileaccessextconnectfileextability_fuzzer.h"

#include <string>
#include <memory>

#include "hilog_wrapper.h"
#include "accesstoken_kit.h"
#include "token_setproc.h"
#include "nativetoken_kit.h"
#include "file_access_ext_connection.h"
#include "user_file_service_token_mock.h"

namespace OHOS {
using namespace std;
using namespace FileAccessFwk;

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
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::UserFileServiceTokenMock tokenMock;
    tokenMock.SetFileManagerToken();
    auto conn = OHOS::sptr<OHOS::FileAccessFwk::FileAccessExtConnection>();
    if (conn == nullptr) {
        return 0;
    }

    OHOS::ConnectFileExtAbility(conn, data, size);
    return 0;
}
