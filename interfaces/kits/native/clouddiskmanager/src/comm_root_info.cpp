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

#include "comm_root_info.h"

#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"

namespace OHOS::FileManagement::CloudDiskService {
using namespace LibN;

constexpr const char* PATH = "path";
constexpr const char* STATE = "state";
constexpr const char* DISPLAY_NAME_RES_ID = "displayNameResId";
constexpr const char* CUSTOM_ALIAS = "customAlias";
constexpr const char* BUNDLE_NAME = "bundleName";
constexpr const char* ACTIVE = "ACTIVE";
constexpr const char* INACTIVE = "INACTIVE";

enum class State {
    INACTIVE = 0,
    ACTIVE,
};

static napi_value SyncFolderConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value args[1] = {0}; // 1: argc
    napi_value res = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, args, &res, &data);
    if (status != napi_ok) {
        HILOG_ERROR("SyncFolderConstructor, status is not napi_ok");
        return nullptr;
    }

    return res;
}

void InitSyncFolder(napi_env env, napi_value exports)
{
    char className[] = "SyncFolder";
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_PROPERTY(PATH, NVal::CreateUTF8String(env, "").val_),
        DECLARE_NAPI_PROPERTY(STATE, NVal::CreateInt32(env, static_cast<int32_t>(State::INACTIVE)).val_),
        DECLARE_NAPI_PROPERTY(DISPLAY_NAME_RES_ID, NVal::CreateInt32(env, 0).val_),
        DECLARE_NAPI_PROPERTY(CUSTOM_ALIAS, NVal::CreateUTF8String(env, "").val_),
        DECLARE_NAPI_PROPERTY(BUNDLE_NAME, NVal::CreateUTF8String(env, "").val_)
    };
    napi_value obj = nullptr;
    napi_define_class(env, className, NAPI_AUTO_LENGTH, SyncFolderConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &obj);
    napi_set_named_property(env, exports, className, obj);
}

void InitState(napi_env env, napi_value exports)
{
    char propertyName[] = "SyncFolderState";
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(ACTIVE, NVal::CreateInt32(env, static_cast<int32_t>(State::ACTIVE)).val_),
        DECLARE_NAPI_STATIC_PROPERTY(INACTIVE, NVal::CreateInt32(env, static_cast<int32_t>(State::INACTIVE)).val_)
    };
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    napi_define_properties(env, obj, sizeof(desc) / sizeof(desc[0]), desc);
    napi_set_named_property(env, exports, propertyName, obj);
}
} // namespace OHOS::FileManagement::CloudDiskService