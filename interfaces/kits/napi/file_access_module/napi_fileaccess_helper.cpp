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
static napi_ref g_constructorRef = nullptr;

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
    NAPI_CALL(env, napi_create_reference(env, cons, 1, &g_constructorRef));
    NAPI_CALL(env, napi_set_named_property(env, exports, FILEACCESS_CLASS_NAME.c_str(), cons));

    napi_property_descriptor export_properties[] = {
        DECLARE_NAPI_FUNCTION("createFileAccessHelper", NAPI_CreateFileAccessHelper),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(export_properties) / sizeof(export_properties[0]),
        export_properties));
    return exports;
}

napi_value FileAccessHelperConstructor(napi_env env, napi_callback_info info)
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
        HILOG_INFO(" FA Model");
        return nullptr;
    }

    auto context = OHOS::AbilityRuntime::GetStageModeContext(env, argv[PARAM0]);
    NAPI_ASSERT(env, context != nullptr, " FileAccessHelperConstructor: failed to get native context");
    HILOG_INFO(" Stage Model");
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

napi_value OpenFileWrap(napi_env env, napi_callback_info info, FileAccessHelperOpenFileCB *openFileCB)
{
    size_t argcAsync = ARGS_THREE;
    const size_t argcPromise = ARGS_TWO;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = 0;
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

    if (argcAsync > argcPromise) {
        ret = OpenFileAsync(env, args, ARGS_TWO, openFileCB);
    } else {
        ret = OpenFilePromise(env, openFileCB);
    }
    return ret;
}

napi_value OpenFileAsync(napi_env env,
                         napi_value *args,
                         const size_t argCallback,
                         FileAccessHelperOpenFileCB *openFileCB)
{
    if (args == nullptr || openFileCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valueType));
    if (valueType == napi_function) {
        NAPI_CALL(env, napi_create_reference(env, args[argCallback], 1, &openFileCB->cbBase.cbInfo.callback));
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
    napi_value result = 0;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

napi_value OpenFilePromise(napi_env env, FileAccessHelperOpenFileCB *openFileCB)
{
    if (openFileCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = 0;
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

void OpenFileExecuteCB(napi_env env, void *data)
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

void OpenFileAsyncCompleteCB(napi_env env, napi_status status, void *data)
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

void OpenFilePromiseCompleteCB(napi_env env, napi_status status, void *data)
{
    FileAccessHelperOpenFileCB *openFileCB = static_cast<FileAccessHelperOpenFileCB *>(data);
    napi_value result = nullptr;
    napi_create_int32(env, openFileCB->result, &result);
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, openFileCB->cbBase.deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, openFileCB->cbBase.asyncWork));
    delete openFileCB;
    openFileCB = nullptr;
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

napi_value CreateFileWrap(napi_env env, napi_callback_info info, FileAccessHelperCreateFileCB *createFileCB)
{
    size_t argcAsync = ARGS_THREE;
    const size_t argcPromise = ARGS_TWO;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = 0;
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

    if (argcAsync > argcPromise) {
        ret = CreateFileAsync(env, args, ARGS_TWO, createFileCB);
    } else {
        ret = CreateFilePromise(env, createFileCB);
    }
    return ret;
}

napi_value CreateFileAsync(napi_env env,
                           napi_value *args,
                           const size_t argCallback,
                           FileAccessHelperCreateFileCB *createFileCB)
{
    if (args == nullptr || createFileCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valueType));
    if (valueType == napi_function) {
        NAPI_CALL(env, napi_create_reference(env, args[argCallback], 1, &createFileCB->cbBase.cbInfo.callback));
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
    napi_value result = 0;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

napi_value CreateFilePromise(napi_env env, FileAccessHelperCreateFileCB *createFileCB)
{
    if (createFileCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = 0;
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

void CreateFileExecuteCB(napi_env env, void *data)
{
    FileAccessHelperCreateFileCB *createFileCB = static_cast<FileAccessHelperCreateFileCB *>(data);
    createFileCB->execResult = ERR_ERROR;
    if (createFileCB->fileAccessHelper == nullptr) {
        HILOG_ERROR(" NAPI_OpenFile, fileAccessHelper uri is empty");
        return ;
    }
    if (createFileCB->parentUri.empty()) {
        HILOG_ERROR(" NAPI_OpenFile, fileAccessHelper uri is empty");
        return ;
    }
    OHOS::Uri parentUri(createFileCB->parentUri);
    std::string newFile = "";
    OHOS::Uri newFileUri(newFile);
    int err = createFileCB->fileAccessHelper->CreateFile(parentUri, createFileCB->name, newFileUri);
    createFileCB->result = newFileUri.ToString();
    createFileCB->execResult = err;
}

void CreateFileAsyncCompleteCB(napi_env env, napi_status status, void *data)
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

void CreateFilePromiseCompleteCB(napi_env env, napi_status status, void *data)
{
    FileAccessHelperCreateFileCB *createFileCB = static_cast<FileAccessHelperCreateFileCB *>(data);
    napi_value result = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, createFileCB->result.c_str(), NAPI_AUTO_LENGTH, &result));
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, createFileCB->cbBase.deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, createFileCB->cbBase.asyncWork));
    delete createFileCB;
    createFileCB = nullptr;
}



}  // namespace AppExecFwk
}  // namespace OHOS