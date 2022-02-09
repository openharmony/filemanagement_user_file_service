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
#ifndef STORAGE_FILE_SYS_EVENT_RECEIVER_H
#define STORAGE_FILE_SYS_EVENT_RECEIVER_H

#include <string>
#include <unordered_map>

#include "common_event_manager.h"
#include "common_event_subscribe_info.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "ext_storage_status.h"
#include "matching_skills.h"

namespace OHOS {
namespace FileManagerService {
/**
 * @class ExtStorageSubscriber
 * Receive system common event.
 */
class ExtStorageSubscriber : public EventFwk::CommonEventSubscriber {
public:
    ExtStorageSubscriber() = default;
    explicit ExtStorageSubscriber(const EventFwk::CommonEventSubscribeInfo &subscriberInfo);
    static bool Subscriber(void);
    virtual ~ExtStorageSubscriber() = default;
    /**
     * @brief System common event receiver.
     * @param eventData Common event data.
     */
    virtual void OnReceiveEvent(const EventFwk::CommonEventData &eventData) override;

    bool CheckMountPoint(const std::string &path);

    std::unordered_map<std::string, ExtStorageStatus> mountStatus;
};
}  // namespace FileManagerService
}  // namespace OHOS

#endif // STORAGE_FILE_SYS_EVENT_RECEIVER_H