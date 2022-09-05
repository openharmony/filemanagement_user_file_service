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
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
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
    constexpr size_t ARGC_ZERO = 0;
    constexpr size_t ARGC_ONE = 1;
    constexpr size_t ARGC_TWO = 2;
    constexpr size_t ARGC_THREE = 3;
    constexpr size_t MAX_ARG_COUNT = 5;
}

using namespace OHOS::AppExecFwk;
using namespace OHOS::AbilityRuntime;
using OHOS::Security::AccessToken::AccessTokenKit;

JsFileAccessExtAbility* JsFileAccessExtAbility::Create(const std::unique_ptr<Runtime> &runtime)
{
    return new JsFileAccessExtAbility(static_cast<JsRuntime&>(*runtime));
}

JsFileAccessExtAbility::JsFileAccessExtAbility(JsRuntime &jsRuntime) : jsRuntime_(jsRuntime) {}
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
        HILOG_ERROR("Failed to get srcPath");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return;
    }

    std::string moduleName(Extension::abilityInfo_->moduleName);
    moduleName.append("::").append(abilityInfo_->name);
    HandleScope handleScope(jsRuntime_);

    jsObj_ = jsRuntime_.LoadModule(moduleName, srcPath, abilityInfo_->hapPath);
    if (jsObj_ == nullptr) {
        HILOG_ERROR("Failed to get jsObj_");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return;
    }

    NativeObject* obj = ConvertNativeValueTo<NativeObject>(jsObj_->Get());
    if (obj == nullptr) {
        HILOG_ERROR("Failed to get JsFileAccessExtAbility object");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
}

