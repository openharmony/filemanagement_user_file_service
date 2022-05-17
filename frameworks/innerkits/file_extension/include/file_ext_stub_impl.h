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

#ifndef OHOS_APPEXECFWK_FILEEXT_STUB_IMPL_H
#define OHOS_APPEXECFWK_FILEEXT_STUB_IMPL_H

#include <memory>
#include "file_ext_stub.h"
#include "js_file_ext_ability.h"
#include "native_engine/native_value.h"

namespace OHOS {
namespace AppExecFwk {
using AbilityRuntime::JsFileExtAbility;
class FileExtStubImpl : public FileExtStub {
public:
    explicit FileExtStubImpl(const std::shared_ptr<JsFileExtAbility>& extension, napi_env env)
        : extension_(extension) {}

    virtual ~FileExtStubImpl() {}

    int OpenFile(const Uri &uri, const std::string &mode) override;
    int CreateFile(const Uri &parentUri, const std::string &displayName,  Uri &newFileUri) override;
    int CloseFile(int fd, const std::string &uri) override;
    int Mkdir(const Uri &parentUri, const std::string &displayName, Uri &newFileUri) override;
    int Delete(const Uri &sourceFileUri) override;
    int Move(const Uri &sourceFileUri, const Uri &targetParentUri, Uri &newFileUri) override;
    int Rename(const Uri &sourceFileUri, const std::string &displayName, Uri &newFileUri) override;
private:
    std::shared_ptr<JsFileExtAbility> GetOwner();

private:
    std::shared_ptr<JsFileExtAbility> extension_;
};
} // namespace AppExecFwk
} // namespace OHOS
#endif // OHOS_APPEXECFWK_FILEEXT_STUB_IMPL_H

