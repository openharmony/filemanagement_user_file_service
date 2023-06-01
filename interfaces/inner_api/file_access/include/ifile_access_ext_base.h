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

#ifndef I_FILE_ACCESS_EXT_BASE_H
#define I_FILE_ACCESS_EXT_BASE_H

#include <bitset>
#include <cstdint>
#include <string_ex.h>
#include <iremote_broker.h>

#include "file_access_extension_info.h"
#include "file_access_framework_errno.h"
#include "file_filter.h"
#include "image_source.h"
#include "uri.h"

namespace OHOS {
namespace FileAccessFwk {
using namespace DistributedFS;
using namespace Media;

class IFileAccessExtBase : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.FileAccessFwk.IFileAccessExtBase");

    enum {
        CMD_OPEN_FILE = 1,
        CMD_CREATE_FILE,
        CMD_MKDIR,
        CMD_DELETE,
        CMD_MOVE,
        CMD_COPY,
        CMD_RENAME,
        CMD_LIST_FILE,
        CMD_SCAN_FILE,
        CMD_START_WATCHER,
        CMD_STOP_WATCHER,
        CMD_QUERY,
        CMD_GET_ROOTS,
        CMD_ACCESS,
        CMD_GET_THUMBNAIL,
        CMD_GET_FILEINFO_FROM_URI,
        CMD_GET_FILEINFO_FROM_RELATIVE_PATH
    };

    virtual int OpenFile(const Uri &uri, const int flags, int &fd) = 0;
    virtual int CreateFile(const Uri &parent, const std::string &displayName, Uri &newFile) = 0;
    virtual int Mkdir(const Uri &parent, const std::string &displayName, Uri &newFile) = 0;
    virtual int Delete(const Uri &sourceFile) = 0;
    virtual int Move(const Uri &sourceFile, const Uri &targetParent, Uri &newFile) = 0;
    virtual int Copy(const Uri &sourceUri, const Uri &destUri, std::vector<CopyResult> &copyResult,
        bool force = false) = 0;
    virtual int Rename(const Uri &sourceFile, const std::string &displayName, Uri &newFile) = 0;
    virtual int ListFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
        const FileFilter &filter, std::vector<FileInfo> &fileInfoVec) = 0;
    virtual int ScanFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
        const FileFilter &filter, std::vector<FileInfo> &fileInfoVec) = 0;
    virtual int GetThumbnail(const Uri &uri, const ThumbnailSize &thumbnailSize,
        std::shared_ptr<PixelMap> &pixelMap) = 0;
    virtual int Query(const Uri &uri, std::vector<std::string> &columns, std::vector<std::string> &results) = 0;
    virtual int GetFileInfoFromUri(const Uri &selectFile, FileInfo &fileInfo) = 0;
    virtual int GetFileInfoFromRelativePath(const std::string &selectFile, FileInfo &fileInfo) = 0;
    virtual int GetRoots(std::vector<RootInfo> &rootInfoVec) = 0;
    virtual int Access(const Uri &uri, bool &isExist) = 0;
    virtual int StartWatcher(const Uri &uri) = 0;
    virtual int StopWatcher(const Uri &uri) = 0;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // I_FILE_ACCESS_EXT_BASE_H
