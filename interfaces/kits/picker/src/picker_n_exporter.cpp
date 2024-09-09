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

static void StartModalPickerExecute(napi_env env, void *data)
{
    HILOG_INFO("[picker]: StartModalPickerExecute begin");
    auto *context = static_cast<PickerAsyncContext*>(data);
    while (!context->pickerCallBack->ready) {
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME_MS));
    }
    HILOG_INFO("[picker]: StartModalPickerExecute is ready.");
}

static void MakeResultWithArr(napi_env env, std::string key, napi_value &result,
    std::shared_ptr<PickerCallBack> pickerCallBack)
{
    napi_value array;
    napi_create_array(env, &array);
    napi_status status = napi_generic_failure;
    if (pickerCallBack->want.GetParams().HasParam(key.c_str())) {
        std::vector<std::string> list = pickerCallBack->want.GetStringArrayParam(key.c_str());
        HILOG_INFO("[picker]: %{public}s size. %{public}zu ", key.c_str(), list.size());
        for (size_t i = 0; i < list.size(); i++) {
            napi_value uri = nullptr;
            napi_create_string_utf8(env, list[i].c_str(), NAPI_AUTO_LENGTH, &uri);
            napi_set_element(env, array, i, uri);
        }
        if (key == "ability.params.stream") {
            key = "ability_params_stream";
        }
        status = napi_set_named_property(env, result, key.c_str(), array);
        if (status != napi_ok) {
            HILOG_ERROR("[picker]: napi_set_named_property uri failed");
        }
    }
}

static void MakeResultWithInt(napi_env env, std::string key, napi_value &result,
    std::shared_ptr<PickerCallBack> pickerCallBack)
{
    napi_status status = napi_generic_failure;
    if (pickerCallBack->want.GetParams().HasParam(key.c_str())) {
        const int32_t suffixindex = pickerCallBack->want.GetIntParam(key.c_str(), -1);
        HILOG_INFO("[picker]: %{public}s is %{public}d ", key.c_str(), suffixindex);
        napi_value suffix = nullptr;
        napi_create_int32(env, suffixindex, &suffix);
        status = napi_set_named_property(env, result, key.c_str(), suffix);
        if (status != napi_ok) {
            HILOG_ERROR("[picker]: napi_set_named_property suffix failed");
        }
    }
}

static napi_value MakeResultWithPickerCallBack(napi_env env, std::shared_ptr<PickerCallBack> pickerCallBack)
{
    if (pickerCallBack == nullptr) {
        HILOG_ERROR("pickerCallBack is null");
        return nullptr;
    }
    napi_value result = nullptr;
    napi_status status = napi_generic_failure;
    napi_create_object(env, &result);

    const int32_t resCode = pickerCallBack->resultCode;
    HILOG_INFO("[picker]: resCode is %{public}d.", resCode);
    napi_value resultCode = nullptr;
    napi_create_int32(env, resCode, &resultCode);
    status = napi_set_named_property(env, result, "resultCode", resultCode);
    if (status != napi_ok) {
        HILOG_ERROR("[picker]: napi_set_named_property resultCode failed");
    }
    MakeResultWithArr(env, "ability.params.stream", result, pickerCallBack);
    MakeResultWithArr(env, "uriArr", result, pickerCallBack);
    MakeResultWithInt(env, "userSuffixIndex", result, pickerCallBack);
    return result;
}

static void StartModalPickerAsyncCallbackComplete(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("[picker]: StartModalPickerAsyncCallbackComplete begin.");
    auto *context = static_cast<PickerAsyncContext*>(data);
    if (context == nullptr) {
        HILOG_ERROR("Async context is null");
        return;
    }
    auto jsContext = make_unique<JSAsyncContextOutput>();
    jsContext->status = false;
    status = napi_get_undefined(env, &jsContext->data);
    if (status != napi_ok) {
        HILOG_ERROR("[picker]: napi_get_undefined jsContext->data failed");
    }
    status = napi_get_undefined(env, &jsContext->error);
    if (status != napi_ok) {
        HILOG_ERROR("[picker]: napi_get_undefined jsContext->error failed");
    }
    napi_value result = MakeResultWithPickerCallBack(env, context->pickerCallBack);
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
    if (window_) {
        window_ = nullptr;
    }
}

