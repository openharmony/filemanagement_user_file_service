/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FILE_ACCESS_SERVICE_H
#define FILE_ACCESS_SERVICE_H

#include <memory>
#include <mutex>
#include <system_ability.h>
#include <vector>

#include "file_access_service_stub.h"
#include "iremote_object.h"
#include "uri.h"

namespace OHOS {
namespace FileAccessFwk {

class FileAccessService final : public SystemAbility, public FileAccessServiceStub {
    DECLARE_SYSTEM_ABILITY(FileAccessService)

public:
    static sptr<FileAccessService> GetInstance();
    virtual ~FileAccessService() = default;

    virtual void OnStart() override;
    virtual void OnStop() override;
    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;

public:
    int32_t RegisterNotify(Uri uri, const sptr<IFileAccessObserver> &observer, bool notifyForDescendants) override;
    int32_t UnregisterNotify(Uri uri, const sptr<IFileAccessObserver> &observer) override;
    int32_t OnChange(Uri uri, NotifyType notifyType) override;

private:
    FileAccessService();
    bool IsServiceReady() const;
    static sptr<FileAccessService> instance_;
    bool ready_ = false;
    static std::mutex mutex_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_SERVICE_H