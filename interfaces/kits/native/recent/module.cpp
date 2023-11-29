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

#include <memory>
#include <vector>

#include "filemgmt_libn.h"
#include "hilog_wrapper.h"
#include "recent_n_exporter.h"
#include "os_account_manager.h"
#include "parameter.h"

namespace OHOS::FileManagement::Recent {
using namespace std;
using namespace LibN;

static bool GetDeviceType(std::string &deviceType)
{
    char deviceTypeChar[PARAM_CONST_VALUE_LEN_MAX];
    int32_t ret = GetParameter("const.product.devicetype", "0", deviceTypeChar, PARAM_CONST_VALUE_LEN_MAX);
    if (ret < 0) {
        HILOG_ERROR("Get deviceType fail. %{public}d", ret);
        return false;
    }
    deviceType = deviceTypeChar;
    return true;
}

static bool GetUserName(std::string &userName)
{
    ErrCode errCode = OHOS::AccountSA::OsAccountManager::GetOsAccountShortName(userName);
    if (errCode != ERR_OK) {
        HILOG_ERROR("GetOsAccountShortName fail.");
        return false;
    }
    return true;
}

static std::string GetRecentDir()
{
    std::string result = "/storage/Users/currentUser/.Recent";
    std::string deviceType;
    if (GetDeviceType(deviceType) && deviceType == "2in1") {
        std::string userName;
        if (GetUserName(userName)) {
            result = "/storage/Users/" + userName + "/.Recent";
        }
    }
    HILOG_INFO("GetRecentDir %{public}s", result.c_str());
    return result;
}

static napi_value Export(napi_env env, napi_value exports)
{
    std::vector<unique_ptr<NExporter>> products;
    products.emplace_back(make_unique<RecentNExporter>(env, exports));
    for (auto &&product : products) {
        if (!product->Export()) {
            HILOG_ERROR("INNER BUG. Failed to export class %{public}s for module recent",
                product->GetClassName().c_str());
            return nullptr;
        } else {
            HILOG_INFO("Class %{public}s for module fileio has been exported", product->GetClassName().c_str());
        }
    }

    RecentNExporter::recentPath_ = GetRecentDir();
    return exports;
}

static napi_module _module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Export,
    .nm_modname = "file.recent",
    .nm_priv = ((void *)0),
    .reserved = {0}
};

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&_module);
}
} // namespace OHOS::AppFileService::ModuleFileUri