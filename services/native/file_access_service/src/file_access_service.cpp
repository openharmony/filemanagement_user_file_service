/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "file_access_service.h"

#include <unistd.h>

#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace FileAccessFwk {
sptr<FileAccessService> FileAccessService::instance_;
std::mutex FileAccessService::mutex_;
sptr<FileAccessService> FileAccessService::GetInstance()
{
    if (instance_ != nullptr) {
        return instance_;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (instance_ == nullptr) {
        instance_ = new FileAccessService();
        if (instance_ == nullptr) {
            HILOG_ERROR("GetInstance nullptr");
            return instance_;
        }
    }
    return instance_;
}

FileAccessService::FileAccessService() : SystemAbility(FILE_ACCESS_SERVICE_ID, false) {}

void FileAccessService::OnStart()
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "OnStart");
    if (!Publish(FileAccessService::GetInstance())) {
        HILOG_ERROR("OnStart register to system ability manager failed");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return;
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    ready_ = true;
    return;
}

void FileAccessService::OnStop()
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "OnStop");
    if (!ready_) {
        HILOG_ERROR("OnStop is not ready, nothing to do");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return;
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    ready_ = false;
}

int32_t FileAccessService::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    return ERR_OK;
}

bool FileAccessService::IsServiceReady() const
{
    return ready_;
}

int32_t FileAccessService::RegisterNotify(Uri uri, const sptr<IFileAccessObserver> &observer,
    bool notifyForDescendants)
{
    return ERR_OK;
}

int32_t FileAccessService::UnregisterNotify(Uri uri, const sptr<IFileAccessObserver> &observer)
{
    return ERR_OK;
}
} // namespace FileAccessFwk
} // namespace OHOS