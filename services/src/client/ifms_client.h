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
#ifndef STORAGE_IFILE_MANAGER_CLIENT_H
#define STORAGE_IFILE_MANAGER_CLIENT_H
#include "cmd_options.h"
#include "file_info.h"
namespace OHOS {
namespace FileManagerService {
class IFmsClient {
public:
    virtual ~IFmsClient() {}
    static IFmsClient *GetFmsInstance();
    virtual int Mkdir(const std::string &name, const std::string &path) = 0;
    virtual int ListFile(const std::string &type, const std::string &path, const CmdOptions &option,
        std::vector<std::unique_ptr<FileInfo>> &fileRes) = 0;
    virtual int GetRoot(const std::string &devName, std::vector<std::unique_ptr<FileInfo>> &fileRes) = 0;
    virtual int CreateFile(const std::string &name, const std::string &path, std::string &uri) = 0;
};
} // namespace FileManagerService {
} // namespace OHOS
#endif // STORAGE_IFILE_MANAGER_CLIENT_H