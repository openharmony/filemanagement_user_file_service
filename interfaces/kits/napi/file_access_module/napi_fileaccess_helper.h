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

#ifndef NAPI_FILEACCESS_HELPER_H
#define NAPI_FILEACCESS_HELPER_H

#include "file_access_common.h"

namespace OHOS {
namespace FileAccessFwk {
    napi_value FileAccessHelperInit(napi_env env, napi_value exports);
    napi_value FileAccessHelperConstructor(napi_env env, napi_callback_info info);
    napi_value NAPI_OpenFile(napi_env env, napi_callback_info info);
    napi_value OpenFileWrap(napi_env env, napi_callback_info info, FileAccessHelperOpenFileCB *openFileCB);
    napi_value OpenFileAsync(napi_env env,
                             napi_value *args,
                             const size_t argCallback,
                             FileAccessHelperOpenFileCB *openFileCB);
    napi_value OpenFilePromise(napi_env env, FileAccessHelperOpenFileCB *openFileCB);
    void OpenFileExecuteCB(napi_env env, void *data);
    void OpenFileAsyncCompleteCB(napi_env env, napi_status status, void *data);
    void OpenFilePromiseCompleteCB(napi_env env, napi_status status, void *data);

    napi_value NAPI_CreateFile(napi_env env, napi_callback_info info);
    napi_value CreateFileWrap(napi_env env, napi_callback_info info, FileAccessHelperCreateFileCB *createFileCB);
    napi_value CreateFileAsync(napi_env env,
                               napi_value *args,
                               const size_t argCallback,
                               FileAccessHelperCreateFileCB *createFileCB);
    napi_value CreateFilePromise(napi_env env, FileAccessHelperCreateFileCB *createFileCB);
    void CreateFileExecuteCB(napi_env env, void *data);
    void CreateFileAsyncCompleteCB(napi_env env, napi_status status, void *data);
    void CreateFilePromiseCompleteCB(napi_env env, napi_status status, void *data);

    napi_value NAPI_Mkdir(napi_env env, napi_callback_info info);
    napi_value MkdirWrap(napi_env env, napi_callback_info info, FileAccessHelperMkdirCB *mkdirCB);
    napi_value MkdirAsync(napi_env env,
                          napi_value *args,
                          const size_t argCallback,
                          FileAccessHelperMkdirCB *mkdirCB);
    napi_value MkdirPromise(napi_env env, FileAccessHelperMkdirCB *mkdirCB);
    void MkdirExecuteCB(napi_env env, void *data);
    void MkdirAsyncCompleteCB(napi_env env, napi_status status, void *data);
    void MkdirPromiseCompleteCB(napi_env env, napi_status status, void *data);

    napi_value NAPI_Delete(napi_env env, napi_callback_info info);
    napi_value DeleteWrap(napi_env env, napi_callback_info info, FileAccessHelperDeleteCB *deleteCB);
    napi_value DeleteAsync(napi_env env,
                           napi_value *args,
                           const size_t argCallback,
                           FileAccessHelperDeleteCB *deleteCB);
    napi_value DeletePromise(napi_env env, FileAccessHelperDeleteCB *deleteCB);
    void DeleteExecuteCB(napi_env env, void *data);
    void DeleteAsyncCompleteCB(napi_env env, napi_status status, void *data);
    void DeletePromiseCompleteCB(napi_env env, napi_status status, void *data);

    napi_value NAPI_Move(napi_env env, napi_callback_info info);
    napi_value MoveWrap(napi_env env, napi_callback_info info, FileAccessHelperMoveCB *moveCB);
    napi_value MoveAsync(napi_env env,
                         napi_value *args,
                         const size_t argCallback,
                         FileAccessHelperMoveCB *moveCB);
    napi_value MovePromise(napi_env env, FileAccessHelperMoveCB *moveCB);
    void MoveExecuteCB(napi_env env, void *data);
    void MoveAsyncCompleteCB(napi_env env, napi_status status, void *data);
    void MovePromiseCompleteCB(napi_env env, napi_status status, void *data);
    
    napi_value NAPI_Rename(napi_env env, napi_callback_info info);
    napi_value RenameWrap(napi_env env, napi_callback_info info, FileAccessHelperRenameCB *renameCB);
    napi_value RenameAsync(napi_env env,
                           napi_value *args,
                           const size_t argCallback,
                           FileAccessHelperRenameCB *renameCB);
    napi_value RenamePromise(napi_env env, FileAccessHelperRenameCB *renameCB);
    void RenameExecuteCB(napi_env env, void *data);
    void RenameAsyncCompleteCB(napi_env env, napi_status status, void *data);
    void RenamePromiseCompleteCB(napi_env env, napi_status status, void *data);

    napi_value NAPI_ListFile(napi_env env, napi_callback_info info);
    napi_value ListFileWrap(napi_env env, napi_callback_info info, FileAccessHelperListFileCB *listFileCB);
    napi_value ListFileAsync(napi_env env,
                             napi_value *args,
                             const size_t argCallback,
                             FileAccessHelperListFileCB *listFileCB);
    napi_value ListFilePromise(napi_env env, FileAccessHelperListFileCB *listFileCB);
    void ListFileExecuteCB(napi_env env, void *data);
    void ListFileAsyncCompleteCB(napi_env env, napi_status status, void *data);
    void ListFilePromiseCompleteCB(napi_env env, napi_status status, void *data);

    napi_value NAPI_GetRoots(napi_env env, napi_callback_info info);
    napi_value GetRootsWrap(napi_env env, napi_callback_info info, FileAccessHelperGetRootsCB *getRootsCB);
    napi_value GetRootsAsync(napi_env env,
                             napi_value *args,
                             const size_t argCallback,
                             FileAccessHelperGetRootsCB *getRootsCB);
    napi_value GetRootsPromise(napi_env env, FileAccessHelperGetRootsCB *getRootsCB);
    void GetRootsExecuteCB(napi_env env, void *data);
    void GetRootsAsyncCompleteCB(napi_env env, napi_status status, void *data);
    void GetRootsPromiseCompleteCB(napi_env env, napi_status status, void *data);
}
} // namespace FileAccessFwk
#endif // FILE_ACCESS_NAPI_H