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

#ifndef JS_FILE_EXT_ABILITY_H
#define JS_FILE_EXT_ABILITY_H

#include "file_access_ext_ability.h"

#include "js_runtime.h"
#include "native_engine/native_reference.h"
#include "native_engine/native_value.h"

namespace OHOS {
namespace FileAccessFwk {
using namespace AbilityRuntime;
class JsFileAccessExtAbility : public FileAccessExtAbility {
public:
    JsFileAccessExtAbility(JsRuntime& jsRuntime);
    virtual ~JsFileAccessExtAbility() override;

    static JsFileAccessExtAbility* Create(const std::unique_ptr<Runtime>& runtime);

    void Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord> &record,
        const std::shared_ptr<AppExecFwk::OHOSApplication> &application,
        std::shared_ptr<AppExecFwk::AbilityHandler> &handler,
        const sptr<IRemoteObject> &token) override;

    void OnStart(const AAFwk::Want &want) override;

    sptr<IRemoteObject> OnConnect(const AAFwk::Want &want) override;

private:
    NativeValue* CallObjectMethod(const char *name, NativeValue * const *argv = nullptr, size_t argc = 0);
    void GetSrcPath(std::string &srcPath);

    JsRuntime& jsRuntime_;
    std::shared_ptr<NativeReference> jsObj_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // JS_FILE_EXT_ABILITY_H