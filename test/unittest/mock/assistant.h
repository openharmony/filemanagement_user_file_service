/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef TEST_UNITTEST_MOCK_FILE_ASSISTANT_H
#define TEST_UNITTEST_MOCK_FILE_ASSISTANT_H

#include <gmock/gmock.h>

#ifdef FILE_SUBSYSTEM_DEBUG_LOCAL
#include <node_api.h>
#else
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#endif
#include "uv.h"

namespace OHOS {
namespace FileAccessFwk {
class Assistant {
public:
    virtual ~Assistant() = default;
    virtual bool Bool() = 0;
    virtual int Int() = 0;
public:
    virtual napi_status napi_get_uv_event_loop(napi_env, struct uv_loop_s**) = 0;
    virtual napi_status napi_call_function(napi_env, napi_value, napi_value, size_t, const napi_value*, napi_value*)
        = 0;
    virtual napi_status napi_get_reference_value(napi_env, napi_ref, napi_value*) = 0;
    virtual napi_status napi_get_named_property(napi_env, napi_value, const char*, napi_value*) = 0;
    virtual int uv_queue_work(uv_loop_t*, uv_work_t*, uv_work_cb, uv_after_work_cb) = 0;
    virtual napi_status napi_get_value_int32(napi_env, napi_value, int32_t*) = 0;
    virtual napi_status napi_get_value_int64(napi_env, napi_value, int64_t*) = 0;
    virtual napi_status napi_create_string_utf8(napi_env, const char*, size_t, napi_value*) = 0;
    virtual napi_status napi_create_int32(napi_env, int32_t, napi_value*) = 0;
    virtual napi_status napi_create_int64(napi_env, int64_t, napi_value*) = 0;
    virtual napi_status napi_get_value_string_utf8(napi_env, napi_value, char*, size_t, size_t*) = 0;
    virtual napi_status napi_get_boolean(napi_env, bool, napi_value*) = 0;
    virtual napi_status napi_create_array(napi_env, napi_value*) = 0;
    virtual napi_status napi_get_array_length(napi_env, napi_value, uint32_t*) = 0;
    virtual napi_status napi_get_element(napi_env, napi_value, uint32_t, napi_value*) = 0;
    virtual napi_status napi_escape_handle(napi_env, napi_escapable_handle_scope, napi_value, napi_value*) = 0;
    virtual napi_status napi_get_null(napi_env, napi_value*) = 0;
    virtual napi_status napi_create_object(napi_env, napi_value*) = 0;
    virtual napi_status napi_create_array_with_length(napi_env, size_t, napi_value*) = 0;
    virtual napi_status napi_create_double(napi_env, double, napi_value*) = 0;
    virtual napi_status napi_set_named_property(napi_env, napi_value, const char*, napi_value) = 0;
    virtual napi_status napi_create_function(napi_env, const char*, size_t, napi_callback, void*, napi_value*) = 0;
    virtual napi_status napi_get_cb_info(napi_env, napi_callback_info, size_t*, napi_value*, napi_value*, void**) = 0;
    virtual napi_status napi_get_undefined(napi_env, napi_value*) = 0;
    virtual napi_status napi_get_value_bool(napi_env, napi_value, bool*) = 0;
public:
    static inline std::shared_ptr<Assistant> ins_ = nullptr;
};

class AssistantMock : public Assistant {
public:
    MOCK_METHOD0(Bool, bool());
    MOCK_METHOD0(Int, int());
public:
    MOCK_METHOD2(napi_get_uv_event_loop, napi_status(napi_env, struct uv_loop_s**));
    MOCK_METHOD6(napi_call_function, napi_status(napi_env, napi_value, napi_value, size_t, const napi_value*,
        napi_value*));
    MOCK_METHOD3(napi_get_reference_value, napi_status(napi_env, napi_ref, napi_value*));
    MOCK_METHOD4(napi_get_named_property, napi_status(napi_env, napi_value, const char*, napi_value*));
    MOCK_METHOD4(uv_queue_work, int(uv_loop_t*, uv_work_t*, uv_work_cb, uv_after_work_cb));
    MOCK_METHOD3(napi_get_value_int32, napi_status(napi_env, napi_value, int32_t*));
    MOCK_METHOD3(napi_get_value_int64, napi_status(napi_env, napi_value, int64_t*));
    MOCK_METHOD3(napi_create_int64, napi_status(napi_env, int64_t, napi_value*));
    MOCK_METHOD4(napi_create_string_utf8, napi_status(napi_env, const char*, size_t, napi_value*));
    MOCK_METHOD3(napi_create_int32, napi_status(napi_env, int32_t, napi_value*));
    MOCK_METHOD5(napi_get_value_string_utf8, napi_status(napi_env, napi_value, char*, size_t, size_t*));
    MOCK_METHOD3(napi_get_boolean, napi_status(napi_env, bool, napi_value*));
    MOCK_METHOD2(napi_create_array, napi_status(napi_env, napi_value*));
    MOCK_METHOD3(napi_get_array_length, napi_status(napi_env, napi_value, uint32_t*));
    MOCK_METHOD4(napi_get_element, napi_status(napi_env, napi_value, uint32_t, napi_value*));
    MOCK_METHOD4(napi_escape_handle, napi_status(napi_env, napi_escapable_handle_scope, napi_value, napi_value*));
    MOCK_METHOD2(napi_get_null, napi_status(napi_env, napi_value*));
    MOCK_METHOD2(napi_create_object, napi_status(napi_env, napi_value*));
    MOCK_METHOD3(napi_create_array_with_length, napi_status(napi_env, size_t, napi_value*));
    MOCK_METHOD3(napi_create_double, napi_status(napi_env, double, napi_value*));
    MOCK_METHOD4(napi_set_named_property, napi_status(napi_env, napi_value, const char*, napi_value));
    MOCK_METHOD6(napi_create_function, napi_status(napi_env, const char*, size_t, napi_callback, void*, napi_value*));
    MOCK_METHOD6(napi_get_cb_info, napi_status(napi_env, napi_callback_info, size_t*, napi_value*, napi_value*,
        void**));
    MOCK_METHOD2(napi_get_undefined, napi_status(napi_env, napi_value*));
    MOCK_METHOD3(napi_get_value_bool, napi_status(napi_env, napi_value, bool*));
};

} // End of namespace FileAccessFwk
} // End of namespace OHOS
#endif // TEST_UNITTEST_MOCK_FILE_ASSISTANT_H