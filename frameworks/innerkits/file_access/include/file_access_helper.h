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

#include <mutex>
#include <map>
#include <string>

#include "file_ext_connection.h"
#include "foundation/ability/ability_runtime/frameworks/kits/appkit/native/ability_runtime/context/context.h"
#include "ifile_ext_base.h"
#include "want.h"
#include "hilog_wrapper.h"


namespace OHOS {
namespace FileAccessFwk {
using string = std::string;
class FileAccessHelper final : public std::enable_shared_from_this<FileAccessHelper> {
public:
    ~FileAccessHelper() = default;

    static std::shared_ptr<FileAccessHelper> Creator(const std::shared_ptr<OHOS::AbilityRuntime::Context> &context,
        const AAFwk::Want &want);

private:
    FileAccessHelper(const std::shared_ptr<OHOS::AbilityRuntime::Context> &context, const AAFwk::Want &want,
        const sptr<IFileExtBase> &fileExtProxy);
    void AddFileAccessDeathRecipient(const sptr<IRemoteObject> &token);
    void OnSchedulerDied(const wptr<IRemoteObject> &remote);

    sptr<IRemoteObject> token_ = {};
    AAFwk::Want want_ = {};
    sptr<IFileExtBase> fileExtProxy_ = nullptr;
    sptr<IRemoteObject::DeathRecipient> callerDeathRecipient_ = nullptr;
    sptr<FileExtConnection> fileExtConnection_ = nullptr;
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
