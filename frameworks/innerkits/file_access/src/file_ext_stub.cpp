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

#include "file_ext_stub.h"

#include "hilog_wrapper.h"

namespace OHOS {
namespace FileAccessFwk {
FileExtStub::FileExtStub()
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
}

FileExtStub::~FileExtStub()
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    stubFuncMap_.clear();
}

int FileExtStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    HILOG_INFO("tag dsa %{public}s Received stub message: %{public}d", __func__, code);
    std::u16string descriptor = FileExtStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_INFO("tag dsa local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    const auto &itFunc = stubFuncMap_.find(code);
    if (itFunc != stubFuncMap_.end()) {
        return (this->*(itFunc->second))(data, reply);
    }

    HILOG_INFO("tag dsa %{public}s remote request unhandled: %{public}d", __func__, code);
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}
} // namespace FileAccessFwk
} // namespace OHOS
