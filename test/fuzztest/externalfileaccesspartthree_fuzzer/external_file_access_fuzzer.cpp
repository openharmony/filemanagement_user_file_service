/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "external_file_access_fuzzer.h"

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

bool ListFileFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        HILOG_ERROR("parameter data is nullptr or parameter size <= 0.");
        return false;
    }
    shared_ptr<FileAccessHelper> helper = GetFileAccessHelper();
    if (helper == nullptr) {
        HILOG_ERROR("GetFileAccessHelper return nullptr.");
        return false;
    }

    FileInfo fileInfo;
    fileInfo.uri = std::string(reinterpret_cast<const char*>(data), size);
    int64_t offset = 0;
    SharedMemoryInfo memInfo;
    int result = SharedMemoryOperation::CreateSharedMemory("FileInfo List", DEFAULT_CAPACITY_200KB,
        memInfo);
    if (result != OHOS::FileAccessFwk::ERR_OK) {
        HILOG_ERROR("CreateSharedMemory failed. ret : %{public}d", result);
        return false;
    }
    FileFilter filter;
    result = helper->ListFile(fileInfo, offset, filter, memInfo);
    SharedMemoryOperation::DestroySharedMemory(memInfo);
    if (result != OHOS::FileAccessFwk::ERR_OK) {
        HILOG_ERROR("ListFile failed. ret : %{public}d", result);
        return false;
    }
    return true;
}

bool ScanFileFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        HILOG_ERROR("parameter data is nullptr or parameter size <= 0.");
        return false;
    }
    shared_ptr<FileAccessHelper> helper = GetFileAccessHelper();
    if (helper == nullptr) {
        HILOG_ERROR("GetFileAccessHelper return nullptr.");
        return false;
    }

    FileInfo fileInfo;
    fileInfo.uri = std::string(reinterpret_cast<const char*>(data), size);
    int64_t offset = 0;
    int64_t maxCount = 1000;
    std::vector<FileInfo> fileInfoVec;
    FileFilter filter;
    int result = helper->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    if (result != OHOS::FileAccessFwk::ERR_OK) {
        HILOG_ERROR("ScanFile failed. ret : %{public}d", result);
        return false;
    }
    return true;
}

bool GetFileInfoFromUriFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        HILOG_ERROR("parameter data is nullptr or parameter size <= 0.");
        return false;
    }
    Uri uri(std::string(reinterpret_cast<const char*>(data), size));
    shared_ptr<FileAccessHelper> helper = GetFileAccessHelper();
    if (helper == nullptr) {
        HILOG_ERROR("GetFileAccessHelper return nullptr.");
        return false;
    }
    FileInfo fileinfo;
    int result = helper->GetFileInfoFromUri(uri, fileinfo);
    if (result != OHOS::FileAccessFwk::ERR_OK) {
        HILOG_ERROR("GetFileInfoFromUri failed. ret : %{public}d", result);
        return false;
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
    OHOS::ListFileFuzzTest(data, size);
    OHOS::ScanFileFuzzTest(data, size);
    OHOS::GetFileInfoFromUriFuzzTest(data, size);
    return 0;
}
