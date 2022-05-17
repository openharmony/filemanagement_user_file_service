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

#include "uri.h"

#include "hilog_wrapper.h"
#include "napi_base_context.h"
#include "securec.h"

using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string FILEACCESS_CLASS_NAME = "FileAccessHelper";
constexpr int NO_ERROR = 0;
constexpr int INVALID_PARAMETER = -1;

std::string NapiValueToStringUtf8(napi_env env, napi_value value)
{
    HILOG_INFO("tag dsa %{public}s,called", __func__);
    std::string result = "";
    return UnwrapStringFromJS(env, value, result);
}

int NapiValueToInt32Utf8(napi_env env, napi_value value)
{
    HILOG_INFO("tag dsa %{public}s,called", __func__);
    int result = 0;
    return UnwrapInt32FromJS(env, value, result);
}
}

std::list<std::shared_ptr<OHOS::AppExecFwk::FileAccessHelper>> g_fileAccessHelperList;
static napi_ref g_constructorRef = nullptr;

napi_value AcquireFileAccessHelperWrap(napi_env env, napi_callback_info info, FileAccessHelperCB *fileAccessHelperCB)
{
    HILOG_INFO("tag dsa %{public}s,called", __func__);
    if (fileAccessHelperCB == nullptr) {
        HILOG_ERROR("tag dsa %{public}s,fileAccessHelperCB == nullptr", __func__);
        return nullptr;
    }

    size_t requireArgc = ARGS_THREE;
    size_t argc = ARGS_THREE;
    napi_value args[ARGS_THREE] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    if (argc > requireArgc) {
        HILOG_ERROR("tag dsa %{public}s, Wrong argument count %{public}zu.", __func__, argc);
        return nullptr;
    }

    napi_value result = nullptr;
    napi_value cons = nullptr;
    if (napi_get_reference_value(env, g_constructorRef, &cons) != napi_ok) {
        return nullptr;
    }
    NAPI_CALL(env, napi_new_instance(env, cons, ARGS_THREE, args, &result));

    if (!IsTypeForNapiValue(env, result, napi_object)) {
        HILOG_ERROR("tag dsa %{public}s, IsTypeForNapiValue isn`t object", __func__);
        return nullptr;
    }

    if (IsTypeForNapiValue(env, result, napi_null)) {
        HILOG_ERROR("tag dsa %{public}s, IsTypeForNapiValue is null", __func__);
        return nullptr;
    }

    if (IsTypeForNapiValue(env, result, napi_undefined)) {
        HILOG_ERROR("tag dsa %{public}s, IsTypeForNapiValue is undefined", __func__);
        return nullptr;
    }

    delete fileAccessHelperCB;
    fileAccessHelperCB = nullptr;
    HILOG_INFO("tag dsa %{public}s,end", __func__);
    return result;
}

napi_value NAPI_AcquireFileAccessHelperCommon(napi_env env, napi_callback_info info, AbilityType abilityType)
{
    HILOG_INFO("tag dsa %{public}s,called", __func__);
    FileAccessHelperCB *fileAccessHelperCB = new (std::nothrow) FileAccessHelperCB;
    if (fileAccessHelperCB == nullptr) {
        HILOG_ERROR("tag dsa %{public}s, FileAccessHelperCB == nullptr", __func__);
        return WrapVoidToJS(env);
    }

    fileAccessHelperCB->cbBase.cbInfo.env = env;
    fileAccessHelperCB->cbBase.errCode = NAPI_ERR_NO_ERROR;
    fileAccessHelperCB->cbBase.abilityType = abilityType;
    napi_value ret = AcquireFileAccessHelperWrap(env, info, fileAccessHelperCB);
    if (ret == nullptr) {
        HILOG_ERROR("tag dsa %{public}s, ret == nullptr", __func__);
        if (fileAccessHelperCB != nullptr) {
            delete fileAccessHelperCB;
            fileAccessHelperCB = nullptr;
        }
        ret = WrapVoidToJS(env);
    }
    HILOG_INFO("tag dsa %{public}s,end", __func__);
    return ret;
}

napi_value NAPI_CreateFileAccessHelper(napi_env env, napi_callback_info info)
{
    HILOG_INFO("tag dsa %{public}s, called", __func__);
    return NAPI_AcquireFileAccessHelperCommon(env, info,  AbilityType::EXTENSION);
}

napi_value FileAccessHelperInit(napi_env env, napi_value exports)
{
    HILOG_INFO("tag dsa %{public}s,called start ", __func__);
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("openFile", NAPI_OpenFile),
        DECLARE_NAPI_FUNCTION("mkdir", NAPI_Mkdir),
        DECLARE_NAPI_FUNCTION("createFile", NAPI_CreateFile),
        DECLARE_NAPI_FUNCTION("delete", NAPI_Delete),
        DECLARE_NAPI_FUNCTION("move", NAPI_Move),
        DECLARE_NAPI_FUNCTION("rename", NAPI_Rename),
        DECLARE_NAPI_FUNCTION("closeFile", NAPI_CloseFile),
        DECLARE_NAPI_FUNCTION("release", NAPI_Release),
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
    HILOG_INFO("tag dsa %{public}s, called", __func__);
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
        HILOG_INFO("tag dsa FA Model");
        return nullptr;
    } else {
        auto context = OHOS::AbilityRuntime::GetStageModeContext(env, argv[PARAM0]);
        NAPI_ASSERT(env, context != nullptr, "FileAccessHelperConstructor: failed to get native context");
        HILOG_INFO("tag dsa Stage Model");
        fileAccessHelper = FileAccessHelper::Creator(context, want);
    }
    NAPI_ASSERT(env, fileAccessHelper != nullptr, "FileAccessHelperConstructor: fileAccessHelper is nullptr");
    g_fileAccessHelperList.emplace_back(fileAccessHelper);
    napi_wrap(env, thisVar, fileAccessHelper.get(), [](napi_env env, void *data, void *hint) {
            FileAccessHelper *objectInfo = static_cast<FileAccessHelper *>(data);
            g_fileAccessHelperList.remove_if([objectInfo](const std::shared_ptr<FileAccessHelper> &fileAccessHelper) {
                    return objectInfo == fileAccessHelper.get();
                });
        }, nullptr, nullptr);
    HILOG_INFO("tag dsa %{public}s,called end", __func__);
    return thisVar;
}

napi_value NAPI_OpenFile(napi_env env, napi_callback_info info)
{
    HILOG_INFO("tag dsa %{public}s,called", __func__);
    FileAccessHelperOpenFileCB *openFileCB = new (std::nothrow) FileAccessHelperOpenFileCB;
    if (openFileCB == nullptr) {
        HILOG_ERROR("tag dsa %{public}s, openFileCB == nullptr.", __func__);
        return WrapVoidToJS(env);
    }
    openFileCB->cbBase.cbInfo.env = env;
    openFileCB->cbBase.asyncWork = nullptr;
    openFileCB->cbBase.deferred = nullptr;
    openFileCB->cbBase.ability = nullptr;

    napi_value ret = OpenFileWrap(env, info, openFileCB);
    if (ret == nullptr) {
        HILOG_ERROR("tag dsa %{public}s,ret == nullptr", __func__);
        if (openFileCB != nullptr) {
            delete openFileCB;
            openFileCB = nullptr;
        }
        ret = WrapVoidToJS(env);
    }
    HILOG_INFO("tag dsa %{public}s,end", __func__);
    return ret;
}

