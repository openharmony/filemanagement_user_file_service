/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef FILE_URI_CHECK_H
#define FILE_URI_CHECK_H

#include "hilog_wrapper.h"

namespace OHOS {
namespace FileAccessFwk {

static const std::string PATH_INVALID_FLAG1 = "../";
static const std::string PATH_INVALID_FLAG2 = "/..";
static const uint32_t PATH_INVALID_FLAG_LEN = 3;
static const char FILE_SEPARATOR_CHAR = '/';

static bool IsFilePathValid(const std::string &filePath)
{
    size_t pos = filePath.find(PATH_INVALID_FLAG1);
    while (pos != std::string::npos) {
        if (pos == 0 || filePath[pos - 1] == FILE_SEPARATOR_CHAR) {
            HILOG_ERROR("Relative path is not allowed, path contain ../");
            return false;
        }
        pos = filePath.find(PATH_INVALID_FLAG1, pos + PATH_INVALID_FLAG_LEN);
    }
    pos = filePath.rfind(PATH_INVALID_FLAG2);
    if ((pos != std::string::npos) && (filePath.size() - pos == PATH_INVALID_FLAG_LEN)) {
        HILOG_ERROR("Relative path is not allowed, path tail is /..");
        return false;
    }
    return true;
}
} // FileAccessFwk
} // OHOS
#endif // FILE_URI_CHECK_H
