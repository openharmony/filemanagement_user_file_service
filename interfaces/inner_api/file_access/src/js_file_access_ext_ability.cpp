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
#include "file_access_extension_info.h"
#include "file_access_framework_errno.h"
#include "file_access_observer_common.h"
#include "file_access_service_proxy.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "js_runtime.h"
#include "js_runtime_utils.h"
#include "n_error.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_common_util.h"
#include "napi_common_want.h"
#include "napi_remote_object.h"
#include "system_ability_definition.h"
#include "user_access_tracer.h"

namespace OHOS {
namespace FileAccessFwk {
namespace {
    constexpr size_t ARGC_ZERO = 0;
    constexpr size_t ARGC_ONE = 1;
    constexpr size_t ARGC_TWO = 2;
    constexpr size_t ARGC_THREE = 3;
    constexpr size_t ARGC_FOUR = 4;
    constexpr size_t MAX_ARG_COUNT = 5;
    constexpr int EXCEPTION = -1;
    constexpr int NOEXCEPTION = -2;
}

using namespace OHOS::AppExecFwk;
using namespace OHOS::AbilityRuntime;
using namespace OHOS::FileManagement::LibN;
using OHOS::Security::AccessToken::AccessTokenKit;

JsFileAccessExtAbility *JsFileAccessExtAbility::Create(const std::unique_ptr<Runtime> &runtime)
{
    return new JsFileAccessExtAbility(static_cast<JsRuntime &>(*runtime));
}

JsFileAccessExtAbility::JsFileAccessExtAbility(JsRuntime &jsRuntime) : jsRuntime_(jsRuntime) {}

JsFileAccessExtAbility::~JsFileAccessExtAbility()
{
    jsRuntime_.FreeNativeReference(std::move(jsObj_));
}

void JsFileAccessExtAbility::Init(const std::shared_ptr<AbilityLocalRecord> &record,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    UserAccessTracer trace;
    trace.Start("Init");
    FileAccessExtAbility::Init(record, application, handler, token);
    std::string srcPath = "";
    GetSrcPath(srcPath);
    if (srcPath.empty()) {
        HILOG_ERROR("Failed to get srcPath");
        return;
    }

    std::string moduleName(Extension::abilityInfo_->moduleName);
    moduleName.append("::").append(abilityInfo_->name);
    HandleScope handleScope(jsRuntime_);

    jsObj_ = jsRuntime_.LoadModule(moduleName, srcPath, abilityInfo_->hapPath,
        abilityInfo_->compileMode == AbilityRuntime::CompileMode::ES_MODULE);
    if (jsObj_ == nullptr) {
        HILOG_ERROR("Failed to get jsObj_");
        return;
    }

    if (jsObj_->GetNapiValue() == nullptr) {
        HILOG_ERROR("Failed to get JsFileAccessExtAbility value");
        return;
    }
}

void JsFileAccessExtAbility::OnStart(const AAFwk::Want &want)
{
    UserAccessTracer trace;
    trace.Start("OnStart");
    Extension::OnStart(want);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());
    napi_value napiWant = OHOS::AppExecFwk::WrapWant(env, want);
    napi_value argv[] = { napiWant };
    CallObjectMethod("onCreate", argv, ARGC_ONE);
}

sptr<IRemoteObject> JsFileAccessExtAbility::OnConnect(const AAFwk::Want &want)
{
    UserAccessTracer trace;
    trace.Start("OnConnect");
    Extension::OnConnect(want);
    sptr<FileAccessExtStubImpl> remoteObject(
        new (std::nothrow) FileAccessExtStubImpl(std::static_pointer_cast<JsFileAccessExtAbility>(shared_from_this()),
        reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine())));
    if (remoteObject == nullptr) {
        HILOG_ERROR("No memory allocated for FileExtStubImpl");
        return nullptr;
    }

    return remoteObject->AsObject();
}

napi_value JsFileAccessExtAbility::CallObjectMethod(const char *name, napi_value const *argv, size_t argc)
{
    UserAccessTracer trace;
    trace.Start("CallObjectMethod");
    if (!jsObj_) {
        HILOG_ERROR("JsFileAccessExtAbility::CallObjectMethod jsObj Not found FileAccessExtAbility.js");
        return nullptr;
    }

    HandleEscape handleEscape(jsRuntime_);
    auto &nativeEngine = jsRuntime_.GetNativeEngine();
    auto env = reinterpret_cast<napi_env>(&nativeEngine);

    napi_value value = jsObj_->GetNapiValue();
    if (value == nullptr) {
        HILOG_ERROR("Failed to get FileAccessExtAbility value");
        return nullptr;
    }

    napi_value method = nullptr;
    napi_get_named_property(env, value, name, &method);
    if (method == nullptr) {
        HILOG_ERROR("Failed to get '%{public}s' from FileAccessExtAbility object", name);
        return nullptr;
    }

    napi_value result = nullptr;
    if (napi_call_function(env, value, method, argc, argv, &result) != napi_ok) {
        HILOG_ERROR("Call function fail");
        return nullptr;
    }
    return handleEscape.Escape(result);
}

static int DoCallJsMethod(CallJsParam *param)
{
    UserAccessTracer trace;
    trace.Start("DoCallJsMethod");
    JsRuntime *jsRuntime = param->jsRuntime;
    if (jsRuntime == nullptr) {
        HILOG_ERROR("failed to get jsRuntime.");
        return EINVAL;
    }

    HandleEscape handleEscape(*jsRuntime);
    auto &nativeEngine = jsRuntime->GetNativeEngine();
    auto env = reinterpret_cast<napi_env>(&nativeEngine);
    size_t argc = 0;
    napi_value argv[MAX_ARG_COUNT] = { nullptr };
    if (param->argParser != nullptr) {
        if (!param->argParser(env, argv, argc)) {
            HILOG_ERROR("failed to get params.");
            return EINVAL;
        }
    }

    napi_value value = nullptr;
    auto ref = reinterpret_cast<napi_ref>(param->jsObj);
    napi_get_reference_value(env, ref, &value);
    if (value == nullptr) {
        HILOG_ERROR("failed to get native value object.");
        return EINVAL;
    }
    napi_value method = nullptr;
    napi_get_named_property(env, value, param->funcName.c_str(), &method);
    if (method == nullptr) {
        HILOG_ERROR("failed to get %{public}s from FileExtAbility object.", param->funcName.c_str());
        return EINVAL;
    }
    if (param->retParser == nullptr) {
        HILOG_ERROR("ResultValueParser must not null.");
        return EINVAL;
    }
    napi_value result = nullptr;
    napi_call_function(env, value, method, argc, argv, &result);
    if (result == nullptr) {
        HILOG_ERROR("Napi call function fail.");
        return E_GETRESULT;
    }
    if (!param->retParser(env, handleEscape.Escape(result))) {
        HILOG_ERROR("Parser js result fail.");
        return E_GETRESULT;
    }
    return ERR_OK;
}