napi_value OpenFileWrap(napi_env env, napi_callback_info info, FileAccessHelperOpenFileCB *openFileCB)
{
    HILOG_INFO("tag dsa %{public}s,called", __func__);
    size_t argcAsync = ARGS_THREE;
    const size_t argcPromise = ARGS_TWO;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = 0;
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, &thisVar, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        HILOG_ERROR("tag dsa %{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[PARAM0], &valuetype));
    if (valuetype == napi_string) {
        openFileCB->uri = NapiValueToStringUtf8(env, args[PARAM0]);
        HILOG_INFO("tag dsa %{public}s,uri=%{public}s", __func__, openFileCB->uri.c_str());
    }

    NAPI_CALL(env, napi_typeof(env, args[PARAM1], &valuetype));
    if (valuetype == napi_string) {
        openFileCB->mode = NapiValueToStringUtf8(env, args[PARAM1]);
        HILOG_INFO("tag dsa %{public}s,mode=%{public}s", __func__, openFileCB->mode.c_str());
    }

    FileAccessHelper *objectInfo = nullptr;
    napi_unwrap(env, thisVar, (void **)&objectInfo);
    HILOG_INFO("tag dsa %{public}s,FileAccessHelper objectInfo", __func__);
    openFileCB->fileAccessHelper = objectInfo;

    if (argcAsync > argcPromise) {
        ret = OpenFileAsync(env, args, ARGS_TWO, openFileCB);
    } else {
        ret = OpenFilePromise(env, openFileCB);
    }
    HILOG_INFO("tag dsa %{public}s,end", __func__);
    return ret;
}

napi_value OpenFileAsync(napi_env env, napi_value *args, const size_t argCallback, FileAccessHelperOpenFileCB *openFileCB)
{
    HILOG_INFO("tag dsa %{public}s, asyncCallback.", __func__);
    if (args == nullptr || openFileCB == nullptr) {
        HILOG_ERROR("tag dsa %{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valuetype));
    if (valuetype == napi_function) {
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
    HILOG_INFO("tag dsa %{public}s, asyncCallback end.", __func__);
    return result;
}

napi_value OpenFilePromise(napi_env env, FileAccessHelperOpenFileCB *openFileCB)
{
    HILOG_INFO("tag dsa %{public}s, promise.", __func__);
    if (openFileCB == nullptr) {
        HILOG_ERROR("tag dsa %{public}s, param == nullptr.", __func__);
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
    HILOG_INFO("tag dsa %{public}s, promise end.", __func__);
    return promise;
}

void OpenFileExecuteCB(napi_env env, void *data)
{
    HILOG_INFO("tag dsa NAPI_OpenFile, worker pool thread execute.");
    FileAccessHelperOpenFileCB *OpenFileCB = static_cast<FileAccessHelperOpenFileCB *>(data);
    if (OpenFileCB->fileAccessHelper != nullptr) {
        OpenFileCB->execResult = INVALID_PARAMETER;
        if (!OpenFileCB->uri.empty()) {
            OHOS::Uri uri(OpenFileCB->uri);
            OpenFileCB->result = OpenFileCB->fileAccessHelper->OpenFile(uri, OpenFileCB->mode);
            OpenFileCB->execResult = NO_ERROR;
        } else {
            HILOG_ERROR("tag dsa NAPI_OpenFile, fileAccessHelper uri is empty");
        }
    } else {
        HILOG_ERROR("tag dsa NAPI_OpenFile, fileAccessHelper == nullptr");
    }
    HILOG_INFO("tag dsa NAPI_OpenFile, worker pool thread execute end.");
}

void OpenFileAsyncCompleteCB(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("tag dsa NAPI_OpenFile, main event thread complete.");
    FileAccessHelperOpenFileCB *OpenFileCB = static_cast<FileAccessHelperOpenFileCB *>(data);
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_TWO] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, OpenFileCB->cbBase.cbInfo.callback, &callback));

    result[PARAM0] = GetCallbackErrorValue(env, OpenFileCB->execResult);
    napi_create_int32(env, OpenFileCB->result, &result[PARAM1]);
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_TWO, &result[PARAM0], &callResult));

    if (OpenFileCB->cbBase.cbInfo.callback != nullptr) {
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, OpenFileCB->cbBase.cbInfo.callback));
    }
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, OpenFileCB->cbBase.asyncWork));
    delete OpenFileCB;
    OpenFileCB = nullptr;
    HILOG_INFO("tag dsa NAPI_OpenFile, main event thread complete end.");
}

void OpenFilePromiseCompleteCB(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("tag dsa NAPI_OpenFileCB,  main event thread complete.");
    FileAccessHelperOpenFileCB *OpenFileCB = static_cast<FileAccessHelperOpenFileCB *>(data);
    napi_value result = nullptr;
    napi_create_int32(env, OpenFileCB->result, &result);
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, OpenFileCB->cbBase.deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, OpenFileCB->cbBase.asyncWork));
    delete OpenFileCB;
    OpenFileCB = nullptr;
    HILOG_INFO("tag dsa NAPI_OpenFileCB,  main event thread complete end.");
}

napi_value NAPI_CreateFile(napi_env env, napi_callback_info info)
{
    HILOG_INFO("tag dsa %{public}s,called", __func__);
    FileAccessHelperCreateFileCB *createFileCB = new (std::nothrow) FileAccessHelperCreateFileCB;
    if (createFileCB == nullptr) {
        HILOG_ERROR("tag dsa %{public}s, createFileCB == nullptr.", __func__);
        return WrapVoidToJS(env);
    }
    createFileCB->cbBase.cbInfo.env = env;
    createFileCB->cbBase.asyncWork = nullptr;
    createFileCB->cbBase.deferred = nullptr;
    createFileCB->cbBase.ability = nullptr;

    napi_value ret = CreateFileWrap(env, info, createFileCB);
    if (ret == nullptr) {
        HILOG_ERROR("tag dsa %{public}s,ret == nullptr", __func__);
        if (createFileCB != nullptr) {
            delete createFileCB;
            createFileCB = nullptr;
        }
        ret = WrapVoidToJS(env);
    }
    HILOG_INFO("tag dsa %{public}s,end", __func__);
    return ret;
}

