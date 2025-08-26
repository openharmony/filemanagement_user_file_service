/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef USER_FILE_SERVICE_TOKEN_MOCK_H
#define USER_FILE_SERVICE_TOKEN_MOCK_H

#include "access_token.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

namespace OHOS {
namespace FileAccessFwk {
using namespace Security::AccessToken;
static constexpr int32_t DEFAULT_API_VERSION = 12;
class UserFileServiceTokenMock {
public:
    explicit UserFileServiceTokenMock();
    void SetSaToken(const std::string& process);
    bool SetHapToken(const std::string& bundle, const std::vector<std::string>& reqPerm, bool isSystemApp);
    ~UserFileServiceTokenMock();
    void SetFileManagerToken();
private:
    int32_t AllocTestHapToken(const HapInfoParams& hapInfo, HapPolicyParams& hapPolicy, AccessTokenIDEx &tokenIdEx);
    uint64_t selfTokenId_;
    uint32_t mockTokenId_;
};
}  // namespace FileAccessFwk
}  // namespace OHOS
#endif  // USER_FILE_SERVICE_TOKEN_MOCK_H
