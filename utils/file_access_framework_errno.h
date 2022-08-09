/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FILE_ACCESS_FRAMEWORK_ERRNO_H
#define FILE_ACCESS_FRAMEWORK_ERRNO_H

#include "errors.h"

namespace OHOS {
namespace FileAccessFwk {
enum {
    MODULE_FILE_ACCESS_FRAMEWORK = 0x01
};
constexpr ErrCode BASE_OFFSET = -ErrCodeOffset(SUBSYS_FILEMANAGEMENT, MODULE_FILE_ACCESS_FRAMEWORK);
enum {
    ERR_OK = 0,
    ERR_IPC_ERROR = BASE_OFFSET,            // ipc error
    ERR_PERMISSION_DENIED,                  // no permission
    ERR_INVALID_FD,                         // invalid fd
    ERR_INVALID_URI,                        // invalid uri
    ERR_URI_CHECK,                          // check uri head fail
    ERR_FILEIO_FAIL,                        // fileio fail
    ERR_PARAM_NUMBER,                       // Parameter number is abnormal
    ERR_INVALID_PARAM,                      // invalid parameter
    ERR_PARSER_FAIL,                        // parser js result error
    ERR_OPERATION_NOT_PERMITTED,            // Operation not permitted
    ERR_GET_FILEACCESS_HELPER               // get fileAccessHelper fail
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_FRAMEWORK_ERRNO_H