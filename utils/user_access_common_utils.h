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

#ifndef USER_ACCESS_COMMON_UTILS_H
#define USER_ACCESS_COMMON_UTILS_H

#include "os_account_manager.h"
#include "parameter.h"

namespace OHOS {
namespace FileAccessFwk {
const std::string FULL_MOUNT_ENABLE_PARAMETER = "const.filemanager.full_mount.enable";

static bool IsFullMountEnable()
{
    char value[] = "false";
    int ret = GetParameter(FULL_MOUNT_ENABLE_PARAMETER.c_str(), "false", value, sizeof(value));
    if ((ret > 0) && (std::string(value) == "true")) {
        HILOG_INFO("Supporting all mounts");
        return true;
    }
    HILOG_INFO("Not supporting all mounts");
    return false;
}

static bool GetUserName(std::string &userName)
{
    userName = "currentUser";
    return true;
}

} // FileAccessFwk
} // OHOS
#endif //USER_ACCESS_COMMON_UTILS_H