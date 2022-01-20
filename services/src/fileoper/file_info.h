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
#ifndef STORAGE_SERVICES_FILE_INFO_H
#define STORAGE_SERVICES_FILE_INFO_H

#include <cstdint>
#include <string>
#include "ipc_types.h"
#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"

namespace OHOS {
namespace FileManagerService {
class FileInfo {
public:
    FileInfo(const std::string &name, const std::string &path, const std::string &type) : path_(path),
        name_(name), type_(type) {}
    FileInfo() = default;
    ~FileInfo() = default;

    void SetName(std::string &name)
    {
        name_ = name;
    }
    void SetPath(std::string &path)
    {
        path_ = path;
    }
    void SetType(std::string &type)
    {
        type_ = type;
    }
    void SetSize(int64_t size)
    {
        size_ = size;
    }
    void SetAddedTime(int64_t time)
    {
        addedTime_ = time;
    }
    void SetModifiedTime(int64_t time)
    {
        modifiedTime_ =  time;
    }
    std::string GetName() const
    {
        return name_;
    }
    std::string GetPath() const
    {
        return path_;
    }
    std::string GetType() const
    {
        return type_;
    }
    int64_t GetSize() const
    {
        return size_;
    }
    int64_t GetAddedTime() const
    {
        return addedTime_;
    }
    int64_t GetModifiedTime() const
    {
        return modifiedTime_;
    }
private:
    std::string path_;
    std::string name_;
    std::string type_;
    int64_t size_ {0};
    int64_t addedTime_ {0};
    int64_t modifiedTime_ {0};
};
} // namespace FileManagerService
} // namespace OHOS
#endif // STORAGE_SERVICES_FILE_INFO_H