int JsFileAccessExtAbility::CallJsMethod(const std::string &funcName, JsRuntime &jsRuntime, NativeReference *jsObj,
    InputArgsParser argParser, ResultValueParser retParser)
{
    UserAccessTracer trace;
    trace.Start("CallJsMethod");
    uv_loop_s *loop = nullptr;
    napi_status status = napi_get_uv_event_loop(reinterpret_cast<napi_env>(&jsRuntime.GetNativeEngine()), &loop);
    if (status != napi_ok) {
        HILOG_ERROR("failed to get uv event loop.");
        return EINVAL;
    }
    auto param = std::make_shared<CallJsParam>(funcName, &jsRuntime, jsObj, argParser, retParser);
    if (param == nullptr) {
        HILOG_ERROR("failed to new param.");
        return EINVAL;
    }
    auto work = std::make_shared<uv_work_t>();
    if (work == nullptr) {
        HILOG_ERROR("failed to new uv_work_t.");
        return EINVAL;
    }
    work->data = reinterpret_cast<void *>(param.get());
    int ret = uv_queue_work(
        loop, work.get(), [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            CallJsParam *param = reinterpret_cast<CallJsParam *>(work->data);
            napi_handle_scope scope = nullptr;
            napi_env env = reinterpret_cast<napi_env>(&(param->jsRuntime->GetNativeEngine()));
            napi_open_handle_scope(env, &scope);
            do {
                if (param == nullptr) {
                    HILOG_ERROR("failed to get CallJsParam.");
                    break;
                }
                if (DoCallJsMethod(param) != ERR_OK) {
                    HILOG_ERROR("failed to call DoCallJsMethod.");
                }
            } while (false);
            std::unique_lock<std::mutex> lock(param->fileOperateMutex);
            param->isReady = true;
            param->fileOperateCondition.notify_one();
            napi_close_handle_scope(env, scope);
        });
    if (ret != 0) {
        HILOG_ERROR("failed to exec uv_queue_work.");
        return EINVAL;
    }
    std::unique_lock<std::mutex> lock(param->fileOperateMutex);
    param->fileOperateCondition.wait(lock, [param]() { return param->isReady; });
    return ERR_OK;
}

