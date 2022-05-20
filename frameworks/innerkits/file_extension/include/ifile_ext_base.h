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

#ifndef OHOS_APPEXECFWK_I_FILEEXTBASE_H
#define OHOS_APPEXECFWK_I_FILEEXTBASE_H

#include <bitset>
#include <string_ex.h>
#include <iremote_broker.h>
#include "uri.h"

namespace OHOS {
namespace AppExecFwk {
struct FileInfo
{
    Uri uri;
    std::string fileName;
    std::string mode;
    size_t size;
    int64_t mtime;
    std::string mimiType;
    std::bitset<32> flags;
};

struct DeviceInfo
{
    Uri uri;
    std::string displayname;
    std::string deviceId;
    std::bitset<32> flags;
};

const std::string ATTRIBUTE_COLUMN_SUMMARY = "summary";
const std::string ATTRIBUTE_COLUMN_LAST_MODIFIED = "last_modified";
const std::string ATTRIBUTE_COLUMN_ICON = "icon";
const std::string ATTRIBUTE_COLUMN_FLAGS = "flags";
const std::string ATTRIBUTE_COLUMN_SIZE = "size";

const std::bitset<32> FLAG_SUPPORTS_THUMBNAIL = 1;
const std::bitset<32> FLAG_SUPPORTS_WRITE = 1 << 1;
const std::bitset<32> FLAG_SUPPORTS_DELETE = 1 << 2;
const std::bitset<32> FLAG_DIR_SUPPORTS_CREATE = 1 << 3;
const std::bitset<32> FLAG_DIR_PREFERS_LAST_MODIFIED = 1 << 4;
const std::bitset<32> FLAG_SUPPORTS_RENAME = 1 << 5;
const std::bitset<32> FLAG_SUPPORTS_COPY = 1 << 6;
const std::bitset<32> FLAG_SUPPORTS_MOVE = 1 << 7;
const std::bitset<32> FLAG_SUPPORTS_REMOVE = 1 << 8;

class IFileExtBase : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.AppExecFwk.IFileExtBase");

    enum {
        CMD_OPEN_FILE = 1,
        CMD_CLOSE_FILE = 2,
        CMD_CREATE_FILE = 3,
        CMD_MKDIR = 4,
        CMD_DELETE = 5,
        CMD_MOVE = 6,
        CMD_RENAME = 7
    };

    virtual int OpenFile(const Uri &uri, const std::string &mode) = 0;
    virtual int CloseFile(int fd, const std::string &uri) = 0;
    virtual int CreateFile(const Uri &parentUri, const std::string &displayName, Uri &newFileUri) = 0;
    virtual int Mkdir(const Uri &parentUri, const std::string &displayName, Uri &newFileUri) = 0;
    virtual int Delete(const Uri &sourceFileUri) = 0;
    virtual int Move(const Uri &sourceFileUri, const Uri &targetParentUri, Uri &newFileUri) = 0;
    virtual int Rename(const Uri &sourceFileUri, const std::string &displayName, Uri &newFileUri) = 0;
};
} // namespace AppExecFwk
} // namespace OHOS
#endif // OHOS_APPEXECFWK_I_FILEEXTBASE_H
