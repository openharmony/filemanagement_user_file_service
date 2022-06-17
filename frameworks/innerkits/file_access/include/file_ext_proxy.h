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

#ifndef FILE_EXT_PROXY_H
#define FILE_EXT_PROXY_H

#include <iremote_proxy.h>

#include "file_extension_info.h"
#include "ifile_ext_base.h"

namespace OHOS {
namespace FileAccessFwk {
class FileExtProxy : public IRemoteProxy<IFileExtBase> {
public:
    explicit FileExtProxy(const sptr<IRemoteObject>& remote) : IRemoteProxy<IFileExtBase>(remote) {}

    virtual ~FileExtProxy() {}

    virtual int OpenFile(const Uri &uri, int flags) override;
    virtual int CreateFile(const Uri &parentUri, const std::string &displayName,  Uri &newFileUri) override;
    virtual int Mkdir(const Uri &parentUri, const std::string &displayName, Uri &newFileUri) override;
    virtual int Delete(const Uri &sourceFileUri) override;
    virtual int Rename(const Uri &sourceFileUri, const std::string &displayName, Uri &newFileUri) override;

    virtual std::vector<FileInfo> ListFile(const Uri &sourceFileUri) override;
    virtual std::vector<DeviceInfo> GetRoots() override;
private:
    static inline BrokerDelegator<FileExtProxy> delegator_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_EXT_PROXY_H

