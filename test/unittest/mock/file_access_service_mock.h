/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#ifndef FILE_ACCESS_SERVICE_MOCK_H
#define FILE_ACCESS_SERVICE_MOCK_H

#include "file_access_service_base_stub.h"

namespace OHOS {
namespace FileAccessFwk {
using Uri = OHOS::Uri;
class FileAccessServiceMock : public FileAccessServiceBaseStub {
public:
    FileAccessServiceMock() {}

    ~FileAccessServiceMock() {}
    MOCK_METHOD2(OnChange, int32_t(const Uri &uri, NotifyType notifyType));
    MOCK_METHOD4(RegisterNotify, int32_t(const Uri &uri, bool notifyForDescendants,
        const sptr<IFileAccessObserver> &observer, const ConnectExtensionInfo& info));
    MOCK_METHOD3(UnregisterNotify, int32_t(const Uri &uri, const sptr<IFileAccessObserver> &observer,
        const ConnectExtensionInfo& info));
    MOCK_METHOD2(UnregisterNotifyNoObserver, int32_t(const Uri &uri, const ConnectExtensionInfo& info));
    MOCK_METHOD2(GetExtensionProxy, int32_t(const ConnectExtensionInfo& info,
        sptr<IFileAccessExtBase> &extensionProxy));
    MOCK_METHOD0(AsObject, sptr<IRemoteObject>());
    MOCK_METHOD2(CleanAllNotify, int32_t(Uri uri, const std::shared_ptr<ConnectExtensionInfo> &info));
    MOCK_METHOD4(OnRemoteRequest, int32_t(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option));
    MOCK_METHOD4(SendRequest, int(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option));
    MOCK_METHOD2(ConnectFileExtAbility, int32_t(const AAFwk::Want &want,
        const sptr<AAFwk::IAbilityConnection>& connection));
    MOCK_METHOD1(DisConnectFileExtAbility, int32_t(const sptr<AAFwk::IAbilityConnection>& connection));
    MOCK_METHOD1(Register, int32_t(const SyncFolder &rootInfo));
    MOCK_METHOD1(Unregister, int32_t(const std::string &path));
    MOCK_METHOD1(Active, int32_t(const std::string &path));
    MOCK_METHOD1(Deactive, int32_t(const std::string &path));
    MOCK_METHOD1(GetSyncFolders, int32_t(std::vector<SyncFolder> &syncFolders));
    MOCK_METHOD1(GetAllSyncFolders, int32_t(std::vector<SyncFolderExt> &syncFolderExts));
    MOCK_METHOD2(UpdateDisplayName, int32_t(const std::string &path, const std::string &displayName));
    MOCK_METHOD1(UnregisterForSa, int32_t(const std::string &path));
    MOCK_METHOD1(GetAllSyncFoldersForSa, int32_t(std::vector<SyncFolderExt> &syncFolderExts));
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_SERVICE_MOCK_H
