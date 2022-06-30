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
#include <vector>

#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "napi_base_context.h"
#include "napi_common_fileaccess.h"
#include "n_func_arg.h"
#include "n_val.h"
#include "securec.h"
#include "uni_error.h"
#include "uri.h"

using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;
using namespace OHOS::DistributedFS;

namespace OHOS {
namespace FileAccessFwk {
namespace {
const std::string FILEACCESS_CLASS_NAME = "FileAccessHelper";
static napi_ref g_constructorRef = nullptr;
constexpr uint32_t INITIAL_REFCOUNT = 1;

std::string NapiValueToStringUtf8(napi_env env, napi_value value)
{
    std::string result = "";
    return UnwrapStringFromJS(env, value, result);
}

int NapiValueToInt32Utf8(napi_env env, napi_value value)
{
    int result = ERR_OK;
    return UnwrapInt32FromJS(env, value, result);
}
}

std::list<std::shared_ptr<FileAccessHelper>> g_fileAccessHelperList;

napi_value AcquireFileAccessHelperWrap(napi_env env, napi_callback_info info, FileAccessHelperCB *fileAccessHelperCB)
{
    if (fileAccessHelperCB == nullptr) {
        HILOG_ERROR("%{public}s,fileAccessHelperCB == nullptr", __func__);
        return nullptr;
    }

    size_t requireArgc = ARGS_THREE;
    size_t argc = ARGS_THREE;
    napi_value args[ARGS_THREE] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    if (argc > requireArgc) {
        HILOG_ERROR("%{public}s, Wrong argument count%{public}zu.", __func__, argc);
        return nullptr;
    }

    napi_value result = nullptr;
    napi_value cons = nullptr;
    if (napi_get_reference_value(env, g_constructorRef, &cons) != napi_ok) {
        HILOG_ERROR("%{public}s, g_constructorRef reference is fail", __func__);
        return nullptr;
    }
    NAPI_CALL(env, napi_new_instance(env, cons, ARGS_THREE, args, &result));

    if (!IsTypeForNapiValue(env, result, napi_object)) {
        HILOG_ERROR("%{public}s, IsTypeForNapiValue isn`t object", __func__);
        return nullptr;
    }

    if (IsTypeForNapiValue(env, result, napi_null)) {
        HILOG_ERROR("%{public}s, IsTypeForNapiValue is null", __func__);
        return nullptr;
    }

    if (IsTypeForNapiValue(env, result, napi_undefined)) {
        HILOG_ERROR("%{public}s, IsTypeForNapiValue is undefined", __func__);
        return nullptr;
    }

    delete fileAccessHelperCB;
    fileAccessHelperCB = nullptr;
    return result;
}

static napi_value FileAccessHelperConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_TWO;
    napi_value argv[ARGS_TWO] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NAPI_ASSERT(env, argc > 0, "Wrong number of arguments");
    AAFwk::Want want;
    OHOS::AppExecFwk::UnwrapWant(env, argv[PARAM1], want);
    std::shared_ptr<FileAccessHelper> fileAccessHelper = nullptr;
    bool isStageMode = false;
    napi_status status = AbilityRuntime::IsStageContext(env, argv[PARAM0], isStageMode);
    if (status != napi_ok || !isStageMode) {
        HILOG_INFO("No support FA Model");
        return nullptr;
    }

    auto context = OHOS::AbilityRuntime::GetStageModeContext(env, argv[PARAM0]);
    NAPI_ASSERT(env, context != nullptr, " FileAccessHelperConstructor: failed to get native context");
    fileAccessHelper = FileAccessHelper::Creator(context, want);

    NAPI_ASSERT(env, fileAccessHelper != nullptr, " FileAccessHelperConstructor: fileAccessHelper is nullptr");
    g_fileAccessHelperList.emplace_back(fileAccessHelper);

    napi_wrap(env, thisVar, fileAccessHelper.get(), [](napi_env env, void *data, void *hint) {
            FileAccessHelper *objectInfo = static_cast<FileAccessHelper *>(data);
            g_fileAccessHelperList.remove_if([objectInfo](const std::shared_ptr<FileAccessHelper> &fileAccessHelper) {
                    return objectInfo == fileAccessHelper.get();
                });
            if (objectInfo != nullptr) {
                objectInfo->Release();
                delete objectInfo;
            }
        }, nullptr, nullptr);
    return thisVar;
}

napi_value NAPI_AcquireFileAccessHelperCommon(napi_env env, napi_callback_info info, AbilityType abilityType)
{
    FileAccessHelperCB *fileAccessHelperCB = new (std::nothrow) FileAccessHelperCB;
    if (fileAccessHelperCB == nullptr) {
        HILOG_ERROR("%{public}s, FileAccessHelperCB == nullptr", __func__);
        return WrapVoidToJS(env);
    }

    fileAccessHelperCB->cbBase.cbInfo.env = env;
    fileAccessHelperCB->cbBase.errCode = NAPI_ERR_NO_ERROR;
    fileAccessHelperCB->cbBase.abilityType = abilityType;
    napi_value ret = AcquireFileAccessHelperWrap(env, info, fileAccessHelperCB);
    if (ret == nullptr) {
        HILOG_ERROR("%{public}s, ret == nullptr", __func__);
        if (fileAccessHelperCB != nullptr) {
            delete fileAccessHelperCB;
            fileAccessHelperCB = nullptr;
        }
        ret = WrapVoidToJS(env);
    }
    return ret;
}

napi_value NAPI_CreateFileAccessHelper(napi_env env, napi_callback_info info)
{
    return NAPI_AcquireFileAccessHelperCommon(env, info,  AbilityType::EXTENSION);
}

napi_value FileAccessHelperInit(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("openFile", NAPI_OpenFile),
        DECLARE_NAPI_FUNCTION("mkdir", NAPI_Mkdir),
        DECLARE_NAPI_FUNCTION("createFile", NAPI_CreateFile),
        DECLARE_NAPI_FUNCTION("delete", NAPI_Delete),
        DECLARE_NAPI_FUNCTION("move", NAPI_Move),
        DECLARE_NAPI_FUNCTION("rename", NAPI_Rename),
        DECLARE_NAPI_FUNCTION("listFile", NAPI_ListFile),
        DECLARE_NAPI_FUNCTION("getRoots", NAPI_GetRoots),
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
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(export_properties) / sizeof(export_properties[0]),
        export_properties));
    return exports;
}

