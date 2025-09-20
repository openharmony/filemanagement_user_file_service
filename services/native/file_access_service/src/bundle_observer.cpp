/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "bundle_observer.h"
#include "file_access_service.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include  "hilog_wrapper.h"
#include "iservice_registry.h"
#include "notify_work_service.h"
#include "system_ability_definition.h"
#include "ufs_db_services_constants.h"
#include "ufs_rdb_adapter.h"

namespace OHOS {
namespace FileAccessFwk {

const int32_t SA_ID = 5010;
const int32_t APP_DEFAULT_INDEX = 0;
const int32_t USER_ID_DEFAULT = 100;

BundleObserver::BundleObserver(const EventFwk::CommonEventSubscribeInfo &subscribeInfo,
    wptr<FileAccessService> fileAccessSvc)
    :CommonEventSubscriber(subscribeInfo), fileAccessSvc_(fileAccessSvc)
{
}

BundleObserver::~BundleObserver()
{
}

int32_t BundleObserver::HandleBundleBroadcast()
{
    HILOG_INFO("HandleBundleBroadcast Begin");
    auto saMgrPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saMgrPtr == nullptr) {
        HILOG_ERROR("saMgrPtr is nullptr");
        return ERR_INVALID_VALUE;
    }
    std::vector<int64_t> extraDataIdList;
    int32_t ret = saMgrPtr->GetCommonEventExtraDataIdlist(SA_ID, extraDataIdList,
        EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    if (ret != ERR_OK) {
        HILOG_ERROR("GetCommonEventExtraDataIdList failed, ret = %{public}d", ret);
        return ret;
    }
    for (auto dataId : extraDataIdList) {
        MessageParcel dataParcel;
        int32_t getDataRet = saMgrPtr->GetOnDemandReasonExtraData(dataId, dataParcel);
        if (getDataRet != ERR_OK) {
            HILOG_ERROR("GetOnDemandReasonExtraData failed, ret = %{public}d", getDataRet);
            continue;
        }
        auto extraData = dataParcel.ReadParcelable<OnDemandReasonExtraData>();
        if (extraData == nullptr) {
            HILOG_ERROR("extraData is nullptr");
            continue;
        }
        map<string, string> want = extraData->GetWant();
        string userIdStr = GetValueByKey(want, "userId");
        string bundleName = GetValueByKey(want, "bundleName");
        string appIndexStr = GetValueByKey(want, "appIndex");
        int32_t appIndex = GetNumberFromString(appIndexStr, APP_DEFAULT_INDEX);
        int32_t userId = GetNumberFromString(userIdStr, USER_ID_DEFAULT);
        ClearSyncFolder(bundleName, userId, appIndex);
    }
    return ERR_OK;
}

int32_t BundleObserver::ClearSyncFolder(const string &bundleName, int32_t userId, int32_t appIndex)
{
    HILOG_INFO("Begin ClearSyncRoot, bundleName: %{public}s, userId: %{public}d, appIndex: %{public}d",
        bundleName.c_str(), userId, appIndex);
    // 0910 After the registration and encapsulation of the unified function are resolved, adjust the code logic.
    auto ptr = fileAccessSvc_.promote();
    if (ptr == nullptr) {
        HILOG_ERROR("fileAccessService is nullptr");
        return ERR_INVALID_VALUE;
    }
    int32_t ret = ptr->UnregisterAllByBundle(bundleName, userId, appIndex);
    if (ret != ERR_OK) {
        HILOG_ERROR("Clear syncFolder failed, ret:%{public}d, bundleName:%{public}s", ret, bundleName.c_str());
        return ret;
    }
    return ERR_OK;
}

void BundleObserver::OnReceiveEvent(const EventFwk::CommonEventData &data)
{
    HILOG_INFO("BundleObserver::OnReceiveEvent start");
    const OHOS::AAFwk::Want want = data.GetWant();
    string action = want.GetAction();
    if (action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED) {
        OHOS::AAFwk::WantParams params = want.GetParams();
        int32_t userId = params.GetIntParam("userId", USER_ID_DEFAULT);
        std::string bundleName = want.GetStringParam("bundleName");
        int32_t appIndex = params.GetIntParam("appIndex", APP_DEFAULT_INDEX);
        int32_t ret = ClearSyncFolder(bundleName, userId, appIndex);
        if (ret != 0) {
            HILOG_ERROR("BundleObserver::OnReceiveEvent ClearSyncRoot failed");
            return;
        }
    }
}

std::string BundleObserver::GetValueByKey(const std::map<string, string> &map, const string &key)
{
    auto it = map.find(key);
    if (it != map.end()) {
        return it->second;
    }
    return "";
}

int32_t BundleObserver::GetNumberFromString(const string &str, const int32_t defaultValue)
{
    if (str.empty()) {
        HILOG_ERROR("GetNumberFromString failed, str is empty");
        return defaultValue;
    }
    return static_cast<int32_t>(std::atoi(str.c_str()));
}
} // namespace FileAccessFwk
} // namespace OHOS