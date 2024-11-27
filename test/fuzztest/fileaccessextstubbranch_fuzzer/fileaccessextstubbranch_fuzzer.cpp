/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "fileaccessextstubbranch_fuzzer.h"

#include <cstring>
#include <memory>
#include <queue>

#include "accesstoken_kit.h"
#include "extension_base.h"
#include "extension_context.h"
#include "file_access_ext_stub.h"
#include "file_access_ext_stub_impl.h"
#include "fileaccessextabilitymock.h"
#include "iremote_broker.h"
#include "message_parcel.h"

namespace OHOS {
using namespace std;
using namespace FileAccessFwk;
using namespace AbilityRuntime;

using FAFVariant = std::variant<int32_t, int64_t, uint32_t, bool, std::vector<std::string>>;

queue<bool> que;
FAFVariant arg;

bool GetResult()
{
    bool ret = true;
    if (!que.empty()) {
        ret = que.front();
        que.pop();
    }
    return ret;
}

Parcelable::Parcelable() : Parcelable(false)
{}

Parcelable::Parcelable(bool asRemote)
{
    asRemote_ = asRemote;
    behavior_ = 0;
}

template <typename T>
bool Parcel::Write(T value)
{
    size_t desireCapacity = sizeof(T);

    if (EnsureWritableCapacity(desireCapacity)) {
        *reinterpret_cast<T *>(data_ + writeCursor_) = value;
        writeCursor_ += desireCapacity;
        dataSize_ += desireCapacity;
        return true;
    }

    return false;
}

bool Parcel::WriteParcelable(const Parcelable *object)
{
    if (object == nullptr) {
        return WriteInt32(0);
    }

    if (!object->asRemote_) {
        bool flag = WriteInt32(1);
        object->Marshalling(*this);
        return flag;
    }

    bool flag = WriteInt32(1);
    WriteRemoteObject(const_cast<Parcelable*>(object));
    return flag;
}

bool Parcel::WriteInt32(int32_t value)
{
    Write<int32_t>(value);
    return GetResult();
}

bool Parcel::WriteUint32(uint32_t value)
{
    Write<uint32_t>(value);
    return GetResult();
}

int32_t Parcel::ReadInt32()
{
    return GetResult();
}

bool Parcel::ReadInt32(int32_t &value)
{
    value = std::get<int32_t>(arg);
    return GetResult();
}

bool Parcel::ReadBool()
{
    return GetResult();
}

bool Parcel::ReadBool(bool &value)
{
    if (std::get_if<bool>(&arg)) {
        value = std::get<bool>(arg);
    }
    return GetResult();
}

bool Parcel::WriteBool(bool value)
{
    return GetResult();
}

bool Parcel::WriteString(const std::string &value)
{
    return GetResult();
}

bool Parcel::ReadString(std::string &value)
{
    std::vector<std::string> vstr = std::get<std::vector<std::string>>(arg);
    if (!vstr.empty()) {
        value = vstr[0];
        vstr.erase(vstr.begin());
    }
    return GetResult();
}

bool Parcel::ReadStringVector(std::vector<std::string> *value)
{
    *value = std::get<std::vector<std::string>>(arg);
    return GetResult();
}

bool MessageParcel::WriteFileDescriptor(int fd)
{
    return GetResult();
}

bool Parcel::ReadUint32(uint32_t &value)
{
    value = std::get<uint32_t>(arg);
    return GetResult();
}

template<class T>
T TypeCast(const uint8_t *data, int *pos = nullptr)
{
    if (pos) {
        *pos += sizeof(T);
    }
    return *(reinterpret_cast<const T*>(data));
}

bool OnRemoteRequestFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(uint32_t)) {
        return true;
    }

    uint32_t code = TypeCast<uint32_t>(data);
    MessageParcel msg;
    MessageParcel reply;
    MessageOption option;

    fileAccessExtStub->OnRemoteRequest(code, msg, reply, option);

    msg.WriteInterfaceToken(FileAccessExtStub::GetDescriptor());
    fileAccessExtStub->OnRemoteRequest(code, msg, reply, option);
    return true;
}

