/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
static std::mutex recentPathMutex;

// 宏定义 done
// unit done
// callback done

// 错误码 417闭环
// 与被拉起方对齐规格 发什么、收什么 ---417找dongzheng对齐 417闭环 -- 发type、收uri
// 确认createUIExtension是否能满足要求---待dujingcheng答复 417闭环 --可以
// 与JS互通

bool PickerNExporter::Export()
{
    return exports_.AddProp({
        NVal::DeclareNapiFunction("startDownloadPicker", StartDownloadPicker)
    });
}

string PickerNExporter::GetClassName()
{
    return PickerNExporter::className_;
}

static void StartDownloadPickerExecute(napi_env env, void *data)
{
    HILOG_INFO("lby StartDownloadPickerExecute begin");
    auto *context = static_cast<PickerAsyncContext*>(data);
    while (!context->pickerCallBack->ready) {
        HILOG_INFO("lby StartDownloadPickerExecute not ready.");
        // std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
    }
}

static void StartDownloadPickerAsyncCallbackComplete(napi_env env, napi_status status, void *data)
{
    // 返回值处理，对齐返回值后修改
    
    auto *context = static_cast<PickerAsyncContext*>(data);
    CHECK_NULL_PTR_RETURN_VOID(context, "Async context is null");

    auto jsContext = make_unique<JSAsyncContextOutput>();
    jsContext->status = false;

    CHECK_ARGS_RET_VOID(env, napi_get_undefined(env, &jsContext->data), JS_ERR_PARAMETER_INVALID);
    CHECK_ARGS_RET_VOID(env, napi_get_undefined(env, &jsContext->error), JS_ERR_PARAMETER_INVALID);

    const string uri = context->pickerCallBack->uri;
    napi_value jsUri = nullptr;
    CHECK_ARGS_RET_VOID(env, napi_create_string_utf8(env, uri.c_str(),
        NAPI_AUTO_LENGTH, &jsUri), JS_INNER_FAIL);
    if (jsUri == nullptr) {
        HILOG_ERROR("jsUri is nullptr.");
    }
    napi_value result = nullptr;
    status = napi_set_named_property(env, result, "uri", jsUri);
    if (status != napi_ok) {
        HILOG_ERROR("napi_set_named_property uri failed");
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
    // 仅有StageContext可以转化为AbilityContext
    napi_status status = AbilityRuntime::IsStageContext(env, AsyncContext->argv[ARGS_ZERO], isStageMode);
    if (status != napi_ok || !isStageMode) {
        HILOG_ERROR("is not StageMode context");
        return nullptr;
    }
    auto context = AbilityRuntime::GetStageModeContext(env, AsyncContext->argv[ARGS_ZERO]);
    if (context == nullptr) {
        HILOG_ERROR("Failed to get native stage context instance");
        return nullptr;
    }
    auto abilityContext = AbilityRuntime::Context::ConvertTo<AbilityRuntime::AbilityContext>(context);
    if (abilityContext == nullptr) {
        auto uiExtensionContext = AbilityRuntime::Context::ConvertTo<AbilityRuntime::UIExtensionContext>(context);
        if (uiExtensionContext == nullptr) {
            HILOG_ERROR("Fail to convert to abilityContext or uiExtensionContext");
            return nullptr;
        }
        return uiExtensionContext->GetUIContent();
    }
    return abilityContext->GetUIContent();
}

static napi_value StartPickerExtension(napi_env env, napi_callback_info info,
    unique_ptr<PickerAsyncContext> &AsyncContext)
{
    HILOG_INFO("lby StartPickerExtension begin.");
    Ace::UIContent *uiContent = GetUIContent(env, info, AsyncContext);
    if (uiContent == nullptr) {
        HILOG_ERROR("lby get uiContent failed");
        return nullptr;
    }
    AAFwk::Want request;
    AppExecFwk::UnwrapWant(env, AsyncContext->argv[ARGS_ONE], request);
    // todo: 这里是否要约定targetType
    // 对齐todo：request的内容是要传给被拉起方的，需要对齐
    // request、AsyncContext
    std::string targetType = "filedownload";
    request.SetParam(ABILITY_WANT_PARAMS_UIEXTENSIONTARGETTYPE, targetType);
    AsyncContext->pickerCallBack = make_shared<PickerCallBack>();
    auto callback = std::make_shared<ModalUICallback>(uiContent, AsyncContext->pickerCallBack.get());
    // todo: 这里要实现 ModalUICallback
    Ace::ModalUIExtensionCallbacks extensionCallback = {
        std::bind(&ModalUICallback::OnRelease, callback, std::placeholders::_1),
        std::bind(&ModalUICallback::OnResultForModal, callback, std::placeholders::_1, std::placeholders::_2),
        std::bind(&ModalUICallback::OnReceive, callback, std::placeholders::_1),
        std::bind(&ModalUICallback::OnError, callback, std::placeholders::_1, std::placeholders::_2,
            std::placeholders::_3),
        std::bind(&ModalUICallback::OnDestroy, callback),
    };
    Ace::ModalUIExtensionConfig config;
    config.isProhibitBack = true;
    int sessionId = uiContent->CreateModalUIExtension(request, extensionCallback, config);
    if (sessionId == 0) {
        HILOG_ERROR("lby create modalUIExtension failed");
        return nullptr;
    }
    callback->SetSessionId(sessionId);
    napi_value result = nullptr;
    // todo：这里JS_INNER_FAIL未定义
    CHECK_ARGS(env, napi_get_boolean(env, true, &result), JS_INNER_FAIL);
    return result;
}

// todo：模板理解+拆解
template <class AsyncContext>
static napi_status AsyncContextSetStaticObjectInfo(napi_env env, napi_callback_info info,
    AsyncContext &asyncContext, const size_t minArgs, const size_t maxArgs)
{
    HILOG_INFO("lby AsyncContextSetStaticObjectInfo begin.");
    // 设置上下文静态信息
    napi_value thisVar = nullptr;
    asyncContext->argc = maxArgs;
    CHECK_STATUS_RET(napi_get_cb_info(env, info, &asyncContext->argc, &(asyncContext->argv[ARGS_ZERO]), &thisVar,
        nullptr), "Failed to get cb info");
    CHECK_COND_RET(((asyncContext->argc >= minArgs) && (asyncContext->argc <= maxArgs)), napi_invalid_arg,
        "Number of args is invalid");
    if (minArgs > 0) {
        CHECK_COND_RET(asyncContext->argv[ARGS_ZERO] != nullptr, napi_invalid_arg, "Argument list is empty");
    }
    // 获取context->callbackRef
    CHECK_STATUS_RET(PickerNapiUtils::GetParamCallback(env, asyncContext), "Failed to get callback param!");
    return napi_ok;
}

static napi_value ParseArgsStartDownloadPicker(napi_env env, napi_callback_info info,
    unique_ptr<PickerAsyncContext> &context)
{
    // 开始解析参数
    HILOG_INFO("lby ParseArgsStartDownloadPicker begin.");
    constexpr size_t minArgs = ARGS_TWO;
    constexpr size_t maxArgs = ARGS_THREE;
    CHECK_ARGS(env, AsyncContextSetStaticObjectInfo(env, info, context, minArgs, maxArgs),
        JS_ERR_PARAMETER_INVALID);
    // todo：实现PickerNapiUtils：：GetParamCallback
    // todo: 这里为什么要设置两次GetParamCallback
    NAPI_CALL(env, PickerNapiUtils::GetParamCallback(env, context));
    CHECK_NULLPTR_RET(StartPickerExtension(env, info, context));
    napi_value result = nullptr;
    CHECK_ARGS(env, napi_get_boolean(env, true, &result), JS_INNER_FAIL);
    return result;
}

napi_value PickerNExporter::StartDownloadPicker(napi_env env, napi_callback_info info)
{
    // todo:PickerAsyncContext 理解+拆解
    HILOG_INFO("lby StartDownloadPicker begin.");
    unique_ptr<PickerAsyncContext> asyncContext = make_unique<PickerAsyncContext>();
    // asyncContext->resultNapiType = ResultNapiType::TYPE_PHOTOACCESS_HELPER;
    // todo: 这里ParseArgsStartDownloadPicker完后已经拉起了UI，NapiCreateAsyncWork的作用
    ParseArgsStartDownloadPicker(env, info, asyncContext);
    // todo：实现PickerNapiUtils：：NapiCreateAsyncWork
    return PickerNapiUtils::NapiCreateAsyncWork(env, asyncContext, "StrartDownloadPicker",
        StartDownloadPickerExecute, StartDownloadPickerAsyncCallbackComplete);
}

PickerNExporter::PickerNExporter(napi_env env, napi_value exports) : NExporter(env, exports)
{
    HILOG_INFO("PickerNExporter is called.");
}

PickerNExporter::~PickerNExporter() {}
} // namespace Picker
} // namespace OHOS
