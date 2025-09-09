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

#ifndef CLOUD_DISK_COMMON_H
#define CLOUD_DISK_COMMON_H

#include "parcel.h"

#ifndef CLOUD_DISK_EXPORT
#define CLOUD_DISK_EXPORT __attribute__((visibility("default")))
#endif

namespace OHOS {
namespace FileManagement {
    enum class State {
        INACTIVE,
        ACTIVE,
        MAX_VALUE
    };

    struct SyncFolder : public OHOS::Parcelable {
        std::string path_;
        State state_ { State::INACTIVE };
        uint32_t displayNameResId_ { 0 }; // 0: default resId value
        std::string displayName_;

        SyncFolder() = default;
        SyncFolder(const std::string &path, State state, uint32_t resId, const std::string &displayName)
            :path_(path), state_(state), displayNameResId_(resId), displayName_(displayName) {}
        bool Marshalling(Parcel &parcel) const override;
        bool ReadFromParcel(Parcel &parcel);
        static SyncFolder *Unmarshalling(Parcel &parcel);
    };

    struct SyncFolderExt : SyncFolder {
        std::string bundleName_;

        SyncFolderExt() = default;
        SyncFolderExt(const std::string &path,
            State state, uint32_t resId, const std::string &displayName, const std::string &bundleName)
            :SyncFolder(path, state, resId, displayName), bundleName_(bundleName) {}
        bool Marshalling(Parcel &parcel) const override;
        bool ReadFromParcel(Parcel &parcel);
        static SyncFolderExt *Unmarshalling(Parcel &parcel);
    };
} // namespace FileManagement
} // namespace OHOS
#endif // CLOUD_DISK_COMMON_H