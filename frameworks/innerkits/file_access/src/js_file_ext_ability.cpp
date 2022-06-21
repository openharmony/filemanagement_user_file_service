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
    HILOG_INFO("%{public}s.", __func__);
    return new JsFileExtAbility(static_cast<JsRuntime&>(*runtime));
}

JsFileExtAbility::JsFileExtAbility(JsRuntime& jsRuntime) : jsRuntime_(jsRuntime) {}
JsFileExtAbility::~JsFileExtAbility() = default;

void JsFileExtAbility::Init(const std::shared_ptr<AbilityLocalRecord> &record,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    HILOG_INFO("%{public}s begin.", __func__);
    FileExtAbility::Init(record, application, handler, token);
    std::string srcPath = "";
    GetSrcPath(srcPath);
    if (srcPath.empty()) {
        HILOG_ERROR("%{public}s Failed to get srcPath", __func__);
        return;
    }

    std::string moduleName(Extension::abilityInfo_->moduleName);
    moduleName.append("::").append(abilityInfo_->name);
    HILOG_INFO("%{public}s module:%{public}s, srcPath:%{public}s.",
        __func__, moduleName.c_str(), srcPath.c_str());
    HandleScope handleScope(jsRuntime_);

    jsObj_ = jsRuntime_.LoadModule(moduleName, srcPath);
    if (jsObj_ == nullptr) {
        HILOG_ERROR("%{public}s Failed to get jsObj_", __func__);
        return;
    }
    HILOG_INFO("%{public}s ConvertNativeValueTo.", __func__);
    NativeObject* obj = ConvertNativeValueTo<NativeObject>(jsObj_->Get());
    if (obj == nullptr) {
        HILOG_ERROR("%{public}s Failed to get JsFileExtAbility object", __func__);
        return;
    }
    HILOG_INFO("%{public}s end.", __func__);
}

void JsFileExtAbility::OnStart(const AAFwk::Want &want)
{
    Extension::OnStart(want);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());
    napi_value napiWant = OHOS::AppExecFwk::WrapWant(env, want);
    NativeValue* nativeWant = reinterpret_cast<NativeValue*>(napiWant);
    NativeValue* argv[] = {nativeWant};
    CallObjectMethod("onCreate", argv, ARGC_ONE);
}

sptr<IRemoteObject> JsFileExtAbility::OnConnect(const AAFwk::Want &want)
{
    HILOG_INFO("%{public}s begin.", __func__);
    Extension::OnConnect(want);
    HILOG_INFO("%{public}s end. ", __func__);
    return nullptr;
}

NativeValue* JsFileExtAbility::CallObjectMethod(const char* name, NativeValue* const* argv, size_t argc)
{
    if (!jsObj_) {
        HILOG_WARN("JsFileExtAbility::CallObjectMethod jsObj Not found FileExtAbility.js");
        return nullptr;
    }

    HandleScope handleScope(jsRuntime_);
    auto& nativeEngine = jsRuntime_.GetNativeEngine();

    NativeValue* value = jsObj_->Get();
    NativeObject* obj = ConvertNativeValueTo<NativeObject>(value);
    if (obj == nullptr) {
        HILOG_ERROR("%{public}s Failed to get FileExtAbility object", __func__);
        return nullptr;
    }

    NativeValue* method = obj->GetProperty(name);
    if (method == nullptr) {
        HILOG_ERROR("%{public}s Failed to get '%{public}s' from FileExtAbility object", __func__, name);
        return nullptr;
    }
    return handleScope.Escape(nativeEngine.CallFunction(value, method, argv, argc));
}

void JsFileExtAbility::GetSrcPath(std::string &srcPath)
{
    HILOG_INFO("JsFileExtAbility %{public}s .", __func__);
    if (!Extension::abilityInfo_->isStageBasedModel) {
        /* temporary compatibility api8 + config.json */
        srcPath.append(Extension::abilityInfo_->package);
        srcPath.append("/assets/js/");
        if (!Extension::abilityInfo_->srcPath.empty()) {
            srcPath.append(Extension::abilityInfo_->srcPath);
        }
        srcPath.append("/").append(Extension::abilityInfo_->name).append(".abc");
        return;
    }

    if (!Extension::abilityInfo_->srcEntrance.empty()) {
        srcPath.append(Extension::abilityInfo_->moduleName + "/");
        srcPath.append(Extension::abilityInfo_->srcEntrance);
        srcPath.erase(srcPath.rfind('.'));
        srcPath.append(".abc");
    }
}
} // namespace FileAccessFwk
} // namespace OHOS