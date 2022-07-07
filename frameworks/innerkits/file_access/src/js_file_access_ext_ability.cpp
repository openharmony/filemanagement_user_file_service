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

#include "js_file_access_ext_ability.h"

#include "ability_info.h"
#include "accesstoken_kit.h"
#include "extension_context.h"
#include "file_access_ext_stub_impl.h"
#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
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

JsFileAccessExtAbility* JsFileAccessExtAbility::Create(const std::unique_ptr<Runtime>& runtime)
{
    return new JsFileAccessExtAbility(static_cast<JsRuntime&>(*runtime));
}

JsFileAccessExtAbility::JsFileAccessExtAbility(JsRuntime& jsRuntime) : jsRuntime_(jsRuntime) {}
JsFileAccessExtAbility::~JsFileAccessExtAbility() = default;

void JsFileAccessExtAbility::Init(const std::shared_ptr<AbilityLocalRecord> &record,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Init");
    FileAccessExtAbility::Init(record, application, handler, token);
    std::string srcPath = "";
    GetSrcPath(srcPath);
    if (srcPath.empty()) {
        HILOG_ERROR("%{public}s Failed to get srcPath", __func__);
        return;
    }

    std::string moduleName(Extension::abilityInfo_->moduleName);
    moduleName.append("::").append(abilityInfo_->name);
    HandleScope handleScope(jsRuntime_);

    jsObj_ = jsRuntime_.LoadModule(moduleName, srcPath);
    if (jsObj_ == nullptr) {
        HILOG_ERROR("%{public}s Failed to get jsObj_", __func__);
        return;
    }

    NativeObject* obj = ConvertNativeValueTo<NativeObject>(jsObj_->Get());
    if (obj == nullptr) {
        HILOG_ERROR("%{public}s Failed to get JsFileAccessExtAbility object", __func__);
        return;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
}

void JsFileAccessExtAbility::OnStart(const AAFwk::Want &want)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "OnStart");
    Extension::OnStart(want);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());
    napi_value napiWant = OHOS::AppExecFwk::WrapWant(env, want);
    NativeValue* nativeWant = reinterpret_cast<NativeValue*>(napiWant);
    NativeValue* argv[] = {nativeWant};
    CallObjectMethod("onCreate", argv, ARGC_ONE);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
}

sptr<IRemoteObject> JsFileAccessExtAbility::OnConnect(const AAFwk::Want &want)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "OnConnect");
    Extension::OnConnect(want);
    sptr<FileAccessExtStubImpl> remoteObject = new (std::nothrow) FileAccessExtStubImpl(
        std::static_pointer_cast<JsFileAccessExtAbility>(shared_from_this()),
        reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine()));
    if (remoteObject == nullptr) {
        HILOG_ERROR("%{public}s No memory allocated for FileExtStubImpl", __func__);
        return nullptr;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return remoteObject->AsObject();
}

NativeValue* JsFileAccessExtAbility::CallObjectMethod(const char* name, NativeValue* const* argv, size_t argc)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CallObjectMethod");
    if (!jsObj_) {
        HILOG_ERROR("JsFileAccessExtAbility::CallObjectMethod jsObj Not found FileAccessExtAbility.js");
        return nullptr;
    }

    HandleScope handleScope(jsRuntime_);
    auto& nativeEngine = jsRuntime_.GetNativeEngine();

    NativeValue* value = jsObj_->Get();
    if (value == nullptr) {
        HILOG_ERROR("%{public}s Failed to get FileAccessExtAbility value", __func__);
        return nullptr;
    }

    NativeObject* obj = ConvertNativeValueTo<NativeObject>(value);
    if (obj == nullptr) {
        HILOG_ERROR("%{public}s Failed to get FileAccessExtAbility object", __func__);
        return nullptr;
    }

    NativeValue* method = obj->GetProperty(name);
    if (method == nullptr) {
        HILOG_ERROR("%{public}s Failed to get '%{public}s' from FileAccessExtAbility object", __func__, name);
        return nullptr;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return handleScope.Escape(nativeEngine.CallFunction(value, method, argv, argc));
}

static bool DoAsnycWork(CallbackParam *param)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "DoAsnycWork");
    if (param == nullptr || param->jsObj == nullptr) {
        HILOG_ERROR("Not found js file object");
        return false;
    }

    HandleScope handleScope(param->jsRuntime);
    NativeValue* value = param->jsObj->Get();
    if (value == nullptr) {
        HILOG_ERROR("Failed to get native value object");
        return false;
    }

    NativeObject* obj = ConvertNativeValueTo<NativeObject>(value);
    if (obj == nullptr) {
        HILOG_ERROR("Failed to get FileExtAbility object");
        return false;
    }

    NativeValue* method = obj->GetProperty(param->funcName);
    if (method == nullptr) {
        HILOG_ERROR("Failed to get '%{public}s' from FileExtAbility object",  param->funcName);
        return false;
    }

    auto& nativeEngine = param->jsRuntime.GetNativeEngine();
    param->result = handleScope.Escape(nativeEngine.CallFunction(value, method, param->argv, param->argc));
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return true;
}

