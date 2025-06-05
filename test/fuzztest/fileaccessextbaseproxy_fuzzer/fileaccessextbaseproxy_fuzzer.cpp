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
#include "fileaccessextbaseproxy_fuzzer.h"

#include <string>
#include <vector>

#include "accesstoken_kit.h"
#include "file_access_helper.h"
#include "file_access_ext_base_proxy.h"
#include "file_info_shared_memory.h"
#include "iservice_registry.h"
#include "token_setproc.h"
#include "nativetoken_kit.h"

namespace OHOS {
using namespace std;
using namespace FileAccessFwk;

const int ABILITY_ID = 5003;
shared_ptr<FileAccessHelper> g_fah = nullptr;
const int UID_TRANSFORM_TMP = 20000000;
const int UID_DEFAULT = 0;

template <class T>
T TypeCast(const uint8_t *data, int *pos = nullptr)
{
    if (pos) {
        *pos += sizeof(T);
    }
    return *(reinterpret_cast<const T *>(data));
}

void SetNativeToken()
{
    uint64_t tokenId;
    const char *perms[] = {
        "ohos.permission.FILE_ACCESS_MANAGER",
        "ohos.permission.GET_BUNDLE_INFO_PRIVILEGED",
        "ohos.permission.CONNECT_FILE_ACCESS_EXTENSION"
    };
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 3,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .aplStr = "system_core",
    };

    infoInstance.processName = "SetUpTestCase";
    tokenId = GetAccessTokenId(&infoInstance);
    const uint64_t systemAppMask = (static_cast<uint64_t>(1) << 32);
    tokenId |= systemAppMask;
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

shared_ptr<FileAccessHelper> GetFileAccessHelper()
{
    if (g_fah != nullptr) {
        return g_fah;
    }
    SetNativeToken();
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
        printf("GetRegisteredFileAccessExtAbilityInfo failed.");
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
        printf("not found bundleName.");
        return nullptr;
    }
    vector<AAFwk::Want> wants {want};
    g_fah = FileAccessHelper::Creator(remoteObj, wants);
    setuid(UID_DEFAULT);
    if (g_fah == nullptr) {
        printf("creator fileAccessHelper return nullptr.");
        return nullptr;
    }
    return g_fah;
}

bool OpenFileFuzzTest(sptr<IFileAccessExtBase> proxy, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t) + sizeof(int)) {
        return true;
    }

    int pos = 0;
    int32_t flags = TypeCast<int32_t>(data, &pos);
    int fd = TypeCast<int>(data + pos, &pos);
    Urie uri(string(reinterpret_cast<const char *>(data + pos), size - sizeof(int32_t) - sizeof(int)));
    proxy->OpenFile(uri, flags, fd);
    return true;
}

bool CreateFileFuzzTest(sptr<IFileAccessExtBase> proxy, const uint8_t *data, size_t size)
{
    int len = size / 3;
    Urie parent(string(reinterpret_cast<const char *>(data), len));
    string displayName(string(reinterpret_cast<const char *>(data + len), len));
    Urie newFile(string(reinterpret_cast<const char *>(data + len + len), len));
    proxy->CreateFile(parent, displayName, newFile);
    return true;
}

bool MkdirFuzzTest(sptr<IFileAccessExtBase> proxy, const uint8_t *data, size_t size)
{
    int len = size / 3;
    Urie parent(string(reinterpret_cast<const char *>(data), len));
    string displayName(string(reinterpret_cast<const char *>(data + len), len));
    Urie newFile(string(reinterpret_cast<const char *>(data + len + len), len));
    proxy->Mkdir(parent, displayName, newFile);
    return true;
}

bool DeleteFuzzTest(sptr<IFileAccessExtBase> proxy, const uint8_t *data, size_t size)
{
    Urie sourceFile(string(reinterpret_cast<const char *>(data), size));
    proxy->Delete(sourceFile);
    return true;
}

bool MoveFuzzTest(sptr<IFileAccessExtBase> proxy, const uint8_t *data, size_t size)
{
    int len = size / 3;
    Urie sourceFile(string(reinterpret_cast<const char *>(data), len));
    string targetParent(string(reinterpret_cast<const char *>(data + len), len));
    Urie newFile(string(reinterpret_cast<const char *>(data + len + len), len));
    proxy->Move(sourceFile, targetParent, newFile);
    return true;
}

