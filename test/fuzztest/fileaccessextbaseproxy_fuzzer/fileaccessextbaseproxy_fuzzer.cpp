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

#include "user_file_service_token_mock.h"

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

bool MkdirFuzzTest(sptr<IFileAccessExtBase> proxy, const uint8_t *data, size_t size)
{
    int len = size / 3;
    Urie parent(string(reinterpret_cast<const char *>(data), len));
    string displayName(string(reinterpret_cast<const char *>(data + len), len));
    Urie newFile(string(reinterpret_cast<const char *>(data + len + len), len));
    proxy->Mkdir(parent, displayName, newFile);
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
    OHOS::UserFileServiceTokenMock tokenMock;
    tokenMock.SetFileManagerToken();
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
    OHOS::MkdirFuzzTest(proxy, data, size);
    OHOS::MoveFuzzTest(proxy, data, size);
    OHOS::RenameFuzzTest(proxy, data, size);
    OHOS::ListFileFuzzTest(proxy, data, size);
    OHOS::ScanFileFuzzTest(proxy, data, size);

    OHOS::UrieFuzzTest(data, size);
    return 0;
}