static bool IsTypeRight(napi_env env, napi_value val, napi_valuetype type)
{
    napi_valuetype valueType;
    napi_status status = napi_typeof(env, val, &valueType);
    if (status != napi_ok || valueType != type) {
        HILOG_ERROR("[picker] Type is not right, type: %{public}d", valueType);
        return false;
    }
    return true;
}

static ErrCode GetWindowName(napi_env env, napi_value properties, sptr<Rosen::Window> &window)
{
    HILOG_INFO("[picker] Begin GetWindowName.");
    napi_value name;
    napi_status status;
    status = napi_get_named_property(env, properties, "name", &name);
    if (status != napi_ok) {
        HILOG_ERROR("Get name from properties fail.");
        return ERR_INV;
    }
    size_t nameLen;
    status = napi_get_value_string_utf8(env, name, NULL, 0, &nameLen);
    if (status != napi_ok) {
        HILOG_ERROR("[picker] Get window name length fail.");
        return ERR_INV;
    }
    char *nameBuf = new char[nameLen + 1];
    status = napi_get_value_string_utf8(env, name, nameBuf, nameLen + 1, &nameLen);
    if (status != napi_ok) {
        HILOG_ERROR("[picker] Get value string UTF8 fail.");
        return ERR_INV;
    }
    HILOG_INFO("[picker] Get window name: %{public}s", nameBuf);
    auto customWindow = Rosen::Window::Find(nameBuf);
    if (!customWindow) {
        HILOG_ERROR("[picker] Window find fail.");
        return ERR_INV;
    }
    window = customWindow;
    HILOG_INFO("[picker] Window found: %{public}s", nameBuf);
    delete[] nameBuf;
    return ERR_OK;
}

template <class AsyncContext>
static ErrCode GetCustomShowingWindow(napi_env env, AsyncContext &asyncContext,
    const napi_callback_info info, sptr<Rosen::Window> &window)
{
    HILOG_INFO("[picker] GetCustomShowingWindow enter.");
    napi_status status;
    if (!IsTypeRight(env, asyncContext->argv[ARGS_TWO], napi_object)) {
        HILOG_ERROR("[picker] The type of the parameter transferred to the window is not object.");
        return ERR_INV;
    }
    auto windowObj = asyncContext->argv[ARGS_TWO];
    napi_value getPropertiesFunc;
    status = napi_get_named_property(env, windowObj, "getWindowProperties", &getPropertiesFunc);
    if (status != napi_ok || !getPropertiesFunc) {
        HILOG_ERROR("[picker] getWindowProperties fail.");
        return ERR_INV;
    }
    if (!IsTypeRight(env, getPropertiesFunc, napi_function)) {
        HILOG_ERROR("[picker] The type of the parameter transferred to the getPropertiesFunc is not function.");
        return ERR_INV;
    }
    napi_value properties;
    status = napi_call_function(env, windowObj, getPropertiesFunc, 0, nullptr, &properties);
    if (status != napi_ok || !properties) {
        HILOG_INFO("[picker] Call getPropertiesFunc fail.");
        return ERR_INV;
    }
    return GetWindowName(env, properties, window);
}

Ace::UIContent *GetUIContent(napi_env env, napi_callback_info info,
    unique_ptr<PickerAsyncContext> &AsyncContext)
{
    bool isStageMode = false;
    napi_status status = AbilityRuntime::IsStageContext(env, AsyncContext->argv[ARGS_ZERO], isStageMode);
    if (status != napi_ok || !isStageMode) {
        HILOG_ERROR("[picker]: is not StageMode context");
        return nullptr;
    }
    auto context = AbilityRuntime::GetStageModeContext(env, AsyncContext->argv[ARGS_ZERO]);
    if (context == nullptr) {
        HILOG_ERROR("[picker]: Failed to get native stage context instance");
        return nullptr;
    }
    auto abilityContext = AbilityRuntime::Context::ConvertTo<AbilityRuntime::AbilityContext>(context);
    if (abilityContext == nullptr) {
        auto uiExtensionContext = AbilityRuntime::Context::ConvertTo<AbilityRuntime::UIExtensionContext>(context);
        if (uiExtensionContext == nullptr) {
            HILOG_ERROR("[picker]: Fail to convert to abilityContext or uiExtensionContext");
            return nullptr;
        }
        return uiExtensionContext->GetUIContent();
    }
    return abilityContext->GetUIContent();
}

