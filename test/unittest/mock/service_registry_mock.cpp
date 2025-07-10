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

#include "iservice_registry.h"

#include "assistant.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager_mock.h"
#include "ipc_skeleton.h"
#include "iremote_object.h"

namespace OHOS {
using namespace OHOS::FileAccessFwk;
SystemAbilityManagerClient& SystemAbilityManagerClient::GetInstance()
{
    static auto instance = std::make_shared<SystemAbilityManagerClient>();
    return *instance;
}

sptr<ISystemAbilityManager> SystemAbilityManagerClient::GetSystemAbilityManager()
{
    if (Assistant::ins_ == nullptr) {
        return false;
    }
    if (Assistant::ins_->Bool() == false) {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(systemAbilityManagerLock_);
    if (systemAbilityManager_ != nullptr) {
        return systemAbilityManager_;
    }

    systemAbilityManager_ = sptr<ISystemAbilityManager>(new ISystemAbilityManagerMock());
    return systemAbilityManager_;
}

void SystemAbilityManagerClient::DestroySystemAbilityManagerObject()
{
    std::lock_guard<std::mutex> lock(systemAbilityManagerLock_);
    systemAbilityManager_.clear();
}
} // namespace OHOS