napi_value CreateFileWrap(napi_env env, napi_callback_info info, FileAccessHelperCreateFileCB *createFileCB)
{
    HILOG_INFO("tag dsa %{public}s,called", __func__);
    size_t argcAsync = ARGS_THREE;
    const size_t argcPromise = ARGS_TWO;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = 0;
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, &thisVar, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        HILOG_ERROR("tag dsa %{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[PARAM0], &valuetype));
    if (valuetype == napi_string) {
        createFileCB->parentUri = NapiValueToStringUtf8(env, args[PARAM0]);
        HILOG_INFO("tag dsa %{public}s,parentUri=%{public}s", __func__, createFileCB->parentUri.c_str());
    }

    NAPI_CALL(env, napi_typeof(env, args[PARAM1], &valuetype));
    if (valuetype == napi_string) {
        createFileCB->name = NapiValueToStringUtf8(env, args[PARAM1]);
        HILOG_INFO("tag dsa %{public}s,name=%{public}s", __func__, createFileCB->name.c_str());
    }

    FileAccessHelper *objectInfo = nullptr;
    napi_unwrap(env, thisVar, (void **)&objectInfo);
    HILOG_INFO("tag dsa %{public}s,FileAccessHelper objectInfo", __func__);
    createFileCB->fileAccessHelper = objectInfo;

    if (argcAsync > argcPromise) {
        ret = CreateFileAsync(env, args, ARGS_TWO, createFileCB);
    } else {
        ret = CreateFilePromise(env, createFileCB);
    }
    HILOG_INFO("tag dsa %{public}s,end", __func__);
    return ret;
}

napi_value CreateFileAsync(napi_env env, napi_value *args, const size_t argCallback, FileAccessHelperCreateFileCB *createFileCB)
{
    HILOG_INFO("tag dsa %{public}s, asyncCallback.", __func__);
    if (args == nullptr || createFileCB == nullptr) {
        HILOG_ERROR("tag dsa %{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valuetype));
    if (valuetype == napi_function) {
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
    HILOG_INFO("tag dsa %{public}s, asyncCallback end.", __func__);
    return result;
}

napi_value CreateFilePromise(napi_env env, FileAccessHelperCreateFileCB *createFileCB)
{
    HILOG_INFO("tag dsa %{public}s, promise.", __func__);
    if (createFileCB == nullptr) {
        HILOG_ERROR("tag dsa %{public}s, param == nullptr.", __func__);
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
    HILOG_INFO("tag dsa %{public}s, promise end.", __func__);
    return promise;
}

void CreateFileExecuteCB(napi_env env, void *data)
{
    HILOG_INFO("tag dsa NAPI_CreateFile, worker pool thread execute.");
    FileAccessHelperCreateFileCB *CreateFileCB = static_cast<FileAccessHelperCreateFileCB *>(data);
    if (CreateFileCB->fileAccessHelper != nullptr) {
        CreateFileCB->execResult = INVALID_PARAMETER;
        if (!CreateFileCB->parentUri.empty()) {
            OHOS::Uri parentUri(CreateFileCB->parentUri);
            std::string newFile = "";
            OHOS::Uri newFileUri(newFile);
            int err = CreateFileCB->fileAccessHelper->CreateFile(parentUri, CreateFileCB->name, newFileUri);
            CreateFileCB->result = newFileUri.ToString();
            CreateFileCB->execResult = err;
        } else {
            HILOG_ERROR("tag dsa NAPI_CreateFile, fileAccessHelper uri is empty");
        }
    } else {
        HILOG_ERROR("tag dsa NAPI_CreateFile, fileAccessHelper == nullptr");
    }
    HILOG_INFO("tag dsa NAPI_CreateFile, worker pool thread execute end.");
}

void CreateFileAsyncCompleteCB(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("tag dsa NAPI_CreateFile, main event thread complete.");
    FileAccessHelperCreateFileCB *CreateFileCB = static_cast<FileAccessHelperCreateFileCB *>(data);
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_TWO] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, CreateFileCB->cbBase.cbInfo.callback, &callback));

    result[PARAM0] = GetCallbackErrorValue(env, CreateFileCB->execResult);
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, CreateFileCB->result.c_str(), NAPI_AUTO_LENGTH, &result[PARAM1]));
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_TWO, &result[PARAM0], &callResult));

    if (CreateFileCB->cbBase.cbInfo.callback != nullptr) {
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, CreateFileCB->cbBase.cbInfo.callback));
    }
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, CreateFileCB->cbBase.asyncWork));
    delete CreateFileCB;
    CreateFileCB = nullptr;
    HILOG_INFO("tag dsa NAPI_CreateFile, main event thread complete end.");
}

void CreateFilePromiseCompleteCB(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("tag dsa NAPI_CreateFile,  main event thread complete.");
    FileAccessHelperCreateFileCB *CreateFileCB = static_cast<FileAccessHelperCreateFileCB *>(data);
    napi_value result = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, CreateFileCB->result.c_str(), NAPI_AUTO_LENGTH, &result));
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, CreateFileCB->cbBase.deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, CreateFileCB->cbBase.asyncWork));
    delete CreateFileCB;
    CreateFileCB = nullptr;
    HILOG_INFO("tag dsa NAPI_CreateFile,  main event thread complete end.");
}

napi_value NAPI_Mkdir(napi_env env, napi_callback_info info)
{
    HILOG_INFO("tag dsa%{public}s,called", __func__);
    FileAccessHelperMkdirCB *mkdirCB = new (std::nothrow) FileAccessHelperMkdirCB;
    if (mkdirCB == nullptr) {
        HILOG_ERROR("tag dsa%{public}s, mkdirCB == nullptr.", __func__);
        return WrapVoidToJS(env);
    }
    mkdirCB->cbBase.cbInfo.env = env;
    mkdirCB->cbBase.asyncWork = nullptr;
    mkdirCB->cbBase.deferred = nullptr;
    mkdirCB->cbBase.ability = nullptr;

    napi_value ret = MkdirWrap(env, info, mkdirCB);
    if (ret == nullptr) {
        HILOG_ERROR("tag dsa%{public}s,ret == nullptr", __func__);
        if (mkdirCB != nullptr) {
            delete mkdirCB;
            mkdirCB = nullptr;
        }
        ret = WrapVoidToJS(env);
    }
    HILOG_INFO("tag dsa%{public}s,end", __func__);
    return ret;
}

