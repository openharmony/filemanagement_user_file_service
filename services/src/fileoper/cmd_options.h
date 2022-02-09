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
#ifndef STORAGE_SERVICES_DEV_INFO_H
#define STORAGE_SERVICES_DEV_INFO_H

#include <string>

namespace OHOS {
namespace FileManagerService {
class DevInfo {
public:
    DevInfo() = default;
    ~DevInfo() = default;
    DevInfo(const std::string &nameIn, const std::string &pathIn) : name_(nameIn), path_(pathIn)
    {}

    DevInfo(const DevInfo &dev)
    {
        this->name_ = dev.name_;
        this->path_ = dev.path_;
    }

    DevInfo& operator=(const DevInfo& dev)
    {
        this->name_ = dev.name_;
        this->path_ = dev.path_;
        return *this;
    }

    std::string GetName() const
    {
        return name_;
    }

    void SetName(const std::string& name)
    {
        name_ = name;
    }

    std::string GetPath() const
    {
        return path_;
    }

    void SetPath(const std::string& path)
    {
        path_ = path;
    }

private:
    std::string name_ {""};
    std::string path_ {""};
};

class CmdOptions {
public:
    CmdOptions() = default;
    ~CmdOptions() = default;

    CmdOptions(DevInfo devIn, int64_t offsetIn, int64_t countIn, bool hasOptIn)
        : dev_(devIn), offset_(offsetIn), count_(countIn), hasOpt_(hasOptIn)
    {}
    CmdOptions(const std::string &nameIn, const std::string &pathIn,
        int64_t offsetIn, int64_t countIn, bool hasOptIn)
        : dev_(nameIn, pathIn), offset_(offsetIn), count_(countIn), hasOpt_(hasOptIn)
    {}

    CmdOptions(const CmdOptions &option) = default;
    CmdOptions& operator=(const CmdOptions& option) = default;

    DevInfo GetDevInfo() const
    {
        return dev_;
    }

    void SetDevInfo(const DevInfo& dev)
    {
        dev_ = dev;
    }

    int64_t GetOffset() const
    {
        return offset_;
    }

    void SetOffset(int64_t offset)
    {
        offset_ = offset;
    }

    int64_t GetCount() const
    {
        return count_;
    }

    void setCount(int64_t count)
    {
        count_ = count;
    }

    bool GetHasOpt() const
    {
        return hasOpt_;
    }

    void SetHasOpt(bool hasOpt)
    {
        hasOpt_ = hasOpt;
    }

private:
    DevInfo dev_;
    int64_t offset_ {0};
    int64_t count_ {0};
    bool hasOpt_ {false};
};
} // namespace FileManagerService
} // namespace OHOS
#endif // STORAGE_SERVICES_DEV_INFO_H