void JsFileAccessExtAbility::GetSrcPath(std::string &srcPath)
{
    UserAccessTracer trace;
    trace.Start("GetSrcPath");
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

int JsFileAccessExtAbility::OpenFile(const Uri &uri, int flags, int &fd)
{
    UserAccessTracer trace;
    trace.Start("OpenFile");
    auto value = std::make_shared<Value<int>>();
    if (value == nullptr) {
        HILOG_ERROR("OpenFile value is nullptr.");
        return E_GETRESULT;
    }

    auto argParser = [uri, flags](napi_env &env, napi_value argv[], size_t &argc) -> bool {
        napi_value nativeUri = nullptr;
        napi_create_string_utf8(env, uri.ToString().c_str(), uri.ToString().length(), &nativeUri);
        napi_value nativeFlags = nullptr;
        napi_create_int32(env, flags, &nativeFlags);
        if (nativeUri == nullptr || nativeFlags == nullptr) {
            HILOG_ERROR("create uri or flags native js value fail.");
            return false;
        }
        argv[ARGC_ZERO] = nativeUri;
        argv[ARGC_ONE] = nativeFlags;
        argc = ARGC_TWO;
        return true;
    };
    auto retParser = [value](napi_env &env, napi_value result) -> bool {
        if (GetFdAndCodeFromJs(env, result, value) != napi_ok) {
            HILOG_ERROR("Convert js object fail.");
            return false;
        }
        return true;
    };

    auto errCode = CallJsMethod("openFile", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        return errCode;
    }

    if (value->code != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return value->code;
    }

    fd = value->data;
    if (fd < ERR_OK) {
        HILOG_ERROR("Failed to get file descriptor fd: %{public}d", fd);
        return E_GETRESULT;
    }
    return ERR_OK;
}

int JsFileAccessExtAbility::CreateFile(const Uri &parent, const std::string &displayName, Uri &newFile)
{
    UserAccessTracer trace;
    trace.Start("CreateFile");
    auto value = std::make_shared<Value<std::string>>();
    if (!value) {
        HILOG_ERROR("CreateFile value is nullptr.");
        return E_GETRESULT;
    }

    auto argParser = [parent, displayName](napi_env &env, napi_value argv[], size_t &argc) -> bool {
        napi_value nativeParent = nullptr;
        napi_create_string_utf8(env, parent.ToString().c_str(), parent.ToString().length(), &nativeParent);
        napi_value nativeDisplayName = nullptr;
        napi_create_string_utf8(env, displayName.c_str(), displayName.length(), &nativeDisplayName);
        if (nativeParent == nullptr || nativeDisplayName == nullptr) {
            HILOG_ERROR("create parent uri or displayName native js value fail.");
            return false;
        }
        argv[ARGC_ZERO] = nativeParent;
        argv[ARGC_ONE] = nativeDisplayName;
        argc = ARGC_TWO;
        return true;
    };
    auto retParser = [value](napi_env &env, napi_value result) -> bool {
        if (GetUriAndCodeFromJs(env, result, value) != napi_ok) {
            HILOG_ERROR("Convert js object fail.");
            return false;
        }
        return true;
    };

    auto errCode = CallJsMethod("createFile", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        return errCode;
    }

    if (value->code != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return value->code;
    }

    if ((value->data).empty()) {
        HILOG_ERROR("call CreateFile with return empty.");
        return E_GETRESULT;
    }

    newFile = Uri(value->data);
    return ERR_OK;
}

int JsFileAccessExtAbility::Mkdir(const Uri &parent, const std::string &displayName, Uri &newFile)
{
    UserAccessTracer trace;
    trace.Start("Mkdir");
    auto value = std::make_shared<Value<std::string>>();
    if (!value) {
        HILOG_ERROR("Mkdir value is nullptr.");
        return E_GETRESULT;
    }

    auto argParser = [parent, displayName](napi_env &env, napi_value *argv, size_t &argc) -> bool {
        napi_value nativeParent = nullptr;
        napi_create_string_utf8(env, parent.ToString().c_str(), parent.ToString().length(), &nativeParent);
        napi_value nativeDisplayName = nullptr;
        napi_create_string_utf8(env, displayName.c_str(), displayName.length(), &nativeDisplayName);
        if (nativeParent == nullptr || nativeDisplayName == nullptr) {
            HILOG_ERROR("create parent uri native js value fail.");
            return false;
        }
        argv[ARGC_ZERO] = nativeParent;
        argv[ARGC_ONE] = nativeDisplayName;
        argc = ARGC_TWO;
        return true;
    };

    auto retParser = [value](napi_env &env, napi_value result) -> bool {
        if (GetUriAndCodeFromJs(env, result, value) != napi_ok) {
            HILOG_ERROR("Convert js object fail.");
            return false;
        }
        return true;
    };

    auto errCode = CallJsMethod("mkdir", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        return errCode;
    }

    if (value->code != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return value->code;
    }

    if ((value->data).empty()) {
        HILOG_ERROR("call Mkdir with return empty.");
        return E_GETRESULT;
    }
    newFile = Uri(value->data);
    return ERR_OK;
}

int JsFileAccessExtAbility::Delete(const Uri &sourceFile)
{
    UserAccessTracer trace;
    trace.Start("Delete");
    auto ret = std::make_shared<int>();
    if (!ret) {
        HILOG_ERROR("Delete value is nullptr.");
        return E_GETRESULT;
    }

    auto argParser = [uri = sourceFile](napi_env &env, napi_value *argv, size_t &argc) -> bool {
        napi_value nativeUri = nullptr;
        napi_create_string_utf8(env, uri.ToString().c_str(), uri.ToString().length(), &nativeUri);
        if (nativeUri == nullptr) {
            HILOG_ERROR("create sourceFile uri native js value fail.");
            return false;
        }
        argv[ARGC_ZERO] = nativeUri;
        argc = ARGC_ONE;
        return true;
    };

    auto retParser = [ret](napi_env &env, napi_value result) -> bool {
        if (napi_get_value_int32(env, result, ret.get()) != napi_ok) {
            HILOG_ERROR("Convert js value fail.");
            return false;
        }
        return true;
    };

    auto errCode = CallJsMethod("delete", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        return errCode;
    }

    if (*ret != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return *ret;
    }

    return ERR_OK;
}

int JsFileAccessExtAbility::Move(const Uri &sourceFile, const Uri &targetParent, Uri &newFile)
{
    UserAccessTracer trace;
    trace.Start("Move");
    auto value = std::make_shared<Value<std::string>>();
    if (value == nullptr) {
        HILOG_ERROR("Move value is nullptr.");
        return E_GETRESULT;
    }

    auto argParser = [sourceFile, targetParent](napi_env &env, napi_value *argv, size_t &argc) -> bool {
        napi_value srcUri = nullptr;
        napi_create_string_utf8(env, sourceFile.ToString().c_str(), sourceFile.ToString().length(), &srcUri);
        napi_value dstUri = nullptr;
        napi_create_string_utf8(env, targetParent.ToString().c_str(), targetParent.ToString().length(), &dstUri);
        if (srcUri == nullptr || dstUri == nullptr) {
            HILOG_ERROR("create sourceFile uri native js value fail.");
            return false;
        }
        argv[ARGC_ZERO] = srcUri;
        argv[ARGC_ONE] = dstUri;
        argc = ARGC_TWO;
        return true;
    };

    auto retParser = [value](napi_env &env, napi_value result) -> bool {
        if (GetUriAndCodeFromJs(env, result, value) != napi_ok) {
            HILOG_ERROR("Convert js object fail.");
            return false;
        }
        return true;
    };

    auto errCode = CallJsMethod("move", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        return errCode;
    }

    if (value->code != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return value->code;
    }

    if ((value->data).empty()) {
        HILOG_ERROR("call move with return empty.");
        return E_GETRESULT;
    }
    newFile = Uri(value->data);
    return ERR_OK;
}

static void TranslateResult(Result &result)
{
    if (errCodeTable.find(result.errCode) != errCodeTable.end()) {
        result.errCode = errCodeTable.at(result.errCode).first;
        if (result.errMsg.empty()) {
            result.errMsg = errCodeTable.at(result.errCode).second;
        }
    }
}

static bool GetResultByJs(napi_env &env, napi_value nativeResult, Result &result, const int &ret)
{
    UserAccessTracer trace;
    trace.Start("GetResultsByJs");
    if (nativeResult == nullptr) {
        HILOG_ERROR("Convert js object fail.");
        return false;
    }

    if (ret == NOEXCEPTION) {
        napi_value sourceUri = nullptr;
        napi_get_named_property(env, nativeResult, "sourceUri", &sourceUri);
        if (GetStringValue(env, sourceUri, result.sourceUri) != napi_ok) {
            HILOG_ERROR("Convert sourceUri fail.");
            return false;
        }

        napi_value destUri = nullptr;
        napi_get_named_property(env, nativeResult, "destUri", &destUri);
        if (GetStringValue(env, destUri, result.destUri) != napi_ok) {
            HILOG_ERROR("Convert destUri fail.");
            return false;
        }
    }
    if ((ret == NOEXCEPTION) || (ret == EXCEPTION)) {
        napi_value errCode = nullptr;
        napi_get_named_property(env, nativeResult, "errCode", &errCode);
        if (napi_get_value_int32(env, errCode, &result.errCode) != napi_ok) {
            HILOG_ERROR("Convert errCode fail.");
            return false;
        }
    }
    return true;
}

static bool ParserGetJsResult(napi_env &env, napi_value nativeValue, std::vector<Result> &result,
    int &copyRet)
{
    UserAccessTracer trace;
    trace.Start("ParserGetJsResult");
    if (nativeValue == nullptr) {
        HILOG_ERROR("Convert js object fail.");
        return false;
    }

    napi_value code = nullptr;
    napi_get_named_property(env, nativeValue, "code", &code);
    if (napi_get_value_int32(env, code, &copyRet) != napi_ok) {
        HILOG_ERROR("Convert js value fail.");
        return false;
    }

    if (copyRet == ERR_OK) {
        return true;
    }

    napi_value nativeArray = nullptr;
    napi_create_array(env, &nativeArray);
    napi_get_named_property(env, nativeValue, "results", &nativeArray);
    if (nativeArray == nullptr) {
        HILOG_ERROR("nativeArray is nullptr");
        return false;
    }

    uint32_t length = 0;
    if (napi_get_array_length(env, nativeArray, &length) != napi_ok) {
        HILOG_ERROR("Get nativeArray length fail");
        return false;
    }

    for (uint32_t i = 0; i < length; i++) {
        napi_value nativeResult = nullptr;
        napi_get_element(env, nativeArray, i, &nativeResult);
        if (nativeResult == nullptr) {
            HILOG_ERROR("get native FileInfo fail.");
            return false;
        }

        Result res;
        bool ret = GetResultByJs(env, nativeResult, res, copyRet);
        if (ret) {
            TranslateResult(res);
            result.push_back(res);
        }
    }

    return true;
}

int JsFileAccessExtAbility::Copy(const Uri &sourceUri, const Uri &destUri, std::vector<Result> &copyResult,
    bool force)
{
    UserAccessTracer trace;
    trace.Start("Copy");
    auto argParser = [sourceUri, destUri, force](napi_env &env, napi_value *argv, size_t &argc) -> bool {
        napi_value srcNativeUri = nullptr;
        napi_create_string_utf8(env, sourceUri.ToString().c_str(), sourceUri.ToString().length(), &srcNativeUri);

        napi_value dstNativeUri = nullptr;
        napi_create_string_utf8(env, destUri.ToString().c_str(), destUri.ToString().length(), &dstNativeUri);

        napi_value forceCopy = nullptr;
        napi_get_boolean(env, force, &forceCopy);
        if (srcNativeUri == nullptr || dstNativeUri == nullptr || forceCopy == nullptr) {
            HILOG_ERROR("create arguments native js value fail.");
            return false;
        }
        argv[ARGC_ZERO] = srcNativeUri;
        argv[ARGC_ONE] = dstNativeUri;
        argv[ARGC_TWO] = forceCopy;
        argc = ARGC_THREE;
        return true;
    };

    int copyRet = EXCEPTION;
    auto retParser = [&copyResult, &copyRet](napi_env &env, napi_value result) -> bool {
        return ParserGetJsResult(env, result, copyResult, copyRet);
    };

    auto errCode = CallJsMethod("copy", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        Result result{ "", "", errCode, "" };
        TranslateResult(result);
        copyResult.push_back(result);
        return EXCEPTION;
    }

    return copyRet;
}

int JsFileAccessExtAbility::Rename(const Uri &sourceFile, const std::string &displayName, Uri &newFile)
{
    UserAccessTracer trace;
    trace.Start("Rename");
    auto value = std::make_shared<Value<std::string>>();
    if (value == nullptr) {
        HILOG_ERROR("Rename value is nullptr.");
        return E_GETRESULT;
    }
    auto argParser = [sourceFile, displayName](napi_env &env, napi_value *argv, size_t &argc) -> bool {
        napi_value nativeSourceFile = nullptr;
        napi_create_string_utf8(env, sourceFile.ToString().c_str(), sourceFile.ToString().length(), &nativeSourceFile);

        napi_value nativeDisplayName = nullptr;
        napi_create_string_utf8(env, displayName.c_str(), displayName.length(), &nativeDisplayName);
        if (nativeSourceFile == nullptr || nativeDisplayName == nullptr) {
            HILOG_ERROR("create sourceFile uri or displayName native js value fail.");
            return false;
        }
        argv[ARGC_ZERO] = nativeSourceFile;
        argv[ARGC_ONE] = nativeDisplayName;
        argc = ARGC_TWO;
        return true;
    };

    auto retParser = [value](napi_env &env, napi_value result) -> bool {
        if (GetUriAndCodeFromJs(env, result, value) != napi_ok) {
            HILOG_ERROR("Convert js object fail.");
            return false;
        }
        return true;
    };

    auto errCode = CallJsMethod("rename", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        return errCode;
    }

    if (value->code != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return value->code;
    }

    if ((value->data).empty()) {
        HILOG_ERROR("call Rename with return empty.");
        return E_GETRESULT;
    }
    newFile = Uri(value->data);
    return ERR_OK;
}

bool JsFileAccessExtAbility::ParserListFileJsResult(napi_env &env, napi_value nativeValue,
    Value<std::vector<FileInfo>> &result)
{
    napi_value code = nullptr;
    napi_get_named_property(env, nativeValue, "code", &code);
    if (napi_get_value_int32(env, code, &result.code) != napi_ok) {
        HILOG_ERROR("Convert code fail.");
        return false;
    }

    napi_value nativeArray = nullptr;
    napi_create_array(env, &nativeArray);
    napi_get_named_property(env, nativeValue, "infos", &nativeArray);
    if (nativeArray == nullptr) {
        HILOG_ERROR("Convert js array object fail.");
        return false;
    }

    uint32_t length = 0;
    if (napi_get_array_length(env, nativeArray, &length) != napi_ok) {
        HILOG_ERROR("Get nativeArray length fail.");
        return false;
    }
    for (uint32_t i = 0; i < length; i++) {
        napi_value nativeFileInfo = nullptr;
        napi_get_element(env, nativeArray, i, &nativeFileInfo);
        if (nativeFileInfo == nullptr) {
            HILOG_ERROR("get native FileInfo fail.");
            return false;
        }

        FileInfo fileInfo;
        if (GetFileInfoFromJs(env, nativeFileInfo, fileInfo) != napi_ok) {
            HILOG_ERROR("Convert fileInfo js value fail.");
            return false;
        }

        result.data.emplace_back(std::move(fileInfo));
    }
    return true;
}

int JsFileAccessExtAbility::MakeStringNativeArray(napi_env &env, std::vector<std::string> &inputArray,
    napi_value resultArray)
{
    if (resultArray == nullptr) {
        HILOG_ERROR("Create NativeArray nullptr");
        return E_GETRESULT;
    }

    for (uint32_t i = 0; i < inputArray.size(); i++) {
        napi_value nativeValue = nullptr;
        napi_create_string_utf8(env, inputArray[i].c_str(), inputArray[i].length(), &nativeValue);
        if (nativeValue == nullptr) {
            HILOG_ERROR("Create NativeValue fail.");
            return E_GETRESULT;
        }

        if (napi_set_element(env, resultArray, i, nativeValue) != napi_ok) {
            HILOG_ERROR("Add NativeValue to NativeArray fail.");
            return E_IPCS;
        }
    }

    return ERR_OK;
}

int JsFileAccessExtAbility::MakeJsNativeFileFilter(napi_env &env, const FileFilter &filter, napi_value nativeFilter)
{
    napi_value suffixArray = nullptr;
    napi_create_array_with_length(env, filter.GetSuffix().size(), &suffixArray);
    if (suffixArray == nullptr) {
        HILOG_ERROR("Create Suffix native array value fail.");
        return E_GETRESULT;
    }

    std::vector<std::string> suffixVec = filter.GetSuffix();
    int errorCode = MakeStringNativeArray(env, suffixVec, suffixArray);
    if (errorCode != ERR_OK) {
        HILOG_ERROR("Create Suffix native array value fail, code:%{public}d.", errorCode);
        return errorCode;
    }

    napi_value displayNameArray = nullptr;
    napi_create_array_with_length(env, filter.GetDisplayName().size(), &displayNameArray);
    if (displayNameArray == nullptr) {
        HILOG_ERROR("Create DisplayName native array value fail.");
        return E_GETRESULT;
    }

    std::vector<std::string> displayNameVec = filter.GetDisplayName();
    errorCode = MakeStringNativeArray(env, displayNameVec, displayNameArray);
    if (errorCode != ERR_OK) {
        HILOG_ERROR("Create DisplayName native array value fail, code:%{public}d.", errorCode);
        return errorCode;
    }

    napi_value mimeTypeArray = nullptr;
    napi_create_array_with_length(env, filter.GetMimeType().size(), &mimeTypeArray);
    if (mimeTypeArray == nullptr) {
        HILOG_ERROR("Create MimeType native array value fail.");
        return E_GETRESULT;
    }

    std::vector<std::string> mimeTypeVec = filter.GetMimeType();
    errorCode = MakeStringNativeArray(env, mimeTypeVec, mimeTypeArray);
    if (errorCode != ERR_OK) {
        HILOG_ERROR("Create MimeType native array value fail, code:%{public}d.", errorCode);
        return errorCode;
    }

    napi_value nativeFileSizeOver = nullptr;
    napi_create_int64(env, filter.GetFileSizeOver(), &nativeFileSizeOver);
    if (nativeFileSizeOver == nullptr) {
        HILOG_ERROR("Create NativeFileSizeOver native js value fail.");
        return E_GETRESULT;
    }

    napi_value nativeLastModifiedAfter = nullptr;
    napi_create_double(env, filter.GetLastModifiedAfter(), &nativeLastModifiedAfter);
    if (nativeLastModifiedAfter == nullptr) {
        HILOG_ERROR("Create NativeLastModifiedAfter native js value fail.");
        return E_GETRESULT;
    }

    napi_value nativeExcludeMedia = nullptr;
    napi_get_boolean(env, filter.GetExcludeMedia(), &nativeExcludeMedia);
    if (nativeExcludeMedia == nullptr) {
        HILOG_ERROR("Create NativeExcludeMedia native js value fail.");
        return E_GETRESULT;
    }

    if (napi_set_named_property(env, nativeFilter, "suffix", suffixArray) != napi_ok) {
        HILOG_ERROR("Set suffix property to Filter NativeValue fail.");
        return EINVAL;
    }

    if (napi_set_named_property(env, nativeFilter, "displayName", displayNameArray) != napi_ok) {
        HILOG_ERROR("Set displayName property to Filter NativeValue fail.");
        return EINVAL;
    }

    if (napi_set_named_property(env, nativeFilter, "mimeType", mimeTypeArray) != napi_ok) {
        HILOG_ERROR("Set mimeType property to Filter NativeValue fail.");
        return EINVAL;
    }

    if (napi_set_named_property(env, nativeFilter, "fileSizeOver", nativeFileSizeOver) != napi_ok) {
        HILOG_ERROR("Set fileSizeOver property to Filter NativeValue fail.");
        return EINVAL;
    }

    if (napi_set_named_property(env, nativeFilter, "lastModifiedAfter", nativeLastModifiedAfter) != napi_ok) {
        HILOG_ERROR("Set lastModifiedAfter property to Filter NativeValue fail.");
        return EINVAL;
    }

    if (napi_set_named_property(env, nativeFilter, "excludeMedia", nativeExcludeMedia) != napi_ok) {
        HILOG_ERROR("Set excludeMedia property to Filter NativeValue fail.");
        return EINVAL;
    }
    return ERR_OK;
}

bool JsFileAccessExtAbility::BuildFilterParam(napi_env &env, const FileFilter &filter, const FilterParam &param,
    napi_value *argv, size_t &argc)
{
    string uriStr = param.fileInfo.uri;
    napi_value uri = nullptr;
    napi_create_string_utf8(env, uriStr.c_str(), uriStr.length(), &uri);
    if (uri == nullptr) {
        HILOG_ERROR("Create sourceFile uri native js value fail.");
        return false;
    }

    napi_value nativeOffset = nullptr;
    napi_create_int64(env, param.offset, &nativeOffset);
    if (nativeOffset == nullptr) {
        HILOG_ERROR("Create offset native js value fail.");
        return false;
    }

    napi_value nativeMaxCount = nullptr;
    napi_create_int64(env, param.maxCount, &nativeMaxCount);
    if (nativeMaxCount == nullptr) {
        HILOG_ERROR("Create maxCount native js value fail.");
        return false;
    }

    napi_value nativeFilter = nullptr;
    if (filter.GetHasFilter()) {
        napi_create_object(env, &nativeFilter);
        if (nativeFilter == nullptr) {
            HILOG_ERROR("Create js NativeValue fail.");
            return false;
        }
        int ret = MakeJsNativeFileFilter(env, filter, nativeFilter);
        if (ret != ERR_OK) {
            HILOG_ERROR("Create js NativeValue fail.");
            return false;
        }
    } else {
        nativeFilter = nullptr;
        napi_get_null(env, &nativeFilter);
        if (nativeFilter == nullptr) {
            HILOG_ERROR("Create js NativeValue fail.");
            return false;
        }
    }

    argv[ARGC_ZERO] = uri;
    argv[ARGC_ONE] = nativeOffset;
    argv[ARGC_TWO] = nativeMaxCount;
    argv[ARGC_THREE] = nativeFilter;
    argc = ARGC_FOUR;
    return true;
}

int JsFileAccessExtAbility::ListFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
    const FileFilter &filter, std::vector<FileInfo> &fileInfoVec)
{
    UserAccessTracer trace;
    trace.Start("ListFile");
    auto value = std::make_shared<Value<std::vector<FileInfo>>>();
    if (value == nullptr) {
        HILOG_ERROR("ListFile value is nullptr.");
        return E_GETRESULT;
    }

    auto argParser = [fileInfo, offset, maxCount, filter](napi_env &env, napi_value *argv, size_t &argc) -> bool {
        struct FilterParam param;
        param.fileInfo = fileInfo;
        param.offset = offset;
        param.maxCount = maxCount;

        return BuildFilterParam(env, filter, param, argv, argc);
    };

    auto retParser = [this, value](napi_env &env, napi_value result) -> bool {
        Value<std::vector<FileInfo>> fileInfo;
        bool ret = ParserListFileJsResult(env, result, fileInfo);
        if (!ret) {
            HILOG_ERROR("Parser js value fail.");
            return ret;
        }

        *value = std::move(fileInfo);
        return true;
    };

    auto errCode = CallJsMethod("listFile", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        return errCode;
    }

    if (value->code != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return value->code;
    }

    fileInfoVec = std::move(value->data);
    return ERR_OK;
}

