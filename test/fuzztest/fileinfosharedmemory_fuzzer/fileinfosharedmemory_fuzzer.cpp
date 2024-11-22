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
#include "fileinfosharedmemory_fuzzer.h"

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

bool MarshallingFuzzTest(shared_ptr<SharedMemoryInfo> info, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int) + sizeof(uint64_t)) {
        return true;
    }

    int pos = 0;
    MessageParcel reply;
    info->memFd = TypeCast<int>(data, &pos);
    info->memSize = TypeCast<uint64_t>(data + pos);
    info->Marshalling(reply);

    return true;
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

bool ExpandSharedMemoryFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(uint64_t)) {
        return true;
    }

    int pos = 0;
    uint64_t memSize = TypeCast<uint64_t>(data, &pos);
    string memName(reinterpret_cast<const char*>(data + pos), size - pos);
    SharedMemoryInfo memInfo;
    SharedMemoryOperation::CreateSharedMemory(memName.c_str(), memSize, memInfo);
    SharedMemoryOperation::ExpandSharedMemory(memInfo);
    SharedMemoryOperation::DestroySharedMemory(memInfo);

    return true;
}

bool WriteFileInfosFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t) + sizeof(int64_t) + sizeof(int64_t)) {
        return true;
    }

    int pos = 0;
    int32_t mode = TypeCast<int32_t>(data, &pos);
    int64_t sz = TypeCast<int64_t>(data + pos, &pos);
    int64_t mtime = TypeCast<int64_t>(data + pos, &pos);
    int len = (size - pos) >> 2;
    std::string uri(*reinterpret_cast<const char*>(data + pos), len);
    std::string relativePath(*reinterpret_cast<const char*>(data + pos + len), len);
    std::string fileName(*reinterpret_cast<const char*>(data + pos + len + len), len);
    std::string mimeType(*reinterpret_cast<const char*>(data + pos + len + len + len), len);

    FileInfo info(uri, relativePath, fileName, mode, mimeType);
    info.size = sz;
    info.mtime = mtime;
    vector<FileInfo> fileInfoVec;
    fileInfoVec.emplace_back(info);

    SharedMemoryInfo memInfo;
    SharedMemoryOperation::WriteFileInfos(fileInfoVec, memInfo);
    SharedMemoryOperation::ReadFileInfo(info, memInfo);
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

    OHOS::MarshallingFuzzTest(sharedMemoryInfo, data, size);
    OHOS::UnmarshallingFuzzTest(sharedMemoryInfo, data, size);

    OHOS::CreateSharedMemoryFuzzTest(data, size);
    OHOS::ExpandSharedMemoryFuzzTest(data, size);
    OHOS::WriteFileInfosFuzzTest(data, size);

    return 0;
}
