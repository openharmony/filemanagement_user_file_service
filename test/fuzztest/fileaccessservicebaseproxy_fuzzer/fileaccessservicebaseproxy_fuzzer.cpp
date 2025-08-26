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

#include "fileaccessservicebaseproxy_fuzzer.h"

#include <string>

#include "app_file_access_ext_connection.h"
#include "file_access_observer_stub.h"
#include "file_access_service_client.h"

#include "accesstoken_kit.h"
#include "file_access_helper.h"
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

class TestObserver : public FileAccessObserverStub {
public:
    TestObserver() {};
    virtual ~TestObserver() = default;
    int OnChange(const NotifyMessage &notifyMessage) override;
};

int TestObserver::OnChange(const NotifyMessage &notifyMessage)
{
    return 1;
}

bool OnChangeFuzzTest(sptr<IFileAccessServiceBase> proxy, const uint8_t* data, size_t size)
{
    if (data == nullptr || size < sizeof(NotifyType)) {
        return true;
    }

    int pos = 0;
    NotifyType notifyType = TypeCast<NotifyType>(data, &pos);
    Uri uri(string(reinterpret_cast<const char *>(data + pos), size - pos));
    proxy->OnChange(uri, notifyType);
    return true;
}

bool ConnectFileExtAbilityFuzzTest(sptr<IFileAccessServiceBase> proxy, const uint8_t* data, size_t size)
{
    int len = size / 2;
    string bundleName(reinterpret_cast<const char *>(data), len);
    string infoName(reinterpret_cast<const char *>(data + len), len);
    AAFwk::Want want;
    want.SetElementName(bundleName, infoName);

    auto helper = GetFileAccessHelper();
    if (helper == nullptr) {
        printf("helper is nullptr.");
        return 0;
    }
    auto connectInfo = helper->GetConnectInfo("com.ohos.UserFile.ExternalFileManager");
    if (connectInfo == nullptr) {
        printf("connectInfo is nullptr");
        return 0;
    }
    auto fileAccessExtConnection = connectInfo->fileAccessExtConnection;
    if (fileAccessExtConnection == nullptr) {
        printf("fileAccessExtConnection is nullptr");
    }
    proxy->ConnectFileExtAbility(want, connectInfo->fileAccessExtConnection);
    proxy->DisConnectFileExtAbility(connectInfo->fileAccessExtConnection);
    return true;
}

bool UnregisterNotifyNoObserverFuzzTest(sptr<IFileAccessServiceBase> proxy, const uint8_t* data, size_t size)
{
    if (data == nullptr || size < sizeof(bool) || g_fah == nullptr) {
        return true;
    }

    int pos = sizeof(bool);
    Uri uri(string(reinterpret_cast<const char *>(data + pos), size - pos));
    AAFwk::Want want;
    want.SetElementName(EXTERNAL_BNUDLE_NAME, "FileExtensionAbility");
    sptr<ConnectExtensionInfo> info = sptr(new (std::nothrow) ConnectExtensionInfo(want, g_fah->token_));
    proxy->UnregisterNotifyNoObserver(uri, *info);
    return true;
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::UserFileServiceTokenMock tokenMock;
    tokenMock.SetFileManagerToken();
    auto proxy = OHOS::FileAccessFwk::FileAccessServiceClient::GetInstance();
    if (proxy == nullptr) {
        printf("service proxy is nullptr");
        return 0;
    }
    /* Run your code on data */
    OHOS::OnChangeFuzzTest(proxy, data, size);
    OHOS::ConnectFileExtAbilityFuzzTest(proxy, data, size);
    OHOS::UnregisterNotifyNoObserverFuzzTest(proxy, data, size);
    return 0;
}