int JsFileAccessExtAbility::ScanFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
    const FileFilter &filter, std::vector<FileInfo> &fileInfoVec)
{
    UserAccessTracer trace;
    trace.Start("ScanFile");
    auto value = std::make_shared<Value<std::vector<FileInfo>>>();
    if (value == nullptr) {
        HILOG_ERROR("ScanFile value is nullptr.");
        return E_GETRESULT;
    }

    auto argParser = [fileInfo, offset, maxCount, filter](napi_env &env, napi_value *argv, size_t &argc) -> bool {
        struct FilterParam param;
        param.fileInfo = fileInfo;
        param.offset = offset;
        param.maxCount = maxCount;

        return BuildFilterParam(env, filter, param, argv, argc);
    };

    auto retParser = [value](napi_env &env, napi_value result) -> bool {
        Value<std::vector<FileInfo>> fileInfo;
        bool ret = ParserListFileJsResult(env, result, fileInfo);
        if (!ret) {
            HILOG_ERROR("Parser js value fail.");
            return ret;
        }

        *value = std::move(fileInfo);
        return true;
    };

    auto errCode = CallJsMethod("scanFile", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        return errCode;
    }

    if (value->code != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return value->code;
    }

    fileInfoVec = std::move(value->data);
    return ERR_OK;
}

