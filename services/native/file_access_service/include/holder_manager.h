/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef HOLDER_MANAGER_H_
#define HOLDER_MANAGER_H_

#include <atomic>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <securec.h>
#include <type_traits>

namespace OHOS {
namespace FileAccessFwk {
template<typename Type>
class HolderManager {
public:
    static constexpr int CODE_CAN_NOT_FIND = 0;
    HolderManager() {}

    ~HolderManager()
    {
        std::lock_guard<std::mutex> guard(holderMutex_);
        holder_.clear();
    }

    uint32_t save(Type content)
    {
        uint32_t id;
        do {
            id = genId();
        } while (exist(id));
        std::lock_guard<std::mutex> guard(holderMutex_);
        holder_.insert(std::pair<uint32_t, Type>(id, content));
        return id;
    }

    Type get(uint32_t id)
    {
        std::lock_guard<std::mutex> guard(holderMutex_);
        auto iter = holder_.find(id);
        if (iter != holder_.end()) {
            return iter->second;
        }
        return Type();
    }

    Type pop(uint32_t id)
    {
        std::lock_guard<std::mutex> guard(holderMutex_);
        auto iter = holder_.find(id);
        if (iter != holder_.end()) {
            auto res = iter->second;
            holder_.erase(id);
            return res;
        }
        return Type();
    }

    void release(uint32_t id)
    {
        std::lock_guard<std::mutex> guard(holderMutex_);
        if (holder_.count(id)) {
            holder_.erase(id);
        }
    }

    bool exist(uint32_t id)
    {
        std::lock_guard<std::mutex> guard(holderMutex_);
        return holder_.count(id);
    }

    uint32_t getId(std::function<bool(const Type &)> func)
    {
        auto haveIter = find_if(holder_.begin(), holder_.end(),
            [func](const std::pair<uint32_t, Type> type) {
                return func(type.second);
            });
        if (haveIter == holder_.end()) {
            return CODE_CAN_NOT_FIND;
        }
        return haveIter->first;
    }

private:
    // key is automatic growth number
    std::map<uint32_t, Type> holder_;
    std::mutex holderMutex_;
    std::atomic<uint32_t> gId_ = 1;
    uint32_t genId()
    {
        return gId_++;
    }
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FRAMEWORKS_INNERKITSIMPL_UTILS_INCLUDE_IMAGE_HOLDER_MANAGER_H_