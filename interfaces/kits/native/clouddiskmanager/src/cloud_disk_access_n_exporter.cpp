/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "cloud_disk_access_n_exporter.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "cloud_disk_error.h"
#include "cloud_disk_placeholder_manager.h"
#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"

namespace OHOS::FileManagement::CloudDiskService {
using namespace std;
using namespace LibN;
using namespace FileAccessFwk;

namespace {
const string CLOUD_DISK_ACCESSOR = "CloudDiskAccessor";

bool IsInvalidPath(const string &path)
{
    return path.empty() || path.find('\0') != string::npos;
}

NError MakeCloudDiskServiceNError(int32_t errCode)
{
    return NError([errCode]() -> tuple<uint32_t, string> {
        auto cloudDiskErrorInfo = GetCloudDiskErrorInfo(errCode);
        return {static_cast<uint32_t>(cloudDiskErrorInfo.code), cloudDiskErrorInfo.message};
    });
}

NError MakeCloudDiskApiNError(int32_t errCode)
{
    return NError([errCode]() -> tuple<uint32_t, string> {
        auto cloudDiskErrorInfo = GetCloudDiskApiErrorInfo(errCode);
        return {static_cast<uint32_t>(cloudDiskErrorInfo.code), cloudDiskErrorInfo.message};
    });
}

bool ParseStringArg(napi_env env, napi_value arg, string &value)
{
    auto [succ, str, strLen] = NVal(env, arg).ToUTF8String();
    if (!succ || str == nullptr) {
        return false;
    }
    value.assign(str.get(), strLen);
    return !IsInvalidPath(value);
}

napi_value Constructor(napi_env env, napi_callback_info cbinfo)
{
    HILOG_INFO("CloudDiskManagerNapi::CloudDiskAccessor::Constructor start");
    NFuncArg funcArg(env, cbinfo);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        HILOG_ERROR("CloudDiskManagerNapi::CloudDiskAccessor::Constructor invalid argument count");
        MakeCloudDiskApiNError(CLOUD_DISK_INVALID_ARG).ThrowErr(env);
        return nullptr;
    }

    string syncFolderPath;
    if (!ParseStringArg(env, funcArg[NARG_POS::FIRST], syncFolderPath)) {
        HILOG_ERROR("CloudDiskManagerNapi::CloudDiskAccessor::Constructor invalid sync folder path");
        MakeCloudDiskApiNError(CLOUD_DISK_INVALID_ARG).ThrowErr(env);
        return nullptr;
    }

    unique_ptr<CloudDiskPlaceholderManager> cloudDiskPlaceholderManager =
        make_unique<CloudDiskPlaceholderManager>(syncFolderPath);
    if (!NClass::SetEntityFor<CloudDiskPlaceholderManager>(
        env, funcArg.GetThisVar(), std::move(cloudDiskPlaceholderManager))) {
        HILOG_ERROR("CloudDiskManagerNapi::CloudDiskAccessor::Constructor set entity failed");
        MakeCloudDiskApiNError(CLOUD_DISK_TRY_AGAIN).ThrowErr(env);
        return nullptr;
    }
    HILOG_INFO("CloudDiskManagerNapi::CloudDiskAccessor::Constructor success");
    return funcArg.GetThisVar();
}

napi_value IsPlaceholderFile(napi_env env, napi_callback_info cbinfo)
{
    HILOG_INFO("CloudDiskManagerNapi::CloudDiskAccessor::IsPlaceholderFile start");
    NFuncArg funcArg(env, cbinfo);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        HILOG_ERROR("CloudDiskManagerNapi::CloudDiskAccessor::IsPlaceholderFile invalid argument count");
        MakeCloudDiskApiNError(CLOUD_DISK_INVALID_ARG).ThrowErr(env);
        return nullptr;
    }

    string relativePath;
    if (!ParseStringArg(env, funcArg[NARG_POS::FIRST], relativePath)) {
        HILOG_ERROR("CloudDiskManagerNapi::CloudDiskAccessor::IsPlaceholderFile invalid relative path");
        MakeCloudDiskApiNError(CLOUD_DISK_INVALID_ARG).ThrowErr(env);
        return nullptr;
    }

    auto isPlaceholder = make_shared<bool>(false);
    auto cloudDiskPlaceholderAccess = NClass::GetEntityOf<CloudDiskPlaceholderManager>(env, funcArg.GetThisVar());
    if (!cloudDiskPlaceholderAccess) {
        HILOG_ERROR("CloudDiskManagerNapi::CloudDiskAccessor::IsPlaceholderFile get entity failed");
        MakeCloudDiskApiNError(CLOUD_DISK_TRY_AGAIN).ThrowErr(env);
        return nullptr;
    }
    auto cbExec = [relativePath, isPlaceholder, cloudDiskPlaceholderAccess]() -> NError {
        HILOG_INFO("CloudDiskManagerNapi::CloudDiskAccessor::IsPlaceholderFile async execute");
        if (!cloudDiskPlaceholderAccess) {
            HILOG_ERROR("CloudDiskManagerNapi::CloudDiskAccessor::IsPlaceholderFile entity is nullptr");
            return MakeCloudDiskApiNError(CLOUD_DISK_TRY_AGAIN);
        }
        int ret = cloudDiskPlaceholderAccess->IsPlaceholderFile(relativePath, *isPlaceholder);
        HILOG_INFO("CloudDiskManagerNapi::CloudDiskAccessor::IsPlaceholderFile async ret=%{public}d", ret);
        return MakeCloudDiskServiceNError(ret);
    };
    auto cbCompl = [isPlaceholder](napi_env env, NError err) -> NVal {
        if (err) {
            HILOG_ERROR("CloudDiskManagerNapi::CloudDiskAccessor::IsPlaceholderFile async complete with error");
            return {env, err.GetNapiErr(env)};
        }
        HILOG_INFO("CloudDiskManagerNapi::CloudDiskAccessor::IsPlaceholderFile async complete success");
        return NVal::CreateBool(env, *isPlaceholder);
    };
    const string procedureName = "clouddisk_isPlaceholderFile";
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
}
} // namespace

CloudDiskAccessNExporter::CloudDiskAccessNExporter(napi_env env, napi_value exports) : NExporter(env, exports) {}

CloudDiskAccessNExporter::~CloudDiskAccessNExporter() {};

std::string CloudDiskAccessNExporter::GetClassName()
{
    return CLOUD_DISK_ACCESSOR;
}

bool CloudDiskAccessNExporter::Export()
{
    vector<napi_property_descriptor> props = {
        NVal::DeclareNapiFunction("isPlaceholderFile", IsPlaceholderFile)
    };
    string className = GetClassName();
    bool succ = false;
    napi_value classValue = nullptr;
    tie(succ, classValue) = NClass::DefineClass(
        exports_.env_, className, Constructor, std::move(props));
    if (!succ) {
        HILOG_ERROR("CloudDiskManagerNapi::Failed to define class %{public}s", className.c_str());
        MakeCloudDiskApiNError(CLOUD_DISK_TRY_AGAIN).ThrowErr(exports_.env_);
        return false;
    }
    succ = NClass::SaveClass(exports_.env_, className, classValue);
    if (!succ) {
        HILOG_ERROR("CloudDiskManagerNapi::Failed to save class %{public}s", className.c_str());
        MakeCloudDiskApiNError(CLOUD_DISK_TRY_AGAIN).ThrowErr(exports_.env_);
        return false;
    }
    return exports_.AddProp(className, classValue);
}
} // namespace OHOS::FileManagement::CloudDiskService
