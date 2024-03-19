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
    void Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord> &record,
        const std::shared_ptr<AppExecFwk::OHOSApplication> &application,
        std::shared_ptr<AppExecFwk::AbilityHandler> &handler,
        const sptr<IRemoteObject> &token) override {};
    void OnStart(const AAFwk::Want &want) override {};
    sptr<IRemoteObject> OnConnect(const AAFwk::Want &want) override { return 0; };
    int OpenFile(const Uri &uri, const int flags, int &fd) override { return 0; };
    int CreateFile(const Uri &parent, const std::string &displayName,  Uri &newFile) override { return 0; };
    int Mkdir(const Uri &parent, const std::string &displayName, Uri &newFile) override { return 0; };
    int Delete(const Uri &sourceFile) override { return 0; };
    int Move(const Uri &sourceFile, const Uri &targetParent, Uri &newFile) override { return 0; };
    int Copy(const Uri &sourceUri, const Uri &destUri, std::vector<Result> &copyResult, bool force = false) override
        { return 0; };
    int CopyFile(const Uri &sourceUri, const Uri &destUri, const std::string &fileName,
    	Uri &newFileUri) override { return 0; };
    int Rename(const Uri &sourceFile, const std::string &displayName, Uri &newFile) override { return 0; };
    int ListFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount, const FileFilter &filter,
        std::vector<FileInfo> &fileInfoVec) override { return 0; };
    int ScanFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount, const FileFilter &filter,
        std::vector<FileInfo> &fileInfoVec) override { return 0; };
    int GetFileInfoFromUri(const Uri &selectFile, FileInfo &fileInfo) override { return 0; };
    int GetFileInfoFromRelativePath(const std::string &selectFile, FileInfo &fileInfo) override { return 0; };
    int GetRoots(std::vector<RootInfo> &rootInfoVec) override { return 0; };
    int Access(const Uri &uri, bool &isExist) override { return 0; };
    int Query(const Uri &uri, std::vector<std::string> &columns, std::vector<std::string> &results) override
        { return 0; };
    int StartWatcher(const Uri &uri) override { return 0; };
    int StopWatcher(const Uri &uri) override { return 0; };
    int MoveItem(const Uri &sourceFile, const Uri &targetParent, std::vector<Result> &moveResult,
                 bool force = false) override { return 0; };
    int MoveFile(const Uri &sourceFile, const Uri &targetParent, std::string &fileName, Uri &newFile) override
        { return 0; };
    int GetFileInfoNum(const std::string &sourceFileUri, const FileFilter &filter, bool recursion,
        uint32_t &counts) override { return 0; };
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_EXT_ABILITY_MOCK_H