bool CopyFuzzTest(sptr<IFileAccessExtBase> proxy, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(bool)) {
        return true;
    }

    vector<Result> copyResult;
    int32_t retCode = 0;
    int pos = 0;
    bool force = TypeCast<bool>(data, &pos);
    int len = (size - pos) / 2;
    Urie sourceUri(string(reinterpret_cast<const char *>(data + pos), len));
    Urie destUri(string(reinterpret_cast<const char *>(data + pos + len), len));

    proxy->Copy(sourceUri, destUri, copyResult, retCode, force);
    return true;
}

bool CopyFileFuzzTest(sptr<IFileAccessExtBase> proxy, const uint8_t *data, size_t size)
{
    int len = size / 3;
    Urie sourceUri(string(reinterpret_cast<const char *>(data), len));
    Urie destUri(string(reinterpret_cast<const char *>(data + len), len));
    string fileName(string(reinterpret_cast<const char *>(data + len + len), len));
    Urie newFileUri;
    proxy->CopyFile(sourceUri, destUri, fileName, newFileUri);
    return true;
}

bool RenameFuzzTest(sptr<IFileAccessExtBase> proxy, const uint8_t *data, size_t size)
{
    int len = size / 2;
    Urie sourceFile(string(reinterpret_cast<const char *>(data), len));
    string displayName(string(reinterpret_cast<const char *>(data + len), len));
    Urie newFile;
    proxy->Rename(sourceFile, displayName, newFile);
    return true;
}

bool ListFileFuzzTest(sptr<IFileAccessExtBase> proxy, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int64_t)) {
        return true;
    }

    int pos = 0;
    int64_t offset = TypeCast<int64_t>(data, &pos);

    FileInfo fileInfo;
    fileInfo.uri = std::string(reinterpret_cast<const char*>(data + pos), size - pos);
    SharedMemoryInfo memInfo;
    int result = SharedMemoryOperation::CreateSharedMemory("FileInfo List", DEFAULT_CAPACITY_200KB, memInfo);
    if (result != OHOS::FileAccessFwk::ERR_OK) {
        printf("CreateSharedMemory failed. ret : %d", result);
        return false;
    }
    FileFilter filter;
    proxy->ListFile(fileInfo, offset, filter, memInfo);
    SharedMemoryOperation::DestroySharedMemory(memInfo);
    return true;
}

bool ScanFileFuzzTest(sptr<IFileAccessExtBase> proxy, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int64_t) + sizeof(int64_t)) {
        return true;
    }

    int pos = 0;
    int64_t offset = TypeCast<int64_t>(data, &pos);
    int64_t maxCount = TypeCast<int64_t>(data + pos, &pos);

    FileInfo fileInfo;
    fileInfo.uri = std::string(reinterpret_cast<const char*>(data + pos), size - pos);
    std::vector<FileInfo> fileInfoVec;
    FileFilter filter;
    proxy->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
    return true;
}

bool QueryFuzzTest(sptr<IFileAccessExtBase> proxy, const uint8_t *data, size_t size)
{
    int len = size / 4;
    Urie uri(string(reinterpret_cast<const char *>(data), len));
    vector<string> columns {
        string(reinterpret_cast<const char *>(data + len), len),
        string(reinterpret_cast<const char *>(data + len + len), len),
        string(reinterpret_cast<const char *>(data + len + len + len), len)
    };
    vector<string> results;
    proxy->Query(uri, columns, results);
    return true;
}

bool GetFileInfoFromUriFuzzTest(sptr<IFileAccessExtBase> proxy, const uint8_t *data, size_t size)
{
    Urie selectFile(string(reinterpret_cast<const char *>(data), size));
    FileInfo fileInfo;
    proxy->GetFileInfoFromUri(selectFile, fileInfo);
    return true;
}

bool GetRootsFuzzTest(sptr<IFileAccessExtBase> proxy, const uint8_t *data, size_t size)
{
    (void)data;
    vector<RootInfo> rootInfoVec;
    proxy->GetRoots(rootInfoVec);
    return true;
}

bool AccessFuzzTest(sptr<IFileAccessExtBase> proxy, const uint8_t *data, size_t size)
{
    Urie uri(string(reinterpret_cast<const char *>(data), size));
    bool isExist = false;
    proxy->Access(uri, isExist);
    return true;
}

bool StartWatcherFuzzTest(sptr<IFileAccessExtBase> proxy, const uint8_t *data, size_t size)
{
    Urie uri(string(reinterpret_cast<const char *>(data), size));
    proxy->StartWatcher(uri);
    return true;
}

