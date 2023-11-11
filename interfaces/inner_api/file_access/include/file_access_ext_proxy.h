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

#ifndef FILE_ACCESS_EXT_PROXY_H
#define FILE_ACCESS_EXT_PROXY_H

#include <functional>
#include <iosfwd>
#include <vector>

#include "file_access_extension_info.h"
#include "ifile_access_ext_base.h"
#include "image_source.h"
#include "iremote_broker.h"
#include "iremote_object.h"
#include "iremote_proxy.h"
#include "refbase.h"
#include "uri.h"

namespace OHOS {
namespace FileAccessFwk {
using namespace Media;
class FileAccessExtProxy : public IRemoteProxy<IFileAccessExtBase> {
public:
    explicit FileAccessExtProxy(const sptr<IRemoteObject> &remote) : IRemoteProxy<IFileAccessExtBase>(remote) {}

    virtual ~FileAccessExtProxy() {}

    virtual int OpenFile(const Uri &uri, const int flags, int &fd) override;
    virtual int CreateFile(const Uri &parent, const std::string &displayName, Uri &newFile) override;
    virtual int Mkdir(const Uri &parent, const std::string &displayName, Uri &newFile) override;
    virtual int Delete(const Uri &sourceFile) override;
    virtual int Move(const Uri &sourceFile, const Uri &targetParent, Uri &newFile) override;
    virtual int Copy(const Uri &sourceUri, const Uri &destUri, std::vector<Result> &copyResult,
        bool force = false) override;
    virtual int Rename(const Uri &sourceFile, const std::string &displayName, Uri &newFile) override;
    virtual int ListFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
        const FileFilter &filter, std::vector<FileInfo> &fileInfoVec) override;
    virtual int ScanFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
        const FileFilter &filter, std::vector<FileInfo> &fileInfoVec) override;
    virtual int GetThumbnail(const Uri &uri, const ThumbnailSize &thumbnailSize,
        std::shared_ptr<PixelMap> &pixelMap) override;
    virtual int Query(const Uri &uri, std::vector<std::string> &columns, std::vector<std::string> &results) override;
    virtual int GetFileInfoFromUri(const Uri &selectFile, FileInfo &fileInfo) override;
    virtual int GetFileInfoFromRelativePath(const std::string &selectFile, FileInfo &fileInfo) override;
    virtual int GetRoots(std::vector<RootInfo> &rootInfoVec) override;
    virtual int Access(const Uri &uri, bool &isExist) override;
    virtual int StartWatcher(const Uri &uri) override;
    virtual int StopWatcher(const Uri &uri, bool isUnregisterAll) override;
    virtual int MoveItem(const Uri &sourceFile, const Uri &targetParent, std::vector<Result> &moveResult,
                         bool force = false) override;
    virtual int MoveFile(const Uri &sourceFile, const Uri &targetParent, std::string &fileName, Uri &newFile) override;
private:
    static inline BrokerDelegator<FileAccessExtProxy> delegator_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_EXT_PROXY_H