/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "recent_n_exporter.h"

#include <unistd.h>
#include <utime.h>
#include <sys/stat.h>

#include "access_token.h"
#include "accesstoken_kit.h"
#include "file_uri.h"
#include "file_utils.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace FileManagement {
namespace Recent {
using namespace std;
using namespace LibN;
using namespace AppFileService::ModuleFileUri;

static bool CheckPermission(const std::string &permission)
{
    Security::AccessToken::AccessTokenID tokenCaller = IPCSkeleton::GetCallingTokenID();
    return Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller, permission) ==
        Security::AccessToken::PermissionState::PERMISSION_GRANTED;
}

napi_value RecentNExporter::AddRecentFile(napi_env env, napi_callback_info cbinfo)
{
    if (!CheckPermission(FILE_ACCESS_PERMISSION)) {
        HILOG_ERROR("Failed to get file access permission");
        NError(E_PERMISSION).ThrowErr(env);
        return nullptr;
    }
    NFuncArg funcArg(env, cbinfo);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        HILOG_ERROR("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    auto [succ, uri, ignore] = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    FileUri fileUri(string(uri.get()));
    auto filePath = fileUri.GetPath();
    struct stat statBuf;
    if (stat(filePath.c_str(), &statBuf) < 0) {
        HILOG_ERROR("Failed to stat uri, errno=%{public}d", errno);
        NError(errno).ThrowErr(env);
        return nullptr;
    }
    string recentFilePath = RECENT_PATH + to_string(statBuf.st_dev) + "_" + to_string(statBuf.st_ino);
    auto accessRet = access(recentFilePath.c_str(), F_OK);
    if (accessRet == 0) {
        struct utimbuf newTime;
        newTime.actime = statBuf.st_atime;
        newTime.modtime = time(nullptr);
        if (utime(recentFilePath.c_str(), &newTime) < 0) {
            HILOG_ERROR("Failed to utime uri, errno=%{public}d", errno);
            NError(errno).ThrowErr(env);
            return nullptr;
        }
    } else if (accessRet < 0 && errno == ENOENT) {
        if (symlink(filePath.c_str(), recentFilePath.c_str()) < 0) {
            HILOG_ERROR("Failed to symlink uri, errno=%{public}d", errno);
            NError(errno).ThrowErr(env);
            return nullptr;
        }
    } else {
        HILOG_ERROR("Failed to access uri, errno=%{public}d", errno);
        NError(errno).ThrowErr(env);
        return nullptr;
    }
    return NVal::CreateUndefined(env).val_;
}

napi_value RecentNExporter::RemoveRecentFile(napi_env env, napi_callback_info cbinfo)
{
    if (!CheckPermission(FILE_ACCESS_PERMISSION)) {
        HILOG_ERROR("Failed to get file access permission");
        NError(E_PERMISSION).ThrowErr(env);
        return nullptr;
    }
    NFuncArg funcArg(env, cbinfo);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        HILOG_ERROR("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    auto [succ, uri, ignore] = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    FileUri fileUri(string(uri.get()));
    auto filePath = fileUri.GetPath();
    struct stat statBuf;
    if (stat(filePath.c_str(), &statBuf) < 0) {
        HILOG_ERROR("Failed to stat uri, errno=%{public}d", errno);
        NError(errno).ThrowErr(env);
        return nullptr;
    }
    string recentFilePath = RECENT_PATH + to_string(statBuf.st_dev) + "_" + to_string(statBuf.st_ino);
    if (access(recentFilePath.c_str(), F_OK) < 0) {
        HILOG_ERROR("Failed to access uri, errno=%{public}d", errno);
        NError(errno).ThrowErr(env);
        return nullptr;
    }
    if (unlink(recentFilePath.c_str()) < 0) {
        HILOG_ERROR("Failed to unlink uri, errno=%{public}d", errno);
        NError(errno).ThrowErr(env);
    }
    return nullptr;
}

static void Deleter(struct NameListArg *arg)
{
    for (int i = 0; i < arg->direntNum; i++) {
        free((arg->namelist)[i]);
        (arg->namelist)[i] = nullptr;
    }
    free(arg->namelist);
}

static int64_t GetFileMtime(const string &fileName)
{
    string filePath = RECENT_PATH + fileName;
    struct stat statBuf;
    if (stat(filePath.c_str(), &statBuf) < 0) {
        HILOG_ERROR("Failed to stat uri, errno=%{public}d, fileName=%{public}s", errno, fileName.c_str());
        return errno;
    }
    return static_cast<int64_t>(statBuf.st_mtime);
}

static string GetName(const string &path)
{
    auto pos = path.find_last_of('/');
    if (pos == string::npos) {
        HILOGE("Failed to split filename from path, path: %{public}s", path.c_str());
    }
    return path.substr(pos + 1);
}

static int SortReceneFile(const struct dirent **a, const struct dirent **b)
{
    return GetFileMtime(string((*a)->d_name)) > GetFileMtime(string((*b)->d_name));
}

static int FilterFunc(const struct dirent *filename)
{
    if (string_view(filename->d_name) == "." || string_view(filename->d_name) == "..") {
        return false;
    }
    return true;
}

static napi_value GetFileInfo(napi_env env, const string &filePath, const struct stat &statBuf)
{
    FileUri fileUri(filePath);
    NVal obj = NVal::CreateObject(env);
    obj.AddProp({
        NVal::DeclareNapiProperty("uri", NVal::CreateUTF8String(env, fileUri.ToString()).val_),
        NVal::DeclareNapiProperty("srcPath", NVal::CreateUTF8String(env, filePath).val_),
        NVal::DeclareNapiProperty("fileName", NVal::CreateUTF8String(env, GetName(filePath)).val_),
        NVal::DeclareNapiProperty("size", NVal::CreateInt64(env, statBuf.st_size).val_),
        NVal::DeclareNapiProperty("mtime", NVal::CreateInt64(env, statBuf.st_mtime).val_),
        NVal::DeclareNapiProperty("ctime", NVal::CreateInt64(env, statBuf.st_ctime).val_),
        NVal::DeclareNapiProperty("mode", NVal::CreateInt64(env, statBuf.st_mode).val_),
    });
    return obj.val_;
}

static napi_value ListFileCore(napi_env env)
{
    unique_ptr<struct NameListArg, decltype(Deleter)*> pNameList = { new (nothrow) struct NameListArg, Deleter };
    if (!pNameList) {
        HILOG_ERROR("Failed to request heap memory.");
        NError(ENOMEM).ThrowErr(env);
        return nullptr;
    }
    pNameList->direntNum = scandir(RECENT_PATH.c_str(), &(pNameList->namelist), FilterFunc, SortReceneFile);
    if (pNameList->direntNum < 0) {
        HILOG_ERROR("Failed to scan dir, errno=%{public}d", errno);
        NError(errno).ThrowErr(env);
        return nullptr;
    }
    napi_value res = nullptr;
    if (napi_create_array(env, &res) != napi_ok) {
        HILOG_ERROR("Failed to create array");
        NError(UNKROWN_ERR).ThrowErr(env);
        return nullptr;
    }
    auto buf = CreateUniquePtr<char[]>(BUF_SIZE);
    for (int i = 0; i < pNameList->direntNum; ++i) {
        string recentFilePath = RECENT_PATH + string((*(pNameList->namelist[i])).d_name);
        if (i < MAX_RECENT_SIZE) {
            auto readLinkRes = readlink(recentFilePath.c_str(), buf.get(), BUF_SIZE);
            if (readLinkRes < 0) {
                HILOG_ERROR("Failed to readlink uri, errno=%{public}d", errno);
                NError(errno).ThrowErr(env);
                return nullptr;
            }
            struct stat statBuf;
            if (lstat(recentFilePath.c_str(), &statBuf) < 0) {
                HILOG_ERROR("Failed to lstat file, errno=%{public}d", errno);
                NError(errno).ThrowErr(env);
                return nullptr;
            }
            if (napi_set_element(env, res, i, GetFileInfo(env, string(buf.get(), readLinkRes), statBuf)) != napi_ok) {
                HILOG_ERROR("Failed to set element");
                NError(UNKROWN_ERR).ThrowErr(env);
                return nullptr;
            }
        } else {
            if (unlink(recentFilePath.c_str()) < 0) {
                HILOG_ERROR("Failed to unlink file, errno=%{public}d", errno);
                NError(errno).ThrowErr(env);
                return nullptr;
            }
        }
    }
    return res;
}

napi_value RecentNExporter::ListRecentFile(napi_env env, napi_callback_info cbinfo)
{
    if (!CheckPermission(FILE_ACCESS_PERMISSION)) {
        HILOG_ERROR("Failed to get file access permission");
        NError(E_PERMISSION).ThrowErr(env);
        return nullptr;
    }
    NFuncArg funcArg(env, cbinfo);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        HILOG_ERROR("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    return ListFileCore(env);
}

bool RecentNExporter::Export()
{
    return exports_.AddProp({
        NVal::DeclareNapiFunction("add", AddRecentFile),
        NVal::DeclareNapiFunction("remove", RemoveRecentFile),
        NVal::DeclareNapiFunction("listFile", ListRecentFile),
    });
}

string RecentNExporter::GetClassName()
{
    return RecentNExporter::className;
}

RecentNExporter::RecentNExporter(napi_env env, napi_value exports) : NExporter(env, exports) {}

RecentNExporter::~RecentNExporter() {}
} // namespace Recent
} // namespace FileManagement
} // namespace OHOS
