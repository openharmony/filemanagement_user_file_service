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

#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "cloud_disk_js_manager.h"
#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"

namespace OHOS::FileManagement::CloudDiskService {
using namespace std;
using namespace LibN;
using namespace FileAccessFwk;

namespace {
bool IsInvalidPath(const string &path)
{
    return path.empty() || path.find('\0') != string::npos;
}

NError MakeCloudDiskNError(int32_t errCode)
{
    return NError([errCode]() -> tuple<uint32_t, string> {
        int32_t cloudDiskErrCode = CloudDiskJSManager::ConvertToCloudDiskApiErrCode(errCode);
        return {static_cast<uint32_t>(cloudDiskErrCode), CloudDiskJSManager::GetCloudDiskErrMsg(cloudDiskErrCode)};
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
} // namespace

CloudDiskAccessNExporter::CloudDiskAccessNExporter(napi_env env, napi_value exports) : NExporter(env, exports) {}

CloudDiskAccessNExporter::~CloudDiskAccessNExporter() {};

napi_value CloudDiskAccessNExporter::Constructor(napi_env env, napi_callback_info cbinfo)
{
    NFuncArg funcArg(env, cbinfo);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        HILOG_ERROR("Number of arguments unmatched");
        MakeCloudDiskNError(E_INVALID_PARAM).ThrowErr(env);
        return nullptr;
    }

    string syncFolderPath;
    if (!ParseStringArg(env, funcArg[NARG_POS::FIRST], syncFolderPath)) {
        HILOG_ERROR("Invalid sync folder path");
        MakeCloudDiskNError(E_INVALID_PARAM).ThrowErr(env);
        return nullptr;
    }

    unique_ptr<CloudDiskJSManager> cloudDiskJSManager = make_unique<CloudDiskJSManager>(syncFolderPath);
    if (!NClass::SetEntityFor<CloudDiskJSManager>(env, funcArg.GetThisVar(), std::move(cloudDiskJSManager))) {
        MakeCloudDiskNError(E_TRY_AGAIN).ThrowErr(env);
        return nullptr;
    }
    return funcArg.GetThisVar();
}

napi_value CloudDiskAccessNExporter::IsPlaceholderFile(napi_env env, napi_callback_info cbinfo)
{
    NFuncArg funcArg(env, cbinfo);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        HILOG_ERROR("Number of arguments unmatched");
        MakeCloudDiskNError(E_INVALID_PARAM).ThrowErr(env);
        return nullptr;
    }

    string relativePath;
    if (!ParseStringArg(env, funcArg[NARG_POS::FIRST], relativePath)) {
        HILOG_ERROR("Invalid relative path");
        MakeCloudDiskNError(E_INVALID_PARAM).ThrowErr(env);
        return nullptr;
    }

    auto isPlaceholder = make_shared<bool>(false);
    auto cloudDiskJSAccess = NClass::GetEntityOf<CloudDiskJSManager>(env, funcArg.GetThisVar());
    if (!cloudDiskJSAccess) {
        HILOG_ERROR("GetEntityOf cloudDiskJSAccess failed");
        MakeCloudDiskNError(E_TRY_AGAIN).ThrowErr(env);
        return nullptr;
    }
    auto cbExec = [relativePath, isPlaceholder, cloudDiskJSAccess]() -> NError {
        if (!cloudDiskJSAccess) {
            HILOG_ERROR("cloudDiskJSAccess is nullptr");
            return MakeCloudDiskNError(E_TRY_AGAIN);
        }
        int ret = cloudDiskJSAccess->IsPlaceholderFile(relativePath, *isPlaceholder);
        return MakeCloudDiskNError(ret);
    };
    auto cbCompl = [isPlaceholder](napi_env env, NError err) -> NVal {
        if (err) {
            return {env, err.GetNapiErr(env)};
        }
        return NVal::CreateBool(env, *isPlaceholder);
    };
    const string procedureName = "clouddisk_isPlaceholderFile";
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
}

std::string CloudDiskAccessNExporter::GetClassName()
{
    return CloudDiskAccessNExporter::className_;
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
        exports_.env_, className, CloudDiskAccessNExporter::Constructor, std::move(props));
    if (!succ) {
        HILOG_ERROR("Failed to define class %{public}s", className.c_str());
        MakeCloudDiskNError(E_TRY_AGAIN).ThrowErr(exports_.env_);
        return false;
    }
    succ = NClass::SaveClass(exports_.env_, className, classValue);
    if (!succ) {
        HILOG_ERROR("Failed to save class %{public}s", className.c_str());
        MakeCloudDiskNError(E_TRY_AGAIN).ThrowErr(exports_.env_);
        return false;
    }
    return exports_.AddProp(className, classValue);
}
} // namespace OHOS::FileManagement::CloudDiskService
