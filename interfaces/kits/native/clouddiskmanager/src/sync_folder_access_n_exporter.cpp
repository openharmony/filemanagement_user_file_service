/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "sync_folder_access_n_exporter.h"

#include <tuple>

#include "cloud_disk_js_manager.h"
#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"

namespace OHOS::FileManagement::CloudDiskService {
using namespace std;
using namespace LibN;
using namespace FileAccessFwk;
constexpr const char* PATH = "path";
constexpr const char* STATE = "state";
constexpr const char* DISPLAY_NAME_RES_ID = "displayNameResId";
constexpr const char* CUSTOM_ALIAS = "customAlias";
constexpr const char* BUNDLE_NAME = "bundleName";

static napi_status CreateOneSyncFolderExt(napi_env env,
    const SyncFolderExt &syncFolderExt, napi_value resVec, size_t index)
{
    napi_value ele = nullptr;
    napi_status status = napi_ok;
    if ((status = napi_create_object(env, &ele)) != napi_ok) {
        HILOG_ERROR("Create object failed");
        return status;
    }
    if ((status = napi_set_named_property(
        env, ele, PATH, NVal::CreateUTF8String(env, syncFolderExt.path_).val_)) != napi_ok) {
        HILOG_ERROR("Set named property path failed");
        return status;
    }
    if ((status = napi_set_named_property(env, ele, STATE,
        NVal::CreateInt32(env, static_cast<int32_t>(syncFolderExt.state_)).val_)) != napi_ok) {
        HILOG_ERROR("Set named property state failed");
        return status;
    }
    if (syncFolderExt.displayNameResId_ != 0) {
        if ((status = napi_set_named_property(env, ele, DISPLAY_NAME_RES_ID,
            NVal::CreateInt32(env, syncFolderExt.displayNameResId_).val_)) != napi_ok) {
            HILOG_ERROR("Set named property resId failed");
            return status;
        }
    }
    if (!syncFolderExt.displayName_.empty()) {
        if ((status = napi_set_named_property(env,
            ele, CUSTOM_ALIAS, NVal::CreateUTF8String(env, syncFolderExt.displayName_).val_)) != napi_ok) {
            HILOG_ERROR("Set named property displayName failed");
            return status;
        }
    }
    if ((status = napi_set_named_property(env,
        ele, BUNDLE_NAME, NVal::CreateUTF8String(env, syncFolderExt.bundleName_).val_)) != napi_ok) {
        HILOG_ERROR("Set named property bundleName failed");
        return status;
    }
    if ((status = napi_set_element(env, resVec, index, ele)) != napi_ok) {
        HILOG_ERROR("Set element failed");
        return status;
    }
    return napi_ok;
}

static std::tuple<napi_status, napi_value> CreateSyncFolderExtList(napi_env env,
    const std::vector<SyncFolderExt> &syncFolderExts)
{
    napi_value res = nullptr;
    napi_status status = napi_ok;
    if ((status = napi_create_array(env, &res)) != napi_ok) {
        HILOG_ERROR("Create array failed");
        return { status, res };
    }
    for (size_t i = 0; i < syncFolderExts.size(); ++i) {
        if ((status = CreateOneSyncFolderExt(env, syncFolderExts[i], res, i)) != napi_ok) {
            return { status, res };
        }
    }
    return { napi_ok, res };
}

SyncFolderAccessNExporter::SyncFolderAccessNExporter(napi_env env, napi_value exports): NExporter(env, exports) {}

SyncFolderAccessNExporter::~SyncFolderAccessNExporter() {};

napi_value SyncFolderAccessNExporter::GetAllSyncFolders(napi_env env, napi_callback_info cbinfo)
{
    NFuncArg funcArg(env, cbinfo);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        HILOG_ERROR("Number of arguments unmatched");
        NError(E_INVALID_PARAM).ThrowErr(env);
        return nullptr;
    }
    auto result = std::make_shared<std::vector<SyncFolderExt>>();
    auto syncFolderJSAccess = NClass::GetEntityOf<CloudDiskJSManager>(env, funcArg.GetThisVar());
    if (!syncFolderJSAccess) {
        HILOG_ERROR("GetEntityOf syncFolderJSAccess failed");
        NError(E_TRY_AGAIN).ThrowErr(env);
        return nullptr;
    }
    auto cbExec = [result, syncFolderJSAccess]() -> NError {
        if (!syncFolderJSAccess) {
            HILOG_ERROR("syncFolderJSAccess is nullptr");
            return NError(E_TRY_AGAIN);
        }
        int ret = syncFolderJSAccess->GetAllSyncFolders(*result);
        return NError(ret);
    };
    auto cbCompl = [result](napi_env env, NError err) -> NVal {
        if (err) {
            return {env, err.GetNapiErr(env)};
        }
        auto [succSta, syncFolderVec] = CreateSyncFolderExtList(env, *result);
        if (succSta == napi_ok) {
            return {env, syncFolderVec};
        }
        HILOG_ERROR("CreateSyncFolderExtList failed");
        return {env, NError(E_TRY_AGAIN).GetNapiErr(env)};
    };
    const string procedureName = "clouddisk_getAllSyncFolders";
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
}

std::string SyncFolderAccessNExporter::GetClassName()
{
    return SyncFolderAccessNExporter::className_;
}

napi_value SyncFolderAccessNExporter::Constructor(napi_env env, napi_callback_info cbinfo)
{
    NFuncArg funcArg(env, cbinfo);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        HILOG_ERROR("Number of arguments unmatched");
        NError(E_INVALID_PARAM).ThrowErr(env);
        return nullptr;
    }
    unique_ptr<CloudDiskJSManager> cloudDiskJSManager = make_unique<CloudDiskJSManager>();
    if (!NClass::SetEntityFor<CloudDiskJSManager>(env, funcArg.GetThisVar(), std::move(cloudDiskJSManager))) {
        NError(E_TRY_AGAIN).ThrowErr(env);
        return nullptr;
    }
    return funcArg.GetThisVar();
}

bool SyncFolderAccessNExporter::Export()
{
    vector<napi_property_descriptor> props = {
        NVal::DeclareNapiFunction("getAllSyncFolders", GetAllSyncFolders)
    };
    string className = GetClassName();
    bool succ = false;
    napi_value classValue = nullptr;
    std::tie(succ, classValue) = NClass::DefineClass(
        exports_.env_, className, SyncFolderAccessNExporter::Constructor, std::move(props));
    if (!succ) {
        HILOG_ERROR("Failed to define class %{public}s", className.c_str());
        NError(E_TRY_AGAIN).ThrowErr(exports_.env_);
        return false;
    }
    succ = NClass::SaveClass(exports_.env_, className, classValue);
    if (!succ) {
        HILOG_ERROR("Failed to save class %{public}s", className.c_str());
        NError(E_TRY_AGAIN).ThrowErr(exports_.env_);
        return false;
    }
    return exports_.AddProp(className, classValue);
}
} // namespace OHOS::FileManagement::CloudDiskService
