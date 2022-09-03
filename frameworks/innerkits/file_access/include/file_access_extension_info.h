/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FILE_ACCESS_EXTENSION_INFO_H
#define FILE_ACCESS_EXTENSION_INFO_H

#include <bitset>
#include <string>

#include "parcel.h"
#include "uri.h"

namespace OHOS {
namespace FileAccessFwk {
/**
 * Indicates the type of the device.
 */
constexpr int32_t DEVICE_LOCAL_DISK = 1;                // Local c,d... disk
constexpr int32_t DEVICE_SHARED_DISK = 2;               // Multi-user shared disk
constexpr int32_t DEVICE_SHARED_TERMINAL = 3;           // Distributed networking terminal device
constexpr int32_t DEVICE_NETWORK_NEIGHBORHOODS = 4;     // Network neighbor device
constexpr int32_t DEVICE_EXTERNAL_MTP = 5;              // MTP device
constexpr int32_t DEVICE_EXTERNAL_USB = 6;              // USB device
constexpr int32_t DEVICE_EXTERNAL_CLOUD = 7;            // Cloud disk device

/**
 * Indicates the supported capabilities of the device.
 */
const int32_t DEVICE_FLAG_SUPPORTS_READ = 1;
const int32_t DEVICE_FLAG_SUPPORTS_WRITE = 1 << 1;

/**
 * Indicates the supported capabilities of the file or directory.
 */
const int32_t DOCUMENT_FLAG_REPRESENTS_FILE = 1;
const int32_t DOCUMENT_FLAG_REPRESENTS_DIR = 1 << 1;
const int32_t DOCUMENT_FLAG_SUPPORTS_READ = 1 << 2;
const int32_t DOCUMENT_FLAG_SUPPORTS_WRITE = 1 << 3;

struct FileInfo : public virtual OHOS::Parcelable {
public:
    std::string uri;
    std::string fileName;
    int32_t mode;
    int64_t size;
    int64_t mtime;
    std::string mimeType;

    FileInfo() = default;
    FileInfo(std::string uriIn, std::string fileNameIn, int32_t modeIn, std::string mimeTypeIn)
        : uri(uriIn), fileName(fileNameIn), mode(modeIn), mimeType(mimeTypeIn)
    {}

    bool ReadFromParcel(Parcel &parcel)
    {
        uri = parcel.ReadString();
        fileName = parcel.ReadString();
        mode = parcel.ReadInt32();
        size = parcel.ReadInt64();
        mtime = parcel.ReadInt64();
        mimeType = parcel.ReadString();
        return true;
    }

    virtual bool Marshalling(Parcel &parcel) const override
    {
        if (!parcel.WriteString(uri)) {
            return false;
        }
        if (!parcel.WriteString(fileName)) {
            return false;
        }
        if (!parcel.WriteInt32(mode)) {
            return false;
        }
        if (!parcel.WriteInt64(size)) {
            return false;
        }
        if (!parcel.WriteInt64(mtime)) {
            return false;
        }
        if (!parcel.WriteString(mimeType)) {
            return false;
        }
        return true;
    }

    static FileInfo *Unmarshalling(Parcel &parcel)
    {
        FileInfo *info = new (std::nothrow) FileInfo();
        if (info == nullptr) {
            return nullptr;
        }

        if (!info->ReadFromParcel(parcel)) {
            delete info;
            info = nullptr;
        }
        return info;
    }
};

struct RootInfo : public virtual OHOS::Parcelable {
public:
    int32_t deviceType;
    std::string uri;
    std::string displayName;
    int32_t deviceFlags;

    bool ReadFromParcel(Parcel &parcel)
    {
        deviceType = parcel.ReadInt32();
        uri = parcel.ReadString();
        displayName = parcel.ReadString();
        deviceFlags = parcel.ReadInt32();
        return true;
    }

    virtual bool Marshalling(Parcel &parcel) const override
    {
        if (!parcel.WriteInt32(deviceType)) {
            return false;
        }
        if (!parcel.WriteString(uri)) {
            return false;
        }
        if (!parcel.WriteString(displayName)) {
            return false;
        }
        if (!parcel.WriteInt32(deviceFlags)) {
            return false;
        }
        return true;
    }

    static RootInfo *Unmarshalling(Parcel &parcel)
    {
        RootInfo *info = new (std::nothrow) RootInfo();
        if (info == nullptr) {
            return nullptr;
        }

        if (!info->ReadFromParcel(parcel)) {
            delete info;
            info = nullptr;
        }
        return info;
    }
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_EXTENSION_INFO_H