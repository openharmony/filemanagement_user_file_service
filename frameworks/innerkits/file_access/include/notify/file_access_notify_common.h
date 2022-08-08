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

#ifndef FILE_ACCESS_NOTIFY_COMMON_H
#define FILE_ACCESS_NOTIFY_COMMON_H

#include <string>

#include "file_access_extension_info.h"
#include "parcel.h"

namespace OHOS {
namespace FileAccessFwk {
enum NotifyType {
    NOTIFY_DEVICE_ONLINE = 1,
    NOTIFY_DEVICE_OFFLINE
};

struct NotifyMessage : public virtual OHOS::Parcelable {
public:
    DeviceType deviceType;
    NotifyType notifyType;
    std::string srcUri;     // source uri when notifyType is (NOTIFY_FILE_MOVE/NOTIFY_FILE_RENAME), other case is "".
    std::string dstUri;     // destination uri for all notifyType

    NotifyMessage() = default;
    NotifyMessage(const DeviceType deviceTypeIn, const NotifyType &notifyTypeIn, const std::string &srcUriIn,
        const std::string &dstUriIn)
        : deviceType(deviceTypeIn), notifyType(notifyTypeIn), srcUri(srcUriIn), dstUri(dstUriIn)
    {}

    bool ReadFromParcel(Parcel &parcel)
    {
        deviceType = (DeviceType)parcel.ReadInt32();
        notifyType = (NotifyType)parcel.ReadInt32();
        srcUri = parcel.ReadString();
        dstUri = parcel.ReadString();
        return true;
    }

    virtual bool Marshalling(Parcel &parcel) const override
    {
        if (!parcel.WriteInt32((int32_t)deviceType)) {
            return false;
        }
        if (!parcel.WriteInt32((int32_t)notifyType)) {
            return false;
        }
        if (!parcel.WriteString(srcUri)) {
            return false;
        }
        if (!parcel.WriteString(dstUri)) {
            return false;
        }
        return true;
    }

    static NotifyMessage *Unmarshalling(Parcel &parcel)
    {
        NotifyMessage *message = new(std::nothrow) NotifyMessage();
        if (message == nullptr) {
            return nullptr;
        }

        if (!message->ReadFromParcel(parcel)) {
            delete message;
            message = nullptr;
        }
        return message;
    }
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_NOTIFY_COMMON_H