napi_value MkdirWrap(napi_env env, napi_callback_info info, FileAccessHelperMkdirCB *mkdirCB)
{
    HILOG_INFO("tag dsa%{public}s,called", __func__);
    size_t argcAsync = ARGS_THREE;
    const size_t argcPromise = ARGS_TWO;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = 0;
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, &thisVar, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        HILOG_ERROR("tag dsa%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[PARAM0], &valuetype));
    if (valuetype == napi_string) {
        mkdirCB->parentUri = NapiValueToStringUtf8(env, args[PARAM0]);
        HILOG_INFO("tag dsa%{public}s,parentUri=%{public}s", __func__, mkdirCB->parentUri.c_str());
    }

    NAPI_CALL(env, napi_typeof(env, args[PARAM1], &valuetype));
    if (valuetype == napi_string) {
        mkdirCB->name = NapiValueToStringUtf8(env, args[PARAM1]);
        HILOG_INFO("tag dsa%{public}s,name=%{public}s", __func__, mkdirCB->name.c_str());
    }

    FileAccessHelper *objectInfo = nullptr;
    napi_unwrap(env, thisVar, (void **)&objectInfo);
    HILOG_INFO("tag dsa%{public}s,FileAccessHelper objectInfo", __func__);
    mkdirCB->fileAccessHelper = objectInfo;

    if (argcAsync > argcPromise) {
        ret = MkdirAsync(env, args, ARGS_TWO, mkdirCB);
    } else {
        ret = MkdirPromise(env, mkdirCB);
    }
    HILOG_INFO("tag dsa%{public}s,end", __func__);
    return ret;
}

napi_value MkdirAsync(napi_env env, napi_value *args, const size_t argCallback, FileAccessHelperMkdirCB *mkdirCB)
{
    HILOG_INFO("tag dsa%{public}s, asyncCallback.", __func__);
    if (args == nullptr || mkdirCB == nullptr) {
        HILOG_ERROR("tag dsa%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valuetype));
    if (valuetype == napi_function) {
        NAPI_CALL(env, napi_create_reference(env, args[argCallback], 1, &mkdirCB->cbBase.cbInfo.callback));
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
    napi_value result = 0;
    NAPI_CALL(env, napi_get_null(env, &result));
    HILOG_INFO("tag dsa%{public}s, asyncCallback end.", __func__);
    return result;
}

napi_value MkdirPromise(napi_env env, FileAccessHelperMkdirCB *mkdirCB)
{
    HILOG_INFO("tag dsa%{public}s, promise.", __func__);
    if (mkdirCB == nullptr) {
        HILOG_ERROR("tag dsa%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = 0;
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
    HILOG_INFO("tag dsa%{public}s, promise end.", __func__);
    return promise;
}

void MkdirExecuteCB(napi_env env, void *data)
{
    HILOG_INFO("tag dsaNAPI_Mkdir, worker pool thread execute.");
    FileAccessHelperMkdirCB *MkdirCB = static_cast<FileAccessHelperMkdirCB *>(data);
    if (MkdirCB->fileAccessHelper != nullptr) {
        MkdirCB->execResult = INVALID_PARAMETER;
        if (!MkdirCB->parentUri.empty()) {
            OHOS::Uri parentUri(MkdirCB->parentUri);
            std::string newDir = "";
            OHOS::Uri newDirUri(newDir);
            int err = MkdirCB->fileAccessHelper->Mkdir(parentUri, MkdirCB->name, newDirUri);
            MkdirCB->result = newDirUri.ToString();
            MkdirCB->execResult = err;
        } else {
            HILOG_ERROR("tag dsaNAPI_Mkdir, fileAccessHelper parentUri is empty");
        }
    } else {
        HILOG_ERROR("tag dsaNAPI_Mkdir, fileAccessHelper == nullptr");
    }
    HILOG_INFO("tag dsaNAPI_Mkdir, worker pool thread execute end.");
}

void MkdirAsyncCompleteCB(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("tag dsaNAPI_Mkdir, main event thread complete.");
    FileAccessHelperMkdirCB *MkdirCB = static_cast<FileAccessHelperMkdirCB *>(data);
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_TWO] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, MkdirCB->cbBase.cbInfo.callback, &callback));

    result[PARAM0] = GetCallbackErrorValue(env, MkdirCB->execResult);
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, MkdirCB->result.c_str(), NAPI_AUTO_LENGTH, &result[PARAM1]));
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_TWO, &result[PARAM0], &callResult));

    if (MkdirCB->cbBase.cbInfo.callback != nullptr) {
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, MkdirCB->cbBase.cbInfo.callback));
    }
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, MkdirCB->cbBase.asyncWork));
    delete MkdirCB;
    MkdirCB = nullptr;
    HILOG_INFO("tag dsaNAPI_Mkdir, main event thread complete end.");
}

void MkdirPromiseCompleteCB(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("tag dsaNAPI_Mkdir,  main event thread complete.");
    FileAccessHelperMkdirCB *MkdirCB = static_cast<FileAccessHelperMkdirCB *>(data);
    napi_value result = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, MkdirCB->result.c_str(), NAPI_AUTO_LENGTH, &result));
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, MkdirCB->cbBase.deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, MkdirCB->cbBase.asyncWork));
    delete MkdirCB;
    MkdirCB = nullptr;
    HILOG_INFO("tag dsaNAPI_Mkdir,  main event thread complete end.");
}

napi_value NAPI_Delete(napi_env env, napi_callback_info info)
{
    HILOG_INFO("tag dsa %{public}s,called", __func__);
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
        HILOG_ERROR("tag dsa %{public}s,ret == nullptr", __func__);
        if (deleteCB != nullptr) {
            delete deleteCB;
            deleteCB = nullptr;
        }
        ret = WrapVoidToJS(env);
    }
    HILOG_INFO("tag dsa %{public}s,end", __func__);
    return ret;
}

napi_value DeleteWrap(napi_env env, napi_callback_info info, FileAccessHelperDeleteCB *deleteCB)
{
    HILOG_INFO("tag dsa %{public}s,called", __func__);
    size_t argcAsync = ARGS_TWO;
    const size_t argcPromise = ARGS_ONE;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = 0;
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, &thisVar, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        HILOG_ERROR("tag dsa %{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[PARAM0], &valuetype));
    if (valuetype == napi_string) {
        deleteCB->selectFileUri = NapiValueToStringUtf8(env, args[PARAM0]);
        HILOG_INFO("tag dsa %{public}s,selectFileUri=%{public}s", __func__, deleteCB->selectFileUri.c_str());
    }

    FileAccessHelper *objectInfo = nullptr;
    napi_unwrap(env, thisVar, (void **)&objectInfo);
    HILOG_INFO("tag dsa %{public}s,FileAccessHelper objectInfo", __func__);
    deleteCB->fileAccessHelper = objectInfo;

    if (argcAsync > argcPromise) {
        ret = DeleteAsync(env, args, ARGS_ONE, deleteCB);
    } else {
        ret = DeletePromise(env, deleteCB);
    }
    HILOG_INFO("tag dsa %{public}s,end", __func__);
    return ret;
}

napi_value DeleteAsync(napi_env env, napi_value *args, const size_t argCallback, FileAccessHelperDeleteCB *deleteCB)
{
    HILOG_INFO("tag dsa %{public}s, asyncCallback.", __func__);
    if (args == nullptr || deleteCB == nullptr) {
        HILOG_ERROR("tag dsa %{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valuetype));
    if (valuetype == napi_function) {
        NAPI_CALL(env, napi_create_reference(env, args[argCallback], 1, &deleteCB->cbBase.cbInfo.callback));
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
    napi_value result = 0;
    NAPI_CALL(env, napi_get_null(env, &result));
    HILOG_INFO("tag dsa %{public}s, asyncCallback end.", __func__);
    return result;
}

napi_value DeletePromise(napi_env env, FileAccessHelperDeleteCB *deleteCB)
{
    HILOG_INFO("tag dsa %{public}s, promise.", __func__);
    if (deleteCB == nullptr) {
        HILOG_ERROR("tag dsa %{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = 0;
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
    HILOG_INFO("tag dsa %{public}s, promise end.", __func__);
    return promise;
}

void DeleteExecuteCB(napi_env env, void *data)
{
    HILOG_INFO("tag dsa NAPI_Delete, worker pool thread execute.");
    FileAccessHelperDeleteCB *DeleteCB = static_cast<FileAccessHelperDeleteCB *>(data);
    if (DeleteCB->fileAccessHelper != nullptr) {
        DeleteCB->execResult = INVALID_PARAMETER;
        if (!DeleteCB->selectFileUri.empty()) {
            OHOS::Uri selectFileUri(DeleteCB->selectFileUri);
            DeleteCB->result = DeleteCB->fileAccessHelper->Delete(selectFileUri);
            DeleteCB->execResult = NO_ERROR;
        } else {
            HILOG_ERROR("tag dsa NAPI_Delete, fileAccessHelper selectFileUri is empty");
        }
    } else {
        HILOG_ERROR("tag dsa NAPI_Delete, fileAccessHelper == nullptr");
    }
    HILOG_INFO("tag dsa NAPI_Delete, worker pool thread execute end.");
}

void DeleteAsyncCompleteCB(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("tag dsa NAPI_Delete, main event thread complete.");
    FileAccessHelperDeleteCB *DeleteCB = static_cast<FileAccessHelperDeleteCB *>(data);
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_TWO] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, DeleteCB->cbBase.cbInfo.callback, &callback));

    result[PARAM0] = GetCallbackErrorValue(env, DeleteCB->execResult);
    napi_create_int32(env, DeleteCB->result, &result[PARAM1]);
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_TWO, &result[PARAM0], &callResult));

    if (DeleteCB->cbBase.cbInfo.callback != nullptr) {
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, DeleteCB->cbBase.cbInfo.callback));
    }
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, DeleteCB->cbBase.asyncWork));
    delete DeleteCB;
    DeleteCB = nullptr;
    HILOG_INFO("tag dsa NAPI_Delete, main event thread complete end.");
}

