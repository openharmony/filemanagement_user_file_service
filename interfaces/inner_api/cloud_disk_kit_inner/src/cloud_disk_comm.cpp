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
#include "cloud_disk_comm.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace FileManagement {
bool SyncFolder::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(path_)) {
        HILOG_ERROR("write path failed");
        return false;
    }
    if (!parcel.WriteInt32(static_cast<int32_t>(state_))) {
        HILOG_ERROR("write state failed");
        return false;
    }
    if (!parcel.WriteUint32(displayNameResId_)) {
        HILOG_ERROR("write resId failed");
        return false;
    }
    if (!parcel.WriteString(displayName_)) {
        HILOG_ERROR("write displayName failed");
        return false;
    }
    return true;
}

bool SyncFolder::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadString(path_)) {
        HILOG_ERROR("read path failed");
        return false;
    }
    int32_t state = 0;
    if (!parcel.ReadInt32(state)) {
        HILOG_ERROR("read state failed");
        return false;
    }
    if (state < static_cast<int32_t>(State::INACTIVE) || state >= static_cast<int32_t>(State::MAX_VALUE)) {
        HILOG_ERROR("state check failed, read state is: %{public}d", state);
        return false;
    }
    state_ = static_cast<State>(state);
    if (!parcel.ReadUint32(displayNameResId_)) {
        HILOG_ERROR("read resId failed");
        return false;
    }
    if (!parcel.ReadString(displayName_)) {
        HILOG_ERROR("read displayName failed");
        return false;
    }
    return true;
}

SyncFolder *SyncFolder::Unmarshalling(Parcel &parcel)
{
    SyncFolder *info = new (std::nothrow) SyncFolder();
    if (info == nullptr) {
        HILOG_ERROR("new syncFolder failed");
        return nullptr;
    }

    if (!info->ReadFromParcel(parcel)) {
        HILOG_ERROR("syncFolder ReadFromParcel failed");
        delete info;
        info = nullptr;
    }
    return info;
}

bool SyncFolderExt::Marshalling(Parcel &parcel) const
{
    if (!SyncFolder::Marshalling(parcel)) {
        HILOG_ERROR("syncFolder marshalling failed");
        return false;
    }
    if (!parcel.WriteString(bundleName_)) {
        HILOG_ERROR("write bundleName failed");
        return false;
    }
    return true;
}

bool SyncFolderExt::ReadFromParcel(Parcel &parcel)
{
    if (!SyncFolder::ReadFromParcel(parcel)) {
        HILOG_ERROR("syncFolder ReadFromParcel failed");
        return false;
    }
    if (!parcel.ReadString(bundleName_)) {
        HILOG_ERROR("read bundleName failed");
        return false;
    }
    return true;
}

SyncFolderExt *SyncFolderExt::Unmarshalling(Parcel &parcel)
{
    SyncFolderExt *info = new (std::nothrow) SyncFolderExt();
    if (info == nullptr) {
        HILOG_ERROR("new syncFolderExt failed");
        return nullptr;
    }

    if (!info->ReadFromParcel(parcel)) {
        HILOG_ERROR("syncFolderExt ReadFromParcel failed");
        delete info;
        info = nullptr;
    }
    return info;
}
} // namespace FileManagement
} // namespace OHOS