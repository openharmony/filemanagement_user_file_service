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

#include "modal_ui_callback.h"
#include "picker_n_exporter.h"

namespace OHOS {
namespace Picker {
using namespace OHOS::Ace;
using namespace std;
ModalUICallback::ModalUICallback(Ace::UIContent* uiContent, std::shared_ptr<PickerCallBack> pickerCallBack)
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
    HILOG_INFO("modal picker: OnRelease enter. release code is %{public}d", releaseCode);
    this->uiContent->CloseModalUIExtension(this->sessionId_);
    pickerCallBack_->ready = true;
}

void ModalUICallback::OnError(int32_t code, const std::string& name, const std::string& message)
{
    HILOG_ERROR("modal picker: OnError enter. errorCode=%{public}d, name=%{public}s, message=%{public}s",
        code, name.c_str(), message.c_str());
}

void ModalUICallback::OnResultForModal(int32_t resultCode, const OHOS::AAFwk::Want &result)
{
    HILOG_INFO("modal picker: OnResultForModal enter. resultCode is %{public}d, %{public}s",
        resultCode,  result.ToString().c_str());
    pickerCallBack_->resultCode = resultCode;
    pickerCallBack_->want = result;
    pickerCallBack_->ready = true;
}

void ModalUICallback::OnReceive(const OHOS::AAFwk::WantParams &request)
{
    HILOG_INFO("modal picker: OnReceive enter.");
}

void ModalUICallback::OnDestroy()
{
    HILOG_INFO("modal picker: OnDestroy enter.");
}
} // namespace Picker
} // namespace OHOS