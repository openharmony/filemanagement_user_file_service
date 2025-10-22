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
#include "fileinfosharedmemory_unmarshalling_fuzzer.h"

#include <cstring>
#include <memory>

#include "file_info_shared_memory.h"

namespace OHOS {
using namespace std;
using namespace FileAccessFwk;

template<class T>
T TypeCast(const uint8_t *data, int *pos = nullptr)
{
    if (pos) {
        *pos += sizeof(T);
    }
    return *(reinterpret_cast<const T*>(data));
}

bool UnmarshallingFuzzTest(shared_ptr<SharedMemoryInfo> info, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(uint64_t)) {
        return true;
    }

    MessageParcel reply;
    reply.WriteUint64(*reinterpret_cast<const uint64_t*>(data));
    info->Unmarshalling(reply);

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    auto sharedMemoryInfo = std::make_shared<OHOS::FileAccessFwk::SharedMemoryInfo>();
    if (sharedMemoryInfo == nullptr) {
        return 0;
    }

    OHOS::UnmarshallingFuzzTest(sharedMemoryInfo, data, size);

    return 0;
}
