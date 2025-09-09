/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "notify_work_service.h"

#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "hilog_wrapper.h"
#include "want.h"
#include "want_params.h"

namespace OHOS::FileAccessFwk {
constexpr const char *NOTIFY_EVENT_TYPES[] = {
    "REGISTER",
    "UNREGISTER",
    "ACTIVE",
    "INACTIVE",
    "UPDATE"
};

NotifyWorkService &NotifyWorkService::GetInstance()
{
    static NotifyWorkService instance;
    return instance;
}

bool NotifyWorkService::NotifySyncFolderEvent(const FileManagement::SyncFolderExt &rootInfoExt,
    NotifyWorkService::EventType eventType)
{
    AAFwk::Want want;
    HILOG_INFO("Start publish event, bundleName is: %{public}s", rootInfoExt.bundleName_.c_str());
    want.SetBundle(rootInfoExt.bundleName_);
    want.SetParam("bundleName", rootInfoExt.bundleName_);
    if (eventType < EventType::REGISTER || eventType >= EventType::MAX_EVENT_TYPE) {
        HILOG_ERROR("EventType is invalid: %{public}d", static_cast<int>(eventType));
        return false;
    }
    want.SetParam("eventType", NOTIFY_EVENT_TYPES[static_cast<int>(eventType)]);
    want.SetParam("path", rootInfoExt.path_);
    want.SetParam("state", static_cast<int>(rootInfoExt.state_));
    if (rootInfoExt.displayNameResId_ != 0) {
        want.SetParam("displayNameResId", std::to_string(rootInfoExt.displayNameResId_));
    }
    if (!rootInfoExt.displayName_.empty()) {
        want.SetParam("displayName", rootInfoExt.displayName_);
    }
    want.SetAction("usual.event.CLOUD_DISK_STATE_CHANGED");
    EventFwk::CommonEventData commonData {want};
    HILOG_INFO("End publish event, bundleName is: %{public}s, eventType: %{public}s",
        rootInfoExt.bundleName_.c_str(), NOTIFY_EVENT_TYPES[static_cast<int>(eventType)]);
    return EventFwk::CommonEventManager::PublishCommonEvent(commonData);
}
}