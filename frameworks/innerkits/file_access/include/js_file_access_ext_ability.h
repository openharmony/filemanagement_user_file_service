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

#ifndef JS_FILE_ACCESS_EXT_ABILITY_H
#define JS_FILE_ACCESS_EXT_ABILITY_H

#include "file_access_ext_ability.h"
#include "file_access_extension_info.h"
#include "file_access_framework_errno.h"
#include "js_runtime.h"
#include "napi_common_fileaccess.h"
#include "native_engine/native_reference.h"
#include "native_engine/native_value.h"

namespace OHOS {
namespace FileAccessFwk {
using namespace AbilityRuntime;

using InputArgsParser = std::function<bool(NativeEngine&, NativeValue* [], size_t&)>;
using ResultValueParser = std::function<bool(NativeEngine&, NativeValue*)>;

struct CallJsParam {
    std::mutex fileOperateMutex;
    std::condition_variable fileOperateCondition;
    bool isReady = false;
    std::string funcName;
    JsRuntime *jsRuntime;
    NativeReference *jsObj;
    InputArgsParser argParser;
    ResultValueParser retParser;

    CallJsParam(const std::string &funcNameIn, JsRuntime *jsRuntimeIn, NativeReference *jsObjIn,
        InputArgsParser &argParserIn, ResultValueParser &retParserIn)
        : funcName(funcNameIn), jsRuntime(jsRuntimeIn), jsObj(jsObjIn), argParser(argParserIn), retParser(retParserIn)
    {}
};

class JsFileAccessExtAbility : public FileAccessExtAbility {
public:
    JsFileAccessExtAbility(JsRuntime &jsRuntime);
    virtual ~JsFileAccessExtAbility() override;

    static JsFileAccessExtAbility* Create(const std::unique_ptr<Runtime> &runtime);

    void Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord> &record,
        const std::shared_ptr<AppExecFwk::OHOSApplication> &application,
        std::shared_ptr<AppExecFwk::AbilityHandler> &handler,
        const sptr<IRemoteObject> &token) override;
    void OnStart(const AAFwk::Want &want) override;
    sptr<IRemoteObject> OnConnect(const AAFwk::Want &want) override;
    int OpenFile(const Uri &uri, const int flags, int &fd) override;
    int CreateFile(const Uri &parent, const std::string &displayName,  Uri &newFile) override;
    int Mkdir(const Uri &parent, const std::string &displayName, Uri &newFile) override;
    int Delete(const Uri &sourceFile) override;
    int Move(const Uri &sourceFile, const Uri &targetParent, Uri &newFile) override;
    int Rename(const Uri &sourceFile, const std::string &displayName, Uri &newFile) override;
    int ListFile(const Uri &sourceFile, std::vector<FileInfo> &fileInfo) override;
    int GetRoots(std::vector<RootInfo> &rootInfo) override;
    int Access(const Uri &uri, bool &isExist) override;

private:
    NativeValue* CallObjectMethod(const char *name, NativeValue * const *argv = nullptr, size_t argc = 0);
    int CallJsMethod(const std::string &funcName, JsRuntime &jsRuntime, NativeReference *jsObj,
        InputArgsParser argParser, ResultValueParser retParser);
    static NativeValue* FuncCallback(NativeEngine *engine, NativeCallbackInfo *info);
    void GetSrcPath(std::string &srcPath);

    JsRuntime &jsRuntime_;
    std::shared_ptr<NativeReference> jsObj_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // JS_FILE_ACCESS_EXT_ABILITY_H