static void OpenFilePromiseCompleteCB(napi_env env, napi_status status, void *data)
{
    FileAccessHelperOpenFileCB *openFileCB = static_cast<FileAccessHelperOpenFileCB *>(data);
    napi_value result = nullptr;
    napi_create_int32(env, openFileCB->result, &result);
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, openFileCB->cbBase.deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, openFileCB->cbBase.asyncWork));
    delete openFileCB;
    openFileCB = nullptr;
}

static void OpenFileAsyncCompleteCB(napi_env env, napi_status status, void *data)
{
    FileAccessHelperOpenFileCB *openFileCB = static_cast<FileAccessHelperOpenFileCB *>(data);
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_TWO] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, openFileCB->cbBase.cbInfo.callback, &callback));

    result[PARAM0] = GetCallbackErrorValue(env, openFileCB->execResult);
    napi_create_int32(env, openFileCB->result, &result[PARAM1]);
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_TWO, &result[PARAM0], &callResult));

    if (openFileCB->cbBase.cbInfo.callback != nullptr) {
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, openFileCB->cbBase.cbInfo.callback));
    }
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, openFileCB->cbBase.asyncWork));
    delete openFileCB;
    openFileCB = nullptr;
}

static void OpenFileExecuteCB(napi_env env, void *data)
{
    FileAccessHelperOpenFileCB *openFileCB = static_cast<FileAccessHelperOpenFileCB *>(data);
    openFileCB->execResult = ERR_ERROR;

    if (openFileCB->fileAccessHelper == nullptr) {
        HILOG_ERROR(" NAPI_OpenFile, fileAccessHelper uri is empty");
        return ;
    }

    if (openFileCB->uri.empty()) {
        HILOG_ERROR(" NAPI_OpenFile, fileAccessHelper uri is empty");
        return ;
    }
    OHOS::Uri uri(openFileCB->uri);
    openFileCB->result = openFileCB->fileAccessHelper->OpenFile(uri, openFileCB->flags);
    openFileCB->execResult = ERR_OK;
}

static napi_value OpenFilePromise(napi_env env, FileAccessHelperOpenFileCB *openFileCB)
{
    if (openFileCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    openFileCB->cbBase.deferred = deferred;

    NAPI_CALL(env,
        napi_create_async_work(env,
            nullptr,
            resourceName,
            OpenFileExecuteCB,
            OpenFilePromiseCompleteCB,
            (void *)openFileCB,
            &openFileCB->cbBase.asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, openFileCB->cbBase.asyncWork));
    return promise;
}

static napi_value OpenFileAsync(napi_env env,
                                napi_value *args,
                                const size_t argCallback,
                                FileAccessHelperOpenFileCB *openFileCB)
{
    if (args == nullptr || openFileCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }

    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valueType));
    if (valueType == napi_function) {
        NAPI_CALL(env, napi_create_reference(env,
                                             args[argCallback],
                                             INITIAL_REFCOUNT,
                                             &openFileCB->cbBase.cbInfo.callback));
    }

    NAPI_CALL(env,
        napi_create_async_work(env,
            nullptr,
            resourceName,
            OpenFileExecuteCB,
            OpenFileAsyncCompleteCB,
            (void *)openFileCB,
            &openFileCB->cbBase.asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, openFileCB->cbBase.asyncWork));
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

static napi_value OpenFileWrap(napi_env env, napi_callback_info info, FileAccessHelperOpenFileCB *openFileCB)
{
    size_t argcAsync = ARGS_THREE;
    const size_t argcPromise = ARGS_TWO;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, &thisVar, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        HILOG_ERROR("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[PARAM0], &valueType));
    if (valueType != napi_string) {
        UniError(EINVAL).ThrowErr(env, "Type of arguments unmatched");
        return nullptr;
    }
    openFileCB->uri = NapiValueToStringUtf8(env, args[PARAM0]);

    NAPI_CALL(env, napi_typeof(env, args[PARAM1], &valueType));
    if (valueType != napi_number) {
        UniError(EINVAL).ThrowErr(env, "Type of arguments unmatched");
        return nullptr;
    }
    openFileCB->flags = NapiValueToInt32Utf8(env, args[PARAM1]);

    FileAccessHelper *objectInfo = nullptr;
    napi_unwrap(env, thisVar, (void **)&objectInfo);
    openFileCB->fileAccessHelper = objectInfo;

    napi_value ret = nullptr;
    if (argcAsync > argcPromise) {
        ret = OpenFileAsync(env, args, ARGS_TWO, openFileCB);
    } else {
        ret = OpenFilePromise(env, openFileCB);
    }
    return ret;
}

napi_value NAPI_OpenFile(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::THREE)) {
        HILOG_ERROR("%{public}s, Number of arguments unmatched.", __func__);
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    if (funcArg.GetArgc() == NARG_CNT::THREE && !NVal(env, funcArg[NARG_POS::THIRD]).TypeIs(napi_function)) {
        UniError(EINVAL).ThrowErr(env, "Type of arguments unmatched");
        return nullptr;
    }

    FileAccessHelperOpenFileCB *openFileCB = new (std::nothrow) FileAccessHelperOpenFileCB;
    if (openFileCB == nullptr) {
        HILOG_ERROR("%{public}s, openFileCB == nullptr.", __func__);
        return WrapVoidToJS(env);
    }
    openFileCB->cbBase.cbInfo.env = env;
    openFileCB->cbBase.asyncWork = nullptr;
    openFileCB->cbBase.deferred = nullptr;
    openFileCB->cbBase.ability = nullptr;

    napi_value ret = OpenFileWrap(env, info, openFileCB);
    if (ret == nullptr) {
        HILOG_ERROR("%{public}s,ret == nullptr", __func__);
        if (openFileCB != nullptr) {
            delete openFileCB;
            openFileCB = nullptr;
        }
        ret = WrapVoidToJS(env);
    }
    return ret;
}

static void CreateFilePromiseCompleteCB(napi_env env, napi_status status, void *data)
{
    FileAccessHelperCreateFileCB *createFileCB = static_cast<FileAccessHelperCreateFileCB *>(data);
    napi_value result = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, createFileCB->result.c_str(), NAPI_AUTO_LENGTH, &result));
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, createFileCB->cbBase.deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, createFileCB->cbBase.asyncWork));
    delete createFileCB;
    createFileCB = nullptr;
}

static void CreateFileAsyncCompleteCB(napi_env env, napi_status status, void *data)
{
    FileAccessHelperCreateFileCB *createFileCB = static_cast<FileAccessHelperCreateFileCB *>(data);
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_TWO] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, createFileCB->cbBase.cbInfo.callback, &callback));

    result[PARAM0] = GetCallbackErrorValue(env, createFileCB->execResult);
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, createFileCB->result.c_str(), NAPI_AUTO_LENGTH, &result[PARAM1]));
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_TWO, &result[PARAM0], &callResult));

    if (createFileCB->cbBase.cbInfo.callback != nullptr) {
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, createFileCB->cbBase.cbInfo.callback));
    }
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, createFileCB->cbBase.asyncWork));
    delete createFileCB;
    createFileCB = nullptr;
}

