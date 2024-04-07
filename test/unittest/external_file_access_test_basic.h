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
#ifndef OHOS_EXTERNAL_FILE_ACCESS_TEST_BASIC_H
#define OHOS_EXTERNAL_FILE_ACCESS_TEST_BASIC_H

#include <gtest/gtest.h>

#define private public
#include "file_access_helper.h"
#undef private

namespace OHOS {
namespace FileAccessFwk {
using namespace std;

const int ABILITY_ID = 5003;
const int INIT_THREADS_NUMBER = 4;
const int ACTUAL_SUCCESS_THREADS_NUMBER = 1;
const int UID_TRANSFORM_TMP = 20000000;
const int SLEEP_TIME = 2;
const int UID_DEFAULT = 0;
const int FILE_COUNT_1 = 1;
const int FILE_COUNT_2 = 2;
const int FILE_COUNT_3 = 3;
const int FILE_COUNT_4 = 4;
const int FILE_COUNT_5 = 5;
const int FILE_COUNT_6 = 6;

class FileExtensionHelperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static bool ReplaceBundleNameFromPath(std::string &path, const std::string &newName);
    static shared_ptr<FileAccessHelper> GetFileAccessHelper();
};
} // namespace FileAccessFwk
} // namespace OHOS

#endif // OHOS_EXTERNAL_FILE_ACCESS_TEST_H
