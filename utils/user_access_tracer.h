/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_USER_ACCESS_TRACER
#define OHOS_USER_ACCESS_TRACER

#include <string.h>
#include "hitrace_meter.h"

class UserAccessTracer final {
public:
    UserAccessTracer() = default;

    virtual ~UserAccessTracer()
    {
        for (int32_t i = 0; i < count_; i++) {
            FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        }
        count_ = 0;
    }

    void Start(const std::string &label)
    {
        StartTrace(HITRACE_TAG_FILEMANAGEMENT, label);
        count_++;
    }

    void Finish()
    {
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        count_--;
    }
private:
    int32_t count_ = 0;
};

#endif // OHOS_USER_ACCESS_TRACER
