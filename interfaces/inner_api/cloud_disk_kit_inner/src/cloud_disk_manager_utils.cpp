/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "cloud_disk_manager_utils.h"

#include <atomic>
#include <chrono>

namespace OHOS::FileManagement {
// NAPI and Taihe share this allocator through cloud_disk_manager_js_kit.
uint64_t CreateCloudDiskAccessorId()
{
    static std::atomic<uint64_t> nextAccessorId{
        static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count())
    };
    return nextAccessorId.fetch_add(1, std::memory_order_relaxed);
}
} // namespace OHOS::FileManagement
