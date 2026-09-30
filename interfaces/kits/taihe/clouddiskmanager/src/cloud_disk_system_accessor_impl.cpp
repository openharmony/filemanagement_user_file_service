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

#include "ohos.file.cloudDiskManager.impl.hpp"
#include "ohos.file.cloudDiskManager.proj.hpp"

#include <climits>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>

#include "accesstoken_kit.h"
#include "cloud_disk_error.h"
#include "cloud_disk_manager_utils.h"
#include "cloud_disk_service_manager.h"
#include "hilog_wrapper.h"
#include "hydrate_progress_callback.h"
#include "ipc_skeleton.h"
#include "taihe/runtime.hpp"
#include "tokenid_kit.h"

namespace {
namespace TH = ohos::file::cloudDiskManager;
namespace FM = OHOS::FileManagement;
using FM::CloudDiskService::CloudDiskServiceManager;
using FM::CloudDiskService::TaiheHydrateProgressCallback;
using ProgressCallback = taihe::callback<void(TH::HydrateProgress const &)>;

void ThrowError(int32_t code)
{
    auto error = FM::GetCloudDiskErrorInfo(code);
    HILOG_ERROR("CloudDiskSystemAccessor Taihe failed, ret:%{public}d, code:%{public}d", code, error.code);
    taihe::set_business_error(error.code, error.message);
}

int32_t CheckAccess()
{
    using namespace OHOS::Security::AccessToken;
    auto token = OHOS::IPCSkeleton::GetCallingTokenID();
    auto type = AccessTokenKit::GetTokenTypeFlag(token);
    if (type != TOKEN_HAP && type != TOKEN_NATIVE) {
        HILOG_ERROR("CloudDiskSystemAccessor Taihe unsupported caller type:%{public}d", static_cast<int32_t>(type));
        return FM::E_PERMISSION;
    }
    if (AccessTokenKit::VerifyAccessToken(token, "ohos.permission.ACCESS_CLOUD_DISK_INFO") != PERMISSION_GRANTED) {
        HILOG_ERROR("CloudDiskSystemAccessor Taihe ACCESS_CLOUD_DISK_INFO permission denied");
        return FM::E_PERMISSION;
    }
    if (type != TOKEN_NATIVE &&
        !TokenIdKit::IsSystemAppByFullTokenID(OHOS::IPCSkeleton::GetCallingFullTokenID())) {
        HILOG_ERROR("CloudDiskSystemAccessor Taihe caller is not a system application");
        return FM::E_PERMISSION_SYSTEM;
    }
    return FM::E_OK;
}

bool ReadPath(taihe::string_view value, std::string &path)
{
    if (value.empty() || value.size() > PATH_MAX ||
        std::string_view(value.data(), value.size()).find('\0') != std::string_view::npos) {
        HILOG_ERROR("CloudDiskSystemAccessor Taihe invalid file path");
        return false;
    }
    path.assign(value.data(), value.size());
    return true;
}

class CloudDiskSystemAccessorImpl {
public:
    CloudDiskSystemAccessorImpl()
    {
        HILOG_INFO("CloudDiskSystemAccessor Taihe constructor");
        if (accessorId_ == 0) {
            HILOG_ERROR("CloudDiskSystemAccessor Taihe constructor generated invalid accessor ID");
            ThrowError(FM::E_TRY_AGAIN);
        }
    }

    ~CloudDiskSystemAccessorImpl()
    {
        if (progress_ == nullptr) {
            return;
        }
        progress_->Close();
        int32_t ret = CloudDiskServiceManager::GetInstance().UnregisterProgressCallback(
            accessorId_, progress_);
        if (ret != FM::E_OK && ret != FM::E_NOT_SUPPORT) {
            HILOG_ERROR("CloudDiskSystemAccessor Taihe destructor unregister failed, ret:%{public}d", ret);
        }
    }

