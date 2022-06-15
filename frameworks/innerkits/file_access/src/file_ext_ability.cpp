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

#include "file_ext_ability.h"

#include "ability_loader.h"
#include "connection_manager.h"
#include "extension_context.h"
#include "hilog_wrapper.h"
#include "js_file_ext_ability.h"
#include "runtime.h"

namespace OHOS {
namespace FileAccessFwk {
using namespace OHOS::AppExecFwk;

CreatorFunc FileExtAbility::creator_ = nullptr;
void FileExtAbility::SetCreator(const CreatorFunc& creator)
{
    creator_ = creator;
}

FileExtAbility* FileExtAbility::Create(const std::unique_ptr<Runtime>& runtime)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    if (!runtime) {
        return new FileExtAbility();
    }
    if (creator_) {
        return creator_(runtime);
    }
    HILOG_INFO("tag dsa FileExtAbility::Create runtime");
    switch (runtime->GetLanguage()) {
        case Runtime::Language::JS:
            HILOG_INFO("tag dsa Runtime::Language::JS --> JsFileExtAbility");
            return JsFileExtAbility::Create(runtime);

        default:
            HILOG_INFO("tag dsa default --> FileExtAbility");
            return new FileExtAbility();
    }
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
}

void FileExtAbility::Init(const std::shared_ptr<AbilityLocalRecord> &record,
    const std::shared_ptr<OHOSApplication> &application,
    std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    ExtensionBase<>::Init(record, application, handler, token);
    HILOG_INFO("tag dsa %{public}s end.", __func__);
}
} // namespace FileAccessFwk
} // namespace OHOS