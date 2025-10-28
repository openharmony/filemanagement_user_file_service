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

#include "external_file_access_creator_fuzzer.h"

#include <cstdio>
#include <thread>
#include <unistd.h>

#include "accesstoken_kit.h"
#include "token_setproc.h"
#include "nativetoken_kit.h"
#include "file_access_framework_errno.h"
#include "file_access_helper.h"
#include "file_info_shared_memory.h"
#include "iservice_registry.h"
#include "hilog_wrapper.h"
#include "user_file_service_token_mock.h"
namespace OHOS {
using namespace std;
using namespace OHOS;
using namespace FileAccessFwk;

const int ABILITY_ID = 5003;
const int UID_TRANSFORM_TMP = 20000000;
const int UID_DEFAULT = 0;

bool CreatorFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        HILOG_ERROR("parameter data is nullptr or parameter size <= 0.");
        return false;
    }
    std::string bundleName(reinterpret_cast<const char*>(data), size);
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        return false;
    }
    auto remoteObj = saManager->GetSystemAbility(ABILITY_ID);
    AAFwk::Want want;
    want.SetElementName(bundleName, "FileExtensionAbility");
    vector<AAFwk::Want> wants {want};
    setuid(UID_TRANSFORM_TMP);
    shared_ptr<FileAccessHelper> helper = nullptr;
    helper = FileAccessHelper::Creator(remoteObj, wants);
    setuid(UID_DEFAULT);
    if (helper == nullptr) {
        HILOG_ERROR("creator return nullptr.");
        return false;
    }
    helper->Release();
    return true;
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::UserFileServiceTokenMock tokenMock;
    tokenMock.SetFileManagerToken();
    /* Run your code on data */
    OHOS::CreatorFuzzTest(data, size);
    return 0;
}
