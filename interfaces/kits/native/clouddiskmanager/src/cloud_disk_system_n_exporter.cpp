/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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


#include "cloud_disk_system_n_exporter.h"

#include <climits>
#include <cmath>
#include <memory>
#include <tuple>

#include "accesstoken_kit.h"
#include "cloud_disk_error.h"
#include "cloud_disk_service_manager.h"
#include "hilog_wrapper.h"
#include "hydrate_progress_callback.h"
#include "ipc_skeleton.h"
#include "tokenid_kit.h"

namespace OHOS::FileManagement::CloudDiskService {
using namespace LibN;
namespace {
const std::string CLASS_NAME = "CloudDiskSystemAccessor";
const std::string EVENT_NAME = "hydrateProgress";
constexpr int32_t MAX_CALLBACK_TYPE_LIMIT = 2;
constexpr int32_t MAX_PRIORITY_LIMIT = 2;
const napi_type_tag SYSTEM_ACCESSOR_TAG = {0x6d5c01ca4b984269, 0xa61382ed04f9e5b7};

struct SystemAccessor {
    sptr<HydrateProgressCallback> progress;

    ~SystemAccessor()
    {
        if (progress != nullptr) {
            progress->Close();
            int32_t ret = CloudDiskServiceManager::GetInstance().UnregisterProgressCallback(progress);
            if (ret != E_OK && ret != E_NOT_SUPPORT) {
                HILOG_ERROR("CloudDiskSystemAccessor::~SystemAccessor unregister callback failed, ret:%{public}d", ret);
            }
        }
    }
};

NError MakeError(int32_t code)
{
    return NError([code]() -> std::tuple<uint32_t, std::string> {
        auto error = GetCloudDiskErrorInfo(code);
        return {static_cast<uint32_t>(error.code), error.message};
    });
}

napi_value Throw(napi_env env, int32_t code)
{
    MakeError(code).ThrowErr(env);
    return nullptr;
}

SystemAccessor *GetAccessor(napi_env env, napi_value receiver)
{
    bool matches = false;
    napi_status status = napi_check_object_type_tag(env, receiver, &SYSTEM_ACCESSOR_TAG, &matches);
    if (status != napi_ok || !matches) {
        HILOG_ERROR("CloudDiskSystemAccessor::GetAccessor type tag check failed, status:%{public}d, matches:%{public}d",
            static_cast<int32_t>(status), static_cast<int32_t>(matches));
        return nullptr;
    }
    auto accessor = NClass::GetEntityOf<SystemAccessor>(env, receiver);
    if (accessor == nullptr) {
        HILOG_ERROR("CloudDiskSystemAccessor::GetAccessor native instance is null");
    }
    return accessor;
}

int32_t CheckAccess()
{
    using namespace Security::AccessToken;
    auto token = IPCSkeleton::GetCallingTokenID();
    auto type = AccessTokenKit::GetTokenTypeFlag(token);
    if (type != TOKEN_HAP && type != TOKEN_NATIVE) {
        HILOG_ERROR("CloudDiskSystemAccessor::CheckAccess unsupported caller type:%{public}d",
            static_cast<int32_t>(type));
        return FileManagement::E_PERMISSION;
    }
    if (AccessTokenKit::VerifyAccessToken(token, "ohos.permission.ACCESS_CLOUD_DISK_INFO") != PERMISSION_GRANTED) {
        HILOG_ERROR("CloudDiskSystemAccessor::CheckAccess ACCESS_CLOUD_DISK_INFO permission denied");
        return FileManagement::E_PERMISSION;
    }
    if (type != TOKEN_NATIVE && !TokenIdKit::IsSystemAppByFullTokenID(IPCSkeleton::GetCallingFullTokenID())) {
        HILOG_ERROR("CloudDiskSystemAccessor::CheckAccess caller is not a system application");
        return FileManagement::E_PERMISSION_SYSTEM;
    }
    return E_OK;
}

bool ReadString(napi_env env, napi_value value, std::string &result)
{
    auto [ok, text, length] = NVal(env, value).ToUTF8String();
    if (!ok || text == nullptr || length == 0 || length > PATH_MAX) {
        return false;
    }
    result.assign(text.get(), length);
    return result.find('\0') == std::string::npos;
}

bool ReadEnum(napi_env env, napi_value value, int32_t maximum, int32_t &result)
{
    double number = 0;
    if (napi_get_value_double(env, value, &number) != napi_ok || !std::isfinite(number) ||
        number < 0 || number > maximum || std::floor(number) != number) {
        return false;
    }
    result = static_cast<int32_t>(number);
    return true;
}

napi_value Constructor(napi_env env, napi_callback_info info)
{
    HILOG_INFO("CloudDiskSystemAccessor::Constructor start");
    NFuncArg args(env, info);
    if (!args.InitArgs(NARG_CNT::ZERO)) {
        HILOG_ERROR("CloudDiskSystemAccessor::Constructor invalid arguments, argc:%{public}zu", args.GetArgc());
        return Throw(env, FileManagement::E_PARAMETER_ERROR);
    }
    auto accessor = std::make_unique<SystemAccessor>();
    auto finalize = [](napi_env env, void *data, void *hint) {
        delete static_cast<SystemAccessor *>(data);
    };
    napi_status status = napi_type_tag_object(env, args.GetThisVar(), &SYSTEM_ACCESSOR_TAG);
    if (status != napi_ok) {
        HILOG_ERROR("CloudDiskSystemAccessor::Constructor set type tag failed, status:%{public}d",
            static_cast<int32_t>(status));
        return Throw(env, FileManagement::E_TRY_AGAIN);
    }
    status = napi_wrap(env, args.GetThisVar(), accessor.get(), finalize, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR("CloudDiskSystemAccessor::Constructor wrap native instance failed, status:%{public}d",
            static_cast<int32_t>(status));
        return Throw(env, FileManagement::E_TRY_AGAIN);
    }
    accessor.release();
    return args.GetThisVar();
}

napi_value HandleAsyncResult(napi_env env, const NVal &result, const char *operation)
{
    if (result.val_ != nullptr) {
        return result.val_;
    }
    HILOG_ERROR("CloudDiskSystemAccessor::%{public}s schedule async work failed", operation);
    bool pending = false;
    if (napi_is_exception_pending(env, &pending) == napi_ok && pending) {
        HILOG_INFO("napi_is_exception_pending");
        return nullptr;
    }
    return Throw(env, FileManagement::E_TRY_AGAIN);
}

napi_value RunOperation(napi_env env, napi_callback_info info, bool hydrate)
{
    const char *operation = hydrate ? "hydratePlaceholder" : "dehydrateFile";
    HILOG_INFO("CloudDiskSystemAccessor::%{public}s start", operation);
    int32_t accessRet = CheckAccess();
    if (accessRet != E_OK) {
        return Throw(env, accessRet);
    }
    NFuncArg args(env, info);
    if (!args.InitArgs(hydrate ? NARG_CNT::THREE : NARG_CNT::ONE)) {
        HILOG_ERROR("invalid arguments, argc:%{public}zu", args.GetArgc());
        return Throw(env, FileManagement::E_PARAMETER_ERROR);
    }
    std::string path;
    int32_t type = 0;
    int32_t priority = 0;
    if (!ReadString(env, args[NARG_POS::FIRST], path)) {
        HILOG_ERROR("invalid filePath: expected a nonempty absolute path");
        return Throw(env, FileManagement::E_PARAMETER_ERROR);
    }
    if (hydrate && !ReadEnum(env, args[NARG_POS::SECOND], MAX_CALLBACK_TYPE_LIMIT, type)) {
        HILOG_ERROR("invalid callbackType, expected integer 0..%{public}d", MAX_CALLBACK_TYPE_LIMIT);
        return Throw(env, FileManagement::E_PARAMETER_ERROR);
    }
    if (hydrate && !ReadEnum(env, args[NARG_POS::THIRD], MAX_PRIORITY_LIMIT, priority)) {
        HILOG_ERROR("invalid priority, expected integer 0..%{public}d", MAX_PRIORITY_LIMIT);
        return Throw(env, FileManagement::E_PARAMETER_ERROR);
    }
    auto accessor = GetAccessor(env, args.GetThisVar());
    if (accessor == nullptr) {
        HILOG_ERROR("CloudDiskSystemAccessor::%{public}s invalid receiver", operation);
        return Throw(env, FileManagement::E_PARAMETER_ERROR);
    }
    auto execute = [path, type, priority, hydrate]() -> NError {
        auto &manager = CloudDiskServiceManager::GetInstance();
        int32_t ret = hydrate ? manager.StartHydrationByPath(path, type, priority) : manager.DehydrateFileByPath(path);
        if (ret != E_OK) {
            HILOG_ERROR("execute failed ret:%{public}d, path:%{private}s", ret, path.c_str());
        }
        return MakeError(ret);
    };
    auto complete = [](napi_env env, NError error) -> NVal {
        return error ? NVal(env, error.GetNapiErr(env)) : NVal::CreateUndefined(env);
    };
    auto result = NAsyncWorkPromise(env, NVal(env, args.GetThisVar())).Schedule(
        hydrate ? "clouddisk_hydratePlaceholder" : "clouddisk_dehydrateFile", execute, complete);
    return HandleAsyncResult(env, result, operation);
}

napi_value HydratePlaceholder(napi_env env, napi_callback_info info)
{
    return RunOperation(env, info, true);
}

napi_value DehydrateFile(napi_env env, napi_callback_info info)
{
    return RunOperation(env, info, false);
}

bool ParseEvent(napi_env env, NFuncArg &args, bool on)
{
    const char *operation = on ? "on" : "off";
    if (!args.InitArgs(on ? NARG_CNT::TWO : NARG_CNT::ONE, NARG_CNT::TWO)) {
        HILOG_ERROR("CloudDiskSystemAccessor::%{public}s invalid arguments, argc:%{public}zu",
            operation, args.GetArgc());
        return false;
    }
    std::string event;
    if (!ReadString(env, args[NARG_POS::FIRST], event) || event != EVENT_NAME) {
        HILOG_ERROR("CloudDiskSystemAccessor::%{public}s invalid event: expected hydrateProgress", operation);
        return false;
    }
    if (args.GetArgc() == NARG_CNT::TWO) {
        napi_valuetype type = napi_undefined;
        napi_status status = napi_typeof(env, args[NARG_POS::SECOND], &type);
        if (status != napi_ok) {
            HILOG_ERROR("CloudDiskSystemAccessor::%{public}s get callback type failed, status:%{public}d",
                operation, static_cast<int32_t>(status));
            return false;
        }
        if (type != napi_function) {
            HILOG_ERROR("CloudDiskSystemAccessor::%{public}s invalid callback type:%{public}d",
                operation, static_cast<int32_t>(type));
            return false;
        }
    }
    return true;
}

napi_value On(napi_env env, napi_callback_info info)
{
    HILOG_INFO("CloudDiskSystemAccessor::on start");
    int32_t accessRet = CheckAccess();
    if (accessRet != E_OK) {
        return Throw(env, accessRet);
    }
    NFuncArg args(env, info);
    if (!ParseEvent(env, args, true)) {
        return Throw(env, FileManagement::E_PARAMETER_ERROR);
    }
    auto accessor = GetAccessor(env, args.GetThisVar());
    if (accessor == nullptr) {
        HILOG_ERROR("CloudDiskSystemAccessor::on invalid receiver");
        return Throw(env, FileManagement::E_PARAMETER_ERROR);
    }
    if (accessor->progress != nullptr) {
        HILOG_ERROR("CloudDiskSystemAccessor::on progress callback is already registered");
        return Throw(env, FileManagement::E_CALLBACK_ALREADY_REGISTERED);
    }
    auto progress = HydrateProgressCallback::Create(env, args[NARG_POS::SECOND]);
    if (progress == nullptr) {
        HILOG_ERROR("CloudDiskSystemAccessor::on create progress callback failed");
        return Throw(env, FileManagement::E_TRY_AGAIN);
    }
    int32_t ret = CloudDiskServiceManager::GetInstance().RegisterProgressCallback(progress);
    if (ret != E_OK) {
        HILOG_ERROR("CloudDiskSystemAccessor::on register progress callback failed, ret:%{public}d", ret);
        progress->Close();
        return Throw(env, ret);
    }
    accessor->progress = std::move(progress);
    return NVal::CreateUndefined(env).val_;
}

napi_value Off(napi_env env, napi_callback_info info)
{
    HILOG_INFO("CloudDiskSystemAccessor::off start");
    int32_t accessRet = CheckAccess();
    if (accessRet != E_OK) {
        return Throw(env, accessRet);
    }
    NFuncArg args(env, info);
    if (!ParseEvent(env, args, false)) {
        return Throw(env, FileManagement::E_PARAMETER_ERROR);
    }
    auto accessor = GetAccessor(env, args.GetThisVar());
    if (accessor == nullptr) {
        HILOG_ERROR("CloudDiskSystemAccessor::off invalid receiver");
        return Throw(env, FileManagement::E_PARAMETER_ERROR);
    }
    if (accessor->progress == nullptr) {
        HILOG_ERROR("CloudDiskSystemAccessor::off progress callback is not registered");
        return Throw(env, FileManagement::E_CALLBACK_NOT_REGISTERED);
    }
    int32_t ret = CloudDiskServiceManager::GetInstance().UnregisterProgressCallback(accessor->progress);
    accessor->progress->Close();
    accessor->progress = nullptr;
    if (ret != E_OK) {
        HILOG_ERROR("CloudDiskSystemAccessor::off unregister progress callback failed, ret:%{public}d", ret);
        return Throw(env, ret);
    }
    return NVal::CreateUndefined(env).val_;
}

} // namespace

std::string CloudDiskSystemNExporter::GetClassName()
{
    return CLASS_NAME;
}

bool CloudDiskSystemNExporter::Export()
{
    HILOG_INFO("CloudDiskSystemAccessor::Export start");
    std::vector<napi_property_descriptor> props = {
        NVal::DeclareNapiFunction("hydratePlaceholder", HydratePlaceholder),
        NVal::DeclareNapiFunction("dehydrateFile", DehydrateFile),
        NVal::DeclareNapiFunction("on", On),
        NVal::DeclareNapiFunction("off", Off)
    };
    std::string className = GetClassName();
    bool succ = false;
    napi_value classValue = nullptr;
    std::tie(succ, classValue) = NClass::DefineClass(
        exports_.env_, className, Constructor, std::move(props));
    if (!succ) {
        HILOG_ERROR("CloudDiskManagerNapi::Failed to define class %{public}s", className.c_str());
        MakeError(FileManagement::E_TRY_AGAIN).ThrowErr(exports_.env_);
        return false;
    }
    succ = NClass::SaveClass(exports_.env_, className, classValue);
    if (!succ) {
        HILOG_ERROR("CloudDiskManagerNapi::Failed to save class %{public}s", className.c_str());
        MakeError(FileManagement::E_TRY_AGAIN).ThrowErr(exports_.env_);
        return false;
    }
    succ = exports_.AddProp(className, classValue);
    if (!succ) {
        HILOG_ERROR("CloudDiskManagerNapi::Failed to add class %{public}s to exports", className.c_str());
    }
    return succ;
}
} // namespace OHOS::FileManagement::CloudDiskService
