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

#include "media_file_oper.h"

#include <vector>

#include "cmd_options.h"
#include "file_info.h"
#include "file_manager_service_def.h"
#include "file_manager_service_errno.h"
#include "ipc_types.h"
#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"
#include "log.h"
#include "media_data_ability_const.h"
#include "media_file_utils.h"

using namespace std;

namespace OHOS {
namespace FileManagerService {
int MediaFileOper::OperProcess(uint32_t code, MessageParcel &data, MessageParcel &reply) const
{
    int errCode = SUCCESS;
    // media process
    switch (code) {
        case Operation::MAKE_DIR: {
            string name = data.ReadString();
            string path = data.ReadString();
            errCode = Mkdir(name, path);
            break;
        }
        case Operation::LIST_FILE: {
            string devName = data.ReadString();
            string devPath = data.ReadString();
            string type = data.ReadString();
            string path = data.ReadString();
            int off = data.ReadInt32();
            int count = data.ReadInt32();
            // put fileInfo into reply
            errCode = ListFile(type, path, off, count, reply);
            break;
        }
        case Operation::CREATE_FILE: {
            string name = data.ReadString();
            string path = data.ReadString();
            string uri;
            errCode = CreateFile(name, path, uri);
            reply.WriteString(uri);
            break;
        }
        default: {
            DEBUG_LOG("not valid code %{public}d.", code);
            break;
        }
    }
    return errCode;
}

int MediaFileOper::CreateFile(const std::string &name, const std::string &path, std::string &uri) const
{
    string type = "file";
    return MediaFileUtils::DoInsert(name, path, type, uri);
}

int MediaFileOper::ListFile(const string type, const string &path, int offset, int count, MessageParcel &reply) const
{
    shared_ptr<NativeRdb::AbsSharedResultSet> result;
    int res = MediaFileUtils::DoListFile(type, path, offset, count, result);
    return  MediaFileUtils::GetFileInfoFromResult(result, reply, res);
}

int MediaFileOper::Mkdir(const string &name, const string &path) const
{
    DEBUG_LOG("MediaFileOper::mkdir path %{public}s.", path.c_str());
    return SUCCESS;
}
} // namespace FileManagerService
} // namespace OHOS