NativeValue* JsFileAccessExtAbility::AsnycCallObjectMethod(const char* name, NativeValue* const* argv, size_t argc)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "AsnycCallObjectMethod");
    std::shared_ptr<ThreadLockInfo> lockInfo = std::make_shared<ThreadLockInfo>();
    std::shared_ptr<CallbackParam> param = std::make_shared<CallbackParam>(CallbackParam {
        .lockInfo = lockInfo.get(),
        .jsRuntime = jsRuntime_,
        .jsObj = jsObj_,
        .funcName = name,
        .argv = argv,
        .argc = argc,
        .result = nullptr
    });
    if (param == nullptr) {
        HILOG_ERROR("failed to new param");
        return nullptr;
    }
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine()), &loop);
    std::shared_ptr<uv_work_t> work = std::make_shared<uv_work_t>();
    if (work == nullptr) {
        HILOG_ERROR("failed to new uv_work_t");
        return nullptr;
    }
    work->data = reinterpret_cast<void *>(param.get());
    uv_queue_work(loop, work.get(), [](uv_work_t *work) {}, [](uv_work_t *work, int status) {
        CallbackParam *param = reinterpret_cast<CallbackParam *>(work->data);
        if (!DoAsnycWork(param)) {
            HILOG_ERROR("failed to call DoAsnycWork");
        }
        std::unique_lock<std::mutex> lock(param->lockInfo->fileOperateMutex);
        param->lockInfo->isReady = true;
        param->lockInfo->fileOperateCondition.notify_all();
    });

    std::unique_lock<std::mutex> lock(param->lockInfo->fileOperateMutex);
    param->lockInfo->fileOperateCondition.wait(lock, [param]() { return param->lockInfo->isReady; });
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return std::move(param->result);
}

void JsFileAccessExtAbility::GetSrcPath(std::string &srcPath)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetSrcPath");
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

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
}

