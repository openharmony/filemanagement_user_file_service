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

#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "parcel.h"
#include "system_ability_definition.h"
#include "uri.h"

namespace OHOS {
namespace FileAccessFwk {
class FileExtInfo
{
public:
    FileExtInfo() = default;
    ~FileExtInfo() = default;

    static sptr<AppExecFwk::IBundleMgr> GetBundleMgrProxy()
    {
        sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!systemAbilityManager) {
            return nullptr;
        }

        sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
        if (!remoteObject) {
            return nullptr;
        }

        return iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    }
};

struct FileInfo : public virtual OHOS::Parcelable {
public:
    Uri uri = Uri("");
    std::string fileName;
    std::string mode;
    int64_t size {0};
    int64_t mtime {0};
    std::string mimeType;

    bool ReadFromParcel(Parcel &parcel)
    {
        std::unique_ptr<Uri> uriInfo(parcel.ReadParcelable<Uri>());
        if (uriInfo == nullptr) {
            return false;
        }
        uri = *uriInfo;

        fileName = parcel.ReadString();
        mode = parcel.ReadString();
        size = parcel.ReadInt64();
        mtime = parcel.ReadInt64();
        mimeType = parcel.ReadString();
        return true;
    }

    virtual bool Marshalling(Parcel &parcel) const override
    {
        if (!parcel.WriteParcelable(&uri)) {
            return false;
        }
        if (!parcel.WriteString(fileName)) {
            return false;
        }
        if (!parcel.WriteString(mode)) {
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

struct DeviceInfo : public virtual OHOS::Parcelable {
public:
    Uri uri = Uri("");
    std::string displayName;
    std::string deviceId;
    uint32_t flags {0};

    bool ReadFromParcel(Parcel &parcel)
    {
        std::unique_ptr<Uri> uriInfo(parcel.ReadParcelable<Uri>());
        if (uriInfo == nullptr) {
            return false;
        }
        uri = *uriInfo;

        displayName = parcel.ReadString();
        deviceId = parcel.ReadString();
        flags = parcel.ReadUint32();
        return true;
    }

    virtual bool Marshalling(Parcel &parcel) const override
    {
        if (!parcel.WriteParcelable(&uri)) {
            return false;
        }
        if (!parcel.WriteString(displayName)) {
            return false;
        }
        if (!parcel.WriteString(deviceId)) {
            return false;
        }
        if (!parcel.WriteUint32(flags)) {
            return false;
        }
        return true;
    }

    static DeviceInfo *Unmarshalling(Parcel &parcel)
    {
        DeviceInfo *info = new (std::nothrow) DeviceInfo();
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

const uint32_t FLAG_SUPPORTS_THUMBNAIL = 1;
const uint32_t FLAG_SUPPORTS_WRITE = 1 << 1;
const uint32_t FLAG_SUPPORTS_READ = 1 << 2;
const uint32_t FLAG_SUPPORTS_DELETE = 1 << 3;
const uint32_t FLAG_SUPPORTS_RENAME = 1 << 4;
const uint32_t FLAG_SUPPORTS_MOVE = 1 << 5;
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_EXTENSION_INFO_H