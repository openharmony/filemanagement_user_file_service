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

#ifndef I_FILE_EXT_BASE_H
#define I_FILE_EXT_BASE_H

#include <bitset>
#include <cstdint>
#include <string_ex.h>
#include <iremote_broker.h>

#include "file_extension_info.h"
#include "uri.h"

namespace OHOS {
namespace FileAccessFwk {
class IFileExtBase : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.FileAccessFwk.IFileExtBase");

    enum {
        CMD_OPEN_FILE = 1,
        CMD_CREATE_FILE = 2,
        CMD_MKDIR = 3,
        CMD_DELETE = 4,
        CMD_MOVE = 5,
        CMD_RENAME = 6,
        CMD_LIST_FILE = 7,
        CMD_GET_ROOTS = 8
    };

    virtual int OpenFile(const Uri &uri, int flags) = 0;
    virtual int CreateFile(const Uri &parentUri, const std::string &displayName, Uri &newFileUri) = 0;
    virtual int Mkdir(const Uri &parentUri, const std::string &displayName, Uri &newFileUri) = 0;
    virtual int Delete(const Uri &sourceFileUri) = 0;
    virtual int Move(const Uri &sourceFileUri, const Uri &targetParentUri, Uri &newFileUri) = 0;
    virtual int Rename(const Uri &sourceFileUri, const std::string &displayName, Uri &newFileUri) = 0;

    virtual std::vector<FileInfo> ListFile(const Uri &sourceFileUri) = 0;
    virtual std::vector<DeviceInfo> GetRoots() = 0;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // I_FILE_EXT_BASE_H
