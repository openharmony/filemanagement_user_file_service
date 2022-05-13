/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "js_file_ext_ability.h"

#include "extension_context.h"
#include "ability_info.h"
#include "accesstoken_kit.h"
#include "bytrace.h"
#include "file_ext_stub_impl.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "js_runtime.h"
#include "js_runtime_utils.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_common_util.h"
#include "napi_common_want.h"
#include "napi_remote_object.h"

namespace OHOS {
namespace AbilityRuntime {
namespace {
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
}

using namespace OHOS::AppExecFwk;
using OHOS::Security::AccessToken::AccessTokenKit;

JsFileExtAbility* JsFileExtAbility::Create(const std::unique_ptr<Runtime>& runtime)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    return new JsFileExtAbility(static_cast<JsRuntime&>(*runtime));
}

JsFileExtAbility::JsFileExtAbility(JsRuntime& jsRuntime) : jsRuntime_(jsRuntime) {}
JsFileExtAbility::~JsFileExtAbility() = default;

void JsFileExtAbility::Init(const std::shared_ptr<AbilityLocalRecord> &record,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    FileExtAbility::Init(record, application, handler, token);
    std::string srcPath = "";
    GetSrcPath(srcPath);
    if (srcPath.empty()) {
        HILOG_ERROR("tag dsa Failed to get srcPath");
        return;
    }

    std::string moduleName(Extension::abilityInfo_->moduleName);
    moduleName.append("::").append(abilityInfo_->name);
    HILOG_INFO("tag dsa %{public}s module:%{public}s, srcPath:%{public}s.", __func__, moduleName.c_str(), srcPath.c_str());
    HandleScope handleScope(jsRuntime_);
    //auto& engine = jsRuntime_.GetNativeEngine();

    jsObj_ = jsRuntime_.LoadModule(moduleName, srcPath);
    if (jsObj_ == nullptr) {
        HILOG_ERROR("tag dsa Failed to get jsObj_");
        return;
    }
    HILOG_INFO("tag dsa JsFileExtAbility::Init ConvertNativeValueTo.");
    NativeObject* obj = ConvertNativeValueTo<NativeObject>(jsObj_->Get());
    if (obj == nullptr) {
        HILOG_ERROR("tag dsa Failed to get JsFileExtAbility object");
        return;
    }

    /*
    auto context = GetContext();
    if (context == nullptr) {
        HILOG_ERROR("tag dsa Failed to get context");
        return;
    }

    HILOG_INFO("tag dsa JsFileExtAbility::Init CreateJsDataShareExtAbilityContext.");
    NativeValue* contextObj = CreateJsDataShareExtAbilityContext(engine, context);
    auto contextRef = jsRuntime_.LoadSystemModule("application.DataShareExtensionAbilityContext",
        &contextObj, ARGC_ONE);
    contextObj = contextRef->Get();
    HILOG_INFO("tag dsa JsFileExtAbility::Init Bind.");
    context->Bind(jsRuntime_, contextRef.release());
    HILOG_INFO("tag dsa JsFileExtAbility::SetProperty.");
    obj->SetProperty("context", contextObj);

    auto nativeObj = ConvertNativeValueTo<NativeObject>(contextObj);
    if (nativeObj == nullptr) {
        HILOG_ERROR("tag dsa Failed to get datashare extension ability native object");
        return;
    }

    HILOG_INFO("tag dsa Set datashare extension ability context");

    nativeObj->SetNativePointer(new std::weak_ptr<AbilityRuntime::Context>(context),
        [](NativeEngine*, void* data, void*) {
            HILOG_INFO("tag dsa Finalizer for weak_ptr datashare extension ability context is called");
            delete static_cast<std::weak_ptr<AbilityRuntime::Context>*>(data);
        }, nullptr);
    */
    HILOG_INFO("tag dsa JsFileExtAbility::Init end.");
}

void JsFileExtAbility::OnStart(const AAFwk::Want &want)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    Extension::OnStart(want);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());
    napi_value napiWant = OHOS::AppExecFwk::WrapWant(env, want);
    NativeValue* nativeWant = reinterpret_cast<NativeValue*>(napiWant);
    NativeValue* argv[] = {nativeWant};
    CallObjectMethod("onCreate", argv, ARGC_ONE);
    HILOG_INFO("tag dsa %{public}s end.", __func__);
}

