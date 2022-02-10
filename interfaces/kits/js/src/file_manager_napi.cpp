/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "file_manager_napi.h"

#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include "file_manager_napi_def.h"
#include "file_manager_proxy.h"
#include "file_manager_service_errno.h"
#include "ifms_client.h"
#include "log.h"
namespace OHOS {
namespace FileManagerService {
using namespace std;
using namespace DistributedFS;
struct AsyncFileInfoArg {
    NRef ref_;
    vector<unique_ptr<FileInfo>> fileRes_;
    explicit AsyncFileInfoArg(NVal ref) : ref_(ref), fileRes_() {};
    ~AsyncFileInfoArg() = default;
};

struct AsyncUriArg {
    NRef ref_;
    string uri_;
    explicit AsyncUriArg(NVal ref) : ref_(ref), uri_() {};
    ~AsyncUriArg() = default;
};

tuple<bool, IFmsClient*> FileManagerNapi::GetFmsClient()
{
    if (fmsClient_ == nullptr) {
        fmsClient_ = IFmsClient::GetFmsInstance();
    }
    if (fmsClient_ == nullptr) {
        ERR_LOG("fms get instance fails");
        return make_tuple(false, nullptr);
    } else {
        return make_tuple(true, fmsClient_);
    }
}

UniError DealWithErrno(int err)
{
    unordered_map<int, int> errMap = {
        {FAIL, ESRCH},
        {E_CREATE_FAIL, EPERM},
        {E_NOEXIST, ENOENT},
        {E_EMPTYFOLDER, ENOTDIR},
        {SUCCESS, ERRNO_NOERR},
    };
    if (errMap.count(err) == 0) {
        ERR_LOG("unhandler err number %{public}d", err);
        return UniError(FAIL);
    } else {
        return UniError(errMap[err]);
    }
}

bool GetDevInfoArg(const NVal &prop, DevInfo &dev)
{
    if (prop.HasProp("name")) {
        bool ret = false;
        unique_ptr<char[]> name;
        tie(ret, name, ignore) = prop.GetProp("name").ToUTF8String();
        if (!ret) {
            return false;
        }
        dev.SetName(std::string(name.get()));
    }
    return true;
}

tuple<bool, unique_ptr<char[]>, unique_ptr<char[]>, CmdOptions> GetCreateFileArgs(napi_env env, NFuncArg &funcArg)
{
    bool succ = false;
    unique_ptr<char[]> path;
    unique_ptr<char[]> fileName;
    CmdOptions option("local", "", 0, 0, false);
    tie(succ, path, ignore) = NVal(env, funcArg[CreateFileArgs::CF_PATH]).ToUTF8String();
    if (!succ) {
        return {false, nullptr, nullptr, option};
    }
    tie(succ, fileName, ignore) = NVal(env, funcArg[CreateFileArgs::CF_FILENAME]).ToUTF8String();
    if (!succ) {
        return {false, nullptr, nullptr, option};
    }

    if (funcArg.GetArgc() < CreateFileArgs::CF_OPTION) {
        return {false, nullptr, nullptr, option};
    }

    NVal op(env, NVal(env, funcArg[CreateFileArgs::CF_OPTION]).val_);
    if (op.TypeIs(napi_function)) {
        return {true, move(path), move(fileName), option};
    }

    option.SetHasOpt(true);
    if (!op.HasProp("dev")) {
        return {true, move(path), move(fileName), option};
    }

    NVal prop(op.GetProp("dev"));
    DevInfo dev("local", "");
    if (!GetDevInfoArg(prop, dev)) {
        ERR_LOG("CreateFile func get dev para fails");
        option.SetDevInfo(dev);
        return {false, nullptr, nullptr, option};
    }

    option.SetDevInfo(dev);
    return {true, move(path), move(fileName), option};
}

napi_value FileManagerNapi::CreateFile(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(CREATE_FILE_PARA_MIN, CREATE_FILE_PARA_MAX)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    unique_ptr<char[]> path;
    unique_ptr<char[]> fileName;
    CmdOptions option;
    tie(succ, path, fileName, option) = GetCreateFileArgs(env, funcArg);
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "CreateFile func get args fails");
        return nullptr;
    }
    auto arg = make_shared<AsyncUriArg>(NVal(env, funcArg.GetThisVar()));
    auto cbExec = [arg, path = string(path.get()), fileName = string(fileName.get()), option = option]
        (napi_env env) -> UniError {
        IFmsClient* client = nullptr;
        bool succ = false;
        tie(succ, client) = GetFmsClient();
        if (!succ) {
            return UniError(ESRCH);
        }
        string uri = "";
        int err = client->CreateFile(path, fileName, option, arg->uri_);
        return DealWithErrno(err);
    };
    auto cbComplete = [arg](napi_env env, UniError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        } else {
            return NVal::CreateUTF8String(env, arg->uri_);
        }
    };
    string procedureName = "CreateFile";
    int argc = funcArg.GetArgc();
    NVal thisVar(env, funcArg.GetThisVar());
    if (argc == CREATE_FILE_PARA_MIN || (argc != CREATE_FILE_PARA_MAX && option.GetHasOpt())) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    } else {
        int cbIdx = (!option.GetHasOpt() ?
            CreateFileArgs::CF_CALLBACK_WITHOUT_OP : CreateFileArgs::CF_CALLBACK_WITH_OP);
        NVal cb(env, funcArg[cbIdx]);
        return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
    }
}

