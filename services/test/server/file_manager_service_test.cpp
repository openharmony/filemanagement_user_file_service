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

#include "abs_shared_result_set.h"
#include "file_info.h"
#include "file_manager_service.h"
#include "file_manager_service_def.h"
#include "file_manager_service_stub.h"
#include "ifms_client.h"
#include "medialibrary_db_const.h"
#include "media_file_oper.h"

namespace {
using namespace std;
using namespace OHOS;
using namespace FileManagerService;
class FileManagerServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        cout << "FileManagerServiceTest code test" << endl;
    }
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_STORAGE_file_Manager_Service_OnDump_0000
 * @tc.name: file_Manager_Service_OnDump_0000
 * @tc.desc: Test function of OnDump interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000GJ9T3
 */
HWTEST_F(FileManagerServiceTest, file_Manager_Service_OnDump_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileManagerServiceTest-begin file_Manager_Service_OnDump_0000";
    try {
        OHOS::FileManagerService::FileManagerService *service = new OHOS::FileManagerService::FileManagerService(0);
        if (service != nullptr) {
            service->OnDump();
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileManagerServiceTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileManagerServiceTest-end file_Manager_Service_OnDump_0000";
}

/**
 * @tc.number: SUB_STORAGE_file_Manager_Service_OnStart_0000
 * @tc.name: file_Manager_Service_OnStart_0000
 * @tc.desc: Test function of  OnStart interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000GJ9T3
 */
HWTEST_F(FileManagerServiceTest, file_Manager_Service_OnStart_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileManagerServiceTest-begin file_Manager_Service_OnStart_0000";
    try {
        OHOS::FileManagerService::FileManagerService *service = new OHOS::FileManagerService::FileManagerService(0);
        if (service != nullptr) {
            service->OnStart();
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileManagerServiceTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileManagerServiceTest-end file_Manager_Service_OnStart_0000";
}

/**
 * @tc.number: SUB_STORAGE_file_Manager_Service_OnStop_0000
 * @tc.name: file_Manager_Service_OnStop_0000
 * @tc.desc: Test function of  OnStop interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000GJ9T3
 */
HWTEST_F(FileManagerServiceTest, Ffile_Manager_Service_OnStop_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileManagerServiceTest-begin file_Manager_Service_OnStop_0000";
    try {
        OHOS::FileManagerService::FileManagerService *service = new OHOS::FileManagerService::FileManagerService(0);
        if (service != nullptr) {
            service->OnStop();
        }
    } catch (...) {
        GTEST_LOG_(INFO) << "FileManagerServiceTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "FileManagerServiceTest-end file_Manager_Service_OnStop_0000";
}
} // namespace
