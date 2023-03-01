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

#include "file_access_ext_ability_module_loader.h"

#include <string>
#include <utility>

#include "file_access_ext_ability.h"

namespace OHOS {
namespace FileAccessFwk {

FileAccessExtAbilityModuleLoader::FileAccessExtAbilityModuleLoader() = default;
FileAccessExtAbilityModuleLoader::~FileAccessExtAbilityModuleLoader() = default;

Extension *FileAccessExtAbilityModuleLoader::Create(const std::unique_ptr<Runtime>& runtime) const
{
    return FileAccessExtAbility::Create(runtime);
}

extern "C" __attribute__((visibility("default"))) void* OHOS_EXTENSION_GetExtensionModule()
{
    return &FileAccessExtAbilityModuleLoader::GetInstance();
}

extern "C" __attribute__((visibility("default"))) void SetCreator(const CreatorFunc& creator)
{
    return FileAccessExtAbility::SetCreator(creator);
}

std::map<std::string, std::string> FileAccessExtAbilityModuleLoader::GetParams()
{
    std::map<std::string, std::string> params;
    const std::string FILE_ACCESS_TYPE = "12";
    params.insert(std::pair<std::string, std::string>("type", FILE_ACCESS_TYPE));
    params.insert(std::pair<std::string, std::string>("name", "FileAccessExtension"));
    return params;
}
} // namespace FileAccessFwk
} // namespace OHOS