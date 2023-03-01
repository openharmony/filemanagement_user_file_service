/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "napi_notify_callback.h"

#include <memory>

#include "n_val.h"
#include "uv.h"

using namespace OHOS::FileManagement::LibN;
namespace OHOS {
namespace FileAccessFwk {
namespace {
    const int ARGS_ONE = 1;
}

NapiNotifyCallback::NapiNotifyCallback(napi_env env, napi_value callback)
{
    env_ = env;
    napi_create_reference(env_, callback, 1, &callback_);
    napi_get_uv_event_loop(env_, &loop_);
}

NapiNotifyCallback::~NapiNotifyCallback()
{
    napi_delete_reference(env_, callback_);
}

int NapiNotifyCallback::OnNotify(const NotifyMessage& message)
{
    uv_work_t* work = new uv_work_t();
    if (work == nullptr) {
        HILOG_ERROR("failed to new uv_work_t");
        return EINVAL;
    }
    CallbackParam* param = new CallbackParam(this, message);
    if (param == nullptr) {
        HILOG_ERROR("failed to new param");
        delete work;
        return EINVAL;
    }
    work->data = param;
    int ret = uv_queue_work(loop_, work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            CallbackParam *param = reinterpret_cast<CallbackParam *>(work->data);
            NVal napiNotifyMessage = NVal::CreateObject(param->callback_->env_);
            napiNotifyMessage.AddProp("deviceType",
                NVal::CreateInt32(param->callback_->env_, param->message_.deviceType).val_);
            napiNotifyMessage.AddProp("notifyType",
                NVal::CreateInt32(param->callback_->env_, param->message_.notifyType).val_);
            napiNotifyMessage.AddProp("srcUri",
                NVal::CreateUTF8String(param->callback_->env_, param->message_.srcUri).val_);
            napiNotifyMessage.AddProp("dstUri",
                NVal::CreateUTF8String(param->callback_->env_, param->message_.dstUri).val_);

            napi_value callback = nullptr;
            napi_value args[ARGS_ONE] = {napiNotifyMessage.val_};
            napi_get_reference_value(param->callback_->env_, param->callback_->callback_, &callback);
            napi_value global = nullptr;
            napi_get_global(param->callback_->env_, &global);
            napi_value result = nullptr;
            napi_status ret = napi_call_function(param->callback_->env_, global, callback, ARGS_ONE, args, &result);
            if (ret != napi_ok) {
                HILOG_ERROR("Notify failed, status:%{public}d.", ret);
            }
            delete param;
            delete work;
        });
    if (ret != 0) {
        delete (CallbackParam *)(work->data);
        work->data = nullptr;
        delete work;
        work = nullptr;
        return E_NOTIFY;
    }

    return ERR_OK;
}
} // namespace FileAccessFwk
} // namespace OHOS