static napi_value StartPickerExtension(napi_env env, napi_callback_info info,
    unique_ptr<PickerAsyncContext> &asyncContext)
{
    HILOG_INFO("[picker]: StartPickerExtension begin.");
    Ace::UIContent *uiContent;
    if (asyncContext->argc == 3 && window_) {
        HILOG_INFO("[picker] Will get uiContent by window.");
        uiContent = window_->GetUIContent();
    } else {
        uiContent= GetUIContent(env, info, asyncContext);
    }
    
    if (uiContent == nullptr) {
        HILOG_ERROR("[picker]: get uiContent failed");
        return nullptr;
    }
    AAFwk::Want request;
    AppExecFwk::UnwrapWant(env, asyncContext->argv[ARGS_ONE], request);

    std::string targetType = request.GetStringParam("extType");
    std::string pickerType;
    if (request.GetParams().HasParam("pickerType")) {
        pickerType = request.GetStringParam("pickerType");
    }
    request.SetParam(ABILITY_WANT_PARAMS_UIEXTENSIONTARGETTYPE, targetType);
    asyncContext->pickerCallBack = make_shared<PickerCallBack>();
    auto callback = std::make_shared<ModalUICallback>(uiContent, asyncContext->pickerCallBack);
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
    HILOG_INFO("[picker]: will CreateModalUIExtension by extType: %{public}s, pickerType: %{public}s",
        targetType.c_str(), pickerType.c_str());
    int sessionId = uiContent->CreateModalUIExtension(request, extensionCallback, config);
    if (sessionId == 0) {
        HILOG_ERROR("[picker]: create modalUIExtension failed");
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
    HILOG_INFO("[picker]: AsyncContextSetStaticObjectInfo begin.");
    napi_value thisVar = nullptr;
    asyncContext->argc = maxArgs;
    napi_status ret = napi_get_cb_info(env, info, &asyncContext->argc, &(asyncContext->argv[ARGS_ZERO]),
        &thisVar, nullptr);
    if (ret != napi_ok) {
        HILOG_ERROR("[picker]: Failed to get cb info");
        return ret;
    }
    if (asyncContext->argc == ARGS_THREE) {
        int res = GetCustomShowingWindow(env, asyncContext, info, window_);
        if (res != ERR_OK) {
            HILOG_ERROR("[picker] Failed to get cb window_ info.");
            return napi_invalid_arg;
        }
    }
    if (!((asyncContext->argc >= minArgs) && (asyncContext->argc <= maxArgs))) {
        HILOG_ERROR("[picker]: Number of args is invalid");
        return napi_invalid_arg;
    }
    if (minArgs > 0) {
        if (asyncContext->argv[ARGS_ZERO] == nullptr) {
            HILOG_ERROR("[picker]: Argument list is empty");
            return napi_invalid_arg;
        }
    }
    return napi_ok;
}

static napi_value ParseArgsStartModalPicker(napi_env env, napi_callback_info info,
    unique_ptr<PickerAsyncContext> &context)
{
    HILOG_INFO("[picker]: ParseArgsStartModalPicker begin.");
    constexpr size_t minArgs = ARGS_TWO;
    constexpr size_t maxArgs = ARGS_THREE;
    napi_status status = AsyncContextSetStaticObjectInfo(env, info, context, minArgs, maxArgs);
    if (status != napi_ok) {
        HILOG_ERROR("[picker]: AsyncContextSetStaticObjectInfo faild");
        return nullptr;
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
    HILOG_INFO("[picker]: StartModalPicker begin.");
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
