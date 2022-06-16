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
namespace FileAccessFwk {
FileExtStub::FileExtStub()
{
    HILOG_INFO("%{public}s begin.", __func__);
    stubFuncMap_[CMD_OPEN_FILE] = &FileExtStub::CmdOpenFile;
    stubFuncMap_[CMD_CREATE_FILE] = &FileExtStub::CmdCreateFile;
    stubFuncMap_[CMD_MKDIR] = &FileExtStub::CmdMkdir;
    stubFuncMap_[CMD_DELETE] = &FileExtStub::CmdDelete;
    stubFuncMap_[CMD_RENAME] = &FileExtStub::CmdRename;
    HILOG_INFO("%{public}s end.", __func__);
}

FileExtStub::~FileExtStub()
{
    HILOG_INFO("%{public}s begin.", __func__);
    HILOG_INFO("%{public}s end.", __func__);
    stubFuncMap_.clear();
}

int FileExtStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    HILOG_INFO("%{public}s Received stub message: %{public}d", __func__, code);
    std::u16string descriptor = FileExtStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_INFO("%{public}s local descriptor is not equal to remote", __func__);
        return ERR_INVALID_STATE;
    }

    const auto &itFunc = stubFuncMap_.find(code);
    if (itFunc != stubFuncMap_.end()) {
        return (this->*(itFunc->second))(data, reply);
    }

    HILOG_INFO("%{public}s remote request unhandled: %{public}d", __func__, code);
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode FileExtStub::CmdOpenFile(MessageParcel &data, MessageParcel &reply)
{
    HILOG_INFO("%{public}s begin.", __func__);
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
    HILOG_INFO("%{public}s retutn fd: %{public}d.", __func__, fd);

    if (!reply.WriteFileDescriptor(fd)) {
        HILOG_ERROR("%{public}s  fail to WriteFileDescriptor fd", __func__);
        return ERR_INVALID_VALUE;
    }
    HILOG_INFO("%{public}s end.", __func__);
    return NO_ERROR;
}

ErrCode FileExtStub::CmdCreateFile(MessageParcel &data, MessageParcel &reply)
{
    HILOG_INFO("%{public}s begin.", __func__);
    std::shared_ptr<Uri> parentUri(data.ReadParcelable<Uri>());
    if (parentUri == nullptr) {
        HILOG_ERROR("%{public}s parentUri is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    std::string displayName = data.ReadString();
    if (displayName.empty()) {
        HILOG_ERROR("%{public}s mode is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    std::shared_ptr<Uri> newFileUri(data.ReadParcelable<Uri>());
    if (newFileUri == nullptr) {
        HILOG_ERROR("%{public}s newFileUri is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    int ret = CreateFile(*parentUri, displayName, *newFileUri);
    if (ret < 0) {
        HILOG_ERROR("%{public}s CreateFile fail, ret is %{pubilc}d", __func__, ret);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("%{public}s fail to WriteInt32 ret", __func__);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteParcelable(&(*newFileUri))) {
        HILOG_ERROR("%{public}s fail to WriteParcelable type", __func__);
        return ERR_INVALID_VALUE;
    }
    HILOG_INFO("%{public}s end. ret:%d, newFileUri = %{public}s", __func__, ret, newFileUri->ToString().c_str());
    HILOG_INFO("%{public}s end. newFileUri = %{public}s", __func__, newFileUri->ToString().c_str());
    return NO_ERROR;
}

ErrCode FileExtStub::CmdMkdir(MessageParcel &data, MessageParcel &reply)
{
    HILOG_INFO("%{public}s begin.", __func__);
    std::shared_ptr<Uri> parentUri(data.ReadParcelable<Uri>());
    if (parentUri == nullptr) {
        HILOG_ERROR("%{public}s parentUri is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    std::string displayName = data.ReadString();
    if (displayName.empty()) {
        HILOG_ERROR("%{public}s mode is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    std::shared_ptr<Uri> newFileUri(data.ReadParcelable<Uri>());
    if (newFileUri == nullptr) {
        HILOG_ERROR("%{public}s newFileUri is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    int ret = Mkdir(*parentUri, displayName, *newFileUri);
    if (ret < 0) {
        HILOG_ERROR("%{public}s Mkdir fail, ret is %{pubilc}d", __func__, ret);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("%{public}s fail to WriteInt32 ret", __func__);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteParcelable(&(*newFileUri))) {
        HILOG_ERROR("%{public}s fail to WriteParcelable type", __func__);
        return ERR_INVALID_VALUE;
    }

    HILOG_INFO("%{public}s end. ret:%d, newFileUri = %{public}s", __func__, ret, newFileUri->ToString().c_str());
    HILOG_INFO("%{public}s end. newFileUri = %{public}s", __func__, newFileUri->ToString().c_str());
    return NO_ERROR;
}

ErrCode FileExtStub::CmdDelete(MessageParcel &data, MessageParcel &reply)
{
    HILOG_INFO("%{public}s begin.", __func__);
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
    HILOG_INFO("%{public}s end.", __func__);
    return NO_ERROR;
}

ErrCode FileExtStub::CmdRename(MessageParcel &data, MessageParcel &reply)
{
    HILOG_INFO("%{public}s begin.", __func__);
    std::shared_ptr<Uri> sourceFileUri(data.ReadParcelable<Uri>());
    if (sourceFileUri == nullptr) {
        HILOG_ERROR("%{public}s sourceFileUri is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    std::string displayName = data.ReadString();
    if (displayName.empty()) {
        HILOG_ERROR("%{public}s mode is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }
    std::shared_ptr<Uri> newFileUri(data.ReadParcelable<Uri>());
    if (newFileUri == nullptr) {
        HILOG_ERROR("%{public}s newFileUri is nullptr", __func__);
        return ERR_INVALID_VALUE;
    }

    int ret = Rename(*sourceFileUri, displayName, *newFileUri);
    if (ret < 0) {
        HILOG_ERROR("%{public}s Rename fail, ret is %{pubilc}d", __func__, ret);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteInt32(ret)) {
        HILOG_ERROR("%{public}s fail to WriteInt32 ret", __func__);
        return ERR_INVALID_VALUE;
    }

    if (!reply.WriteParcelable(&(*newFileUri))) {
        HILOG_ERROR("%{public}s fail to WriteParcelable type", __func__);
        return ERR_INVALID_VALUE;
    }

    HILOG_INFO("%{public}s end. ret:%d, newFileUri = %{public}s", __func__, ret, newFileUri->ToString().c_str());
    HILOG_INFO("%{public}s end. newFileUri = %{public}s", __func__, newFileUri->ToString().c_str());
    return NO_ERROR;
}
} // namespace FileAccessFwk
} // namespace OHOS