bool CmdOpenFileFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;

    Uri uri(string(reinterpret_cast<const char*>(data), size));
    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(false);
    fileAccessExtStub->CmdOpenFile(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    arg = -1;
    fileAccessExtStub->CmdOpenFile(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    que.push(false);
    arg = TypeCast<int32_t>(data);
    fileAccessExtStub->CmdOpenFile(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    que.push(true);
    que.push(false);
    arg = TypeCast<int32_t>(data);
    fileAccessExtStub->CmdOpenFile(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    que.push(true);
    que.push(true);
    arg = TypeCast<int32_t>(data);
    fileAccessExtStub->CmdOpenFile(msg, reply);
    return true;
}

bool CmdCreateFileFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    int pos = size >> 1;

    vector<string> displayName{""};
    arg = displayName;
    fileAccessExtStub->CmdCreateFile(msg, reply);

    Uri uri(string(reinterpret_cast<const char*>(data), pos));
    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(false);
    arg = displayName;
    fileAccessExtStub->CmdCreateFile(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    arg = displayName;
    fileAccessExtStub->CmdCreateFile(msg, reply);

    displayName.clear();
    displayName.emplace_back(reinterpret_cast<const char*>(data + pos), size - pos);
    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    que.push(false);
    arg = displayName;
    fileAccessExtStub->CmdCreateFile(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    que.push(true);
    que.push(false);
    arg = displayName;
    fileAccessExtStub->CmdCreateFile(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    que.push(true);
    que.push(true);
    arg = displayName;
    fileAccessExtStub->CmdCreateFile(msg, reply);
    return true;
}

bool CmdMkdirFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    int pos = size >> 1;

    Uri uri(string(reinterpret_cast<const char*>(data), pos));
    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(false);
    fileAccessExtStub->CmdMkdir(msg, reply);

    vector<string> displayName{""};
    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    arg = displayName;
    fileAccessExtStub->CmdMkdir(msg, reply);

    displayName.clear();
    displayName.emplace_back(reinterpret_cast<const char*>(data + pos), size - pos);
    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    que.push(false);
    arg = displayName;
    fileAccessExtStub->CmdMkdir(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    que.push(true);
    que.push(false);
    arg = displayName;
    fileAccessExtStub->CmdMkdir(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    que.push(true);
    que.push(true);
    arg = displayName;
    fileAccessExtStub->CmdMkdir(msg, reply);
    return true;
}

bool CmdDeleteFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;

    Uri uri(string(reinterpret_cast<const char*>(data), size));
    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(false);
    fileAccessExtStub->CmdDelete(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    fileAccessExtStub->CmdDelete(msg, reply);
    return true;
}

bool CmdMoveFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    int pos = size >> 1;
    Uri sourceFile(string(reinterpret_cast<const char*>(data), pos));
    Uri targetParent(string(reinterpret_cast<const char*>(data + pos), size - pos));

    msg.FlushBuffer();
    msg.WriteParcelable(&sourceFile);
    fileAccessExtStub->CmdMove(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&sourceFile);
    msg.WriteParcelable(&targetParent);
    que = {};
    que.push(false);
    fileAccessExtStub->CmdMove(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&sourceFile);
    msg.WriteParcelable(&targetParent);
    que = {};
    que.push(true);
    que.push(false);
    fileAccessExtStub->CmdMove(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&sourceFile);
    msg.WriteParcelable(&targetParent);
    que = {};
    que.push(true);
    que.push(true);
    fileAccessExtStub->CmdMove(msg, reply);
    return true;
}

bool CmdCopyFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    int pos = (size - 1) >> 1;
    Uri sourceUri(string(reinterpret_cast<const char*>(data + 1), pos));
    Uri destUri(string(reinterpret_cast<const char*>(data + pos + 1), size - pos - 1));

    msg.FlushBuffer();
    msg.WriteParcelable(&sourceUri);
    fileAccessExtStub->CmdCopy(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&sourceUri);
    msg.WriteParcelable(&destUri);
    que = {};
    que.push(false);
    fileAccessExtStub->CmdCopy(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&sourceUri);
    msg.WriteParcelable(&destUri);
    que = {};
    que.push(true);
    que.push(false);
    arg = TypeCast<bool>(data);
    fileAccessExtStub->CmdCopy(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&sourceUri);
    msg.WriteParcelable(&destUri);
    que = {};
    que.push(true);
    que.push(true);
    que.push(false);
    arg = TypeCast<bool>(data);
    fileAccessExtStub->CmdCopy(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&sourceUri);
    msg.WriteParcelable(&destUri);
    que = {};
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(false);
    arg = TypeCast<bool>(data);
    fileAccessExtStub->CmdCopy(msg, reply);
    return true;
}

bool CmdCopyFileFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    vector<string> str{ string(reinterpret_cast<const char*>(data), size) };

    que = {};
    que.push(false);
    arg = str;
    fileAccessExtStub->CmdCopyFile(msg, reply);

    que = {};
    que.push(true);
    que.push(false);
    fileAccessExtStub->CmdCopyFile(msg, reply);

    que = {};
    que.push(true);
    que.push(true);
    que.push(false);
    fileAccessExtStub->CmdCopyFile(msg, reply);

    que = {};
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(false);
    fileAccessExtStub->CmdCopyFile(msg, reply);

    que = {};
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(false);
    fileAccessExtStub->CmdCopyFile(msg, reply);

    que = {};
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(true);
    fileAccessExtStub->CmdCopyFile(msg, reply);
    return true;
}

bool CmdRenameFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    int pos = size >> 1;
    Uri sourceFile(string(reinterpret_cast<const char*>(data), pos));
    vector<string> displayName{ string(reinterpret_cast<const char*>(data + pos), size - pos) };

    msg.FlushBuffer();
    msg.WriteParcelable(&sourceFile);
    que = {};
    que.push(false);
    fileAccessExtStub->CmdRename(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&sourceFile);
    que = {};
    que.push(true);
    arg = vector<string>();
    fileAccessExtStub->CmdRename(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&sourceFile);
    que = {};
    que.push(true);
    que.push(false);
    arg = displayName;
    fileAccessExtStub->CmdRename(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&sourceFile);
    que = {};
    que.push(true);
    que.push(true);
    que.push(false);
    arg = displayName;
    fileAccessExtStub->CmdRename(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&sourceFile);
    que = {};
    que.push(true);
    que.push(true);
    que.push(true);
    arg = displayName;
    fileAccessExtStub->CmdRename(msg, reply);
    return true;
}

bool CmdGetRootsFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    msg.WriteBuffer(data, size);

    que = {};
    que.push(false);
    fileAccessExtStub->CmdGetRoots(msg, reply);

    que = {};
    que.push(true);
    que.push(false);
    fileAccessExtStub->CmdGetRoots(msg, reply);

    que = {};
    que.push(true);
    que.push(true);
    que.push(false);
    fileAccessExtStub->CmdGetRoots(msg, reply);

    que = {};
    que.push(true);
    que.push(true);
    que.push(true);
    fileAccessExtStub->CmdGetRoots(msg, reply);
    return true;
}

bool CmdQueryFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    if (data == nullptr || size < sizeof(int64_t)) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    Uri uri(string(reinterpret_cast<const char*>(data + sizeof(int64_t)), size - sizeof(int64_t)));

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(false);
    fileAccessExtStub->CmdQuery(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    que.push(false);
    arg = TypeCast<int64_t>(data);
    fileAccessExtStub->CmdQuery(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    que.push(true);
    que.push(false);
    arg = TypeCast<int64_t>(data);
    fileAccessExtStub->CmdQuery(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(false);
    arg = TypeCast<int64_t>(data);
    fileAccessExtStub->CmdQuery(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(true);
    arg = TypeCast<int64_t>(data);
    fileAccessExtStub->CmdQuery(msg, reply);
    return true;
}

bool CmdGetFileInfoFromUriFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    Uri uri(string(reinterpret_cast<const char*>(data), size));

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(false);
    fileAccessExtStub->CmdGetFileInfoFromUri(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    que.push(false);
    fileAccessExtStub->CmdGetFileInfoFromUri(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    que.push(true);
    fileAccessExtStub->CmdGetFileInfoFromUri(msg, reply);
    return true;
}

bool CmdGetFileInfoFromRelativePathFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data,
    size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    Uri uri(string(reinterpret_cast<const char*>(data), size));

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(false);
    fileAccessExtStub->CmdGetFileInfoFromRelativePath(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    que.push(false);
    fileAccessExtStub->CmdGetFileInfoFromRelativePath(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    que.push(true);
    fileAccessExtStub->CmdGetFileInfoFromRelativePath(msg, reply);
    return true;
}

bool CmdAccessFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    Uri uri(string(reinterpret_cast<const char*>(data), size));

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(false);
    fileAccessExtStub->CmdAccess(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    que.push(false);
    fileAccessExtStub->CmdAccess(msg, reply);

    msg.FlushBuffer();
    msg.WriteParcelable(&uri);
    que = {};
    que.push(true);
    que.push(true);
    fileAccessExtStub->CmdAccess(msg, reply);
    return true;
}

bool CmdStartWatcherFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    vector<string> uriString{ string(reinterpret_cast<const char*>(data), size) };

    que = {};
    que.push(false);
    arg = vector<string>();
    fileAccessExtStub->CmdStartWatcher(msg, reply);

    que = {};
    que.push(true);
    arg = vector<string>();
    fileAccessExtStub->CmdStartWatcher(msg, reply);

    que = {};
    que.push(true);
    que.push(false);
    arg = uriString;
    fileAccessExtStub->CmdStartWatcher(msg, reply);

    que = {};
    que.push(true);
    que.push(true);
    arg = uriString;
    fileAccessExtStub->CmdStartWatcher(msg, reply);
    return true;
}

bool CmdStopWatcherFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    vector<string> uriString{ string(reinterpret_cast<const char*>(data), size) };

    que = {};
    que.push(false);
    arg = vector<string>();
    fileAccessExtStub->CmdStopWatcher(msg, reply);

    que = {};
    que.push(true);
    arg = vector<string>();
    fileAccessExtStub->CmdStopWatcher(msg, reply);

    que = {};
    que.push(true);
    que.push(false);
    arg = uriString;
    fileAccessExtStub->CmdStopWatcher(msg, reply);

    que = {};
    que.push(true);
    que.push(true);
    arg = uriString;
    fileAccessExtStub->CmdStopWatcher(msg, reply);
    return true;
}

bool CmdMoveItemFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    int pos = size >> 1;
    string sourceFile(reinterpret_cast<const char*>(data), pos);
    string targetParent(reinterpret_cast<const char*>(data + pos), size - pos);
    arg = vector<string>{sourceFile, targetParent};

    que = {};
    que.push(false);
    fileAccessExtStub->CmdMoveItem(msg, reply);

    que = {};
    que.push(true);
    que.push(false);
    fileAccessExtStub->CmdMoveItem(msg, reply);

    que = {};
    que.push(true);
    que.push(true);
    que.push(false);
    fileAccessExtStub->CmdMoveItem(msg, reply);

    que = {};
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(false);
    fileAccessExtStub->CmdMoveItem(msg, reply);

    que = {};
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(false);
    fileAccessExtStub->CmdMoveItem(msg, reply);

    que = {};
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(false);
    fileAccessExtStub->CmdMoveItem(msg, reply);

    que = {};
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(true);
    fileAccessExtStub->CmdMoveItem(msg, reply);
    return true;
}

bool CmdMoveFileFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    int pos = size / 3;
    vector<string> item {   string(reinterpret_cast<const char*>(data), pos),
                            string(reinterpret_cast<const char*>(data + pos), pos),
                            string(reinterpret_cast<const char*>(data + pos + pos), size - pos - pos)};
    arg = item;
    que = {};
    que.push(false);
    fileAccessExtStub->CmdMoveFile(msg, reply);

    que = {};
    que.push(true);
    que.push(false);
    fileAccessExtStub->CmdMoveFile(msg, reply);

    que = {};
    que.push(true);
    que.push(true);
    que.push(false);
    fileAccessExtStub->CmdMoveFile(msg, reply);

    que = {};
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(false);
    fileAccessExtStub->CmdMoveFile(msg, reply);

    que = {};
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(false);
    fileAccessExtStub->CmdMoveFile(msg, reply);

    que = {};
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(true);
    que.push(true);
    fileAccessExtStub->CmdMoveFile(msg, reply);
    return true;
}

} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    auto ability = std::make_shared<OHOS::FileAccessFwk::FileAccessExtAbilityMock>();
    auto stub = std::make_shared<OHOS::FileAccessFwk::FileAccessExtStubImpl>(ability, nullptr);
    if (stub == nullptr) {
        return 0;
    }

    OHOS::OnRemoteRequestFuzzTest(stub, data, size);
    OHOS::CmdOpenFileFuzzTest(stub, data, size);
    OHOS::CmdCreateFileFuzzTest(stub, data, size);
    OHOS::CmdMkdirFuzzTest(stub, data, size);
    OHOS::CmdDeleteFuzzTest(stub, data, size);
    OHOS::CmdMoveFuzzTest(stub, data, size);
    OHOS::CmdCopyFuzzTest(stub, data, size);
    OHOS::CmdCopyFileFuzzTest(stub, data, size);
    OHOS::CmdRenameFuzzTest(stub, data, size);
    OHOS::CmdGetRootsFuzzTest(stub, data, size);
    OHOS::CmdQueryFuzzTest(stub, data, size);
    OHOS::CmdGetFileInfoFromUriFuzzTest(stub, data, size);
    OHOS::CmdGetFileInfoFromRelativePathFuzzTest(stub, data, size);
    OHOS::CmdAccessFuzzTest(stub, data, size);
    OHOS::CmdStartWatcherFuzzTest(stub, data, size);
    OHOS::CmdStopWatcherFuzzTest(stub, data, size);
    OHOS::CmdMoveItemFuzzTest(stub, data, size);
    OHOS::CmdMoveFileFuzzTest(stub, data, size);
    return 0;
}
