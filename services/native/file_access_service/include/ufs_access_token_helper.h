/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_UFS_ACCESS_TOKEN_HELPER_H
#define OHOS_UFS_ACCESS_TOKEN_HELPER_H

#include <string>

namespace OHOS::FileAccessFwk {
class UfsAccessTokenHelper final {
public:
    static bool CheckCallerPermission(const std::string &permissionName);
    static int32_t GetCallerBundleNameAndIndex(std::string &bundleName, int &index);
    static bool CheckPermission(uint32_t tokenId, const std::string &permissionName);
    static int32_t GetBundleNameByToken(uint32_t tokenId, std::string &bundleName, int &index);
    static int32_t GetUserId();
    static bool PathToPhysicalPath(const std::string& path, const std::string& userId,
        std::string& physicalPath);
    static std::string ReplacePlaceholders(const std::string& path, const std::string& userId);
    static bool CheckPathPermission(const std::string& path);
};
} // namespace OHOS::FileAccessFwk

#endif // OHOS_UFS_ACCESS_TOKEN_HELPER_H