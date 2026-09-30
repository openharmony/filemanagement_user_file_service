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

#include "hydrate_progress_callback.h"

#include <new>

#include "hilog_wrapper.h"
#include "taihe/runtime_ani.hpp"

namespace OHOS::FileManagement::CloudDiskService {
namespace TH = ::ohos::file::cloudDiskManager;
namespace {
constexpr ani_size HYDRATE_PROGRESS_LOCAL_REF_CAPACITY = 16;
} // namespace

TaiheHydrateProgressCallback::TaiheHydrateProgressCallback(ani_vm *vm,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler, CallbackView callback)
    : vm_(vm), handler_(handler), callback_(std::in_place, callback)
{
}

TaiheHydrateProgressCallback::~TaiheHydrateProgressCallback()
{
    Close();
}

sptr<TaiheHydrateProgressCallback> TaiheHydrateProgressCallback::Create(CallbackView callback)
{
    auto *env = taihe::get_env();
    ani_vm *vm = nullptr;
    if (env == nullptr || callback.is_error() || env->GetVM(&vm) != ANI_OK || vm == nullptr) {
        HILOG_ERROR("Cannot create hydration callback without a valid ANI environment");
        return nullptr;
    }
    auto runner = AppExecFwk::EventRunner::Current();
    if (runner == nullptr) {
        runner = AppExecFwk::EventRunner::GetMainEventRunner();
    }
    if (runner == nullptr) {
        HILOG_ERROR("Cannot find an event runner for hydration callback");
        return nullptr;
    }
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    sptr<TaiheHydrateProgressCallback> result(new (std::nothrow)
        TaiheHydrateProgressCallback(vm, handler, callback));
    if (result == nullptr) {
        HILOG_ERROR("Allocate hydration callback failed");
        return nullptr;
    }
    HILOG_INFO("Taihe hydration progress callback created");
    return result;
}

void TaiheHydrateProgressCallback::Close()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (closed_.exchange(true)) {
        return;
    }
    HILOG_INFO("Close Taihe hydration progress callback");
    callback_.reset();
}

bool TaiheHydrateProgressCallback::IsClosed() const
{
    return closed_.load();
}

void TaiheHydrateProgressCallback::OnProgress(const HydrateProgress &progress)
{
    if (progress.state < static_cast<int32_t>(HydrateProgressState::PENDING) ||
        progress.state > static_cast<int32_t>(HydrateProgressState::CANCELLED)) {
        HILOG_ERROR("Invalid hydration progress state:%{public}d", progress.state);
        return;
    }
    if (IsClosed()) {
        return;
    }
    wptr<TaiheHydrateProgressCallback> weak(this);
    auto task = [weak, progress]() {
        auto callback = weak.promote();
        if (callback != nullptr) {
            callback->Dispatch(progress);
        }
    };
    if (handler_ == nullptr || !handler_->PostTask(task, "CloudDiskHydrateProgress")) {
        HILOG_ERROR("Post hydration progress to the ANI event runner failed");
    }
}

void TaiheHydrateProgressCallback::Dispatch(const HydrateProgress &progress)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (IsClosed() || !callback_.has_value()) {
        return;
    }
    ani_env *env = nullptr;
    if (taihe::get_vm() != vm_ || vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK || env == nullptr) {
        HILOG_ERROR("Hydration callback event runner has no matching ANI environment");
        return;
    }
    if (env->CreateLocalScope(HYDRATE_PROGRESS_LOCAL_REF_CAPACITY) != ANI_OK) {
        HILOG_ERROR("Create hydration callback ANI scope failed");
        return;
    }
    {
        // Keep the handler alive if it calls off() and clears callback_ reentrantly.
        auto callback = *callback_;
        TH::HydrateProgress value {progress.filePath, TH::HydrateProgressState::from_value(progress.state),
            static_cast<double>(progress.processedSize), static_cast<double>(progress.totalSize)};
        callback(value);
    }
    if (taihe::has_error()) {
        HILOG_ERROR("Hydration progress handler raised an exception");
        taihe::reset_error();
    }
    if (env->DestroyLocalScope() != ANI_OK) {
        HILOG_ERROR("Destroy hydration callback ANI scope failed");
    }
}
} // namespace OHOS::FileManagement::CloudDiskService
