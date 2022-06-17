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

#include "file_manager_proxy.h"
#include "fms_manager_proxy_mock.h"
#include "ifms_client.h"

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
    EXPECT_EQ(ret, 1);
    GTEST_LOG_(INFO) << "FileManagerProxyTest-end File_Manager_Proxy_Mkdir_0000";
}
} // namespace
