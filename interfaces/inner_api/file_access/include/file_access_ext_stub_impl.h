/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#ifndef FILE_ACCESS_EXT_STUB_IMPL_H
#define FILE_ACCESS_EXT_STUB_IMPL_H

#include <memory>
#include <vector>

#include "file_access_ext_ability.h"
#include "file_access_ext_base_stub.h"
#include "file_access_extension_info.h"
#include "js_native_api_types.h"
#include "uri.h"
#include "uri_ext.h"

namespace OHOS {
namespace FileAccessFwk {
class FileAccessExtStubImpl : public FileAccessExtBaseStub {
public:
    explicit FileAccessExtStubImpl(const std::shared_ptr<FileAccessExtAbility>& extension, napi_env env)
        : extension_(extension) {}

    virtual ~FileAccessExtStubImpl() {}

    bool CheckCallingPermission(const std::string &permission);
    int OpenFile(const Urie &uri, const int flags, int &fd) override;
    int CreateFile(const Urie &parent, const std::string &displayName, Urie &newFile) override;
    int Mkdir(const Urie &parent, const std::string &displayName, Urie &newFile) override;
    int Delete(const Urie &sourceFile) override;
    int Move(const Urie &sourceFile, const Urie &targetParent, Urie &newFile) override;
    int Copy(const Urie &sourceUri, const Urie &destUri, std::vector<Result> &copyResult,
        int32_t& retCode, bool force = false) override;
    int CopyFile(const Urie &sourceUri, const Urie &destUri, const std::string &fileName,
        Urie &newFileUri) override;
    int Rename(const Urie &sourceFile, const std::string &displayName, Urie &newFile) override;
    int ListFile(const FileInfo &fileInfo, const int64_t offset, const FileFilter &filter,
        SharedMemoryInfo &memInfo) override;
    int ScanFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
        const FileFilter &filter, std::vector<FileInfo> &fileInfoVec) override;
    int Query(const Urie &uri, const std::vector<std::string> &columns, std::vector<std::string> &results) override;
    int GetFileInfoFromUri(const Urie &selectFile, FileInfo &fileInfo) override;
    int GetFileInfoFromRelativePath(const std::string &selectFile, FileInfo &fileInfo) override;
    int GetRoots(std::vector<RootInfo> &rootInfoVec) override;
    int Access(const Urie &uri, bool &isExist) override;
    int StartWatcher(const Urie &uri) override;
    int StopWatcher(const Urie &uri) override;
    int MoveItem(const Urie &sourceFile, const Urie &targetParent, std::vector<Result> &moveResult,
                 int32_t& retCode, bool force = false) override;
    int MoveFile(const Urie &sourceFile, const Urie &targetParent, const std::string &fileName, Urie &newFile) override;
private:
    std::shared_ptr<FileAccessExtAbility> GetOwner();
    std::shared_ptr<FileAccessExtAbility> extension_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_EXT_STUB_IMPL_H
