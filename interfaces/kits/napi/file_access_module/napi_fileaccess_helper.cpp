napi_value NAPI_Rename(napi_env env, napi_callback_info info)
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

napi_value RenameWrap(napi_env env, napi_callback_info info, FileAccessHelperRenameCB *renameCB)
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

    if (argcAsync > argcPromise) {
        ret = RenameAsync(env, args, ARGS_TWO, renameCB);
    } else {
        ret = RenamePromise(env, renameCB);
    }
    return ret;
}

napi_value RenameAsync(napi_env env, napi_value *args, const size_t argCallback, FileAccessHelperRenameCB *renameCB)
{
    if (args == nullptr || renameCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valueType));
    if (valueType == napi_function) {
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
    return result;
}

napi_value RenamePromise(napi_env env, FileAccessHelperRenameCB *renameCB)
{
    if (renameCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
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
    return promise;
}

void RenameExecuteCB(napi_env env, void *data)
{
    FileAccessHelperRenameCB *renameCB = static_cast<FileAccessHelperRenameCB *>(data);
    renameCB->execResult = ERR_ERROR;
    if (renameCB->fileAccessHelper == nullptr) {
        HILOG_ERROR("NAPI_Rename, fileAccessHelper == nullptr");
        return ;
    }
    if (renameCB->sourceFileUri.empty()) {
        HILOG_ERROR("NAPI_Rename, fileAccessHelper sourceFileUri is empty");
        return ;
    }
    OHOS::Uri sourceFileUri(renameCB->sourceFileUri);
    std::string newFile = "";
    OHOS::Uri newFileUri(newFile);
    int err = renameCB->fileAccessHelper->Rename(sourceFileUri, renameCB->displayName, newFileUri);
    renameCB->result = newFileUri.ToString();
    renameCB->execResult = err;
}

void RenameAsyncCompleteCB(napi_env env, napi_status status, void *data)
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

void RenamePromiseCompleteCB(napi_env env, napi_status status, void *data)
{
    FileAccessHelperRenameCB *renameCB = static_cast<FileAccessHelperRenameCB *>(data);
    napi_value result = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, renameCB->result.c_str(), NAPI_AUTO_LENGTH, &result));
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, renameCB->cbBase.deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, renameCB->cbBase.asyncWork));
    delete renameCB;
    renameCB = nullptr;
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

napi_value ListFileWrap(napi_env env, napi_callback_info info, FileAccessHelperListFileCB *listFileCB)
{
    size_t argcAsync = ARGS_TWO;
    const size_t argcPromise = ARGS_ONE;
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
    listFileCB->sourceFileUri = NapiValueToStringUtf8(env, args[PARAM0]);

    FileAccessHelper *objectInfo = nullptr;
    napi_unwrap(env, thisVar, (void **)&objectInfo);
    listFileCB->fileAccessHelper = objectInfo;

    if (argcAsync > argcPromise) {
        ret = ListFileAsync(env, args, ARGS_ONE, listFileCB);
    } else {
        ret = ListFilePromise(env, listFileCB);
    }
    return ret;
}

napi_value ListFileAsync(napi_env env,
                         napi_value *args,
                         const size_t argCallback,
                         FileAccessHelperListFileCB *listFileCB)
{
    if (args == nullptr || listFileCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valueType));
    if (valueType == napi_function) {
        NAPI_CALL(env, napi_create_reference(env, args[argCallback], 1, &listFileCB->cbBase.cbInfo.callback));
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
    napi_value result = 0;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

napi_value ListFilePromise(napi_env env, FileAccessHelperListFileCB *listFileCB)
{
    if (listFileCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = 0;
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

void ListFileExecuteCB(napi_env env, void *data)
{
    FileAccessHelperListFileCB *listFileCB = static_cast<FileAccessHelperListFileCB *>(data);
    listFileCB->execResult = ERR_ERROR;
    if (listFileCB->fileAccessHelper == nullptr) {
        HILOG_ERROR(" NAPI_ListFile, fileAccessHelper == nullptr");
        return ;
    }
    if (listFileCB->sourceFileUri.empty()) {
        HILOG_ERROR(" NAPI_ListFile, fileAccessHelper sourceFileUri is empty");
        return ;
    }
    OHOS::Uri sourceFileUri(listFileCB->sourceFileUri);
    listFileCB->result = listFileCB->fileAccessHelper->ListFile(sourceFileUri);
    listFileCB->execResult = 0;
}

void ListFileAsyncCompleteCB(napi_env env, napi_status status, void *data)
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

void ListFilePromiseCompleteCB(napi_env env, napi_status status, void *data)
{
    FileAccessHelperListFileCB *listFileCB = static_cast<FileAccessHelperListFileCB *>(data);
    napi_value result = nullptr;
    result = WrapArrayFileInfoToJS(env, listFileCB->result);
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, listFileCB->cbBase.deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, listFileCB->cbBase.asyncWork));
    delete listFileCB;
    listFileCB = nullptr;
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

napi_value GetRootsWrap(napi_env env, napi_callback_info info, FileAccessHelperGetRootsCB *getRootsCB)
{
    size_t argcAsync = ARGS_ONE;
    const size_t argcPromise = ARGS_ZERO;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = 0;
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, &thisVar, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        HILOG_ERROR("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    FileAccessHelper *objectInfo = nullptr;
    napi_unwrap(env, thisVar, (void **)&objectInfo);
    getRootsCB->fileAccessHelper = objectInfo;

    if (argcAsync > argcPromise) {
        ret = GetRootsAsync(env, args, ARGS_ZERO, getRootsCB);
    } else {
        ret = GetRootsPromise(env, getRootsCB);
    }
    return ret;
}

napi_value GetRootsAsync(napi_env env,
                         napi_value *args,
                         const size_t argCallback,
                         FileAccessHelperGetRootsCB *getRootsCB)
{
    if (args == nullptr || getRootsCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valueType));
    if (valueType == napi_function) {
        NAPI_CALL(env, napi_create_reference(env, args[argCallback], 1, &getRootsCB->cbBase.cbInfo.callback));
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
    napi_value result = 0;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

napi_value GetRootsPromise(napi_env env, FileAccessHelperGetRootsCB *getRootsCB)
{
    if (getRootsCB == nullptr) {
        HILOG_ERROR("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = 0;
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

void GetRootsExecuteCB(napi_env env, void *data)
{
    FileAccessHelperGetRootsCB *getRootsCB = static_cast<FileAccessHelperGetRootsCB *>(data);
    getRootsCB->execResult = ERR_ERROR;
    if (getRootsCB->fileAccessHelper == nullptr) {
        HILOG_ERROR(" NAPI_GetRoots, fileAccessHelper == nullptr");
        return ;
    }
    getRootsCB->result = getRootsCB->fileAccessHelper->GetRoots();
    getRootsCB->execResult = ERR_OK;
}

void GetRootsAsyncCompleteCB(napi_env env, napi_status status, void *data)
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

void GetRootsPromiseCompleteCB(napi_env env, napi_status status, void *data)
{
    FileAccessHelperGetRootsCB *getRootsCB = static_cast<FileAccessHelperGetRootsCB *>(data);
    napi_value result = nullptr;
    result = WrapArrayDeviceInfoToJS(env, getRootsCB->result);
    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, getRootsCB->cbBase.deferred, result));
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, getRootsCB->cbBase.asyncWork));
    delete getRootsCB;
    getRootsCB = nullptr;
}