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

#ifndef FILE_ITERATOR_ENTITY_H
#define FILE_ITERATOR_ENTITY_H

#include <mutex>

#include "file_access_extension_info.h"
#include "file_access_helper.h"
#include "hilog_wrapper.h"
#include "file_filter.h"

namespace OHOS {
namespace FileAccessFwk {

constexpr int64_t MAX_COUNT = 1000;     // ListFile get file's max count

struct FileIteratorEntity {
    FileAccessHelper *fileAccessHelper { nullptr };
    std::mutex entityOperateMutex;
    FileInfo fileInfo;
    std::vector<FileInfo> fileInfoVec;
    int64_t offset { 0 };
    uint64_t pos { 0 };
    FileFilter filter { {}, {}, {}, FileFilter::INVALID_SIZE, FileFilter::INVALID_MODIFY_AFTER, false, false };
    int flag { 0 };
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ITERATOR_ENTITY_H