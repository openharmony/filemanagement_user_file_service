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

#include "file_info.h"
#include "file_manager_service_def.h"
#include "file_manager_service_errno.h"
#include "file_manager_service_stub.h"
#include "log.h"
#include "media_file_utils.h"

using namespace std;

namespace OHOS {
namespace FileManagerService {
FileManagerProxy::FileManagerProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IFileManagerService>(impl) {}

int FileManagerProxy::GetRoot(const std::string &devName, vector<FileInfo> &fileRes) const
{
    // currently getRoot don't need to sendReq
    fileRes.emplace_back(IMAGE_ROOT_NAME, FISRT_LEVEL_ALBUM, ALBUM_TYPE);
    fileRes.emplace_back(VIDEO_ROOT_NAME, FISRT_LEVEL_ALBUM, ALBUM_TYPE);
    fileRes.emplace_back(AUDIO_ROOT_NAME, FISRT_LEVEL_ALBUM, ALBUM_TYPE);
    fileRes.emplace_back(FILE_ROOT_NAME, FISRT_LEVEL_ALBUM, ALBUM_TYPE);
    return SUCCESS;
}

int FileManagerProxy::CreateFile(const string &name, const string &path, string &uri)
{
    MessageParcel data;
    data.WriteString(name);
    data.WriteString(path);
    MessageParcel reply;
    MessageOption option;
    int err = Remote()->SendRequest(Operation::CREATE_FILE, data, reply, option);
    if (err != ERR_NONE) {
        ERR_LOG("inner error send request fail %{public}d", err);
        return FAIL;
    }
    reply.ReadString(uri);
    reply.ReadInt32(err);
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
    std::vector<FileInfo> &fileRes)
{
    MessageParcel data;
    CmdOptions op(option);
    std::string devName(op.GetDevInfo().GetName());
    std::string devPath(op.GetDevInfo().GetPath());
    int32_t offset = op.GetOffset();
    int32_t count = op.GetCount();

    data.WriteString(devName);
    data.WriteString(devPath);
    data.WriteString(type);
    data.WriteString(path);
    data.WriteInt32(offset);
    data.WriteInt32(count);
    MessageParcel reply;
    MessageOption messageOption;
    uint32_t code = Equipment::INTERNAL_STORAGE;
    if (op.GetDevInfo().GetName() == "external_storage") {
        code = (Equipment::EXTERNAL_STORAGE << EQUIPMENT_SHIFT) | Operation::LIST_FILE;
    }
    int err = Remote()->SendRequest(code, data, reply, messageOption);
    if (err != ERR_NONE) {
        ERR_LOG("inner error send request fail %{public}d", err);
        return FAIL;
    }
    int fileInfoNum = 0;
    reply.ReadInt32(fileInfoNum);
    while (fileInfoNum) {
        FileInfo file;
        MediaFileUtils::PopFileInfo(file, reply);
        fileRes.emplace_back(file);
        fileInfoNum--;
    }
    reply.ReadInt32(err);
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