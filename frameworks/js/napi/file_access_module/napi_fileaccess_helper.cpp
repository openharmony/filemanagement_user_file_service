/*
 * Copyright (C) 2022-2024 Huawei Device Co., Ltd.
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

#include "napi_fileaccess_helper.h"

#include <cstring>
#include <utility>
#include <vector>
#include <mutex>

#include "file_access_framework_errno.h"
#include "file_access_helper.h"
#include "file_info_entity.h"
#include "filemgmt_libn.h"
#include "hilog_wrapper.h"
#include "napi_base_context.h"
#include "napi_common_fileaccess.h"
#include "napi_file_info_exporter.h"
#include "napi_observer_callback.h"
#include "napi_root_iterator_exporter.h"
#include "root_iterator_entity.h"
#include "securec.h"
#include "uri.h"
#include "file_access_check_util.h"

using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;
using namespace OHOS::FileManagement::LibN;

namespace OHOS {
namespace FileAccessFwk {
namespace {
    const std::string FILEACCESS_CLASS_NAME = "FileAccessHelper";
    thread_local napi_ref g_constructorRef = nullptr;
    constexpr uint32_t INITIAL_REFCOUNT = 1;
    constexpr int COPY_EXCEPTION = -1;
    using CallbackExec = std::function<NError()>;
    using CallbackComplete = std::function<NVal(napi_env, NError)>;
}

static std::mutex g_fileAccessHelperMutex;
static std::list<std::shared_ptr<FileAccessHelper>> g_fileAccessHelperList = {};

static std::shared_ptr<FileAccessHelper> GetGlobalFileAccessHelper(const FileAccessHelper *helper)
{
    if (helper == nullptr) {
        HILOG_ERROR("Input helper is nullptr");
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(g_fileAccessHelperMutex);
    auto helperIter = std::find_if(g_fileAccessHelperList.begin(), g_fileAccessHelperList.end(),
        [&helper](const std::shared_ptr<FileAccessHelper> &element) {
            return element.get() == helper;
    });
    if (helperIter == g_fileAccessHelperList.end()) {
        HILOG_ERROR("Cannot find helper in global");
        return nullptr;
    }
    return *helperIter;
}

static void AddFileAccessHelperList(std::pair<std::shared_ptr<FileAccessHelper>, int> &createResult)
{
    std::lock_guard<std::mutex> lock(g_fileAccessHelperMutex);
    g_fileAccessHelperList.emplace_back(createResult.first);
    HILOG_INFO("g_fileAccessHelperList size %{public}zu", g_fileAccessHelperList.size());
}

static void RemoveFileAccessHelperList(FileAccessHelper *objectInfo)
{
    std::lock_guard<std::mutex> lock(g_fileAccessHelperMutex);
    g_fileAccessHelperList.remove_if([objectInfo](const std::shared_ptr<FileAccessHelper> &fileAccessHelper) {
        return objectInfo == fileAccessHelper.get();
    });
    HILOG_INFO("g_fileAccessHelperList size %{public}zu", g_fileAccessHelperList.size());
}

static napi_value FileAccessHelperConstructor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE, NARG_CNT::TWO)) {
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }
    napi_value thisVar = funcArg.GetThisVar();
    std::pair<std::shared_ptr<FileAccessHelper>, int> createResult{nullptr, ERR_OK};
    bool isStageMode = false;
    napi_status status = AbilityRuntime::IsStageContext(env, funcArg.GetArg(PARAM0), isStageMode);
    if (status != napi_ok || !isStageMode) {
        HILOG_INFO("No support FA Model");
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }
    auto context = OHOS::AbilityRuntime::GetStageModeContext(env, funcArg.GetArg(PARAM0));
    CHECK_STATUS_RETURN(context != nullptr, NapiFileInfoExporter::ThrowError(env, E_GETRESULT),
        "FileAccessHelperConstructor: failed to get native context");
    if (funcArg.GetArgc() == NARG_CNT::ONE) {
        createResult = FileAccessHelper::Creator(context);
    } else if (funcArg.GetArgc() == NARG_CNT::TWO) {
        std::vector<AAFwk::Want> wants;
        bool suss = UnwrapArrayWantFromJS(env, funcArg.GetArg(PARAM1), wants);
        CHECK_STATUS_RETURN(suss, NapiFileInfoExporter::ThrowError(env, E_GETRESULT),
            "UnwrapArrayWantFromJS failed to get native wants");
        createResult = FileAccessHelper::Creator(context, wants);
    }
    CHECK_STATUS_RETURN(!(createResult.first == nullptr || createResult.second != ERR_OK),
        NapiFileInfoExporter::ThrowError(env, createResult.second),
        "FileAccessHelperConstructor: Creator failed ret=%{public}d", createResult.second);
    AddFileAccessHelperList(createResult);
    auto finalize = [](napi_env env, void *data, void *hint) {
        FileAccessHelper *objectInfo = static_cast<FileAccessHelper *>(data);
        if (objectInfo != nullptr) {
            RemoveFileAccessHelperList(objectInfo);
            objectInfo = nullptr;
        }
    };
    if (napi_wrap(env, thisVar, createResult.first.get(), finalize, nullptr, nullptr) != napi_ok) {
        finalize(env, createResult.first.get(), nullptr);
        return NapiFileInfoExporter::ThrowError(env, E_GETRESULT);
    }
    return thisVar;
}

napi_value AcquireFileAccessHelperWrap(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE, NARG_CNT::TWO)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    napi_value result = nullptr;
    napi_value cons = nullptr;
    if (funcArg.GetArgc() == NARG_CNT::ONE) {
        size_t requireArgc = ARGS_ONE;
        size_t argc = ARGS_ONE;
        napi_value args[ARGS_ONE] = {nullptr};
        if (napi_get_cb_info(env, info, &argc, args, nullptr, nullptr) != napi_ok) {
            return nullptr;
        }
        CHECK_STATUS_RETURN(!(argc > requireArgc || napi_get_reference_value(env, g_constructorRef, &cons) != napi_ok),
            nullptr, "Wrong argument count%{public}zu. or g_constructorRef reference is fail", argc);
        if (napi_new_instance(env, cons, ARGS_ONE, args, &result) != napi_ok) {
            return nullptr;
        }
    } else if (funcArg.GetArgc() == NARG_CNT::TWO) {
        size_t requireArgc = ARGS_TWO;
        size_t argc = ARGS_TWO;
        napi_value args[ARGS_TWO] = {nullptr};
        if (napi_get_cb_info(env, info, &argc, args, nullptr, nullptr) != napi_ok) {
            return nullptr;
        }
        CHECK_STATUS_RETURN(!(argc > requireArgc || napi_get_reference_value(env, g_constructorRef, &cons) != napi_ok),
            nullptr, "Wrong argument count%{public}zu. or g_constructorRef reference is fail", argc);
        if (napi_new_instance(env, cons, ARGS_TWO, args, &result) != napi_ok) {
            return nullptr;
        }
    }
    CHECK_STATUS_RETURN(IsTypeForNapiValue(env, result, napi_object), nullptr,
        "IsTypeForNapiValue isn't object");
    FileAccessHelper *fileAccessHelper = nullptr;
    CHECK_STATUS_RETURN(napi_unwrap(env, result, (void **)&fileAccessHelper) == napi_ok, nullptr,
        "Faild to get fileAccessHelper");
    CHECK_STATUS_RETURN(fileAccessHelper != nullptr, nullptr, "fileAccessHelper is nullptr");
    HILOG_INFO("g_fileAccessHelperList size %{public}zu", g_fileAccessHelperList.size());
    return result;
}

napi_value NAPI_CreateFileAccessHelper(napi_env env, napi_callback_info info)
{
    napi_value ret = AcquireFileAccessHelperWrap(env, info);
    if (ret == nullptr) {
        ret = WrapVoidToJS(env);
    }
    return ret;
}

napi_value NAPI_GetFileAccessAbilityInfo(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO, NARG_CNT::ONE)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    auto result = std::make_shared<std::vector<AAFwk::Want>>();
    if (!result) {
        NError(E_GETRESULT).ThrowErr(env);
        return nullptr;
    }

    auto cbExec = [result]() -> NError {
        int ret = FileAccessHelper::GetRegisteredFileAccessExtAbilityInfo(*result);
        return NError(ret);
    };
    auto cbComplete = [result](napi_env env, NError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        }
        napi_value jsArray = WrapArrayWantToJS(env, *result);
        return {env, jsArray};
    };
    const std::string procedureName = "getFileAccessAbilityInfo";
    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::ZERO) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    }
    NVal cb(env, funcArg[NARG_POS::FIRST]);
    if (!cb.TypeIs(napi_function)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
}

napi_value FileAccessHelperInit(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("openFile", NAPI_OpenFile),
        DECLARE_NAPI_FUNCTION("mkDir", NAPI_Mkdir),
        DECLARE_NAPI_FUNCTION("createFile", NAPI_CreateFile),
        DECLARE_NAPI_FUNCTION("delete", NAPI_Delete),
        DECLARE_NAPI_FUNCTION("move", NAPI_Move),
        DECLARE_NAPI_FUNCTION("query", NAPI_Query),
        DECLARE_NAPI_FUNCTION("copy", NAPI_Copy),
        DECLARE_NAPI_FUNCTION("copyFile", NAPI_CopyFile),
        DECLARE_NAPI_FUNCTION("rename", NAPI_Rename),
        DECLARE_NAPI_FUNCTION("getRoots", NAPI_GetRoots),
        DECLARE_NAPI_FUNCTION("access", NAPI_Access),
        DECLARE_NAPI_FUNCTION("getFileInfoFromUri", NAPI_GetFileInfoFromUri),
        DECLARE_NAPI_FUNCTION("getFileInfoFromRelativePath", NAPI_GetFileInfoFromRelativePath),
        DECLARE_NAPI_FUNCTION("registerObserver", NAPI_RegisterObserver),
        DECLARE_NAPI_FUNCTION("unregisterObserver", NAPI_UnregisterObserver),
        DECLARE_NAPI_FUNCTION("moveItem", NAPI_MoveItem),
        DECLARE_NAPI_FUNCTION("moveFile", NAPI_MoveFile),
    };
    napi_value cons = nullptr;
    NAPI_CALL(env,
        napi_define_class(env,
            FILEACCESS_CLASS_NAME.c_str(),
            NAPI_AUTO_LENGTH,
            FileAccessHelperConstructor,
            nullptr,
            sizeof(properties) / sizeof(*properties),
            properties,
            &cons));
    NAPI_CALL(env, napi_create_reference(env, cons, INITIAL_REFCOUNT, &g_constructorRef));
    NAPI_CALL(env, napi_set_named_property(env, exports, FILEACCESS_CLASS_NAME.c_str(), cons));

    napi_property_descriptor export_properties[] = {
        DECLARE_NAPI_FUNCTION("createFileAccessHelper", NAPI_CreateFileAccessHelper),
        DECLARE_NAPI_FUNCTION("getFileAccessAbilityInfo", NAPI_GetFileAccessAbilityInfo),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(export_properties) / sizeof(export_properties[0]),
        export_properties));
    return exports;
}

static FileAccessHelper *GetFileAccessHelper(napi_env env, napi_value thisVar)
{
    if (thisVar == nullptr) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    FileAccessHelper *fileAccessHelper = nullptr;
    if (napi_unwrap(env, thisVar, (void **)&fileAccessHelper) != napi_ok) {
        NError(E_GETRESULT).ThrowErr(env);
        return nullptr;
    }

    if (fileAccessHelper == nullptr) {
        NError(E_GETRESULT).ThrowErr(env);
        return nullptr;
    }
    return fileAccessHelper;
}

static std::tuple<bool, std::unique_ptr<char[]>, std::unique_ptr<char[]>> GetReadArg(napi_env env,
                                                                                     napi_value sourceFile,
                                                                                     napi_value targetParent)
{
    bool succ = false;
    std::unique_ptr<char[]> uri = nullptr;
    std::unique_ptr<char[]> name = nullptr;
    std::tie(succ, uri, std::ignore) = NVal(env, sourceFile).ToUTF8String();
    if (!succ) {
        NError(EINVAL).ThrowErr(env);
        return { false, std::move(uri), std::move(name) };
    }

    std::tie(succ, name, std::ignore) = NVal(env, targetParent).ToUTF8String();
    if (!succ) {
        NError(EINVAL).ThrowErr(env);
        return { false, std::move(uri), std::move(name) };
    }

    return { true, std::move(uri), std::move(name) };
}

napi_value NAPI_OpenFile(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::THREE)) {
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }
    bool succ = false;
    std::unique_ptr<char[]> uri;
    std::tie(succ, uri, std::ignore) = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    if (!succ) {
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }
    int flags;
    std::tie(succ, flags) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }
    FileAccessHelper *fileAccessHelper = GetFileAccessHelper(env, funcArg.GetThisVar());
    if (fileAccessHelper == nullptr) {
        return nullptr;
    }
    auto result = std::make_shared<int>();
    if (!result) {
        return NapiFileInfoExporter::ThrowError(env, E_GETRESULT);
    }
    string uriString(uri.get());
    auto cbExec = [uriString, flags, result, fileAccessHelper]() -> NError {
        OHOS::Uri uri(uriString);
        int ret = fileAccessHelper->OpenFile(uri, flags, *result);
        return NError(ret);
    };
    auto cbComplete = [result](napi_env env, NError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        }
        return { NVal::CreateInt32(env, *result) };
    };
    const std::string procedureName = "openFile";
    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::TWO) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    }
    NVal cb(env, funcArg[NARG_POS::THIRD]);
    if (!cb.TypeIs(napi_function)) {
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }
    return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
}

napi_value NAPI_CreateFile(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::THREE)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    bool succ = false;
    std::unique_ptr<char[]> uri;
    std::unique_ptr<char[]> displayName;
    std::tie(succ, uri, displayName) = GetReadArg(env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND]);
    if (!succ) {
        return nullptr;
    }

    FileAccessHelper *fileAccessHelper = GetFileAccessHelper(env, funcArg.GetThisVar());
    if (fileAccessHelper == nullptr) {
        return nullptr;
    }

    auto result = std::make_shared<string>();
    if (!result) {
        NError(E_GETRESULT).ThrowErr(env);
        return nullptr;
    }

    string uriString(uri.get());
    string name(displayName.get());
    auto cbExec = [uriString, name, result, fileAccessHelper]() -> NError {
        OHOS::Uri uri(uriString);
        std::string newFile = "";
        OHOS::Uri newFileUri(newFile);
        int ret = fileAccessHelper->CreateFile(uri, name, newFileUri);
        *result = newFileUri.ToString();
        return NError(ret);
    };
    auto cbComplete = [result](napi_env env, NError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        }
        return { NVal::CreateUTF8String(env, *result) };
    };
    const std::string procedureName = "createFile";
    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::TWO) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    }

    NVal cb(env, funcArg[NARG_POS::THIRD]);
    if (!cb.TypeIs(napi_function)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
}

napi_value NAPI_Mkdir(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::THREE)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    bool succ = false;
    std::unique_ptr<char[]> uri;
    std::unique_ptr<char[]> displayName;
    std::tie(succ, uri, displayName) = GetReadArg(env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND]);
    if (!succ) {
        return nullptr;
    }

    FileAccessHelper *fileAccessHelper = GetFileAccessHelper(env, funcArg.GetThisVar());
    if (fileAccessHelper == nullptr) {
        return nullptr;
    }

    auto result = std::make_shared<string>();
    if (!result) {
        NError(E_GETRESULT).ThrowErr(env);
        return nullptr;
    }

    string uriString(uri.get());
    string name(displayName.get());
    auto cbExec = [uriString, name, result, fileAccessHelper]() -> NError {
        OHOS::Uri uri(uriString);
        std::string newFile = "";
        OHOS::Uri newFileUri(newFile);
        int ret = fileAccessHelper->Mkdir(uri, name, newFileUri);
        *result = newFileUri.ToString();
        return NError(ret);
    };
    auto cbComplete = [result](napi_env env, NError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        }
        return { NVal::CreateUTF8String(env, *result) };
    };
    const std::string procedureName = "mkdir";
    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::TWO) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    }

    NVal cb(env, funcArg[NARG_POS::THIRD]);
    if (!cb.TypeIs(napi_function)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
}

napi_value NAPI_Delete(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE, NARG_CNT::TWO)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    bool succ = false;
    std::unique_ptr<char[]> uri;
    std::tie(succ, uri, std::ignore) = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    if (!succ) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    FileAccessHelper *fileAccessHelper = GetFileAccessHelper(env, funcArg.GetThisVar());
    if (fileAccessHelper == nullptr) {
        return nullptr;
    }

    auto result = std::make_shared<int>();
    if (!result) {
        NError(E_GETRESULT).ThrowErr(env);
        return nullptr;
    }

    string uriString(uri.get());
    auto cbExec = [uriString, result, fileAccessHelper]() -> NError {
        OHOS::Uri uri(uriString);
        *result = fileAccessHelper->Delete(uri);
        return NError(*result);
    };
    auto cbComplete = [result](napi_env env, NError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        }
        return { NVal::CreateInt32(env, ERRNO_NOERR) };
    };

    const std::string procedureName = "delete";
    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::ONE) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    }

    NVal cb(env, funcArg[NARG_POS::SECOND]);
    if (!cb.TypeIs(napi_function)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
}

napi_value NAPI_Move(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::THREE)) {
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }
    bool succ = false;
    std::unique_ptr<char[]> sourceFile;
    std::unique_ptr<char[]> targetParent;
    std::tie(succ, sourceFile, targetParent) = GetReadArg(env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND]);
    if (!succ) {
        return nullptr;
    }
    FileAccessHelper *fileAccessHelper = GetFileAccessHelper(env, funcArg.GetThisVar());
    if (fileAccessHelper == nullptr) {
        return nullptr;
    }
    auto result = std::make_shared<string>();
    if (!result) {
        return NapiFileInfoExporter::ThrowError(env, E_GETRESULT);
    }
    string sourceFileString(sourceFile.get());
    string targetParentString(targetParent.get());
    auto cbExec = [sourceFileString, targetParentString, result, fileAccessHelper]() -> NError {
        OHOS::Uri uri(sourceFileString);
        OHOS::Uri targetParentUri(targetParentString);
        std::string newFile = "";
        OHOS::Uri newFileUri(newFile);
        int ret = fileAccessHelper->Move(uri, targetParentUri, newFileUri);
        *result = newFileUri.ToString();
        return NError(ret);
    };
    auto cbComplete = [result](napi_env env, NError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        }
        return { NVal::CreateUTF8String(env, *result) };
    };
    const std::string procedureName = "move";
    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::TWO) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    }
    NVal cb(env, funcArg[NARG_POS::THIRD]);
    if (!cb.TypeIs(napi_function)) {
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }
    return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
}

napi_value NAPI_Query(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::THREE)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    bool succ = false;
    std::unique_ptr<char[]> uri;
    std::unique_ptr<char[]> metaJson;
    std::tie(succ, uri, metaJson) = GetReadArg(env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND]);
    if (!succ) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    FileAccessHelper *fileAccessHelper = GetFileAccessHelper(env, funcArg.GetThisVar());
    if (fileAccessHelper == nullptr) {
        return nullptr;
    }

    std::string uriString(uri.get());
    std::string metaJsonString(metaJson.get());
    auto metaJsonPtr = std::make_shared<string>(metaJsonString);
    auto cbExec = [uriString, metaJsonPtr, fileAccessHelper]() -> NError {
        OHOS::Uri uri(uriString);
        int ret = fileAccessHelper->Query(uri, *metaJsonPtr);
        return NError(ret);
    };

    auto cbComplete = [metaJsonPtr](napi_env env, NError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        }
        return { NVal::CreateUTF8String(env, *metaJsonPtr) };
    };

    const std::string procedureName = "query";
    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::TWO) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    }

    NVal cb(env, funcArg[NARG_POS::THIRD]);
    if (!cb.TypeIs(napi_function)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
}

static napi_value CreateObjectArray(napi_env env, std::vector<Result> result)
{
    uint32_t status = napi_ok;
    napi_value copyResultArray = nullptr;
    status = napi_create_array_with_length(env, result.size(), &copyResultArray);
    if (status != napi_ok) {
        HILOG_ERROR("Create napi array fail");
        return nullptr;
    }

    for (size_t i = 0; i < result.size(); i++) {
        Result &tmpResult = result.at(i);
        napi_value resultVal;
        status |= napi_create_object(env, &resultVal);
        napi_value tmpVal;
        status |= napi_create_string_utf8(env, tmpResult.sourceUri.c_str(), tmpResult.sourceUri.length(), &tmpVal);
        status |= napi_set_named_property(env, resultVal, "sourceUri", tmpVal);
        status |= napi_create_string_utf8(env, tmpResult.destUri.c_str(), tmpResult.destUri.length(), &tmpVal);
        status |= napi_set_named_property(env, resultVal, "destUri", tmpVal);
        status |= napi_create_int32(env, tmpResult.errCode, &tmpVal);
        status |= napi_set_named_property(env, resultVal, "errCode", tmpVal);
        status |= napi_create_string_utf8(env, tmpResult.errMsg.c_str(), tmpResult.errMsg.length(), &tmpVal);
        status |= napi_set_named_property(env, resultVal, "errMsg", tmpVal);
        status |= napi_set_element(env, copyResultArray, i, resultVal);
        if (status != napi_ok) {
            HILOG_ERROR("Create CopyResult object error");
            return nullptr;
        }
    }
    return copyResultArray;
}

static std::tuple<bool, std::string, std::string, bool> GetCopyArguments(napi_env env, NFuncArg &funcArg)
{
    bool retStatus = false;
    std::unique_ptr<char[]> srcPath;
    std::unique_ptr<char[]> destPath;
    std::tie(retStatus, srcPath, destPath) = GetReadArg(env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND]);
    if (!retStatus) {
        HILOG_ERROR("Get first or second argument error");
        return std::make_tuple(false, "", "", false);
    }
    std::string srcPathStr (srcPath.get());
    std::string destPathStr (destPath.get());

    bool force = false;
    if (funcArg.GetArgc() == NARG_CNT::THREE) {
        NVal thirdArg(env, funcArg[NARG_POS::THIRD]);
        if (thirdArg.TypeIs(napi_boolean)) {
            std::tie(retStatus, force) = NVal(env, funcArg[NARG_POS::THIRD]).ToBool();
            if (!retStatus) {
                HILOG_ERROR("Get third argument error");
                return std::make_tuple(false, "", "", false);
            }
        }
    }

    if (funcArg.GetArgc() == NARG_CNT::FOUR) {
        NVal thirdArg(env, funcArg[NARG_POS::THIRD]);
        if (thirdArg.TypeIs(napi_boolean)) {
            std::tie(retStatus, force) = NVal(env, funcArg[NARG_POS::THIRD]).ToBool();
            if (!retStatus) {
                HILOG_ERROR("Get third argument error");
                return std::make_tuple(false, "", "", false);
            }
        }
    }

    return std::make_tuple(true, srcPathStr, destPathStr, force);
}

static std::tuple<bool, std::string, std::string, std::string> GetCopyFileArguments(napi_env env,
    NFuncArg &funcArg)
{
    bool retStatus = false;
    std::unique_ptr<char[]> srcPath = nullptr;
    std::unique_ptr<char[]> destPath = nullptr;
    std::tie(retStatus, srcPath, destPath) = GetReadArg(env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND]);
    if (!retStatus) {
        HILOG_ERROR("Get first or second argument error");
        return std::make_tuple(false, "", "", "");
    }
    std::string srcPathStr(srcPath.get());
    std::string destPathStr(destPath.get());

    bool succFileName = false;
    std::unique_ptr<char[]> fileName = nullptr;
    std::tie(succFileName, fileName, std::ignore) = NVal(env, funcArg[NARG_POS::THIRD]).ToUTF8String();
    if (!succFileName) {
        HILOG_ERROR("Get third argument error");
        NError(EINVAL).ThrowErr(env);
        return {false, std::move(srcPathStr), std::move(destPathStr), ""};
    }
    std::string fileNameStr(fileName.get());
    return std::make_tuple(true, srcPathStr, destPathStr, fileNameStr);
}

static napi_value AddNAsyncWork(napi_env env, std::string procedureName, NFuncArg &funcArg, CallbackExec cbExec,
    CallbackComplete cbComplete)
{
    NVal thisVar(env, funcArg.GetThisVar());

    if (funcArg.GetArgc() == NARG_CNT::TWO) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    }

    if (funcArg.GetArgc() == NARG_CNT::THREE) {
        NVal thirdArg(env, funcArg[NARG_POS::THIRD]);
        if (thirdArg.TypeIs(napi_function)) {
            return NAsyncWorkCallback(env, thisVar, thirdArg).Schedule(procedureName, cbExec, cbComplete).val_;
        }
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    }

    NVal cb(env, funcArg[NARG_POS::FOURTH]);
    if (!cb.TypeIs(napi_function)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
}

napi_value NAPI_Copy(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }
    bool retStatus = false;
    std::string srcPathStr;
    std::string destPathStr;
    bool force = false;
    std::tie(retStatus, srcPathStr, destPathStr, force) = GetCopyArguments(env, funcArg);
    if (!retStatus) {
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }
    FileAccessHelper *fileAccessHelper = GetFileAccessHelper(env, funcArg.GetThisVar());
    if (fileAccessHelper == nullptr) {
        return nullptr;
    }
    auto result = std::make_shared<std::vector<Result>>();
    if (!result) {
        return NapiFileInfoExporter::ThrowError(env, E_GETRESULT);
    }
    int ret = ERR_OK;
    auto cbExec = [srcPathStr, destPathStr, force, result, &ret, fileAccessHelper]() -> NError {
        OHOS::Uri srcUri(srcPathStr);
        OHOS::Uri destUri(destPathStr);
        ret = fileAccessHelper->Copy(srcUri, destUri, *result, force);
        if ((ret == COPY_EXCEPTION) && !result->empty()) {
            return NError(result->at(0).errCode);
        }
        return NError();
    };
    auto cbComplete = [&ret, result](napi_env env, NError err) -> NVal {
        if (ret == COPY_EXCEPTION) {
            return { env, err.GetNapiErr(env) };
        }
        return { env, CreateObjectArray(env, *result) };
    };
    return AddNAsyncWork(env, "copy", funcArg, cbExec, cbComplete);
}

static napi_value AddCopyFileNAsyncWork(napi_env env, NFuncArg &funcArg, CallbackExec cbExec,
    CallbackComplete cbComplete)
{
    const std::string procedureName = "copyFile";
    NVal thisVar(env, funcArg.GetThisVar());

    if (funcArg.GetArgc() == NARG_CNT::THREE) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    }

    NVal cb(env, funcArg[NARG_POS::FOURTH]);
    if (!cb.TypeIs(napi_function)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
}

napi_value NAPI_CopyFile(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FOUR)) {
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }
    bool retStatus = false;
    std::string srcFileName = "";
    std::string destPath = "";
    std::string fileName = "";
    std::tie(retStatus, srcFileName, destPath, fileName) = GetCopyFileArguments(env, funcArg);
    if (!retStatus) {
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }

    FileAccessHelper *fileAccessHelper = GetFileAccessHelper(env, funcArg.GetThisVar());
    if (fileAccessHelper == nullptr) {
        return nullptr;
    }
    auto result = std::make_shared<string>();
    auto cbExec = [srcFileName, destPath, fileName, result, fileAccessHelper]() -> NError {
        OHOS::Uri srcUri(srcFileName);
        OHOS::Uri destUri(destPath);
        OHOS::Uri newFileUri("");
        int ret = fileAccessHelper->CopyFile(srcUri, destUri, fileName, newFileUri);
        *result = newFileUri.ToString();
        return NError(ret);
    };
    auto cbComplete = [result](napi_env env, NError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        }
        return NVal::CreateUTF8String(env, *result);
    };
    return AddCopyFileNAsyncWork(env, funcArg, cbExec, cbComplete);
}

napi_value NAPI_Rename(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::THREE)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    bool succ = false;
    std::unique_ptr<char[]> uri;
    std::unique_ptr<char[]> displayName;
    std::tie(succ, uri, displayName) = GetReadArg(env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND]);
    if (!succ) {
        return nullptr;
    }

    FileAccessHelper *fileAccessHelper = GetFileAccessHelper(env, funcArg.GetThisVar());
    if (fileAccessHelper == nullptr) {
        return nullptr;
    }

    auto result = std::make_shared<string>();
    if (!result) {
        NError(E_GETRESULT).ThrowErr(env);
        return nullptr;
    }

    string uriString(uri.get());
    string name(displayName.get());
    auto cbExec = [uriString, name, result, fileAccessHelper]() -> NError {
        OHOS::Uri uri(uriString);
        std::string newFile = "";
        OHOS::Uri newFileUri(newFile);
        int ret = fileAccessHelper->Rename(uri, name, newFileUri);
        *result = newFileUri.ToString();
        return NError(ret);
    };
    auto cbComplete = [result](napi_env env, NError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        }
        return { NVal::CreateUTF8String(env, *result) };
    };
    const std::string procedureName = "rename";
    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::TWO) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    }

    NVal cb(env, funcArg[NARG_POS::THIRD]);
    if (!cb.TypeIs(napi_function)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
}

static int MakeGetRootsResult(napi_env &env,
                              std::shared_ptr<FileAccessHelper> helper,
                              std::vector<RootInfo> &rootInfoVec,
                              NVal &nVal)
{
    auto objRootIterator = NClass::InstantiateClass(env, NapiRootIteratorExporter::className_, {});
    if (objRootIterator == nullptr) {
        HILOG_INFO("Cannot instantiate class NapiRootIteratorExporter");
        return E_GETRESULT;
    }

    auto rootIteratorEntity = NClass::GetEntityOf<RootIteratorEntity>(env, objRootIterator);
    if (rootIteratorEntity == nullptr) {
        HILOG_INFO("Cannot get the entity of RootIteratorEntity");
        return E_GETRESULT;
    }

    std::lock_guard<std::mutex> lock(rootIteratorEntity->entityOperateMutex);
    rootIteratorEntity->fileAccessHelper = helper;
    rootIteratorEntity->devVec = std::move(rootInfoVec);
    rootIteratorEntity->pos = 0;
    nVal = { env, objRootIterator };

    return ERR_OK;
}

napi_value NAPI_GetRoots(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO, NARG_CNT::ONE)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    FileAccessHelper *helper = GetFileAccessHelper(env, funcArg.GetThisVar());
    std::shared_ptr<FileAccessHelper> fileAccessHelper = GetGlobalFileAccessHelper(helper);
    if (fileAccessHelper == nullptr) {
        NError(E_GETRESULT).ThrowErr(env);
        return nullptr;
    }

    auto result = std::make_shared<std::vector<RootInfo>>();
    if (!result) {
        NError(E_GETRESULT).ThrowErr(env);
        return nullptr;
    }

    auto cbExec = [result, fileAccessHelper]() -> NError {
        int ret = fileAccessHelper->GetRoots(*result);
        return NError(ret);
    };
    auto cbComplete = [fileAccessHelper, result](napi_env env, NError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        }

        NVal nVal;
        int ret = MakeGetRootsResult(env, fileAccessHelper, *result, nVal);
        if (ret != ERR_OK) {
            return { env, NError([ret]() -> std::tuple<uint32_t, std::string> {
                return { ret, "Make GetRoots Result fail" };
            }).GetNapiErr(env) };
        }

        return nVal;
    };

    const std::string procedureName = "getRoots";
    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::ZERO) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    }

    NVal cb(env, funcArg[NARG_POS::FIRST]);
    if (!cb.TypeIs(napi_function)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
}

napi_value NAPI_Access(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE, NARG_CNT::TWO)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    bool succ = false;
    std::unique_ptr<char[]> uri;
    std::tie(succ, uri, std::ignore) = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    if (!succ) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    FileAccessHelper *fileAccessHelper = GetFileAccessHelper(env, funcArg.GetThisVar());
    if (fileAccessHelper == nullptr) {
        return nullptr;
    }

    auto result = std::make_shared<bool>();
    string uriString(uri.get());
    auto cbExec = [uriString, result, fileAccessHelper]() -> NError {
        OHOS::Uri uri(uriString);
        bool isExist = false;
        int ret = fileAccessHelper->Access(uri, isExist);
        *result = isExist;
        return NError(ret);
    };
    auto cbComplete = [result](napi_env env, NError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        }
        return { NVal::CreateBool(env, *result) };
    };

    const std::string procedureName = "access";
    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::ONE) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    }
    NVal cb(env, funcArg[NARG_POS::SECOND]);
    if (!cb.TypeIs(napi_function)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
}

static int MakeFileInfoResult(napi_env &env,
                              std::shared_ptr<FileAccessHelper> helper,
                              FileInfo &fileinfo,
                              NVal &nVal)
{
    auto objFileInfo = NClass::InstantiateClass(env, NapiFileInfoExporter::className_, {});
    if (objFileInfo == nullptr) {
        HILOG_INFO("Cannot instantiate class NapiFileInfoExporter");
        return E_GETRESULT;
    }

    auto fileInfoEntity = NClass::GetEntityOf<FileInfoEntity>(env, objFileInfo);
    if (fileInfoEntity == nullptr) {
        HILOG_INFO("Cannot get the entity of fileInfoEntity");
        return E_GETRESULT;
    }
    fileInfoEntity->fileAccessHelper = helper;
    fileInfoEntity->fileInfo = std::move(fileinfo);
    nVal = { env, objFileInfo };

    return ERR_OK;
}

napi_value NAPI_GetFileInfoFromUri(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE, NARG_CNT::TWO)) {
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }
    bool succ = false;
    std::unique_ptr<char[]> uri;
    std::tie(succ, uri, std::ignore) = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    if (!succ) {
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }
    FileAccessHelper *helper = GetFileAccessHelper(env, funcArg.GetThisVar());
    std::shared_ptr<FileAccessHelper> fileAccessHelper = GetGlobalFileAccessHelper(helper);
    if (fileAccessHelper == nullptr) {
        return nullptr;
    }
    auto result = std::make_shared<FileInfo>();
    if (!result) {
        return NapiFileInfoExporter::ThrowError(env, E_GETRESULT);
    }
    string uriString(uri.get());
    auto cbExec = [uriString, result, fileAccessHelper]() -> NError {
        OHOS::Uri uri(uriString);
        int ret = fileAccessHelper->GetFileInfoFromUri(uri, *result);
        return NError(ret);
    };
    auto cbComplete = [fileAccessHelper, result](napi_env env, NError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        }
        NVal nVal;
        int ret = MakeFileInfoResult(env, fileAccessHelper, *result, nVal);
        if (ret != ERR_OK) {
            return { env, NError([ret]() -> std::tuple<uint32_t, std::string> {
                return { ret, "Make FileInfo Result fail" };
            }).GetNapiErr(env) };
        }
        return nVal;
    };
    const std::string procedureName = "getFileInfoFromUri";
    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::ONE) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    }
    NVal cb(env, funcArg[NARG_POS::SECOND]);
    if (!cb.TypeIs(napi_function)) {
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }
    return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
}

napi_value NAPI_GetFileInfoFromRelativePath(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE, NARG_CNT::TWO)) {
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }
    bool succ = false;
    std::unique_ptr<char[]> uri;
    std::tie(succ, uri, std::ignore) = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    if (!succ) {
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }
    FileAccessHelper *helper = GetFileAccessHelper(env, funcArg.GetThisVar());
    std::shared_ptr<FileAccessHelper> fileAccessHelper = GetGlobalFileAccessHelper(helper);
    if (fileAccessHelper == nullptr) {
        HILOG_ERROR("Global FileAcHelper does not exist");
        return nullptr;
    }
    auto result = std::make_shared<FileInfo>();
    if (!result) {
        return NapiFileInfoExporter::ThrowError(env, E_GETRESULT);
    }
    string uriString(uri.get());
    auto cbExec = [uriString, result, fileAccessHelper]() -> NError {
        string relativePath(uriString);
        int ret = fileAccessHelper->GetFileInfoFromRelativePath(relativePath, *result);
        return NError(ret);
    };
    auto cbComplete = [fileAccessHelper, result](napi_env env, NError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        }
        NVal nVal;
        int ret = MakeFileInfoResult(env, fileAccessHelper, *result, nVal);
        if (ret != ERR_OK) {
            return { env, NError([ret]() -> std::tuple<uint32_t, std::string> {
                return { ret, "Make FileInfo Result fail" };
            }).GetNapiErr(env) };
        }
        return nVal;
    };
    const std::string procedureName = "getFileInfoFromRelativePath";
    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::ONE) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    }
    NVal cb(env, funcArg[NARG_POS::SECOND]);
    if (!cb.TypeIs(napi_function)) {
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }
    return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
}

struct FileObserverCallbackWrapper {
    sptr<IFileAccessObserver> callback;
};

static bool parseRegisterObserverArgs(napi_env env, NFuncArg &funcArg, std::string &uri, bool &notifyForDescendants)
{
    bool succ = false;
    std::unique_ptr<char[]> uriPtr;

    std::tie(succ, uriPtr, std::ignore) = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    if (!succ) {
        NError(EINVAL).ThrowErr(env);
        return false;
    }

    uri = uriPtr.get();
    std::tie(succ, notifyForDescendants) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        NError(EINVAL).ThrowErr(env);
        return false;
    }

    napi_value napiCallback = funcArg[NARG_POS::THIRD];
    if (!NVal(env, napiCallback).TypeIs(napi_function)) {
        NError(EINVAL).ThrowErr(env);
        return false;
    }
    return succ;
}

static bool RegisterObserver(napi_env env,  NFuncArg &funcArg, sptr<IFileAccessObserver> &observer)
{
    std::string uriString;
    bool notifyForDescendants = false;
    if (!parseRegisterObserverArgs(env, funcArg, uriString, notifyForDescendants)) {
        NError(EINVAL).ThrowErr(env);
        HILOG_ERROR("parse Args error");
        return false;
    }

    OHOS::Uri uri(uriString);
    FileAccessHelper *fileAccessHelper = GetFileAccessHelper(env, funcArg.GetThisVar());
    if (fileAccessHelper == nullptr) {
        return false;
    }

    auto retCode = fileAccessHelper->RegisterNotify(uri, notifyForDescendants, observer);
    if (retCode != ERR_OK) {
        NError(retCode).ThrowErr(env);
        return false;
    }
    return true;
}

napi_value NAPI_RegisterObserver(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    napi_value napiCallback = funcArg[NARG_POS::THIRD];
    FileObserverCallbackWrapper* wrapper = nullptr;
    if (napi_unwrap(env, napiCallback, (void **)&wrapper) != napi_ok || wrapper == nullptr) {
        std::shared_ptr<NapiObserver> observer = std::make_shared<NapiObserver>(env, napiCallback);
        sptr<IFileAccessObserver> callback(new (std::nothrow) NapiObserverCallback(observer));
        if (callback == nullptr) {
            NError(EINVAL).ThrowErr(env);
            return nullptr;
        }
        if (!RegisterObserver(env, funcArg, callback)) {
            HILOG_ERROR("RegisterObserver failed");
            NError(EINVAL).ThrowErr(env);
            return nullptr;
        }

        auto finalize = [](napi_env env, void *data, void *hint) {
            FileObserverCallbackWrapper *observerWrapper = static_cast<FileObserverCallbackWrapper *>(data);
            if (observerWrapper != nullptr) {
                delete observerWrapper;
            }
        };
        std::unique_ptr<FileObserverCallbackWrapper> observerWrapper = std::make_unique<FileObserverCallbackWrapper>();
        observerWrapper->callback = callback;
        if (napi_wrap(env, napiCallback, observerWrapper.get(), finalize, nullptr, nullptr) != napi_ok) {
            NError(EINVAL).ThrowErr(env);
            HILOG_ERROR("napi_wrap error");
            return nullptr;
        }
        observerWrapper.release();
    } else {
        if (!RegisterObserver(env, funcArg, wrapper->callback)) {
            HILOG_ERROR("RegisterObserver failed");
            NError(EINVAL).ThrowErr(env);
            return nullptr;
        }
    }
    return NVal::CreateUndefined(env).val_;
}

napi_value NAPI_UnregisterObserver(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE, NARG_CNT::TWO)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    bool succ = false;
    std::unique_ptr<char[]> uriPtr;
    std::tie(succ, uriPtr, std::ignore) = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    if (!succ) {
        NError(EINVAL).ThrowErr(env);
        HILOG_ERROR("Get uri error");
        return nullptr;
    }

    FileAccessHelper *fileAccessHelper = GetFileAccessHelper(env, funcArg.GetThisVar());
    if (fileAccessHelper == nullptr) {
        return nullptr;
    }

    std::string uriString(uriPtr.get());
    OHOS::Uri uri(uriString);
    int retCode = EINVAL;
    if (funcArg.GetArgc() == NARG_CNT::ONE) {
        retCode = fileAccessHelper->UnregisterNotify(uri);
    } else {
        napi_value napiCallback = funcArg[NARG_POS::SECOND];
        if (!NVal(env, napiCallback).TypeIs(napi_function)) {
            NError(EINVAL).ThrowErr(env);
            return nullptr;
        }

        std::unique_ptr<FileObserverCallbackWrapper> observerWrapper;
        if (napi_unwrap(env, napiCallback, (void **)&(observerWrapper)) != napi_ok || observerWrapper == nullptr) {
            NError(EINVAL).ThrowErr(env);
            return nullptr;
        }

        auto wrapper = observerWrapper.release();
        retCode = fileAccessHelper->UnregisterNotify(uri, wrapper->callback);
    }

    if (retCode != ERR_OK) {
        NError(retCode).ThrowErr(env);
        return nullptr;
    }
    return NVal::CreateUndefined(env).val_;
}

static std::tuple<bool, std::string, std::string, bool> GetMoveItemArguments(napi_env env, NFuncArg &funcArg)
{
    bool retStatus = false;
    std::unique_ptr<char[]> srcPath;
    std::unique_ptr<char[]> destPath;
    std::tie(retStatus, srcPath, destPath) = GetReadArg(env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND]);
    if (!retStatus) {
        HILOG_ERROR("Get first or second argument error");
        return std::make_tuple(false, "", "", false);
    }
    std::string srcPathStr (srcPath.get());
    std::string destPathStr (destPath.get());

    bool force = false;
    if (funcArg.GetArgc() == NARG_CNT::THREE) {
        NVal thirdArg(env, funcArg[NARG_POS::THIRD]);
        if (thirdArg.TypeIs(napi_boolean)) {
            std::tie(retStatus, force) = NVal(env, funcArg[NARG_POS::THIRD]).ToBool();
            if (!retStatus) {
                HILOG_ERROR("Get third argument error");
                return std::make_tuple(false, "", "", false);
            }
        }
    }

    return std::make_tuple(true, srcPathStr, destPathStr, force);
}

napi_value NAPI_MoveItem(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        HILOG_ERROR("init args failed");
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }
    bool retStatus = false;
    std::string srcPathStr;
    std::string destPathStr;
    bool force = false;
    std::tie(retStatus, srcPathStr, destPathStr, force) = GetMoveItemArguments(env, funcArg);
    if (!retStatus) {
        HILOG_ERROR("parse args failed");
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }
    FileAccessHelper *fileAccessHelper = GetFileAccessHelper(env, funcArg.GetThisVar());
    if (fileAccessHelper == nullptr) {
        HILOG_ERROR("get fillAccessHelper failed");
        return nullptr;
    }
    auto result = std::make_shared<std::vector<Result>>();
    if (result == nullptr) {
        return NapiFileInfoExporter::ThrowError(env, E_GETRESULT);
    }
    int ret = ERR_OK;
    auto cbExec = [srcPathStr, destPathStr, force, result, &ret, fileAccessHelper]() -> NError {
        OHOS::Uri srcUri(srcPathStr);
        OHOS::Uri destUri(destPathStr);
        ret = fileAccessHelper->MoveItem(srcUri, destUri, *result, force);
        if ((ret == COPY_EXCEPTION) && !result->empty()) {
            return NError(result->at(0).errCode);
        }
        return NError();
    };
    auto cbComplete = [&ret, result](napi_env env, NError err) -> NVal {
        if (ret == COPY_EXCEPTION) {
            return { env, err.GetNapiErr(env) };
        }
        return { env, CreateObjectArray(env, *result) };
    };
    return AddNAsyncWork(env, "moveItem", funcArg, cbExec, cbComplete);
}

static std::tuple<bool, std::unique_ptr<char[]>, std::unique_ptr<char[]>, std::unique_ptr<char[]>> GetMoveFileArg(
    napi_env env, napi_value sourceFile, napi_value targetParent, napi_value fileName)
{
    bool ret = false;
    std::unique_ptr<char[]> sourceFileUri = nullptr;
    std::unique_ptr<char[]> targetParentUri = nullptr;
    std::unique_ptr<char[]> name = nullptr;
    std::tie(ret, sourceFileUri, std::ignore) = NVal(env, sourceFile).ToUTF8String();
    if (!ret) {
        NError(EINVAL).ThrowErr(env);
        return { false, std::move(sourceFileUri), std::move(targetParentUri), std::move(name) };
    }

    std::tie(ret, targetParentUri, std::ignore) = NVal(env, targetParent).ToUTF8String();
    if (!ret) {
        NError(EINVAL).ThrowErr(env);
        return { false, std::move(sourceFileUri), std::move(targetParentUri), std::move(name) };
    }

    std::tie(ret, name, std::ignore) = NVal(env, fileName).ToUTF8String();
    if (!ret) {
        NError(EINVAL).ThrowErr(env);
        return { false, std::move(sourceFileUri), std::move(targetParentUri), std::move(name) };
    }

    return { true, std::move(sourceFileUri), std::move(targetParentUri), std::move(name) };
}

napi_value NAPI_MoveFile(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::FOUR)) {
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }
    bool succ = false;
    std::unique_ptr<char[]> sourceFile;
    std::unique_ptr<char[]> targetParent;
    std::unique_ptr<char[]> fileName;
    std::tie(succ, sourceFile, targetParent, fileName) =
        GetMoveFileArg(env, funcArg[NARG_POS::FIRST], funcArg[NARG_POS::SECOND], funcArg[NARG_POS::THIRD]);
    if (!succ) {
        return nullptr;
    }
    FileAccessHelper *fileAccessHelper = GetFileAccessHelper(env, funcArg.GetThisVar());
    if (fileAccessHelper == nullptr) {
        return nullptr;
    }
    auto result = std::make_shared<string>();
    if (!result) {
        return NapiFileInfoExporter::ThrowError(env, E_GETRESULT);
    }
    string sourceFileString(sourceFile.get());
    string targetParentString(targetParent.get());
    string fileNameString(fileName.get());
    auto cbExec = [sourceFileString, targetParentString, fileNameString, result, fileAccessHelper]() -> NError {
        OHOS::Uri sourceUri(sourceFileString);
        OHOS::Uri targetParentUri(targetParentString);
        OHOS::Uri newFileUri("");
        std::string fileName(fileNameString);
        int ret = fileAccessHelper->MoveFile(sourceUri, targetParentUri, fileName, newFileUri);
        *result = newFileUri.ToString();
        return NError(ret);
    };
    auto cbComplete = [result](napi_env env, NError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        }
        return { NVal::CreateUTF8String(env, *result) };
    };
    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == NARG_CNT::THREE) {
        return NAsyncWorkPromise(env, thisVar).Schedule("moveFile", cbExec, cbComplete).val_;
    }
    NVal cb(env, funcArg[NARG_POS::FOURTH]);
    if (!cb.TypeIs(napi_function)) {
        return NapiFileInfoExporter::ThrowError(env, EINVAL);
    }
    return NAsyncWorkCallback(env, thisVar, cb).Schedule("moveFile", cbExec, cbComplete).val_;
}
} // namespace FileAccessFwk
} // namespace OHOS