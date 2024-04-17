
/*
 * Copyright (C) 2023 - 2024 Huawei Device Co., Ltd.
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

#ifndef _FILE_INFO_SHARED_MEMORY_H_
#define _FILE_INFO_SHARED_MEMORY_H_

#include <inttypes.h>
#include <stdint.h>
#include <string>
#include <sys/mman.h>
#include <unistd.h>
#include <utility>
#include <vector>

#include "ashmem.h"
#include "file_access_extension_info.h"
#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "ipc_file_descriptor.h"
#include "parcel.h"
#include "securec.h"

namespace OHOS {
namespace FileAccessFwk {
namespace {
    const uint32_t DEFAULT_CAPACITY_200KB = 200 * 1024;
    const uint32_t MAX_CAPACITY_2MB = 2 * 1024 * 1024;
} // namespace

class SharedMemoryInfo : public OHOS::Parcelable {
public:
    const char *memName {nullptr};
    int memFd {-1};
    uint8_t *memHead {nullptr};
    uint8_t *memTail {nullptr};
    uint64_t memSize {0};

    uint8_t *dataPtr {nullptr};
    uint64_t dataSize {0};
    uint32_t dataCounts = 0;

    bool isOver {false};
    uint32_t totalDataCounts = 0;
    uint32_t leftDataCounts = 0;

    SharedMemoryInfo() = default;
    SharedMemoryInfo(const SharedMemoryInfo &) = delete;
    SharedMemoryInfo &operator=(SharedMemoryInfo &) = delete;

    bool Marshalling(Parcel &parcel) const override
    {
        if (!WriteFileDescriptor(parcel, memFd)) {
            HILOG_ERROR("WriteFileDescriptor fail");
            return false;
        }
        if (!parcel.WriteUint64(memSize)) {
            return false;
        }
        return true;
    }

    static SharedMemoryInfo *Unmarshalling(Parcel &parcel)
    {
        auto *obj = new (std::nothrow) SharedMemoryInfo();
        if (obj != nullptr && !obj->ReadFromParcel(parcel)) {
            delete obj;
            obj = nullptr;
        }
        return obj;
    }

    bool ReadFromParcel(Parcel &parcel)
    {
        if (!ReadFileDescriptor(parcel)) {
            return false;
        }

        memSize = parcel.ReadUint64();
        return true;
    }

    bool Empty() const
    {
        return dataCounts == 0;
    }

    uint32_t Size() const
    {
        return dataCounts;
    }

    void Clear()
    {
        dataPtr = memHead;
        dataSize = 0;
        dataCounts = 0;
        isOver = false;
        totalDataCounts = 0;
        leftDataCounts = 0;
    }

private:
    bool WriteFileDescriptor(Parcel &parcel, int fd) const
    {
        if (fd < 0) {
            return false;
        }
        int dupFd = dup(fd);
        if (dupFd < 0) {
            return false;
        }
        sptr<IPCFileDescriptor> descriptor = new (std::nothrow)IPCFileDescriptor(dupFd);
        if (descriptor == nullptr) {
            HILOG_ERROR("create IPCFileDescriptor object failed");
            return false;
        }
        return parcel.WriteObject<IPCFileDescriptor>(descriptor);
    }

    bool ReadFileDescriptor(Parcel &parcel)
    {
        sptr<IPCFileDescriptor> descriptor = parcel.ReadObject<IPCFileDescriptor>();
        if (descriptor == nullptr) {
            return false;
        }
        int fd = descriptor->GetFd();
        if (fd < 0) {
            return false;
        }
        memFd = dup(fd);
        return true;
    }
};

class SharedMemoryOperation {
public:
    static int CreateSharedMemory(const char *memName, uint64_t memSize, SharedMemoryInfo &memInfo)
    {
        memInfo.memName = memName;
        memInfo.memSize = memSize;
        memInfo.memFd = AshmemCreate(memInfo.memName, memInfo.memSize);
        if (memInfo.memFd < 0) {
            HILOG_ERROR("Create shared memory error, shared memory name is %{public}s code: %{public}d",
                    memInfo.memName, memInfo.memFd);
            return memInfo.memFd;
        }

        int ret = AshmemSetProt(memInfo.memFd, PROT_READ | PROT_WRITE);
        if (ret < 0) {
            HILOG_ERROR("Set shared memory protection mask error, code: %{public}d", ret);
            ::close(memInfo.memFd);
            return ret;
        }

        return MapSharedMemory(memInfo);
    }

    static int ExpandSharedMemory(SharedMemoryInfo &memInfo)
    {
        uint64_t memSize = 0;
        auto alloc = CalculateMemSize(memSize, memInfo);
        if (alloc) {
            DestroySharedMemory(memInfo);
            int ret = CreateSharedMemory(memInfo.memName, memSize, memInfo);
            if (ret != ERR_OK) {
                HILOG_ERROR("Recreate shared memory error, code: %{public}d", ret);
                return ret;
            }
            HILOG_INFO("Recreate shared memory success, memory size: %{public}" PRIu64, memSize);
        }
        return ERR_OK;
    }

    static void DestroySharedMemory(SharedMemoryInfo &memInfo)
    {
        if (memInfo.memHead != nullptr) {
            ::munmap(memInfo.memHead, memInfo.memSize);
            ::close(memInfo.memFd);
            memInfo.memHead = nullptr;
        }
    }

    static int MapSharedMemory(SharedMemoryInfo &memInfo)
    {
        memInfo.memHead = reinterpret_cast<uint8_t *>(
            ::mmap(nullptr, memInfo.memSize, PROT_READ | PROT_WRITE, MAP_SHARED, memInfo.memFd, 0));
        if (memInfo.memHead == MAP_FAILED) {
            int ret = errno;
            ::close(memInfo.memFd);
            HILOG_ERROR("Shared memory map error, code: %{public}d", ret);
            return ret;
        }
        memInfo.memTail = memInfo.memHead + memInfo.memSize;
        memInfo.dataPtr = memInfo.memHead;
        memInfo.Clear();
        return ERR_OK;
    }

    static bool CalculateMemSize(uint64_t &memSize, SharedMemoryInfo &memInfo)
    {
        int64_t allocSize = ((memInfo.leftDataCounts + memInfo.totalDataCounts - 1) / memInfo.totalDataCounts + 1)
            * DEFAULT_CAPACITY_200KB;

        if (allocSize >= MAX_CAPACITY_2MB && memInfo.memSize < MAX_CAPACITY_2MB) {
            memSize = MAX_CAPACITY_2MB;
            HILOG_ERROR("memSize: %{public}" PRIu64, memSize);
            return true;
        }

        if (allocSize < MAX_CAPACITY_2MB && memInfo.memSize < MAX_CAPACITY_2MB && memInfo.memSize < allocSize) {
            memSize = allocSize;
            HILOG_ERROR("memSize: %{public}" PRIu64, memSize);
            return true;
        }
        return false;
    }

    static uint32_t WriteFileInfos(std::vector<FileInfo> &fileInfoVec, SharedMemoryInfo &memInfo)
    {
        uint32_t count = 0;
        for (auto &info : fileInfoVec) {
            int ret = WriteFileInfo(info, memInfo);
            if (!ret) {
                return count;
            }
            count++;
        }
        return count;
    }

    static bool WriteFileInfo(FileInfo &info, SharedMemoryInfo &memInfo)
    {
        uint64_t fileInfoSize = GetFileInfoSize(info);
        uint64_t leftSize = memInfo.memSize - memInfo.dataSize;
        if (leftSize < fileInfoSize) {
            HILOG_INFO("Shared memory cannot hold current data");
            return false;
        }

        uint8_t *dataPtr = memInfo.dataPtr;
        uint64_t dataSize = memInfo.dataSize;
        bool ret = WriteString(info.uri, memInfo);
        ret = ret && WriteString(info.relativePath, memInfo);
        ret = ret && WriteString(info.fileName, memInfo);
        ret = ret && WriteInt32(info.mode, memInfo);
        ret = ret && WriteInt64(info.size, memInfo);
        ret = ret && WriteInt64(info.mtime, memInfo);
        ret = ret && WriteString(info.mimeType, memInfo);
        if (!ret) {
            memInfo.dataPtr = dataPtr;
            memInfo.dataSize = dataSize;
            HILOG_ERROR("Write FileInfo into shared memmory failed");
            return false;
        }
        memInfo.dataCounts++;
        return true;
    }

    static bool ReadFileInfo(FileInfo &info, SharedMemoryInfo &memInfo)
    {
        if (memInfo.dataCounts == 0) {
            HILOG_INFO("No data in shared memory");
            return false;
        }
        uint8_t *dataPtr = memInfo.dataPtr;
        uint64_t dataSize = memInfo.dataSize;
        bool ret = ReadString(info.uri, memInfo);
        ret = ret && ReadString(info.relativePath, memInfo);
        ret = ret && ReadString(info.fileName, memInfo);
        ret = ret && ReadInt32(info.mode, memInfo);
        ret = ret && ReadInt64(info.size, memInfo);
        ret = ret && ReadInt64(info.mtime, memInfo);
        ret = ret && ReadString(info.mimeType, memInfo);
        if (!ret) {
            memInfo.dataPtr = dataPtr;
            memInfo.dataSize = dataSize;
            return false;
        }
        memInfo.dataCounts--;
        return true;
    }

private:
    template <typename T>
    static bool WriteInteger(T &integer, SharedMemoryInfo &memInfo)
    {
        uint8_t *src = reinterpret_cast<uint8_t *>(&integer);
        uint8_t *dest = memInfo.dataPtr;
        if (memcpy_s(dest, sizeof(T), src, sizeof(T)) != ERR_OK) {
            HILOG_ERROR("WriteInteger failed:%{public}s", std::strerror(errno));
            return false;
        }
        memInfo.dataPtr += sizeof(T);
        memInfo.dataSize += sizeof(T);
        return true;
    }

    template <typename T>
    static bool ReadInteger(T &integer, SharedMemoryInfo &memInfo)
    {
        uint8_t *src = memInfo.dataPtr;
        uint8_t *dest = reinterpret_cast<uint8_t *>(&integer);
        if (memcpy_s(dest, sizeof(T), src, sizeof(T)) != ERR_OK) {
            HILOG_ERROR("ReadInteger failed:%{public}s", std::strerror(errno));
            return false;
        }
        memInfo.dataPtr += sizeof(T);
        memInfo.dataSize -= sizeof(T);
        return true;
    }

    static bool WriteInt32(int32_t &value, SharedMemoryInfo &memInfo)
    {
        return WriteInteger<int32_t>(value, memInfo);
    }

    static bool WriteInt64(int64_t &value, SharedMemoryInfo &memInfo)
    {
        return WriteInteger<int64_t>(value, memInfo);
    }

    static bool ReadInt32(int32_t &value, SharedMemoryInfo &memInfo)
    {
        return ReadInteger<int32_t>(value, memInfo);
    }

    static bool ReadInt64(int64_t &value, SharedMemoryInfo &memInfo)
    {
        return ReadInteger<int64_t>(value, memInfo);
    }

    static bool WriteString(std::string &inVal, SharedMemoryInfo &memInfo)
    {
        std::string::size_type len = inVal.size();
        bool ret = WriteInteger<std::string::size_type>(len, memInfo);
        if (!ret) {
            return false;
        }
        if (len == 0) {
            return true;
        }

        const uint8_t *src = reinterpret_cast<const uint8_t *>(inVal.c_str());
        uint8_t *dest = memInfo.dataPtr;
        if (memcpy_s(dest, len, src, len) != ERR_OK) {
            HILOG_ERROR("WriteString failed:%{public}s", std::strerror(errno));
            return false;
        }
        memInfo.dataPtr += len;
        memInfo.dataSize += len;
        return true;
    }

    static bool ReadString(std::string &outVal, SharedMemoryInfo &memInfo)
    {
        std::string::size_type len = 0;
        int ret = ReadInteger<std::string::size_type>(len, memInfo);
        if (!ret) {
            return false;
        }
        if (len == 0) {
            outVal = "";
            return true;
        }

        uint8_t *src = memInfo.dataPtr;
        uint8_t dest[len + 1];
        if (memcpy_s(dest, len, src, len) != ERR_OK) {
            HILOG_ERROR("ReadString failed:%{public}s", std::strerror(errno));
            return false;
        }
        dest[len] = 0;
        outVal = reinterpret_cast<char *>(dest);
        memInfo.dataPtr += len;
        memInfo.dataSize -= len;
        return true;
    }

    static uint64_t GetFileInfoSize(FileInfo &info)
    {
        return sizeof(std::string::size_type) + info.uri.size() + sizeof(std::string::size_type) +
            info.relativePath.size() + sizeof(std::string::size_type) + info.fileName.size() + sizeof(info.mode) +
            sizeof(info.size) + sizeof(info.mtime) + sizeof(std::string::size_type) + info.mimeType.size();
    }
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // _FILE_INFO_SHARED_MEMORY_H_
