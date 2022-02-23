/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FMS_MANAGER_PROXY_MOCK_H
#define FMS_MANAGER_PROXY_MOCK_H
#include <gtest/gtest.h>
#include "gmock/gmock.h"
#include "iremote_stub.h"
#include "iremote_proxy.h"
#include "ifms_client.h"

namespace OHOS {
namespace FileManagerService {
class FmsManagerProxyMock : public IRemoteStub<IFileManagerService>, public IFmsClient {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"FmsManagerProxyMock");
    FmsManagerProxyMock() {}
    virtual ~FmsManagerProxyMock() {}
    MOCK_METHOD4(SendRequest, int(uint32_t, MessageParcel &, MessageParcel &, MessageOption &));
    int32_t InvokeSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        return ERR_NONE;
    }
    virtual int Mkdir(const std::string &name, const std::string &path) override
    {
        return ERR_NONE;
    }
    virtual int ListFile(const std::string &type, const std::string &path, const CmdOptions &option,
        std::vector<std::shared_ptr<FileInfo>> &fileRes) override
    {
        return ERR_NONE;
    }
    virtual int GetRoot(const CmdOptions &option, std::vector<std::shared_ptr<FileInfo>> &fileRes) override
    {
        return ERR_NONE;
    }
    virtual int CreateFile(const std::string &path, const std::string &fileName,
        const CmdOptions &option, std::string &uri) override
    {
        return ERR_NONE;
    }
};
}  // namespace FileManagerService
}  // namespace OHOS

#endif  // FMS_MANAGER_PROXY_MOCK_H