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

namespace OHOS {
namespace FileManagerService {
class ExtStorageStatus {
public:
    ExtStorageStatus() = default;
    ~ExtStorageStatus() = default;
    ExtStorageStatus(const std::string &id, const std::string &diskId,
        const std::string &fsUuid, const std::string &path, const int32_t notifyCode)
        : id_(id), diskId_(diskId), fsUuid_(fsUuid), path_(path), notifyCode_(notifyCode) {}
    
    ExtStorageStatus(const ExtStorageStatus &extStatus)
    {
        this->id_ = extStatus.id_;
        this->diskId_ = extStatus.diskId_;
        this->fsUuid_ = extStatus.fsUuid_;
        this->path_ = extStatus.path_;
        this->notifyCode_ = extStatus.notifyCode_;
    }

    ExtStorageStatus& operator=(const ExtStorageStatus &extStatus)
    {
        this->id_ = extStatus.id_;
        this->diskId_ = extStatus.diskId_;
        this->fsUuid_ = extStatus.fsUuid_;
        this->path_ = extStatus.path_;
        this->notifyCode_ = extStatus.notifyCode_;
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

    int32_t GetNotifyCode()
    {
        return notifyCode_;
    }

    void SetNotifyCode(const int32_t &notifyCode)
    {
        notifyCode_ = notifyCode;
    }

private:
    std::string id_;
    std::string diskId_;
    std::string fsUuid_;
    std::string path_;
    int32_t notifyCode_;
};
} // namespace FileManagerService
} // namespace OHOS
#endif // STORAGE_EXT_STORAGE_STATUS_H