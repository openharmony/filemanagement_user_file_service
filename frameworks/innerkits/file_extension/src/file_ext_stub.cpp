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

#include "file_ext_stub.h"

#include "hilog_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
FileExtStub::FileExtStub()
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    stubFuncMap_[CMD_OPEN_FILE] = &FileExtStub::CmdOpenFile;
    stubFuncMap_[CMD_CLOSE_FILE] = &FileExtStub::CmdCloseFile;
    stubFuncMap_[CMD_CREATE_FILE] = &FileExtStub::CmdCreateFile;
    stubFuncMap_[CMD_MKDIR] = &FileExtStub::CmdMkdir;
    stubFuncMap_[CMD_DELETE] = &FileExtStub::CmdDelete;
    stubFuncMap_[CMD_MOVE] = &FileExtStub::CmdMove;
    stubFuncMap_[CMD_RENAME] = &FileExtStub::CmdRename;
}

FileExtStub::~FileExtStub()
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    stubFuncMap_.clear();
}

int FileExtStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    HILOG_INFO("tag dsa %{public}s Received stub message: %{public}d", __func__, code);
    std::u16string descriptor = FileExtStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_INFO("tag dsa local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    const auto &itFunc = stubFuncMap_.find(code);
    if (itFunc != stubFuncMap_.end()) {
        return (this->*(itFunc->second))(data, reply);
    }

    HILOG_INFO("tag dsa %{public}s remote request unhandled: %{public}d", __func__, code);
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode FileExtStub::CmdOpenFile(MessageParcel &data, MessageParcel &reply)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    std::shared_ptr<Uri> uri(data.ReadParcelable<Uri>());
    if (uri == nullptr) {
        HILOG_ERROR("tag dsa FileExtStub uri is nullptr");
        return ERR_INVALID_VALUE;
    }
    std::string mode = data.ReadString();
    if (mode.empty()) {
        HILOG_ERROR("tag dsa FileExtStub mode is nullptr");
        return ERR_INVALID_VALUE;
    }
    int fd = OpenFile(*uri, mode);
    if (fd < 0) {
        HILOG_ERROR("tag dsa OpenFile fail, fd is %{pubilc}d", fd);
        return ERR_INVALID_VALUE;
    }
    HILOG_INFO("tag dsa %{public}s retutn fd: %{public}d.", __func__, fd);

    if (!reply.WriteFileDescriptor(fd)) {
        HILOG_ERROR("tag dsa fail to WriteFileDescriptor fd");
        return ERR_INVALID_VALUE;
    }
    HILOG_INFO("tag dsa %{public}s end.", __func__);
    return NO_ERROR;
}

ErrCode FileExtStub::CmdCloseFile(MessageParcel &data, MessageParcel &reply)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    int fd = data.ReadFileDescriptor();
    if (fd < 0) {
        HILOG_ERROR("tag dsa FileExtStub fd is invalid");
        return ERR_INVALID_VALUE;
    }
    std::string uri = data.ReadString();
    if (uri.empty()) {
        HILOG_ERROR("tag dsa FileExtStub uri is nullptr");
        return ERR_INVALID_VALUE;
    }

    int ret = CloseFile(fd, uri);
    if (ret < 0) {
        HILOG_ERROR("tag dsa CloseFile fail, ret is %{pubilc}d", ret);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("tag dsa fail to WriteInt32 ret");
        return ERR_INVALID_VALUE;
    }
    HILOG_INFO("tag dsa %{public}s end.", __func__);
    return NO_ERROR;
}

ErrCode FileExtStub::CmdCreateFile(MessageParcel &data, MessageParcel &reply)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    std::shared_ptr<Uri> parentUri(data.ReadParcelable<Uri>());
    if (parentUri == nullptr) {
        HILOG_ERROR("tag dsa FileExtStub parentUri is nullptr");
        return ERR_INVALID_VALUE;
    }
    std::string displayName = data.ReadString();
    if (displayName.empty()) {
        HILOG_ERROR("tag dsa FileExtStub mode is nullptr");
        return ERR_INVALID_VALUE;
    }
    std::shared_ptr<Uri> newFileUri(data.ReadParcelable<Uri>());
    if (newFileUri == nullptr) {
        HILOG_ERROR("tag dsa FileExtStub newFileUri is nullptr");
        return ERR_INVALID_VALUE;
    }

    int ret = CreateFile(*parentUri, displayName, *newFileUri);
    if (ret < 0) {
        HILOG_ERROR("tag dsa CreateFile fail, ret is %{pubilc}d", ret);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("tag dsa fail to WriteInt32 ret");
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteParcelable(&(*newFileUri))) {
        HILOG_ERROR("fail to WriteParcelable type");
        return ERR_INVALID_VALUE;
    }
    HILOG_INFO("tag dsa %{public}s end. ret:%d, newFileUri = %{public}s", __func__, ret, newFileUri->ToString().c_str());
    HILOG_INFO("tag dsa %{public}s end. newFileUri = %{public}s", __func__, newFileUri->ToString().c_str());
    return NO_ERROR;
}

