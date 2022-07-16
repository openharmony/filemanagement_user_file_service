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

#ifndef FILE_ACCESS_HELPER_H
#define FILE_ACCESS_HELPER_H

#include <unordered_map>
#include <string>
#include <vector>

#include "context.h"
#include "file_access_ext_connection.h"
#include "file_access_extension_info.h"
#include "ifile_access_ext_base.h"
#include "iremote_object.h"
#include "refbase.h"
#include "uri.h"
#include "want.h"

using Uri = OHOS::Uri;

namespace OHOS {
namespace FileAccessFwk {
using string = std::string;

struct ConnectInfo {
    AAFwk::Want want = {};
    sptr<IFileAccessExtBase> fileAccessExtProxy = nullptr;
    std::shared_ptr<FileAccessExtConnection> fileAccessExtConnection = nullptr;
};

class FileAccessHelper final : public std::enable_shared_from_this<FileAccessHelper> {
public:
    ~FileAccessHelper() = default;
    // get all ability want info
    static std::vector<AAFwk::Want> GetRegisterFileAccessExtAbilityInfo();
    // create and connect all ability
    static std::shared_ptr<FileAccessHelper> Creator(const std::shared_ptr<OHOS::AbilityRuntime::Context> &context);
    // create and connect with want, if created, only connect with want
    static std::shared_ptr<FileAccessHelper> Creator(const std::shared_ptr<OHOS::AbilityRuntime::Context> &context,
        const std::vector<AAFwk::Want> &wants);

    sptr<IFileAccessExtBase> GetProxy(Uri &uri);
    bool GetProxy();
    bool Release();
    int OpenFile(Uri &uri, int flags);
    int CreateFile(Uri &parent, const std::string &displayName, Uri &newFile);
    int Mkdir(Uri &parent, const std::string &displayName, Uri &newDir);
    int Delete(Uri &selectFile);
    int Move(Uri &sourceFile, Uri &targetParent, Uri &newFile);
    int Rename(Uri &sourceFile, const std::string &displayName, Uri &newFile);
    std::vector<FileInfo> ListFile(Uri &sourceFile);
    std::vector<DeviceInfo> GetRoots();

    bool AddService(AAFwk::Want &want);
private:
    FileAccessHelper(const std::shared_ptr<OHOS::AbilityRuntime::Context> &context,
        const std::unordered_map<std::string, std::shared_ptr<ConnectInfo>> &cMap);
    void AddFileAccessDeathRecipient(const sptr<IRemoteObject> &token);
    void OnSchedulerDied(const wptr<IRemoteObject> &remote);

    // get ConnectInfo obj from cMap_ with key
    std::shared_ptr<ConnectInfo> GetConnectInfo(const std::string &key);
    // get ConnectInfo obj from cMap_ with uri (key is part of uri)
    std::shared_ptr<ConnectInfo> GetConnectInfo(Uri &uri);
    // get ConnectInfo obj from cMap_ with want (key is part of uri)
    std::shared_ptr<ConnectInfo> GetConnectInfo(const AAFwk::Want &want);
    // insert connect info to cMap_
    void InsertConnectInfo(const std::string &key,
                           const AAFwk::Want &want,
                           const sptr<IFileAccessExtBase> &fileExtProxy,
                           std::shared_ptr<FileAccessExtConnection> fileExtConnection);

    sptr<IRemoteObject> token_ = nullptr;
    // save connects info
    std::unordered_map<std::string, std::shared_ptr<ConnectInfo>> cMap_;

    // save wants info
    static std::unordered_map<std::string, AAFwk::Want> wantsMap_;
    // get key from wantsMap_ with want (key is configed in ability server)
    static std::string GetKeyOfWantsMap(const AAFwk::Want &want);

    bool isSystemCaller_ = false;
    sptr<IRemoteObject::DeathRecipient> callerDeathRecipient_ = nullptr;
};

class FileAccessDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    using RemoteDiedHandler = std::function<void(const wptr<IRemoteObject> &)>;
    explicit FileAccessDeathRecipient(RemoteDiedHandler handler);
    virtual ~FileAccessDeathRecipient();
    virtual void OnRemoteDied(const wptr<IRemoteObject> &remote);

private:
    RemoteDiedHandler handler_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_HELPER_H
