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

#ifndef TEST_UNITTEST_MOCK_FILE_ACCESS_EXT_ABILITY_MOCK_H
#define TEST_UNITTEST_MOCK_FILE_ACCESS_EXT_ABILITY_MOCK_H

#include <gmock/gmock.h>

#include "file_access_ext_ability.h"

namespace OHOS {
namespace FileAccessFwk {
using namespace Media;
class FileAccessExtAbilityMock : public FileAccessExtAbility {
public:
    MOCK_METHOD4(Init, void (const std::shared_ptr<AbilityLocalRecord> &record,
                            const std::shared_ptr<OHOSApplication> &application,
                            std::shared_ptr<AbilityHandler> &handler,
                            const sptr<IRemoteObject> &token));
    MOCK_METHOD3(OpenFile, int(const Uri &uri, const int flags, int &fd));
    MOCK_METHOD3(CreateFile, int(const Uri &parent, const std::string &displayName, Uri &newFile));
    MOCK_METHOD3(Mkdir, int(const Uri &parent, const std::string &displayName, Uri &newFile));
    MOCK_METHOD1(Delete, int(const Uri &sourceFile));
    MOCK_METHOD3(Move, int(const Uri &sourceFile, const Uri &targetParent, Uri &newFile));
    MOCK_METHOD4(Copy, int(const Uri &sourceUri, const Uri &destUri, std::vector<Result> &copyResult, bool force));
    MOCK_METHOD4(CopyFile, int(const Uri &sourceUri, const Uri &destUri, const std::string &fileName, Uri &newFileUri));
    MOCK_METHOD3(Rename, int(const Uri &sourceFile, const std::string &displayName, Uri &newFile));
    MOCK_METHOD5(ListFile, int(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
        const FileFilter &filter, std::vector<FileInfo> &fileInfoVec));
    MOCK_METHOD5(ScanFile, int(const FileInfo &fileInfo, const int64_t offset, const int64_t maxCount,
        const FileFilter &filter, std::vector<FileInfo> &fileInfoVec));
    MOCK_METHOD3(GetThumbnail, int(const Uri &uri, const Size &size, std::unique_ptr<PixelMap> &pixelMap));
    MOCK_METHOD3(Query, int(const Uri &uri, std::vector<std::string> &columns, std::vector<std::string> &results));
    MOCK_METHOD2(GetFileInfoFromUri, int(const Uri &selectFile, FileInfo &fileInfo));
    MOCK_METHOD2(GetFileInfoFromRelativePath, int(const std::string &selectFile, FileInfo &fileInfo));
    MOCK_METHOD1(GetRoots, int(std::vector<RootInfo> &rootInfoVec));
    MOCK_METHOD2(Access, int(const Uri &uri, bool &isExist));
    MOCK_METHOD1(StartWatcher, int(const Uri &uri));
    MOCK_METHOD1(StopWatcher, int(const Uri &uri));
    MOCK_METHOD4(MoveItem, int(const Uri &sourceFile, const Uri &targetParent, std::vector<Result> &moveResult,
        bool force));
    MOCK_METHOD4(MoveFile, int(const Uri &sourceFile, const Uri &targetParent, std::string &fileName, Uri &newFile));
    MOCK_METHOD4(GetFileInfoNum, int(const std::string &sourceFileUri, const FileFilter &filter, bool recursion,
        uint32_t &counts));
};
} // End of namespace NativePreferences
} // End of namespace OHOS
#endif // TEST_UNITTEST_MOCK_FILE_ACCESS_EXT_ABILITY_MOCK_H