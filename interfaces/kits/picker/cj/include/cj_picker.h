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
#ifndef INTERFACES_KITS_CJ_PICKER_H
#define INTERFACES_KITS_CJ_PICKER_H

#include <cstdint>
#include <dirent.h>
#include <thread>
#include <string>
#include "cj_lambda.h"
#include "cj_common_ffi.h"
#include "context.h"
#include "data_ability_helper.h"
#include "data_ability_observer_stub.h"
#include "data_ability_predicates.h"
#include "filemgmt_libn.h"
#include "ui_extension_context.h"
#include "ability_context.h"
#include "want.h"
#include "want_params.h"
#include "ui_content.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace CjPicker {

using WantHandle = void*;
static sptr<Rosen::Window> window_;

struct CjPickerCallBack {
    bool ready = false;
    int32_t resultCode;
    OHOS::AAFwk::Want want;
};

extern "C" {
struct PickerResult {
    int32_t resultCode;
    int32_t userSuffixIndex;
    bool isOriginal;
    CArrString ability_params_stream;
    CArrString uriArr;
    CArrString photoUris;
};

FFI_EXPORT int32_t FfiOHOSFilePickerModalPicker(OHOS::AbilityRuntime::Context* context, WantHandle config, char* windowName, void (*callback)(PickerResult));
}

class CjModalUICallback {
public:
    explicit CjModalUICallback(Ace::UIContent* uiContent, std::shared_ptr<CjPickerCallBack> pickerCallBack, void (*callback)(PickerResult));
    void OnRelease(int32_t releaseCode);
    void OnResultForModal(int32_t resultCode, const OHOS::AAFwk::Want& result);
    void OnReceive(const OHOS::AAFwk::WantParams& request);
    void OnError(int32_t code, const std::string& name, const std::string& message);
    void OnDestroy();
    void SetSessionId(int32_t sessionId);

private:
    int32_t sessionId_ = 0;
    std::function<void(PickerResult)> callback{nullptr};
    Ace::UIContent* uiContent;
    std::shared_ptr<CjPickerCallBack> pickerCallBack_;
};

} // namespace CjPicker
} // namespace OHOS

#endif // INTERFACES_KITS_CJ_PICKER_H