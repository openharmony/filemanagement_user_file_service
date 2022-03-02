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

static bool GetClientUid(int &uid)
{
    uid = IPCSkeleton::GetCallingUid();
    return true;
}

static sptr<AppExecFwk::IBundleMgr> GetSysBundleManager()
{
    auto bundleObj =
        OHOS::DelayedSingleton<AAFwk::SaMgrClient>::GetInstance()->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (bundleObj == nullptr) {
        ERR_LOG("failed to get bundle manager service");
        return nullptr;
    }
    sptr<AppExecFwk::IBundleMgr> bms = iface_cast<AppExecFwk::IBundleMgr>(bundleObj);
    return bms;
}

static string GetClientBundleName(int uid)
{
    std::string bundleName = "";
    auto bms = GetSysBundleManager();
    if (bms == nullptr) {
        ERR_LOG("failed to get bundle manager service bms == nullptr");
        return bundleName;
    }
    auto result = bms->GetBundleNameForUid(uid, bundleName);
    DEBUG_LOG("GetClientBundleName: bundleName is %{public}s ", bundleName.c_str());
    if (!result) {
        ERR_LOG("GetBundleNameForUid fail");
        return "";
    }
    return bundleName;
}

bool CheckClientPermission(const std::string& permissionStr)
{
    int uid = 0;
    if (!GetClientUid(uid)) {
        ERR_LOG("GetClientUid: fail ");
        return false;
    }
    if (uid == 0) {
        ERR_LOG("uid as root, white list pass");
        return true;
    }
    DEBUG_LOG("GetClientBundleName: uid is %{public}d ", uid);
    std::string bundleName = GetClientBundleName(uid);
    if (IsSameTextStr(bundleName, "ohos.acts.distributeddatamgr.distributedfile") ||
        IsSameTextStr(bundleName, "ohos.acts.storage.filemanager") ||
        IsSameTextStr(bundleName, "com.ohos.filepicker") ||
        IsSameTextStr(bundleName, "com.example.filemanager")) {
        DEBUG_LOG("CheckClientPermission: Pass the white list");
        return true;
    }
    return false;
}

int FileManagerServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    // change permission string after finishing accessToken
    string permission = "permission";
    if (!CheckClientPermission(permission)) {
        ERR_LOG("checkpermission error FAIL");
    }
    if (!MediaFileUtils::InitHelper(AsObject())) {
        ERR_LOG("InitHelper error %{public}d", FAIL);
        reply.WriteInt32(FAIL);
        return FAIL;
    }
    // do file process
    int32_t errCode = OperProcess(code, data, reply);
    reply.WriteInt32(errCode);
    return errCode;
}
} // namespace FileManagerService
} // namespace OHOS