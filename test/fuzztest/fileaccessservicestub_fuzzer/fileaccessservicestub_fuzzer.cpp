/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "fileaccessservicestub_fuzzer.h"

#include <cstring>
#include <memory>

#include "extension_context.h"
#include "file_access_service_stub.h"
#include "fileaccessservicemock.h"
#include "message_parcel.h"

namespace OHOS {
using namespace std;
using namespace FileAccessFwk;

template<class T>
T TypeCast(const uint8_t *data, int *pos = nullptr)
{
    if (pos) {
        *pos += sizeof(T);
    }
    return *(reinterpret_cast<const T*>(data));
}

bool OnRemoteRequestFuzzTest(shared_ptr<FileAccessServiceStub> fileAccessServiceStub, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(uint32_t)) {
        return true;
    }

    uint32_t code = TypeCast<uint32_t>(data);
    MessageParcel msg;
    msg.WriteInterfaceToken(FileAccessServiceStub::GetDescriptor());
    MessageParcel reply;
    MessageOption option;

    fileAccessServiceStub->OnRemoteRequest(code, msg, reply, option);
    return true;
}

ErrCode CmdOnChangeFuzzTest(shared_ptr<FileAccessServiceStub> fileAccessServiceStub, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(uint32_t)) {
        return true;
    }

    int pos = 0;
    int32_t notifyType = TypeCast<int32_t>(data, &pos);
    Uri uri(string(reinterpret_cast<const char*>(data + pos), size - pos));

    MessageParcel msg;
    MessageParcel reply;
    msg.WriteParcelable(&uri);
    msg.WriteInt32(notifyType);

    fileAccessServiceStub->CmdOnChange(msg, reply);
    return FileAccessFwk::ERR_OK;
}

ErrCode CmdRegisterNotifyFuzzTest(shared_ptr<FileAccessServiceStub> fileAccessServiceStub, const uint8_t *data,
    size_t size)
{
    Uri uri(string(reinterpret_cast<const char*>(data), size));

    MessageParcel msg;
    MessageParcel reply;
    msg.WriteParcelable(&uri);

    fileAccessServiceStub->CmdOnChange(msg, reply);
    return FileAccessFwk::ERR_OK;
}

ErrCode CmdUnregisterNotifyFuzzTest(shared_ptr<FileAccessServiceStub> fileAccessServiceStub, const uint8_t *data,
    size_t size)
{
    if (data == nullptr || size < sizeof(bool)) {
        return true;
    }

    int pos = 0;
    bool observerNotNull = TypeCast<bool>(data, &pos);
    Uri uri(string(reinterpret_cast<const char*>(data + pos), size - pos));

    MessageParcel msg;
    MessageParcel reply;
    msg.WriteParcelable(&uri);
    msg.WriteBool(observerNotNull);

    fileAccessServiceStub->CmdUnregisterNotify(msg, reply);
    return FileAccessFwk::ERR_OK;
}

ErrCode CmdGetExensionProxyFuzzTest(shared_ptr<FileAccessServiceStub> fileAccessServiceStub, const uint8_t *data,
    size_t size)
{
    int len = size >> 1;
    AAFwk::Want want;
    want.SetElementName(std::string(reinterpret_cast<const char*>(data), len),
        std::string(reinterpret_cast<const char*>(data + len), size - len));
    MessageParcel msg;
    MessageParcel reply;
    msg.WriteParcelable(&want);
    msg.WriteRemoteObject(nullptr);

    fileAccessServiceStub->CmdGetExensionProxy(msg, reply);
    return FileAccessFwk::ERR_OK;
}

} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    auto stub = std::make_shared<OHOS::FileAccessFwk::FileAccessServiceMock>();
    if (stub == nullptr) {
        return 0;
    }

    OHOS::OnRemoteRequestFuzzTest(stub, data, size);
    OHOS::CmdOnChangeFuzzTest(stub, data, size);
    OHOS::CmdRegisterNotifyFuzzTest(stub, data, size);
    OHOS::CmdUnregisterNotifyFuzzTest(stub, data, size);
    OHOS::CmdGetExensionProxyFuzzTest(stub, data, size);
    return 0;
}