bool JsFileAccessExtAbility::ParserGetRootsJsResult(napi_env &env, napi_value nativeValue,
    Value<std::vector<RootInfo>> &result)
{
    napi_value code = nullptr;
    napi_get_named_property(env, nativeValue, "code", &code);
    if (napi_get_value_int32(env, code, &result.code) != napi_ok) {
        HILOG_ERROR("Get code fail");
        return false;
    }

    napi_value nativeArray = nullptr;
    napi_create_array(env, &nativeArray);
    napi_get_named_property(env, nativeValue, "roots", &nativeArray);
    if (nativeArray == nullptr) {
        HILOG_ERROR("nativeArray is nullptr");
        return false;
    }

    uint32_t length = 0;
    if (napi_get_array_length(env, nativeArray, &length) != napi_ok) {
        HILOG_ERROR("Get nativeArray length fail");
        return false;
    }

    for (uint32_t i = 0; i < length; i++) {
        napi_value nativeRootInfo = nullptr;
        napi_get_element(env, nativeArray, i, &nativeRootInfo);
        if (nativeRootInfo == nullptr) {
            HILOG_ERROR("Get native FileInfo fail.");
            return false;
        }

        RootInfo rootInfo;
        if (GetRootInfo(env, nativeRootInfo, rootInfo) != napi_ok) {
            HILOG_ERROR("Get native rootInfo fail.");
            return false;
        }

        result.data.emplace_back(std::move(rootInfo));
    }
    return true;
}

int JsFileAccessExtAbility::GetRoots(std::vector<RootInfo> &rootInfoVec)
{
    UserAccessTracer trace;
    trace.Start("GetRoots");
    auto value = std::make_shared<Value<std::vector<RootInfo>>>();
    if (value == nullptr) {
        HILOG_ERROR("GetRoots value is nullptr.");
        return E_GETRESULT;
    }

    auto argParser = [](napi_env &env, napi_value *argv, size_t &argc) -> bool {
        argc = ARGC_ZERO;
        return true;
    };

    auto retParser = [value](napi_env &env, napi_value result) -> bool {
        Value<std::vector<RootInfo>> rootInfoVec;
        bool ret = ParserGetRootsJsResult(env, result, rootInfoVec);
        if (!ret) {
            HILOG_ERROR("Parser js value fail.");
            return ret;
        }

        *value = std::move(rootInfoVec);
        return true;
    };

    auto errCode = CallJsMethod("getRoots", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d", errCode);
        return errCode;
    }

    if (value->code != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return value->code;
    }

    rootInfoVec = std::move(value->data);
    return ERR_OK;
}