sptr<IRemoteObject> JsFileExtAbility::OnConnect(const AAFwk::Want &want)
{
    BYTRACE_NAME(BYTRACE_TAG_DISTRIBUTEDDATA, __PRETTY_FUNCTION__);
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    Extension::OnConnect(want);
    sptr<FileExtStubImpl> remoteObject = new (std::nothrow) FileExtStubImpl(
        std::static_pointer_cast<JsFileExtAbility>(shared_from_this()),
        reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine()));
    if (remoteObject == nullptr) {
        HILOG_ERROR("tag dsa %{public}s No memory allocated for FileExtStubImpl", __func__);
        return nullptr;
    }
    HILOG_INFO("tag dsa %{public}s end. ", __func__);
    return remoteObject->AsObject();
}

NativeValue* JsFileExtAbility::CallObjectMethod(const char* name, NativeValue* const* argv, size_t argc)
{
    HILOG_INFO("tag dsa JsFileExtAbility::CallObjectMethod(%{public}s), begin", name);

    if (!jsObj_) {
        HILOG_WARN("Not found FileExtAbility.js");
        return nullptr;
    }

    HandleScope handleScope(jsRuntime_);
    auto& nativeEngine = jsRuntime_.GetNativeEngine();

    NativeValue* value = jsObj_->Get();
    NativeObject* obj = ConvertNativeValueTo<NativeObject>(value);
    if (obj == nullptr) {
        HILOG_ERROR("tag dsa Failed to get FileExtAbility object");
        return nullptr;
    }

    NativeValue* method = obj->GetProperty(name);
    if (method == nullptr) {
        HILOG_ERROR("tag dsa Failed to get '%{public}s' from FileExtAbility object", name);
        return nullptr;
    }
    HILOG_INFO("tag dsa JsFileExtAbility::CallFunction(%{public}s), success", name);
    return handleScope.Escape(nativeEngine.CallFunction(value, method, argv, argc));
}

void JsFileExtAbility::GetSrcPath(std::string &srcPath)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    if (!Extension::abilityInfo_->isStageBasedModel) {
        /* temporary compatibility api8 + config.json */
        srcPath.append(Extension::abilityInfo_->package);
        srcPath.append("/assets/js/");
        if (!Extension::abilityInfo_->srcPath.empty()) {
            srcPath.append(Extension::abilityInfo_->srcPath);
        }
        srcPath.append("/").append(Extension::abilityInfo_->name).append(".abc");
        HILOG_INFO("tag dsa %{public}s end1, srcPath:%{public}s", __func__, srcPath.c_str());
        return;
    }

    if (!Extension::abilityInfo_->srcEntrance.empty()) {
        srcPath.append(Extension::abilityInfo_->moduleName + "/");
        srcPath.append(Extension::abilityInfo_->srcEntrance);
        srcPath.erase(srcPath.rfind('.'));
        srcPath.append(".abc");
    }
    HILOG_INFO("tag dsa %{public}s end2, srcPath:%{public}s", __func__, srcPath.c_str());
}

int JsFileExtAbility::OpenFile(const Uri &uri, const std::string &mode)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());

    napi_value napiUri = nullptr;
    napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    napi_value napiMode = nullptr;
    napi_create_string_utf8(env, mode.c_str(), NAPI_AUTO_LENGTH, &napiMode);

    NativeValue* nativeUri = reinterpret_cast<NativeValue*>(napiUri);
    NativeValue* nativeMode = reinterpret_cast<NativeValue*>(napiMode);
    NativeValue* argv[] = {nativeUri, nativeMode};
    NativeValue* nativeResult = CallObjectMethod("openFile", argv, ARGC_TWO);
    int ret = -1;
    if (nativeResult == nullptr) {
        HILOG_ERROR("tag dsa %{public}s call openFile with return null.", __func__);
        return ret;
    }
    ret = OHOS::AppExecFwk::UnwrapInt32FromJS(env, reinterpret_cast<napi_value>(nativeResult));
    HILOG_INFO("tag dsa %{public}s end. return fd:%{public}d", __func__, ret);
    return ret;
}

