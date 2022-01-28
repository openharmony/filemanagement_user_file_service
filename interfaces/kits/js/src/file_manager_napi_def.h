/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#ifndef STORAGE_FILE_MANAGER_NAPI_DEF_H
#define STORAGE_FILE_MANAGER_NAPI_DEF_H
#include "n_async_work_callback.h"
#include "n_async_work_promise.h"
#include "n_func_arg.h"
#include "uni_error.h"

namespace OHOS {
namespace FileManagerService {
enum CreateFileArgs {
    CF_DEV = 0,
    CF_FILENAME = 1,
    CF_PATH = 2,
    CF_CALLBACK = 3,
};

enum GetRootArgs {
    GR_DEV = 0,
    GR_CALLBACK = 1,
};

enum ListFileArgs {
    LF_PATH = 0,
    LF_TYPE = 1,
    LF_OPTION = 2,
    LF_CALLBACK_WITHOUT_OP = 2,
    LF_CALLBACK_WITH_OP = 3,
};

constexpr int CREATE_FILE_PARA_MAX = 4;
constexpr int CREATE_FILE_PARA_MIN = 3;
constexpr int GET_ROOT_PARA_MAX = 2;
constexpr int GET_ROOT_PARA_MIN = 1;
constexpr int LIST_FILE_PARA_MAX = 4;
constexpr int LIST_FILE_PARA_MIN = 2;
} // namespace FileManagerService
} // namespace OHOS
#endif // STORAGE_FILE_MANAGER_NAPI_DEF_H
