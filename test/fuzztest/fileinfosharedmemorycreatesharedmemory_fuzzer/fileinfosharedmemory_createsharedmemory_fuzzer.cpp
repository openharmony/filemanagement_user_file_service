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
#include "fileinfosharedmemory_createsharedmemory_fuzzer.h"

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

bool CreateSharedMemoryFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(uint64_t)) {
        return true;
    }

    int pos = 0;
    uint64_t memSize = TypeCast<uint64_t>(data, &pos);
    string memName(reinterpret_cast<const char*>(data + pos), size - pos);
    SharedMemoryInfo memInfo;
    SharedMemoryOperation::CreateSharedMemory(memName.c_str(), memSize, memInfo);
    SharedMemoryOperation::DestroySharedMemory(memInfo);

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

    OHOS::CreateSharedMemoryFuzzTest(data, size);

    return 0;
}
