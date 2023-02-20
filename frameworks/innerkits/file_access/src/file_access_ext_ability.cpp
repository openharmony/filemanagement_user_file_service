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

#include "ability_loader.h"
#include "connection_manager.h"
#include "extension_context.h"
#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
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

int FileAccessExtAbility::GetRoots(std::vector<RootInfo> &rootInfoVec)
{
    HILOG_ERROR("FileAccessExtAbility::GetRoots Undefined operation");
    return EPERM;
}

int FileAccessExtAbility::GetThumbnail(const Uri &uri, Size &size, std::shared_ptr<PixelMap> &pixelMap)
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

bool FileAccessExtAbility::GetNotifyManager()
{
    if (notifyManager_ == nullptr) {
        notifyManager_ = std::make_unique<FileAccessNotifyManager>();
        if (notifyManager_ == nullptr) {
            return false;
        }
    }
    return true;
}

int FileAccessExtAbility::RegisterNotify(sptr<IFileAccessNotify> &notify)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "RegisterNotify");
    if (!GetNotifyManager()) {
        HILOG_ERROR("GetNotifyManager fail.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_REGISTER;
    }
    int ret = notifyManager_->RegisterNotify(notify);
    if (ret != ERR_OK) {
        HILOG_ERROR("NotifyManager register notify fail.");
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtAbility::UnregisterNotify(sptr<IFileAccessNotify> &notify)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "UnregisterNotify");
    if (!GetNotifyManager()) {
        HILOG_ERROR("GetNotifyManager fail.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_REMOVE;
    }
    int ret = notifyManager_->UnregisterNotify(notify);
    if (ret != ERR_OK) {
        HILOG_ERROR("NotifyManager unregister notify fail.");
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}

int FileAccessExtAbility::Notify(const NotifyMessage& message)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Notify");
    if (!GetNotifyManager()) {
        HILOG_ERROR("GetNotifyManager fail.");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return E_NOTIFY;
    }
    int ret = notifyManager_->Notify(message);
    if (ret != ERR_OK) {
        HILOG_ERROR("NotifyManager handle notify fail.");
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
    return ret;
}
} // namespace FileAccessFwk
} // namespace OHOS