/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "file_access_framework_errno.h"
#include "file_access_helper.h"
#include "file_info_entity.h"
#include "filemgmt_libn.h"
#include "hilog_wrapper.h"
#include "ifile_access_notify.h"
#include "napi_base_context.h"
#include "napi_common_fileaccess.h"
#include "napi_file_info_exporter.h"
#include "napi_notify_callback.h"
#include "napi_root_iterator_exporter.h"
#include "root_iterator_entity.h"
#include "securec.h"
#include "uri.h"

using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;
using namespace OHOS::FileManagement::LibN;

namespace OHOS {
namespace FileAccessFwk {
namespace {
    const std::string FILEACCESS_CLASS_NAME = "FileAccessHelper";
    static napi_ref g_constructorRef = nullptr;
    constexpr uint32_t INITIAL_REFCOUNT = 1;
}

std::list<std::shared_ptr<FileAccessHelper>> g_fileAccessHelperList;

static napi_value FileAccessHelperConstructor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE, NARG_CNT::TWO)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    napi_value thisVar = funcArg.GetThisVar();
    std::pair<std::shared_ptr<FileAccessHelper>, int> createResult{nullptr, ERR_OK};
    bool isStageMode = false;
    napi_status status = AbilityRuntime::IsStageContext(env, funcArg.GetArg(PARAM0), isStageMode);
    if (status != napi_ok || !isStageMode) {
        HILOG_INFO("No support FA Model");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    auto context = OHOS::AbilityRuntime::GetStageModeContext(env, funcArg.GetArg(PARAM0));
    if (context == nullptr) {
        HILOG_ERROR("FileAccessHelperConstructor: failed to get native context");
        NError(E_GETRESULT).ThrowErr(env);
        return nullptr;
    }

    if (funcArg.GetArgc() == NARG_CNT::ONE) {
        createResult = FileAccessHelper::Creator(context);
    } else if (funcArg.GetArgc() == NARG_CNT::TWO) {
        std::vector<AAFwk::Want> wants;
        bool suss = UnwrapArrayWantFromJS(env, funcArg.GetArg(PARAM1), wants);
        if (!suss) {
            HILOG_ERROR("UnwrapArrayWantFromJS failed to get native wants");
            NError(E_GETRESULT).ThrowErr(env);
            return nullptr;
        }
        createResult = FileAccessHelper::Creator(context, wants);
    }
    if (createResult.first == nullptr || createResult.second != ERR_OK) {
        HILOG_ERROR("FileAccessHelperConstructor: Creator failed ret=%{public}d", createResult.second);
        NError(createResult.second).ThrowErr(env);
        return nullptr;
    }
    g_fileAccessHelperList.emplace_back(createResult.first);

    auto finalize = [](napi_env env, void *data, void *hint) {
        FileAccessHelper *objectInfo = static_cast<FileAccessHelper *>(data);
        if (objectInfo != nullptr) {
            objectInfo->Release();
            g_fileAccessHelperList.remove_if([objectInfo](const std::shared_ptr<FileAccessHelper> &fileAccessHelper) {
                    return objectInfo == fileAccessHelper.get();
                });
            objectInfo = nullptr;
        }
    };
    if (napi_wrap(env, thisVar, createResult.first.get(), finalize, nullptr, nullptr) != napi_ok) {
        finalize(env, createResult.first.get(), nullptr);
        NError(E_GETRESULT).ThrowErr(env);
        return nullptr;
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

        if (argc > requireArgc || napi_get_reference_value(env, g_constructorRef, &cons) != napi_ok) {
            HILOG_ERROR("Wrong argument count%{public}zu. or g_constructorRef reference is fail", argc);
            return nullptr;
        }

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

        if (argc > requireArgc || napi_get_reference_value(env, g_constructorRef, &cons) != napi_ok) {
            HILOG_ERROR("Wrong argument count%{public}zu. or g_constructorRef reference is fail", argc);
            return nullptr;
        }

        if (napi_new_instance(env, cons, ARGS_TWO, args, &result) != napi_ok) {
            return nullptr;
        }
    }

    if (!IsTypeForNapiValue(env, result, napi_object)) {
        HILOG_ERROR("IsTypeForNapiValue isn`t object");
        return nullptr;
    }

    FileAccessHelper *fileAccessHelper = nullptr;
    if (napi_unwrap(env, result, (void **)&fileAccessHelper) != napi_ok) {
        HILOG_ERROR("Faild to get fileAccessHelper");
        return nullptr;
    }

    if (fileAccessHelper == nullptr) {
        HILOG_ERROR("fileAccessHelper is nullptr");
        return nullptr;
    }

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
    if (result == nullptr) {
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
        DECLARE_NAPI_FUNCTION("rename", NAPI_Rename),
        DECLARE_NAPI_FUNCTION("getRoots", NAPI_GetRoots),
        DECLARE_NAPI_FUNCTION("access", NAPI_Access),
        DECLARE_NAPI_FUNCTION("uriToFileInfo", NAPI_UriToFileInfo),
        DECLARE_NAPI_FUNCTION("on", NAPI_On),
        DECLARE_NAPI_FUNCTION("off", NAPI_Off)
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
    g_fileAccessHelperList.clear();
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

void InitOpenFlags(napi_env env, napi_value exports)
{
    char propertyName[] = "OPENFLAGS";
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("READ", NVal::CreateInt32(env, READ).val_),
        DECLARE_NAPI_STATIC_PROPERTY("WRITE", NVal::CreateInt32(env, WRITE).val_),
        DECLARE_NAPI_STATIC_PROPERTY("WRITE_READ", NVal::CreateInt32(env, WRITE_READ).val_)
    };
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    napi_define_properties(env, obj, sizeof(desc) / sizeof(desc[0]), desc);
    napi_set_named_property(env, exports, propertyName, obj);
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

    int flags;
    std::tie(succ, flags) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    FileAccessHelper *fileAccessHelper = GetFileAccessHelper(env, funcArg.GetThisVar());
    if (fileAccessHelper == nullptr) {
        return nullptr;
    }

    auto result = std::make_shared<int>();
    if (result == nullptr) {
        NError(E_GETRESULT).ThrowErr(env);
        return nullptr;
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
        NError(EINVAL).ThrowErr(env);
        return nullptr;
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
    if (result == nullptr) {
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
    if (result == nullptr) {
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
    if (result == nullptr) {
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
        NError(EINVAL).ThrowErr(env);
        return nullptr;
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
    if (result == nullptr) {
        NError(E_GETRESULT).ThrowErr(env);
        return nullptr;
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
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
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
    if (result == nullptr) {
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

static int MakeGetRootsResult(napi_env &env, FileAccessHelper *helper, std::vector<RootInfo> &rootInfoVec, NVal &nVal)
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

    FileAccessHelper *fileAccessHelper = GetFileAccessHelper(env, funcArg.GetThisVar());
    if (fileAccessHelper == nullptr) {
        NError(E_GETRESULT).ThrowErr(env);
        return nullptr;
    }

    auto result = std::make_shared<std::vector<RootInfo>>();
    if (result == nullptr) {
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

static int MakeFileInfoResult(napi_env &env, FileAccessHelper *helper, FileInfo &fileinfo, NVal &nVal)
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

napi_value NAPI_UriToFileInfo(napi_env env, napi_callback_info info)
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

    auto result = std::make_shared<FileInfo>();
    if (result == nullptr) {
        NError(E_GETRESULT).ThrowErr(env);
        return nullptr;
    }

    string uriString(uri.get());
    auto cbExec = [uriString, result, fileAccessHelper]() -> NError {
        OHOS::Uri uri(uriString);
        int ret = fileAccessHelper->UriToFileInfo(uri, *result);
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

    const std::string procedureName = "uriToFileInfo";
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

napi_value NAPI_On(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        NError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    FileAccessHelper *fileAccessHelper = GetFileAccessHelper(env, funcArg.GetThisVar());
    if (fileAccessHelper == nullptr) {
        NError(EINVAL).ThrowErr(env, "Get FileAccessHelper fail");
        return nullptr;
    }

    napi_value napiCallback = funcArg[NARG_POS::FIRST];
    if (!NVal(env, napiCallback).TypeIs(napi_function)) {
        NError(EINVAL).ThrowErr(env, "Argument must be function");
        return nullptr;
    }

    std::shared_ptr<INotifyCallback> callback = std::make_shared<NapiNotifyCallback>(env, napiCallback);
    if (callback == nullptr) {
        NError(EINVAL).ThrowErr(env, "new NapiNotifyCallback fail.");
        return nullptr;
    }

    auto retCode = fileAccessHelper->On(callback);
    if (retCode != ERR_OK) {
        NError(retCode).ThrowErr(env, "FileAccessHelper::On fail.");
        return nullptr;
    }
    return NVal::CreateUndefined(env).val_;
}

napi_value NAPI_Off(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        NError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    FileAccessHelper *fileAccessHelper = GetFileAccessHelper(env, funcArg.GetThisVar());
    if (fileAccessHelper == nullptr) {
        NError(EINVAL).ThrowErr(env, "Get FileAccessHelper fail");
        return nullptr;
    }

    auto retCode = fileAccessHelper->Off();
    if (retCode != ERR_OK) {
        NError(retCode).ThrowErr(env, "FileAccessHelper::Off fail.");
        return nullptr;
    }
    return NVal::CreateUndefined(env).val_;
}
} // namespace FileAccessFwk
} // namespace OHOS