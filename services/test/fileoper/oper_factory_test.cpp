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

#include "file_manager_service_def.h"
#include "file_manager_service_stub.h"
#include "media_data_ability_const.h"
#include "abs_shared_result_set.h"

namespace {
using namespace std;
using namespace OHOS;
using namespace FileManagerService;
class OperFactoryTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        cout << "OperFactoryTest code test" << endl;
    }
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_STORAGE_oper_factory_GetFileOper_0000
 * @tc.name: oper_factory_GetFileOper_0000
 * @tc.desc: Test function of  GetFileOper interface for SUCCESS which INTERNAL_STORAGE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000GJ9T3
 */
HWTEST_F(OperFactoryTest, oper_factory_GetFileOper_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OperFactoryTest-begin oper_factory_GetFileOper_0000";
    try {
        OperFactory *oper = new OperFactory();
        std::unique_ptr<FileOper> result;
        if (oper != nullptr) {
            result = oper->GetFileOper(Equipment::INTERNAL_STORAGE);
        }
        EXPECT_NE(result, nullptr);
    } catch (...) {
        GTEST_LOG_(INFO) << "OperFactoryTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "OperFactoryTest-end oper_factory_GetFileOper_0000";
}

/**
 * @tc.number: SUB_STORAGE_oper_factory_GetFileOper_0001
 * @tc.name: oper_factory_GetFileOper_0001
 * @tc.desc: Test function of  GetFileOper for SUCCESS which EXTERNAL_STORAGE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000GJ9T3
 */
HWTEST_F(OperFactoryTest, oper_factory_GetFileOper_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OperFactoryTest-begin oper_factory_GetFileOper_0001";
    try {
        OperFactory *oper = new OperFactory();
        std::unique_ptr<FileOper> result;
        if (oper != nullptr) {
            result = oper->GetFileOper(Equipment::EXTERNAL_STORAGE);
        }
        EXPECT_NE(result, nullptr);
    } catch (...) {
        GTEST_LOG_(INFO) << "OperFactoryTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "OperFactoryTest-end oper_factory_GetFileOper_0001";
}

/**
 * @tc.number: SUB_STORAGE_oper_factory_GetFileOper_0002
 * @tc.name: oper_factory_GetFileOper_0002
 * @tc.desc: Test function of  GetFileOper interface for SUCCESS wich default.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: AR000GJ9T3
 */
HWTEST_F(OperFactoryTest, oper_factory_GetFileOper_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OperFactoryTest-begin oper_factory_GetFileOper_0002";
    try {
        OperFactory *oper = new OperFactory();
        std::unique_ptr<FileOper> result;
        if (oper != nullptr) {
            result = oper->GetFileOper(3);
        }
        EXPECT_EQ(result, nullptr);
    } catch (...) {
        GTEST_LOG_(INFO) << "OperFactoryTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "OperFactoryTest-end oper_factory_GetFileOper_0002";
}
} // namespace