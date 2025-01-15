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
#define ABILITY_WANT_PARAMS_UIEXTENSIONTARGETTYPE "ability.want.params.uiExtensionTargetType"

#include "cj_picker.h"

#include "modal_ui_extension_config.h"
#include "ability.h"
#include "ui_extension_context.h"

namespace OHOS {
namespace CjPicker {

using namespace OHOS::Ace;

const int32_t ERR_OK = 0;
const int32_t ERR_INVALID_ARG = 13900020;
const int32_t ERR_INV = -1;

static Ace::UIContent* GetUIContextByContext(OHOS::AbilityRuntime::Context* context)
{
    auto contextsptr = context->shared_from_this();
    auto abilityContext = AbilityRuntime::Context::ConvertTo<AbilityRuntime::AbilityContext>(contextsptr);
    if (abilityContext == nullptr) {
        auto uiExtensionContext = AbilityRuntime::Context::ConvertTo<AbilityRuntime::UIExtensionContext>(contextsptr);
        if (uiExtensionContext == nullptr) {
            HILOG_ERROR("[picker]: Fail to convert to abilityContext or uiExtensionContext");
            return nullptr;
        }
        return uiExtensionContext->GetUIContent();
    }
    return abilityContext->GetUIContent();
}

static int32_t StartCjPickerExtension(OHOS::AbilityRuntime::Context* context,
    WantHandle want, void (*cjCallback)(PickerResult))
{
    HILOG_INFO("[picker]: StartPickerExtension begin.");
    Ace::UIContent *uiContent;
    if (window_) {
        HILOG_INFO("[picker] Will get uiContent by window.");
        uiContent = window_->GetUIContent();
    } else {
        HILOG_INFO("[picker] Will get uiContent by context.");
        uiContent = GetUIContextByContext(context);
    }

    if (uiContent == nullptr) {
        HILOG_ERROR("[picker]: get uiContent failed");
        return ERR_INVALID_ARG;
    }
    AAFwk::Want request = *reinterpret_cast<AAFwk::Want*>(want);

    std::string targetType = request.GetStringParam("extType");
    std::string pickerType;
    if (request.GetParams().HasParam("pickerType")) {
        pickerType = request.GetStringParam("pickerType");
    }
    request.SetParam(ABILITY_WANT_PARAMS_UIEXTENSIONTARGETTYPE, targetType);
    auto pickerCallback = std::make_shared<CjPickerCallBack>();
    auto callback = std::make_shared<CjModalUICallback>(uiContent, pickerCallback, cjCallback);
    Ace::ModalUIExtensionCallbacks extensionCallback = {
        .onRelease = std::bind(&CjModalUICallback::OnRelease, callback, std::placeholders::_1),
        .onResult = std::bind(&CjModalUICallback::OnResultForModal, callback, std::placeholders::_1,
            std::placeholders::_2),
        .onReceive = std::bind(&CjModalUICallback::OnReceive, callback, std::placeholders::_1),
        .onError = std::bind(&CjModalUICallback::OnError, callback, std::placeholders::_1, std::placeholders::_2,
            std::placeholders::_3),
        .onDestroy = std::bind(&CjModalUICallback::OnDestroy, callback),
    };
    Ace::ModalUIExtensionConfig config;
    config.prohibitedRemoveByNavigation = false;
    config.prohibitedRemoveByRouter = false;
    HILOG_INFO("[picker]: will CreateModalUIExtension by extType: %{public}s, pickerType: %{public}s",
        targetType.c_str(), pickerType.c_str());
    int sessionId = uiContent->CreateModalUIExtension(request, extensionCallback, config);
    if (sessionId == 0) {
        HILOG_ERROR("[picker]: create modalUIExtension failed");
        return ERR_INVALID_ARG;
    }
    callback->SetSessionId(sessionId);
    return ERR_OK;
}

static int32_t GetWindow(char* windowName, sptr<Rosen::Window> &window)
{
    auto customWindow = Rosen::Window::Find(windowName);
    if (!customWindow) {
        HILOG_ERROR("[picker] Window find fail.");
        return ERR_INV;
    }
    window = customWindow;
    HILOG_INFO("[picker] Window found: %{public}s", windowName);
    return ERR_OK;
}

static char* MallocCString(const std::string &origin)
{
    if (origin.empty()) {
        return nullptr;
    }
    auto length = origin.length() + 1;
    char *res = static_cast<char *>(malloc(sizeof(char) * length));
    if (res == nullptr) {
        return nullptr;
    }
    return std::char_traits<char>::copy(res, origin.c_str(), length);
}

static CArrString MallocStringVec(std::vector<std::string> &origin)
{
    CArrString res{};
    if (origin.empty()) {
        return res;
    }

    size_t size = origin.size();
    if (size == 0 || size > std::numeric_limits<size_t>::max() / sizeof(char *)) {
        return res;
    }
    res.head = static_cast<char **>(malloc(sizeof(char *) * size));
    if (res.head == nullptr) {
        return res;
    }
    size_t i = 0;
    for (; i < size; ++i) {
        res.head[i] = MallocCString(origin[i]);
    }
    res.size = static_cast<int64_t>(i);

    return res;
}

static PickerResult MakePickerResult(std::shared_ptr<CjPickerCallBack> pickerCallback)
{
    PickerResult ret{};
    ret.resultCode = pickerCallback->resultCode;
    ret.userSuffixIndex = pickerCallback->want.GetIntParam("userSuffixIndex", -1);
    ret.isOriginal = pickerCallback->want.GetBoolParam("isOriginal", false);
    if (pickerCallback->want.GetParams().HasParam("ability.params.stream")) {
        auto list = pickerCallback->want.GetStringArrayParam("ability.params.stream");
        ret.ability_params_stream = MallocStringVec(list);
    }
    if (pickerCallback->want.GetParams().HasParam("uriArr")) {
        auto list = pickerCallback->want.GetStringArrayParam("uriArr");
        ret.uriArr = MallocStringVec(list);
    }
    if (pickerCallback->want.GetParams().HasParam("select-item-list")) {
        auto list = pickerCallback->want.GetStringArrayParam("select-item-list");
        ret.photoUris = MallocStringVec(list);
    }
    return ret;
}

extern "C" {
int32_t FfiOHOSFilePickerModalPicker(OHOS::AbilityRuntime::Context* context,
    WantHandle config, char* windowName, void (*callback)(PickerResult))
{
    if (!context || !config) {
        HILOG_ERROR("[picker] context or config is nullptr.");
        return ERR_INVALID_ARG;
    }
    if (windowName) {
        int32_t status = GetWindow(windowName, window_);
        if (status != ERR_OK) {
            return status;
        }
    }
    return StartCjPickerExtension(context, config, callback);
}
}

CjModalUICallback::CjModalUICallback(Ace::UIContent* uiContent,
    std::shared_ptr<CjPickerCallBack> pickerCallBack, void (*callback)(PickerResult))
{
    this->uiContent = uiContent;
    this->pickerCallBack_ = pickerCallBack;
    this->callback = CJLambda::Create(callback);
}

void CjModalUICallback::SetSessionId(int32_t sessionId)
{
    this->sessionId_ = sessionId;
}

void CjModalUICallback::OnRelease(int32_t releaseCode)
{
    HILOG_INFO("[picker] OnRelease enter. release code is %{public}d", releaseCode);
    if (!pickerCallBack_) {
        HILOG_ERROR("[picker] OnRelease error");
        return;
    }
    this->uiContent->CloseModalUIExtension(this->sessionId_);
    pickerCallBack_->ready = true;

    auto ret = MakePickerResult(pickerCallBack_);
    this->callback(ret);
    if (window_) {
        window_ = nullptr;
    }
}

void CjModalUICallback::OnError(int32_t code, const std::string& name, const std::string& message)
{
    HILOG_ERROR("[picker] OnError enter. errorCode=%{public}d, name=%{public}s, message=%{public}s",
        code, name.c_str(), message.c_str());
    this->uiContent->CloseModalUIExtension(this->sessionId_);
}

void CjModalUICallback::OnResultForModal(int32_t resultCode, const OHOS::AAFwk::Want &result)
{
    HILOG_INFO("[picker] OnResultForModal enter. resultCode is %{public}d,", resultCode);
    if (!pickerCallBack_) {
        HILOG_ERROR("[picker] OnResultForModal error.");
        return;
    }
    pickerCallBack_->resultCode = resultCode;
    pickerCallBack_->want = result;
}

void CjModalUICallback::OnReceive(const OHOS::AAFwk::WantParams &request)
{
    HILOG_INFO("[picker] OnReceive enter.");
}

void CjModalUICallback::OnDestroy()
{
    HILOG_INFO("[picker] OnDestroy enter.");
}

} // namespace CjPicker
} // namespace OHOS