bool StopWatcherFuzzTest(sptr<IFileAccessExtBase> proxy, const uint8_t *data, size_t size)
{
    Urie uri(string(reinterpret_cast<const char *>(data), size));
    proxy->StopWatcher(uri);
    return true;
}

bool MoveItemFuzzTest(sptr<IFileAccessExtBase> proxy, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(bool)) {
        return true;
    }

    vector<Result> moveResult;
    int32_t retCode = 0;
    int pos = 0;
    bool force = TypeCast<bool>(data, &pos);
    int len = (size - pos) / 2;
    Urie sourceFile(string(reinterpret_cast<const char *>(data + pos), len));
    Urie targetParent(string(reinterpret_cast<const char *>(data + pos + len), len));

    proxy->MoveItem(sourceFile, targetParent, moveResult, retCode, force);
    return true;
}

bool MoveFileFuzzTest(sptr<IFileAccessExtBase> proxy, const uint8_t *data, size_t size)
{
    int len = size / 3;
    Urie sourceFile(string(reinterpret_cast<const char *>(data), len));
    Urie targetParent(string(reinterpret_cast<const char *>(data + len), len));
    string fileName(string(reinterpret_cast<const char *>(data + len + len), len));
    Urie newFile;
    proxy->MoveFile(sourceFile, targetParent, fileName, newFile);
    return true;
}

bool UrieFuzzTest(const uint8_t *data, size_t size)
{
    int len = size / 2;
    Urie uri(string(reinterpret_cast<const char *>(data), len));
    Urie other(string(reinterpret_cast<const char *>(data + len), len));

    uri.uriString_ = string(reinterpret_cast<const char *>(data), len);
    uri.GetScheme();
    uri.GetSchemeSpecificPart();
    uri.GetAuthority();
    uri.GetHost();
    uri.GetPort();
    uri.GetUserInfo();
    uri.GetQuery();
    uri.GetPath();
    uri.GetFragment();
    uri.IsHierarchical();
    uri.IsAbsolute();
    uri.IsRelative();
    uri.ToString();
    uri.CheckScheme();
    uri.ParseScheme();
    uri.ParseSsp();
    uri.ParseAuthority();
    uri.ParseUserInfo();
    uri.ParseHost();
    uri.ParsePort();
    uri.ParsePath();
    uri.ParsePath(NOT_FOUND);
    uri.ParseQuery();
    uri.ParseFragment();
    uri.FindSchemeSeparator();
    uri.FindFragmentSeparator();
    uri.Equals(other);
    uri.CompareTo(other);
    vector<std::string> segments;
    uri.GetPathSegments(segments);
    Parcel parcel;
    uri.Marshalling(parcel);
    uri.Unmarshalling(parcel);
    return (uri == other);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    auto helper = OHOS::GetFileAccessHelper();
    if (helper == nullptr) {
        printf("helper is nullptr.");
        return false;
    }
    auto proxy = helper->GetProxyByBundleName(OHOS::EXTERNAL_BNUDLE_NAME);
    if (proxy == nullptr) {
        printf("get proxy failed.");
        return 0;
    }

    OHOS::OpenFileFuzzTest(proxy, data, size);
    OHOS::CreateFileFuzzTest(proxy, data, size);
    OHOS::MkdirFuzzTest(proxy, data, size);
    OHOS::DeleteFuzzTest(proxy, data, size);
    OHOS::MoveFuzzTest(proxy, data, size);
    OHOS::CopyFuzzTest(proxy, data, size);
    OHOS::CopyFileFuzzTest(proxy, data, size);
    OHOS::RenameFuzzTest(proxy, data, size);
    OHOS::ListFileFuzzTest(proxy, data, size);
    OHOS::ScanFileFuzzTest(proxy, data, size);
    OHOS::QueryFuzzTest(proxy, data, size);
    OHOS::GetFileInfoFromUriFuzzTest(proxy, data, size);
    OHOS::GetRootsFuzzTest(proxy, data, size);
    OHOS::AccessFuzzTest(proxy, data, size);
    OHOS::StartWatcherFuzzTest(proxy, data, size);
    OHOS::StopWatcherFuzzTest(proxy, data, size);
    OHOS::MoveItemFuzzTest(proxy, data, size);
    OHOS::MoveFileFuzzTest(proxy, data, size);

    OHOS::UrieFuzzTest(data, size);
    return 0;
}
