/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef INTERFACES_KITS_NATIVE_RECENT_RECENT_N_EXPOTER_H
#define INTERFACES_KITS_NATIVE_RECENT_RECENT_N_EXPOTER_H

#include <dirent.h>

#include "filemgmt_libn.h"

namespace OHOS {
namespace FileManagement {
namespace Recent {
struct NameListArg {
    struct dirent** namelist = { nullptr };
    int direntNum = 0;
};

class RecentNExporter final : public LibN::NExporter {
public:
    inline static const std::string className = "Recent";

    bool Export() override;
    std::string GetClassName() override;

    static napi_value AddRecentFile(napi_env env, napi_callback_info cbinfo);
    static napi_value RemoveRecentFile(napi_env env, napi_callback_info cbinfo);
    static napi_value ListRecentFile(napi_env env, napi_callback_info cbinfo);

    RecentNExporter(napi_env env, napi_value exports);
    ~RecentNExporter() override;
};

const std::string RECENT_PATH = "/storage/Users/.Recent/";
const std::string FILE_ACCESS_PERMISSION = "ohos.permission.FILE_ACCESS_MANAGER";
constexpr int BUF_SIZE = 1024;
constexpr int MAX_RECENT_SIZE = 100;
} // namespace RecentNExporter
} // namespace FileManagement
} // namespace OHOS
#endif // INTERFACES_KITS_JS_RECENT_RECENT_N_EXPOTER_H
