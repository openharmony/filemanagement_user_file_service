/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#ifndef OHOS_APPEXECFWK_FILEACCESS_COMMON_H
#define OHOS_APPEXECFWK_FILEACCESS_COMMON_H

#include "ability.h"
#include "file_access_helper.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "napi_common.h"
#include "napi_common_util.h"
#include "want.h"

using Want = OHOS::AAFwk::Want;
using Ability = OHOS::AppExecFwk::Ability;
using AbilityStartSetting = OHOS::AppExecFwk::AbilityStartSetting;

namespace OHOS {
namespace AppExecFwk {

struct CBBase {
    CallbackInfo cbInfo;
    napi_async_work asyncWork;
    napi_deferred deferred;
    Ability *ability = nullptr;
    AbilityType abilityType = AbilityType::UNKNOWN;
    int errCode = 0;
};

struct FileAccessHelperCB {
    CBBase cbBase;
    napi_ref uri = nullptr;
    napi_value result = nullptr;
};

struct FileAccessHelperOpenFileCB {
    CBBase cbBase;
    FileAccessHelper *fileAccessHelper = nullptr;
    std::string uri;
    std::string mode;
    int result = 0;
    int execResult;
};

struct FileAccessHelperCreateFileCB {
    CBBase cbBase;
    FileAccessHelper *fileAccessHelper = nullptr;
    std::string parentUri;
    std::string name;
    std::string result;
    int execResult;
};

struct FileAccessHelperMkdirCB {
    CBBase cbBase;
    FileAccessHelper *fileAccessHelper = nullptr;
    std::string parentUri;
    std::string name;
    std::string result;
    int execResult;
};

struct FileAccessHelperDeleteCB {
    CBBase cbBase;
    FileAccessHelper *fileAccessHelper = nullptr;
    std::string selectFileUri;
    int result = 0;
    int execResult;
};

struct FileAccessHelperMoveCB {
    CBBase cbBase;
    FileAccessHelper *fileAccessHelper = nullptr;
    std::string sourceFileUri;
    std::string targetParentUri;
    std::string result;
    int execResult;
};

struct FileAccessHelperRenameCB {
    CBBase cbBase;
    FileAccessHelper *fileAccessHelper = nullptr;
    std::string sourceFileUri;
    std::string displayName;
    std::string result;
    int execResult;
};

struct FileAccessHelperCloseFileCB {
    CBBase cbBase;
    FileAccessHelper *fileAccessHelper = nullptr;
    int fd;
    std::string uri;
    int result;
    int execResult;
};

struct FileAccessHelperReleaseCB {
    CBBase cbBase;
    FileAccessHelper *fileAccessHelper = nullptr;
    bool result = false;
};
}
}
#endif