int JsFileAccessExtAbility::Access(const Uri &uri, bool &isExist)
{
    UserAccessTracer trace;
    trace.Start("Access");
    auto value = std::make_shared<Value<bool>>();
    if (value == nullptr) {
        HILOG_ERROR("Access value is nullptr.");
        return E_GETRESULT;
    }

    auto argParser = [uri](napi_env &env, napi_value *argv, size_t &argc) -> bool {
        napi_value nativeUri = nullptr;
        napi_create_string_utf8(env, uri.ToString().c_str(), uri.ToString().length(), &nativeUri);
        argv[ARGC_ZERO] = nativeUri;
        argc = ARGC_ONE;
        return true;
    };

    auto retParser = [value](napi_env &env, napi_value result) -> bool {
        napi_value isExist = nullptr;
        napi_get_named_property(env, result, "isExist", &isExist);
        if (napi_get_value_bool(env, isExist, &value->data) != napi_ok) {
            HILOG_ERROR("Convert js value fail.");
            return false;
        }

        napi_value code = nullptr;
        napi_get_named_property(env, result, "code", &code);
        if (napi_get_value_int32(env, code, &value->code) != napi_ok) {
            HILOG_ERROR("Convert js value fail.");
            return false;
        }
        return true;
    };

    auto errCode = CallJsMethod("access", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        return errCode;
    }

    if (value->code != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return value->code;
    }

    isExist = value->data;
    return ERR_OK;
}

bool JsFileAccessExtAbility::ParserQueryFileJsResult(napi_env &env, napi_value nativeValue,
    Value<std::vector<std::string>> &results)
{
    napi_value code = nullptr;
    napi_get_named_property(env, nativeValue, "code", &code);
    if (napi_get_value_int32(env, code, &results.code) != napi_ok) {
        HILOG_ERROR("Convert code fail.");
        return false;
    }

    napi_value nativeArray = nullptr;
    napi_create_array(env, &nativeArray);
    napi_get_named_property(env, nativeValue, "results", &nativeArray);
    if (nativeArray == nullptr) {
        HILOG_ERROR("Convert js array object fail.");
        return false;
    }

    uint32_t length = 0;
    if (napi_get_array_length(env, nativeArray, &length) != napi_ok) {
        HILOG_ERROR("Get nativeArray length fail.");
        return false;
    }
    for (uint32_t i = 0; i < length; i++) {
        napi_value queryResult = nullptr;
        napi_get_element(env, nativeArray, i, &queryResult);
        if (queryResult == nullptr) {
            HILOG_ERROR("Get native queryResult fail.");
            return false;
        }

        std::string tempValue;
        if (GetStringValue(env, queryResult, tempValue) != napi_ok) {
            HILOG_ERROR("Convert js value fail.");
            return false;
        }
        results.data.emplace_back(std::move(tempValue));
    }
    return true;
}

static void ConvertColumn(std::vector<std::string> &columns)
{
    for (auto &column : columns) {
        for (auto &it : CONVERT_FILE_COLUMN) {
            if (column == it.first) {
                column = it.second;
            }
        }
    }
}

int JsFileAccessExtAbility::Query(const Uri &uri, std::vector<std::string> &columns, std::vector<std::string> &results)
{
    UserAccessTracer trace;
    trace.Start("Query");
    auto value = std::make_shared<Value<std::vector<std::string>>>();
    if (value == nullptr) {
        HILOG_ERROR("Query value is nullptr.");
        return E_GETRESULT;
    }

    ConvertColumn(columns);
    auto argParser = [uri, &columns](napi_env &env, napi_value *argv, size_t &argc) -> bool {
        if (ConstructQueryArg(env, argv, argc, uri, columns) != napi_ok) {
            HILOG_ERROR("Construct arg fail.");
            return false;
        }
        return true;
    };

    auto retParser = [value](napi_env &env, napi_value result) -> bool {
        Value<std::vector<std::string>> queryResult;
        bool ret = ParserQueryFileJsResult(env, result, queryResult);
        if (!ret) {
            HILOG_ERROR("Parser js value fail.");
            return ret;
        }
        value->code = queryResult.code;
        value->data = queryResult.data;
        return ret;
    };

    auto errCode = CallJsMethod("query", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        return errCode;
    }

    if (value->code != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return value->code;
    }

    results = std::move(value->data);
    return ERR_OK;
}

int JsFileAccessExtAbility::GetFileInfoFromUri(const Uri &selectFile, FileInfo &fileInfo)
{
    UserAccessTracer trace;
    trace.Start("GetFileInfoFromUri");
    auto value = std::make_shared<Value<FileInfo>>();
    if (value == nullptr) {
        HILOG_ERROR("GetFileInfoFromUri value is nullptr.");
        return E_GETRESULT;
    }

    auto argParser = [selectFile](napi_env &env, napi_value *argv, size_t &argc) -> bool {
        napi_value nativeUri = nullptr;
        napi_create_string_utf8(env, selectFile.ToString().c_str(), selectFile.ToString().length(), &nativeUri);
        if (nativeUri == nullptr) {
            HILOG_ERROR("create selectFile uri native js value fail.");
            return false;
        }
        argv[ARGC_ZERO] = nativeUri;
        argc = ARGC_ONE;
        return true;
    };

    auto retParser = [value](napi_env &env, napi_value result) -> bool {
        napi_value code = nullptr;
        napi_get_named_property(env, result, "code", &code);
        if (napi_get_value_int32(env, code, &value->code) != napi_ok) {
            HILOG_ERROR("Convert js value fail.");
            return false;
        }

        FileInfo fileInfo;
        napi_value nativeFileInfo = nullptr;
        if (napi_get_named_property(env, result, "fileInfo", &nativeFileInfo) != napi_ok) {
            HILOG_INFO("Convert fileInfo js value failed");
            return false;
        }

        if (GetFileInfoFromJs(env, nativeFileInfo, fileInfo) != napi_ok) {
            HILOG_ERROR("Convert fileInfo js value fail.");
            return false;
        }
        value->data = std::move(fileInfo);
        return true;
    };

    auto errCode = CallJsMethod("getFileInfoFromUri", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        return errCode;
    }

    if (value->code != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return value->code;
    }

    fileInfo = std::move(value->data);
    return ERR_OK;
}

int JsFileAccessExtAbility::GetFileInfoFromRelativePath(const std::string &selectFileRealtivePath, FileInfo &fileInfo)
{
    UserAccessTracer trace;
    trace.Start("GetFileInfoFromRelativePath");
    auto value = std::make_shared<Value<FileInfo>>();
    if (value == nullptr) {
        HILOG_ERROR("GetFileInfoFromRelativePath value is nullptr.");
        return E_GETRESULT;
    }

    auto argParser = [selectFileRealtivePath](napi_env &env, napi_value *argv, size_t &argc) -> bool {
        napi_value nativePath = nullptr;
        napi_create_string_utf8(env, selectFileRealtivePath.c_str(), selectFileRealtivePath.length(), &nativePath);
        if (nativePath == nullptr) {
            HILOG_ERROR("create selectFileRealtivePath native js value fail.");
            return false;
        }
        argv[ARGC_ZERO] = nativePath;
        argc = ARGC_ONE;
        return true;
    };

    auto retParser = [value](napi_env &env, napi_value result) -> bool {
        napi_value code = nullptr;
        napi_get_named_property(env, result, "code", &code);
        if (napi_get_value_int32(env, code, &value->code) != napi_ok) {
            HILOG_ERROR("Convert js value fail.");
            return false;
        }

        FileInfo fileInfo;
        napi_value nativeFileInfo = nullptr;
        if (napi_get_named_property(env, result, "fileInfo", &nativeFileInfo) != napi_ok) {
            HILOG_INFO("Convert fileInfo js value failed");
            return false;
        }

        if (GetFileInfoFromJs(env, nativeFileInfo, fileInfo) != napi_ok) {
            HILOG_ERROR("Convert fileInfo js value fail.");
            return false;
        }
        value->data = std::move(fileInfo);
        return true;
    };

    auto errCode = CallJsMethod("getFileInfoFromRelativePath", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        return errCode;
    }

    if (value->code != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return value->code;
    }

    fileInfo = std::move(value->data);
    return ERR_OK;
}

