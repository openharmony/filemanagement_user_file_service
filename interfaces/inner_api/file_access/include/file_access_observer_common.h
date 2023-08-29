/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FILE_ACCESS_OBSERVER_COMMON_H
#define FILE_ACCESS_OBSERVER_COMMON_H

#include <string>
#include <vector>

#include "file_access_extension_info.h"
#include "hilog_wrapper.h"
#include "parcel.h"

namespace OHOS {
namespace FileAccessFwk {

enum NotifyType {
    NOTIFY_ADD = 0,
    NOTIFY_DELETE,
    NOTIFY_MOVE_TO,
    NOTIFY_MOVE_FROM,
    NOTIFY_MOVE_SELE
};

struct NotifyMessage : public OHOS::Parcelable {
public:
    NotifyType notifyType_;
    std::vector<std::string> uris_;
    NotifyMessage() = default;
    NotifyMessage(const NotifyType notifyType, const std::vector<std::string> &uris)
        : notifyType_(notifyType), uris_(uris) {}

    bool ReadFromParcel(Parcel &parcel)
    {
        notifyType_ = NotifyType(parcel.ReadInt32());
        auto count = parcel.ReadInt32();
        if (count > 1000) {
            return ;
        }
        for (int i = 0; i < count; i++) {
            uris_.push_back(parcel.ReadString());
        }
        return true;
    }

    virtual bool Marshalling(Parcel &parcel) const override
    {
        if (!parcel.WriteInt32(notifyType_)) {
            HILOG_ERROR("NotifyMessage Unmarshalling error:write deviceType fail.");
            return false;
        }
        if (!parcel.WriteInt32(uris_.size())) {
            HILOG_ERROR("NotifyMessage Unmarshalling error:write notifyType fail.");
            return false;
        }
        for (unsigned int i = 0; i < uris_.size(); i++) {
            if (!parcel.WriteString(uris_[i])) {
                HILOG_ERROR("NotifyMessage Unmarshalling error:write srcUri fail.");
                return false;
            }
        }
        return true;
    }

    static NotifyMessage *Unmarshalling(Parcel &parcel)
    {
        NotifyMessage *message = new (std::nothrow) NotifyMessage();
        if (message == nullptr) {
            HILOG_ERROR("NotifyMessage Unmarshalling error:new object fail.");
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
#endif // FILE_ACCESS_OBSERVER_COMMON_H
