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

#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "bundle_mgr_interface.h"
#include "context.h"
#include "file_access_ext_connection.h"
#include "file_access_extension_info.h"
#include "ifile_access_ext_base.h"
#include "image_source.h"
#include "iobserver_callback.h"
#include "iremote_object.h"
#include "refbase.h"
#include "uri.h"
#include "want.h"

using Uri = OHOS::Uri;

namespace OHOS {
namespace FileAccessFwk {
using namespace Media;
using string = std::string;

namespace {
    static const std::string FILE_SCHEME_NAME = "file";
    static const std::string MEDIA_BNUDLE_NAME_ALIAS = "media";
    static const std::string MEDIA_BNUDLE_NAME = "com.ohos.medialibrary.medialibrarydata";
    static const std::string EXTERNAL_BNUDLE_NAME_ALIAS = "docs";
    static const std::string EXTERNAL_BNUDLE_NAME = "com.ohos.UserFile.ExternalFileManager";
}

struct ConnectInfo {
    AAFwk::Want want = {};
    sptr<FileAccessExtConnection> fileAccessExtConnection = nullptr;
};

class FileAccessHelper final : public std::enable_shared_from_this<FileAccessHelper> {
public:
    ~FileAccessHelper() = default;
    // get all ability want info
    static int GetRegisteredFileAccessExtAbilityInfo(std::vector<AAFwk::Want> &wantVec);
    // create and connect all ability
    static std::pair<std::shared_ptr<FileAccessHelper>, int>
        Creator(const std::shared_ptr<OHOS::AbilityRuntime::Context> &context);
    // create and connect with want, if created, only connect with want
    static std::pair<std::shared_ptr<FileAccessHelper>, int>
        Creator(const std::shared_ptr<OHOS::AbilityRuntime::Context> &context, const std::vector<AAFwk::Want> &wants);
    static std::shared_ptr<FileAccessHelper> Creator(const sptr<IRemoteObject> &token,
        const std::vector<AAFwk::Want> &wants);

    bool Release();
    int Access(Uri &uri, bool &isExist);
    int OpenFile(Uri &uri, const int flags, int &fd);
    int CreateFile(Uri &parent, const std::string &displayName, Uri &newFile);
    int Mkdir(Uri &parent, const std::string &displayName, Uri &newDir);
    int Delete(Uri &selectFile);
    int Move(Uri &sourceFile, Uri &targetParent, Uri &newFile);
    int Copy(Uri &sourceUri, Uri &destUri, std::vector<Result> &copyResult, bool force = false);
    int Rename(Uri &sourceFile, const std::string &displayName, Uri &newFile);
    int ListFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount, const FileFilter &filter,
        std::vector<FileInfo> &fileInfoVec);
    int ScanFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount, const FileFilter &filter,
        std::vector<FileInfo> &fileInfoVec);
    int GetThumbnail(Uri &uri, ThumbnailSize &thumbnailSize, std::shared_ptr<PixelMap> &pixelMap);
    int Query(Uri &uri, std::string &metaJson);
    int GetFileInfoFromUri(Uri &selectFile, FileInfo &fileInfo);
    int GetFileInfoFromRelativePath(std::string &selectFile, FileInfo &fileInfo);
    int GetRoots(std::vector<RootInfo> &rootInfoVec);
    int RegisterNotify(Uri uri, bool notifyForDescendants, sptr<IFileAccessObserver> &observer);
    int UnregisterNotify(Uri uri, sptr<IFileAccessObserver> &observer);
    int UnregisterNotify(Uri uri);
    int MoveItem(Uri &sourceFile, Uri &targetParent, std::vector<Result> &moveResult, bool force);
    int MoveFile(Uri &sourceFile, Uri &targetParent, std::string &fileName, Uri &newFile);
private:
    int StartWatcher(Uri &uri);
    int StopWatcher(Uri &uri, bool isUnregisterAll);
    sptr<IFileAccessExtBase> GetProxyByUri(Uri &uri);
    sptr<IFileAccessExtBase> GetProxyByBundleName(const std::string &bundleName);
    bool GetProxy();
    static sptr<AppExecFwk::IBundleMgr> GetBundleMgrProxy();
    FileAccessHelper(const std::shared_ptr<OHOS::AbilityRuntime::Context> &context,
        const std::unordered_map<std::string, std::shared_ptr<ConnectInfo>> &cMap);
    FileAccessHelper(const sptr<IRemoteObject> &token,
        const std::unordered_map<std::string, std::shared_ptr<ConnectInfo>> &cMap);

    void AddFileAccessDeathRecipient(const sptr<IRemoteObject> &token);
    void OnSchedulerDied(const wptr<IRemoteObject> &remote);

    std::shared_ptr<ConnectInfo> GetConnectInfo(const std::string &bundleName);

    int CopyOperation(Uri &sourceUri, Uri &destUri, std::vector<Result> &copyResult, bool force = false);
    int IsDirectory(Uri &uri, bool &isDir);

    sptr<IRemoteObject> token_ = nullptr;
    std::unordered_map<std::string, std::shared_ptr<ConnectInfo>> cMap_;
    std::mutex deathRecipientMutex_;
    static std::vector<AAFwk::Want> wants_;
    static std::string GetKeyOfWants(const AAFwk::Want &want);

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