int JsFileExtAbility::CloseFile(int fd, const std::string &uri)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());

    napi_value napiFd = nullptr;
    napi_create_int32(env, fd, &napiFd);
    napi_value napiUri = nullptr;
    napi_create_string_utf8(env, uri.c_str(), NAPI_AUTO_LENGTH, &napiUri);

    NativeValue* nativeFd = reinterpret_cast<NativeValue*>(napiFd);
    NativeValue* nativeUri = reinterpret_cast<NativeValue*>(napiUri);
    NativeValue* argv[] = {nativeFd, nativeUri};
    NativeValue* nativeResult = CallObjectMethod("closeFile", argv, ARGC_TWO);
    int ret = -1;
    if (nativeResult == nullptr) {
        HILOG_ERROR("tag dsa %{public}s call closeFile with return null.", __func__);
        return ret;
    }
    ret = OHOS::AppExecFwk::UnwrapInt32FromJS(env, reinterpret_cast<napi_value>(nativeResult));
    HILOG_INFO("tag dsa %{public}s end. return fd:%{public}d", __func__, ret);
    return ret;
}

int JsFileExtAbility::CreateFile(const Uri &parentUri, const std::string &displayName,  Uri &newFileUri)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());

    napi_value napiParentUri = nullptr;
    napi_create_string_utf8(env, parentUri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiParentUri);
    napi_value napiDisplayName = nullptr;
    napi_create_string_utf8(env, displayName.c_str(), NAPI_AUTO_LENGTH, &napiDisplayName);

    NativeValue* nativeParentUri = reinterpret_cast<NativeValue*>(napiParentUri);
    NativeValue* nativeDisplayName = reinterpret_cast<NativeValue*>(napiDisplayName);
    NativeValue* argv[] = {nativeParentUri, nativeDisplayName};
    NativeValue* nativeResult = CallObjectMethod("createFile", argv, ARGC_TWO);
    int ret = -1;
    if (nativeResult == nullptr) {
        HILOG_ERROR("tag dsa %{public}s call createFile with return null.", __func__);
        return ret;
    }
    std::string uriStr = OHOS::AppExecFwk::UnwrapStringFromJS(env, reinterpret_cast<napi_value>(nativeResult));
    if (uriStr.empty()) {
        HILOG_ERROR("tag dsa %{public}s call Mkdir with return empty.", __func__);
        return ret;
    } else {
        ret = 0;
    }
    newFileUri = Uri(uriStr);
    HILOG_INFO("tag dsa %{public}s end. return fd:%{public}d, newFileUri = %{public}s", __func__, ret, uriStr.c_str());
    return ret;
}

int JsFileExtAbility::Mkdir(const Uri &parentUri, const std::string &displayName, Uri &newFileUri)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());

    napi_value napiParentUri = nullptr;
    napi_create_string_utf8(env, parentUri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiParentUri);
    napi_value napiDisplayName = nullptr;
    napi_create_string_utf8(env, displayName.c_str(), NAPI_AUTO_LENGTH, &napiDisplayName);

    NativeValue* nativeParentUri = reinterpret_cast<NativeValue*>(napiParentUri);
    NativeValue* nativeDisplayName = reinterpret_cast<NativeValue*>(napiDisplayName);
    NativeValue* argv[] = {nativeParentUri, nativeDisplayName};
    NativeValue* nativeResult = CallObjectMethod("mkdir", argv, ARGC_TWO);
    int ret = -1;
    if (nativeResult == nullptr) {
        HILOG_ERROR("tag dsa %{public}s call Mkdir with return null.", __func__);
        return ret;
    }
    std::string uriStr = OHOS::AppExecFwk::UnwrapStringFromJS(env, reinterpret_cast<napi_value>(nativeResult));
    if (uriStr.empty()) {
        HILOG_ERROR("tag dsa %{public}s call Mkdir with return empty.", __func__);
        return ret;
    } else {
        ret = 0;
    }
    newFileUri = Uri(uriStr);
    HILOG_INFO("tag dsa %{public}s end. return fd:%{public}d, newFileUri = %{public}s", __func__, ret, uriStr.c_str());
    return ret;
}

