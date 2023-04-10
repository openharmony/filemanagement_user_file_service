/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "filemgmt_libn.h"
#include "file_access_extension_info.h"
#include "file_access_notify_common.h"
#include "hilog_wrapper.h"
#include "js_native_api.h"
#include "napi/native_common.h"

namespace OHOS {
namespace FileAccessFwk {
using namespace FileManagement::LibN;

static napi_value CreateStringUtf8(napi_env env, const std::string &str)
{
    napi_value value = nullptr;
    if (napi_create_string_utf8(env, str.c_str(), str.length(), &value) != napi_ok) {
        HILOG_ERROR("CreateStringUtf8, value is not napi_ok");
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
        HILOG_ERROR("FileInfoConstructor, status is not napi_ok");
        return nullptr;
    }

    return res;
}

static napi_value RootInfoConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value args[1] = {0};
    napi_value res = nullptr;
    void *data = nullptr;

    napi_status status = napi_get_cb_info(env, info, &argc, args, &res, &data);
    if (status != napi_ok) {
        HILOG_ERROR("RootInfoConstructor, status is not napi_ok");
        return nullptr;
    }

    return res;
}

void InitDeviceFlag(napi_env env, napi_value exports)
{
    char propertyName[] = "DeviceFlag";
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("SUPPORTS_READ", NVal::CreateInt32(env, DEVICE_FLAG_SUPPORTS_READ).val_),
        DECLARE_NAPI_STATIC_PROPERTY("SUPPORTS_WRITE", NVal::CreateInt32(env, DEVICE_FLAG_SUPPORTS_WRITE).val_)
    };
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    napi_define_properties(env, obj, sizeof(desc) / sizeof(desc[0]), desc);
    napi_set_named_property(env, exports, propertyName, obj);
}

void InitDocumentFlag(napi_env env, napi_value exports)
{
    char propertyName[] = "DocumentFlag";
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("REPRESENTS_FILE", NVal::CreateInt32(env, DOCUMENT_FLAG_REPRESENTS_FILE).val_),
        DECLARE_NAPI_STATIC_PROPERTY("REPRESENTS_DIR", NVal::CreateInt32(env, DOCUMENT_FLAG_REPRESENTS_DIR).val_),
        DECLARE_NAPI_STATIC_PROPERTY("SUPPORTS_READ", NVal::CreateInt32(env, DOCUMENT_FLAG_SUPPORTS_READ).val_),
        DECLARE_NAPI_STATIC_PROPERTY("SUPPORTS_WRITE", NVal::CreateInt32(env, DOCUMENT_FLAG_SUPPORTS_WRITE).val_)
    };
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    napi_define_properties(env, obj, sizeof(desc) / sizeof(desc[0]), desc);
    napi_set_named_property(env, exports, propertyName, obj);
}

void InitNotifyType(napi_env env, napi_value exports)
{
    char propertyName[] = "NotifyType";
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_ONLINE", NVal::CreateInt32(env, NOTIFY_DEVICE_ONLINE).val_),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_OFFLINE", NVal::CreateInt32(env, NOTIFY_DEVICE_OFFLINE).val_)
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
        { "relativePath", nullptr, nullptr, nullptr, nullptr, CreateStringUtf8(env, "relativePath"),
            napi_writable, nullptr },
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
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_LOCAL_DISK", NVal::CreateInt32(env, DEVICE_LOCAL_DISK).val_),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_SHARED_DISK", NVal::CreateInt32(env, DEVICE_SHARED_DISK).val_),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_SHARED_TERMINAL", NVal::CreateInt32(env, DEVICE_SHARED_TERMINAL).val_),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_NETWORK_NEIGHBORHOODS",
            NVal::CreateInt32(env, DEVICE_NETWORK_NEIGHBORHOODS).val_),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_EXTERNAL_MTP", NVal::CreateInt32(env, DEVICE_EXTERNAL_MTP).val_),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_EXTERNAL_USB", NVal::CreateInt32(env, DEVICE_EXTERNAL_USB).val_),
        DECLARE_NAPI_STATIC_PROPERTY("DEVICE_EXTERNAL_CLOUD", NVal::CreateInt32(env, DEVICE_EXTERNAL_CLOUD).val_)
    };
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    napi_define_properties(env, obj, sizeof(desc) / sizeof(desc[0]), desc);
    napi_set_named_property(env, exports, propertyName, obj);
}

