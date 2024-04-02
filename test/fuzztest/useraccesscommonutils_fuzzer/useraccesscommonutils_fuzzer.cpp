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
#include "useraccesscommonutils_fuzzer.h"

#include <cstring>

#include "hilog_wrapper.h"
#include "user_access_common_utils.h"

namespace OHOS {
using namespace std;
using namespace FileAccessFwk;

bool GetDeviceTypeFuzzTest(const uint8_t *data, size_t size)
{
    string deviceType;
    GetDeviceType(deviceType);
    return true;
}

bool GetUserNameFuzzTest(const uint8_t *data, size_t size)
{
    string userName;
    GetUserName(userName);
    return true;
}

} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::GetDeviceTypeFuzzTest(data, size);
    OHOS::GetUserNameFuzzTest(data, size);

    return 0;
}
