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

#ifndef CLOUD_DISK_TAIHE_HYDRATE_PROGRESS_CALLBACK_H
#define CLOUD_DISK_TAIHE_HYDRATE_PROGRESS_CALLBACK_H

#include <atomic>
#include <memory>
#include <mutex>
#include <optional>

#include "ani.h"
#include "cloud_disk_progress_callback_stub.h"
#include "event_handler.h"
#include "ohos.file.cloudDiskManager.proj.hpp"
#include "taihe/callback.hpp"

namespace OHOS::FileManagement::CloudDiskService {
class TaiheHydrateProgressCallback final : public CloudDiskProgressCallbackStub {
public:
    using Callback = taihe::callback<void(::ohos::file::cloudDiskManager::HydrateProgress const &)>;
    using CallbackView = taihe::callback_view<void(::ohos::file::cloudDiskManager::HydrateProgress const &)>;

    static sptr<TaiheHydrateProgressCallback> Create(CallbackView callback);
    ~TaiheHydrateProgressCallback() override;
    void Close();
    bool IsClosed() const;
    void OnProgress(const HydrateProgress &progress) override;

private:
    TaiheHydrateProgressCallback(ani_vm *vm, const std::shared_ptr<AppExecFwk::EventHandler> &handler,
        CallbackView callback);
    void Dispatch(const HydrateProgress &progress);

    ani_vm *vm_;
    const std::shared_ptr<AppExecFwk::EventHandler> handler_;
    std::optional<Callback> callback_;
    // A progress handler may call off() while Dispatch holds the lock.
    std::recursive_mutex mutex_;
    std::atomic<bool> closed_{false};
};
} // namespace OHOS::FileManagement::CloudDiskService

#endif // CLOUD_DISK_TAIHE_HYDRATE_PROGRESS_CALLBACK_H
