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

#include "external_storage_oper.h"

#include <vector>

#include "cmd_response.h"
#include "external_storage_utils.h"
#include "file_info.h"
#include "file_manager_service_def.h"
#include "file_manager_service_errno.h"
#include "log.h"

using namespace std;
namespace OHOS {
namespace FileManagerService {
int ExternalStorageOper::OperProcess(uint32_t code, MessageParcel &data, MessageParcel &reply) const
{
    DEBUG_LOG("ExternalStorageOper::OperProcess");
    int errCode = SUCCESS;
    switch (code) {
        case Operation::LIST_FILE: {
            std::string devName = data.ReadString();
            std::string devPath = data.ReadString();
            std::string type = data.ReadString();
            std::string path = data.ReadString();
            int64_t offset = data.ReadInt64();
            int64_t count = data.ReadInt64();

            CmdOptions option(devName, devPath, offset, count, true);
            errCode = this->ListFile(type, path, option, reply);
            break;
        }
        case Operation::CREATE_FILE: {
            std::string name = data.ReadString();
            std::string uri = data.ReadString();
            errCode = this->CreateFile(uri, name, reply);
            break;
        }
        case Operation::GET_ROOT: {
            string path = data.ReadString();
            // name for extension
            string name = "name";
            errCode = GetRoot(name, path, reply);
            break;
        }
        default: {
            DEBUG_LOG("not valid code %{public}d.", code);
            break;
        }
    }
    return errCode;
}

int ExternalStorageOper::GetRoot(const std::string &name, const std::string &path, MessageParcel &reply) const
{
    std::vector<std::shared_ptr<FileInfo>> fileList;
    int ret = ExternalStorageUtils::DoGetRoot(name, path, fileList);
    CmdResponse cmdResponse;
    cmdResponse.SetErr(ret);
    cmdResponse.SetFileInfoList(fileList);
    reply.WriteParcelable(&cmdResponse);
    return ret;
}

int ExternalStorageOper::CreateFile(const std::string &uri, const std::string &name, MessageParcel &reply) const
{
    std::string resultUir;
    int ret = ExternalStorageUtils::DoCreateFile(uri, name, resultUir);
    CmdResponse cmdResponse;
    cmdResponse.SetErr(ret);
    cmdResponse.SetUri(resultUir);
    reply.WriteParcelable(&cmdResponse);
    return ret;
}

int ExternalStorageOper::ListFile(const std::string &type, const std::string &uri, const CmdOptions &option,
    MessageParcel &reply) const
{
    std::vector<std::shared_ptr<FileInfo>> fileList;
    int ret = ExternalStorageUtils::DoListFile(type, uri, option, fileList);
    CmdResponse cmdResponse;
    cmdResponse.SetErr(ret);
    cmdResponse.SetFileInfoList(fileList);
    reply.WriteParcelable(&cmdResponse);
    return ret;
}
} // namespace FileManagerService
} // namespace OHOS