void InitRootInfo(napi_env env, napi_value exports)
{
    char className[] = "RootInfo";
    napi_property_descriptor desc[] = {
        { "deviceType", nullptr, nullptr, nullptr, nullptr, CreateStringUtf8(env, "deviceType"),
            napi_writable, nullptr },
        { "uri", nullptr, nullptr, nullptr, nullptr, CreateStringUtf8(env, "uri"), napi_writable, nullptr },
        { "relativePath", nullptr, nullptr, nullptr, nullptr, CreateStringUtf8(env, "relativePath"),
            napi_writable, nullptr },
        { "displayName", nullptr, nullptr, nullptr, nullptr, CreateStringUtf8(env, "displayName"),
            napi_writable, nullptr },
        { "deviceFlags", nullptr, nullptr, nullptr, nullptr, CreateStringUtf8(env, "deviceFlags"),
            napi_writable, nullptr }
    };
    napi_value obj = nullptr;
    napi_define_class(env, className, NAPI_AUTO_LENGTH, RootInfoConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &obj);
    napi_set_named_property(env, exports, className, obj);
}

void InitOpenFlags(napi_env env, napi_value exports)
{
    char propertyName[] = "OPENFLAGS";
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("READ", NVal::CreateInt32(env, READ).val_),
        DECLARE_NAPI_STATIC_PROPERTY("WRITE", NVal::CreateInt32(env, WRITE).val_),
        DECLARE_NAPI_STATIC_PROPERTY("WRITE_READ", NVal::CreateInt32(env, WRITE_READ).val_)
    };
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    napi_define_properties(env, obj, sizeof(desc) / sizeof(desc[0]), desc);
    napi_set_named_property(env, exports, propertyName, obj);
}

void InitQueryFlags(napi_env env, napi_value exports)
{
    char propertyName[] = "FileKey";
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("DISPLAY_NAME", NVal::CreateUTF8String(env, DISPLAY_NAME).val_),
        DECLARE_NAPI_STATIC_PROPERTY("DATE_ADDED", NVal::CreateUTF8String(env, DATE_ADDED).val_),
        DECLARE_NAPI_STATIC_PROPERTY("DATE_MODIFIED", NVal::CreateUTF8String(env, DATE_MODIFIED).val_),
        DECLARE_NAPI_STATIC_PROPERTY("RELATIVE_PATH", NVal::CreateUTF8String(env, RELATIVE_PATH).val_),
        DECLARE_NAPI_STATIC_PROPERTY("FILE_SIZE", NVal::CreateUTF8String(env, FILE_SIZE).val_),
        DECLARE_NAPI_STATIC_PROPERTY("WIDTH", NVal::CreateUTF8String(env, WIDTH).val_),
        DECLARE_NAPI_STATIC_PROPERTY("HEIGHT", NVal::CreateUTF8String(env, HEIGHT).val_),
        DECLARE_NAPI_STATIC_PROPERTY("DURATION", NVal::CreateUTF8String(env, DURATION).val_),
    };
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    napi_define_properties(env, obj, sizeof(desc) / sizeof(desc[0]), desc);
    napi_set_named_property(env, exports, propertyName, obj);
}

void InitCopyResult(napi_env env, napi_value exports)
{
    char className[] = "CopyResult";
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("sourceUri", NVal::CreateUTF8String(env, "sourceUri").val_),
        DECLARE_NAPI_STATIC_PROPERTY("destUri", NVal::CreateUTF8String(env, "destUri").val_),
        DECLARE_NAPI_STATIC_PROPERTY("errCode", NVal::CreateInt32(env, 0).val_),
        DECLARE_NAPI_STATIC_PROPERTY("errMsg", NVal::CreateUTF8String(env, "errMsg").val_),
    };
    napi_value obj = nullptr;
    napi_define_class(env, className, NAPI_AUTO_LENGTH, RootInfoConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &obj);
    napi_set_named_property(env, exports, className, obj);
}
} // namespace FileAccessFwk
} // namespace OHOS