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
#include "file_ext_stub_impl.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "js_runtime.h"
#include "js_runtime_utils.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_common_fileaccess.h"
#include "napi_common_util.h"
#include "napi_common_want.h"
#include "napi_remote_object.h"

namespace OHOS {
namespace FileAccessFwk {
namespace {
constexpr size_t ARGC_ZERO = 0;
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
}

using namespace OHOS::AppExecFwk;
using namespace OHOS::AbilityRuntime;
using OHOS::Security::AccessToken::AccessTokenKit;

JsFileExtAbility* JsFileExtAbility::Create(const std::unique_ptr<Runtime>& runtime)
{
    HILOG_INFO("%{public}s begin.", __func__);
    HILOG_INFO("%{public}s end.", __func__);
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
    HILOG_INFO("%{public}s begin.", __func__);
    Extension::OnStart(want);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());
    napi_value napiWant = OHOS::AppExecFwk::WrapWant(env, want);
    NativeValue* nativeWant = reinterpret_cast<NativeValue*>(napiWant);
    NativeValue* argv[] = {nativeWant};
    CallObjectMethod("onCreate", argv, ARGC_ONE);
    HILOG_INFO("%{public}s end.", __func__);
}

sptr<IRemoteObject> JsFileExtAbility::OnConnect(const AAFwk::Want &want)
{
    HILOG_INFO("%{public}s begin.", __func__);
    Extension::OnConnect(want);
    sptr<FileExtStubImpl> remoteObject = new (std::nothrow) FileExtStubImpl(
        std::static_pointer_cast<JsFileExtAbility>(shared_from_this()),
        reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine()));
    if (remoteObject == nullptr) {
        HILOG_ERROR("%{public}s No memory allocated for FileExtStubImpl", __func__);
        return nullptr;
    }
    HILOG_INFO("%{public}s end. ", __func__);
    return remoteObject->AsObject();
}

NativeValue* JsFileExtAbility::CallObjectMethod(const char* name, NativeValue* const* argv, size_t argc)
{
    HILOG_INFO("%{public}s name = %{public}s, begin", __func__, name);

    if (!jsObj_) {
        HILOG_WARN("%{public}s Not found FileExtAbility.js", __func__);
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
    HILOG_INFO("%{public}s, name = %{public}s, success", __func__, name);
    return handleScope.Escape(nativeEngine.CallFunction(value, method, argv, argc));
}

void JsFileExtAbility::GetSrcPath(std::string &srcPath)
{
    HILOG_INFO("%{public}s begin.", __func__);
    if (!Extension::abilityInfo_->isStageBasedModel) {
        /* temporary compatibility api8 + config.json */
        srcPath.append(Extension::abilityInfo_->package);
        srcPath.append("/assets/js/");
        if (!Extension::abilityInfo_->srcPath.empty()) {
            srcPath.append(Extension::abilityInfo_->srcPath);
        }
        srcPath.append("/").append(Extension::abilityInfo_->name).append(".abc");
        HILOG_INFO("%{public}s end1, srcPath:%{public}s", __func__, srcPath.c_str());
        return;
    }

    if (!Extension::abilityInfo_->srcEntrance.empty()) {
        srcPath.append(Extension::abilityInfo_->moduleName + "/");
        srcPath.append(Extension::abilityInfo_->srcEntrance);
        srcPath.erase(srcPath.rfind('.'));
        srcPath.append(".abc");
    }
    HILOG_INFO("%{public}s end2, srcPath:%{public}s", __func__, srcPath.c_str());
}

int JsFileExtAbility::OpenFile(const Uri &uri, int flags)
{
    HILOG_INFO("%{public}s begin.", __func__);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());

    napi_value napiUri = nullptr;
    napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    napi_value napiFlags = nullptr;
    napi_create_int32(env, flags, &napiFlags);

    NativeValue* nativeUri = reinterpret_cast<NativeValue*>(napiUri);
    NativeValue* nativeFlags = reinterpret_cast<NativeValue*>(napiFlags);
    NativeValue* argv[] = {nativeUri, nativeFlags};
    NativeValue* nativeResult = CallObjectMethod("openFile", argv, ARGC_TWO);
    int ret = -1;
    if (nativeResult == nullptr) {
        HILOG_ERROR("%{public}s call openFile with return null.", __func__);
        return ret;
    }
    ret = OHOS::AppExecFwk::UnwrapInt32FromJS(env, reinterpret_cast<napi_value>(nativeResult));
    HILOG_INFO("%{public}s end. return fd:%{public}d", __func__, ret);
    return ret;
}

