/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#ifndef STORAGE_EXT_STORAGE_STATUS_H
#define STORAGE_EXT_STORAGE_STATUS_H

#include <string>

#include "file_manager_service_def.h"

namespace OHOS {
namespace FileManagerService {
class ExtStorageStatus {
public:
    ExtStorageStatus() = default;
    ~ExtStorageStatus() = default;
    ExtStorageStatus(const std::string &id, const std::string &diskId,
        const std::string &fsUuid, const std::string &path, const VolumeState &volumeState)
        : id_(id), diskId_(diskId), fsUuid_(fsUuid), path_(path), volumeState_(volumeState) {}
    
    ExtStorageStatus(const ExtStorageStatus &extStatus)
    {
        this->id_ = extStatus.id_;
        this->diskId_ = extStatus.diskId_;
        this->fsUuid_ = extStatus.fsUuid_;
        this->path_ = extStatus.path_;
        this->volumeState_ = extStatus.volumeState_;
    }

    ExtStorageStatus& operator=(const ExtStorageStatus &extStatus)
    {
        this->id_ = extStatus.id_;
        this->diskId_ = extStatus.diskId_;
        this->fsUuid_ = extStatus.fsUuid_;
        this->path_ = extStatus.path_;
        this->volumeState_ = extStatus.volumeState_;
        return *this;
    }

    std::string GetId()
    {
        return id_;
    }

    void SetId(const std::string &id)
    {
        id_ = id;
    }

    std::string GetDiskId()
    {
        return diskId_;
    }

    void SetDiskId(const std::string &diskId)
    {
        diskId_ = diskId;
    }

    std::string GetFsUuid()
    {
        return fsUuid_;
    }

    void SetFsUuid(const std::string &fsUuid)
    {
        fsUuid_ = fsUuid;
    }

    std::string GetPath()
    {
        return path_;
    }

    void SetPath(const std::string &path)
    {
        path_ = path;
    }

    VolumeState GetVolumeState()
    {
        return volumeState_;
    }

    void SetVolumeState(const VolumeState &volumeState)
    {
        volumeState_ = volumeState;
    }

private:
    std::string id_;
    std::string diskId_;
    std::string fsUuid_;
    std::string path_;
    VolumeState volumeState_;
};
} // namespace FileManagerService
} // namespace OHOS
#endif // STORAGE_EXT_STORAGE_STATUS_H