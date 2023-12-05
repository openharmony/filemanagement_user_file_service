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

#include "file_access_ext_ability.h"

#include "connection_manager.h"
#include "extension_context.h"
#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "if_system_ability_manager.h"
#include "js_file_access_ext_ability.h"
#include "runtime.h"

namespace OHOS {
namespace FileAccessFwk {
using namespace OHOS::AppExecFwk;

CreatorFunc FileAccessExtAbility::creator_ = nullptr;
void FileAccessExtAbility::SetCreator(const CreatorFunc &creator)
{
    creator_ = creator;
}

FileAccessExtAbility* FileAccessExtAbility::Create(const std::unique_ptr<Runtime> &runtime)
{
    if (runtime == nullptr) {
        return new FileAccessExtAbility();
    }

    if (creator_) {
        return creator_(runtime);
    }

    switch (runtime->GetLanguage()) {
        case Runtime::Language::JS:
            return JsFileAccessExtAbility::Create(runtime);

        default:
            return new FileAccessExtAbility();
    }
}

void FileAccessExtAbility::Init(const std::shared_ptr<AbilityLocalRecord> &record,
    const std::shared_ptr<OHOSApplication> &application,
    std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    ExtensionBase<>::Init(record, application, handler, token);
}

int FileAccessExtAbility::OpenFile(const Uri &uri, const int flags, int &fd)
{
    HILOG_ERROR("FileAccessExtAbility::OpenFile Undefined operation");
    return EPERM;
}

int FileAccessExtAbility::CreateFile(const Uri &parent, const std::string &displayName, Uri &newFile)
{
    HILOG_ERROR("FileAccessExtAbility::CreateFile Undefined operation");
    return EPERM;
}

int FileAccessExtAbility::Mkdir(const Uri &parent, const std::string &displayName, Uri &newFile)
{
    HILOG_ERROR("FileAccessExtAbility::Mkdir Undefined operation");
    return EPERM;
}

int FileAccessExtAbility::Delete(const Uri &sourceFile)
{
    HILOG_ERROR("FileAccessExtAbility::Delete Undefined operation");
    return EPERM;
}

int FileAccessExtAbility::Move(const Uri &sourceFile, const Uri &targetParent, Uri &newFile)
{
    HILOG_ERROR("FileAccessExtAbility::Move Undefined operation");
    return EPERM;
}

int FileAccessExtAbility::Copy(const Uri &sourceUri, const Uri &destUri, std::vector<Result> &copyResult, bool force)
{
    HILOG_ERROR("FileAccessExtAbility::Copy Undefined operation");
    return EPERM;
}

int FileAccessExtAbility::CopyFile(const Uri &sourceUri, const Uri &destUri, const std::string &fileName,
    Uri &newFileUri)
{
    HILOG_ERROR("FileAccessExtAbility::Copy file Undefined operation");
    return EPERM;
}

int FileAccessExtAbility::Rename(const Uri &sourceFile, const std::string &displayName, Uri &newFile)
{
    HILOG_ERROR("FileAccessExtAbility::Rename Undefined operation");
    return EPERM;
}

int FileAccessExtAbility::ListFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
    const FileFilter &filter, std::vector<FileInfo> &fileInfoVec)
{
    HILOG_ERROR("FileAccessExtAbility::ListFile Undefined operation");
    return EPERM;
}

int FileAccessExtAbility::ScanFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
    const FileFilter &filter, std::vector<FileInfo> &fileInfoVec)
{
    HILOG_ERROR("FileAccessExtAbility::ScanFile Undefined operation");
    return EPERM;
}

int FileAccessExtAbility::Query(const Uri &uri, std::vector<std::string> &columns, std::vector<std::string> &results)
{
    HILOG_ERROR("FileAccessExtAbility::Query Undefined operation");
    return EPERM;
}

int FileAccessExtAbility::GetRoots(std::vector<RootInfo> &rootInfoVec)
{
    HILOG_ERROR("FileAccessExtAbility::GetRoots Undefined operation");
    return EPERM;
}

int FileAccessExtAbility::GetThumbnail(const Uri &uri, const Size &size, std::unique_ptr<PixelMap> &pixelMap)
{
    HILOG_ERROR("FileAccessExtAbility::GetThumbnail Undefined operation");
    return EPERM;
}

int FileAccessExtAbility::GetFileInfoFromUri(const Uri &selectFile, FileInfo &fileInfo)
{
    HILOG_ERROR("FileAccessExtAbility::GetFileInfoFromUri Undefined operation");
    return EPERM;
}

int FileAccessExtAbility::GetFileInfoFromRelativePath(const std::string &selectFile, FileInfo &fileInfo)
{
    HILOG_ERROR("FileAccessExtAbility::GetFileInfoFromRelativePath Undefined operation");
    return EPERM;
}

int FileAccessExtAbility::Access(const Uri &uri, bool &isExist)
{
    HILOG_ERROR("FileAccessExtAbility::IsFileExist Undefined operation");
    return EPERM;
}

int FileAccessExtAbility::StartWatcher(const Uri &uri)
{
    HILOG_ERROR("FileAccessExtAbility::StartWatcher Undefined operation");
    return EPERM;
}

int FileAccessExtAbility::StopWatcher(const Uri &uri, bool isUnregisterAll)
{
    HILOG_ERROR("FileAccessExtAbility::StopWatcher Undefined operation");
    return EPERM;
}

int FileAccessExtAbility::MoveItem(const Uri &sourceUri, const Uri &targetParent, std::vector<Result> &moveResult,
                                   bool force)
{
    HILOG_ERROR("FileAccessExtAbility::MoveItem Undefined operation");
    return EPERM;
}

int FileAccessExtAbility::MoveFile(const Uri &sourceUri, const Uri &targetParent, std::string &fileName, Uri &newFile)
{
    HILOG_ERROR("FileAccessExtAbility::MoveFile Undefined operation");
    return EPERM;
}

int FileAccessExtAbility::GetFileInfoNum(const std::string &sourceFileUri, const FileFilter &filter, bool recursion,
    uint32_t &counts)
{
    HILOG_ERROR("FileAccessExtAbility::GetFileInfoNum Undefined operation");
    return EPERM;
}
} // namespace FileAccessFwk
} // namespace OHOS