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

#ifndef FILE_ACCESS_EXT_ABILITY_H
#define FILE_ACCESS_EXT_ABILITY_H

#include "extension_base.h"
#include "file_access_extension_info.h"
#include "file_access_observer_common.h"
#include "file_filter.h"
#include "image_source.h"

namespace OHOS {
namespace AbilityRuntime {
class Runtime;
}
namespace FileAccessFwk {
using namespace Media;
using namespace AbilityRuntime;
class FileAccessExtAbility;
using CreatorFunc = std::function<FileAccessExtAbility* (const std::unique_ptr<Runtime>& runtime)>;
class FileAccessExtAbility : public ExtensionBase<> {
public:
    FileAccessExtAbility() = default;
    virtual ~FileAccessExtAbility() = default;

    virtual void Init(const std::shared_ptr<AbilityLocalRecord> &record,
        const std::shared_ptr<OHOSApplication> &application,
        std::shared_ptr<AbilityHandler> &handler,
        const sptr<IRemoteObject> &token) override;

    static FileAccessExtAbility* Create(const std::unique_ptr<Runtime>& runtime);

    virtual int OpenFile(const Uri &uri, const int flags, int &fd);
    virtual int CreateFile(const Uri &parent, const std::string &displayName, Uri &newFile);
    virtual int Mkdir(const Uri &parent, const std::string &displayName, Uri &newFile);
    virtual int Delete(const Uri &sourceFile);
    virtual int Move(const Uri &sourceFile, const Uri &targetParent, Uri &newFile);
    virtual int Copy(const Uri &sourceUri, const Uri &destUri, std::vector<Result> &copyResult, bool force = false);
    virtual int Rename(const Uri &sourceFile, const std::string &displayName, Uri &newFile);
    virtual int ListFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
        const FileFilter &filter, std::vector<FileInfo> &fileInfoVec);
    virtual int ScanFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
        const FileFilter &filter, std::vector<FileInfo> &fileInfoVec);
    virtual int GetThumbnail(const Uri &uri, const Size &size, std::unique_ptr<PixelMap> &pixelMap);
    virtual int Query(const Uri &uri, std::vector<std::string> &columns, std::vector<std::string> &results);
    virtual int GetFileInfoFromUri(const Uri &selectFile, FileInfo &fileInfo);
    virtual int GetFileInfoFromRelativePath(const std::string &selectFile, FileInfo &fileInfo);
    virtual int GetRoots(std::vector<RootInfo> &rootInfoVec);
    virtual int Access(const Uri &uri, bool &isExist);
    static void SetCreator(const CreatorFunc& creator);
    virtual int StartWatcher(const Uri &uri);
    virtual int StopWatcher(const Uri &uri, bool isUnregisterAll);
    virtual int MoveItem(const Uri &sourceFile, const Uri &targetParent, std::vector<Result> &moveResult,
                         bool force = false);
    virtual int MoveFile(const Uri &sourceFile, const Uri &targetParent, std::string &fileName, Uri &newFile);
private:
    static CreatorFunc creator_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_EXT_ABILITY_H