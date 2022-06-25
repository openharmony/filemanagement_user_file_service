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

#include "file_access_ext_ability.h"

#include "ability_loader.h"
#include "connection_manager.h"
#include "extension_context.h"
#include "hilog_wrapper.h"
#include "js_file_access_ext_ability.h"
#include "runtime.h"

namespace OHOS {
namespace FileAccessFwk {
using namespace OHOS::AppExecFwk;

CreatorFunc FileAccessExtAbility::creator_ = nullptr;
void FileAccessExtAbility::SetCreator(const CreatorFunc& creator)
{
    creator_ = creator;
}

FileAccessExtAbility* FileAccessExtAbility::Create(const std::unique_ptr<Runtime>& runtime)
{
    if (!runtime) {
        return new FileAccessExtAbility();
    }
    if (creator_) {
        return creator_(runtime);
    }
    switch (runtime->GetLanguage()) {
        case Runtime::Language::JS:
            return JsFileAccessExtAbility::Create(runtime);

        default:
            return new FileAccessExtAbility();
    }
}

void FileAccessExtAbility::Init(const std::shared_ptr<AbilityLocalRecord> &record,
    const std::shared_ptr<OHOSApplication> &application,
    std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    ExtensionBase<>::Init(record, application, handler, token);
}
} // namespace FileAccessFwk
} // namespace OHOS