static void CreateFileExecuteCB(napi_env env, void *data)
{
    FileAccessHelperCreateFileCB *createFileCB = static_cast<FileAccessHelperCreateFileCB *>(data);
    createFileCB->execResult = ERR_ERROR;
    if (createFileCB->fileAccessHelper == nullptr) {
        HILOG_ERROR(" NAPI_CreateFile, fileAccessHelper is nullptr");
        return ;
    }

    if (createFileCB->parentUri.empty()) {
        HILOG_ERROR(" NAPI_CreateFile, parentUri uri is empty");
        return ;
    }
    OHOS::Uri parentUri(createFileCB->parentUri);
    std::string newFile = "";
    OHOS::Uri newFileUri(newFile);
    int err = createFileCB->fileAccessHelper->CreateFile(parentUri, createFileCB->name, newFileUri);
    createFileCB->result = newFileUri.ToString();
    createFileCB->execResult = err;
}

static napi_value CreateFilePromise(napi_env env, FileAccessHelperCreateFileCB *createFileCB)
{
    if (createFileCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    createFileCB->cbBase.deferred = deferred;

    NAPI_CALL(env,
        napi_create_async_work(env,
            nullptr,
            resourceName,
            CreateFileExecuteCB,
            CreateFilePromiseCompleteCB,
            (void *)createFileCB,
            &createFileCB->cbBase.asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, createFileCB->cbBase.asyncWork));
    return promise;
}

static napi_value CreateFileAsync(napi_env env,
                                  napi_value *args,
                                  const size_t argCallback,
                                  FileAccessHelperCreateFileCB *createFileCB)
{
    if (args == nullptr || createFileCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }

    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valueType));
    if (valueType == napi_function) {
        NAPI_CALL(env, napi_create_reference(env,
                                             args[argCallback],
                                             INITIAL_REFCOUNT,
                                             &createFileCB->cbBase.cbInfo.callback));
    }

    NAPI_CALL(env,
        napi_create_async_work(env,
            nullptr,
            resourceName,
            CreateFileExecuteCB,
            CreateFileAsyncCompleteCB,
            (void *)createFileCB,
            &createFileCB->cbBase.asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, createFileCB->cbBase.asyncWork));
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

static napi_value CreateFileWrap(napi_env env, napi_callback_info info, FileAccessHelperCreateFileCB *createFileCB)
{
    size_t argcAsync = ARGS_THREE;
    const size_t argcPromise = ARGS_TWO;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, &thisVar, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        HILOG_ERROR("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[PARAM0], &valueType));
    if (valueType != napi_string) {
        UniError(EINVAL).ThrowErr(env, "Type of arguments unmatched");
        return nullptr;
    }
    createFileCB->parentUri = NapiValueToStringUtf8(env, args[PARAM0]);

    NAPI_CALL(env, napi_typeof(env, args[PARAM1], &valueType));
    if (valueType != napi_string) {
        UniError(EINVAL).ThrowErr(env, "Type of arguments unmatched");
        return nullptr;
    }
    createFileCB->name = NapiValueToStringUtf8(env, args[PARAM1]);

    FileAccessHelper *objectInfo = nullptr;
    napi_unwrap(env, thisVar, (void **)&objectInfo);
    createFileCB->fileAccessHelper = objectInfo;
    
    napi_value ret = nullptr;
    if (argcAsync > argcPromise) {
        ret = CreateFileAsync(env, args, ARGS_TWO, createFileCB);
    } else {
        ret = CreateFilePromise(env, createFileCB);
    }
    return ret;
}

napi_value NAPI_CreateFile(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::THREE)) {
        HILOG_ERROR("%{public}s, Number of arguments unmatched.", __func__);
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    if (funcArg.GetArgc() == NARG_CNT::THREE && !NVal(env, funcArg[NARG_POS::THIRD]).TypeIs(napi_function)) {
        UniError(EINVAL).ThrowErr(env, "Type of arguments unmatched");
        return nullptr;
    }

    FileAccessHelperCreateFileCB *createFileCB = new (std::nothrow) FileAccessHelperCreateFileCB;
    if (createFileCB == nullptr) {
        HILOG_ERROR("%{public}s, createFileCB == nullptr.", __func__);
        return WrapVoidToJS(env);
    }
    createFileCB->cbBase.cbInfo.env = env;
    createFileCB->cbBase.asyncWork = nullptr;
    createFileCB->cbBase.deferred = nullptr;
    createFileCB->cbBase.ability = nullptr;

    napi_value ret = CreateFileWrap(env, info, createFileCB);
    if (ret == nullptr) {
        HILOG_ERROR("%{public}s,ret == nullptr", __func__);
        if (createFileCB != nullptr) {
            delete createFileCB;
            createFileCB = nullptr;
        }
        ret = WrapVoidToJS(env);
    }
    return ret;
}

static void MkdirPromiseCompleteCB(napi_env env, napi_status status, void *data)
{
    FileAccessHelperMkdirCB *mkdirCB = static_cast<FileAccessHelperMkdirCB *>(data);
    napi_value result = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, mkdirCB->result.c_str(), NAPI_AUTO_LENGTH, &result));
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, mkdirCB->cbBase.deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, mkdirCB->cbBase.asyncWork));
    delete mkdirCB;
    mkdirCB = nullptr;
}

static void MkdirAsyncCompleteCB(napi_env env, napi_status status, void *data)
{
    FileAccessHelperMkdirCB *mkdirCB = static_cast<FileAccessHelperMkdirCB *>(data);
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_TWO] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, mkdirCB->cbBase.cbInfo.callback, &callback));

    result[PARAM0] = GetCallbackErrorValue(env, mkdirCB->execResult);
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, mkdirCB->result.c_str(), NAPI_AUTO_LENGTH, &result[PARAM1]));
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_TWO, &result[PARAM0], &callResult));

    if (mkdirCB->cbBase.cbInfo.callback != nullptr) {
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, mkdirCB->cbBase.cbInfo.callback));
    }
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, mkdirCB->cbBase.asyncWork));
    delete mkdirCB;
    mkdirCB = nullptr;
}