int JsFileExtAbility::Delete(const Uri &sourceFileUri)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());

    napi_value napiUri = nullptr;
    napi_create_string_utf8(env, sourceFileUri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);

    NativeValue* nativeUri = reinterpret_cast<NativeValue*>(napiUri);
    NativeValue* argv[] = {nativeUri};
    NativeValue* nativeResult = CallObjectMethod("delete", argv, ARGC_ONE);
    int ret = -1;
    if (nativeResult == nullptr) {
        HILOG_ERROR("tag dsa %{public}s call delete with return null.", __func__);
        return ret;
    }
    ret = OHOS::AppExecFwk::UnwrapInt32FromJS(env, reinterpret_cast<napi_value>(nativeResult));
    HILOG_INFO("tag dsa %{public}s end. return fd:%{public}d", __func__, ret);
    return ret;
}

int JsFileExtAbility::Move(const Uri &sourceFileUri, const Uri &targetParentUri, Uri &newFileUri)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());

    napi_value napiSourceFileUri = nullptr;
    napi_create_string_utf8(env, sourceFileUri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiSourceFileUri);
    napi_value napiTargetParentUri = nullptr;
    napi_create_string_utf8(env, targetParentUri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiTargetParentUri);

    NativeValue* nativeSourceFileUri = reinterpret_cast<NativeValue*>(napiSourceFileUri);
    NativeValue* nativeTargetParentUri = reinterpret_cast<NativeValue*>(napiTargetParentUri);
    NativeValue* argv[] = {nativeSourceFileUri, nativeTargetParentUri};
    NativeValue* nativeResult = CallObjectMethod("move", argv, ARGC_TWO);
    int ret = -1;
    if (nativeResult == nullptr) {
        HILOG_ERROR("tag dsa %{public}s call move with return null.", __func__);
        return ret;
    }
    std::string uriStr = OHOS::AppExecFwk::UnwrapStringFromJS(env, reinterpret_cast<napi_value>(nativeResult));
    if (uriStr.empty()) {
        HILOG_ERROR("tag dsa %{public}s call move with return empty.", __func__);
        return ret;
    } else {
        ret = 0;
    }
    newFileUri = Uri(uriStr);
    HILOG_INFO("tag dsa %{public}s end. return fd:%{public}d, newFileUri = %{public}s", __func__, ret, uriStr.c_str());
    return ret;
}

int JsFileExtAbility::Rename(const Uri &sourceFileUri, const std::string &displayName, Uri &newFileUri)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());

    napi_value napiSourceFileUri = nullptr;
    napi_create_string_utf8(env, sourceFileUri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiSourceFileUri);
    napi_value napiDisplayName = nullptr;
    napi_create_string_utf8(env, displayName.c_str(), NAPI_AUTO_LENGTH, &napiDisplayName);

    NativeValue* nativeSourceFileUri = reinterpret_cast<NativeValue*>(napiSourceFileUri);
    NativeValue* nativeDisplayName = reinterpret_cast<NativeValue*>(napiDisplayName);
    NativeValue* argv[] = {nativeSourceFileUri, nativeDisplayName};
    NativeValue* nativeResult = CallObjectMethod("rename", argv, ARGC_TWO);
    int ret = -1;
    if (nativeResult == nullptr) {
        HILOG_ERROR("tag dsa %{public}s call rename with return null.", __func__);
        return ret;
    }
    std::string uriStr = OHOS::AppExecFwk::UnwrapStringFromJS(env, reinterpret_cast<napi_value>(nativeResult));
    if (uriStr.empty()) {
        HILOG_ERROR("tag dsa %{public}s call rename with return empty.", __func__);
        return ret;
    } else {
        ret = 0;
    }
    newFileUri = Uri(uriStr);
    HILOG_INFO("tag dsa %{public}s end. return fd:%{public}d, newFileUri = %{public}s", __func__, ret, uriStr.c_str());
    return ret;
}
} // namespace AbilityRuntime
} // namespace OHOS