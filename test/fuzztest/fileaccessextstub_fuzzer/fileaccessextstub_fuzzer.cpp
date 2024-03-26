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
#include "fileaccessextstub_fuzzer.h"

#include <cstring>
#include <memory>

#include "accesstoken_kit.h"
#include "extension_base.h"
#include "extension_context.h"
#include "file_access_ext_stub.h"
#include "file_access_ext_stub_impl.h"
#include "fileaccessextabilitymock.h"
#include "message_parcel.h"

namespace OHOS {
using namespace std;
using namespace FileAccessFwk;
using namespace AbilityRuntime;

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
    msg.WriteInterfaceToken(FileAccessExtStub::GetDescriptor());
    MessageParcel reply;
    MessageOption option;

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
    msg.WriteString(string(reinterpret_cast<const char*>(data), size - sizeof(int32_t)));
    msg.WriteInt32(TypeCast<int32_t>(data));

    fileAccessExtStub->CmdOpenFile(msg, reply);
    return true;
}

bool CmdCreateFileFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    int pos = size >> 1;
    msg.WriteString(string(reinterpret_cast<const char*>(data), pos));
    msg.WriteString(string(reinterpret_cast<const char*>(data + pos), size - pos));

    fileAccessExtStub->CmdCreateFile(msg, reply);
    return true;
}

bool CmdMkdirFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    int pos = size >> 1;
    msg.WriteString(string(reinterpret_cast<const char*>(data), pos));
    msg.WriteString(string(reinterpret_cast<const char*>(data + pos), size - pos));

    fileAccessExtStub->CmdMkdir(msg, reply);
    return true;
}

bool CmdDeleteFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    msg.WriteString(string(reinterpret_cast<const char*>(data), size));

    fileAccessExtStub->CmdDelete(msg, reply);
    return true;
}

bool CmdMoveFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    int pos = size >> 1;
    msg.WriteString(string(reinterpret_cast<const char*>(data), pos));
    msg.WriteString(string(reinterpret_cast<const char*>(data + pos), size - pos));

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
    msg.WriteString(string(reinterpret_cast<const char*>(data + 1), pos));
    msg.WriteString(string(reinterpret_cast<const char*>(data + pos + 1), size - pos - 1));
    msg.WriteBool(TypeCast<bool>(data));

    fileAccessExtStub->CmdCopy(msg, reply);
    return true;
}

bool CmdCopyFileFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    int pos = size / 3;
    msg.WriteString(string(reinterpret_cast<const char*>(data), pos));
    msg.WriteString(string(reinterpret_cast<const char*>(data + pos), pos));
    msg.WriteString(string(reinterpret_cast<const char*>(data + pos + pos), size - pos - pos));

    fileAccessExtStub->CmdCopyFile(msg, reply);
    return true;
}

bool CmdRenameFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    int pos = size >> 1;
    msg.WriteString(string(reinterpret_cast<const char*>(data), pos));
    msg.WriteString(string(reinterpret_cast<const char*>(data + pos), size - pos));

    fileAccessExtStub->CmdRename(msg, reply);
    return true;
}

bool CmdListFileFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    size_t minSize = sizeof(int64_t) + sizeof(int32_t) + sizeof(int64_t) + sizeof(int64_t) + sizeof(int64_t) +
        sizeof(double) + sizeof(bool) + sizeof(bool);
    if (data == nullptr || size < minSize) {
        return true;
    }

    int pos = 0;
    int64_t offset = TypeCast<int64_t>(data, &pos);
    FileInfo fileInfo;
    fileInfo.mode = TypeCast<int32_t>(data + pos, &pos);
    fileInfo.size = TypeCast<int64_t>(data + pos, &pos);
    fileInfo.mtime = TypeCast<int64_t>(data + pos, &pos);

    int64_t fileSizeOver = TypeCast<int64_t>(data + pos, &pos);
    double lastModifiedAfter = TypeCast<double>(data + pos, &pos);
    bool excludeMedia = TypeCast<bool>(data + pos, &pos);
    bool hasFilter = TypeCast<bool>(data + pos, &pos);

    int len = (size - pos) / 6;
    fileInfo.uri = string(reinterpret_cast<const char*>(data + pos), len);
    pos += len;
    fileInfo.relativePath = string(reinterpret_cast<const char*>(data + pos), len);
    pos += len;
    fileInfo.fileName = string(reinterpret_cast<const char*>(data + pos), len);
    pos += len;
    std::vector<std::string> suffix;
    suffix.push_back(string(reinterpret_cast<const char*>(data + pos), len));
    pos += len;
    std::vector<std::string> displayName;
    displayName.push_back(string(reinterpret_cast<const char*>(data + pos), len));
    pos += len;
    std::vector<std::string> mimeType;
    mimeType.push_back(string(reinterpret_cast<const char*>(data + pos), len));
    pos += len;
    FileFilter fileFilter(suffix, displayName, mimeType, fileSizeOver, lastModifiedAfter, excludeMedia, hasFilter);

    SharedMemoryInfo sharedMemoryInfo;
    MessageParcel msg;
    MessageParcel reply;
    msg.WriteParcelable(&fileInfo);
    msg.WriteInt64(offset);
    msg.WriteParcelable(&fileFilter);
    msg.WriteParcelable(&sharedMemoryInfo);

    fileAccessExtStub->CmdListFile(msg, reply);
    return true;
}