static void MkdirExecuteCB(napi_env env, void *data)
{
    FileAccessHelperMkdirCB *mkdirCB = static_cast<FileAccessHelperMkdirCB *>(data);
    mkdirCB->execResult = ERR_ERROR;
    if (mkdirCB->fileAccessHelper == nullptr) {
        HILOG_ERROR(" NAPI_mkdirCB, fileAccessHelper uri is empty");
        return ;
    }

    if (mkdirCB->parentUri.empty()) {
        HILOG_ERROR(" NAPI_mkdirCB, fileAccessHelper uri is empty");
        return ;
    }
    OHOS::Uri parentUri(mkdirCB->parentUri);
    std::string newDir = "";
    OHOS::Uri newDirUri(newDir);
    int err = mkdirCB->fileAccessHelper->Mkdir(parentUri, mkdirCB->name, newDirUri);
    mkdirCB->result = newDirUri.ToString();
    mkdirCB->execResult = err;
}

static napi_value MkdirPromise(napi_env env, FileAccessHelperMkdirCB *mkdirCB)
{
    if (mkdirCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    mkdirCB->cbBase.deferred = deferred;

    NAPI_CALL(env,
        napi_create_async_work(env,
            nullptr,
            resourceName,
            MkdirExecuteCB,
            MkdirPromiseCompleteCB,
            (void *)mkdirCB,
            &mkdirCB->cbBase.asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, mkdirCB->cbBase.asyncWork));
    return promise;
}

static napi_value MkdirAsync(napi_env env,
                             napi_value *args,
                             const size_t argCallback,
                             FileAccessHelperMkdirCB *mkdirCB)
{
    if (args == nullptr || mkdirCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valueType));
    if (valueType == napi_function) {
        NAPI_CALL(env, napi_create_reference(env,
                                             args[argCallback],
                                             INITIAL_REFCOUNT,
                                             &mkdirCB->cbBase.cbInfo.callback));
    }

    NAPI_CALL(env,
        napi_create_async_work(env,
            nullptr,
            resourceName,
            MkdirExecuteCB,
            MkdirAsyncCompleteCB,
            (void *)mkdirCB,
            &mkdirCB->cbBase.asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, mkdirCB->cbBase.asyncWork));
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

static napi_value MkdirWrap(napi_env env, napi_callback_info info, FileAccessHelperMkdirCB *mkdirCB)
{
    size_t argcAsync = ARGS_THREE;
    const size_t argcPromise = ARGS_TWO;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, &thisVar, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        HILOG_ERROR("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[PARAM0], &valueType));
    if (valueType != napi_string) {
        UniError(EINVAL).ThrowErr(env, "Type of arguments unmatched");
        return nullptr;
    }
    mkdirCB->parentUri = NapiValueToStringUtf8(env, args[PARAM0]);

    NAPI_CALL(env, napi_typeof(env, args[PARAM1], &valueType));
    if (valueType != napi_string) {
        UniError(EINVAL).ThrowErr(env, "Type of arguments unmatched");
        return nullptr;
    }
    mkdirCB->name = NapiValueToStringUtf8(env, args[PARAM1]);

    FileAccessHelper *objectInfo = nullptr;
    napi_unwrap(env, thisVar, (void **)&objectInfo);
    mkdirCB->fileAccessHelper = objectInfo;
    
    napi_value ret = nullptr;
    if (argcAsync > argcPromise) {
        ret = MkdirAsync(env, args, ARGS_TWO, mkdirCB);
    } else {
        ret = MkdirPromise(env, mkdirCB);
    }
    return ret;
}

napi_value NAPI_Mkdir(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::THREE)) {
        HILOG_ERROR("%{public}s, Number of arguments unmatched.", __func__);
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    if (funcArg.GetArgc() == NARG_CNT::THREE && !NVal(env, funcArg[NARG_POS::THIRD]).TypeIs(napi_function)) {
        UniError(EINVAL).ThrowErr(env, "Type of arguments unmatched");
        return nullptr;
    }

    FileAccessHelperMkdirCB *mkdirCB = new (std::nothrow) FileAccessHelperMkdirCB;
    if (mkdirCB == nullptr) {
        HILOG_ERROR("%{public}s, mkdirCB == nullptr.", __func__);
        return WrapVoidToJS(env);
    }
    mkdirCB->cbBase.cbInfo.env = env;
    mkdirCB->cbBase.asyncWork = nullptr;
    mkdirCB->cbBase.deferred = nullptr;
    mkdirCB->cbBase.ability = nullptr;

    napi_value ret = MkdirWrap(env, info, mkdirCB);
    if (ret == nullptr) {
        HILOG_ERROR("%{public}s,ret == nullptr", __func__);
        if (mkdirCB != nullptr) {
            delete mkdirCB;
            mkdirCB = nullptr;
        }
        ret = WrapVoidToJS(env);
    }
    return ret;
}

static void DeleteAsyncCompleteCB(napi_env env, napi_status status, void *data)
{
    FileAccessHelperDeleteCB *deleteCB = static_cast<FileAccessHelperDeleteCB *>(data);
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_TWO] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, deleteCB->cbBase.cbInfo.callback, &callback));

    result[PARAM0] = GetCallbackErrorValue(env, deleteCB->execResult);
    napi_create_int32(env, deleteCB->result, &result[PARAM1]);
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_TWO, &result[PARAM0], &callResult));

    if (deleteCB->cbBase.cbInfo.callback != nullptr) {
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, deleteCB->cbBase.cbInfo.callback));
    }
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, deleteCB->cbBase.asyncWork));
    delete deleteCB;
    deleteCB = nullptr;
}

static void DeletePromiseCompleteCB(napi_env env, napi_status status, void *data)
{
    FileAccessHelperDeleteCB *deleteCB = static_cast<FileAccessHelperDeleteCB *>(data);
    napi_value result = nullptr;
    napi_create_int32(env, deleteCB->result, &result);
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, deleteCB->cbBase.deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, deleteCB->cbBase.asyncWork));
    delete deleteCB;
    deleteCB = nullptr;
}

static void DeleteExecuteCB(napi_env env, void *data)
{
    FileAccessHelperDeleteCB *deleteCB = static_cast<FileAccessHelperDeleteCB *>(data);
    deleteCB->execResult = ERR_ERROR;
    if (deleteCB->fileAccessHelper == nullptr) {
        HILOG_ERROR(" NAPI_deleteCB, fileAccessHelper uri is empty");
        return ;
    }

    if (deleteCB->selectFileUri.empty()) {
        HILOG_ERROR(" NAPI_deleteCB, fileAccessHelper uri is empty");
        return ;
    }

    OHOS::Uri selectFileUri(deleteCB->selectFileUri);
    deleteCB->result = deleteCB->fileAccessHelper->Delete(selectFileUri);
    deleteCB->execResult = ERR_OK;
}

