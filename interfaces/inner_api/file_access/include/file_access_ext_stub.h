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

#ifndef FILE_ACCESS_EXT_STUB_H
#define FILE_ACCESS_EXT_STUB_H

#include <errors.h>

#include <cstdint>
#include <map>

#include "ifile_access_ext_base.h"
#include "iremote_stub.h"
#include "message_option.h"
#include "message_parcel.h"
#include "refbase.h"

namespace OHOS {
namespace FileAccessFwk {
class FileAccessExtStub : public IRemoteStub<IFileAccessExtBase> {
public:
    FileAccessExtStub();
    virtual ~FileAccessExtStub();
    int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;
private:
    ErrCode CmdOpenFile(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdCreateFile(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdMkdir(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdDelete(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdMove(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdRename(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdListFile(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdScanFile(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdGetThumbnail(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdQuery(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdGetFileInfoFromUri(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdGetFileInfoFromRelativePath(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdGetRoots(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdAccess(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdRegisterNotify(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdUnregisterNotify(MessageParcel &data, MessageParcel &reply);
    bool CheckCallingPermission(const std::string &permission);
    using RequestFuncType = int (FileAccessExtStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, RequestFuncType> stubFuncMap_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_EXT_STUB_H