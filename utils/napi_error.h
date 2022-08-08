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

#ifndef NAPI_ERROR_H
#define NAPI_ERROR_H

#include "hilog_wrapper.h"
#include "n_napi.h"
#include "n_error.h"

namespace OHOS {
namespace FileAccessFwk {
namespace {
    constexpr int ERRNO_NOERR = 0;
}
class NapiError : public OHOS::FileManagement::LibN::NError {
public:
    NapiError(int ePosix) : errno_(ePosix) {}
    ~NapiError() = default;
    void ThrowErr(napi_env env)
    {
        napi_value tmp = nullptr;
        std::string errMsg = "";
        napi_get_and_clear_last_exception(env, &tmp);
        // Note that ace engine cannot thow errors created by napi_create_error so far
        napi_status throwStatus = napi_throw_error(env, std::to_string(errno_).c_str(), errMsg.c_str());
        if (throwStatus != napi_ok) {
            HILOG_WARN("Failed to throw an exception, %{public}d, code = %{public}s", throwStatus, std::to_string(errno_).c_str());
        }
    }
private:
    int errno_ = ERRNO_NOERR;
};
}
}
#endif