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

#include "file_access_ext_stub.h"

#include "accesstoken_kit.h"
#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace FileAccessFwk {
namespace {
    const std::string FILE_ACCESS_PERMISSION = "ohos.permission.FILE_ACCESS_MANAGER";
}
FileAccessExtStub::FileAccessExtStub()
{
    stubFuncMap_[CMD_OPEN_FILE] = &FileAccessExtStub::CmdOpenFile;
    stubFuncMap_[CMD_CREATE_FILE] = &FileAccessExtStub::CmdCreateFile;
    stubFuncMap_[CMD_MKDIR] = &FileAccessExtStub::CmdMkdir;
    stubFuncMap_[CMD_DELETE] = &FileAccessExtStub::CmdDelete;
    stubFuncMap_[CMD_MOVE] = &FileAccessExtStub::CmdMove;
    stubFuncMap_[CMD_RENAME] = &FileAccessExtStub::CmdRename;
    stubFuncMap_[CMD_LIST_FILE] = &FileAccessExtStub::CmdListFile;
    stubFuncMap_[CMD_GET_ROOTS] = &FileAccessExtStub::CmdGetRoots;
}

FileAccessExtStub::~FileAccessExtStub()
{
    stubFuncMap_.clear();
}

int FileAccessExtStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    if (!CheckCallingPermission(FILE_ACCESS_PERMISSION)) {
        HILOG_ERROR("permission error");
        return ERR_PERMISSION_DENIED;
    }

    std::u16string descriptor = FileAccessExtStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        return ERR_INVALID_STATE;
    }

    const auto &itFunc = stubFuncMap_.find(code);
    if (itFunc != stubFuncMap_.end()) {
        return (this->*(itFunc->second))(data, reply);
    }

    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode FileAccessExtStub::CmdOpenFile(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<Uri> uri(data.ReadParcelable<Uri>());
    if (uri == nullptr) {
        HILOG_ERROR("uri is nullptr");
        return ERR_INVALID_URI;
    }

    int flags = data.ReadInt32();
    if (flags < 0) {
        HILOG_ERROR("flags is invalid");
        return ERR_ERROR;
    }

    int fd = OpenFile(*uri, flags);
    if (fd < 0) {
        HILOG_ERROR("OpenFile fail, fd is %{pubilc}d", fd);
        return ERR_INVALID_FD;
    }

    if (!reply.WriteFileDescriptor(fd)) {
        HILOG_ERROR("fail to WriteFileDescriptor fd");
        return ERR_IPC_ERROR;
    }
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdCreateFile(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<Uri> parent(data.ReadParcelable<Uri>());
    if (parent == nullptr) {
        HILOG_ERROR("parent is nullptr");
        return ERR_INVALID_URI;
    }

    std::string displayName = data.ReadString();
    if (displayName.empty()) {
        HILOG_ERROR("displayName is nullptr");
        return ERR_INVALID_PARAM;
    }

    std::shared_ptr<Uri> fileNew(data.ReadParcelable<Uri>());
    if (fileNew == nullptr) {
        HILOG_ERROR("fileNew is nullptr");
        return ERR_INVALID_URI;
    }

    int ret = CreateFile(*parent, displayName, *fileNew);
    if (ret < 0) {
        HILOG_ERROR("CreateFile fail, ret is %{pubilc}d", ret);
        return ERR_CREATE;
    }

    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("fail to WriteInt32 ret");
        return ERR_IPC_ERROR;
    }

    if (!reply.WriteParcelable(&(*fileNew))) {
        HILOG_ERROR("fail to WriteParcelable type");
        return ERR_IPC_ERROR;
    }
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdMkdir(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<Uri> parent(data.ReadParcelable<Uri>());
    if (parent == nullptr) {
        HILOG_ERROR("parent is nullptr");
        return ERR_INVALID_URI;
    }

    std::string displayName = data.ReadString();
    if (displayName.empty()) {
        HILOG_ERROR("mode is nullptr");
        return ERR_INVALID_PARAM;
    }

    std::shared_ptr<Uri> fileNew(data.ReadParcelable<Uri>());
    if (fileNew == nullptr) {
        HILOG_ERROR("fileNew is nullptr");
        return ERR_INVALID_URI;
    }

    int ret = Mkdir(*parent, displayName, *fileNew);
    if (ret < 0) {
        HILOG_ERROR("Mkdir fail, ret is %{pubilc}d", ret);
        return ERR_CREATE;
    }

    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("fail to WriteInt32 ret");
        return ERR_IPC_ERROR;
    }

    if (!reply.WriteParcelable(&(*fileNew))) {
        HILOG_ERROR("fail to WriteParcelable type");
        return ERR_IPC_ERROR;
    }
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdDelete(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<Uri> uri(data.ReadParcelable<Uri>());
    if (uri == nullptr) {
        HILOG_ERROR("uri is nullptr");
        return ERR_INVALID_URI;
    }

    int ret = Delete(*uri);
    if (ret < 0) {
        HILOG_ERROR("Delete fail, ret is %{pubilc}d", ret);
        return ERR_DELETE;
    }

    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("fail to WriteFileDescriptor ret");
        return ERR_IPC_ERROR;
    }
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdMove(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<Uri> sourceFile(data.ReadParcelable<Uri>());
    if (sourceFile == nullptr) {
        HILOG_ERROR("sourceFile is nullptr");
        return ERR_INVALID_URI;
    }

    std::shared_ptr<Uri> targetParent(data.ReadParcelable<Uri>());
    if (targetParent == nullptr) {
        HILOG_ERROR("targetParent is nullptr");
        return ERR_INVALID_URI;
    }

    std::shared_ptr<Uri> fileNew(data.ReadParcelable<Uri>());
    if (fileNew == nullptr) {
        HILOG_ERROR(" fileNew is nullptr");
        return ERR_INVALID_URI;
    }

    int ret = Move(*sourceFile, *targetParent, *fileNew);
    if (ret < 0) {
        HILOG_ERROR("Move fail, ret is %{pubilc}d", ret);
        return ERR_MOVE;
    }

    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("fail to WriteInt32 ret");
        return ERR_IPC_ERROR;
    }

    if (!reply.WriteParcelable(&(*fileNew))) {
        HILOG_ERROR("fail to WriteParcelable type");
        return ERR_IPC_ERROR;
    }
    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdRename(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<Uri> sourceFile(data.ReadParcelable<Uri>());
    if (sourceFile == nullptr) {
        HILOG_ERROR("sourceFileUri is nullptr");
        return ERR_INVALID_URI;
    }

    std::string displayName = data.ReadString();
    if (displayName.empty()) {
        HILOG_ERROR("mode is nullptr");
        return ERR_INVALID_PARAM;
    }

    std::shared_ptr<Uri> fileNew(data.ReadParcelable<Uri>());
    if (fileNew == nullptr) {
        HILOG_ERROR("fileUriNew is nullptr");
        return ERR_INVALID_URI;
    }

    int ret = Rename(*sourceFile, displayName, *fileNew);
    if (ret < 0) {
        HILOG_ERROR("Rename fail, ret is %{pubilc}d", ret);
        return ERR_RENAME;
    }

    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("fail to WriteInt32 ret");
        return ERR_IPC_ERROR;
    }

    if (!reply.WriteParcelable(&(*fileNew))) {
        HILOG_ERROR("fail to WriteParcelable type");
        return ERR_IPC_ERROR;
    }

    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdListFile(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<Uri> uri(data.ReadParcelable<Uri>());
    if (uri == nullptr) {
        HILOG_ERROR("uri is nullptr");
        return ERR_INVALID_URI;
    }

    std::vector<FileInfo> vec = ListFile(*uri);
    uint64_t count {vec.size()};
    if (!reply.WriteUint64(count)) {
        HILOG_ERROR("fail to WriteInt32 count");
        return ERR_IPC_ERROR;
    }

    for (uint64_t i = 0; i < count; i++) {
        if (!reply.WriteParcelable(&vec[i])) {
            HILOG_ERROR("fail to WriteParcelable vec");
            return ERR_IPC_ERROR;
        }
    }

    return ERR_OK;
}

ErrCode FileAccessExtStub::CmdGetRoots(MessageParcel &data, MessageParcel &reply)
{
    std::vector<DeviceInfo> vec = GetRoots();
    uint64_t count {vec.size()};
    if (!reply.WriteUint64(count)) {
        HILOG_ERROR("fail to WriteInt32 count");
        return ERR_IPC_ERROR;
    }

    for (uint64_t i = 0; i < count; i++) {
        if (!reply.WriteParcelable(&vec[i])) {
            HILOG_ERROR("fail to WriteParcelable ret");
            return ERR_IPC_ERROR;
        }
    }

    return ERR_OK;
}

bool FileAccessExtStub::CheckCallingPermission(const std::string &permission)
{
    Security::AccessToken::AccessTokenID tokenCaller = IPCSkeleton::GetCallingTokenID();
    int res = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller, permission);
    if (res != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        HILOG_ERROR("FileAccessExtStub::CheckCallingPermission have no fileAccess permission");
        return false;
    }
    return true;
}
} // namespace FileAccessFwk
} // namespace OHOS