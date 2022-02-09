/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "ext_storage_subscriber.h"

#include <cinttypes>

#include "app_log_wrapper.h"
#include "appexecfwk_errors.h"
#include "bundle_info.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "log.h"
#include "string_wrapper.h"
#include "int_wrapper.h"
#include "want.h"

using namespace OHOS::AAFwk;
namespace OHOS {
namespace FileManagerService {
/**
 * @brief Receiver Constructor.
 * @param subscriberInfo Subscriber info.
 */
ExtStorageSubscriber::ExtStorageSubscriber(const EventFwk::CommonEventSubscribeInfo &subscriberInfo)
    : EventFwk::CommonEventSubscriber(subscriberInfo)
{}

std::shared_ptr<ExtStorageSubscriber> ExtStorageSubscriber_ = nullptr;
bool ExtStorageSubscriber::Subscriber(void)
{
    if (ExtStorageSubscriber_ == nullptr) {
        EventFwk::MatchingSkills matchingSkills;
        matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
        matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_DISK_REMOVED);
        matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_DISK_UNMOUNTED);
        matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_DISK_MOUNTED);

        EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
        ExtStorageSubscriber_ = std::make_shared<ExtStorageSubscriber>(subscribeInfo);
        EventFwk::CommonEventManager::SubscribeCommonEvent(ExtStorageSubscriber_);
    }
    return true;
}

/**
 * @brief Receive common event.
 * @param eventData Common event data.
 */
void ExtStorageSubscriber::OnReceiveEvent(const EventFwk::CommonEventData &eventData)
{
    const AAFwk::Want& want = eventData.GetWant();
    std::string action = want.GetAction();
    DEBUG_LOG("%{public}s, action:%{public}s.", __func__, action.c_str());

    const AAFwk::WantParams wantParams = want.GetParams();
    std::string id = AAFwk::String::Unbox(AAFwk::IString::Query(wantParams.GetParam("id")));
    std::string diskId = AAFwk::String::Unbox(AAFwk::IString::Query(wantParams.GetParam("diskId")));
    int32_t notifyCode = AAFwk::Integer::Unbox(AAFwk::IInteger::Query(wantParams.GetParam("notifyCode")));
    DEBUG_LOG("%{public}s, id:%{public}s.", __func__, id.c_str());
    DEBUG_LOG("%{public}s, diskId:%{public}s.", __func__, diskId.c_str());
    DEBUG_LOG("%{public}s, notifyCode:%{public}d.", __func__, notifyCode);
  
    if (action == EventFwk::CommonEventSupport::COMMON_EVENT_DISK_MOUNTED) {
        std::string fsUuid = AAFwk::String::Unbox(AAFwk::IString::Query(wantParams.GetParam("fsUuid")));
        std::string path = AAFwk::String::Unbox(AAFwk::IString::Query(wantParams.GetParam("path")));
        
        DEBUG_LOG("%{public}s, id:%{public}s, fsUuid:%{public}s, path:%{public}s.",
            __func__, id.c_str(), fsUuid.c_str(), path.c_str());

        ExtStorageStatus extStatus(id, diskId, fsUuid, path, notifyCode);
        mountStatus.insert(std::pair<std::string, ExtStorageStatus>(path, extStatus));
    }
}
}  // namespace FileManagerService
}  // namespace OHOS