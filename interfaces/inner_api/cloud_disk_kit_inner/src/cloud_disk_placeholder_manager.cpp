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

#include "cloud_disk_placeholder_manager.h"

#ifdef SUPPORT_CLOUD_DISK_MANAGER
#include "cloud_disk_service_manager.h"
#endif
#include "cloud_disk_error.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace FileManagement {

CloudDiskPlaceholderManager::CloudDiskPlaceholderManager(const std::string &syncFolderPath)
    : syncFolderPath_(syncFolderPath)
{
}

int CloudDiskPlaceholderManager::IsPlaceholderFile(const std::string &relativePath, bool &isPlaceholder)
{
    HILOG_INFO("CloudDiskPlaceholderManager::IsPlaceholderFile start");
#ifdef SUPPORT_CLOUD_DISK_MANAGER
    auto ret = FileManagement::CloudDiskService::CloudDiskServiceManager::GetInstance().
        IsPlaceholderFile(syncFolderPath_, relativePath, isPlaceholder);
    HILOG_INFO("CloudDiskPlaceholderManager::IsPlaceholderFile service ret=%{public}d", ret);
    return ret;
#endif
    HILOG_INFO("CloudDiskPlaceholderManager::IsPlaceholderFile unsupported");
    return E_NOT_SUPPORT;
}
} // namespace FileManagement
} // namespace OHOS
