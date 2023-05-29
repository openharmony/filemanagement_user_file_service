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

#ifndef FILE_ACCESS_SERVICE_PROXY_H
#define FILE_ACCESS_SERVICE_PROXY_H

#include "ifile_access_service_base.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace FileAccessFwk {
class FileAccessServiceProxy : public IRemoteProxy<IFileAccessServiceBase> {
public:
    explicit FileAccessServiceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IFileAccessServiceBase>(impl) {}
    ~FileAccessServiceProxy() = default;
    int32_t OnChange(Uri uri, NotifyType notifyType) override;
    int32_t RegisterNotify(Uri uri, const sptr<IFileAccessObserver> &observer, bool notifyForDescendants) override;
    int32_t UnregisterNotify(Uri uri, const sptr<IFileAccessObserver> &observer) override;

private:
    static inline BrokerDelegator<FileAccessServiceProxy> delegator_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_SERVICE_PROXY_H