ErrCode FileExtStub::CmdMkdir(MessageParcel &data, MessageParcel &reply)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    std::shared_ptr<Uri> parentUri(data.ReadParcelable<Uri>());
    if (parentUri == nullptr) {
        HILOG_ERROR("tag dsa FileExtStub parentUri is nullptr");
        return ERR_INVALID_VALUE;
    }
    std::string displayName = data.ReadString();
    if (displayName.empty()) {
        HILOG_ERROR("tag dsa FileExtStub mode is nullptr");
        return ERR_INVALID_VALUE;
    }
    std::shared_ptr<Uri> newFileUri(data.ReadParcelable<Uri>());
    if (newFileUri == nullptr) {
        HILOG_ERROR("tag dsa FileExtStub newFileUri is nullptr");
        return ERR_INVALID_VALUE;
    }

    int ret = Mkdir(*parentUri, displayName, *newFileUri);
    if (ret < 0) {
        HILOG_ERROR("tag dsa Mkdir fail, ret is %{pubilc}d", ret);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("tag dsa fail to WriteInt32 ret");
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteParcelable(&(*newFileUri))) {
        HILOG_ERROR("fail to WriteParcelable type");
        return ERR_INVALID_VALUE;
    }

    HILOG_INFO("tag dsa %{public}s end. ret:%d, newFileUri = %{public}s", __func__, ret, newFileUri->ToString().c_str());
    HILOG_INFO("tag dsa %{public}s end. newFileUri = %{public}s", __func__, newFileUri->ToString().c_str());
    return NO_ERROR;
}

ErrCode FileExtStub::CmdDelete(MessageParcel &data, MessageParcel &reply)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    std::shared_ptr<Uri> uri(data.ReadParcelable<Uri>());
    if (uri == nullptr) {
        HILOG_ERROR("tag dsa FileExtStub uri is nullptr");
        return ERR_INVALID_VALUE;
    }

    int ret = Delete(*uri);
    if (ret < 0) {
        HILOG_ERROR("tag dsa Delete fail, ret is %{pubilc}d", ret);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("tag dsa fail to WriteFileDescriptor ret");
        return ERR_INVALID_VALUE;
    }
    HILOG_INFO("tag dsa %{public}s end.", __func__);
    return NO_ERROR;
}

ErrCode FileExtStub::CmdMove(MessageParcel &data, MessageParcel &reply)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    std::shared_ptr<Uri> sourceFileUri(data.ReadParcelable<Uri>());
    if (sourceFileUri == nullptr) {
        HILOG_ERROR("tag dsa FileExtStub sourceFileUri is nullptr");
        return ERR_INVALID_VALUE;
    }
    std::shared_ptr<Uri> targetParentUri(data.ReadParcelable<Uri>());
    if (targetParentUri == nullptr) {
        HILOG_ERROR("tag dsa FileExtStub targetParentUri is nullptr");
        return ERR_INVALID_VALUE;
    }
    std::shared_ptr<Uri> newFileUri(data.ReadParcelable<Uri>());
    if (newFileUri == nullptr) {
        HILOG_ERROR("tag dsa FileExtStub newFileUri is nullptr");
        return ERR_INVALID_VALUE;
    }

    int ret = Move(*sourceFileUri, *targetParentUri, *newFileUri);
    if (ret < 0) {
        HILOG_ERROR("tag dsa Move fail, ret is %{pubilc}d", ret);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("tag dsa fail to WriteInt32 ret");
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteParcelable(&(*newFileUri))) {
        HILOG_ERROR("fail to WriteParcelable type");
        return ERR_INVALID_VALUE;
    }

    HILOG_INFO("tag dsa %{public}s end. ret:%d, newFileUri = %{public}s", __func__, ret, newFileUri->ToString().c_str());
    HILOG_INFO("tag dsa %{public}s end. newFileUri = %{public}s", __func__, newFileUri->ToString().c_str());
    return NO_ERROR;
}

ErrCode FileExtStub::CmdRename(MessageParcel &data, MessageParcel &reply)
{
    HILOG_INFO("tag dsa %{public}s begin.", __func__);
    std::shared_ptr<Uri> sourceFileUri(data.ReadParcelable<Uri>());
    if (sourceFileUri == nullptr) {
        HILOG_ERROR("tag dsa FileExtStub sourceFileUri is nullptr");
        return ERR_INVALID_VALUE;
    }
    std::string displayName = data.ReadString();
    if (displayName.empty()) {
        HILOG_ERROR("tag dsa FileExtStub mode is nullptr");
        return ERR_INVALID_VALUE;
    }
    std::shared_ptr<Uri> newFileUri(data.ReadParcelable<Uri>());
    if (newFileUri == nullptr) {
        HILOG_ERROR("tag dsa FileExtStub newFileUri is nullptr");
        return ERR_INVALID_VALUE;
    }

    int ret = Rename(*sourceFileUri, displayName, *newFileUri);
    if (ret < 0) {
        HILOG_ERROR("tag dsa Rename fail, ret is %{pubilc}d", ret);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("tag dsa fail to WriteInt32 ret");
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteParcelable(&(*newFileUri))) {
        HILOG_ERROR("fail to WriteParcelable type");
        return ERR_INVALID_VALUE;
    }

    HILOG_INFO("tag dsa %{public}s end. ret:%d, newFileUri = %{public}s", __func__, ret, newFileUri->ToString().c_str());
    HILOG_INFO("tag dsa %{public}s end. newFileUri = %{public}s", __func__, newFileUri->ToString().c_str());
    return NO_ERROR;
}
} // namespace AAFwk
} // namespace OHOS
