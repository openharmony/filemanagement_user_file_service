/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "napi_observer_callback.h"

#include <memory>

#include "n_val.h"
#include "uv.h"

using namespace OHOS::FileManagement::LibN;
namespace OHOS {
namespace FileAccessFwk {
namespace {
    const int ARGS_ONE = 1;
}

NapiObserver::NapiObserver(napi_env env, napi_value callback)
{
    env_ = env;
    napi_status status = napi_create_reference(env_, callback, ARGS_ONE, &cbOnRef_);
    if (status != napi_ok) {
        HILOG_ERROR("create reference failed");
    }
}

NapiObserver::~NapiObserver()
{
    napi_status status = napi_delete_reference(env_, cbOnRef_);
    if (status != napi_ok) {
        HILOG_ERROR("delete reference failed");
    }
}

static napi_status SetValueArray(const napi_env& env,
    const char* fieldStr, const std::vector<std::string> listValue, napi_value& result)
{
    napi_value value = nullptr;
    napi_status status = napi_create_array_with_length(env, listValue.size(), &value);
    if (status != napi_ok) {
        HILOG_ERROR("Create array error! field: %{public}s", fieldStr);
        return status;
    }
    int elementIndex = 0;
    for (auto uri : listValue) {
        napi_value uriRet = nullptr;
        napi_create_string_utf8(env, uri.c_str(), NAPI_AUTO_LENGTH, &uriRet);
        status = napi_set_element(env, value, elementIndex++, uriRet);
        if (status != napi_ok) {
            HILOG_ERROR("Set lite item failed, error: %{public}d", status);
            break;
        }
    }
    status = napi_set_named_property(env, result, fieldStr, value);
    if (status != napi_ok) {
        HILOG_ERROR("Set array named property error! field: %{public}s", fieldStr);
    }

    return status;
}

void NapiObserver::NapiWorkScope(uv_work_t *work, int status)
{
    if (work == nullptr) {
        HILOG_ERROR("Work is nullptr");
        return;
    }
    std::unique_ptr<CallbackParam> param(reinterpret_cast<CallbackParam *>(work->data));
    napi_handle_scope scope = nullptr;
    if (param == nullptr) {
        HILOG_ERROR("Param is nullptr");
        return;
    }
    if (param->napiObserver == nullptr) {
        HILOG_ERROR("param->napiObserver is nullptr");
        return;
    }
    napi_open_handle_scope(param->napiObserver->env_, &scope);
    if (scope == nullptr) {
        HILOG_ERROR("napi_open_handle_scope failed");
        return;
    }

    NVal napiNotifyMessage = NVal::CreateObject(param->napiObserver->env_);
    napiNotifyMessage.AddProp("type",
        NVal::CreateInt32(param->napiObserver->env_, int((param->iNotifyMessage).notifyType_)).val_);
    SetValueArray(param->napiObserver->env_, "uris", param->iNotifyMessage.uris_, napiNotifyMessage.val_);

    napi_value callback = nullptr;
    napi_value args[ARGS_ONE] = {napiNotifyMessage.val_};
    napi_status ret = napi_get_reference_value(param->napiObserver->env_, param->napiObserver->cbOnRef_,
        &callback);
    if (ret != napi_ok) {
        HILOG_ERROR("Notify get reference failed, status:%{public}d.", status);
        napi_close_handle_scope(param->napiObserver->env_, scope);
        return;
    }
    napi_value global = nullptr;
    napi_get_global(param->napiObserver->env_, &global);
    napi_value result = nullptr;
    ret = napi_call_function(param->napiObserver->env_, global, callback, ARGS_ONE, args, &result);
    if (ret != napi_ok) {
        HILOG_ERROR("Notify failed, status:%{public}d.", ret);
        napi_close_handle_scope(param->napiObserver->env_, scope);
        return;
    }
    napi_close_handle_scope(param->napiObserver->env_, scope);
}

void NapiObserver::OnChange(const NotifyMessage &notifyMessage)
{
    uv_loop_s *loop = nullptr;
    napi_status napiStatus = napi_get_uv_event_loop(env_, &loop);
    if (napiStatus != napi_ok || loop == nullptr) {
        HILOG_ERROR("napi_get_uv_event_loop failed");
        return;
    }
    if (work_ == nullptr) {
        work_ = std::make_unique<uv_work_t>();
    }

    if (work_ == nullptr) {
        HILOG_ERROR("napi_get_work failed");
        return;
    }
    std::unique_ptr<CallbackParam> callbackParam = std::make_unique<CallbackParam>(this, notifyMessage);
    work_->data = callbackParam.get();

    auto task = [work {work_.get()}]() {
        NapiWorkScope(work, 0);
    };
    auto ret = napi_send_event(env_, task, napi_eprio_high);
    if (ret != napi_ok) {
        HILOG_ERROR("failed to napi_send_event, ret:%{public}d.", ret);
        return;
    }

    callbackParam.release();
    work_.release();
}
} // namespace FileAccessFwk
} // namespace OHOS
