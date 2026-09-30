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


#ifndef CLOUD_DISK_HYDRATE_PROGRESS_CALLBACK_H
#define CLOUD_DISK_HYDRATE_PROGRESS_CALLBACK_H

#include <atomic>
#include <cstdint>
#include <mutex>

#include "cloud_disk_progress_callback_stub.h"
#include "napi/native_api.h"

namespace OHOS::FileManagement::CloudDiskService {
class HydrateProgressCallback final : public CloudDiskProgressCallbackStub {
public:
    static sptr<HydrateProgressCallback> Create(napi_env env, napi_value handler, uint64_t accessorId);
    void Close(bool removeHook = true);
    void OnProgress(const HydrateProgress &progress) override;

private:
    HydrateProgressCallback(napi_env env, uint64_t accessorId) : env_(env), accessorId_(accessorId) {}
    static void Cleanup(void *data);
    static void Finalize(napi_env env, void *data, void *hint);
    static void CallJs(napi_env env, napi_value function, void *context, void *data);

    napi_env env_;
    const uint64_t accessorId_;
    napi_threadsafe_function function_ = nullptr;
    std::mutex mutex_;
    std::atomic<bool> closed_{false};
    bool hookInstalled_ = false;
};
} // namespace OHOS::FileManagement::CloudDiskService

#endif // CLOUD_DISK_HYDRATE_PROGRESS_CALLBACK_H
