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

#include <cstring>
#include <dirent.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <string_view>
#include <sys/stat.h>
#include <unistd.h>

#include "filemgmt_libn.h"
#include "hilog_wrapper.h"

namespace OHOS::FileManagement {

using namespace FileManagement::LibN;
using namespace std;

constexpr int DIR_DEFAULT_PERM = 0770;
constexpr int FILTER_MATCH = 1;
constexpr int FILTER_DISMATCH = 0;
constexpr int MODE_FORCE_MOVE = 0;
constexpr uint64_t TIME_CONVERT_BASE = 1000000000;
constexpr int SECOND_TO_MILLISECOND = 1000;

struct NameListArg {
    struct dirent** namelist = { nullptr };
    int direntNum = 0;
};

struct StatEntity {
    uv_stat_t stat_;
};

static void Deleter(struct NameListArg *arg)
{
    if (arg == nullptr) {
        HILOG_ERROR("invalid argument");
        return;
    }
    if (arg->namelist == nullptr) {
        HILOG_ERROR("arg->namelist is nullptr");
        return;
    }
    for (int32_t i = 0; i < arg->direntNum; i++) {
        free((arg->namelist)[i]);
        (arg->namelist)[i] = nullptr;
    }
    free(arg->namelist);
    arg->namelist = nullptr;
    delete arg;
    arg = nullptr;
}

static int32_t FilterFunc(const struct dirent *filename)
{
    if (filename == nullptr) {
        return FILTER_DISMATCH;
    }
    if (string_view(filename->d_name) == "." || string_view(filename->d_name) == "..") {
        return FILTER_DISMATCH;
    }
    return FILTER_MATCH;
}

static void fs_req_cleanup(uv_fs_t* req)
{
    uv_fs_req_cleanup(req);
    if (req) {
        delete req;
        req = nullptr;
    }
}

static int CheckFsStatByPath(const string &path, uv_fs_t* req)
{
    int ret = uv_fs_stat(nullptr, req, path.c_str(), nullptr);
    if (ret < 0) {
        HILOG_ERROR("Failed to stat file with path");
        return ret;
    }
    return ERRNO_NOERR;
}

static bool GetStat(const string &path, StatEntity &statEntity)
{
    unique_ptr<uv_fs_t, decltype(fs_req_cleanup)*> stat_req = {
        new (nothrow) uv_fs_t, fs_req_cleanup };
    if (!stat_req) {
        HILOG_ERROR("Failed to request heap memory.");
        return false;
    }
    auto err = CheckFsStatByPath(path, stat_req.get());
    if (!err) {
        statEntity = StatEntity { stat_req->statbuf };
        return true;
    }
    return false;
}

static bool CheckDir(const string &path)
{
    struct stat fileInformation;
    if (stat(path.c_str(), &fileInformation) == 0 && (fileInformation.st_mode & S_IFDIR)) {
        return true;
    } else {
        HILOG_ERROR("Failed to stat file");
    }
    return false;
}

static tuple<bool, int> Access(const string &path)
{
    unique_ptr<uv_fs_t, decltype(fs_req_cleanup)*> access_req = { new uv_fs_t, fs_req_cleanup };
    if (!access_req) {
        HILOG_ERROR("Failed to request heap memory.");
        return {false, ENOMEM};
    }
    int ret = uv_fs_access(nullptr, access_req.get(), path.c_str(), 0, nullptr);
    if (ret < 0 && (string_view(uv_err_name(ret)) != "ENOENT")) {
        HILOG_ERROR("Failed to access file by path");
        return {false, ret};
    }
    bool isExist = (ret == 0);
    return {isExist, ERRNO_NOERR};
}

static bool Mkdir(const string &path)
{
    unique_ptr<uv_fs_t, decltype(fs_req_cleanup)*> mkdir_req = { new uv_fs_t, fs_req_cleanup };
    if (!mkdir_req) {
        HILOG_ERROR("Failed to request heap memory.");
        return false;
    }
    int ret = uv_fs_mkdir(nullptr, mkdir_req.get(), path.c_str(), DIR_DEFAULT_PERM, nullptr);
    if (ret < 0) {
        HILOG_ERROR("Failed to create directory");
        return false;
    }
    if (ret == 0) {
        return true;
    }
    return false;
}

static int CopyAndDeleteFile(const string &src, const string &dest)
{
    // 获取源文件时间
    StatEntity statEntity;
    if (!GetStat(src, statEntity)) {
        HILOG_ERROR("Failed to get file stat.");
        return EINVAL;
    }
    // 拼接秒数和纳秒数
    uint64_t acTimeLong =  static_cast<uint64_t>(statEntity.stat_.st_atim.tv_sec * TIME_CONVERT_BASE +
        statEntity.stat_.st_atim.tv_nsec);
    uint64_t modTimeLong = static_cast<uint64_t>(statEntity.stat_.st_mtim.tv_sec * TIME_CONVERT_BASE +
        statEntity.stat_.st_mtim.tv_nsec);
    double acTime = static_cast<long double>(acTimeLong) / TIME_CONVERT_BASE;
    double modTime = static_cast<long double>(modTimeLong) / TIME_CONVERT_BASE;

    int ret = 0;
    uv_fs_t copyfile_req;
    ret = uv_fs_copyfile(nullptr, &copyfile_req, src.c_str(), dest.c_str(), MODE_FORCE_MOVE, nullptr);
    uv_fs_req_cleanup(&copyfile_req);

    // 设置目标文件时间
    uv_fs_t utime_req;
    uv_fs_utime(nullptr, &utime_req, dest.c_str(), acTime, modTime, nullptr);
    uv_fs_req_cleanup(&utime_req);

    if (ret < 0) {
        HILOG_ERROR("Failed to move file using copyfile interface.");
        return ret;
    }
    uv_fs_t unlink_req;
    ret = uv_fs_unlink(nullptr, &unlink_req, src.c_str(), nullptr);
    if (ret < 0) {
        HILOG_ERROR("Failed to unlink src file");
        ret = uv_fs_unlink(nullptr, &unlink_req, dest.c_str(), nullptr);
        if (ret < 0) {
            HILOG_ERROR("Failed to unlink dest file");
        }
        uv_fs_req_cleanup(&unlink_req);
        return ret;
    }
    uv_fs_req_cleanup(&unlink_req);
    return ERRNO_NOERR;
}

static int RenameFile(const string &src, const string &dest)
{
    unique_ptr<uv_fs_t, decltype(fs_req_cleanup)*> rename_req = {
        new uv_fs_t, fs_req_cleanup };
    if (!rename_req) {
        HILOG_ERROR("RenameFile: Failed to request heap memory.");
        return ENOMEM;
    }
    int ret = uv_fs_rename(nullptr, rename_req.get(), src.c_str(), dest.c_str(), nullptr);
    if (ret < 0 && (string_view(uv_err_name(ret)) == "EXDEV")) {
        HILOG_DEBUG("RenameFile: using CopyAndDeleteFile.");
        return CopyAndDeleteFile(src, dest);
    }
    if (ret < 0) {
        HILOG_ERROR("RenameFile: Failed to move file using rename syscall ret %{public}d ", ret);
        return ret;
    }
    return ERRNO_NOERR;
}

static NError RmDirent(const string &fpath)
{
    unique_ptr<uv_fs_t, decltype(fs_req_cleanup)*> scandir_req = {
        new (nothrow) uv_fs_t, fs_req_cleanup };
    if (!scandir_req) {
        HILOG_ERROR("Failed to request heap memory.");
        return NError(ENOMEM);
    }
    int ret = 0;
    ret = uv_fs_scandir(nullptr, scandir_req.get(), fpath.c_str(), 0, nullptr);
    if (ret < 0) {
        HILOG_ERROR("Failed to scandir, ret: %{public}d", ret);
        return NError(ret);
    }
    uv_dirent_t dent;
    while (uv_fs_scandir_next(scandir_req.get(), &dent) != UV_EOF) {
        string filePath = fpath + "/" + string(dent.name);
        if (dent.type == UV_DIRENT_FILE) {
            unique_ptr<uv_fs_t, decltype(fs_req_cleanup)*> unlink_req = {
                new (nothrow) uv_fs_t, fs_req_cleanup };
            if (!unlink_req) {
                HILOG_ERROR("Failed to request heap memory.");
                return NError(ENOMEM);
            }
            ret = uv_fs_unlink(nullptr, unlink_req.get(), filePath.c_str(), nullptr);
            if (ret < 0) {
                HILOG_ERROR("Failed to unlink file, ret: %{public}d", ret);
                return NError(ret);
            }
        } else if (dent.type == UV_DIRENT_DIR) {
            auto rmDirentRes = RmDirent(filePath);
            if (rmDirentRes) {
                return rmDirentRes;
            }
        }
    }
    unique_ptr<uv_fs_t, decltype(fs_req_cleanup)*> rmdir_req = {
        new (nothrow) uv_fs_t, fs_req_cleanup };
    if (!rmdir_req) {
        HILOG_ERROR("Failed to request heap memory.");
        return NError(ENOMEM);
    }
    ret = uv_fs_rmdir(nullptr, rmdir_req.get(), fpath.c_str(), nullptr);
    if (ret < 0) {
        HILOG_ERROR("Failed to rmdir empty dir, ret: %{public}d", ret);
        return NError(ret);
    }
    return NError(ERRNO_NOERR);
}

static int ScanDir(const string &path)
{
    unique_ptr<struct NameListArg, decltype(Deleter)*> pNameList = { new (nothrow) struct NameListArg, Deleter };
    if (!pNameList) {
        HILOG_ERROR("Failed to request heap memory.");
        return ENOMEM;
    }
    HILOG_INFO("RecursiveFunc: scandir path = %{private}s", path.c_str());
    return scandir(path.c_str(), &(pNameList->namelist), FilterFunc, alphasort);
}
} // OHOS::FileManagement