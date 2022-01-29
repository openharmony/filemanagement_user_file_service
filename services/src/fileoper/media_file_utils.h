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
#ifndef STORAGE_SERIVCES_MEDIA_FILE_UTILS_H
#define STORAGE_SERIVCES_MEDIA_FILE_UTILS_H

#include <string>
#include <vector>

#include "abs_shared_result_set.h"
#include "file_info.h"
#include "file_oper.h"

namespace OHOS {
namespace FileManagerService {
class MediaFileUtils  {
public:
    MediaFileUtils();
    ~MediaFileUtils();
    static int DoListFile(const std::string &type, const std::string &path, int offset, int count,
        std::shared_ptr<NativeRdb::AbsSharedResultSet> &result);
    static std::shared_ptr<NativeRdb::AbsSharedResultSet> DoQuery(std::string selection,
        std::vector<std::string> selectionArgs);
    static int DoInsert(const std::string &name, const std::string &path, const std::string &type, std::string &uri);
    static bool PushFileInfo(std::shared_ptr<NativeRdb::AbsSharedResultSet> result, MessageParcel &reply);
    static bool PopFileInfo(FileInfo &file, MessageParcel &reply);
    static int GetFileInfoFromResult(std::shared_ptr<NativeRdb::AbsSharedResultSet> result,
        MessageParcel &reply, int res);
    static bool InitMediaTableColIndexMap(std::shared_ptr<NativeRdb::AbsSharedResultSet> result);
private:
    inline static std::vector<std::pair<int, std::string>> mediaTableMap = {};
};
} // namespace FileManagerService
} // namespace OHOS
#endif // STORAGE_SERIVCES_MEDIA_FILE_UTILS_H