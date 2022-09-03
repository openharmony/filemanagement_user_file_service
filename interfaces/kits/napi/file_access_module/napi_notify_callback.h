/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NAPI_NOTIFY_CALLBACK_H
#define NAPI_NOTIFY_CALLBACK_H

#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "inotify_callback.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "napi/native_api.h"
#include "uri.h"

namespace OHOS {
namespace FileAccessFwk {
class NapiNotifyCallback : public INotifyCallback {
public:
    NapiNotifyCallback(napi_env env, napi_value callback);
    virtual ~NapiNotifyCallback();
    int OnNotify(const NotifyMessage &message) override;

private:
    struct CallbackParam {
        NapiNotifyCallback *callback_;
        NotifyMessage message_;
        CallbackParam(NapiNotifyCallback *callback, NotifyMessage message)
            : callback_(callback), message_(message)
        {}
    };
    napi_ref callback_ = nullptr;
    napi_env env_;
    uv_loop_s *loop_ = nullptr;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // NAPI_NOTIFY_CALLBACK_H