void DeletePromiseCompleteCB(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("tag dsa NAPI_Delete,  main event thread complete.");
    FileAccessHelperDeleteCB *DeleteCB = static_cast<FileAccessHelperDeleteCB *>(data);
    napi_value result = nullptr;
    napi_create_int32(env, DeleteCB->result, &result);
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, DeleteCB->cbBase.deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, DeleteCB->cbBase.asyncWork));
    delete DeleteCB;
    DeleteCB = nullptr;
    HILOG_INFO("tag dsa NAPI_Delete,  main event thread complete end.");
}

napi_value NAPI_Move(napi_env env, napi_callback_info info)
{
    HILOG_INFO("tag dsa %{public}s,called", __func__);
    FileAccessHelperMoveCB *moveCB = new (std::nothrow) FileAccessHelperMoveCB;
    if (moveCB == nullptr) {
        HILOG_ERROR("tag dsa %{public}s, moveCB == nullptr.", __func__);
        return WrapVoidToJS(env);
    }
    moveCB->cbBase.cbInfo.env = env;
    moveCB->cbBase.asyncWork = nullptr;
    moveCB->cbBase.deferred = nullptr;
    moveCB->cbBase.ability = nullptr;

    napi_value ret = MoveWrap(env, info, moveCB);
    if (ret == nullptr) {
        HILOG_ERROR("tag dsa %{public}s,ret == nullptr", __func__);
        if (moveCB != nullptr) {
            delete moveCB;
            moveCB = nullptr;
        }
        ret = WrapVoidToJS(env);
    }
    HILOG_INFO("tag dsa %{public}s,end", __func__);
    return ret;
}

napi_value MoveWrap(napi_env env, napi_callback_info info, FileAccessHelperMoveCB *moveCB)
{
    HILOG_INFO("tag dsa %{public}s,called", __func__);
    size_t argcAsync = ARGS_THREE;
    const size_t argcPromise = ARGS_TWO;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = 0;
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, &thisVar, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        HILOG_ERROR("tag dsa %{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[PARAM0], &valuetype));
    if (valuetype == napi_string) {
        moveCB->sourceFileUri = NapiValueToStringUtf8(env, args[PARAM0]);
        HILOG_INFO("tag dsa %{public}s,sourceFileUri=%{public}s", __func__, moveCB->sourceFileUri.c_str());
    }

    NAPI_CALL(env, napi_typeof(env, args[PARAM1], &valuetype));
    if (valuetype == napi_string) {
        moveCB->targetParentUri = NapiValueToStringUtf8(env, args[PARAM1]);
        HILOG_INFO("tag dsa %{public}s,targetParentUri=%{public}s", __func__, moveCB->targetParentUri.c_str());
    }

    FileAccessHelper *objectInfo = nullptr;
    napi_unwrap(env, thisVar, (void **)&objectInfo);
    HILOG_INFO("tag dsa %{public}s,FileAccessHelper objectInfo", __func__);
    moveCB->fileAccessHelper = objectInfo;

    if (argcAsync > argcPromise) {
        ret = MoveAsync(env, args, ARGS_TWO, moveCB);
    } else {
        ret = MovePromise(env, moveCB);
    }
    HILOG_INFO("tag dsa %{public}s,end", __func__);
    return ret;
}

napi_value MoveAsync(napi_env env, napi_value *args, const size_t argCallback, FileAccessHelperMoveCB *moveCB)
{
    HILOG_INFO("tag dsa %{public}s, asyncCallback.", __func__);
    if (args == nullptr || moveCB == nullptr) {
        HILOG_ERROR("tag dsa %{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valuetype));
    if (valuetype == napi_function) {
        NAPI_CALL(env, napi_create_reference(env, args[argCallback], 1, &moveCB->cbBase.cbInfo.callback));
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
    napi_value result = 0;
    NAPI_CALL(env, napi_get_null(env, &result));
    HILOG_INFO("tag dsa %{public}s, asyncCallback end.", __func__);
    return result;
}

napi_value MovePromise(napi_env env, FileAccessHelperMoveCB *moveCB)
{
    HILOG_INFO("tag dsa %{public}s, promise.", __func__);
    if (moveCB == nullptr) {
        HILOG_ERROR("tag dsa %{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = 0;
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
    HILOG_INFO("tag dsa %{public}s, promise end.", __func__);
    return promise;
}

void MoveExecuteCB(napi_env env, void *data)
{
    HILOG_INFO("tag dsa NAPI_Move, worker pool thread execute.");
    FileAccessHelperMoveCB *MoveCB = static_cast<FileAccessHelperMoveCB *>(data);
    if (MoveCB->fileAccessHelper != nullptr) {
        MoveCB->execResult = INVALID_PARAMETER;
        if (!MoveCB->sourceFileUri.empty()) {
            OHOS::Uri sourceFileUri(MoveCB->sourceFileUri);
            OHOS::Uri targetParentUri(MoveCB->targetParentUri);
            std::string newFile = "";
            OHOS::Uri newFileUri(newFile);
            int err = MoveCB->fileAccessHelper->Move(sourceFileUri, targetParentUri, newFileUri);
            MoveCB->result = newFileUri.ToString();
            MoveCB->execResult = err;
        } else {
            HILOG_ERROR("tag dsa NAPI_Move, fileAccessHelper sourceFileUri is empty");
        }
    } else {
        HILOG_ERROR("tag dsa NAPI_Move, fileAccessHelper == nullptr");
    }
    HILOG_INFO("tag dsa NAPI_Move, worker pool thread execute end.");
}

void MoveAsyncCompleteCB(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("tag dsa NAPI_Move, main event thread complete.");
    FileAccessHelperCreateFileCB *MoveCB = static_cast<FileAccessHelperCreateFileCB *>(data);
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_TWO] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, MoveCB->cbBase.cbInfo.callback, &callback));

    result[PARAM0] = GetCallbackErrorValue(env, MoveCB->execResult);
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, MoveCB->result.c_str(), NAPI_AUTO_LENGTH, &result[PARAM1]));
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_TWO, &result[PARAM0], &callResult));

    if (MoveCB->cbBase.cbInfo.callback != nullptr) {
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, MoveCB->cbBase.cbInfo.callback));
    }
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, MoveCB->cbBase.asyncWork));
    delete MoveCB;
    MoveCB = nullptr;
    HILOG_INFO("tag dsa NAPI_Move, main event thread complete end.");
}

