/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "file_manager_service_stub.h"

#include "accesstoken_kit.h"
#include "file_manager_service_def.h"
#include "file_manager_service_errno.h"
#include "file_manager_service.h"
#include "ipc_singleton.h"
#include "ipc_skeleton.h"
#include "log.h"
#include "media_file_utils.h"
#include "oper_factory.h"
#include "sa_mgr_client.h"
#include "string_ex.h"
#include "system_ability_definition.h"
using namespace std;
namespace OHOS {
namespace FileManagerService {
static int GetEquipmentCode(uint32_t code)
{
    return (code >> EQUIPMENT_SHIFT) & CODE_MASK;
}

static int GetOperCode(uint32_t code)
{
    return code & CODE_MASK;
}

int FileManagerServiceStub::OperProcess(uint32_t code, MessageParcel &data,
    MessageParcel &reply)
{
    int equipmentId = GetEquipmentCode(code);
    int operCode = GetOperCode(code);
    OperFactory factory = OperFactory();
    auto fp = factory.GetFileOper(equipmentId);
    if (fp == nullptr) {
        ERR_LOG("OnRemoteRequest inner error %{public}d", code);
        return FAIL;
    }
    int errCode = fp->OperProcess(operCode, data, reply);
    return errCode;
}

bool CheckClientPermission(const std::string& permissionStr)
{
    Security::AccessToken::AccessTokenID tokenCaller = IPCSkeleton::GetCallingTokenID();
    int res = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller,
        permissionStr);
    if (res != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        ERR_LOG("Have no media permission");
        return false;
    }
    ERR_LOG("permission check success");
    return true;
}

int FileManagerServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    // check whether request from fms proxy
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        ERR_LOG("reject error remote request");
        reply.WriteInt32(FAIL);
        return FAIL;
    }
    // change permission string after finishing accessToken
    string permission = "ohos.permission.READ_MEDIA";
    if (!CheckClientPermission(permission)) {
        ERR_LOG("checkpermission error FAIL");
        reply.WriteInt32(FAIL);
        return FAIL;
    }
    if (!MediaFileUtils::InitHelper(AsObject())) {
        ERR_LOG("Init MediaLibraryDataAbility Helper error");
        reply.WriteInt32(FAIL);
        return FAIL;
    }
    // do request process
    int32_t errCode = OperProcess(code, data, reply);
    reply.WriteInt32(errCode);
    return errCode;
}
} // namespace FileManagerService
} // namespace OHOS
