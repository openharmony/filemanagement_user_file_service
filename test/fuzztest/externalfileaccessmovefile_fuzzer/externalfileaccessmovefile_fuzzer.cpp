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

#include "externalfileaccessmovefile_fuzzer.h"

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
shared_ptr<FileAccessHelper> g_fah = nullptr;
const int UID_TRANSFORM_TMP = 20000000;
const int UID_DEFAULT = 0;
shared_ptr<FileAccessHelper> GetFileAccessHelper()
{
    if (g_fah != nullptr) {
        return g_fah;
    }
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        return nullptr;
    }
    auto remoteObj = saManager->GetSystemAbility(ABILITY_ID);
    AAFwk::Want want;
    vector<AAFwk::Want> wantVec;
    setuid(UID_TRANSFORM_TMP);
    int ret = FileAccessHelper::GetRegisteredFileAccessExtAbilityInfo(wantVec);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
        HILOG_ERROR("GetRegisteredFileAccessExtAbilityInfo failed.");
        return nullptr;
    }
    bool sus = false;
    for (size_t i = 0; i < wantVec.size(); i++) {
        auto element = wantVec[i].GetElement();
        if (element.GetBundleName() == "com.ohos.UserFile.ExternalFileManager" &&
            element.GetAbilityName() == "FileExtensionAbility") {
            want = wantVec[i];
            sus = true;
            break;
        }
    }
    if (!sus) {
        HILOG_ERROR("not found bundleName.");
        return nullptr;
    }
    vector<AAFwk::Want> wants {want};
    g_fah = FileAccessHelper::Creator(remoteObj, wants);
    setuid(UID_DEFAULT);
    if (g_fah == nullptr) {
        HILOG_ERROR("creator fileAccessHelper return nullptr.");
        return nullptr;
    }
    return g_fah;
}

bool CheckDataAndHelper(const uint8_t* data, size_t size, shared_ptr<FileAccessHelper>& helper)
{
    (void)data;
    helper = GetFileAccessHelper();
    if (helper == nullptr) {
        HILOG_ERROR("GetFileAccessHelper return nullptr.");
        return false;
    }
    return true;
}

bool MoveFuzzTest(const uint8_t* data, size_t size)
{
    shared_ptr<FileAccessHelper> helper;
    if (!CheckDataAndHelper(data, size, helper)) {
        return false;
    }
    vector<RootInfo> info;
    int result = helper->GetRoots(info);
    if (result != OHOS::FileAccessFwk::ERR_OK) {
        return false;
    }
    for (size_t i = 0; i < info.size(); i++) {
        Uri parentUri(info[i].uri);
        Uri newDirUriTest1("");
        Uri newDirUriTest2("");
        size_t len = size >> 2;
        std::string test1(reinterpret_cast<const char*>(data), len);
        std::string test2(reinterpret_cast<const char*>(data + len), len);
        int result1 = helper->Mkdir(parentUri, test1, newDirUriTest1);
        int result2 = helper->Mkdir(parentUri, test2, newDirUriTest2);
        if (result1 != OHOS::FileAccessFwk::ERR_OK || result2 != OHOS::FileAccessFwk::ERR_OK) {
            HILOG_ERROR("Mkdir failed. ret : %{public}d, %{public}d", result1, result2);
            return false;
        }
        Uri testUri("");
        std::string test(reinterpret_cast<const char*>(data + len + len), len);
        result = helper->CreateFile(newDirUriTest1, test, testUri);
        if (result != OHOS::FileAccessFwk::ERR_OK) {
            HILOG_ERROR("CreateFile failed. ret : %{public}d", result);
            return false;
        }
        Uri testUri2("");
        result = helper->Move(testUri, newDirUriTest2, testUri2);
        if (result != OHOS::FileAccessFwk::ERR_OK) {
            HILOG_ERROR("Move failed. ret : %{public}d", result);
            return false;
        }
        result1 = helper->Delete(newDirUriTest1);
        result2 = helper->Delete(newDirUriTest2);
        if (result1 != OHOS::FileAccessFwk::ERR_OK || result2 != OHOS::FileAccessFwk::ERR_OK) {
            HILOG_ERROR("Delete failed. ret : %{public}d, %{public}d", result1, result2);
            return false;
        }
    }
    return true;
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::UserFileServiceTokenMock tokenMock;
    tokenMock.SetFileManagerToken();
    /* Run your code on data */
    OHOS::MoveFuzzTest(data, size);
    return 0;
}
