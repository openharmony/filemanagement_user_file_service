/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <cstdio>
#include <unistd.h>
#include <gtest/gtest.h>

#include "ability_info.h"
#include "accesstoken_kit.h"
#include "context_impl.h"
#include "extension_base.h"
#include "extension_context.h"
#include "file_access_ext_ability.h"
#include "file_access_ext_stub_impl.h"
#include "file_access_observer_common.h"
#include "file_access_extension_info.h"
#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"
#include "hitrace_meter.h"
#include "runtime.h"
#include "js_runtime.h"
#include "js_runtime_utils.h"
#include "js_file_access_ext_ability.h"

namespace {
using namespace std;
using namespace OHOS::FileAccessFwk;

using namespace OHOS::AbilityRuntime;
FileAccessExtAbility* g_ability = nullptr ;

class JsFileAccessExtAbilityTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        cout << "JsFileAccessExtAbilityTest code test" <<endl;
        Runtime::Options options_;
        options_.bundleName = "com.ohos.UserFile.ExternalFileManager";
        options_.codePath = "/data/storage/el1/bundle";
        options_.loadAce = false;
        options_.isBundle = true;
        options_.preload = false;
        unique_ptr<Runtime> runtime = JsRuntime::Create(options_);
        g_ability = FileAccessExtAbility::Create(runtime);
    }
    static void TearDownTestCase()
    {
        delete g_ability;
        g_ability = nullptr;
    };
    void SetUp(){};
    void TearDown(){};
};

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_OpenFile_0000
 * @tc.name: js_file_access_ext_ability_OpenFile_0000
 * @tc.desc: Test function of OpenFile for error which CallJsMethod error
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7U2SX
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_OpenFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_OpenFile_0000";
    try{
    Uri uri("");
    int flags = WRITE_READ;
    int fd;
    int result = g_ability -> OpenFile(uri, flags, fd);
    EXPECT_NE(result, ERR_OK);
    } catch(...) {
        GTEST_LOG_(ERROR) << "js_file_access_ext_ability_OpenFile_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_OpenFile_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_OpenFile_0001
 * @tc.name: js_file_access_OpenFile_0001
 * @tc.desc: Test function of OpenFile interface for failed which openfile's fd is failed.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_OpenFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_OpenFile_0001";
    try{
    Uri uri("");
    int flags = WRITE_READ;
    int fd;
    g_ability -> OpenFile(uri, flags, fd);
    EXPECT_LE(fd, ERR_OK);
    } catch(...) {
        GTEST_LOG_(ERROR) << "js_file_access_ext_ability_OpenFile_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_OpenFile_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_OpenFile_0002
 * @tc.name: js_file_access_OpenFile_0002
 * @tc.desc: Test function of OpenFile interface for failed which openfile's uri is null.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_OpenFile_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_OpenFile_0002";
    try{
    Uri uri("");
    int flags = WRITE_READ;
    int fd;
    int result = g_ability -> OpenFile(uri, flags, fd);
    EXPECT_NE(result, E_URIS);
    } catch(...) {
        GTEST_LOG_(ERROR) << "js_file_access_ext_ability_OpenFile_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_OpenFile_0002";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CreateFile_0000
 * @tc.name: js_file_access_ext_ability_CreateFile_0000
 * @tc.desc: Test function of CreateFile for error which CallJsMethod error
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7U2SX
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_CreateFile_0000, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin js_file_access_ext_ability_CreateFile_0000";
    try {
        Uri parent("");
        std::string displayName("testFile");
        Uri newFile("");
        int result = g_ability->CreateFile(parent, displayName, newFile);
        EXPECT_NE(result, ERR_OK);
    } catch(...) {
        GTEST_LOG_(ERROR) << "js_file_access_ext_ability_CreateFile_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_CreateFile_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Mkdir_0000
 * @tc.name: js_file_access_ext_ability_Mkdir_0000
 * @tc.desc: Test function of Mkdir for error which CallJsMethod error
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7U2SX
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Mkdir_0000, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin js_file_access_ext_ability_Mkdir_0000";
    try {
        Uri parent("");
        std::string displayName("testFile");
        Uri newFile("");
        int result = g_ability->Mkdir(parent, displayName, newFile);
        EXPECT_NE(result, ERR_OK);
    } catch(...) {
        GTEST_LOG_(ERROR) << "js_file_access_ext_ability_Mkdir_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Mkdir_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Delete_0000
 * @tc.name: js_file_access_ext_ability_Delete_0000
 * @tc.desc: Test function of Delete for error which CallJsMethod error
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7U2SX
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Delete_0000, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin js_file_access_ext_ability_Delete_0000";
    try {
        Uri sourcefile("");
        int result = g_ability->Delete(sourcefile);
        EXPECT_NE(result, ERR_OK);
    } catch(...) {
        GTEST_LOG_(ERROR) << "js_file_access_ext_ability_Delete_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Delete_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Move_0000
 * @tc.name: js_file_access_ext_ability_Move_0000
 * @tc.desc: Test function of Move for error which CallJsMethod error
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7U2SX
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Move_0000, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin js_file_access_ext_ability_Move_0000";
    try {
        Uri sourcefile("");
        Uri targetParent("");
        Uri newFile("");
        int result = g_ability->Move(sourcefile, targetParent, newFile);
        EXPECT_NE(result, ERR_OK);
    } catch(...) {
        GTEST_LOG_(ERROR) << "js_file_access_ext_ability_Move_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Move_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Rename_0000
 * @tc.name: js_file_access_ext_ability_Rename_0000
 * @tc.desc: Test function of Rename for error which CallJsMethod error
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7U2SX
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Rename_0000, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin js_file_access_ext_ability_Rename_0000";
    try {
        Uri sourcefile("");
        std::string displayName("testFile");
        Uri newFile("");
        int result = g_ability->Rename(sourcefile, displayName, newFile);
        EXPECT_NE(result, ERR_OK);
    } catch(...) {
        GTEST_LOG_(ERROR) << "js_file_access_ext_ability_Rename_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Rename_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ListFile_0000
 * @tc.name: js_file_access_ext_ability_ListFile_0000
 * @tc.desc: Test function of ListFile for error which CallJsMethod error
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7U2SX
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ListFile_0000, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin js_file_access_ext_ability_ListFile_0000";
    try {
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        std::vector<FileInfo> fileInfoVec;
        int result = g_ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_NE(result, ERR_OK);
    } catch(...) {
        GTEST_LOG_(ERROR) << "js_file_access_ext_ability_ListFile_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ListFile_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ScanFile_0000
 * @tc.name: js_file_access_ext_ability_ScanFile_0000
 * @tc.desc: Test function of ScanFile for error which CallJsMethod error
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7U2SX
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ScanFile_0000, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin js_file_access_ext_ability_ScanFile_0000";
    try {
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        std::vector<FileInfo> fileInfoVec;
        int result = g_ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_NE(result, ERR_OK);
    } catch(...) {
        GTEST_LOG_(ERROR) << "js_file_access_ext_ability_ScanFile_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ScanFile_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetRoots_0000
 * @tc.name: js_file_access_ext_ability_GetRoots_0000
 * @tc.desc: Test function of GetRoots for error which CallJsMethod error
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7U2SX
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetRoots_0000, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin js_file_access_ext_ability_GetRoots_0000";
    try {
        std::vector<RootInfo> rootInfoVec;
        int result = g_ability->GetRoots(rootInfoVec);
        EXPECT_NE(result, ERR_OK);
    } catch(...) {
        GTEST_LOG_(ERROR) << "js_file_access_ext_ability_ScanFile_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ScanFile_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Access_0000
 * @tc.name: js_file_access_ext_ability_Access_0000
 * @tc.desc: Test function of Access for error which CallJsMethod error
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7U2SX
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Access_0000, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin js_file_access_ext_ability_Access_0000";
    try {
        Uri uri("someUriString");
        bool isExist = true;
        int result = g_ability->Access(uri, isExist);
        EXPECT_NE(result, ERR_OK);
    } catch(...) {
        GTEST_LOG_(ERROR) << "js_file_access_ext_ability_Access_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Access_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Query_0000
 * @tc.name: js_file_access_ext_ability_Query_0000
 * @tc.desc: Test function of Query for error which CallJsMethod error
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7U2SX
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Query_0000, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin js_file_access_ext_ability_Query_0000";
    try {
        Uri uri("someUriString");
        std::vector<std::string> columns;
        std::vector<std::string> results;
        int result = g_ability->Query(uri, columns, results);
        EXPECT_NE(result, ERR_OK);
    } catch(...) {
        GTEST_LOG_(ERROR) << "js_file_access_ext_ability_Query_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Query_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetFileInfoFromUri_0000
 * @tc.name: js_file_access_ext_ability_GetFileInfoFromUri_0000
 * @tc.desc: Test function of GetFileInfoFromUri for error which CallJsMethod error
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7U2SX
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetFileInfoFromUri_0000,
        testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin js_file_access_ext_ability_GetFileInfoFromUri_0000";
    try {
        Uri uri("someUriString");
        FileInfo fileInfo;
        int result = g_ability->GetFileInfoFromUri(uri, fileInfo);
        EXPECT_NE(result, ERR_OK);
    } catch(...) {
        GTEST_LOG_(ERROR) << "js_file_access_ext_ability_GetFileInfoFromUri_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetFileInfoFromUri_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetFileInfoFromRelativePath_0000
 * @tc.name: js_file_access_ext_ability_GetFileInfoFromRelativePath_0000
 * @tc.desc: Test function of GetFileInfoFromRelativePath for error which CallJsMethod error
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7U2SX
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetFileInfoFromRelativePath_0000,
        testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin js_file_access_ext_ability_GetFileInfoFromRelativePath_0000";
    try {
        std::string selectFileRealtivePath;
        FileInfo fileInfo;
        int result = g_ability->GetFileInfoFromRelativePath(selectFileRealtivePath, fileInfo);
        EXPECT_NE(result, ERR_OK);
    } catch(...) {
        GTEST_LOG_(ERROR) << "js_file_access_ext_ability_GetFileInfoFromRelativePath_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetFileInfoFromRelativePath_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_StartWatcher_0000
 * @tc.name: js_file_access_ext_ability_StartWatcher_0000
 * @tc.desc: Test function of StartWatcher for error which CallJsMethod error
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7U2SX
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_StartWatcher_0000, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin js_file_access_ext_ability_StartWatcher_0000";
    try {
        Uri uri("someUriString");
        int result = g_ability->StartWatcher(uri);
        EXPECT_NE(result, ERR_OK);
    } catch(...) {
        GTEST_LOG_(ERROR) << "js_file_access_ext_ability_StartWatcher_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_StartWatcher_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_StopWatcher_0000
 * @tc.name: js_file_access_ext_ability_StopWatcher_0000
 * @tc.desc: Test function of StopWatcher for error which CallJsMethod error
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7U2SX
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_StopWatcher_0000, testing::ext::TestSize.Level1) {
    GTEST_LOG_(INFO) << "FileExtensionHelperTest-begin js_file_access_ext_ability_StopWatcher_0000";
    try {
        Uri uri("someUriString");
        int result = g_ability->StopWatcher(uri, true);
        EXPECT_NE(result, ERR_OK);
    } catch(...) {
        GTEST_LOG_(ERROR) << "js_file_access_ext_ability_StopWatcher_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_StopWatcher_0000";
}
}
