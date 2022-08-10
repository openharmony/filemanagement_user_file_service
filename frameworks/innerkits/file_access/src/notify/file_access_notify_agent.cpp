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

#include "file_access_notify_agent.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace FileAccessFwk {
FileAccessNotifyAgent::FileAccessNotifyAgent(std::shared_ptr<INotifyCallback> &notifyCallback)
    : notifyCallback_(notifyCallback)
{
    HILOG_INFO("%{public}s, called start", __func__);
    HILOG_INFO("%{public}s, called end", __func__);
}

FileAccessNotifyAgent::~FileAccessNotifyAgent()
{
    HILOG_INFO("%{public}s, called start", __func__);
    HILOG_INFO("%{public}s, called end", __func__);
}

int FileAccessNotifyAgent::Notify(const NotifyMessage &message)
{
    HILOG_INFO("%{public}s, called start", __func__);
    int ret = ERR_OK;
    if (notifyCallback_ != nullptr) {
        ret = notifyCallback_->OnNotify(message);
    }
    HILOG_INFO("%{public}s, called end", __func__);
    return ret;
}
} // namespace FileAccessFwk
} // namespace OHOS