/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "picker_n_exporter.h"
#include "modal_ui_callback.h"
#include "modal_ui_extension_config.h"
#include "ability_context.h"
#include "context.h"
#include "ability.h"
#ifdef HAS_ACE_ENGINE_PART
#include "ui_content.h"
#endif
#include "ui_extension_context.h"
#include "want.h"

namespace OHOS {
namespace Picker {
using namespace std;
using namespace FileManagement;
using namespace FileManagement::LibN;
using namespace AppExecFwk;
#define WAIT_TIME_MS 100

bool PickerNExporter::Export()
{
    return exports_.AddProp({
        NVal::DeclareNapiFunction("startModalPicker", StartModalPicker)
    });
}

string PickerNExporter::GetClassName()
{
    return PickerNExporter::className_;
}

static napi_value MakeNValWithUriArr(napi_env env, const vector<string> &uriArr)
{
    if (uriArr.size() == 0) {
        return nullptr;
    }
    napi_value jsUris = nullptr;
    napi_create_array_with_length(env, uriArr.size(), &jsUris);
    for (size_t i = 0; i < uriArr.size(); i++) {
        HILOG_DEBUG("modal picker: uriArr[%{public}zu] is %{public}s.", i, uriArr[i].c_str());
        napi_value jsUri = nullptr;
        napi_status status = napi_create_string_utf8(env, uriArr[i].c_str(), NAPI_AUTO_LENGTH, &jsUri);
        if (status != napi_ok) {
            HILOG_ERROR("modal picker: uriArr napi_create_string_utf8 failed.");
        }
        if ((jsUri == nullptr) || (napi_set_element(env, jsUris, i, jsUri) != napi_ok)) {
            HILOG_ERROR("modal picker: jsUri == nullptr or uriArr napi_set_element failed.");
            break;
        }
    }
    return jsUris;
}

static void StartModalPickerExecute(napi_env env, void *data)
{
    HILOG_INFO("modal picker: StartModalPickerExecute begin");
    auto *context = static_cast<PickerAsyncContext*>(data);
    while (!context->pickerCallBack->ready) {
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME_MS));
    }
    HILOG_INFO("modal picker: StartModalPickerExecute is ready.");
}

static void StartModalPickerAsyncCallbackComplete(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("modal picker: StartModalPickerAsyncCallbackComplete begin.");
    auto *context = static_cast<PickerAsyncContext*>(data);
    if (context == nullptr) {
        HILOG_ERROR("Async context is null");
        return;
    }
    auto jsContext = make_unique<JSAsyncContextOutput>();
    jsContext->status = false;
    status = napi_get_undefined(env, &jsContext->data);
    if (status != napi_ok) {
        HILOG_ERROR("modal picker: napi_get_undefined jsContext->data failed");
    }
    status = napi_get_undefined(env, &jsContext->error);
    if (status != napi_ok) {
        HILOG_ERROR("modal picker: napi_get_undefined jsContext->error failed");
    }
    const string uri = context->pickerCallBack->uri;
    napi_value jsUri = nullptr;
    status = napi_create_string_utf8(env, uri.c_str(), NAPI_AUTO_LENGTH, &jsUri);
    if (jsUri == nullptr) {
        HILOG_ERROR("jsUri is nullptr.");
    }
    napi_value result = nullptr;
    napi_create_object(env, &result);
    status = napi_set_named_property(env, result, "uri", jsUri);
    if (status != napi_ok) {
        HILOG_ERROR("modal picker: napi_set_named_property uri failed");
    }
    const vector<string> uriArr = context->pickerCallBack->uriArr;
    if (uriArr.size() > 0) {
        napi_value jsUriArr = MakeNValWithUriArr(env, uriArr);
        status = napi_set_named_property(env, result, "uriArr", jsUriArr);
        if (status != napi_ok) {
            HILOG_ERROR("modal picker: napi_set_named_property uriArr failed");
        }
    }
    if (result != nullptr) {
        jsContext->data = result;
        jsContext->status = true;
    } else {
        PickerNapiUtils::CreateNapiErrorObject(env, jsContext->error, ERR_MEM_ALLOCATION,
            "failed to create js object");
    }
    if (context->work != nullptr) {
        PickerNapiUtils::InvokeJSAsyncMethod(env, context->deferred, context->callbackRef,
            context->work, *jsContext);
    }
    delete context;
}

Ace::UIContent *GetUIContent(napi_env env, napi_callback_info info,
    unique_ptr<PickerAsyncContext> &AsyncContext)
{
    bool isStageMode = false;
    napi_status status = AbilityRuntime::IsStageContext(env, AsyncContext->argv[ARGS_ZERO], isStageMode);
    if (status != napi_ok || !isStageMode) {
        HILOG_ERROR("modal picker: is not StageMode context");
        return nullptr;
    }
    auto context = AbilityRuntime::GetStageModeContext(env, AsyncContext->argv[ARGS_ZERO]);
    if (context == nullptr) {
        HILOG_ERROR("modal picker: Failed to get native stage context instance");
        return nullptr;
    }
    auto abilityContext = AbilityRuntime::Context::ConvertTo<AbilityRuntime::AbilityContext>(context);
    if (abilityContext == nullptr) {
        auto uiExtensionContext = AbilityRuntime::Context::ConvertTo<AbilityRuntime::UIExtensionContext>(context);
        if (uiExtensionContext == nullptr) {
            HILOG_ERROR("modal picker: Fail to convert to abilityContext or uiExtensionContext");
            return nullptr;
        }
        return uiExtensionContext->GetUIContent();
    }
    return abilityContext->GetUIContent();
}

