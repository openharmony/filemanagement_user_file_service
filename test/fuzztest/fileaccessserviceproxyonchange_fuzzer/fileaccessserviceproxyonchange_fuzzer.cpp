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
#include "fileaccessserviceproxyonchange_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>

#include "extension_context.h"
#include "file_access_service_proxy.h"
#include "file_access_ext_ability.h"
#include "js_file_access_ext_ability.h"
#include "js_runtime.h"

namespace OHOS {
namespace FileAccessFwk{
using namespace std;
using namespace OHOS;
using namespace FileAccessFwk;
using namespace AbilityRuntime;
constexpr size_t FOO_MAX_LEN = 1024;
constexpr size_t U32_AT_SIZE = 4;

bool FileAccessServiceProxyOnChangeFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    std::string inputUri(reinterpret_cast<const char*>(data), size);
    Uri uri(inputUri);
    NotifyType notifyType = NotifyType::NOTIFY_ADD;
    sptr<FileAccessServiceProxy> fileAccessProxy = FileAccessServiceProxy::GetInstance();
    fileAccessProxy->OnChange(uri, notifyType);
    return true;
}
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    /* Validate the length of size */
    if (size < OHOS::FileAccessFwk::U32_AT_SIZE || size > OHOS::FileAccessFwk::FOO_MAX_LEN) {
        return 0;
    }
    OHOS::FileAccessFwk::FileAccessServiceProxyOnChangeFuzzTest(move(str), size);
    return 0;
}
