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
#include "fileaccessextdisconnection_fuzzer.h"

#include <string>
#include <memory>

#include "hilog_wrapper.h"
#include "file_access_ext_connection.h"

namespace OHOS {
using namespace std;
using namespace FileAccessFwk;

bool OnAbilityDisconnectDoneFuzzTest(sptr<FileAccessExtConnection> conn, const uint8_t *data, size_t size)
{
    AppExecFwk::ElementName element;
    std::string name(reinterpret_cast<const char*>(data), size);
    element.SetBundleName(name);
    conn->OnAbilityDisconnectDone(element, 0);
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

    OHOS::OnAbilityDisconnectDoneFuzzTest(conn, data, size);
    return 0;
}
