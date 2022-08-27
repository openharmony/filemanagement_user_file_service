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

#ifndef I_FILE_ACCESS_EXT_BASE_H
#define I_FILE_ACCESS_EXT_BASE_H

#include <bitset>
#include <cstdint>
#include <string_ex.h>
#include <iremote_broker.h>

#include "file_access_extension_info.h"
#include "file_access_framework_errno.h"
#include "ifile_access_notify.h"
#include "uri.h"

namespace OHOS {
namespace FileAccessFwk {
class IFileAccessExtBase : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.FileAccessFwk.IFileAccessExtBase");

    enum {
        CMD_OPEN_FILE = 1,
        CMD_CREATE_FILE,
        CMD_MKDIR,
        CMD_DELETE,
        CMD_MOVE,
        CMD_RENAME,
        CMD_LIST_FILE,
        CMD_GET_ROOTS,
        CMD_IS_FILE_EXIST,
        CMD_REGISTER_NOTIFY,
        CMD_UNREGISTER_NOTIFY
    };

    virtual int OpenFile(const Uri &uri, const int flags) = 0;
    virtual int CreateFile(const Uri &parent, const std::string &displayName, Uri &newFile) = 0;
    virtual int Mkdir(const Uri &parent, const std::string &displayName, Uri &newFile) = 0;
    virtual int Delete(const Uri &sourceFile) = 0;
    virtual int Move(const Uri &sourceFile, const Uri &targetParent, Uri &newFile) = 0;
    virtual int Rename(const Uri &sourceFile, const std::string &displayName, Uri &newFile) = 0;

    virtual std::vector<FileInfo> ListFile(const Uri &sourceFile) = 0;
    virtual std::vector<RootInfo> GetRoots() = 0;
    virtual int IsFileExist(const Uri &uri, bool &isExist) = 0;
    virtual int RegisterNotify(sptr<IFileAccessNotify> &notify) = 0;
    virtual int UnregisterNotify(sptr<IFileAccessNotify> &notify) = 0;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // I_FILE_ACCESS_EXT_BASE_H