void MovePromiseCompleteCB(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("tag dsa NAPI_Move,  main event thread complete.");
    FileAccessHelperCreateFileCB *MoveCB = static_cast<FileAccessHelperCreateFileCB *>(data);
    napi_value result = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, MoveCB->result.c_str(), NAPI_AUTO_LENGTH, &result));
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, MoveCB->cbBase.deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, MoveCB->cbBase.asyncWork));
    delete MoveCB;
    MoveCB = nullptr;
    HILOG_INFO("tag dsa NAPI_Move,  main event thread complete end.");
}

napi_value NAPI_Rename(napi_env env, napi_callback_info info)
{
    HILOG_INFO("tag dsa %{public}s,called", __func__);
    FileAccessHelperRenameCB *renameCB = new (std::nothrow) FileAccessHelperRenameCB;
    if (renameCB == nullptr) {
        HILOG_ERROR("tag dsa %{public}s, renameCB == nullptr.", __func__);
        return WrapVoidToJS(env);
    }
    renameCB->cbBase.cbInfo.env = env;
    renameCB->cbBase.asyncWork = nullptr;
    renameCB->cbBase.deferred = nullptr;
    renameCB->cbBase.ability = nullptr;

    napi_value ret = RenameWrap(env, info, renameCB);
    if (ret == nullptr) {
        HILOG_ERROR("tag dsa %{public}s,ret == nullptr", __func__);
        if (renameCB != nullptr) {
            delete renameCB;
            renameCB = nullptr;
        }
        ret = WrapVoidToJS(env);
    }
    HILOG_INFO("tag dsa %{public}s,end", __func__);
    return ret;
}

napi_value RenameWrap(napi_env env, napi_callback_info info, FileAccessHelperRenameCB *renameCB)
{
    HILOG_INFO("tag dsa %{public}s,called", __func__);
    size_t argcAsync = ARGS_THREE;
    const size_t argcPromise = ARGS_TWO;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = 0;
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, &thisVar, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        HILOG_ERROR("tag dsa %{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[PARAM0], &valuetype));
    if (valuetype == napi_string) {
        renameCB->sourceFileUri = NapiValueToStringUtf8(env, args[PARAM0]);
        HILOG_INFO("tag dsa %{public}s,sourceFileUri=%{public}s", __func__, renameCB->sourceFileUri.c_str());
    }

    NAPI_CALL(env, napi_typeof(env, args[PARAM1], &valuetype));
    if (valuetype == napi_string) {
        renameCB->displayName = NapiValueToStringUtf8(env, args[PARAM1]);
        HILOG_INFO("tag dsa %{public}s,displayName=%{public}s", __func__, renameCB->displayName.c_str());
    }

    FileAccessHelper *objectInfo = nullptr;
    napi_unwrap(env, thisVar, (void **)&objectInfo);
    HILOG_INFO("tag dsa %{public}s,FileAccessHelper objectInfo", __func__);
    renameCB->fileAccessHelper = objectInfo;

    if (argcAsync > argcPromise) {
        ret = RenameAsync(env, args, ARGS_TWO, renameCB);
    } else {
        ret = RenamePromise(env, renameCB);
    }
    HILOG_INFO("tag dsa %{public}s,end", __func__);
    return ret;
}

napi_value RenameAsync(napi_env env, napi_value *args, const size_t argCallback, FileAccessHelperRenameCB *renameCB)
{
    HILOG_INFO("tag dsa %{public}s, asyncCallback.", __func__);
    if (args == nullptr || renameCB == nullptr) {
        HILOG_ERROR("tag dsa %{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valuetype));
    if (valuetype == napi_function) {
        NAPI_CALL(env, napi_create_reference(env, args[argCallback], 1, &renameCB->cbBase.cbInfo.callback));
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
    napi_value result = 0;
    NAPI_CALL(env, napi_get_null(env, &result));
    HILOG_INFO("tag dsa %{public}s, asyncCallback end.", __func__);
    return result;
}

napi_value RenamePromise(napi_env env, FileAccessHelperRenameCB *renameCB)
{
    HILOG_INFO("tag dsa %{public}s, promise.", __func__);
    if (renameCB == nullptr) {
        HILOG_ERROR("tag dsa %{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = 0;
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
    HILOG_INFO("tag dsa %{public}s, promise end.", __func__);
    return promise;
}

void RenameExecuteCB(napi_env env, void *data)
{
    HILOG_INFO("tag dsa NAPI_Rename, worker pool thread execute.");
    FileAccessHelperRenameCB *renameCB = static_cast<FileAccessHelperRenameCB *>(data);
    if (renameCB->fileAccessHelper != nullptr) {
        renameCB->execResult = INVALID_PARAMETER;
        if (!renameCB->sourceFileUri.empty()) {
            OHOS::Uri sourceFileUri(renameCB->sourceFileUri);
            std::string newFile = "";
            OHOS::Uri newFileUri(newFile);
            int err = renameCB->fileAccessHelper->Rename(sourceFileUri, renameCB->displayName, newFileUri);
            renameCB->result = newFileUri.ToString();
            renameCB->execResult = err;
        } else {
            HILOG_ERROR("tag dsa NAPI_Rename, fileAccessHelper sourceFileUri is empty");
        }
    } else {
        HILOG_ERROR("tag dsa NAPI_Rename, fileAccessHelper == nullptr");
    }
    HILOG_INFO("tag dsa NAPI_Rename, worker pool thread execute end.");
}

void RenameAsyncCompleteCB(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("tag dsa NAPI_Rename, main event thread complete.");
    FileAccessHelperRenameCB *RenameCB = static_cast<FileAccessHelperRenameCB *>(data);
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_TWO] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, RenameCB->cbBase.cbInfo.callback, &callback));

    result[PARAM0] = GetCallbackErrorValue(env, RenameCB->execResult);
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, RenameCB->result.c_str(), NAPI_AUTO_LENGTH, &result[PARAM1]));
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_TWO, &result[PARAM0], &callResult));

    if (RenameCB->cbBase.cbInfo.callback != nullptr) {
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, RenameCB->cbBase.cbInfo.callback));
    }
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, RenameCB->cbBase.asyncWork));
    delete RenameCB;
    RenameCB = nullptr;
    HILOG_INFO("tag dsa NAPI_Rename, main event thread complete end.");
}

