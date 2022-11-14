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

#include "external_file_access_fuzzer.h"

#include <cstdio>
#include <thread>
#include <unistd.h>

#include "accesstoken_kit.h"
#include "token_setproc.h"
#include "nativetoken_kit.h"
#include "file_access_framework_errno.h"
#include "file_access_helper.h"
#include "iservice_registry.h"

namespace OHOS {
using namespace std;
using namespace OHOS;
using namespace FileAccessFwk;

const int32_t WRITE_READ = 2;
const int ABILITY_ID = 5003;
shared_ptr<FileAccessHelper> g_fah = nullptr;
const int UID_TRANSFORM_TMP = 20000000;
const int UID_DEFAULT = 0;

void SetNativeToken()
{
    uint64_t tokenId;
    const char **perms = new const char *[1];
    perms[0] = "ohos.permission.FILE_ACCESS_MANAGER";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .aplStr = "system_core",
    };

    infoInstance.processName = "SetUpTestCase";
    tokenId = GetAccessTokenId(&infoInstance);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    SetSelfTokenID(tokenId);
    delete[] perms;
}

shared_ptr<FileAccessHelper> GetFileAccessHelper()
{
    if (g_fah != nullptr) {
        return g_fah;
    }
    SetNativeToken();
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObj = saManager->GetSystemAbility(ABILITY_ID);
    AAFwk::Want want;
    vector<AAFwk::Want> wantVec;
    setuid(UID_TRANSFORM_TMP);
    int ret = FileAccessHelper::GetRegisteredFileAccessExtAbilityInfo(wantVec);
    if (ret != OHOS::FileAccessFwk::ERR_OK) {
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
        return nullptr;
    }
    vector<AAFwk::Want> wants {want};
    g_fah = FileAccessHelper::Creator(remoteObj, wants);
    setuid(UID_DEFAULT);
    if (g_fah == nullptr) {
        return nullptr;
    }
    return g_fah;
}

bool CreatorFuzzTest(const uint8_t* data, size_t size)
{
    SetNativeToken();
    if ((data == nullptr) || (size <= 0)) {
        return false;
    }
    std::string bundleName(reinterpret_cast<const char*>(data), size);
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObj = saManager->GetSystemAbility(ABILITY_ID);
    AAFwk::Want want;
    want.SetElementName(bundleName, "FileExtensionAbility");
    vector<AAFwk::Want> wants {want};
    setuid(UID_TRANSFORM_TMP);
    shared_ptr<FileAccessHelper> helper = nullptr;
    helper = FileAccessHelper::Creator(remoteObj, wants);
    setuid(UID_DEFAULT);
    if (helper == nullptr) {
        return false;
    }
    helper->Release();
    return true;
}

bool AccessFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return false;
    }
    Uri uri(std::string(reinterpret_cast<const char*>(data), size));
    shared_ptr<FileAccessHelper> helper = GetFileAccessHelper();
    if (helper == nullptr) {
        return false;
    }
    bool isExist = false;
    int result = helper->Access(uri, isExist);
    if (isExist != true || result != OHOS::FileAccessFwk::ERR_OK) {
        return false;
    }
    return true;
}

bool OpenFileFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return false;
    }
    Uri uri(std::string(reinterpret_cast<const char*>(data), size));
    shared_ptr<FileAccessHelper> helper = GetFileAccessHelper();
    if (helper == nullptr) {
        return false;
    }
    int fd = -1;
    int result = 0;
    result = helper->OpenFile(uri, WRITE_READ, fd);
    if (result != OHOS::FileAccessFwk::ERR_OK) {
        return false;
    }
    close(fd);
    return true;
}

bool CreateFileFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return false;
    }
    shared_ptr<FileAccessHelper> helper = GetFileAccessHelper();
    if (helper == nullptr) {
        return false;
    }

    vector<RootInfo> info;
    int result = helper->GetRoots(info);
    if (result != OHOS::FileAccessFwk::ERR_OK) {
        return false;
    }
    for (size_t i = 0; i < info.size(); i++) {
        Uri parentUri(info[i].uri);
        Uri newFileUri("");
        result = helper->CreateFile(parentUri, "CreateFileFuzzTest", newFileUri);
        if (result != OHOS::FileAccessFwk::ERR_OK) {
            return false;
        }
        result = helper->Delete(newFileUri);
        if (result != OHOS::FileAccessFwk::ERR_OK) {
            return false;
        }
    }
    return true;
}

