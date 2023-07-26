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

#ifndef FILE_INFO_ENTITY_H
#define FILE_INFO_ENTITY_H

#include "file_access_extension_info.h"
#include "file_access_helper.h"
#include "file_filter.h"

namespace OHOS {
namespace FileAccessFwk {
using namespace DistributedFS;

struct FileInfoEntity {
    FileAccessHelper *fileAccessHelper { nullptr };
    FileInfo fileInfo;
    FileFilter filter { {}, {}, {}, 0, 0, false, false };
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_INFO_ENTITY_H