static napi_value DeletePromise(napi_env env, FileAccessHelperDeleteCB *deleteCB)
{
    if (deleteCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    deleteCB->cbBase.deferred = deferred;

    NAPI_CALL(env,
        napi_create_async_work(env,
            nullptr,
            resourceName,
            DeleteExecuteCB,
            DeletePromiseCompleteCB,
            (void *)deleteCB,
            &deleteCB->cbBase.asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, deleteCB->cbBase.asyncWork));
    return promise;
}

static napi_value DeleteAsync(napi_env env,
                              napi_value *args,
                              const size_t argCallback,
                              FileAccessHelperDeleteCB *deleteCB)
{
    if (args == nullptr || deleteCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valueType));
    if (valueType == napi_function) {
        NAPI_CALL(env, napi_create_reference(env,
                                             args[argCallback],
                                             INITIAL_REFCOUNT,
                                             &deleteCB->cbBase.cbInfo.callback));
    }

    NAPI_CALL(env,
        napi_create_async_work(env,
            nullptr,
            resourceName,
            DeleteExecuteCB,
            DeleteAsyncCompleteCB,
            (void *)deleteCB,
            &deleteCB->cbBase.asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, deleteCB->cbBase.asyncWork));
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

static napi_value DeleteWrap(napi_env env, napi_callback_info info, FileAccessHelperDeleteCB *deleteCB)
{
    size_t argcAsync = ARGS_TWO;
    const size_t argcPromise = ARGS_ONE;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, &thisVar, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        HILOG_ERROR("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[PARAM0], &valueType));
    if (valueType != napi_string) {
        UniError(EINVAL).ThrowErr(env, "Type of arguments unmatched");
        return nullptr;
    }
    deleteCB->selectFileUri = NapiValueToStringUtf8(env, args[PARAM0]);

    FileAccessHelper *objectInfo = nullptr;
    napi_unwrap(env, thisVar, (void **)&objectInfo);
    deleteCB->fileAccessHelper = objectInfo;
    
    napi_value ret = nullptr;
    if (argcAsync > argcPromise) {
        ret = DeleteAsync(env, args, ARGS_ONE, deleteCB);
    } else {
        ret = DeletePromise(env, deleteCB);
    }
    return ret;
}

napi_value NAPI_Delete(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE, NARG_CNT::TWO)) {
        HILOG_ERROR("%{public}s, Number of arguments unmatched.", __func__);
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    if (funcArg.GetArgc() == NARG_CNT::TWO && !NVal(env, funcArg[NARG_POS::SECOND]).TypeIs(napi_function)) {
        UniError(EINVAL).ThrowErr(env, "Type of arguments unmatched");
        return nullptr;
    }

    FileAccessHelperDeleteCB *deleteCB = new (std::nothrow) FileAccessHelperDeleteCB;
    if (deleteCB == nullptr) {
        HILOG_ERROR("%{public}s, deleteCB == nullptr.", __func__);
        return WrapVoidToJS(env);
    }
    deleteCB->cbBase.cbInfo.env = env;
    deleteCB->cbBase.asyncWork = nullptr;
    deleteCB->cbBase.deferred = nullptr;
    deleteCB->cbBase.ability = nullptr;

    napi_value ret = DeleteWrap(env, info, deleteCB);
    if (ret == nullptr) {
        HILOG_ERROR("%{public}s,ret == nullptr", __func__);
        if (deleteCB != nullptr) {
            delete deleteCB;
            deleteCB = nullptr;
        }
        ret = WrapVoidToJS(env);
    }
    return ret;
}

static void MovePromiseCompleteCB(napi_env env, napi_status status, void *data)
{
    FileAccessHelperMoveCB *moveCB = static_cast<FileAccessHelperMoveCB *>(data);
    napi_value result = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, moveCB->result.c_str(), NAPI_AUTO_LENGTH, &result));
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, moveCB->cbBase.deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, moveCB->cbBase.asyncWork));
    delete moveCB;
    moveCB = nullptr;
}

static void MoveAsyncCompleteCB(napi_env env, napi_status status, void *data)
{
    FileAccessHelperMoveCB *moveCB = static_cast<FileAccessHelperMoveCB *>(data);
    if (moveCB == nullptr) {
        return ;
    }
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_TWO] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, moveCB->cbBase.cbInfo.callback, &callback));

    result[PARAM0] = GetCallbackErrorValue(env, moveCB->execResult);
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, moveCB->result.c_str(), NAPI_AUTO_LENGTH, &result[PARAM1]));
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_TWO, &result[PARAM0], &callResult));

    if (moveCB->cbBase.cbInfo.callback != nullptr) {
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, moveCB->cbBase.cbInfo.callback));
    }
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, moveCB->cbBase.asyncWork));
    delete moveCB;
    moveCB = nullptr;
}

static void MoveExecuteCB(napi_env env, void *data)
{
    FileAccessHelperMoveCB *moveCB = static_cast<FileAccessHelperMoveCB *>(data);
    moveCB->execResult = ERR_ERROR;
    if (moveCB->fileAccessHelper == nullptr) {
        HILOG_ERROR("NAPI_move, fileAccessHelper is nullptr");
        return ;
    }

    if (moveCB->sourceFileUri.empty()) {
        HILOG_ERROR("NAPI_move, fileAccessHelper uri is empty");
        return ;
    }
    OHOS::Uri sourceFileUri(moveCB->sourceFileUri);
    OHOS::Uri targetParentUri(moveCB->targetParentUri);
    std::string newFile = "";
    OHOS::Uri newFileUri(newFile);
    int err = moveCB->fileAccessHelper->Move(sourceFileUri, targetParentUri, newFileUri);
    moveCB->result = newFileUri.ToString();
    moveCB->execResult = err;
}

