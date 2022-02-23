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

#include "ipc_types.h"
#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"

#include "data_ability_helper.h"

namespace OHOS {
namespace FileManagerService {
class MediaFileUtils  {
public:
    MediaFileUtils();
    ~MediaFileUtils();
    static int DoListFile(const std::string &type, const std::string &path, int offset, int count,
        std::shared_ptr<NativeRdb::AbsSharedResultSet> &result);
    static std::shared_ptr<NativeRdb::AbsSharedResultSet> DoQuery(const std::string &selection,
        const std::vector<std::string> &selectionArgs);
    static std::shared_ptr<NativeRdb::AbsSharedResultSet> DoQuery(const std::string &selection,
        const std::vector<std::string> &selectionArgs, int offset, int count);
    static int DoInsert(const std::string &name, const std::string &path, const std::string &type, std::string &uri);
    static bool GetFileInfo(std::shared_ptr<NativeRdb::AbsSharedResultSet> result, std::shared_ptr<FileInfo> &fileInfo);
    static int GetFileInfoFromResult(std::shared_ptr<NativeRdb::AbsSharedResultSet> result,
        std::vector<std::shared_ptr<FileInfo>> &fileList);
    static bool InitMediaTableColIndexMap(std::shared_ptr<NativeRdb::AbsSharedResultSet> result);
    static bool InitHelper(sptr<IRemoteObject> obj);
private:
    inline static std::vector<std::pair<int, std::string>> mediaTableMap = {};
    inline static std::shared_ptr<AppExecFwk::DataAbilityHelper> abilityHelper = nullptr;
};
} // namespace FileManagerService
} // namespace OHOS
#endif // STORAGE_SERIVCES_MEDIA_FILE_UTILS_H