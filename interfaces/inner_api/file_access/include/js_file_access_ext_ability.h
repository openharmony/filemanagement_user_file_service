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

using InputArgsParser = std::function<bool(napi_env&, napi_value*, size_t&)>;
using ResultValueParser = std::function<bool(napi_env&, napi_value)>;

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

struct FilterParam {
    FileInfo fileInfo;
    int64_t offset;
    int64_t maxCount;
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
    int Copy(const Uri &sourceUri, const Uri &destUri, std::vector<Result> &copyResult, bool force = false) override;
    int Rename(const Uri &sourceFile, const std::string &displayName, Uri &newFile) override;
    int ListFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount, const FileFilter &filter,
        std::vector<FileInfo> &fileInfoVec) override;
    int ScanFile(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount, const FileFilter &filter,
        std::vector<FileInfo> &fileInfoVec) override;
    int GetFileInfoFromUri(const Uri &selectFile, FileInfo &fileInfo) override;
    int GetFileInfoFromRelativePath(const std::string &selectFile, FileInfo &fileInfo) override;
    int GetRoots(std::vector<RootInfo> &rootInfoVec) override;
    int Access(const Uri &uri, bool &isExist) override;
    int Query(const Uri &uri, std::vector<std::string> &columns, std::vector<std::string> &results) override;
    int StartWatcher(const Uri &uri) override;
    int StopWatcher(const Uri &uri, bool isUnregisterAll) override;
    int MoveItem(const Uri &sourceFile, const Uri &targetParent, std::vector<Result> &moveResult,
                 bool force = false) override;
    int MoveFile(const Uri &sourceFile, const Uri &targetParent, std::string &fileName, Uri &newFile) override;
private:
    template <typename T>
    struct Value {
        T data;
        int code {ERR_OK};
    };

    napi_value CallObjectMethod(const char *name, napi_value const *argv = nullptr, size_t argc = 0);
    int CallJsMethod(const std::string &funcName, JsRuntime &jsRuntime, NativeReference *jsObj,
        InputArgsParser argParser, ResultValueParser retParser);
    void GetSrcPath(std::string &srcPath);
    static int Notify(Uri &uri, NotifyType notifyType);
    static napi_value FuncCallback(napi_env env, napi_callback_info info);
    static bool ParserListFileJsResult(napi_env &env, napi_value nativeValue, Value<std::vector<FileInfo>> &result);
    static bool ParserGetRootsJsResult(napi_env &env, napi_value nativeValue, Value<std::vector<RootInfo>> &result);
    static bool ParserQueryFileJsResult(napi_env &env, napi_value nativeValue,
        Value<std::vector<std::string>> &results);
    static int MakeStringNativeArray(napi_env &env, std::vector<std::string> &inputArray, napi_value resultArray);
    static int MakeJsNativeFileFilter(napi_env &env, const FileFilter &filter, napi_value nativeFilter);
    static bool BuildFilterParam(napi_env &env, const FileFilter &filter, const FilterParam &param, napi_value *argv,
        size_t &argc);
    static napi_status GetFileInfoFromJs(napi_env &env, napi_value obj, FileInfo &fileInfo);
    static napi_status GetUriAndCodeFromJs(napi_env &env, napi_value result,
        const std::shared_ptr<Value<std::string>> &value);
    static napi_status GetFdAndCodeFromJs(napi_env &env, napi_value result, const std::shared_ptr<Value<int>> &value);
    static napi_status ConstructQueryArg(napi_env &env, napi_value *argv, size_t &argc, const Uri &uri,
        std::vector<std::string> &columns);
    static napi_status GetRootInfo(napi_env env, napi_value nativeRootInfo, RootInfo &rootInfo);
    JsRuntime &jsRuntime_;
    std::shared_ptr<NativeReference> jsObj_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // JS_FILE_ACCESS_EXT_ABILITY_H