napi_value JsFileAccessExtAbility::FuncCallback(napi_env env, napi_callback_info info)
{
    UserAccessTracer trace;
    trace.Start("FuncCallback");
    if (env == nullptr) {
        HILOG_ERROR("NativeEngine pointer is null.");
        return nullptr;
    }

    if (info == nullptr) {
        HILOG_ERROR("invalid param.");
        return GetUndefinedValue(env);
    }

    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = { nullptr };
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR("napi_get_cb_info fail.");
        return GetUndefinedValue(env);
    }
    if (argc != ARGC_TWO) {
        HILOG_ERROR("invalid args.");
        return GetUndefinedValue(env);
    }

    std::string uriString;
    if (GetStringValue(env, argv[ARGC_ZERO], uriString) != napi_ok) {
        HILOG_ERROR("Get uri fail.");
        return GetUndefinedValue(env);
    }
    int32_t event = UnwrapInt32FromJS(env, argv[ARGC_ONE]);

    Uri uri(uriString);
    NotifyType notifyType = static_cast<NotifyType>(event);
    auto ret = Notify(uri, notifyType);
    if (ret != ERR_OK) {
        HILOG_ERROR("JsFileAccessExtAbility notify error, ret:%{public}d", ret);
    }
    return GetUndefinedValue(env);
}

int JsFileAccessExtAbility::StartWatcher(const Uri &uri)
{
    UserAccessTracer trace;
    trace.Start("StartWatcher");
    auto ret = std::make_shared<int>();
    if (ret == nullptr) {
        HILOG_ERROR("StartWatcher value is nullptr.");
        return E_GETRESULT;
    }

    auto argParser = [uri, this](napi_env &env, napi_value *argv, size_t &argc) -> bool {
        napi_value nativeUri = nullptr;
        napi_create_string_utf8(env, uri.ToString().c_str(), uri.ToString().length(), &nativeUri);
        if (nativeUri == nullptr) {
            HILOG_ERROR("create uri native js value fail.");
            return false;
        }
        const std::string funcName = "FuncCallback";
        napi_value func = nullptr;
        napi_create_function(env, funcName.c_str(), funcName.length(), JsFileAccessExtAbility::FuncCallback, nullptr,
            &func);
        if (func == nullptr) {
            HILOG_ERROR("Create function fail.");
            return false;
        }
        argv[ARGC_ZERO] = nativeUri;
        argv[ARGC_ONE] = func;
        argc = ARGC_TWO;
        return true;
    };

    auto retParser = [ret](napi_env &env, napi_value result) -> bool {
        if (napi_get_value_int32(env, result, ret.get()) != napi_ok) {
            HILOG_ERROR("Convert js value fail.");
            return false;
        }
        return true;
    };

    auto errCode = CallJsMethod("startWatcher", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        return errCode;
    }

    if (*ret != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return *ret;
    }

    return ERR_OK;
}

int JsFileAccessExtAbility::StopWatcher(const Uri &uri, bool isUnregisterAll)
{
    UserAccessTracer trace;
    trace.Start("StopWatcher");
    auto ret = std::make_shared<int>();
    if (ret == nullptr) {
        HILOG_ERROR("StopWatcher value is nullptr.");
        return E_GETRESULT;
    }

    auto argParser = [uri, isUnregisterAll](napi_env &env, napi_value *argv, size_t &argc) -> bool {
        napi_value nativeUri = nullptr;
        napi_create_string_utf8(env, uri.ToString().c_str(), uri.ToString().length(), &nativeUri);
        if (nativeUri == nullptr) {
            HILOG_ERROR("create uri native js value fail.");
            return false;
        }

        napi_value isCleanAll = nullptr;
        if (napi_get_boolean(env, isUnregisterAll, &isCleanAll) != napi_ok) {
            HILOG_ERROR("Get isCleanAll fail.");
            return false;
        }
        argv[ARGC_ZERO] = nativeUri;
        argv[ARGC_ONE] = isCleanAll;
        argc = ARGC_TWO;
        return true;
    };

    auto retParser = [ret](napi_env &env, napi_value result) -> bool {
        if (napi_get_value_int32(env, result, ret.get()) != napi_ok) {
            HILOG_ERROR("Convert js value fail.");
            return false;
        }
        return true;
    };

    auto errCode = CallJsMethod("stopWatcher", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        return errCode;
    }

    if (*ret != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return *ret;
    }

    return ERR_OK;
}

int JsFileAccessExtAbility::Notify(Uri &uri, NotifyType notifyType)
{
    UserAccessTracer trace;
    trace.Start("Notify");
    auto proxy = FileAccessServiceProxy::GetInstance();
    if (proxy == nullptr) {
        HILOG_ERROR("Notify get SA failed");
        return E_LOAD_SA;
    }
    auto ret = proxy->OnChange(uri, notifyType);
    if (ret != ERR_OK) {
        HILOG_ERROR("notify error, ret:%{public}d", ret);
        return ret;
    }
    return ERR_OK;
}

napi_status JsFileAccessExtAbility::GetFileInfoFromJs(napi_env &env, napi_value obj, FileInfo &fileInfo)
{
    napi_value uri = nullptr;
    napi_get_named_property(env, obj, "uri", &uri);
    if (GetStringValue(env, uri, fileInfo.uri) != napi_ok) {
        HILOG_ERROR("Convert uri fail");
        return napi_generic_failure;
    }

    napi_value relativePath = nullptr;
    napi_get_named_property(env, obj, "relativePath", &relativePath);
    if (GetStringValue(env, relativePath, fileInfo.relativePath) != napi_ok) {
        HILOG_ERROR("Convert relativePath fail");
        return napi_generic_failure;
    }

    napi_value fileName = nullptr;
    napi_get_named_property(env, obj, "fileName", &fileName);
    if (GetStringValue(env, fileName, fileInfo.fileName) != napi_ok) {
        HILOG_ERROR("Convert fileName fail");
        return napi_generic_failure;
    }

    napi_value mode = nullptr;
    napi_get_named_property(env, obj, "mode", &mode);
    if (napi_get_value_int32(env, mode, &fileInfo.mode) != napi_ok) {
        HILOG_ERROR("Convert mode fail");
        return napi_generic_failure;
    }

    napi_value size = nullptr;
    napi_get_named_property(env, obj, "size", &size);
    if (napi_get_value_int64(env, size, &fileInfo.size) != napi_ok) {
        HILOG_ERROR("Convert size fail");
        return napi_generic_failure;
    }

    napi_value mtime = nullptr;
    napi_get_named_property(env, obj, "mtime", &mtime);
    if (napi_get_value_int64(env, mtime, &fileInfo.mtime) != napi_ok) {
        HILOG_ERROR("Convert mtime fail");
        return napi_generic_failure;
    }

    napi_value mimeType = nullptr;
    napi_get_named_property(env, obj, "mimeType", &mimeType);
    if (GetStringValue(env, mimeType, fileInfo.mimeType) != napi_ok) {
        HILOG_ERROR("Convert mimeType fail");
        return napi_generic_failure;
    }
    return napi_ok;
}

