/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "taihe_modal_ui_callback.h"

namespace OHOS {
namespace Picker {
using namespace OHOS::Ace;
using namespace std;
ModalUICallback::ModalUICallback(Ace::UIContent* uiContent, std::shared_ptr<AniPickerCallBack> pickerCallBack)
{
    this->uiContent = uiContent;
    this->pickerCallBack_ = pickerCallBack;
}

void ModalUICallback::SetSessionId(int32_t sessionId)
{
    this->sessionId_ = sessionId;
}

void ModalUICallback::OnRelease(int32_t releaseCode)
{
    if (this->uiContent) {
        this->uiContent->CloseModalUIExtension(this->sessionId_);
    }

    if (pickerCallBack_) {
        pickerCallBack_->ready = true;
    }
}

void ModalUICallback::OnError(int32_t code, const std::string& name, const std::string& message)
{
    if (!pickerCallBack_ || !this->uiContent) {
        return;
    }
    this->uiContent->CloseModalUIExtension(this->sessionId_);
    pickerCallBack_->ready = true;
}

void ModalUICallback::OnResultForModal(int32_t resultCode, const OHOS::AAFwk::Want &result)
{
    if (!pickerCallBack_) {
        return;
    }
    pickerCallBack_->resultCode = resultCode;
    pickerCallBack_->want = result;
}

void ModalUICallback::OnReceive(const OHOS::AAFwk::WantParams &request)
{
}

void ModalUICallback::OnDestroy()
{
    if (!pickerCallBack_ || !this->uiContent) {
        return;
    }
    this->uiContent->CloseModalUIExtension(this->sessionId_);
    pickerCallBack_->ready = true;
}
} // namespace Picker
} // namespace OHOS