int JsFileExtAbility::CreateFile(const Uri &parentUri, const std::string &displayName,  Uri &newFileUri)
{
    HILOG_INFO("%{public}s begin.", __func__);
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
        HILOG_ERROR("%{public}s call createFile with return null.", __func__);
        return ret;
    }
    std::string uriStr = OHOS::AppExecFwk::UnwrapStringFromJS(env, reinterpret_cast<napi_value>(nativeResult));
    if (uriStr.empty()) {
        HILOG_ERROR("%{public}s call Mkdir with return empty.", __func__);
        return ret;
    } else {
        ret = NO_ERROR;
    }
    newFileUri = Uri(uriStr);
    HILOG_INFO("%{public}s end. return fd:%{public}d, newFileUri = %{public}s", __func__, ret, uriStr.c_str());
    return ret;
}

int JsFileExtAbility::Mkdir(const Uri &parentUri, const std::string &displayName, Uri &newFileUri)
{
    HILOG_INFO("%{public}s begin.", __func__);
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
        HILOG_ERROR("%{public}s call Mkdir with return null.", __func__);
        return ret;
    }
    std::string uriStr = OHOS::AppExecFwk::UnwrapStringFromJS(env, reinterpret_cast<napi_value>(nativeResult));
    if (uriStr.empty()) {
        HILOG_ERROR("%{public}s call Mkdir with return empty.", __func__);
        return ret;
    } else {
        ret = NO_ERROR;
    }
    newFileUri = Uri(uriStr);
    HILOG_INFO("%{public}s end. return fd:%{public}d, newFileUri = %{public}s", __func__, ret, uriStr.c_str());
    return ret;
}

int JsFileExtAbility::Delete(const Uri &sourceFileUri)
{
    HILOG_INFO("%{public}s begin.", __func__);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());

    napi_value napiUri = nullptr;
    napi_create_string_utf8(env, sourceFileUri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);

    NativeValue* nativeUri = reinterpret_cast<NativeValue*>(napiUri);
    NativeValue* argv[] = {nativeUri};
    NativeValue* nativeResult = CallObjectMethod("delete", argv, ARGC_ONE);
    int ret = -1;
    if (nativeResult == nullptr) {
        HILOG_ERROR("%{public}s call delete with return null.", __func__);
        return ret;
    }
    ret = OHOS::AppExecFwk::UnwrapInt32FromJS(env, reinterpret_cast<napi_value>(nativeResult));
    HILOG_INFO("%{public}s end. return fd:%{public}d", __func__, ret);
    return ret;
}

int JsFileExtAbility::Rename(const Uri &sourceFileUri, const std::string &displayName, Uri &newFileUri)
{
    HILOG_INFO("%{public}s begin.", __func__);
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
        HILOG_ERROR("%{public}s call rename with return null.", __func__);
        return ret;
    }
    std::string uriStr = OHOS::AppExecFwk::UnwrapStringFromJS(env, reinterpret_cast<napi_value>(nativeResult));
    if (uriStr.empty()) {
        HILOG_ERROR("%{public}s call rename with return empty.", __func__);
        return ret;
    } else {
        ret = NO_ERROR;
    }
    newFileUri = Uri(uriStr);
    HILOG_INFO("%{public}s end. return fd:%{public}d, newFileUri = %{public}s", __func__, ret, uriStr.c_str());
    return ret;
}

std::vector<FileInfo> JsFileExtAbility::ListFile(const Uri &sourceFileUri)
{
    HILOG_INFO("%{public}s begin.", __func__);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());

    napi_value napiUri = nullptr;
    std::vector<FileInfo> vec;
    napi_create_string_utf8(env, sourceFileUri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);

    NativeValue* nativeUri = reinterpret_cast<NativeValue*>(napiUri);
    NativeValue* argv[] = {nativeUri};
    NativeValue* nativeResult = CallObjectMethod("listFile", argv, ARGC_ONE);
    if (nativeResult == nullptr) {
        HILOG_ERROR("%{public}s call listFile with return null.", __func__);
        return vec;
    }
    if(UnwrapArrayFileInfoFromJS(env, reinterpret_cast<napi_value>(nativeResult), vec)) {
        HILOG_INFO("%{public}s end vec.size:%{public}d.", __func__, vec.size());
        return vec;
    } else {
        HILOG_ERROR("%{public}s end with faild.", __func__);
        return vec;
    }
}

std::vector<DeviceInfo> JsFileExtAbility::GetRoots()
{
    HILOG_INFO("%{public}s begin.", __func__);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());

    std::vector<DeviceInfo> vec;
    NativeValue* argv[] = {};
    NativeValue* nativeResult = CallObjectMethod("getRoots", argv, ARGC_ZERO);
    if (nativeResult == nullptr) {
        HILOG_ERROR("%{public}s call getRoots with return null.", __func__);
        return vec;
    }
    if(UnwrapArrayDeviceInfoFromJS(env, reinterpret_cast<napi_value>(nativeResult), vec)) {
        HILOG_INFO("%{public}s end vec.size:%{public}d.", __func__, vec.size());
        return vec;
    } else {
        HILOG_ERROR("%{public}s end with faild.", __func__);
        return vec;
    }
}
} // namespace FileAccessFwk
} // namespace OHOS