bool MkdirFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return false;
    }
    shared_ptr<FileAccessHelper> helper = GetFileAccessHelper();
    if (helper == nullptr) {
        return false;
    }

    vector<RootInfo> info;
    int result = helper->GetRoots(info);
    if (result != OHOS::FileAccessFwk::ERR_OK) {
        return false;
    }
    for (size_t i = 0; i < info.size(); i++) {
        Uri parentUri(info[i].uri);
        Uri newDirUri("");
        result = helper->Mkdir(parentUri, "MkdirFuzzTest", newDirUri);
        if (result != OHOS::FileAccessFwk::ERR_OK) {
            return false;
        }
        result = helper->Delete(newDirUri);
        if (result != OHOS::FileAccessFwk::ERR_OK) {
            return false;
        }
    }
    return true;
}

bool DeleteFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return false;
    }
    Uri uri(std::string(reinterpret_cast<const char*>(data), size));
    shared_ptr<FileAccessHelper> helper = GetFileAccessHelper();
    if (helper == nullptr) {
        return false;
    }
    int result = helper->Delete(uri);
    if (result != OHOS::FileAccessFwk::ERR_OK) {
        return false;
    }
    return true;
}

bool MoveFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return false;
    }
    shared_ptr<FileAccessHelper> helper = GetFileAccessHelper();
    if (helper == nullptr) {
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
        result = helper->Mkdir(parentUri, "test1", newDirUriTest1);
        if (result != OHOS::FileAccessFwk::ERR_OK) {
            return false;
        }
        result = helper->Mkdir(parentUri, "test2", newDirUriTest2);
        if (result != OHOS::FileAccessFwk::ERR_OK) {
            return false;
        }
        Uri testUri("");
        result = helper->CreateFile(newDirUriTest1, "test.txt", testUri);
        if (result != OHOS::FileAccessFwk::ERR_OK) {
            return false;
        }
        Uri testUri2("");
        result = helper->Move(testUri, newDirUriTest2, testUri2);
        if (result != OHOS::FileAccessFwk::ERR_OK) {
            return false;
        }
        result = helper->Delete(newDirUriTest1);
        if (result != OHOS::FileAccessFwk::ERR_OK) {
            return false;
        }
        result = helper->Delete(newDirUriTest2);
        if (result != OHOS::FileAccessFwk::ERR_OK) {
            return false;
        }
    }
    return true;
}

bool RenameFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return false;
    }
    shared_ptr<FileAccessHelper> helper = GetFileAccessHelper();
    if (helper == nullptr) {
        return false;
    }
    vector<RootInfo> info;
    int result = helper->GetRoots(info);
    if (result != OHOS::FileAccessFwk::ERR_OK) {
        return false;
    }
    for (size_t i = 0; i < info.size(); i++) {
        Uri parentUri(info[i].uri);
        Uri newDirUriTest("");
        result = helper->Mkdir(parentUri, "test", newDirUriTest);
        if (result != OHOS::FileAccessFwk::ERR_OK) {
            return false;
        }
        Uri renameUri("");
        result = helper->Rename(newDirUriTest, "testRename", renameUri);
        if (result != OHOS::FileAccessFwk::ERR_OK) {
            return false;
        }
        result = helper->Delete(renameUri);
        if (result != OHOS::FileAccessFwk::ERR_OK) {
            return false;
        }
    }
    return true;
}

bool ListFileFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return false;
    }
    shared_ptr<FileAccessHelper> helper = GetFileAccessHelper();
    if (helper == nullptr) {
        return false;
    }

    FileInfo fileInfo;
    fileInfo.uri = std::string(reinterpret_cast<const char*>(data), size);
    int64_t offset = 0;
    int64_t maxCount = 1000;
    std::vector<FileInfo> fileInfoVec;
    FileFilter filter;
    int result = helper->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    if (result != OHOS::FileAccessFwk::ERR_OK) {
        return false;
    }
    return true;
}

bool ScanFileFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return false;
    }
    shared_ptr<FileAccessHelper> helper = GetFileAccessHelper();
    if (helper == nullptr) {
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
        return false;
    }
    return true;
}

}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::CreatorFuzzTest(data, size);
    OHOS::AccessFuzzTest(data, size);
    OHOS::OpenFileFuzzTest(data, size);
    OHOS::MkdirFuzzTest(data, size);
    OHOS::CreateFileFuzzTest(data, size);
    OHOS::DeleteFuzzTest(data, size);
    OHOS::MoveFuzzTest(data, size);
    OHOS::RenameFuzzTest(data, size);
    OHOS::ListFileFuzzTest(data, size);
    OHOS::ScanFileFuzzTest(data, size);
    return 0;
}