int JsFileAccessExtAbility::OpenFile(const Uri &uri, int flags)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "OpenFile");
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());

    napi_value napiUri = nullptr;
    napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    napi_value napiFlags = nullptr;
    napi_create_int32(env, flags, &napiFlags);

    NativeValue* nativeUri = reinterpret_cast<NativeValue*>(napiUri);
    NativeValue* nativeFlags = reinterpret_cast<NativeValue*>(napiFlags);
    NativeValue* argv[] = {nativeUri, nativeFlags};
    NativeValue* nativeResult = AsnycCallObjectMethod("openFile", argv, ARGC_TWO);
    int ret = ERR_ERROR;
    if (nativeResult == nullptr) {
        HILOG_ERROR("%{public}s call openFile with return null.", __func__);
        return ret;
    }

    ret = OHOS::AppExecFwk::UnwrapInt32FromJS(env, reinterpret_cast<napi_value>(nativeResult));
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int JsFileAccessExtAbility::CreateFile(const Uri &parent, const std::string &displayName,  Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CreateFile");
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());

    napi_value napiParent = nullptr;
    napi_create_string_utf8(env, parent.ToString().c_str(), NAPI_AUTO_LENGTH, &napiParent);
    napi_value napiDisplayName = nullptr;
    napi_create_string_utf8(env, displayName.c_str(), NAPI_AUTO_LENGTH, &napiDisplayName);

    NativeValue* nativeParent = reinterpret_cast<NativeValue*>(napiParent);
    NativeValue* nativeDisplayName = reinterpret_cast<NativeValue*>(napiDisplayName);
    NativeValue* argv[] = {nativeParent, nativeDisplayName};
    NativeValue* nativeResult = AsnycCallObjectMethod("createFile", argv, ARGC_TWO);
    int ret = ERR_ERROR;
    if (nativeResult == nullptr) {
        HILOG_ERROR("%{public}s call createFile with return null.", __func__);
        return ret;
    }
    std::string uriStr = OHOS::AppExecFwk::UnwrapStringFromJS(env, reinterpret_cast<napi_value>(nativeResult));
    if (uriStr.empty()) {
        HILOG_ERROR("%{public}s call createFile with return empty.", __func__);
        return ret;
    } else {
        ret = NO_ERROR;
    }

    newFile = Uri(uriStr);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int JsFileAccessExtAbility::Mkdir(const Uri &parent, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Mkdir");
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());

    napi_value napiParent = nullptr;
    napi_create_string_utf8(env, parent.ToString().c_str(), NAPI_AUTO_LENGTH, &napiParent);
    napi_value napiDisplayName = nullptr;
    napi_create_string_utf8(env, displayName.c_str(), NAPI_AUTO_LENGTH, &napiDisplayName);

    NativeValue* nativeParent = reinterpret_cast<NativeValue*>(napiParent);
    NativeValue* nativeDisplayName = reinterpret_cast<NativeValue*>(napiDisplayName);
    NativeValue* argv[] = {nativeParent, nativeDisplayName};
    NativeValue* nativeResult = AsnycCallObjectMethod("mkdir", argv, ARGC_TWO);
    int ret = ERR_ERROR;
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

    newFile = Uri(uriStr);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int JsFileAccessExtAbility::Delete(const Uri &sourceFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Delete");
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());

    napi_value napiUri = nullptr;
    napi_create_string_utf8(env, sourceFile.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);

    NativeValue* nativeUri = reinterpret_cast<NativeValue*>(napiUri);
    NativeValue* argv[] = {nativeUri};
    NativeValue* nativeResult = AsnycCallObjectMethod("delete", argv, ARGC_ONE);
    int ret = ERR_ERROR;
    if (nativeResult == nullptr) {
        HILOG_ERROR("%{public}s call delete with return null.", __func__);
        return ret;
    }

    ret = OHOS::AppExecFwk::UnwrapInt32FromJS(env, reinterpret_cast<napi_value>(nativeResult));
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int JsFileAccessExtAbility::Move(const Uri &sourceFile, const Uri &targetParent, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Move");
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());

    napi_value napiSourceFile = nullptr;
    napi_create_string_utf8(env, sourceFile.ToString().c_str(), NAPI_AUTO_LENGTH, &napiSourceFile);
    napi_value napiTargetParent = nullptr;
    napi_create_string_utf8(env, targetParent.ToString().c_str(), NAPI_AUTO_LENGTH, &napiTargetParent);

    NativeValue* nativeSourceFile = reinterpret_cast<NativeValue*>(napiSourceFile);
    NativeValue* nativeTargetParent = reinterpret_cast<NativeValue*>(napiTargetParent);
    NativeValue* argv[] = {nativeSourceFile, nativeTargetParent};
    NativeValue* nativeResult = AsnycCallObjectMethod("move", argv, ARGC_TWO);
    int ret = ERR_ERROR;
    if (nativeResult == nullptr) {
        HILOG_ERROR("%{public}s call move with return null.", __func__);
        return ret;
    }
    std::string uriStr = OHOS::AppExecFwk::UnwrapStringFromJS(env, reinterpret_cast<napi_value>(nativeResult));
    if (uriStr.empty()) {
        HILOG_ERROR("%{public}s call move with return empty.", __func__);
        return ret;
    } else {
        ret = NO_ERROR;
    }

    newFile = Uri(uriStr);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int JsFileAccessExtAbility::Rename(const Uri &sourceFile, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Rename");
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());

    napi_value napiSourceFile = nullptr;
    napi_create_string_utf8(env, sourceFile.ToString().c_str(), NAPI_AUTO_LENGTH, &napiSourceFile);
    napi_value napiDisplayName = nullptr;
    napi_create_string_utf8(env, displayName.c_str(), NAPI_AUTO_LENGTH, &napiDisplayName);

    NativeValue* nativeSourceFile = reinterpret_cast<NativeValue*>(napiSourceFile);
    NativeValue* nativeDisplayName = reinterpret_cast<NativeValue*>(napiDisplayName);
    NativeValue* argv[] = {nativeSourceFile, nativeDisplayName};
    NativeValue* nativeResult = AsnycCallObjectMethod("rename", argv, ARGC_TWO);
    int ret = ERR_ERROR;
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

    newFile = Uri(uriStr);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

std::vector<FileInfo> JsFileAccessExtAbility::ListFile(const Uri &sourceFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "ListFile");
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());

    napi_value napiUri = nullptr;
    std::vector<FileInfo> vec;
    napi_create_string_utf8(env, sourceFile.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);

    NativeValue* nativeUri = reinterpret_cast<NativeValue*>(napiUri);
    NativeValue* argv[] = {nativeUri};
    NativeValue* nativeResult = AsnycCallObjectMethod("listFile", argv, ARGC_ONE);
    if (nativeResult == nullptr) {
        HILOG_ERROR("%{public}s call listFile with return null.", __func__);
        return vec;
    }
    if (UnwrapArrayFileInfoFromJS(env, reinterpret_cast<napi_value>(nativeResult), vec)) {
        return vec;
    } else {
        HILOG_ERROR("%{public}s end with faild.", __func__);
        return vec;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
}

std::vector<DeviceInfo> JsFileAccessExtAbility::GetRoots()
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetRoots");
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());

    std::vector<DeviceInfo> vec;
    NativeValue* argv[] = {};
    NativeValue* nativeResult = AsnycCallObjectMethod("getRoots", argv, ARGC_ZERO);
    if (nativeResult == nullptr) {
        HILOG_ERROR("%{public}s call getRoots with return null.", __func__);
        return vec;
    }
    if (UnwrapArrayDeviceInfoFromJS(env, reinterpret_cast<napi_value>(nativeResult), vec)) {
        return vec;
    } else {
        HILOG_ERROR("%{public}s end with faild.", __func__);
        return vec;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
}
} // namespace FileAccessFwk
} // namespace OHOS