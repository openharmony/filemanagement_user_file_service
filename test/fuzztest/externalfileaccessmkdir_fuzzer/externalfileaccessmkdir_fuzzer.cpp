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
#include "externalfileaccessmkdir_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>

#include "extension_base.h"
#include "extension_context.h"
#include "message_parcel.h"
#include "file_access_ext_base_stub.h"
#include "file_access_ext_stub_impl.h"
#include "file_access_ext_ability.h"
#include "js_file_access_ext_ability.h"
#include "js_runtime.h"
#include "securec.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"
#include "accesstoken_kit.h"

namespace OHOS {
using namespace std;
using namespace OHOS;
using namespace FileAccessFwk;
using namespace AbilityRuntime;

void SetNativeToken()
{
    uint64_t tokenId;
    const char *perms[] = {
        "ohos.permission.FILE_ACCESS_MANAGER",
        "ohos.permission.GET_BUNDLE_INFO_PRIVILEGED",
        "ohos.permission.CONNECT_FILE_ACCESS_EXTENSION"
    };
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 3,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .aplStr = "system_core",
    };

    infoInstance.processName = "ExternalFileAccessMkdirFuzzTest";
    tokenId = GetAccessTokenId(&infoInstance);
    const uint64_t systemAppMask = (static_cast<uint64_t>(1) << 32);
    tokenId |= systemAppMask;
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

bool ExternalFileAccessMkdirFuzzTest(const uint8_t *data, size_t size)
{
    SetNativeToken();
    // CMD_MKDIR
    uint32_t code = 3;
    MessageParcel datas;
    datas.WriteInterfaceToken(FileAccessExtBaseStub::GetDescriptor());
    datas.WriteBuffer(reinterpret_cast<const char*>(data), size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    auto fileAccessExtAbility = FileAccessExtAbility::Create(nullptr);
    auto fileAccessExtAbilitySharePtr = std::shared_ptr<FileAccessExtAbility>(fileAccessExtAbility);

    sptr<FileAccessExtStubImpl> fileAccessExtStubObj(new (std::nothrow) FileAccessExtStubImpl(
        fileAccessExtAbilitySharePtr, nullptr));

    if (fileAccessExtStubObj) {
        fileAccessExtStubObj->OnRemoteRequest(code, datas, reply, option);
    }

    fileAccessExtAbility = nullptr;
    fileAccessExtStubObj = nullptr;

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::ExternalFileAccessMkdirFuzzTest(data, size);
    return 0;
}