void CreateFileArray(napi_env env, shared_ptr<AsyncFileInfoArg> arg)
{
    for (unsigned int i = 0; i < arg->fileRes_.size(); i++) {
        NVal obj = NVal::CreateObject(env);
        unique_ptr<FileInfo> &res = arg->fileRes_[i];
        obj.AddProp("name", NVal::CreateUTF8String(env, res->GetName()).val_);
        obj.AddProp("path", NVal::CreateUTF8String(env, res->GetPath()).val_);
        obj.AddProp("type", NVal::CreateUTF8String(env, res->GetType()).val_);
        obj.AddProp("size", NVal::CreateInt64(env, res->GetSize()).val_);
        obj.AddProp("added_time", NVal::CreateInt64(env, res->GetAddedTime()).val_);
        obj.AddProp("modified_time", NVal::CreateInt64(env, res->GetModifiedTime()).val_);
        napi_set_property(env, arg->ref_.Deref(env).val_, NVal::CreateInt32(env, i).val_, obj.val_);
    }
}

bool GetRootArgs(napi_env env, NFuncArg &funcArg, CmdOptions &option)
{
    NVal op(env, NVal(env, funcArg[GetRootArgs::GR_OPTION]).val_);
    if (op.TypeIs(napi_function)) {
        return true;
    }

    option.SetHasOpt(true);
    if (!op.HasProp("dev")) {
        return true;
    }

    NVal prop(op.GetProp("dev"));
    DevInfo dev("local", "");
    if (!GetDevInfoArg(prop, dev)) {
        option.SetDevInfo(dev);
        return false;
    }

    option.SetDevInfo(dev);
    return true;
}

napi_value FileManagerNapi::GetRoot(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(GET_ROOT_PARA_MIN, GET_ROOT_PARA_MAX)) {
        UniError(EINVAL).ThrowErr(env, "Number of argments unmatched");
        return nullptr;
    }

    CmdOptions option("local", "", 0, 0, false);
    if (funcArg.GetArgc() != 0) {
        if (!GetRootArgs(env, funcArg, option)) {
            UniError(EINVAL).ThrowErr(env, "GetRoot func get dev para fails");
            return nullptr;
        }
    }

    napi_value fileArr;
    napi_create_array(env, &fileArr);
    auto arg = make_shared<AsyncFileInfoArg>(NVal(env, fileArr));
    auto cbExec = [option = option, arg] (napi_env env) -> UniError {
        IFmsClient* client = nullptr;
        bool succ = false;
        tie(succ, client) = GetFmsClient();
        if (!succ) {
            return UniError(ESRCH);
        }
        int err = client->GetRoot(option, arg->fileRes_);
        return DealWithErrno(err);
    };
    auto cbComplete = [arg](napi_env env, UniError err) -> NVal {
        CreateFileArray(env, arg);
        if (err) {
            return { env, err.GetNapiErr(env) };
        } else {
            return NVal(env, arg->ref_.Deref(env).val_);
        }
    };
    string procedureName = "GetRoot";
    int argc = funcArg.GetArgc();
    NVal thisVar(env, funcArg.GetThisVar());
    if (argc == GET_ROOT_PARA_MIN || (argc != GET_ROOT_PARA_MAX && option.GetHasOpt())) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    } else {
        int cbIdx = (!option.GetHasOpt() ? GetRootArgs::GR_CALLBACK_WITHOUT_OP : GetRootArgs::GR_CALLBACK_WITH_OP);
        NVal cb(env, funcArg[cbIdx]);
        return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
    }
}

