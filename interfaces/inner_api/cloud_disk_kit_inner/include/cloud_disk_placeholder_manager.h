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

#ifndef CLOUD_DISK_PLACEHOLDER_MANAGER_H
#define CLOUD_DISK_PLACEHOLDER_MANAGER_H

#include <string>

namespace OHOS {
namespace FileManagement {
class CloudDiskPlaceholderManager final {
public:
    explicit CloudDiskPlaceholderManager(const std::string &syncFolderPath);
    ~CloudDiskPlaceholderManager() = default;

    int IsPlaceholderFile(const std::string &relativePath, bool &isPlaceholder);

private:
    std::string syncFolderPath_;
};
} // namespace FileManagement
} // namespace OHOS

#endif // CLOUD_DISK_PLACEHOLDER_MANAGER_H
