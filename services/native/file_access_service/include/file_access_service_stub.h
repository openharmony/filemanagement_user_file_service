/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef FILE_ACCESS_SERVICE_STUB_H
#define FILE_ACCESS_SERVICE_STUB_H

#include <map>

#include "ifile_access_service_base.h"
#include "iremote_stub.h"
#include "refbase.h"

namespace OHOS {
namespace FileAccessFwk {
class FileAccessServiceStub : public IRemoteStub<IFileAccessServiceBase> {
public:
    FileAccessServiceStub();
    virtual ~FileAccessServiceStub();
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

protected:
virtual int32_t CleanAllNotify (Uri uri, const std::shared_ptr<ConnectExtensionInfo> &info) = 0;

private:
    ErrCode CmdOnChange(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdRegisterNotify(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdUnregisterNotify(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdGetExensionProxy(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdConnectFileExtAbility(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdDisConnectFileExtAbility(MessageParcel &data, MessageParcel &reply);
    bool CheckCallingPermission(const std::string &permission);
    using RequestFuncType = int (FileAccessServiceStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, RequestFuncType> stubFuncMap_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_SERVICE_STUB_H