bool GetListFileOption(const NVal &argv, CmdOptions &option)
{
    bool ret = false;
    if (argv.HasProp("dev")) {
        NVal prop(argv.GetProp("dev"));
        DevInfo dev("local", "");
        if (!GetDevInfoArg(prop, dev)) {
            option.SetDevInfo(dev);
            return false;
        }
        option.SetDevInfo(dev);
    }
    if (argv.HasProp("offset")) {
        int64_t offset;
        tie(ret, offset) = argv.GetProp("offset").ToInt64();
        if (!ret) {
            ERR_LOG("ListFileArgs LF_OPTION offset para fails");
            return false;
        }
        option.SetOffset(offset);
    }
    if (argv.HasProp("count")) {
        int64_t count;
        tie(ret, count) = argv.GetProp("count").ToInt64();
        if (!ret) {
            ERR_LOG("ListFileArgs LF_OPTION count para fails");
            return false;
        }
        option.setCount(count);
    }
    return true;
}

tuple<bool, unique_ptr<char[]>, unique_ptr<char[]>, CmdOptions> GetListFileArg(
    napi_env env, NFuncArg &funcArg)
{
    bool succ = false;
    unique_ptr<char[]> path;
    unique_ptr<char[]> type;
    CmdOptions option("local", "", 0, 0, false);
    tie(succ, path, ignore) = NVal(env, funcArg[ListFileArgs::LF_PATH]).ToUTF8String();
    if (!succ) {
        ERR_LOG("ListFileArgs LF_PATH para fails");
        return {false, nullptr, nullptr, option};
    }
    tie(succ, type, ignore) = NVal(env, funcArg[ListFileArgs::LF_TYPE]).ToUTF8String();
    if (!succ) {
        ERR_LOG("ListFileArgs LF_TYPE para fails");
        return {false, nullptr, nullptr, option};
    }

    NVal op(env, NVal(env, funcArg[ListFileArgs::LF_OPTION]).val_);
    if (op.TypeIs(napi_function)) {
        return {true, move(type), move(path), option};
    }
    option.SetHasOpt(true);
    if (!GetListFileOption(op, option)) {
        return {false, nullptr, nullptr, option};
    }
    return {true, move(type), move(path), option};
}

napi_value FileManagerNapi::ListFile(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(LIST_FILE_PARA_MIN, LIST_FILE_PARA_MAX)) {
        UniError(EINVAL).ThrowErr(env, "Number of argments unmatched");
        return nullptr;
    }
    bool succ = false;
    unique_ptr<char[]> type;
    unique_ptr<char[]> path;
    CmdOptions option;
    tie(succ, type, path, option) = GetListFileArg(env, funcArg);
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Get argments fails");
        return nullptr;
    }
    napi_value fileArr;
    napi_create_array(env, &fileArr);
    auto arg = make_shared<AsyncFileInfoArg>(NVal(env, fileArr));
    auto cbExec = [type = string(type.get()), path = string(path.get()), option, arg]
        (napi_env env) -> UniError {
        IFmsClient* client = nullptr;
        bool succ = false;
        tie(succ, client) = GetFmsClient();
        if (!succ) {
            return UniError(ESRCH);
        }
        int err = client->ListFile(type, path, option, arg->fileRes_);
        return DealWithErrno(err);
    };

    auto cbComplete = [arg](napi_env env, UniError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        } else {
            CreateFileArray(env, arg);
            return NVal(env, arg->ref_.Deref(env).val_);
        }
    };
    string procedureName = "ListFile";
    int argc = funcArg.GetArgc();
    NVal thisVar(env, funcArg.GetThisVar());
    if (argc == LIST_FILE_PARA_MIN || (argc != LIST_FILE_PARA_MAX && option.GetHasOpt())) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    } else {
        int cbIdx = ((!option.GetHasOpt()) ? ListFileArgs::LF_CALLBACK_WITHOUT_OP : ListFileArgs::LF_CALLBACK_WITH_OP);
        NVal cb(env, funcArg[cbIdx]);
        return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
    }
}

napi_value FileManagerNapi::Mkdir(napi_env env, napi_callback_info info)
{
    return NVal::CreateUndefined(env).val_;
}

bool FileManagerNapi::Export()
{
    return exports_.AddProp( {
        NVal::DeclareNapiFunction("listFile", ListFile),
        NVal::DeclareNapiFunction("createFile", CreateFile),
        NVal::DeclareNapiFunction("getRoot", GetRoot),
    });
}

string FileManagerNapi::GetClassName()
{
    return FileManagerNapi::className_;
}

FileManagerNapi::FileManagerNapi(napi_env env, napi_value exports) : NExporter(env, exports) {}

FileManagerNapi::~FileManagerNapi() {}
} // namespace FileManagerService
} // namespace OHOS
