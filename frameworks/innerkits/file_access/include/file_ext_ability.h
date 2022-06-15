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

#ifndef FILE_EXT_ABILITY_H
#define FILE_EXT_ABILITY_H

#include "extension_base.h"

namespace OHOS {
namespace AbilityRuntime {
class Runtime;
}
namespace FileAccessFwk {
using namespace AbilityRuntime;
class FileExtAbility;
using CreatorFunc = std::function<FileExtAbility* (const std::unique_ptr<Runtime>& runtime)>;
class FileExtAbility : public ExtensionBase<> {
public:
    FileExtAbility() = default;
    virtual ~FileExtAbility() = default;

    virtual void Init(const std::shared_ptr<AbilityLocalRecord> &record,
        const std::shared_ptr<OHOSApplication> &application,
        std::shared_ptr<AbilityHandler> &handler,
        const sptr<IRemoteObject> &token) override;

    static FileExtAbility* Create(const std::unique_ptr<Runtime>& runtime);
    static void SetCreator(const CreatorFunc& creator);
private:
    static CreatorFunc creator_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_EXT_ABILITY_H