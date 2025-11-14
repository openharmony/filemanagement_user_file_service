/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ohos.file.cloudDiskManager.proj.hpp"
#include "ohos.file.cloudDiskManager.impl.hpp"
#include "stdexcept"

#include "cloud_disk_js_manager.h"
#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "taihe/runtime.hpp"
#include "taihe/string.hpp"

using namespace std;
namespace {
class SyncFolderAccessorImpl {
public:
    SyncFolderAccessorImpl() {}

    static ::ohos::file::cloudDiskManager::SyncFolder GetSyncFolder(OHOS::FileManagement::SyncFolderExt folderInfo)
    {
        return {folderInfo.path_,
                folderInfo.bundleName_,
                ohos::file::cloudDiskManager::SyncFolderState::from_value(static_cast<int>(folderInfo.state_)),
                ::taihe::optional<int32_t>(std::in_place_t{}, folderInfo.displayNameResId_),
                ::taihe::optional<taihe::string>(std::in_place_t{}, folderInfo.displayName_)
                };
    }

    ::taihe::array<::ohos::file::cloudDiskManager::SyncFolder> GetAllSyncFoldersSync()
    {
        HILOG_INFO("GetAllSyncFoldersSync begin");
        vector<OHOS::FileManagement::SyncFolderExt> result;
        auto syncFolderJSAccess = make_shared<OHOS::FileManagement::CloudDiskJSManager>();
        vector<ohos::file::cloudDiskManager::SyncFolder> syncFolderRes = {};
        int32_t ret = syncFolderJSAccess->GetAllSyncFolders(result);
        if (ret != OHOS::ERR_OK) {
            HILOG_ERROR("GetAllSyncFolders fail, ret=%{public}d", ret);
            if (ret == OHOS::FileAccessFwk::E_NOT_SUPPORT) {
                ::taihe::set_business_error(ret, "The device doesn't support this api");
            } else {
                ::taihe::set_business_error(ret, "GetAllSyncFolders fail");
            }
            return {};
        }
        syncFolderRes.reserve(result.size());
        for (size_t i = 0; i < result.size(); i++) {
            if (result[i].path_.empty() || result[i].bundleName_.empty()) {
                HILOG_ERROR("folder path or bundleName is empty");
                continue;
            }
            ::ohos::file::cloudDiskManager::SyncFolder tmpFolderInfo = GetSyncFolder(result[i]);
            syncFolderRes.emplace_back(std::move(tmpFolderInfo));
        }
        if (syncFolderRes.empty()) {
            HILOG_ERROR("syncFolderRes is empty");
            return {};
        }
        return taihe::array<ohos::file::cloudDiskManager::SyncFolder>(
            taihe::copy_data_t{}, syncFolderRes.data(), syncFolderRes.size());
    }
};

::ohos::file::cloudDiskManager::SyncFolderAccessor CreateSyncFolderAccessor()
{
    return taihe::make_holder<SyncFolderAccessorImpl, ::ohos::file::cloudDiskManager::SyncFolderAccessor>();
}
}  // namespace

TH_EXPORT_CPP_API_CreateSyncFolderAccessor(CreateSyncFolderAccessor);