    void OnHydrateProgress(taihe::unit type, taihe::callback_view<void(TH::HydrateProgress const &)> callback)
    {
        (void)type;
        HILOG_INFO("CloudDiskSystemAccessor Taihe on begin");
        int32_t ret = CheckAccess();
        if (ret != FM::E_OK) {
            ThrowError(ret);
            return;
        }
        std::unique_lock<std::mutex> lock(mutex_);
        if (progress_ != nullptr) {
            ThrowError(FM::E_CALLBACK_ALREADY_REGISTERED);
            return;
        }
        auto progress = TaiheHydrateProgressCallback::Create(callback);
        if (progress == nullptr) {
            ThrowError(FM::E_TRY_AGAIN);
            return;
        }
        ret = CloudDiskServiceManager::GetInstance().RegisterProgressCallback(accessorId_, progress);
        if (ret != FM::E_OK) {
            lock.unlock();
            progress->Close();
            ThrowError(ret);
            return;
        }
        progress_ = std::move(progress);
    }

    void OffHydrateProgress(taihe::unit type, taihe::optional_view<ProgressCallback> callback)
    {
        (void)type;
        (void)callback;
        HILOG_INFO("CloudDiskSystemAccessor Taihe off begin");
        int32_t ret = CheckAccess();
        if (ret != FM::E_OK) {
            ThrowError(ret);
            return;
        }
        std::unique_lock<std::mutex> lock(mutex_);
        if (progress_ == nullptr) {
            ThrowError(FM::E_CALLBACK_NOT_REGISTERED);
            return;
        }
        ret = CloudDiskServiceManager::GetInstance().UnregisterProgressCallback(accessorId_, progress_);
        auto progress = std::move(progress_);
        lock.unlock();
        progress->Close();
        if (ret != FM::E_OK) {
            ThrowError(ret);
        }
    }

    void HydratePlaceholderSync(taihe::string_view filePath, TH::CallbackType callbackType,
        TH::HydratePriority priority)
    {
        HILOG_INFO("CloudDiskSystemAccessor Taihe hydratePlaceholder begin");
        int32_t ret = CheckAccess();
        if (ret != FM::E_OK) {
            ThrowError(ret);
            return;
        }
        std::string path;
        if (!ReadPath(filePath, path) || !callbackType.is_valid() || !priority.is_valid()) {
            ThrowError(FM::E_PARAMETER_ERROR);
            return;
        }
        ret = CloudDiskServiceManager::GetInstance().StartHydrationByPath(
            path, callbackType.get_value(), priority.get_value(), accessorId_);
        if (ret != FM::E_OK) {
            HILOG_ERROR("Taihe hydratePlaceholder failed, ret:%{public}d, path:%{private}s", ret, path.c_str());
            ThrowError(ret);
        }
    }

    void DehydrateFileSync(taihe::string_view filePath)
    {
        HILOG_INFO("CloudDiskSystemAccessor Taihe dehydrateFile begin");
        int32_t ret = CheckAccess();
        if (ret != FM::E_OK) {
            ThrowError(ret);
            return;
        }
        std::string path;
        if (!ReadPath(filePath, path)) {
            ThrowError(FM::E_PARAMETER_ERROR);
            return;
        }
        ret = CloudDiskServiceManager::GetInstance().DehydrateFileByPath(path);
        if (ret != FM::E_OK) {
            HILOG_ERROR("Taihe dehydrateFile failed, ret:%{public}d, path:%{private}s", ret, path.c_str());
            ThrowError(ret);
        }
    }

private:
    const uint64_t accessorId_ = FM::CreateCloudDiskAccessorId();
    std::mutex mutex_;
    OHOS::sptr<TaiheHydrateProgressCallback> progress_;
};

TH::CloudDiskSystemAccessor CreateCloudDiskSystemAccessor()
{
    return taihe::make_holder<CloudDiskSystemAccessorImpl, TH::CloudDiskSystemAccessor>();
}
} // namespace

TH_EXPORT_CPP_API_CreateCloudDiskSystemAccessor(CreateCloudDiskSystemAccessor);
