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

#include <memory>
#include <new>

#include "cloud_disk_error.h"
#include "cloud_disk_service_manager.h"
#include "hilog_wrapper.h"

namespace OHOS::FileManagement::CloudDiskService {
sptr<HydrateProgressCallback> HydrateProgressCallback::Create(napi_env env, napi_value handler, uint64_t accessorId)
{
    HILOG_INFO("HydrateProgressCallback::Create start");
    sptr<HydrateProgressCallback> callback = new (std::nothrow) HydrateProgressCallback(env, accessorId);
    if (callback == nullptr) {
        HILOG_ERROR("HydrateProgressCallback::Create allocate callback failed");
        return nullptr;
    }
    auto holder = std::make_unique<sptr<HydrateProgressCallback>>(callback);
    napi_value name = nullptr;
    napi_status status = napi_create_string_utf8(env, "hydrateProgress", NAPI_AUTO_LENGTH, &name);
    if (status != napi_ok) {
        HILOG_ERROR("HydrateProgressCallback::Create event name failed, status:%{public}d", status);
        return nullptr;
    }
    // The TSFN owns the single JS callback reference until it is finalized.
    status = napi_create_threadsafe_function(env, handler, nullptr, name, 0, 1, holder.get(), Finalize,
        callback.GetRefPtr(), CallJs, &callback->function_);
    if (status != napi_ok) {
        HILOG_ERROR("HydrateProgressCallback::Create event channel failed, status:%{public}d", status);
        return nullptr;
    }
    holder.release(); // The TSFN finalizer owns this holder until finalization.
    status = napi_unref_threadsafe_function(env, callback->function_);
    if (status != napi_ok) {
        HILOG_ERROR("HydrateProgressCallback::Create unref event channel failed, status:%{public}d", status);
        callback->Close();
        return nullptr;
    }
    status = napi_add_env_cleanup_hook(env, Cleanup, callback.GetRefPtr());
    if (status != napi_ok) {
        HILOG_ERROR("HydrateProgressCallback::Create add cleanup hook failed, status:%{public}d", status);
        callback->Close();
        return nullptr;
    }
    callback->hookInstalled_ = true;
    return callback;
}

void HydrateProgressCallback::Close(bool removeHook)
{
    HILOG_INFO("HydrateProgressCallback::Close start, removeHook:%{public}d", static_cast<int32_t>(removeHook));
    napi_threadsafe_function function = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (closed_.exchange(true)) {
            return;
        }
        function = function_;
        function_ = nullptr;
    }
    if (removeHook && hookInstalled_) {
        napi_status status = napi_remove_env_cleanup_hook(env_, Cleanup, this);
        if (status != napi_ok) {
            HILOG_ERROR("HydrateProgressCallback::Close remove cleanup hook failed, status:%{public}d", status);
        }
    }
    hookInstalled_ = false;
    if (function != nullptr) {
        napi_status status = napi_release_threadsafe_function(function, napi_tsfn_abort);
        if (status != napi_ok) {
            HILOG_ERROR("HydrateProgressCallback::Close release event channel failed, status:%{public}d", status);
        }
    }
}

void HydrateProgressCallback::OnProgress(const HydrateProgress &progress)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (closed_ || function_ == nullptr) {
        return;
    }
    auto event = std::make_unique<HydrateProgress>(progress);
    napi_status status = napi_call_threadsafe_function(function_, event.get(), napi_tsfn_nonblocking);
    if (status == napi_ok) {
        event.release();
    } else if (status != napi_closing) {
        HILOG_ERROR("HydrateProgressCallback::OnProgress queue failed, status:%{public}d, path:%{private}s, "
            "state:%{public}d", status, progress.filePath.c_str(), progress.state);
    }
}

void HydrateProgressCallback::Cleanup(void *data)
{
    HILOG_INFO("HydrateProgressCallback::Cleanup start");
    sptr<HydrateProgressCallback> callback = static_cast<HydrateProgressCallback *>(data);
    callback->Close(false);
    int32_t ret = CloudDiskServiceManager::GetInstance().UnregisterProgressCallback(
        callback->accessorId_, callback);
    if (ret != E_OK && ret != E_NOT_SUPPORT) {
        HILOG_ERROR("HydrateProgressCallback::Cleanup unregister callback failed, ret:%{public}d", ret);
    }
}

void HydrateProgressCallback::Finalize(napi_env env, void *data, void *hint)
{
    delete static_cast<sptr<HydrateProgressCallback> *>(data);
}

void HydrateProgressCallback::CallJs(napi_env env, napi_value function, void *context, void *data)
{
    std::unique_ptr<HydrateProgress> event(static_cast<HydrateProgress *>(data));
    auto callback = static_cast<HydrateProgressCallback *>(context);
    if (env == nullptr || event == nullptr || callback->closed_) {
        return;
    }
    if (function == nullptr) {
        HILOG_ERROR("HydrateProgressCallback::CallJs JS callback is null");
        return;
    }
    napi_value value = nullptr;
    napi_value path = nullptr;
    napi_value state = nullptr;
    napi_value processed = nullptr;
    napi_value total = nullptr;
    napi_value receiver = nullptr;
    const auto &progress = *event;
    napi_status status = napi_ok;
    if ((status = napi_create_object(env, &value)) != napi_ok ||
        (status = napi_create_string_utf8(env, progress.filePath.data(), progress.filePath.size(), &path)) != napi_ok ||
        (status = napi_create_int32(env, progress.state, &state)) != napi_ok ||
        (status = napi_create_double(env, static_cast<double>(progress.processedSize), &processed)) != napi_ok ||
        (status = napi_create_double(env, static_cast<double>(progress.totalSize), &total)) != napi_ok ||
        (status = napi_set_named_property(env, value, "filePath", path)) != napi_ok ||
        (status = napi_set_named_property(env, value, "state", state)) != napi_ok ||
        (status = napi_set_named_property(env, value, "processedSize", processed)) != napi_ok ||
        (status = napi_set_named_property(env, value, "totalSize", total)) != napi_ok ||
        (status = napi_get_undefined(env, &receiver)) != napi_ok) {
        HILOG_ERROR("HydrateProgressCallback::CallJs create progress value failed, status:%{public}d, path:%{private}s",
            status, progress.filePath.c_str());
        return;
    }
    // Setting properties can invoke inherited setters that unsubscribe this instance.
    if (callback->closed_) {
        return;
    }
    napi_value result = nullptr;
    status = napi_call_function(env, receiver, function, 1, &value, &result);
    if (status != napi_ok) {
        HILOG_ERROR("HydrateProgressCallback::CallJs invoke callback failed, status:%{public}d", status);
    }
}
} // namespace OHOS::FileManagement::CloudDiskService
