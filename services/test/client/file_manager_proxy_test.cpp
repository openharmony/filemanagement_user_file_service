/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdio>

#include <gtest/gtest.h>

#include "ifms_client.h"
#include "file_manager_proxy.h"
#include "fms_manager_proxy_mock.h"

namespace {
using namespace std;
using namespace OHOS;
using namespace FileManagerService;
class FileManagerProxyTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        cout << "FileManagerProxyTest code test" << endl;
    }
    static void TearDownTestCase() {};
    void SetUp();
    void TearDown() {};
    std::shared_ptr<FileManagerProxy> proxy_ = nullptr;
    sptr<FmsManagerProxyMock> mock_ = nullptr;
};

void FileManagerProxyTest::SetUp()
{
    mock_ = new OHOS::FileManagerService::FmsManagerProxyMock();
    proxy_ = std::make_shared<FileManagerProxy>(mock_);
}

/**
 * @tc.number: SUB_STORAGE_File_Manager_Proxy_GetRoot_0000
 * @tc.name: File_Manager_Proxy_GetRoot_0000
 * @tc.desc: Test function of GetRoot interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000GJ9T3
 */
HWTEST_F(FileManagerProxyTest, File_Manager_Proxy_GetRoot_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileManagerProxyTest-begin File_Manager_Proxy_GetRoot_0000";
    CmdOptions option;
    std::vector<std::shared_ptr<FileInfo>> fileRes;
    int ret = proxy_->GetRoot(option, fileRes);
    ASSERT_TRUE(ret == 0);
    GTEST_LOG_(INFO) << "FileManagerProxyTest-end File_Manager_Proxy_GetRoot_0000";
}

/**
 * @tc.number: SUB_STORAGE_File_Manager_Proxy_GetFmsInstance_0000
 * @tc.name: File_Manager_Proxy_GetFmsInstance_0000
 * @tc.desc: Test function of GetFmsInstance interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000GJ9T3
 */
HWTEST_F(FileManagerProxyTest, File_Manager_Proxy_GetFmsInstance_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileManagerProxyTest-begin File_Manager_Proxy_GetFmsInstance_0000";
    try {
        IFmsClient* result = IFmsClient::GetFmsInstance();
        EXPECT_NE(result, nullptr);
    } catch (...) {
        GTEST_LOG_(INFO) << "FileManagerProxyTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileManagerProxyTest-end File_Manager_Proxy_GetFmsInstance_0000";
}

/**
 * @tc.number: SUB_STORAGE_File_Manager_Proxy_Mkdir_0000
 * @tc.name: File_Manager_Proxy_Mkdir_0000
 * @tc.desc: Test function of Mkdir interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000GJ9T3
 */
HWTEST_F(FileManagerProxyTest, File_Manager_Proxy_Mkdir_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileManagerProxyTest-begin File_Manager_Proxy_Mkdir_0000";
    std::string name = "File_Manager_Proxy_Mkdir_0000";
    std::string path = "dataability:///album";
    EXPECT_CALL(*mock_, SendRequest(testing::_, testing::_, testing::_, testing::_))
        .Times(1)
        .WillOnce(testing::Invoke(mock_.GetRefPtr(), &FmsManagerProxyMock::InvokeSendRequest));
    int ret = proxy_->Mkdir(name, path);
    EXPECT_TRUE(ret == ERR_NONE);
    GTEST_LOG_(INFO) << "FileManagerProxyTest-end File_Manager_Proxy_Mkdir_0000";
}

/**
 * @tc.number: SUB_STORAGE_File_Manager_Proxy_ListFile_0000
 * @tc.name: File_Manager_Proxy_ListFile_0000
 * @tc.desc: Test function of ListFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000GJ9T3
 */
HWTEST_F(FileManagerProxyTest, File_Manager_Proxy_ListFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileManagerProxyTest-begin File_Manager_Proxy_ListFile_0000";
    std::string type = "file";
    std::string path = "dataability:///album";
    CmdOptions option;
    std::vector<std::shared_ptr<FileInfo>> fileRes;
    EXPECT_CALL(*mock_, SendRequest(testing::_, testing::_, testing::_, testing::_))
        .Times(1)
        .WillOnce(testing::Invoke(mock_.GetRefPtr(), &FmsManagerProxyMock::InvokeSendRequest));
    int ret = proxy_->ListFile(type, path, option, fileRes);
    ASSERT_TRUE(ret == ERR_NONE);
    GTEST_LOG_(INFO) << "FileManagerProxyTest-end File_Manager_Proxy_ListFile_0000";
}

/**
 * @tc.number: SUB_STORAGE_File_Manager_Proxy_CreateFile_0000
 * @tc.name: File_Manager_Proxy_CreateFile_0000
 * @tc.desc: Test function of CreateFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000GJ9T3
 */
HWTEST_F(FileManagerProxyTest, File_Manager_Proxy_CreateFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileManagerProxyTest-begin File_Manager_Proxy_CreateFile_0000";
    std::string path = "dataability:///album";
    std::string fileName = "FMS_File_ManagerProxy_Mkdir_0000";
    CmdOptions option;
    std::string uri;
    EXPECT_CALL(*mock_, SendRequest(testing::_, testing::_, testing::_, testing::_))
        .Times(1)
        .WillOnce(testing::Invoke(mock_.GetRefPtr(), &FmsManagerProxyMock::InvokeSendRequest));
    int ret = proxy_->CreateFile(path, fileName, option, uri);
    ASSERT_TRUE(ret == ERR_NONE);
    GTEST_LOG_(INFO) << "FileManagerProxyTest-end File_Manager_Proxy_CreateFile_0000";
}
} // namespace