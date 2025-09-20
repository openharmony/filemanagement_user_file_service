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

#ifndef BUNDLE_OBSERVER_H
#define BUNDLE_OBSERVER_H

#include <memory>
#include <mutex>
#include <vector>

#include "bundlemgr/bundle_mgr_interface.h"
#include "common_event_subscribe_info.h"
#include "common_event_subscriber.h"
#include "system_ability_ondemand_reason.h"

namespace OHOS {
namespace FileAccessFwk {
using namespace std;
class FileAccessService;
 
class BundleObserver final : public EventFwk::CommonEventSubscriber {
public:
    BundleObserver() = default;
    ~BundleObserver();
 
    int32_t HandleBundleBroadcast();
    int32_t ClearSyncFolder(const string &bundleName, int32_t userId, int32_t appIndex);
    string GetValueByKey(const std::map<string, string> &map, const string &key);
    int32_t GetNumberFromString(const string &str, const int32_t defaultValue);
 
    explicit BundleObserver(const EventFwk::CommonEventSubscribeInfo &subscribeInfo,
        wptr<FileAccessService> fileAccessSvc);
    virtual void OnReceiveEvent(const EventFwk::CommonEventData &data) override;
private:
    wptr<FileAccessService> fileAccessSvc_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif