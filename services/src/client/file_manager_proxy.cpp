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
#include "file_manager_proxy.h"

#include "cmd_response.h"
#include "file_info.h"
#include "file_manager_service_def.h"
#include "file_manager_service_errno.h"
#include "file_manager_service_stub.h"
#include "log.h"
#include "media_file_utils.h"

using namespace std;

namespace OHOS {
namespace FileManagerService {
static int GetCmdResponse(MessageParcel &reply, sptr<CmdResponse> &cmdResponse)
{
    cmdResponse = reply.ReadParcelable<CmdResponse>();
    if (cmdResponse == nullptr) {
        ERR_LOG("Unmarshalling cmdResponse fail");
        return FAIL;
    }
    return cmdResponse->GetErr();
}

FileManagerProxy::FileManagerProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IFileManagerService>(impl) {}
int FileManagerProxy::GetRoot(const CmdOptions &option, vector<unique_ptr<FileInfo>> &fileRes)
{
    CmdOptions op(option);
    if (op.GetDevInfo().GetName() == "external_storage") {
        MessageParcel data;
        data.WriteString(op.GetDevInfo().GetName());
        MessageParcel reply;
        MessageOption messageOption;
        int code = (Equipment::EXTERNAL_STORAGE << EQUIPMENT_SHIFT) | Operation::GET_ROOT;
        int err = Remote()->SendRequest(code, data, reply, messageOption);
        if (err != ERR_NONE) {
            ERR_LOG("GetRoot inner error send request fail %{public}d", err);
            return FAIL;
        }
        sptr<CmdResponse> cmdResponse;
        err = GetCmdResponse(reply, cmdResponse);
        if (err != ERR_NONE) {
            return err;
        }
        fileRes = cmdResponse->GetFileInfoList();
        return err;
    } else {
        unique_ptr<FileInfo> image = make_unique<FileInfo>(IMAGE_ROOT_NAME, FISRT_LEVEL_ALBUM, ALBUM_TYPE);
        fileRes.emplace_back(move(image));
        unique_ptr<FileInfo> video = make_unique<FileInfo>(VIDEO_ROOT_NAME, FISRT_LEVEL_ALBUM, ALBUM_TYPE);
        fileRes.emplace_back(move(video));
        unique_ptr<FileInfo> audio = make_unique<FileInfo>(AUDIO_ROOT_NAME, FISRT_LEVEL_ALBUM, ALBUM_TYPE);
        fileRes.emplace_back(move(audio));
        unique_ptr<FileInfo> file = make_unique<FileInfo>(FILE_ROOT_NAME, FISRT_LEVEL_ALBUM, ALBUM_TYPE);
        fileRes.emplace_back(move(file));
        return SUCCESS;
    }
}

int FileManagerProxy::CreateFile(const std::string &path, const std::string &fileName,
    const CmdOptions &option, std::string &uri)
{
    MessageParcel data;
    data.WriteString(fileName);
    data.WriteString(path);
    MessageParcel reply;
    MessageOption messageOption;
    int code = Operation::CREATE_FILE;
    CmdOptions op(option);
    if (op.GetDevInfo().GetName() == "external_storage") {
        code = (Equipment::EXTERNAL_STORAGE << EQUIPMENT_SHIFT) | Operation::CREATE_FILE;
    }
    int err = Remote()->SendRequest(code, data, reply, messageOption);
    if (err != ERR_NONE) {
        ERR_LOG("inner error send request fail %{public}d", err);
        return FAIL;
    }
    sptr<CmdResponse> cmdResponse;
    err = GetCmdResponse(reply, cmdResponse);
    if (err != ERR_NONE) {
        return err;
    }
    uri = cmdResponse->GetUri();
    return err;
}

IFmsClient *IFmsClient::GetFmsInstance()
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        ERR_LOG("samgr object is NULL.");
        return nullptr;
    }
    sptr<IRemoteObject> object = samgr->GetSystemAbility(FILE_MANAGER_SERVICE_ID);
    if (object == nullptr) {
        ERR_LOG("FileManager Service object is NULL.");
        return nullptr;
    }
    static FileManagerProxy proxy = FileManagerProxy(object);
    return &proxy;
}

int FileManagerProxy::ListFile(const std::string &type, const std::string &path, const CmdOptions &option,
    std::vector<std::unique_ptr<FileInfo>> &fileRes)
{
    MessageParcel data;
    CmdOptions op(option);
    std::string devName(op.GetDevInfo().GetName());
    std::string devPath(op.GetDevInfo().GetPath());
    int64_t offset = op.GetOffset();
    int64_t count = op.GetCount();

    data.WriteString(devName);
    data.WriteString(devPath);
    data.WriteString(type);
    data.WriteString(path);
    data.WriteInt64(offset);
    data.WriteInt64(count);
    MessageParcel reply;
    MessageOption messageOption;
    uint32_t code = Operation::LIST_FILE;
    if (op.GetDevInfo().GetName() == "external_storage") {
        code = (Equipment::EXTERNAL_STORAGE << EQUIPMENT_SHIFT) | Operation::LIST_FILE;
    }
    int err = Remote()->SendRequest(code, data, reply, messageOption);
    if (err != ERR_NONE) {
        ERR_LOG("inner error send request fail %{public}d", err);
        return FAIL;
    }
    sptr<CmdResponse> cmdResponse;
    err = GetCmdResponse(reply, cmdResponse);
    if (err != ERR_NONE) {
        return err;
    }
    fileRes = cmdResponse->GetFileInfoList();
    return err;
}

int FileManagerProxy::Mkdir(const string &name, const string &path)
{
    MessageParcel data;
    data.WriteString(name);
    data.WriteString(path);
    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(Operation::MAKE_DIR, data, reply, option);
    if (err != ERR_NONE) {
        ERR_LOG("inner error send request fail %{public}d", err);
        return FAIL;
    }
    reply.ReadInt32(err);
    return err;
}
} // FileManagerService
} // namespace OHOS