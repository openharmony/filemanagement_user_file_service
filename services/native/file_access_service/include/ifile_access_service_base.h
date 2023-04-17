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

#ifndef I_FILE_ACCESS_SERVICE_BASE_H
#define I_FILE_ACCESS_SERVICE_BASE_H

#include "iremote_broker.h"
#include "iremote_object.h"
#include "iobserver_callback.h"
#include "uri.h"

namespace OHOS {
namespace FileAccessFwk {
using Uri = OHOS::Uri;
class IFileAccessServiceBase : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.FileAccessFwk.IFileAccessServiceBase");

public:
    enum {
        CMD_REGISTER_NOTIFY = 0,
        CMD_UNREGISTER_NOTIFY
    };
    virtual int32_t RegisterNotify(Uri uri, const sptr<IFileAccessObserver> &observer, bool notifyForDescendants) = 0;
    virtual int32_t UnregisterNotify(Uri uri, const sptr<IFileAccessObserver> &observer) = 0;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // I_FILE_ACCESS_SERVICE_BASE_H