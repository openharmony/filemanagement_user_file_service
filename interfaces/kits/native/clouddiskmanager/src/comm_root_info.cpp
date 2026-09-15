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
constexpr const char* IS_SUPPORT_PLACE_HOLDER = "isSupportPlaceHolder";
constexpr const char* ACTIVE = "ACTIVE";
constexpr const char* INACTIVE = "INACTIVE";
constexpr const char* LOW = "LOW";
constexpr const char* NORMAL = "NORMAL";
constexpr const char* HIGH = "HIGH";
constexpr const char* FETCH_DATA = "FETCH_DATA";
constexpr const char* CANCEL_FETCH_DATA = "CANCEL_FETCH_DATA";
constexpr const char* DEHYDRATE = "DEHYDRATE";
constexpr const char* PENDING = "PENDING";
constexpr const char* IN_PROGRESS = "IN_PROGRESS";
constexpr const char* COMPLETED = "COMPLETED";
constexpr const char* CANCELLED = "CANCELLED";

namespace {
enum class State {
    INACTIVE = 0,
    ACTIVE,
};

enum class HydratePriority {
    LOW = 0,
    NORMAL,
    HIGH,
};

enum class CallbackType {
    FETCH_DATA = 0,
    CANCEL_FETCH_DATA,
    DEHYDRATE = 2,
};

enum class HydrateProgressState {
    PENDING = 0,
    IN_PROGRESS,
    COMPLETED,
    CANCELLED,
};
} // namespace

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

static void InitSyncFolder(napi_env env, napi_value exports)
{
    char className[] = "SyncFolder";
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_PROPERTY(PATH, NVal::CreateUTF8String(env, "").val_),
        DECLARE_NAPI_PROPERTY(STATE, NVal::CreateInt32(env, static_cast<int32_t>(State::INACTIVE)).val_),
        DECLARE_NAPI_PROPERTY(DISPLAY_NAME_RES_ID, NVal::CreateInt32(env, 0).val_),
        DECLARE_NAPI_PROPERTY(CUSTOM_ALIAS, NVal::CreateUTF8String(env, "").val_),
        DECLARE_NAPI_PROPERTY(BUNDLE_NAME, NVal::CreateUTF8String(env, "").val_),
        DECLARE_NAPI_PROPERTY(IS_SUPPORT_PLACE_HOLDER, NVal::CreateBool(env, false).val_)
    };
    napi_value obj = nullptr;
    napi_define_class(env, className, NAPI_AUTO_LENGTH, SyncFolderConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &obj);
    napi_set_named_property(env, exports, className, obj);
}

static void InitState(napi_env env, napi_value exports)
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

static void InitHydratePriority(napi_env env, napi_value exports)
{
    char propertyName[] = "HydratePriority";
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(LOW, NVal::CreateInt32(env, static_cast<int32_t>(HydratePriority::LOW)).val_),
        DECLARE_NAPI_STATIC_PROPERTY(NORMAL,
            NVal::CreateInt32(env, static_cast<int32_t>(HydratePriority::NORMAL)).val_),
        DECLARE_NAPI_STATIC_PROPERTY(HIGH, NVal::CreateInt32(env, static_cast<int32_t>(HydratePriority::HIGH)).val_)
    };
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    napi_define_properties(env, obj, sizeof(desc) / sizeof(desc[0]), desc);
    napi_set_named_property(env, exports, propertyName, obj);
}

static void InitCallbackType(napi_env env, napi_value exports)
{
    char propertyName[] = "CallbackType";
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(FETCH_DATA,
            NVal::CreateInt32(env, static_cast<int32_t>(CallbackType::FETCH_DATA)).val_),
        DECLARE_NAPI_STATIC_PROPERTY(CANCEL_FETCH_DATA,
            NVal::CreateInt32(env, static_cast<int32_t>(CallbackType::CANCEL_FETCH_DATA)).val_),
        DECLARE_NAPI_STATIC_PROPERTY(DEHYDRATE,
            NVal::CreateInt32(env, static_cast<int32_t>(CallbackType::DEHYDRATE)).val_)
    };
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    napi_define_properties(env, obj, sizeof(desc) / sizeof(desc[0]), desc);
    napi_set_named_property(env, exports, propertyName, obj);
}

static void InitHydrateProgressState(napi_env env, napi_value exports)
{
    char propertyName[] = "HydrateProgressState";
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(PENDING,
            NVal::CreateInt32(env, static_cast<int32_t>(HydrateProgressState::PENDING)).val_),
        DECLARE_NAPI_STATIC_PROPERTY(IN_PROGRESS,
            NVal::CreateInt32(env, static_cast<int32_t>(HydrateProgressState::IN_PROGRESS)).val_),
        DECLARE_NAPI_STATIC_PROPERTY(COMPLETED,
            NVal::CreateInt32(env, static_cast<int32_t>(HydrateProgressState::COMPLETED)).val_),
        DECLARE_NAPI_STATIC_PROPERTY(CANCELLED,
            NVal::CreateInt32(env, static_cast<int32_t>(HydrateProgressState::CANCELLED)).val_)
    };
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    napi_define_properties(env, obj, sizeof(desc) / sizeof(desc[0]), desc);
    napi_set_named_property(env, exports, propertyName, obj);
}

void InitCommonRootInfo(napi_env env, napi_value exports)
{
    InitSyncFolder(env, exports);
    InitState(env, exports);
    InitHydratePriority(env, exports);
    InitCallbackType(env, exports);
    InitHydrateProgressState(env, exports);
}
} // namespace OHOS::FileManagement::CloudDiskService
