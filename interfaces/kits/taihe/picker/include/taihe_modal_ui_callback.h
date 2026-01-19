/*
 * Copyright (C) 2025-2026 Huawei Device Co., Ltd.
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
#ifndef TAIHE_MODAL_UI_CALLBACK_H
#define TAIHE_MODAL_UI_CALLBACK_H

#include "ability_context.h"
#include "want.h"
#include "want_params.h"
#include <string>
#include "ui_content.h"
#include "ani.h"
#include "taihe/runtime.hpp"
#include "taihe/platform/ani.hpp"

namespace OHOS {
namespace Picker {
constexpr size_t ANI_ARGC_MAX = 5;

struct AniNameListArg {
    struct dirent** namelist = { nullptr };
    int direntNum = 0;
};

struct AniPickerCallBack {
    bool ready = false;
    int32_t resultCode;
    OHOS::AAFwk::Want want;
};

struct AniPickerAsyncContext {
    size_t argc;
    ani_object argv[ANI_ARGC_MAX];
    std::shared_ptr<AniPickerCallBack> pickerCallBack;
};

struct AniInsertAsyncContext {
    std::vector<std::string> uriVec;
    ani_ref result = nullptr;
    int32_t errCode = 0;
    std::string udKey;
};

class ModalUICallback {
public:
    explicit ModalUICallback(Ace::UIContent* uiContent, std::shared_ptr<AniPickerCallBack> pickerCallBack);
    void OnRelease(int32_t releaseCode);
    void OnResultForModal(int32_t resultCode, const OHOS::AAFwk::Want& result);
    void OnReceive(const OHOS::AAFwk::WantParams& request);
    void OnError(int32_t code, const std::string& name, const std::string& message);
    void OnDestroy();
    void SetSessionId(int32_t sessionId);

private:
    int32_t sessionId_ = 0;
    Ace::UIContent* uiContent;
    std::shared_ptr<AniPickerCallBack> pickerCallBack_;
};
} // namespace Picker
} // namespace OHOS

#endif