NativeValue* JsFileAccessExtAbility::FuncCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "FuncCallback");
    if (engine == nullptr || info == nullptr) {
        HILOG_ERROR("invalid param.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return engine->CreateUndefined();
    }

    if (info->argc != ARGC_THREE) {
        HILOG_ERROR("invalid args.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return engine->CreateUndefined();
    }

    int32_t deviceType = UnwrapInt32FromJS(reinterpret_cast<napi_env>(engine),
        reinterpret_cast<napi_value>(info->argv[ARGC_ZERO]));
    int32_t notifyType = UnwrapInt32FromJS(reinterpret_cast<napi_env>(engine),
        reinterpret_cast<napi_value>(info->argv[ARGC_ONE]));
    std::string uri = UnwrapStringFromJS(reinterpret_cast<napi_env>(engine),
        reinterpret_cast<napi_value>(info->argv[ARGC_TWO]));

    if (info->functionInfo == nullptr || info->functionInfo->data == nullptr) {
        HILOG_ERROR("invalid object.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return engine->CreateUndefined();
    }

    JsFileAccessExtAbility* jsExtension = static_cast<JsFileAccessExtAbility*>(info->functionInfo->data);
    if (jsExtension == nullptr) {
        HILOG_ERROR("invalid JsFileAccessExtAbility.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return engine->CreateUndefined();
    }

    NotifyMessage message(deviceType, notifyType, "", uri);
    auto ret = jsExtension->Notify(message);
    if (ret != ERR_OK) {
        HILOG_ERROR("JsFileAccessExtAbility notify error, ret:%{public}d", ret);
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return engine->CreateUndefined();
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

    const std::string funcName = "FuncCallback";
    auto& nativeEngine = jsRuntime_.GetNativeEngine();
    NativeValue* func = nativeEngine.CreateFunction(funcName.c_str(), funcName.length(),
        JsFileAccessExtAbility::FuncCallback, this);
    NativeValue* argvCallback[] = {func};
    CallObjectMethod("registerCallback", argvCallback, ARGC_ONE);
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
        HILOG_ERROR("No memory allocated for FileExtStubImpl");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
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
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return nullptr;
    }

    HandleScope handleScope(jsRuntime_);
    auto& nativeEngine = jsRuntime_.GetNativeEngine();

    NativeValue* value = jsObj_->Get();
    if (value == nullptr) {
        HILOG_ERROR("Failed to get FileAccessExtAbility value");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return nullptr;
    }

    NativeObject* obj = ConvertNativeValueTo<NativeObject>(value);
    if (obj == nullptr) {
        HILOG_ERROR("Failed to get FileAccessExtAbility object");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return nullptr;
    }

    NativeValue* method = obj->GetProperty(name);
    if (method == nullptr) {
        HILOG_ERROR("Failed to get '%{public}s' from FileAccessExtAbility object", name);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return nullptr;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return handleScope.Escape(nativeEngine.CallFunction(value, method, argv, argc));
}

static int DoCallJsMethod(CallJsParam *param)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "DoCallJsMethod");
    JsRuntime *jsRuntime = param->jsRuntime;
    if (jsRuntime == nullptr) {
        HILOG_ERROR("failed to get jsRuntime.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_INVALID_PARAM;
    }
    HandleScope handleScope(*jsRuntime);
    auto& nativeEngine = jsRuntime->GetNativeEngine();
    size_t argc = 0;
    NativeValue *argv[MAX_ARG_COUNT] = { nullptr };
    if (param->argParser != nullptr) {
        if (!param->argParser(nativeEngine, argv, argc)) {
            HILOG_ERROR("failed to get params.");
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
            return ERR_INVALID_PARAM;
        }
    }
    NativeValue *value = param->jsObj->Get();
    if (value == nullptr) {
        HILOG_ERROR("failed to get native value object.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_INVALID_PARAM;
    }
    NativeObject *obj = ConvertNativeValueTo<NativeObject>(value);
    if (obj == nullptr) {
        HILOG_ERROR("failed to get FileExtAbility object.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_INVALID_PARAM;
    }
    NativeValue *method = obj->GetProperty(param->funcName.c_str());
    if (method == nullptr) {
        HILOG_ERROR("failed to get %{public}s from FileExtAbility object.", param->funcName.c_str());
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_INVALID_PARAM;
    }
    if (param->retParser == nullptr) {
        HILOG_ERROR("ResultValueParser must not null.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_INVALID_PARAM;
    }
    if (!param->retParser(nativeEngine, handleScope.Escape(nativeEngine.CallFunction(value, method, argv, argc)))) {
        HILOG_INFO("Parser js result fail.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARSER_FAIL;
    }
    return ERR_OK;
}

int JsFileAccessExtAbility::CallJsMethod(const std::string &funcName, JsRuntime &jsRuntime, NativeReference *jsObj,
    InputArgsParser argParser, ResultValueParser retParser)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CallJsMethod");
    uv_loop_s *loop = nullptr;
    napi_status status = napi_get_uv_event_loop(reinterpret_cast<napi_env>(&jsRuntime.GetNativeEngine()), &loop);
    if (status != napi_ok) {
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        HILOG_ERROR("failed to get uv event loop.");
        return ERR_INVALID_PARAM;
    }
    auto param = std::make_shared<CallJsParam>(funcName, &jsRuntime, jsObj, argParser, retParser);
    if (param == nullptr) {
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        HILOG_ERROR("failed to new param.");
        return ERR_INVALID_PARAM;
    }
    auto work = std::make_shared<uv_work_t>();
    if (work == nullptr) {
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        HILOG_ERROR("failed to new uv_work_t.");
        return ERR_INVALID_PARAM;
    }
    work->data = reinterpret_cast<void *>(param.get());
    int ret = uv_queue_work(loop, work.get(), [](uv_work_t *work) {}, [](uv_work_t *work, int status) {
        CallJsParam *param = reinterpret_cast<CallJsParam *>(work->data);
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
    });
    if (ret != 0) {
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        HILOG_ERROR("failed to exec uv_queue_work.");
        return ERR_INVALID_PARAM;
    }
    std::unique_lock<std::mutex> lock(param->fileOperateMutex);
    param->fileOperateCondition.wait(lock, [param]() { return param->isReady; });
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
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
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
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

template <typename T>
struct Value {
    T data;
    int code {ERR_OK};
};

int JsFileAccessExtAbility::OpenFile(const Uri &uri, int flags, int &fd)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "OpenFile");
    auto value = std::make_shared<Value<int>>();
    if (value == nullptr) {
        HILOG_ERROR("OpenFile value is nullptr.");
        return ERR_NULL_POINTER;
    }

    auto argParser = [uri, flags](NativeEngine &engine, NativeValue *argv[], size_t &argc) -> bool {
        NativeValue *nativeUri = engine.CreateString(uri.ToString().c_str(), uri.ToString().length());
        NativeValue *nativeFlags = engine.CreateNumber((int32_t)flags);
        if (nativeUri == nullptr || nativeFlags == nullptr) {
            HILOG_ERROR("create uri or flags native js value fail.");
            return false;
        }
        argv[ARGC_ZERO] = nativeUri;
        argv[ARGC_ONE] = nativeFlags;
        argc = ARGC_TWO;
        return true;
    };
    auto retParser = [value](NativeEngine &engine, NativeValue *result) -> bool {
        NativeObject *obj = ConvertNativeValueTo<NativeObject>(result);
        if (obj == nullptr) {
            HILOG_ERROR("Convert js object fail.");
            return false;
        }

        bool ret = ConvertFromJsValue(engine, obj->GetProperty("fd"), value->data);
        ret = ret && ConvertFromJsValue(engine, obj->GetProperty("code"), value->code);
        if (!ret) {
            HILOG_ERROR("Convert js value fail.");
        }
        return ret;
    };

    auto errCode = CallJsMethod("openFile", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return errCode;
    }

    if (value->code != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return ERR_FILEIO_FAIL;
    }

    fd = value->data;
    if (fd < ERR_OK) {
        HILOG_ERROR("Failed to get file descriptor fd: %{public}d", fd);
        return ERR_FILEIO_FAIL;
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int JsFileAccessExtAbility::CreateFile(const Uri &parent, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "CreateFile");
    auto value = std::make_shared<Value<std::string>>();
    if (value == nullptr) {
        HILOG_ERROR("CreateFile value is nullptr.");
        return ERR_NULL_POINTER;
    }

    auto argParser = [parent, displayName](NativeEngine &engine, NativeValue *argv[], size_t &argc) -> bool {
        NativeValue *nativeParent = engine.CreateString(parent.ToString().c_str(), parent.ToString().length());
        NativeValue *nativeDisplayName = engine.CreateString(displayName.c_str(), displayName.length());
        if (nativeParent == nullptr || nativeDisplayName == nullptr) {
            HILOG_ERROR("create parent uri or displayName native js value fail.");
            return false;
        }
        argv[ARGC_ZERO] = nativeParent;
        argv[ARGC_ONE] = nativeDisplayName;
        argc = ARGC_TWO;
        return true;
    };
    auto retParser = [value](NativeEngine &engine, NativeValue *result) -> bool {
        NativeObject *obj = ConvertNativeValueTo<NativeObject>(result);
        if (obj == nullptr) {
            HILOG_ERROR("Convert js object fail.");
            return false;
        }

        bool ret = ConvertFromJsValue(engine, obj->GetProperty("uri"), value->data);
        ret = ret && ConvertFromJsValue(engine, obj->GetProperty("code"), value->code);
        if (!ret) {
            HILOG_ERROR("Convert js value fail.");
        }
        return ret;
    };

    auto errCode = CallJsMethod("createFile", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return errCode;
    }

    if (value->code != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return ERR_FILEIO_FAIL;
    }

    if ((value->data).empty()) {
        HILOG_ERROR("call CreateFile with return empty.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARSER_FAIL;
    }

    newFile = Uri(value->data);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int JsFileAccessExtAbility::Mkdir(const Uri &parent, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Mkdir");
    auto value = std::make_shared<Value<std::string>>();
    if (value == nullptr) {
        HILOG_ERROR("Mkdir value is nullptr.");
        return ERR_NULL_POINTER;
    }

    auto argParser = [parent, displayName](NativeEngine &engine, NativeValue *argv[], size_t &argc) -> bool {
        NativeValue *nativeParent = engine.CreateString(parent.ToString().c_str(), parent.ToString().length());
        NativeValue *nativeDisplayName = engine.CreateString(displayName.c_str(), displayName.length());
        if (nativeParent == nullptr || nativeDisplayName == nullptr) {
            HILOG_ERROR("create parent uri native js value fail.");
            return false;
        }
        argv[ARGC_ZERO] = nativeParent;
        argv[ARGC_ONE] = nativeDisplayName;
        argc = ARGC_TWO;
        return true;
    };
    auto retParser = [value](NativeEngine &engine, NativeValue *result) -> bool {
        NativeObject *obj = ConvertNativeValueTo<NativeObject>(result);
        if (obj == nullptr) {
            HILOG_ERROR("Convert js object fail.");
            return false;
        }

        bool ret = ConvertFromJsValue(engine, obj->GetProperty("uri"), value->data);
        ret = ret && ConvertFromJsValue(engine, obj->GetProperty("code"), value->code);
        if (!ret) {
            HILOG_ERROR("Convert js value fail.");
        }

        return ret;
    };

    auto errCode = CallJsMethod("mkdir", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return errCode;
    }

    if (value->code != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return ERR_FILEIO_FAIL;
    }

    if ((value->data).empty()) {
        HILOG_ERROR("call Mkdir with return empty.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARSER_FAIL;
    }
    newFile = Uri(value->data);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int JsFileAccessExtAbility::Delete(const Uri &sourceFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Delete");
    auto ret = std::make_shared<int>();
    if (ret == nullptr) {
        HILOG_ERROR("Delete value is nullptr.");
        return ERR_NULL_POINTER;
    }

    auto argParser = [uri = sourceFile](NativeEngine &engine, NativeValue *argv[], size_t &argc) -> bool {
        NativeValue *nativeUri = engine.CreateString(uri.ToString().c_str(), uri.ToString().length());
        if (nativeUri == nullptr) {
            HILOG_ERROR("create sourceFile uri native js value fail.");
            return false;
        }
        argv[ARGC_ZERO] = nativeUri;
        argc = ARGC_ONE;
        return true;
    };
    auto retParser = [ret](NativeEngine &engine, NativeValue *result) -> bool {
        bool res = ConvertFromJsValue(engine, result, *ret);
        if (!res) {
            HILOG_ERROR("Convert js value fail.");
        }
        return res;
    };

    auto errCode = CallJsMethod("delete", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return errCode;
    }

    if (*ret != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return ERR_FILEIO_FAIL;
    }

    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int JsFileAccessExtAbility::Move(const Uri &sourceFile, const Uri &targetParent, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Move");
    auto value = std::make_shared<Value<std::string>>();
    if (value == nullptr) {
        HILOG_ERROR("Move value is nullptr.");
        return ERR_NULL_POINTER;
    }

    auto argParser = [sourceFile, targetParent](NativeEngine &engine, NativeValue* argv[], size_t &argc) -> bool {
        NativeValue *srcUri = engine.CreateString(sourceFile.ToString().c_str(),
            sourceFile.ToString().length());
        NativeValue *dstUri = engine.CreateString(targetParent.ToString().c_str(), targetParent.ToString().length());
        if (srcUri == nullptr || dstUri == nullptr) {
            HILOG_ERROR("create sourceFile uri native js value fail.");
            return false;
        }
        argv[ARGC_ZERO] = srcUri;
        argv[ARGC_ONE] = dstUri;
        argc = ARGC_TWO;
        return true;
    };
    auto retParser = [value](NativeEngine &engine, NativeValue *result) -> bool {
        NativeObject *obj = ConvertNativeValueTo<NativeObject>(result);
        if (obj == nullptr) {
            HILOG_ERROR("Convert js object fail.");
            return false;
        }

        bool ret = ConvertFromJsValue(engine, obj->GetProperty("uri"), value->data);
        ret = ret && ConvertFromJsValue(engine, obj->GetProperty("code"), value->code);
        if (!ret) {
            HILOG_ERROR("Convert js value fail.");
        }
        return ret;
    };

    auto errCode = CallJsMethod("move", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return errCode;
    }

    if (value->code != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return ERR_FILEIO_FAIL;
    }

    if ((value->data).empty()) {
        HILOG_ERROR("call move with return empty.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARSER_FAIL;
    }
    newFile = Uri(value->data);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int JsFileAccessExtAbility::Rename(const Uri &sourceFile, const std::string &displayName, Uri &newFile)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Rename");
    auto value = std::make_shared<Value<std::string>>();
    if (value == nullptr) {
        HILOG_ERROR("Rename value is nullptr.");
        return ERR_NULL_POINTER;
    }
    auto argParser = [sourceFile, displayName](NativeEngine &engine, NativeValue *argv[], size_t &argc) -> bool {
        NativeValue *nativeSourceFile = engine.CreateString(sourceFile.ToString().c_str(),
            sourceFile.ToString().length());
        NativeValue *nativeDisplayName = engine.CreateString(displayName.c_str(), displayName.length());
        if (nativeSourceFile == nullptr || nativeDisplayName == nullptr) {
            HILOG_ERROR("create sourceFile uri or displayName native js value fail.");
            return false;
        }
        argv[ARGC_ZERO] = nativeSourceFile;
        argv[ARGC_ONE] = nativeDisplayName;
        argc = ARGC_TWO;
        return true;
    };
    auto retParser = [value](NativeEngine &engine, NativeValue *result) -> bool {
        NativeObject *obj = ConvertNativeValueTo<NativeObject>(result);
        if (obj == nullptr) {
            HILOG_ERROR("Convert js object fail.");
            return false;
        }

        bool ret = ConvertFromJsValue(engine, obj->GetProperty("uri"), value->data);
        ret = ret && ConvertFromJsValue(engine, obj->GetProperty("code"), value->code);
        if (!ret) {
            HILOG_ERROR("Convert js value fail.");
        }
        return ret;
    };

    auto errCode = CallJsMethod("rename", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return errCode;
    }

    if (value->code != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return ERR_FILEIO_FAIL;
    }

    if ((value->data).empty()) {
        HILOG_ERROR("call Rename with return empty.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return ERR_PARSER_FAIL;
    }
    newFile = Uri(value->data);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}


static bool ParserListFileJsResult(NativeEngine &engine, NativeValue *nativeValue, Value<std::vector<FileInfo>> &result)
{
    NativeObject *obj = ConvertNativeValueTo<NativeObject>(nativeValue);
    if (obj == nullptr) {
        HILOG_ERROR("Convert js object fail.");
        return false;
    }

    bool ret = ConvertFromJsValue(engine, obj->GetProperty("code"), result.code);
    NativeArray *nativeArray = ConvertNativeValueTo<NativeArray>(obj->GetProperty("infos"));
    if (nativeArray == nullptr) {
        HILOG_ERROR("Convert js array object fail.");
        return false;
    }

    for (uint32_t i = 0; i < nativeArray->GetLength(); i++) {
        NativeValue *nativeFileInfo = nativeArray->GetElement(i);
        if (nativeFileInfo == nullptr) {
            HILOG_ERROR("get native FileInfo fail.");
            return false;
        }

        obj = ConvertNativeValueTo<NativeObject>(nativeFileInfo);
        if (obj == nullptr) {
            HILOG_ERROR("Convert js object fail.");
            return false;
        }

        FileInfo fileInfo;
        ret = ret && ConvertFromJsValue(engine, obj->GetProperty("uri"), fileInfo.uri);
        ret = ret && ConvertFromJsValue(engine, obj->GetProperty("fileName"), fileInfo.fileName);
        ret = ret && ConvertFromJsValue(engine, obj->GetProperty("mode"), fileInfo.mode);
        ret = ret && ConvertFromJsValue(engine, obj->GetProperty("size"), fileInfo.size);
        ret = ret && ConvertFromJsValue(engine, obj->GetProperty("mtime"), fileInfo.mtime);
        ret = ret && ConvertFromJsValue(engine, obj->GetProperty("mimeType"), fileInfo.mimeType);
        if (!ret) {
            HILOG_ERROR("Convert js value fail.");
            return ret;
        }

        result.data.emplace_back(std::move(fileInfo));
    }
    return true;
}

int JsFileAccessExtAbility::ListFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
    const FileFilter &filter, std::vector<FileInfo> &fileInfoVec)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "ListFile");
    auto value = std::make_shared<Value<std::vector<FileInfo>>>();
    if (value == nullptr) {
        HILOG_ERROR("ListFile value is nullptr.");
        return ERR_NULL_POINTER;
    }

    auto argParser =
        [fileInfo, offset, maxCount, filter](NativeEngine &engine, NativeValue *argv[], size_t &argc) -> bool {
        NativeValue *uri = engine.CreateString(fileInfo.uri.c_str(), fileInfo.uri.length());
        if (uri == nullptr) {
            HILOG_ERROR("create sourceFile uri native js value fail.");
            return false;
        }

        NativeValue *nativeOffset = engine.CreateNumber(offset);
        if (nativeOffset == nullptr) {
            HILOG_ERROR("create nativeMaxNum native js value fail.");
            return false;
        }

        NativeValue *nativeMaxCount = engine.CreateNumber(maxCount);
        if (nativeMaxCount == nullptr) {
            HILOG_ERROR("create nativeMaxNum native js value fail.");
            return false;
        }

        argv[ARGC_ZERO] = uri;
        argv[ARGC_ONE] = nativeOffset;
        argv[ARGC_TWO] = nativeMaxCount;
        argc = ARGC_THREE;
        return true;
    };
    auto retParser = [value](NativeEngine &engine, NativeValue *result) -> bool {
        Value<std::vector<FileInfo>> fileInfo;
        bool ret = ParserListFileJsResult(engine, result, fileInfo);
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
        return ERR_FILEIO_FAIL;
    }

    fileInfoVec = std::move(value->data);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

static bool ParserGetRootsJsResult(NativeEngine &engine, NativeValue *nativeValue, Value<std::vector<RootInfo>> &result)
{
    NativeObject *obj = ConvertNativeValueTo<NativeObject>(nativeValue);
    if (obj == nullptr) {
        HILOG_ERROR("Convert js object fail.");
        return false;
    }

    bool ret = ConvertFromJsValue(engine, obj->GetProperty("code"), result.code);
    NativeArray *nativeArray = ConvertNativeValueTo<NativeArray>(obj->GetProperty("roots"));
    if (nativeArray == nullptr) {
        HILOG_ERROR("nativeArray is nullptr");
        return false;
    }

    for (uint32_t i = 0; i < nativeArray->GetLength(); i++) {
        NativeValue *nativeRootInfo = nativeArray->GetElement(i);
        if (nativeRootInfo == nullptr) {
            HILOG_ERROR("get native FileInfo fail.");
            return false;
        }

        obj = ConvertNativeValueTo<NativeObject>(nativeRootInfo);
        if (obj == nullptr) {
            HILOG_ERROR("Convert js object fail.");
            return false;
        }

        RootInfo rootInfo;
        ret = ret && ConvertFromJsValue(engine, obj->GetProperty("deviceType"), rootInfo.deviceType);
        ret = ret && ConvertFromJsValue(engine, obj->GetProperty("uri"), rootInfo.uri);
        ret = ret && ConvertFromJsValue(engine, obj->GetProperty("displayName"), rootInfo.displayName);
        ret = ret && ConvertFromJsValue(engine, obj->GetProperty("deviceFlags"), rootInfo.deviceFlags);
        if (!ret) {
            HILOG_ERROR("Convert js value fail.");
            return ret;
        }

        result.data.emplace_back(std::move(rootInfo));
    }

    return true;
}

int JsFileAccessExtAbility::GetRoots(std::vector<RootInfo> &rootInfoVec)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "GetRoots");
    auto value = std::make_shared<Value<std::vector<RootInfo>>>();
    if (value == nullptr) {
        HILOG_ERROR("GetRoots value is nullptr.");
        return ERR_NULL_POINTER;
    }

    auto argParser = [](NativeEngine &engine, NativeValue *argv[], size_t &argc) -> bool {
        argc = ARGC_ZERO;
        return true;
    };
    auto retParser = [value](NativeEngine &engine, NativeValue *result) -> bool {
        Value<std::vector<RootInfo>> rootInfoVec;
        bool ret = ParserGetRootsJsResult(engine, result, rootInfoVec);
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
        return ERR_FILEIO_FAIL;
    }

    rootInfoVec = std::move(value->data);
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}

int JsFileAccessExtAbility::Access(const Uri &uri, bool &isExist)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Access");
    auto value = std::make_shared<Value<bool>>();
    if (value == nullptr) {
        HILOG_ERROR("Access value is nullptr.");
        return ERR_NULL_POINTER;
    }

    auto argParser = [uri](NativeEngine &engine, NativeValue *argv[], size_t &argc) -> bool {
        NativeValue *nativeUri = engine.CreateString(uri.ToString().c_str(), uri.ToString().length());
        argv[ARGC_ZERO] = nativeUri;
        argc = ARGC_ONE;
        return true;
    };
    auto retParser = [value](NativeEngine &engine, NativeValue *result) -> bool {
        NativeObject *obj = ConvertNativeValueTo<NativeObject>(result);
        if (obj == nullptr) {
            HILOG_ERROR("Convert js object fail.");
            return ERR_PARSER_FAIL;
        }

        bool ret = ConvertFromJsValue(engine, obj->GetProperty("isExist"), value->data);
        ret = ret && ConvertFromJsValue(engine, obj->GetProperty("code"), value->code);
        if (!ret) {
            HILOG_ERROR("Convert js value fail.");
        }
        return ret;
    };

    auto errCode = CallJsMethod("access", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOG_ERROR("CallJsMethod error, code:%{public}d.", errCode);
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return errCode;
    }

    if (value->code != ERR_OK) {
        HILOG_ERROR("fileio fail.");
        return ERR_FILEIO_FAIL;
    }

    isExist = value->data;
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ERR_OK;
}
} // namespace FileAccessFwk
} // namespace OHOS