void RenamePromiseCompleteCB(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("tag dsa NAPI_Rename,  main event thread complete.");
    FileAccessHelperRenameCB *RenameCB = static_cast<FileAccessHelperRenameCB *>(data);
    napi_value result = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, RenameCB->result.c_str(), NAPI_AUTO_LENGTH, &result));
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, RenameCB->cbBase.deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, RenameCB->cbBase.asyncWork));
    delete RenameCB;
    RenameCB = nullptr;
    HILOG_INFO("tag dsa NAPI_Rename,  main event thread complete end.");
}

napi_value NAPI_CloseFile(napi_env env, napi_callback_info info)
{
    HILOG_INFO("tag dsa %{public}s,called", __func__);
    FileAccessHelperCloseFileCB *closeFileCB = new (std::nothrow) FileAccessHelperCloseFileCB;
    if (closeFileCB == nullptr) {
        HILOG_ERROR("%{public}s, closeFileCB == nullptr.", __func__);
        return WrapVoidToJS(env);
    }
    closeFileCB->cbBase.cbInfo.env = env;
    closeFileCB->cbBase.asyncWork = nullptr;
    closeFileCB->cbBase.deferred = nullptr;
    closeFileCB->cbBase.ability = nullptr;

    napi_value ret = CloseFileWrap(env, info, closeFileCB);
    if (ret == nullptr) {
        HILOG_ERROR("tag dsa %{public}s,ret == nullptr", __func__);
        if (closeFileCB != nullptr) {
            delete closeFileCB;
            closeFileCB = nullptr;
        }
        ret = WrapVoidToJS(env);
    }
    HILOG_INFO("tag dsa %{public}s,end", __func__);
    return ret;
}

napi_value CloseFileWrap(napi_env env, napi_callback_info info, FileAccessHelperCloseFileCB *closeFileCB)
{
    HILOG_INFO("tag dsa %{public}s,called", __func__);
    size_t argcAsync = ARGS_THREE;
    const size_t argcPromise = ARGS_TWO;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = 0;
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, &thisVar, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        HILOG_ERROR("tag dsa %{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[PARAM0], &valuetype));
    if (valuetype == napi_number) {
        closeFileCB->fd = NapiValueToInt32Utf8(env, args[PARAM0]);
        HILOG_INFO("tag dsa %{public}s,fd=%d", __func__, closeFileCB->fd);
    }

    NAPI_CALL(env, napi_typeof(env, args[PARAM1], &valuetype));
    if (valuetype == napi_string) {
        closeFileCB->uri = NapiValueToStringUtf8(env, args[PARAM1]);
        HILOG_INFO("tag dsa %{public}s,uri=%{public}s", __func__, closeFileCB->uri.c_str());
    }

    FileAccessHelper *objectInfo = nullptr;
    napi_unwrap(env, thisVar, (void **)&objectInfo);
    HILOG_INFO("tag dsa %{public}s,FileAccessHelper objectInfo", __func__);
    closeFileCB->fileAccessHelper = objectInfo;

    if (argcAsync > argcPromise) {
        ret = CloseFileAsync(env, args, ARGS_TWO, closeFileCB);
    } else {
        ret = CloseFilePromise(env, closeFileCB);
    }
    HILOG_INFO("tag dsa %{public}s,end", __func__);
    return ret;
}

napi_value CloseFileAsync(napi_env env, napi_value *args, const size_t argCallback, FileAccessHelperCloseFileCB *closeFileCB)
{
    HILOG_INFO("tag dsa %{public}s, asyncCallback.", __func__);
    if (args == nullptr || closeFileCB == nullptr) {
        HILOG_ERROR("tag dsa %{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valuetype));
    if (valuetype == napi_function) {
        NAPI_CALL(env, napi_create_reference(env, args[argCallback], 1, &closeFileCB->cbBase.cbInfo.callback));
    }

    NAPI_CALL(env,
        napi_create_async_work(env,
            nullptr,
            resourceName,
            CloseFileExecuteCB,
            CloseFileAsyncCompleteCB,
            (void *)closeFileCB,
            &closeFileCB->cbBase.asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, closeFileCB->cbBase.asyncWork));
    napi_value result = 0;
    NAPI_CALL(env, napi_get_null(env, &result));
    HILOG_INFO("tag dsa %{public}s, asyncCallback end.", __func__);
    return result;
}

napi_value CloseFilePromise(napi_env env, FileAccessHelperCloseFileCB *closeFileCB)
{
    HILOG_INFO("tag dsa %{public}s, promise.", __func__);
    if (closeFileCB == nullptr) {
        HILOG_ERROR("tag dsa %{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = 0;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    closeFileCB->cbBase.deferred = deferred;

    NAPI_CALL(env,
        napi_create_async_work(env,
            nullptr,
            resourceName,
            CloseFileExecuteCB,
            CloseFilePromiseCompleteCB,
            (void *)closeFileCB,
            &closeFileCB->cbBase.asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, closeFileCB->cbBase.asyncWork));
    HILOG_INFO("tag dsa %{public}s, promise end.", __func__);
    return promise;
}

void CloseFileExecuteCB(napi_env env, void *data)
{
    HILOG_INFO("tag dsa NAPI_CloseFile, worker pool thread execute.");
    FileAccessHelperCloseFileCB *CloseFileCB = static_cast<FileAccessHelperCloseFileCB *>(data);
    if (CloseFileCB->fileAccessHelper != nullptr) {
        CloseFileCB->execResult = INVALID_PARAMETER;
        if (CloseFileCB->fd!=NO_ERROR) {
            CloseFileCB->result = CloseFileCB->fileAccessHelper->CloseFile(CloseFileCB->fd, CloseFileCB->uri);
            CloseFileCB->execResult = NO_ERROR;
        } else {
            HILOG_ERROR("tag dsa NAPI_CloseFile, fileAccessHelper fd != NO_ERROR");
        }
    } else {
        HILOG_ERROR("tag dsa NAPI_CloseFile, fileAccessHelper == nullptr");
    }
    HILOG_INFO("tag dsa NAPI_CloseFile, worker pool thread execute end.");
}

void CloseFileAsyncCompleteCB(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("tag dsa NAPI_CloseFile, main event thread complete.");
    FileAccessHelperCloseFileCB *CloseFileCB = static_cast<FileAccessHelperCloseFileCB *>(data);
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_TWO] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, CloseFileCB->cbBase.cbInfo.callback, &callback));

    result[PARAM0] = GetCallbackErrorValue(env, CloseFileCB->execResult);
    napi_create_int32(env, CloseFileCB->result, &result[PARAM1]);
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_TWO, &result[PARAM0], &callResult));

    if (CloseFileCB->cbBase.cbInfo.callback != nullptr) {
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, CloseFileCB->cbBase.cbInfo.callback));
    }
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, CloseFileCB->cbBase.asyncWork));
    delete CloseFileCB;
    CloseFileCB = nullptr;
    HILOG_INFO("tag dsa NAPI_CloseFile, main event thread complete end.");
}

