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
#include <unordered_map>

#include "parcel.h"
#include "uri.h"

namespace OHOS {
namespace FileAccessFwk {
//Properties of the Common file
const std::string DISPLAY_NAME = "display_name";
const std::string RELATIVE_PATH = "relative_path";
const std::string FILE_SIZE = "size";
const std::string DATE_MODIFIED = "date_modified";
const std::string DATE_ADDED = "date_added";
//Properties of the picture file
const std::string HEIGHT = "height";
const std::string WIDTH = "width";
//Properties of an image or audio file
const std::string DURATION = "duration";

const std::string FILE_DATA_ATIME = "atime";
const std::string FILE_DATA_MTIME = "mtime";
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
const uint32_t DEVICE_FLAG_SUPPORTS_READ = 1;
const uint32_t DEVICE_FLAG_SUPPORTS_WRITE = 1 << 1;

/**
 * Indicates the supported capabilities of the file or directory.
 */
const uint32_t DOCUMENT_FLAG_REPRESENTS_FILE = 1;
const uint32_t DOCUMENT_FLAG_REPRESENTS_DIR = 1 << 1;
const uint32_t DOCUMENT_FLAG_SUPPORTS_READ = 1 << 2;
const uint32_t DOCUMENT_FLAG_SUPPORTS_WRITE = 1 << 3;

/**
 * Indicators of the supported capabilities of the file descriptor.
 */
constexpr int32_t READ = 0;
constexpr int32_t WRITE = 1;
constexpr int32_t WRITE_READ = 2;

/**
 * Indicates the supported Event change type.
 */
constexpr int32_t ADD_EVENT = 0;
constexpr int32_t DELETE_EVENT = 1;
constexpr int32_t MOVED_TO = 2;
constexpr int32_t MOVED_FROM = 3;
constexpr int32_t MOVED_SELF = 4;
constexpr int32_t DEVICE_ONLINE = 5;
constexpr int32_t DEVICE_OFFLINE = 6;

//Indicates the device root uris
const std::string DEVICE_URI = "file://docs";

//Indicates the root uri of the External device.
const std::string EXTERNAL_ROOT_URI = "file://docs/storage/External";
/**
 * Indicates the root uri for all device.
 */
static const std::vector<std::string> DEVICE_ROOTS = {EXTERNAL_ROOT_URI};

struct FileInfo : public virtual OHOS::Parcelable {
public:
    std::string uri { "" };
    std::string relativePath { "" };
    std::string fileName { "" };
    int32_t mode { 0 };
    int64_t size { 0 };
    int64_t mtime { 0 };
    std::string mimeType { "" };

    FileInfo() = default;
    FileInfo(std::string uri, std::string relativePath, std::string fileName, int32_t mode, std::string mimeType)
        : uri(uri), relativePath(relativePath), fileName(fileName), mode(mode), mimeType(mimeType)
    {}

    bool ReadFromParcel(Parcel &parcel)
    {
        uri = parcel.ReadString();
        relativePath = parcel.ReadString();
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
        if (!parcel.WriteString(relativePath)) {
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
    int32_t deviceType { 0 };
    std::string uri { "" };
    std::string relativePath { "" };
    std::string displayName { "" };
    int32_t deviceFlags { 0 };

    bool ReadFromParcel(Parcel &parcel)
    {
        deviceType = parcel.ReadInt32();
        uri = parcel.ReadString();
        relativePath = parcel.ReadString();
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
        if (!parcel.WriteString(relativePath)) {
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

struct ThumbnailSize : public virtual OHOS::Parcelable {
public:
    int32_t width { 0 };
    int32_t height { 0 };

    bool ReadFromParcel(Parcel &parcel)
    {
        width = parcel.ReadInt32();
        height = parcel.ReadInt32();
        return true;
    }

    virtual bool Marshalling(Parcel &parcel) const override
    {
        if (!parcel.WriteInt32(width)) {
            return false;
        }
        if (!parcel.WriteInt32(height)) {
            return false;
        }
        return true;
    }

    static ThumbnailSize *Unmarshalling(Parcel &parcel)
    {
        ThumbnailSize *size = new (std::nothrow) ThumbnailSize();
        if (size == nullptr) {
            return nullptr;
        }

        if (!size->ReadFromParcel(parcel)) {
            delete size;
            size = nullptr;
        }
        return size;
    }
};

enum ResultType {
    STRING_TYPE = 1,
    INT32_TYPE,
    INT64_TYPE,
};

static const std::unordered_map<std::string, ResultType> FILE_RESULT_TYPE = {
    { DISPLAY_NAME, STRING_TYPE },
    { RELATIVE_PATH, STRING_TYPE },
    { FILE_SIZE, INT64_TYPE },
    { DATE_ADDED, INT64_TYPE },
    { DATE_MODIFIED, INT64_TYPE },
    { WIDTH, INT32_TYPE },
    { HEIGHT, INT32_TYPE },
    { DURATION, INT32_TYPE },
};

static const std::unordered_map<std::string, std::string> CONVERT_FILE_COLUMN = {
    {DATE_ADDED, FILE_DATA_ATIME},
    {DATE_MODIFIED, FILE_DATA_MTIME}
};

struct CopyResult : public virtual OHOS::Parcelable {
public:
    std::string sourceUri { "" };
    std::string destUri { "" };
    int32_t errCode { 0 };
    std::string errMsg { "" };

    CopyResult() = default;
    CopyResult(std::string sourceUri, std::string destUri, int32_t errCode, std::string errMsg)
        : sourceUri(sourceUri), destUri(destUri), errCode(errCode),  errMsg(errMsg)
    {}

    bool ReadFromParcel(Parcel &parcel)
    {
        sourceUri = parcel.ReadString();
        destUri = parcel.ReadString();
        errCode = parcel.ReadInt32();
        errMsg = parcel.ReadString();
        return true;
    }

    virtual bool Marshalling(Parcel &parcel) const override
    {
        if (!parcel.WriteString(sourceUri)) {
            return false;
        }
        if (!parcel.WriteString(destUri)) {
            return false;
        }
        if (!parcel.WriteInt32(errCode)) {
            return false;
        }
        if (!parcel.WriteString(errMsg)) {
            return false;
        }
        return true;
    }

    static CopyResult *Unmarshalling(Parcel &parcel)
    {
        CopyResult *result = new (std::nothrow)CopyResult();
        if (result == nullptr) {
            return nullptr;
        }

        if (!result->ReadFromParcel(parcel)) {
            delete result;
            result = nullptr;
        }
        return result;
    }
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_EXTENSION_INFO_H