static napi_value MovePromise(napi_env env, FileAccessHelperMoveCB *moveCB)
{
    if (moveCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    moveCB->cbBase.deferred = deferred;

    NAPI_CALL(env,
        napi_create_async_work(env,
            nullptr,
            resourceName,
            MoveExecuteCB,
            MovePromiseCompleteCB,
            (void *)moveCB,
            &moveCB->cbBase.asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, moveCB->cbBase.asyncWork));
    return promise;
}

static napi_value MoveAsync(napi_env env,
                            napi_value *args,
                            const size_t argCallback,
                            FileAccessHelperMoveCB *moveCB)
{
    if (args == nullptr || moveCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valueType));
    if (valueType == napi_function) {
        NAPI_CALL(env, napi_create_reference(env,
                                             args[argCallback],
                                             INITIAL_REFCOUNT,
                                             &moveCB->cbBase.cbInfo.callback));
    }

    NAPI_CALL(env,
        napi_create_async_work(env,
            nullptr,
            resourceName,
            MoveExecuteCB,
            MoveAsyncCompleteCB,
            (void *)moveCB,
            &moveCB->cbBase.asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, moveCB->cbBase.asyncWork));
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

static napi_value MoveWrap(napi_env env, napi_callback_info info, FileAccessHelperMoveCB *moveCB)
{
    size_t argcAsync = ARGS_THREE;
    const size_t argcPromise = ARGS_TWO;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, &thisVar, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        HILOG_ERROR("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[PARAM0], &valueType));
    if (valueType != napi_string) {
        UniError(EINVAL).ThrowErr(env, "Type of arguments unmatched");
        return nullptr;
    }
    moveCB->sourceFileUri = NapiValueToStringUtf8(env, args[PARAM0]);

    NAPI_CALL(env, napi_typeof(env, args[PARAM1], &valueType));
    if (valueType != napi_string) {
        UniError(EINVAL).ThrowErr(env, "Type of arguments unmatched");
        return nullptr;
    }
    moveCB->targetParentUri = NapiValueToStringUtf8(env, args[PARAM1]);

    FileAccessHelper *objectInfo = nullptr;
    napi_unwrap(env, thisVar, (void **)&objectInfo);
    moveCB->fileAccessHelper = objectInfo;
    
    napi_value ret = nullptr;
    if (argcAsync > argcPromise) {
        ret = MoveAsync(env, args, ARGS_TWO, moveCB);
    } else {
        ret = MovePromise(env, moveCB);
    }
    return ret;
}

napi_value NAPI_Move(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::THREE)) {
        HILOG_ERROR("%{public}s, Number of arguments unmatched.", __func__);
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    if (funcArg.GetArgc() == NARG_CNT::THREE && !NVal(env, funcArg[NARG_POS::THIRD]).TypeIs(napi_function)) {
        UniError(EINVAL).ThrowErr(env, "Type of arguments unmatched");
        return nullptr;
    }

    FileAccessHelperMoveCB *moveCB = new (std::nothrow) FileAccessHelperMoveCB;
    if (moveCB == nullptr) {
        HILOG_ERROR("%{public}s, moveCB == nullptr.", __func__);
        return WrapVoidToJS(env);
    }
    moveCB->cbBase.cbInfo.env = env;
    moveCB->cbBase.asyncWork = nullptr;
    moveCB->cbBase.deferred = nullptr;
    moveCB->cbBase.ability = nullptr;

    napi_value ret = MoveWrap(env, info, moveCB);
    if (ret == nullptr) {
        HILOG_ERROR("%{public}s,ret == nullptr", __func__);
        if (moveCB != nullptr) {
            delete moveCB;
            moveCB = nullptr;
        }
        ret = WrapVoidToJS(env);
    }
    return ret;
}

static void RenamePromiseCompleteCB(napi_env env, napi_status status, void *data)
{
    FileAccessHelperRenameCB *renameCB = static_cast<FileAccessHelperRenameCB *>(data);
    napi_value result = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, renameCB->result.c_str(), NAPI_AUTO_LENGTH, &result));
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, renameCB->cbBase.deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, renameCB->cbBase.asyncWork));
    delete renameCB;
    renameCB = nullptr;
}

static void RenameAsyncCompleteCB(napi_env env, napi_status status, void *data)
{
    FileAccessHelperRenameCB *renameCB = static_cast<FileAccessHelperRenameCB *>(data);
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_TWO] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, renameCB->cbBase.cbInfo.callback, &callback));

    result[PARAM0] = GetCallbackErrorValue(env, renameCB->execResult);
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, renameCB->result.c_str(), NAPI_AUTO_LENGTH, &result[PARAM1]));
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_TWO, &result[PARAM0], &callResult));

    if (renameCB->cbBase.cbInfo.callback != nullptr) {
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, renameCB->cbBase.cbInfo.callback));
    }
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, renameCB->cbBase.asyncWork));
    delete renameCB;
    renameCB = nullptr;
}

static void RenameExecuteCB(napi_env env, void *data)
{
    FileAccessHelperRenameCB *renameCB = static_cast<FileAccessHelperRenameCB *>(data);
    renameCB->execResult = ERR_ERROR;
    if (renameCB->fileAccessHelper == nullptr) {
        HILOG_ERROR("NAPI_Rename, fileAccessHelper == nullptr");
        return;
    }

    if (renameCB->sourceFileUri.empty()) {
        HILOG_ERROR("NAPI_Rename, fileAccessHelper sourceFileUri is empty");
        return;
    }
    OHOS::Uri sourceFileUri(renameCB->sourceFileUri);
    std::string newFile = "";
    OHOS::Uri newFileUri(newFile);
    int err = renameCB->fileAccessHelper->Rename(sourceFileUri, renameCB->displayName, newFileUri);
    renameCB->result = newFileUri.ToString();
    renameCB->execResult = err;
}

