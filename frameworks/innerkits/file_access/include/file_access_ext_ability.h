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

#ifndef FRAMEWORK_INNERKITS_FILEACCESS_INCLUDE_FILE_ACCESS_EXT_ABILITY_H
#define FRAMEWORK_INNERKITS_FILEACCESS_INCLUDE_FILE_ACCESS_EXT_ABILITY_H

#include "extension_base.h"
#include "file_access_extension_info.h"

namespace OHOS {
namespace AbilityRuntime {
class Runtime;
}
namespace FileAccessFwk {
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

    virtual int OpenFile(const Uri &uri, const int flags);
    virtual int CreateFile(const Uri &parent, const std::string &displayName,  Uri &newFile);
    virtual int Mkdir(const Uri &parent, const std::string &displayName, Uri &newFile);
    virtual int Delete(const Uri &sourceFile);
    virtual int Move(const Uri &sourceFile, const Uri &targetParent, Uri &newFile);
    virtual int Rename(const Uri &sourceFile, const std::string &displayName, Uri &newFile);

    virtual std::vector<FileInfo> ListFile(const Uri &sourceFile);
    virtual std::vector<DeviceInfo> GetRoots();
    static void SetCreator(const CreatorFunc& creator);
private:
    static CreatorFunc creator_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FRAMEWORK_INNERKITS_FILEACCESS_INCLUDE_FILE_ACCESS_EXT_ABILITY_H