static napi_value StartPickerExtension(napi_env env, napi_callback_info info,
    unique_ptr<PickerAsyncContext> &AsyncContext)
{
    HILOG_INFO("modal picker: StartPickerExtension begin.");
    Ace::UIContent *uiContent = GetUIContent(env, info, AsyncContext);
    if (uiContent == nullptr) {
        HILOG_ERROR("modal picker: get uiContent failed");
        return nullptr;
    }
    AAFwk::Want request;
    AppExecFwk::UnwrapWant(env, AsyncContext->argv[ARGS_ONE], request);

    std::string targetType = request.GetStringParam("extType");
    std::string pickerType;
    if (request.GetParams().HasParam("pickerType")) {
        pickerType = request.GetStringParam("pickerType");
    }
    request.SetParam(ABILITY_WANT_PARAMS_UIEXTENSIONTARGETTYPE, targetType);
    AsyncContext->pickerCallBack = make_shared<PickerCallBack>();
    auto callback = std::make_shared<ModalUICallback>(uiContent, AsyncContext->pickerCallBack.get());
    Ace::ModalUIExtensionCallbacks extensionCallback = {
        .onRelease = std::bind(&ModalUICallback::OnRelease, callback, std::placeholders::_1),
        .onResult = std::bind(&ModalUICallback::OnResultForModal, callback, std::placeholders::_1,
            std::placeholders::_2),
        .onReceive = std::bind(&ModalUICallback::OnReceive, callback, std::placeholders::_1),
        .onError = std::bind(&ModalUICallback::OnError, callback, std::placeholders::_1, std::placeholders::_2,
            std::placeholders::_3),
        .onDestroy = std::bind(&ModalUICallback::OnDestroy, callback),
    };
    Ace::ModalUIExtensionConfig config;
    HILOG_INFO("modal picker: will CreateModalUIExtension by extType: %{public}s, pickerType: %{public}s",
        targetType.c_str(), pickerType.c_str());
    int sessionId = uiContent->CreateModalUIExtension(request, extensionCallback, config);
    if (sessionId == 0) {
        HILOG_ERROR("modal picker: create modalUIExtension failed");
        return nullptr;
    }
    callback->SetSessionId(sessionId);
    napi_value result = nullptr;
    napi_get_boolean(env, true, &result);
    return result;
}

template <class AsyncContext>
static napi_status AsyncContextSetStaticObjectInfo(napi_env env, napi_callback_info info,
    AsyncContext &asyncContext, const size_t minArgs, const size_t maxArgs)
{
    HILOG_INFO("modal picker: AsyncContextSetStaticObjectInfo begin.");
    napi_value thisVar = nullptr;
    asyncContext->argc = maxArgs;
    napi_status ret = napi_get_cb_info(env, info, &asyncContext->argc, &(asyncContext->argv[ARGS_ZERO]),
        &thisVar, nullptr);
    if (ret != napi_ok) {
        HILOG_ERROR("modal picker: Failed to get cb info");
        return ret;
    }
    if (!((asyncContext->argc >= minArgs) && (asyncContext->argc <= maxArgs))) {
        HILOG_ERROR("modal picker: Number of args is invalid");
        return napi_invalid_arg;
    }
    if (minArgs > 0) {
        if (asyncContext->argv[ARGS_ZERO] == nullptr) {
            HILOG_ERROR("modal picker: Argument list is empty");
            return napi_invalid_arg;
        }
    }
    return napi_ok;
}

static napi_value ParseArgsStartModalPicker(napi_env env, napi_callback_info info,
    unique_ptr<PickerAsyncContext> &context)
{
    HILOG_INFO("modal picker: ParseArgsStartModalPicker begin.");
    constexpr size_t minArgs = ARGS_TWO;
    constexpr size_t maxArgs = ARGS_THREE;
    napi_status status = AsyncContextSetStaticObjectInfo(env, info, context, minArgs, maxArgs);
    if (status != napi_ok) {
        HILOG_ERROR("modal picker: AsyncContextSetStaticObjectInfo faild");
    }
    napi_value result = nullptr;
    napi_value ret = StartPickerExtension(env, info, context);
    if (ret == nullptr) {
        return nullptr;
    }
    napi_get_boolean(env, true, &result);
    return result;
}

napi_value PickerNExporter::StartModalPicker(napi_env env, napi_callback_info info)
{
    HILOG_INFO("modal picker: StartModalPicker begin.");
    unique_ptr<PickerAsyncContext> asyncContext = make_unique<PickerAsyncContext>();
    napi_value ret = ParseArgsStartModalPicker(env, info, asyncContext);
    if (ret == nullptr) {
        return nullptr;
    }
    return PickerNapiUtils::NapiCreateAsyncWork(env, asyncContext, "StrartModalPicker",
        StartModalPickerExecute, StartModalPickerAsyncCallbackComplete);
}

PickerNExporter::PickerNExporter(napi_env env, napi_value exports) : NExporter(env, exports)
{
    HILOG_INFO("PickerNExporter is called.");
}

PickerNExporter::~PickerNExporter() {}
} // namespace Picker
} // namespace OHOS