bool CmdScanFileFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    size_t minSize = sizeof(int64_t) + sizeof(int64_t) + sizeof(int32_t) + sizeof(int64_t) + sizeof(int64_t) +
        sizeof(int64_t) + sizeof(double) + sizeof(bool) + sizeof(bool);
    if (data == nullptr || size < minSize) {
        return true;
    }

    int pos = 0;
    int64_t offset = TypeCast<int64_t>(data, &pos);
    int64_t maxCount = TypeCast<int64_t>(data, &pos);
    FileInfo fileInfo;
    fileInfo.mode = TypeCast<int32_t>(data + pos, &pos);
    fileInfo.size = TypeCast<int64_t>(data + pos, &pos);
    fileInfo.mtime = TypeCast<int64_t>(data + pos, &pos);

    int64_t fileSizeOver = TypeCast<int64_t>(data + pos, &pos);
    double lastModifiedAfter = TypeCast<double>(data + pos, &pos);
    bool excludeMedia = TypeCast<bool>(data + pos, &pos);
    bool hasFilter = TypeCast<bool>(data + pos, &pos);

    int len = (size - pos) / 6;
    fileInfo.uri = string(reinterpret_cast<const char*>(data + pos), len);
    pos += len;
    fileInfo.relativePath = string(reinterpret_cast<const char*>(data + pos), len);
    pos += len;
    fileInfo.fileName = string(reinterpret_cast<const char*>(data + pos), len);
    pos += len;
    std::vector<std::string> suffix;
    suffix.push_back(string(reinterpret_cast<const char*>(data + pos), len));
    pos += len;
    std::vector<std::string> displayName;
    displayName.push_back(string(reinterpret_cast<const char*>(data + pos), len));
    pos += len;
    std::vector<std::string> mimeType;
    mimeType.push_back(string(reinterpret_cast<const char*>(data + pos), len));
    pos += len;
    FileFilter fileFilter(suffix, displayName, mimeType, fileSizeOver, lastModifiedAfter, excludeMedia, hasFilter);

    SharedMemoryInfo sharedMemoryInfo;
    MessageParcel msg;
    MessageParcel reply;
    msg.WriteParcelable(&fileInfo);
    msg.WriteInt64(offset);
    msg.WriteInt64(maxCount);
    msg.WriteParcelable(&fileFilter);

    fileAccessExtStub->CmdScanFile(msg, reply);
    return true;
}

bool CmdGetRootsFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;

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
    msg.WriteString(string(reinterpret_cast<const char*>(data + sizeof(int64_t)), size - sizeof(int64_t)));
    msg.WriteInt64(TypeCast<int64_t>(data));

    fileAccessExtStub->CmdQuery(msg, reply);
    return true;
}

bool CmdGetFileInfoFromUriFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    msg.WriteString(string(reinterpret_cast<const char*>(data), size));

    fileAccessExtStub->CmdGetFileInfoFromUri(msg, reply);
    return true;
}

bool CmdGetFileInfoFromRelativePathFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data,
    size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    msg.WriteString(string(reinterpret_cast<const char*>(data), size));

    fileAccessExtStub->CmdGetFileInfoFromRelativePath(msg, reply);
    return true;
}

bool CmdAccessFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    msg.WriteString(string(reinterpret_cast<const char*>(data), size));

    fileAccessExtStub->CmdAccess(msg, reply);
    return true;
}

bool CmdStartWatcherFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    msg.WriteString(string(reinterpret_cast<const char*>(data), size));

    fileAccessExtStub->CmdStartWatcher(msg, reply);
    return true;
}

bool CmdStopWatcherFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    msg.WriteString(string(reinterpret_cast<const char*>(data), size));

    fileAccessExtStub->CmdStopWatcher(msg, reply);
    return true;
}

bool CmdMoveItemFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return true;
    }

    MessageParcel msg;
    MessageParcel reply;
    int pos = (size - 1) >> 1;
    msg.WriteString(string(reinterpret_cast<const char*>(data + 1), pos));
    msg.WriteString(string(reinterpret_cast<const char*>(data + pos + 1), size - pos - 1));
    msg.WriteBool(TypeCast<bool>(data));

    fileAccessExtStub->CmdMoveItem(msg, reply);
    return true;
}

bool CmdMoveFileFuzzTest(shared_ptr<FileAccessExtStub> fileAccessExtStub, const uint8_t *data, size_t size)
{
    MessageParcel msg;
    MessageParcel reply;
    int pos = size / 3;
    msg.WriteString(string(reinterpret_cast<const char*>(data), pos));
    msg.WriteString(string(reinterpret_cast<const char*>(data + pos), pos));
    msg.WriteString(string(reinterpret_cast<const char*>(data + pos + pos), size - pos - pos));

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
    OHOS::CmdListFileFuzzTest(stub, data, size);
    OHOS::CmdScanFileFuzzTest(stub, data, size);
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
