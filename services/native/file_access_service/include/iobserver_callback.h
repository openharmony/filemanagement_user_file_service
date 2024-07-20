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

#ifndef I_FILE_ACCESS_OBSERVER_H
#define I_FILE_ACCESS_OBSERVER_H

#include "file_access_observer_common.h"
#include "iremote_broker.h"
#include "iremote_object.h"

namespace OHOS {
namespace FileAccessFwk {
class IFileAccessObserver : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.FileAccessFwk.IFileAccessObserver");

public:
    enum {
        CMD_ONCHANGE = 0
    };
    virtual void OnChange(NotifyMessage &notifyMessage) = 0;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // I_FILE_ACCESS_OBSERVER_H
