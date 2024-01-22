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

#ifndef TEST_UNITTEST_MOCK_FILE_ASSISTANT_H
#define TEST_UNITTEST_MOCK_FILE_ASSISTANT_H

#include <gmock/gmock.h>

namespace OHOS {
namespace FileAccessFwk {
class Assistant {
public:
    virtual ~Assistant() = default;
    virtual bool Bool() = 0;
    virtual int Int() = 0;
public:
    static std::shared_ptr<Assistant> ins_;
};

class AssistantMock : public Assistant {
public:
    MOCK_METHOD0(Bool, bool());
    MOCK_METHOD0(Int, int());
};

} // End of namespace NativePreferences
} // End of namespace OHOS
#endif // TEST_UNITTEST_MOCK_FILE_ASSISTANT_H