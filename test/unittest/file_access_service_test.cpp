/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "file_access_service.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "system_ability_definition.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::FileAccessFwk;
using namespace OHOS::FileManagement;
const std::string FILE_ACCESS_PERMISSION = "ohos.permission.FILE_ACCESS_MANAGER";
namespace OHOS {
namespace FileAccessFwk {
class FileAccessServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<FileAccessService> fileAccessSvc_;
};

void FileAccessServiceTest::SetUpTestCase(void)
{
    // Setup code to run before all tests
}

void FileAccessServiceTest::TearDownTestCase(void)
{
    // Teardown code to run after all tests
}

void FileAccessServiceTest::SetUp()
{
    int32_t saID = FILE_ACCESS_SERVICE_ID;
    bool runOnCreate = false;
    fileAccessSvc_ = std::make_shared<FileAccessService>(saID, runOnCreate);
    GTEST_LOG_(INFO) << "SetUp";
    // Setup code to run before each test
}

void FileAccessServiceTest::TearDown()
{
    // Teardown code to run after each test
    GTEST_LOG_(INFO) << "TearDown";
}

/**
 * @tc.number: FileAccessService_IsParentUri_001
 * @tc.name: FileAccessService_IsParentUri_001
 * @tc.desc: Verify IsParentUri function works correctly
 */
HWTEST_F(FileAccessServiceTest, FileAccessService_IsParentUri_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceTest-begin FileAccessService_IsParentUri_001";
    ASSERT_NE(fileAccessSvc_, nullptr) << "FileAccessService_IsParentUri_001 fileAccessSvc_ is nullptr";
    std::string cmpStr = "";
    std::string srcStr = "test";
    bool ret = fileAccessSvc_->IsParentUri(cmpStr, srcStr);
    EXPECT_FALSE(ret);

    cmpStr = "test";
    srcStr = "";
    ret = fileAccessSvc_->IsParentUri(cmpStr, srcStr);
    EXPECT_FALSE(ret);

    cmpStr = "test";
    srcStr = "test";
    ret = fileAccessSvc_->IsParentUri(cmpStr, srcStr);
    EXPECT_FALSE(ret);

    cmpStr = "test";
    srcStr = "tes/";
    ret = fileAccessSvc_->IsParentUri(cmpStr, srcStr);
    EXPECT_FALSE(ret);

    cmpStr = "test";
    srcStr = "test/";
    ret = fileAccessSvc_->IsParentUri(cmpStr, srcStr);
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "FileAccessServiceTest-end FileAccessService_IsParentUri_001";
}

/**
 * @tc.number: FileAccessService_IsChildUri_001
 * @tc.name: FileAccessService_IsChildUri_001
 * @tc.desc: Verify IsChildUri function works correctly
 */
HWTEST_F(FileAccessServiceTest, FileAccessService_IsChildUri_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceTest-begin FileAccessService_IsChildUri_001";
    ASSERT_NE(fileAccessSvc_, nullptr) << "FileAccessService_IsChildUri_001 fileAccessSvc_ is nullptr";
    std::string cmpStr = "";
    std::string srcStr = "test";
    bool ret = fileAccessSvc_->IsChildUri(cmpStr, srcStr);
    EXPECT_FALSE(ret);

    cmpStr = "test";
    srcStr = "";
    ret = fileAccessSvc_->IsChildUri(cmpStr, srcStr);
    EXPECT_FALSE(ret);

    cmpStr = "test";
    srcStr = "test";
    ret = fileAccessSvc_->IsChildUri(cmpStr, srcStr);
    EXPECT_FALSE(ret);

    cmpStr = "tes/";
    srcStr = "test";
    ret = fileAccessSvc_->IsChildUri(cmpStr, srcStr);
    EXPECT_FALSE(ret);

    cmpStr = "test/";
    srcStr = "test";
    ret = fileAccessSvc_->IsChildUri(cmpStr, srcStr);
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "FileAccessServiceTest-end FileAccessService_IsChildUri_001";
}

/**
 * @tc.number: FileAccessService_GetBundleNameFromUri_001
 * @tc.name: FileAccessService_GetBundleNameFromUri_001
 * @tc.desc: Get BundleName From Uri function
 */
HWTEST_F(FileAccessServiceTest, FileAccessService_GetBundleNameFromUri_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceTest-begin FileAccessService_GetBundleNameFromUri_001";
    ASSERT_NE(fileAccessSvc_, nullptr) << "FileAccessService_GetBundleNameFromUri_001 fileAccessSvc_ is nullptr";
    OHOS::Uri fileUri("Connect://com.ohos.fileaccess/test");
    std::string bundleName = "";
    bool ret = fileAccessSvc_->GetBundleNameFromUri(fileUri, bundleName);
    EXPECT_FALSE(ret);

    GTEST_LOG_(INFO) << "FileAccessServiceTest-end FileAccessService_GetBundleNameFromUri_001";
}

/**
 * @tc.number: FileAccessService_GetBundleNameFromUri_002
 * @tc.name: FileAccessService_GetBundleNameFromUri_002
 * @tc.desc: Get BundleName From Uri function
 */
HWTEST_F(FileAccessServiceTest, FileAccessService_GetBundleNameFromUri_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceTest-begin FileAccessService_GetBundleNameFromUri_002";
    ASSERT_NE(fileAccessSvc_, nullptr) << "FileAccessService_GetBundleNameFromUri_002 fileAccessSvc_ is nullptr";
    OHOS::Uri fileUri("file://");
    std::string bundleName = "";
    auto ret = fileAccessSvc_->GetBundleNameFromUri(fileUri, bundleName);
    EXPECT_FALSE(ret);

    OHOS::Uri fileUri1("file://media/photo/1/IMG_2020-09-01_15-09-09.jpg");
    bundleName = "";
    ret = fileAccessSvc_->GetBundleNameFromUri(fileUri1, bundleName);
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "FileAccessServiceTest-end FileAccessService_GetBundleNameFromUri_002";
}

/**
 * @tc.number: FileAccessService_GetBundleNameFromUri_004
 * @tc.name: FileAccessService_GetBundleNameFromUri_004
 * @tc.desc: Get BundleName From Uri function
 */
HWTEST_F(FileAccessServiceTest, FileAccessService_GetBundleNameFromUri_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceTest-begin FileAccessService_GetBundleNameFromUri_004";
    ASSERT_NE(fileAccessSvc_, nullptr) << "FileAccessService_GetBundleNameFromUri_004 fileAccessSvc_ is nullptr";
    OHOS::Uri fileUri("file://docs/storage/Users/currentUser/1.txt");
    std::string bundleName = "";
    auto ret = fileAccessSvc_->GetBundleNameFromUri(fileUri, bundleName);
    EXPECT_TRUE(ret);

    OHOS::Uri fileUri1("file://com.ohos.fileaccess/data/storage/el2/bsae/1.txt");
    bundleName = "test";
    ret = fileAccessSvc_->GetBundleNameFromUri(fileUri1, bundleName);
    EXPECT_FALSE(ret);

    GTEST_LOG_(INFO) << "FileAccessServiceTest-end FileAccessService_GetBundleNameFromUri_004";
}

/**
 * @tc.number: FileAccessService_CleanAllNotify_001
 * @tc.name: FileAccessService_CleanAllNotify_001
 * @tc.desc: CleanAllNotify function
 */
HWTEST_F(FileAccessServiceTest, FileAccessService_CleanAllNotify_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceTest-begin FileAccessService_CleanAllNotify_001";
    ASSERT_NE(fileAccessSvc_, nullptr) << "FileAccessService_CleanAllNotify_001 fileAccessSvc_ is nullptr";
    OHOS::Uri uri("");
    int32_t ret = fileAccessSvc_->CleanAllNotify(uri, nullptr);
    EXPECT_NE(ret, 0);
    GTEST_LOG_(INFO) << "FileAccessServiceTest-end FileAccessService_CleanAllNotify_001";
}

/**
 * @tc.number: FileAccessService_CleanAllNotify_002
 * @tc.name: FileAccessService_CleanAllNotify_002
 * @tc.desc: CleanAllNotify function
 */
HWTEST_F(FileAccessServiceTest, FileAccessService_CleanAllNotify_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceTest-begin FileAccessService_CleanAllNotify_002";
    ASSERT_NE(fileAccessSvc_, nullptr) << "FileAccessService_CleanAllNotify_002 fileAccessSvc_ is nullptr";
    OHOS::Uri uri1("file://docs");
    int32_t ret = fileAccessSvc_->CleanAllNotify(uri1, nullptr);
    EXPECT_NE(ret, 0);
    OHOS::Uri uri2("file://com.demo/data/storage/el2/base/1.txt");
    ret = fileAccessSvc_->CleanAllNotify(uri2, nullptr);
    EXPECT_NE(ret, 0);
    GTEST_LOG_(INFO) << "FileAccessServiceTest-end FileAccessService_CleanAllNotify_002";
}

/**
 * @tc.number: FileAccessService_CheckCallingPermission_001
 * @tc.name: FileAccessService_CheckCallingPermission_001
 * @tc.desc: CheckCallingPermission function
 */
HWTEST_F(FileAccessServiceTest, FileAccessService_CheckCallingPermission_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceTest-begin FileAccessService_CheckCallingPermission_001";
    ASSERT_NE(fileAccessSvc_, nullptr) << "FileAccessService_CheckCallingPermission_001 fileAccessSvc_ is nullptr";
    bool ret = fileAccessSvc_->CheckCallingPermission(FILE_ACCESS_PERMISSION);
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << "FileAccessServiceTest-end FileAccessService_CheckCallingPermission_001";
}
} // namespace FileAccessFwk
} // namespace OHOS