napi_status JsFileAccessExtAbility::GetUriAndCodeFromJs(napi_env &env, napi_value result,
    const std::shared_ptr<Value<std::string>> &value)
{
    napi_value uri = nullptr;
    napi_get_named_property(env, result, "uri", &uri);
    if (GetStringValue(env, uri, value->data) != napi_ok) {
        HILOG_ERROR("Convert js value fail.");
        return napi_generic_failure;
    }

    napi_value code = nullptr;
    napi_get_named_property(env, result, "code", &code);
    if (napi_get_value_int32(env, code, &value->code) != napi_ok) {
        HILOG_ERROR("Convert js value fail.");
        return napi_generic_failure;
    }

    return napi_ok;
}

napi_status JsFileAccessExtAbility::GetFdAndCodeFromJs(napi_env &env, napi_value result,
    const std::shared_ptr<Value<int>> &value)
{
    napi_value fd = nullptr;
    napi_get_named_property(env, result, "fd", &fd);
    if (napi_get_value_int32(env, fd, &value->data) != napi_ok) {
        HILOG_ERROR("Convert js value fail.");
        return napi_generic_failure;
    }

    napi_value code = nullptr;
    napi_get_named_property(env, result, "code", &code);
    if (napi_get_value_int32(env, code, &value->code) != napi_ok) {
        HILOG_ERROR("Convert js value fail.");
        return napi_generic_failure;
    }

    return napi_ok;
}

napi_status JsFileAccessExtAbility::ConstructQueryArg(napi_env &env, napi_value *argv, size_t &argc, const Uri &uri,
    std::vector<std::string> &columns)
{
    napi_value nativeUri = nullptr;
    napi_create_string_utf8(env, uri.ToString().c_str(), uri.ToString().length(), &nativeUri);
    if (nativeUri == nullptr) {
        HILOG_ERROR("create uri native js value fail.");
        return napi_generic_failure;
    }

    napi_value resultArray = nullptr;
    napi_create_array_with_length(env, columns.size(), &resultArray);
    if (resultArray == nullptr) {
        HILOG_ERROR("Create MimeType native array value fail.");
        return napi_generic_failure;
    }
    int errorCode = MakeStringNativeArray(env, columns, resultArray);
    if (errorCode != ERR_OK) {
        HILOG_ERROR("Create native array value fail, code:%{public}d.", errorCode);
        return napi_generic_failure;
    }
    argv[ARGC_ZERO] = nativeUri;
    argv[ARGC_ONE] = resultArray;
    argc = ARGC_TWO;
    return napi_ok;
}

napi_status JsFileAccessExtAbility::GetRootInfo(napi_env env, napi_value nativeRootInfo, RootInfo &rootInfo)
{
    napi_value deviceType = nullptr;
    napi_get_named_property(env, nativeRootInfo, "deviceType", &deviceType);
    if (napi_get_value_int32(env, deviceType, &rootInfo.deviceType) != napi_ok) {
        HILOG_ERROR("Get deviceType value fail.");
        return napi_generic_failure;
    }

    napi_value uri = nullptr;
    napi_get_named_property(env, nativeRootInfo, "uri", &uri);
    if (GetStringValue(env, uri, rootInfo.uri) != napi_ok) {
        HILOG_ERROR("Get uri value fail.");
        return napi_generic_failure;
    }

    napi_value relativePath = nullptr;
    napi_get_named_property(env, nativeRootInfo, "relativePath", &relativePath);
    if (GetStringValue(env, relativePath, rootInfo.relativePath) != napi_ok) {
        HILOG_ERROR("Get relativePath value fail.");
        return napi_generic_failure;
    }

    napi_value displayName = nullptr;
    napi_get_named_property(env, nativeRootInfo, "displayName", &displayName);
    if (GetStringValue(env, displayName, rootInfo.displayName) != napi_ok) {
        HILOG_ERROR("Get displayName value fail.");
        return napi_generic_failure;
    }

    napi_value deviceFlags = nullptr;
    napi_get_named_property(env, nativeRootInfo, "deviceFlags", &deviceFlags);
    if (napi_get_value_int32(env, deviceFlags, &rootInfo.deviceFlags) != napi_ok) {
        HILOG_ERROR("Get deviceFlags value fail.");
        return napi_generic_failure;
    }

    return napi_ok;
}

int JsFileAccessExtAbility::MoveItem(const Uri &sourceFile, const Uri &targetParent,
                                     std::vector<Result> &moveResult, bool force)
{
    UserAccessTracer trace;
    trace.Start("MoveItem");
    auto argParser = [sourceFile, targetParent, force](napi_env &env, napi_value *argv, size_t &argc) -> bool {
        HILOG_ERROR("MoveItem argParser start");
        napi_value srcNativeUri = nullptr;
        napi_create_string_utf8(env, sourceFile.ToString().c_str(), sourceFile.ToString().length(), &srcNativeUri);

        napi_value dstNativeUri = nullptr;
        napi_create_string_utf8(env, targetParent.ToString().c_str(), targetParent.ToString().length(), &dstNativeUri);

        napi_value forceMove = nullptr;
        napi_get_boolean(env, force, &forceMove);
        if (srcNativeUri == nullptr || dstNativeUri == nullptr || forceMove == nullptr) {
            HILOG_ERROR("create arguments native js value fail.");
            return false;
        }
        argv[ARGC_ZERO] = srcNativeUri;
        argv[ARGC_ONE] = dstNativeUri;
        argv[ARGC_TWO] = forceMove;
        argc = ARGC_THREE;
        return true;
    };

    int ret = EXCEPTION;
    auto retParser = [&moveResult, &ret](napi_env &env, napi_value result) -> bool {
        return ParserGetJsResult(env, result, moveResult, ret);
    };

    auto errCode = CallJsMethod("moveItem", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        Result result { "", "", errCode, ""};
        TranslateResult(result);
        moveResult.push_back(result);
        return EXCEPTION;
    }

    return ret;
}

int JsFileAccessExtAbility::MoveFile(const Uri &sourceFile, const Uri &targetParent, std::string &fileName,
                                     Uri &newFile)
{
    UserAccessTracer trace;
    trace.Start("MoveFile");
    auto value = std::make_shared<Value<std::string>>();
    if (value == nullptr) {
        HILOG_ERROR("MoveFile value is nullptr.");
        return E_GETRESULT;
    }

    auto argParser = [sourceFile, targetParent, fileName](napi_env &env, napi_value *argv, size_t &argc) -> bool {
        napi_value srcUri = nullptr;
        napi_create_string_utf8(env, sourceFile.ToString().c_str(), sourceFile.ToString().length(), &srcUri);
        napi_value dstUri = nullptr;
        napi_create_string_utf8(env, targetParent.ToString().c_str(), targetParent.ToString().length(), &dstUri);
        napi_value name = nullptr;
        napi_create_string_utf8(env, fileName.c_str(), fileName.length(), &name);
        if (srcUri == nullptr || dstUri == nullptr || name ==nullptr) {
            HILOG_ERROR("create sourceFile uri native js value fail.");
            return false;
        }
        argv[ARGC_ZERO] = srcUri;
        argv[ARGC_ONE] = dstUri;
        argv[ARGC_TWO] = name;
        argc = ARGC_THREE;
        return true;
    };

    auto retParser = [value](napi_env &env, napi_value result) -> bool {
        if (GetUriAndCodeFromJs(env, result, value) != napi_ok) {
            HILOG_ERROR("Convert js object fail.");
            return false;
        }
        return true;
    };

    auto errCode = CallJsMethod("moveFile", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        return errCode;
    }

    if (value->code != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return value->code;
    }

    if ((value->data).empty()) {
        HILOG_ERROR("call move with return empty.");
        return E_GETRESULT;
    }
    newFile = Uri(value->data);
    return ERR_OK;
}
} // namespace FileAccessFwk
} // namespace OHOS