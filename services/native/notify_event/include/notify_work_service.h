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

#ifndef OHOS_FILEMGMT_CLOUD_DISK_NOTIFY_WORK_SERVICE_H
#define OHOS_FILEMGMT_CLOUD_DISK_NOTIFY_WORK_SERVICE_H

#include "cloud_disk_comm.h"
namespace OHOS::FileAccessFwk {
class NotifyWorkService {
public:
    enum class EventType {
        REGISTER,
        UNREGISTER,
        ACTIVE,
        INACTIVE,
        UPDATE,
        MAX_EVENT_TYPE
    };
    static NotifyWorkService &GetInstance();
    bool NotifySyncFolderEvent(const FileManagement::SyncFolderExt &syncFolderExt, EventType eventType);
private:
    NotifyWorkService() = default;
    ~NotifyWorkService() = default;
};
} // namespace OHOS::FileAccessFwk

#endif // OHOS_FILEMGMT_CLOUD_DISK_NOTIFY_WORK_SERVICE_H