static napi_value RenamePromise(napi_env env, FileAccessHelperRenameCB *renameCB)
{
    if (renameCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    renameCB->cbBase.deferred = deferred;

    NAPI_CALL(env,
        napi_create_async_work(env,
                               nullptr,
                               resourceName,
                               RenameExecuteCB,
                               RenamePromiseCompleteCB,
                               (void *)renameCB,
                               &renameCB->cbBase.asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, renameCB->cbBase.asyncWork));
    return promise;
}

static napi_value RenameAsync(napi_env env,
                              napi_value *args,
                              const size_t argCallback,
                              FileAccessHelperRenameCB *renameCB)
{
    if (args == nullptr || renameCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valueType));
    if (valueType == napi_function) {
        NAPI_CALL(env, napi_create_reference(env,
                                             args[argCallback],
                                             INITIAL_REFCOUNT,
                                             &renameCB->cbBase.cbInfo.callback));
    }

    NAPI_CALL(env,
        napi_create_async_work(env,
                               nullptr,
                               resourceName,
                               RenameExecuteCB,
                               RenameAsyncCompleteCB,
                               (void *)renameCB,
                               &renameCB->cbBase.asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, renameCB->cbBase.asyncWork));
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

static napi_value RenameWrap(napi_env env, napi_callback_info info, FileAccessHelperRenameCB *renameCB)
{
    size_t argcAsync = ARGS_THREE;
    const size_t argcPromise = ARGS_TWO;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, &thisVar, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        HILOG_ERROR("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[PARAM0], &valueType));
    if (valueType != napi_string) {
        UniError(EINVAL).ThrowErr(env, "Type of arguments unmatched");
        return nullptr;
    }
    renameCB->sourceFileUri = NapiValueToStringUtf8(env, args[PARAM0]);

    NAPI_CALL(env, napi_typeof(env, args[PARAM1], &valueType));
    if (valueType != napi_string) {
        UniError(EINVAL).ThrowErr(env, "Type of arguments unmatched");
        return nullptr;
    }
    renameCB->displayName = NapiValueToStringUtf8(env, args[PARAM1]);

    FileAccessHelper *objectInfo = nullptr;
    napi_unwrap(env, thisVar, (void **)&objectInfo);
    renameCB->fileAccessHelper = objectInfo;
    
    napi_value ret = nullptr;
    if (argcAsync > argcPromise) {
        ret = RenameAsync(env, args, ARGS_TWO, renameCB);
    } else {
        ret = RenamePromise(env, renameCB);
    }
    return ret;
}

napi_value NAPI_Rename(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::THREE)) {
        HILOG_ERROR("Number of arguments unmatched.");
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    if (funcArg.GetArgc() == NARG_CNT::THREE && !NVal(env, funcArg[NARG_POS::THIRD]).TypeIs(napi_function)) {
        UniError(EINVAL).ThrowErr(env, "Type of arguments unmatched");
        return nullptr;
    }

    FileAccessHelperRenameCB *renameCB = new (std::nothrow) FileAccessHelperRenameCB;
    if (renameCB == nullptr) {
        HILOG_ERROR("%{public}s, renameCB == nullptr.", __func__);
        return WrapVoidToJS(env);
    }
    renameCB->cbBase.cbInfo.env = env;
    renameCB->cbBase.asyncWork = nullptr;
    renameCB->cbBase.deferred = nullptr;
    renameCB->cbBase.ability = nullptr;

    napi_value ret = RenameWrap(env, info, renameCB);
    if (ret == nullptr) {
        HILOG_ERROR("%{public}s,ret == nullptr", __func__);
        if (renameCB != nullptr) {
            delete renameCB;
            renameCB = nullptr;
        }
        ret = WrapVoidToJS(env);
    }
    return ret;
}

static void ListFilePromiseCompleteCB(napi_env env, napi_status status, void *data)
{
    FileAccessHelperListFileCB *listFileCB = static_cast<FileAccessHelperListFileCB *>(data);
    napi_value result = nullptr;
    result = WrapArrayFileInfoToJS(env, listFileCB->result);
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, listFileCB->cbBase.deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, listFileCB->cbBase.asyncWork));
    delete listFileCB;
    listFileCB = nullptr;
}

static void ListFileAsyncCompleteCB(napi_env env, napi_status status, void *data)
{
    FileAccessHelperListFileCB *listFileCB = static_cast<FileAccessHelperListFileCB *>(data);
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_TWO] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, listFileCB->cbBase.cbInfo.callback, &callback));

    result[PARAM0] = GetCallbackErrorValue(env, listFileCB->execResult);
    result[PARAM1] = WrapArrayFileInfoToJS(env, listFileCB->result);
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_TWO, &result[PARAM0], &callResult));

    if (listFileCB->cbBase.cbInfo.callback != nullptr) {
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, listFileCB->cbBase.cbInfo.callback));
    }
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, listFileCB->cbBase.asyncWork));
    delete listFileCB;
    listFileCB = nullptr;
}

static void ListFileExecuteCB(napi_env env, void *data)
{
    FileAccessHelperListFileCB *listFileCB = static_cast<FileAccessHelperListFileCB *>(data);
    listFileCB->execResult = ERR_ERROR;
    if (listFileCB->fileAccessHelper == nullptr) {
        HILOG_ERROR(" NAPI_ListFile, fileAccessHelper == nullptr");
        return;
    }

    if (listFileCB->sourceFileUri.empty()) {
        HILOG_ERROR(" NAPI_ListFile, fileAccessHelper sourceFileUri is empty");
        return;
    }
    OHOS::Uri sourceFileUri(listFileCB->sourceFileUri);
    listFileCB->result = listFileCB->fileAccessHelper->ListFile(sourceFileUri);
    listFileCB->execResult = ERR_OK;
}

static napi_value ListFilePromise(napi_env env, FileAccessHelperListFileCB *listFileCB)
{
    if (listFileCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    listFileCB->cbBase.deferred = deferred;

    NAPI_CALL(env,
        napi_create_async_work(env,
                               nullptr,
                               resourceName,
                               ListFileExecuteCB,
                               ListFilePromiseCompleteCB,
                               (void *)listFileCB,
                               &listFileCB->cbBase.asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, listFileCB->cbBase.asyncWork));
    return promise;
}

static napi_value ListFileAsync(napi_env env,
                                napi_value *args,
                                const size_t argCallback,
                                FileAccessHelperListFileCB *listFileCB)
{
    if (args == nullptr || listFileCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valueType));
    if (valueType == napi_function) {
        NAPI_CALL(env, napi_create_reference(env,
                                             args[argCallback],
                                             INITIAL_REFCOUNT,
                                             &listFileCB->cbBase.cbInfo.callback));
    }

    NAPI_CALL(env,
        napi_create_async_work(env,
                               nullptr,
                               resourceName,
                               ListFileExecuteCB,
                               ListFileAsyncCompleteCB,
                               (void *)listFileCB,
                               &listFileCB->cbBase.asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, listFileCB->cbBase.asyncWork));
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

static napi_value ListFileWrap(napi_env env, napi_callback_info info, FileAccessHelperListFileCB *listFileCB)
{
    size_t argcAsync = ARGS_TWO;
    const size_t argcPromise = ARGS_ONE;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, &thisVar, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        HILOG_ERROR("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[PARAM0], &valueType));
    if (valueType != napi_string) {
        UniError(EINVAL).ThrowErr(env, "Type of arguments unmatched");
        return nullptr;
    }
    listFileCB->sourceFileUri = NapiValueToStringUtf8(env, args[PARAM0]);

    FileAccessHelper *objectInfo = nullptr;
    napi_unwrap(env, thisVar, (void **)&objectInfo);
    listFileCB->fileAccessHelper = objectInfo;
    
    napi_value ret = nullptr;
    if (argcAsync > argcPromise) {
        ret = ListFileAsync(env, args, ARGS_ONE, listFileCB);
    } else {
        ret = ListFilePromise(env, listFileCB);
    }
    return ret;
}

napi_value NAPI_ListFile(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE, NARG_CNT::TWO)) {
        HILOG_ERROR("%{public}s, Number of arguments unmatched.", __func__);
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    if (funcArg.GetArgc() == NARG_CNT::TWO && !NVal(env, funcArg[NARG_POS::SECOND]).TypeIs(napi_function)) {
        UniError(EINVAL).ThrowErr(env, "Type of arguments unmatched");
        return nullptr;
    }
    
    FileAccessHelperListFileCB *listFileCB = new (std::nothrow) FileAccessHelperListFileCB;
    if (listFileCB == nullptr) {
        HILOG_ERROR("%{public}s, listFileCB == nullptr.", __func__);
        return WrapVoidToJS(env);
    }
    listFileCB->cbBase.cbInfo.env = env;
    listFileCB->cbBase.asyncWork = nullptr;
    listFileCB->cbBase.deferred = nullptr;
    listFileCB->cbBase.ability = nullptr;

    napi_value ret = ListFileWrap(env, info, listFileCB);
    if (ret == nullptr) {
        HILOG_ERROR("%{public}s,ret == nullptr", __func__);
        if (listFileCB != nullptr) {
            delete listFileCB;
            listFileCB = nullptr;
        }
        ret = WrapVoidToJS(env);
    }
    return ret;
}

static void GetRootsPromiseCompleteCB(napi_env env, napi_status status, void *data)
{
    FileAccessHelperGetRootsCB *getRootsCB = static_cast<FileAccessHelperGetRootsCB *>(data);
    napi_value result = nullptr;
    result = WrapArrayDeviceInfoToJS(env, getRootsCB->result);
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, getRootsCB->cbBase.deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, getRootsCB->cbBase.asyncWork));
    delete getRootsCB;
    getRootsCB = nullptr;
}

static void GetRootsAsyncCompleteCB(napi_env env, napi_status status, void *data)
{
    FileAccessHelperGetRootsCB *getRootsCB = static_cast<FileAccessHelperGetRootsCB *>(data);
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_TWO] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, getRootsCB->cbBase.cbInfo.callback, &callback));

    result[PARAM0] = GetCallbackErrorValue(env, getRootsCB->execResult);
    result[PARAM1] = WrapArrayDeviceInfoToJS(env, getRootsCB->result);
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_TWO, &result[PARAM0], &callResult));

    if (getRootsCB->cbBase.cbInfo.callback != nullptr) {
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, getRootsCB->cbBase.cbInfo.callback));
    }
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, getRootsCB->cbBase.asyncWork));
    delete getRootsCB;
    getRootsCB = nullptr;
}

static void GetRootsExecuteCB(napi_env env, void *data)
{
    FileAccessHelperGetRootsCB *getRootsCB = static_cast<FileAccessHelperGetRootsCB *>(data);
    getRootsCB->execResult = ERR_ERROR;
    if (getRootsCB->fileAccessHelper == nullptr) {
        HILOG_ERROR(" NAPI_GetRoots, fileAccessHelper == nullptr");
        return;
    }
    getRootsCB->result = getRootsCB->fileAccessHelper->GetRoots();
    getRootsCB->execResult = ERR_OK;
}

static napi_value GetRootsPromise(napi_env env, FileAccessHelperGetRootsCB *getRootsCB)
{
    if (getRootsCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    getRootsCB->cbBase.deferred = deferred;

    NAPI_CALL(env,
        napi_create_async_work(env,
                               nullptr,
                               resourceName,
                               GetRootsExecuteCB,
                               GetRootsPromiseCompleteCB,
                               (void *)getRootsCB,
                               &getRootsCB->cbBase.asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, getRootsCB->cbBase.asyncWork));
    return promise;
}

static napi_value GetRootsAsync(napi_env env,
                                napi_value *args,
                                const size_t argCallback,
                                FileAccessHelperGetRootsCB *getRootsCB)
{
    if (args == nullptr || getRootsCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valueType));
    if (valueType == napi_function) {
        NAPI_CALL(env, napi_create_reference(env,
                                             args[argCallback],
                                             INITIAL_REFCOUNT,
                                             &getRootsCB->cbBase.cbInfo.callback));
    }

    NAPI_CALL(env,
        napi_create_async_work(env,
                               nullptr,
                               resourceName,
                               GetRootsExecuteCB,
                               GetRootsAsyncCompleteCB,
                               (void *)getRootsCB,
                               &getRootsCB->cbBase.asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, getRootsCB->cbBase.asyncWork));
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

static napi_value GetRootsWrap(napi_env env, napi_callback_info info, FileAccessHelperGetRootsCB *getRootsCB)
{
    size_t argcAsync = ARGS_ONE;
    const size_t argcPromise = ARGS_ZERO;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, &thisVar, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        HILOG_ERROR("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    FileAccessHelper *objectInfo = nullptr;
    napi_unwrap(env, thisVar, (void **)&objectInfo);
    getRootsCB->fileAccessHelper = objectInfo;
    
    napi_value ret = nullptr;
    if (argcAsync > argcPromise) {
        ret = GetRootsAsync(env, args, ARGS_ZERO, getRootsCB);
    } else {
        ret = GetRootsPromise(env, getRootsCB);
    }
    return ret;
}

napi_value NAPI_GetRoots(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO, NARG_CNT::ONE)) {
        HILOG_ERROR("%{public}s, Number of arguments unmatched.", __func__);
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    if (funcArg.GetArgc() == NARG_CNT::ONE && !NVal(env, funcArg[NARG_POS::FIRST]).TypeIs(napi_function)) {
        UniError(EINVAL).ThrowErr(env, "Type of arguments unmatched");
        return nullptr;
    }

    FileAccessHelperGetRootsCB *getRootsCB = new (std::nothrow) FileAccessHelperGetRootsCB;
    if (getRootsCB == nullptr) {
        HILOG_ERROR("%{public}s, getRootsCB == nullptr.", __func__);
        return WrapVoidToJS(env);
    }
    getRootsCB->cbBase.cbInfo.env = env;
    getRootsCB->cbBase.asyncWork = nullptr;
    getRootsCB->cbBase.deferred = nullptr;
    getRootsCB->cbBase.ability = nullptr;

    napi_value ret = GetRootsWrap(env, info, getRootsCB);
    if (ret == nullptr) {
        HILOG_ERROR("%{public}s,ret == nullptr", __func__);
        if (getRootsCB != nullptr) {
            delete getRootsCB;
            getRootsCB = nullptr;
        }
        ret = WrapVoidToJS(env);
    }
    return ret;
}
}  // namespace AppExecFwk
}  // namespace OHOS