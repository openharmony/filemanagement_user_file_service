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

#ifndef NAPI_UTILS_H
#define NAPI_UTILS_H

#include <cstdint>

#include "filemgmt_libn.h"
#include "file_filter.h"

namespace OHOS {
namespace FileAccessFwk {
using namespace FileManagement::LibN;

int IsDirectory(const int64_t mode);
int GetFileFilterParam(const NVal &argv, FileFilter &filter);
const int MAX_SUFFIX_LENGTH = 20;
} // namespace FileAccessFwk
} // namespace OHOS
#endif // NAPI_UTILS_H