void CloseFilePromiseCompleteCB(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("tag dsa NAPI_CloseFile,  main event thread complete.");
    FileAccessHelperCloseFileCB *CloseFileCB = static_cast<FileAccessHelperCloseFileCB *>(data);
    napi_value result = nullptr;
    napi_create_int32(env, CloseFileCB->result, &result);
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, CloseFileCB->cbBase.deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, CloseFileCB->cbBase.asyncWork));
    delete CloseFileCB;
    CloseFileCB = nullptr;
    HILOG_INFO("tag dsa NAPI_CloseFile,  main event thread complete end.");
}

napi_value NAPI_Release(napi_env env, napi_callback_info info)
{
    HILOG_INFO("tag dsa %{public}s,called", __func__);
    FileAccessHelperReleaseCB *releaseCB = new (std::nothrow) FileAccessHelperReleaseCB;
    if (releaseCB == nullptr) {
        HILOG_ERROR("tag dsa %{public}s, releaseCB == nullptr.", __func__);
        return WrapVoidToJS(env);
    }
    releaseCB->cbBase.cbInfo.env = env;
    releaseCB->cbBase.asyncWork = nullptr;
    releaseCB->cbBase.deferred = nullptr;

    napi_value ret = ReleaseWrap(env, info, releaseCB);
    if (ret == nullptr) {
        HILOG_ERROR("tag dsa %{public}s,ret == nullptr", __func__);
        delete releaseCB;
        releaseCB = nullptr;
        ret = WrapVoidToJS(env);
    }
    HILOG_INFO("tag dsa %{public}s,end", __func__);
    return ret;
}

napi_value ReleaseWrap(napi_env env, napi_callback_info info, FileAccessHelperReleaseCB *releaseCB)
{
    HILOG_INFO("tag dsa %{public}s,called", __func__);
    size_t argcAsync = ARGS_ONE;
    const size_t argcPromise = ARGS_ZERO;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = nullptr;
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, &thisVar, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        HILOG_ERROR("tag dsa %{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    FileAccessHelper *objectInfo = nullptr;
    napi_unwrap(env, thisVar, (void **)&objectInfo);
    HILOG_INFO("tag dsa FileAccessHelper ReleaseWrap objectInfo = %{public}p", objectInfo);
    releaseCB->fileAccessHelper = objectInfo;

    if (argcAsync > argcPromise) {
        ret = ReleaseAsync(env, args, PARAM0, releaseCB);
    } else {
        ret = ReleasePromise(env, releaseCB);
    }
    HILOG_INFO("tag dsa %{public}s,end", __func__);
    return ret;
}

napi_value ReleaseAsync(napi_env env, napi_value *args, const size_t argCallback, FileAccessHelperReleaseCB *releaseCB)
{
    HILOG_INFO("tag dsa %{public}s, asyncCallback.", __func__);
    if (args == nullptr || releaseCB == nullptr) {
        HILOG_ERROR("tag dsa %{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valuetype));
    if (valuetype == napi_function) {
        NAPI_CALL(env, napi_create_reference(env, args[argCallback], 1, &releaseCB->cbBase.cbInfo.callback));
    }

    NAPI_CALL(env,
        napi_create_async_work(env,
            nullptr,
            resourceName,
            ReleaseExecuteCB,
            ReleaseAsyncCompleteCB,
            (void *)releaseCB,
            &releaseCB->cbBase.asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, releaseCB->cbBase.asyncWork));
    napi_value result = 0;
    NAPI_CALL(env, napi_get_null(env, &result));
    HILOG_INFO("tag dsa %{public}s, asyncCallback end.", __func__);
    return result;
}

napi_value ReleasePromise(napi_env env, FileAccessHelperReleaseCB *releaseCB)
{
    HILOG_INFO("tag dsa %{public}s, promise.", __func__);
    if (releaseCB == nullptr) {
        HILOG_ERROR("tag dsa %{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = 0;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    releaseCB->cbBase.deferred = deferred;

    NAPI_CALL(env,
        napi_create_async_work(env,
            nullptr,
            resourceName,
            ReleaseExecuteCB,
            ReleasePromiseCompleteCB,
            (void *)releaseCB,
            &releaseCB->cbBase.asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, releaseCB->cbBase.asyncWork));
    HILOG_INFO("tag dsa %{public}s, promise end.", __func__);
    return promise;
}

void ReleaseExecuteCB(napi_env env, void *data)
{
    HILOG_INFO("tag dsa NAPI_Release, worker pool thread execute.");
    FileAccessHelperReleaseCB *releaseCB = static_cast<FileAccessHelperReleaseCB *>(data);
    if (releaseCB->fileAccessHelper != nullptr) {
        releaseCB->result = releaseCB->fileAccessHelper->Release();
    } else {
        HILOG_ERROR("tag dsa NAPI_Release, fileAccessHelper == nullptr");
    }
    HILOG_INFO("tag dsa NAPI_Release, worker pool thread execute end.");
}

void ReleaseAsyncCompleteCB(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("tag dsa NAPI_Release, main event thread complete.");
    FileAccessHelperReleaseCB *releaseCB = static_cast<FileAccessHelperReleaseCB *>(data);
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_TWO] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, releaseCB->cbBase.cbInfo.callback, &callback));

    result[PARAM0] = GetCallbackErrorValue(env, NO_ERROR);
    napi_get_boolean(env, releaseCB->result, &result[PARAM1]);
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_TWO, &result[PARAM0], &callResult));

    if (releaseCB->cbBase.cbInfo.callback != nullptr) {
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, releaseCB->cbBase.cbInfo.callback));
    }
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, releaseCB->cbBase.asyncWork));
    delete releaseCB;
    releaseCB = nullptr;
    HILOG_INFO("tag dsa NAPI_Release, main event thread complete end.");
}

void ReleasePromiseCompleteCB(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("tag dsa NAPI_Release,  main event thread complete.");
    FileAccessHelperReleaseCB *releaseCB = static_cast<FileAccessHelperReleaseCB *>(data);
    napi_value result = nullptr;
    napi_get_boolean(env, releaseCB->result, &result);
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, releaseCB->cbBase.deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, releaseCB->cbBase.asyncWork));
    delete releaseCB;
    releaseCB = nullptr;
    HILOG_INFO("tag dsa NAPI_Release,  main event thread complete end.");
}
}  // namespace AppExecFwk
}  // namespace OHOS