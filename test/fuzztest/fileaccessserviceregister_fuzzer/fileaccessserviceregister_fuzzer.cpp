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

#include "fileaccessserviceregister_fuzzer.h"

#include <string>
#include <vector>
#include <securec.h>

#include "file_access_service_client.h"
namespace OHOS {
using namespace std;
using namespace FileAccessFwk;
using namespace FileManagement;

template <class T>
T TypeCast(const uint8_t *data, int *pos = nullptr)
{
    if (pos) {
        *pos += sizeof(T);
    }
    return *(reinterpret_cast<const T *>(data));
}

SyncFolder BuildSyncFolder(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < sizeof(uint32_t) + sizeof(State)) {
        return SyncFolder();
    }
    int pos = 0;
    SyncFolder syncFolder;
    syncFolder.state_ = TypeCast<State>(data, &pos);
    syncFolder.displayNameResId_ = TypeCast<uint32_t>(data + pos, &pos);
    int len = (size - pos) / 2;
    int displayNameLen = size - pos - len;
    std::string path(reinterpret_cast<const char*>(data + pos), len);
    syncFolder.path_ = path;
    std::string name(reinterpret_cast<const char*>(data + pos + len), displayNameLen);
    syncFolder.displayName_ = name;
    return syncFolder;
}

bool DoSomethingRegisterFuzzTest(sptr<IFileAccessServiceBase> proxy, const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return true;
    }
    SyncFolder syncFolder = BuildSyncFolder(data, size);
    proxy->Register(syncFolder);
    return true;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    auto proxy = OHOS::FileAccessFwk::FileAccessServiceClient::GetInstance();
    if (proxy == nullptr) {
        printf("service proxy is nullptr\n");
        return 0;
    }
    OHOS::DoSomethingRegisterFuzzTest(proxy, data, size);
    return 0;
}
}