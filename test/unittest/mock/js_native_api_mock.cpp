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

#include "assistant.h"
#include "uv.h"

int uv_queue_work(uv_loop_t* loop, uv_work_t* req, uv_work_cb work_cb, uv_after_work_cb after_work_cb)
{
    if (work_cb) {
        work_cb(req);
    }
    if (after_work_cb) {
        after_work_cb(req, 0);
    }
    return OHOS::FileAccessFwk::Assistant::ins_->uv_queue_work(loop, req, work_cb, after_work_cb);
}

napi_status napi_get_uv_event_loop(napi_env env, struct uv_loop_s** loop)
{
    return OHOS::FileAccessFwk::Assistant::ins_->napi_get_uv_event_loop(env, loop);
}

napi_status napi_call_function(napi_env env, napi_value recv, napi_value func, size_t argc, const napi_value* argv,
    napi_value* result)
{
    return OHOS::FileAccessFwk::Assistant::ins_->napi_call_function(env, recv, func, argc, argv, result);
}

napi_status napi_get_reference_value(napi_env env, napi_ref ref, napi_value* result)
{
    return OHOS::FileAccessFwk::Assistant::ins_->napi_get_reference_value(env, ref, result);
}

napi_status napi_get_named_property(napi_env env, napi_value object, const char* utf8name, napi_value* result)
{
    return OHOS::FileAccessFwk::Assistant::ins_->napi_get_named_property(env, object, utf8name, result);
}

napi_status napi_get_value_int32(napi_env env, napi_value value, int32_t* result)
{
    return OHOS::FileAccessFwk::Assistant::ins_->napi_get_value_int32(env, value, result);
}

napi_status napi_create_string_utf8(napi_env env, const char* str, size_t length, napi_value* result)
{
    return OHOS::FileAccessFwk::Assistant::ins_->napi_create_string_utf8(env, str, length, result);
}

napi_status napi_create_int32(napi_env env, int32_t value, napi_value* result)
{
    return OHOS::FileAccessFwk::Assistant::ins_->napi_create_int32(env, value, result);
}

napi_status napi_get_value_string_utf8(napi_env env, napi_value value, char* buf, size_t bufsize, size_t* result)
{
    return OHOS::FileAccessFwk::Assistant::ins_->napi_get_value_string_utf8(env, value, buf, bufsize, result);
}

napi_status napi_get_boolean(napi_env env, bool value, napi_value* result)
{
    return OHOS::FileAccessFwk::Assistant::ins_->napi_get_boolean(env, value, result);
}

napi_status napi_create_array(napi_env env, napi_value* result)
{
    return OHOS::FileAccessFwk::Assistant::ins_->napi_create_array(env, result);
}

napi_status napi_get_array_length(napi_env env, napi_value value, uint32_t* result)
{
    return OHOS::FileAccessFwk::Assistant::ins_->napi_get_array_length(env, value, result);
}

napi_status napi_get_element(napi_env env, napi_value object, uint32_t index, napi_value* result)
{
    return OHOS::FileAccessFwk::Assistant::ins_->napi_get_element(env, object, index, result);
}

napi_status napi_escape_handle(napi_env env, napi_escapable_handle_scope scope, napi_value escapee, napi_value* result)
{
    return OHOS::FileAccessFwk::Assistant::ins_->napi_escape_handle(env, scope, escapee, result);
}