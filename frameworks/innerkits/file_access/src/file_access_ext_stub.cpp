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

#include "hilog_wrapper.h"

namespace OHOS {
namespace FileAccessFwk {
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
        HILOG_ERROR("%{public}s  uri is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    int flags = data.ReadInt32();
    if (flags < 0) {
        HILOG_ERROR("%{public}s mode is invalid", __func__);
        return ERR_INVALID_VALUE;
    }
    int fd = OpenFile(*uri, flags);
    if (fd < 0) {
        HILOG_ERROR("%{public}s  OpenFile fail, fd is %{pubilc}d", __func__, fd);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteFileDescriptor(fd)) {
        HILOG_ERROR("%{public}s  fail to WriteFileDescriptor fd", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode FileAccessExtStub::CmdCreateFile(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<Uri> parent(data.ReadParcelable<Uri>());
    if (parent == nullptr) {
        HILOG_ERROR("%{public}s parent is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    std::string displayName = data.ReadString();
    if (displayName.empty()) {
        HILOG_ERROR("%{public}s mode is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    std::shared_ptr<Uri> fileNew(data.ReadParcelable<Uri>());
    if (fileNew == nullptr) {
        HILOG_ERROR("%{public}s fileNew is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    int ret = CreateFile(*parent, displayName, *fileNew);
    if (ret < 0) {
        HILOG_ERROR("%{public}s CreateFile fail, ret is %{pubilc}d", __func__, ret);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("%{public}s fail to WriteInt32 ret", __func__);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteParcelable(&(*fileNew))) {
        HILOG_ERROR("%{public}s fail to WriteParcelable type", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode FileAccessExtStub::CmdMkdir(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<Uri> parent(data.ReadParcelable<Uri>());
    if (parent == nullptr) {
        HILOG_ERROR("%{public}s parent is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    std::string displayName = data.ReadString();
    if (displayName.empty()) {
        HILOG_ERROR("%{public}s mode is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    std::shared_ptr<Uri> fileNew(data.ReadParcelable<Uri>());
    if (fileNew == nullptr) {
        HILOG_ERROR("%{public}s fileNew is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    int ret = Mkdir(*parent, displayName, *fileNew);
    if (ret < 0) {
        HILOG_ERROR("%{public}s Mkdir fail, ret is %{pubilc}d", __func__, ret);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("%{public}s fail to WriteInt32 ret", __func__);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteParcelable(&(*fileNew))) {
        HILOG_ERROR("%{public}s fail to WriteParcelable type", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode FileAccessExtStub::CmdDelete(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<Uri> uri(data.ReadParcelable<Uri>());
    if (uri == nullptr) {
        HILOG_ERROR("%{public}s uri is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    int ret = Delete(*uri);
    if (ret < 0) {
        HILOG_ERROR("%{public}s Delete fail, ret is %{pubilc}d", __func__, ret);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("%{public}s fail to WriteFileDescriptor ret", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode FileAccessExtStub::CmdMove(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<Uri> sourceFile(data.ReadParcelable<Uri>());
    if (sourceFile == nullptr) {
        HILOG_ERROR(" %{public}s sourceFile is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    std::shared_ptr<Uri> targetParent(data.ReadParcelable<Uri>());
    if (targetParent == nullptr) {
        HILOG_ERROR(" %{public}s targetParent is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    std::shared_ptr<Uri> fileNew(data.ReadParcelable<Uri>());
    if (fileNew == nullptr) {
        HILOG_ERROR(" %{public}s fileNew is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    int ret = Move(*sourceFile, *targetParent, *fileNew);
    if (ret < 0) {
        HILOG_ERROR(" %{public}s fail, ret is %{pubilc}d", __func__, ret);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR(" %{public}s fail to WriteInt32 ret", __func__);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteParcelable(&(*fileNew))) {
        HILOG_ERROR(" %{public}s fail to WriteParcelable type", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode FileAccessExtStub::CmdRename(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<Uri> sourceFile(data.ReadParcelable<Uri>());
    if (sourceFile == nullptr) {
        HILOG_ERROR("%{public}s sourceFileUri is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    std::string displayName = data.ReadString();
    if (displayName.empty()) {
        HILOG_ERROR("%{public}s mode is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    std::shared_ptr<Uri> fileNew(data.ReadParcelable<Uri>());
    if (fileNew == nullptr) {
        HILOG_ERROR("%{public}s fileUriNew is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    int ret = Rename(*sourceFile, displayName, *fileNew);
    if (ret < 0) {
        HILOG_ERROR("%{public}s Rename fail, ret is %{pubilc}d", __func__, ret);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("%{public}s fail to WriteInt32 ret", __func__);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteParcelable(&(*fileNew))) {
        HILOG_ERROR("%{public}s fail to WriteParcelable type", __func__);
        return ERR_INVALID_VALUE;
    }

    return NO_ERROR;
}

ErrCode FileAccessExtStub::CmdListFile(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<Uri> uri(data.ReadParcelable<Uri>());
    if (uri == nullptr) {
        HILOG_ERROR("%{public}s uri is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    std::vector<FileInfo> vec = ListFile(*uri);
    uint64_t count {vec.size()};
    if (!reply.WriteUint64(count)) {
        HILOG_ERROR("%{public}s fail to WriteInt32 count", __func__);
        return ERR_INVALID_VALUE;
    }
    for (uint64_t i = 0; i < count; i++) {
        if (!reply.WriteParcelable(&vec[i])) {
            HILOG_ERROR("%{public}s fail to WriteParcelable vec", __func__);
            return ERR_INVALID_VALUE;
        }
    }

    return NO_ERROR;
}

ErrCode FileAccessExtStub::CmdGetRoots(MessageParcel &data, MessageParcel &reply)
{
    std::vector<DeviceInfo> vec = GetRoots();
    uint64_t count {vec.size()};
    if (!reply.WriteUint64(count)) {
        HILOG_ERROR("%{public}s fail to WriteInt32 count", __func__);
        return ERR_INVALID_VALUE;
    }
    for (uint64_t i = 0; i < count; i++) {
        if (!reply.WriteParcelable(&vec[i])) {
            HILOG_ERROR("%{public}s fail to WriteParcelable ret", __func__);
            return ERR_INVALID_VALUE;
        }
    }

    return NO_ERROR;
}
} // namespace FileAccessFwk
} // namespace OHOS