/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef INTERFACES_KITS_NATIVE_CLOUDDISK_NAPI_SYNC_FOLDER_ACCESS_H
#define INTERFACES_KITS_NATIVE_CLOUDDISK_NAPI_SYNC_FOLDER_ACCESS_H

#include "filemgmt_libn.h"

namespace OHOS {
namespace FileManagement {
namespace CloudDiskService {
class SyncFolderAccessNExporter final : public LibN::NExporter {
public:
    inline static const std::string className_ = "SyncFolderAccessor";
    bool Export() override;
    std::string GetClassName() override;

    static napi_value Constructor(napi_env env, napi_callback_info cbinfo);
    static napi_value GetAllSyncFolders(napi_env env, napi_callback_info cbinfo);
    SyncFolderAccessNExporter(napi_env env, napi_value exports);
    ~SyncFolderAccessNExporter() override;
};
} // namespace CloudDiskService
} // namespace FileManagement
} // namespace OHOS
#endif // INTERFACES_KITS_NATIVE_CLOUDDISK_NAPI_SYNC_FOLDER_ACCESS_H
