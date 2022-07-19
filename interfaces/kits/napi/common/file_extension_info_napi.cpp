/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "file_extension_info_napi.h"

#include <cstddef>
#include <cstdint>
#include <string>

#include "file_access_extension_info.h"
#include "hilog_wrapper.h"
#include "js_native_api.h"
#include "napi/native_common.h"

namespace OHOS {
namespace FileAccessFwk {
static napi_value CreateStringUtf8(napi_env env, const std::string &str)
{
    napi_value value = nullptr;
    if (napi_create_string_utf8(env, str.c_str(), str.length(), &value) != napi_ok) {
        HILOG_ERROR("%{public}s, value is not napi_ok", __func__);
        return nullptr;
    }
    return value;
}

static napi_value CreateUint32(napi_env env, uint32_t val)
{
    napi_value value = nullptr;
    if (napi_create_uint32(env, val, &value) != napi_ok) {
        HILOG_ERROR("%{public}s, value is not napi_ok", __func__);
        return nullptr;
    }
    return value;
}

static napi_value FileInfoConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value args[1] = {0};
    napi_value res = nullptr;
    void *data = nullptr;

    napi_status status = napi_get_cb_info(env, info, &argc, args, &res, &data);
    if (status != napi_ok) {
        HILOG_ERROR("%{public}s, status is not napi_ok", __func__);
        return nullptr;
    }

    return res;
}

static napi_value DeviceInfoConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value args[1] = {0};
    napi_value res = nullptr;
    void *data = nullptr;

    napi_status status = napi_get_cb_info(env, info, &argc, args, &res, &data);
    if (status != napi_ok) {
        HILOG_ERROR("%{public}s, status is not napi_ok", __func__);
        return nullptr;
    }

    return res;
}

void InitFlag(napi_env env, napi_value exports)
{
    char propertyName[] = "FLAG";
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("SUPPORTS_THUMBNAIL", CreateUint32(env, FLAG_SUPPORTS_THUMBNAIL)),
        DECLARE_NAPI_STATIC_PROPERTY("SUPPORTS_WRITE", CreateUint32(env, FLAG_SUPPORTS_WRITE)),
        DECLARE_NAPI_STATIC_PROPERTY("SUPPORTS_READ", CreateUint32(env, FLAG_SUPPORTS_READ)),
        DECLARE_NAPI_STATIC_PROPERTY("SUPPORTS_DELETE", CreateUint32(env, FLAG_SUPPORTS_DELETE)),
        DECLARE_NAPI_STATIC_PROPERTY("SUPPORTS_RENAME", CreateUint32(env, FLAG_SUPPORTS_RENAME)),
        DECLARE_NAPI_STATIC_PROPERTY("SUPPORTS_MOVE", CreateUint32(env, FLAG_SUPPORTS_MOVE))
    };
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    napi_define_properties(env, obj, sizeof(desc) / sizeof(desc[0]), desc);
    napi_set_named_property(env, exports, propertyName, obj);
}

void InitFileInfo(napi_env env, napi_value exports)
{
    char className[] = "FileInfo";
    napi_property_descriptor desc[] = {
        { "uri", nullptr, nullptr, nullptr, nullptr, CreateStringUtf8(env, "uri"), napi_writable, nullptr },
        { "fileName", nullptr, nullptr, nullptr, nullptr, CreateStringUtf8(env, "fileName"), napi_writable, nullptr },
        { "mode", nullptr, nullptr, nullptr, nullptr, CreateStringUtf8(env, "mode"), napi_writable, nullptr },
        { "size", nullptr, nullptr, nullptr, nullptr, CreateStringUtf8(env, "size"), napi_writable, nullptr },
        { "mtime", nullptr, nullptr, nullptr, nullptr, CreateStringUtf8(env, "mtime"), napi_writable, nullptr },
        { "mimiType", nullptr, nullptr, nullptr, nullptr, CreateStringUtf8(env, "mimiType"), napi_writable, nullptr }
    };
    napi_value obj = nullptr;
    napi_define_class(env, className, NAPI_AUTO_LENGTH, FileInfoConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &obj);
    napi_set_named_property(env, exports, className, obj);
}

void InitDeviceType(napi_env env, napi_value exports)
{
    char propertyName[] = "DeviceType";
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("LOCAL_DISK", CreateUint32(env, DEVICE_LOCAL_DISK)),
        DECLARE_NAPI_STATIC_PROPERTY("SHARED_DISK", CreateUint32(env, DEVICE_SHARED_DISK)),
        DECLARE_NAPI_STATIC_PROPERTY("SHARED_TERMINAL", CreateUint32(env, DEVICE_SHARED_TERMINAL)),
        DECLARE_NAPI_STATIC_PROPERTY("NETWORK_NEIGHBORHOODS", CreateUint32(env, DEVICE_NETWORK_NEIGHBORHOODS)),
        DECLARE_NAPI_STATIC_PROPERTY("EXTERNAL_MTP", CreateUint32(env, DEVICE_EXTERNAL_MTP)),
        DECLARE_NAPI_STATIC_PROPERTY("EXTERNAL_USB", CreateUint32(env, DEVICE_EXTERNAL_USB)),
        DECLARE_NAPI_STATIC_PROPERTY("EXTERNAL_CLOUD", CreateUint32(env, DEVICE_EXTERNAL_CLOUD))
    };
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    napi_define_properties(env, obj, sizeof(desc) / sizeof(desc[0]), desc);
    napi_set_named_property(env, exports, propertyName, obj);
}

void InitDeviceInfo(napi_env env, napi_value exports)
{
    char className[] = "DeviceInfo";
    napi_property_descriptor desc[] = {
        { "uri", nullptr, nullptr, nullptr, nullptr, CreateStringUtf8(env, "uri"), napi_writable, nullptr },
        { "displayName", nullptr, nullptr, nullptr, nullptr,
            CreateStringUtf8(env, "displayName"), napi_writable, nullptr },
        { "deviceId", nullptr, nullptr, nullptr, nullptr, CreateStringUtf8(env, "deviceId"), napi_writable, nullptr },
        { "flags", nullptr, nullptr, nullptr, nullptr, CreateStringUtf8(env, "flags"), napi_writable, nullptr },
        { "type", nullptr, nullptr, nullptr, nullptr, CreateStringUtf8(env, "type"), napi_writable, nullptr }
    };
    napi_value obj = nullptr;
    napi_define_class(env, className, NAPI_AUTO_LENGTH, DeviceInfoConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &obj);
    napi_set_named_property(env, exports, className, obj);
}
} // namespace FileAccessFwk
} // namespace OHOS