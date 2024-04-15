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
#include "fileaccessextconnection_fuzzer.h"

#include <cstring>
#include <memory>

#include "hilog_wrapper.h"
#include "file_access_ext_connection.h"

namespace OHOS {
using namespace std;
using namespace FileAccessFwk;

bool OnAbilityConnectDoneFuzzTest(shared_ptr<FileAccessExtConnection> conn)
{
    AppExecFwk::ElementName element;
    sptr<IRemoteObject> remoteObject = nullptr;
    conn->OnAbilityConnectDone(element, remoteObject, 0);
    return true;
}

bool OnAbilityDisconnectDoneFuzzTest(shared_ptr<FileAccessExtConnection> conn)
{
    AppExecFwk::ElementName element;
    conn->OnAbilityDisconnectDone(element, 0);
    return true;
}

bool IsExtAbilityConnectedFuzzTest(shared_ptr<FileAccessExtConnection> conn)
{
    conn->IsExtAbilityConnected();
    return true;
}

bool GetFileExtProxyFuzzTest(shared_ptr<FileAccessExtConnection> conn)
{
    conn->GetFileExtProxy();
    return true;
}

} // namespace OHOS
