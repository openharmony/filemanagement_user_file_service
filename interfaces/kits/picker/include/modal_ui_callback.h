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
#ifndef MODAL_UI_CALLBACK_H
#define MODAL_UI_CALLBACK_H

#include "ability_context.h"
#include "want.h"
#include "want_params.h"
#include <string>
#include "picker_n_exporter.h"
#include "ui_content.h"

namespace OHOS {
namespace Picker {

class ModalUICallback {
public:
    explicit ModalUICallback(Ace::UIContent* uiContent, PickerCallBack* pickerCallBack);
    void OnRelease(int32_t releaseCode);
    void OnResultForModal(int32_t resultCode, const OHOS::AAFwk::Want& result);
    void OnReceive(const OHOS::AAFwk::WantParams& request);
    void OnError(int32_t code, const std::string& name, const std::string& message);
    void OnDestroy();
    void SetSessionId(int32_t sessionId);

private:
    int32_t sessionId_ = 0;
    Ace::UIContent* uiContent;
    PickerCallBack* pickerCallBack_;
};
} // namespace Picker
} // namespace OHOS

#endif