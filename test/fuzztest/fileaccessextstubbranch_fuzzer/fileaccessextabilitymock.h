/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef FILE_ACCESS_EXT_ABILITY_MOCK_H
#define FILE_ACCESS_EXT_ABILITY_MOCK_H

#include "file_access_ext_ability.h"

namespace OHOS {
namespace FileAccessFwk {
using namespace AbilityRuntime;

class FileAccessExtAbilityMock : public FileAccessExtAbility {
public:
    void Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord>&,
        const std::shared_ptr<AppExecFwk::OHOSApplication>&, std::shared_ptr<AppExecFwk::AbilityHandler>&,
        const sptr<IRemoteObject>&) override {}
    void OnStart(const AAFwk::Want&) override {}
    sptr<IRemoteObject> OnConnect(const AAFwk::Want&) override { return nullptr; }
    int OpenFile(const Uri&, const int, int&) override { return 0; }
    int CreateFile(const Uri&, const std::string&,  Uri&) override { return 0; }
    int Mkdir(const Uri&, const std::string&, Uri&) override { return 0; }
    int Delete(const Uri&) override { return 0; }
    int Move(const Uri&, const Uri&, Uri&) override { return 0; }
    int Copy(const Uri&, const Uri&, std::vector<Result>&, bool force = false) override { return 0; }
    int CopyFile(const Uri&, const Uri&, const std::string&, Uri&) override { return 0; }
    int Rename(const Uri&, const std::string&, Uri&) override { return 0; }
    int ListFile(const FileInfo&, const int64_t, const int64_t, const FileFilter&, std::vector<FileInfo>&) override
        { return 0; }
    int ScanFile(const FileInfo&, const int64_t, const int64_t, const FileFilter&, std::vector<FileInfo>&) override
        { return 0; }
    int GetFileInfoFromUri(const Uri&, FileInfo&) override { return 0; }
    int GetFileInfoFromRelativePath(const std::string&, FileInfo&) override { return 0; }
    int GetRoots(std::vector<RootInfo>&) override { return 0; }
    int Access(const Uri&, bool&) override { return 0; }
    int Query(const Uri&, std::vector<std::string>&, std::vector<std::string>&) override { return 0; }
    int StartWatcher(const Uri&) override { return 0; }
    int StopWatcher(const Uri&) override { return 0; }
    int MoveItem(const Uri&, const Uri&, std::vector<Result>&, bool force = false) override { return 0; }
    int MoveFile(const Uri&, const Uri&, std::string&, Uri&) override { return 0; }
    int GetFileInfoNum(const std::string&, const FileFilter&, bool, uint32_t&) override { return 0; }
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_EXT_ABILITY_MOCK_H