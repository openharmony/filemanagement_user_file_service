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

#include "file_manager_service.h"

#include "ext_storage/ext_storage_subscriber.h"
#include "iservice_registry.h"
#include "log.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace FileManagerService {
REGISTER_SYSTEM_ABILITY_BY_ID(FileManagerService, FILE_MANAGER_SERVICE_ID, true);

FileManagerService::FileManagerService(int32_t systemAbilityId, bool runOnCreate)
    : SystemAbility(systemAbilityId, runOnCreate) {}
void FileManagerService::OnDump() {}

void FileManagerService::OnStart()
{
    DEBUG_LOG("FileManagerService OnStart");
    bool res = Publish(this);
    if (!res) {
        ERR_LOG("FileManagerService OnStart invalid");
    }
}

void FileManagerService::OnStop()
{
    DEBUG_LOG("FileManagerService OnStop");
}
} // namespace FileManagerService
} // namespace OHOS
