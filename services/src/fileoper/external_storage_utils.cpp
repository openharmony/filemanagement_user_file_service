/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "external_storage_utils.h"

#include <cstring>
#include <unordered_map>

#include <cerrno>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "file_manager_service_def.h"
#include "file_manager_service_errno.h"
#include "log.h"

namespace OHOS {
namespace FileManagerService {
static bool GetFileInfo(const std::string &path, const std::string &name, FileInfo &fileInfo)
{
    std::string fullPath("");
    fullPath.append(path).append("/").append(name);
    struct stat st;
    if (lstat(fullPath.c_str(), &st) != 0) {
        ERR_LOG("check file info fail.");
        return false;
    }
    std::string fPath(path.c_str());
    std::string fName(name.c_str());

    fileInfo.SetPath(fPath);
    std::string type = "file";
    fileInfo.SetType(type);
    fileInfo.SetName(fName);
    fileInfo.SetSize(st.st_size);
    fileInfo.SetAddedTime(static_cast<long>(st.st_ctim.tv_sec));
    fileInfo.SetModifiedTime(static_cast<long>(st.st_mtim.tv_sec));
    return true;
}

static bool ConvertUriToAbsolutePath(const std::string &uri, std::string &path)
{
    // TODO convert uri to absolute path
    path = "/data/media";
    return true;
}

int ExternalStorageUtils::DoListFile(const std::string &type, const std::string &uri, MessageParcel &reply)
{
    std::string path;
    if (!ConvertUriToAbsolutePath(uri, path)) {
        ERR_LOG("invalid uri[%{public}s].", uri.c_str());
        return E_NOEXIST;
    }

    DIR *dir = opendir(path.c_str());
    if (!dir) {
        ERR_LOG("opendir path[%{public}s] fail.", path.c_str());
        return E_NOEXIST;
    }
    std::vector<FileInfo> fileList;
    for (struct dirent *ent = readdir(dir); ent != nullptr; ent = readdir(dir)) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }
        FileInfo fileInfo;
        if (!GetFileInfo(path, ent->d_name, fileInfo)) {
            continue;
        }
        fileList.push_back(fileInfo);
    }
    closedir(dir);
    int fileCount = static_cast<int32_t>(fileList.size());
    reply.WriteInt32(fileCount);
    if (fileCount == 0) {
        return E_EMPTYFOLDER;
    }
    for (auto file : fileList) {
        reply.WriteString(file.GetPath());
        reply.WriteString(file.GetType());
        reply.WriteString(file.GetName());
        reply.WriteInt64(file.GetSize());
        reply.WriteInt64(file.GetAddedTime());
        reply.WriteInt64(file.GetModifiedTime());
    }
    return SUCCESS;
}

int ExternalStorageUtils::DoCreateFile(const std::string &uri, const std::string &name, MessageParcel &reply)
{
    std::string path;
    if (!ConvertUriToAbsolutePath(uri, path)) {
        ERR_LOG("invalid uri[%{public}s].", uri.c_str());
        return E_NOEXIST;
    }

    path.append("/").append(name);
    if (access(path.c_str(), F_OK) == 0) {
        ERR_LOG("target file[%{public}s] exist.", path.c_str());
        return E_CREATE_FAIL;
    }

    int fd = open(path.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0771);
    if (fd == -1) {
        ERR_LOG("create file[%{public}s] fail.", path.c_str());
        return E_CREATE_FAIL;
    }
    close(fd);

    reply.WriteString(uri);
    return SUCCESS;
}

bool ExternalStorageUtils::PopFileInfo(FileInfo &fileInfo, MessageParcel &reply)
{
    std::string path;
    std::string type;
    std::string name;
    int64_t size = 0;
    int64_t at = 0;
    int64_t mt = 0;

    reply.ReadString(path);
    reply.ReadString(type);
    reply.ReadString(name);
    reply.ReadInt64(size);
    reply.ReadInt64(at);
    reply.ReadInt64(mt);
    fileInfo = FileInfo(name, path, type);
    fileInfo.SetSize(size);
    fileInfo.SetAddedTime(at);
    fileInfo.SetModifiedTime(mt);
    return true;
}
} // namespace FileManagerService
} // namespace OHOS