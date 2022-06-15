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
namespace FileAccessFwk {
namespace {
constexpr size_t ARGC_ONE = 1;
}

using namespace OHOS::AppExecFwk;
using namespace OHOS::AbilityRuntime;
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
    HILOG_INFO("tag dsa %{public}s module:%{public}s, srcPath:%{public}s.",
        __func__, moduleName.c_str(), srcPath.c_str());
    HandleScope handleScope(jsRuntime_);

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
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    Extension::OnConnect(want);
    HILOG_INFO("tag dsa %{public}s end. ", __func__);
    return nullptr;
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
} // namespace FileAccessFwk
} // namespace OHOS