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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include "assistant.h"
#include "file_access_extension_info.h"
#include "file_access_framework_errno.h"
#include "file_access_service_proxy.h"
#include "file_access_service_mock.h"
#include "if_system_ability_manager_mock.h"
#include "iservice_registry.h"

int32_t OHOS::SystemAbilityLoadCallbackStub::OnRemoteRequest(unsigned int, OHOS::MessageParcel&, OHOS::MessageParcel&,
    OHOS::MessageOption&)
{
    return 0;
}

namespace OHOS::FileAccessFwk {
using namespace std;
using namespace testing;
using namespace testing::ext;

FileAccessServiceStub::FileAccessServiceStub() {}

FileAccessServiceStub::~FileAccessServiceStub() {}

int32_t FileAccessServiceStub::OnRemoteRequest(unsigned int, OHOS::MessageParcel&, OHOS::MessageParcel&,
    OHOS::MessageOption&)
{
    return 0;
}

class IFileAccessObserverMock : public IFileAccessObserver {
public:
    MOCK_METHOD1(OnChange, void(NotifyMessage &notifyMessage));
    MOCK_METHOD0(AsObject, sptr<IRemoteObject>());
};

class FileAccessServiceProxyTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        Assistant::ins_ = insMoc;
        SystemAbilityManagerClient::GetInstance().systemAbilityManager_ = sptr<ISystemAbilityManager>(samgr);
    }
    static void TearDownTestCase()
    {
        insMoc = nullptr;
        samgr = nullptr;
        impl = nullptr;
        Assistant::ins_ = nullptr;
        SystemAbilityManagerClient::GetInstance().systemAbilityManager_ = nullptr;
    }
    void SetUp() {}
    void TearDown() {}
public:
    static inline shared_ptr<AssistantMock> insMoc = make_shared<AssistantMock>();
    static inline sptr<FileAccessServiceMock> impl = sptr<FileAccessServiceMock>(new FileAccessServiceMock());
    static inline ISystemAbilityManagerMock* samgr = new ISystemAbilityManagerMock();
};

/**
 * @tc.number: user_file_service_file_access_service_proxy_GetInstance_0001
 * @tc.name: file_access_service_proxy_GetInstance_0001
 * @tc.desc: Test function of GetInstance interface for ERROR because samgr is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8Y05B
 */
HWTEST_F(FileAccessServiceProxyTest, file_access_service_proxy_GetInstance_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-begin file_access_service_proxy_GetInstance_0001";
    try {
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(false));

        shared_ptr<FileAccessServiceProxy> proxy = make_shared<FileAccessServiceProxy>(nullptr);
        proxy->serviceProxy_ = nullptr;
        auto result = proxy->GetInstance();
        EXPECT_TRUE(result == nullptr);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessServiceProxyTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-end file_access_service_proxy_GetInstance_0001";
}

/**
 * @tc.number: user_file_service_file_access_service_proxy_GetInstance_0002
 * @tc.name: file_access_service_proxy_GetInstance_0002
 * @tc.desc: Test function of GetInstance interface for ERROR because samgr->LoadSystemAbility doesn't return ERR_OK.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8Y05B
 */
HWTEST_F(FileAccessServiceProxyTest, file_access_service_proxy_GetInstance_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-begin file_access_service_proxy_GetInstance_0002";
    try {
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true));
        EXPECT_CALL(*samgr, LoadSystemAbility(An<int32_t>(), An<const sptr<ISystemAbilityLoadCallback>&>()))
            .WillOnce(Return(E_PERMISSION));

        shared_ptr<FileAccessServiceProxy> proxy = make_shared<FileAccessServiceProxy>(nullptr);
        proxy->serviceProxy_ = nullptr;
        auto result = proxy->GetInstance();
        EXPECT_TRUE(result == nullptr);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessServiceProxyTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-end file_access_service_proxy_GetInstance_0002";
}

/**
 * @tc.number: user_file_service_file_access_service_proxy_GetInstance_0003
 * @tc.name: file_access_service_proxy_GetInstance_0003
 * @tc.desc: Test function of GetInstance interface for ERROR because wait_for timeout.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8Y05B
 */
HWTEST_F(FileAccessServiceProxyTest, file_access_service_proxy_GetInstance_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-begin file_access_service_proxy_GetInstance_0003";
    try {
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true));
        EXPECT_CALL(*samgr, LoadSystemAbility(An<int32_t>(), An<const sptr<ISystemAbilityLoadCallback>&>()))
            .WillOnce(Return(ERR_OK));

        shared_ptr<FileAccessServiceProxy> proxy = make_shared<FileAccessServiceProxy>(nullptr);
        proxy->serviceProxy_ = nullptr;
        auto result = proxy->GetInstance();
        EXPECT_TRUE(result == nullptr);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessServiceProxyTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-end file_access_service_proxy_GetInstance_0003";
}

/**
 * @tc.number: user_file_service_file_access_service_proxy_OnChange_0000
 * @tc.name: file_access_service_proxy_OnChange_0000
 * @tc.desc: Test function of OnChange interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8Y05B
 */
HWTEST_F(FileAccessServiceProxyTest, file_access_service_proxy_OnChange_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-begin file_access_service_proxy_OnChange_0000";
    try {
        shared_ptr<FileAccessServiceProxy> proxy = make_shared<FileAccessServiceProxy>(impl);

        Uri uri("");
        NotifyType notifyType = NotifyType::NOTIFY_ADD;
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(false));
        auto result = proxy->OnChange(uri, notifyType);
        EXPECT_EQ(result, E_IPCS);
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true)).WillOnce(Return(false));
        result = proxy->OnChange(uri, notifyType);
        EXPECT_EQ(result, E_IPCS);
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(false));
        result = proxy->OnChange(uri, notifyType);
        EXPECT_EQ(result, E_IPCS);
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(E_URIS));
        result = proxy->OnChange(uri, notifyType);
        EXPECT_EQ(result, E_URIS);
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true))
            .WillOnce(Return(false));
        EXPECT_CALL(*insMoc, Int()).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        result = proxy->OnChange(uri, notifyType);
        EXPECT_EQ(result, ERR_OK);
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true))
            .WillOnce(Return(true));
        EXPECT_CALL(*insMoc, Int()).WillOnce(Return(E_IPCS));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        result = proxy->OnChange(uri, notifyType);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessServiceProxyTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-end file_access_service_proxy_OnChange_0000";
}

/**
 * @tc.number: user_file_service_file_access_service_proxy_RegisterNotify_0000
 * @tc.name: file_access_service_proxy_RegisterNotify_0000
 * @tc.desc: Test function of RegisterNotify interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8Y05B
 */
HWTEST_F(FileAccessServiceProxyTest, file_access_service_proxy_RegisterNotify_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-begin file_access_service_proxy_RegisterNotify_0000";
    try {
        shared_ptr<FileAccessServiceProxy> proxy = make_shared<FileAccessServiceProxy>(impl);

        Uri uri("");
        bool notifyForDescendants = false;
        shared_ptr<ConnectExtensionInfo> info = nullptr;
        sptr<IFileAccessObserverMock> observer = sptr<IFileAccessObserverMock>(new IFileAccessObserverMock());
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(false));
        auto result = proxy->RegisterNotify(uri, notifyForDescendants, observer, info);
        EXPECT_EQ(result, E_IPCS);
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true)).WillOnce(Return(false));
        result = proxy->RegisterNotify(uri, notifyForDescendants, observer, info);
        EXPECT_EQ(result, E_IPCS);
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(false));
        EXPECT_CALL(*observer, AsObject()).WillOnce(Return(nullptr));
        result = proxy->RegisterNotify(uri, notifyForDescendants, observer, info);
        EXPECT_EQ(result, E_IPCS);
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true))
            .WillOnce(Return(false));
        EXPECT_CALL(*observer, AsObject()).WillOnce(Return(nullptr));
        result = proxy->RegisterNotify(uri, notifyForDescendants, observer, info);
        EXPECT_EQ(result, E_IPCS);
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true))
            .WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*observer, AsObject()).WillOnce(Return(nullptr));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(E_URIS));
        shared_ptr<ConnectExtensionInfo> info2 = make_shared<ConnectExtensionInfo>();
        result = proxy->RegisterNotify(uri, notifyForDescendants, observer, info2);
        EXPECT_EQ(result, E_URIS);
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true))
            .WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*insMoc, Int()).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*observer, AsObject()).WillOnce(Return(nullptr));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        result = proxy->RegisterNotify(uri, notifyForDescendants, observer, info2);
        EXPECT_EQ(result, ERR_OK);
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true))
            .WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*insMoc, Int()).WillOnce(Return(E_IPCS));
        EXPECT_CALL(*observer, AsObject()).WillOnce(Return(nullptr));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        result = proxy->RegisterNotify(uri, notifyForDescendants, observer, info2);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessServiceProxyTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-end file_access_service_proxy_RegisterNotify_0000";
}

/**
 * @tc.number: user_file_service_file_access_service_proxy_UnregisterNotifyInternal_0000
 * @tc.name: file_access_service_proxy_UnregisterNotifyInternal_0000
 * @tc.desc: Test function of UnregisterNotifyInternal interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8Y05B
 */
HWTEST_F(FileAccessServiceProxyTest, file_access_service_proxy_UnregisterNotifyInternal_0000,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-begin file_access_service_proxy_UnregisterNotifyInternal_0000";
    try {
        shared_ptr<FileAccessServiceProxy> proxy = make_shared<FileAccessServiceProxy>(impl);
        MessageParcel data;

        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(E_URIS));
        auto result = proxy->UnregisterNotifyInternal(data);
        EXPECT_EQ(result, E_URIS);
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(false));
        EXPECT_CALL(*insMoc, Int()).WillOnce(Return(ERR_OK));
        result = proxy->UnregisterNotifyInternal(data);
        EXPECT_EQ(result, ERR_OK);
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true));
        EXPECT_CALL(*insMoc, Int()).WillOnce(Return(E_IPCS));
        result = proxy->UnregisterNotifyInternal(data);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessServiceProxyTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-end file_access_service_proxy_UnregisterNotifyInternal_0000";
}

/**
 * @tc.number: user_file_service_file_access_service_proxy_UnregisterNotify_0000
 * @tc.name: file_access_service_proxy_UnregisterNotify_0000
 * @tc.desc: Test function of UnregisterNotify interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8Y05B
 */
HWTEST_F(FileAccessServiceProxyTest, file_access_service_proxy_UnregisterNotify_0000,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-begin file_access_service_proxy_UnregisterNotify_0000";
    try {
        shared_ptr<FileAccessServiceProxy> proxy = make_shared<FileAccessServiceProxy>(impl);
        Uri uri("");
        sptr<IFileAccessObserverMock> observer = nullptr;
        shared_ptr<ConnectExtensionInfo> info = nullptr;
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(false));
        auto result = proxy->UnregisterNotify(uri, observer, info);
        EXPECT_EQ(result, E_IPCS);
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true)).WillOnce(Return(false));
        result = proxy->UnregisterNotify(uri, observer, info);
        EXPECT_EQ(result, E_IPCS);
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(false));
        result = proxy->UnregisterNotify(uri, observer, info);
        EXPECT_EQ(result, E_IPCS);
        observer = sptr<IFileAccessObserverMock>(new IFileAccessObserverMock());
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(false));
        result = proxy->UnregisterNotify(uri, observer, info);
        EXPECT_EQ(result, E_IPCS);
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true))
            .WillOnce(Return(false));
        EXPECT_CALL(*observer, AsObject()).WillOnce(Return(nullptr));
        result = proxy->UnregisterNotify(uri, observer, info);
        EXPECT_EQ(result, E_IPCS);
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true))
            .WillOnce(Return(false));
        EXPECT_CALL(*observer, AsObject()).WillOnce(Return(nullptr));
        result = proxy->UnregisterNotify(uri, observer, info);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessServiceProxyTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-end file_access_service_proxy_UnregisterNotify_0000";
}

/**
 * @tc.number: user_file_service_file_access_service_proxy_GetExensionProxy_0000
 * @tc.name: file_access_service_proxy_GetExensionProxy_0000
 * @tc.desc: Test function of GetExensionProxy interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8Y05B
 */
HWTEST_F(FileAccessServiceProxyTest, file_access_service_proxy_GetExensionProxy_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-begin file_access_service_proxy_GetExensionProxy_0000";
    try {
        shared_ptr<FileAccessServiceProxy> proxy = make_shared<FileAccessServiceProxy>(impl);

        shared_ptr<ConnectExtensionInfo> info = nullptr;
        sptr<IFileAccessExtBase> extensionProxy = nullptr;
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(false));
        auto result = proxy->GetExensionProxy(info, extensionProxy);
        EXPECT_EQ(result, E_IPCS);
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true));
        result = proxy->GetExensionProxy(info, extensionProxy);
        EXPECT_EQ(result, E_GETINFO);
        info = make_shared<ConnectExtensionInfo>();
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true)).WillOnce(Return(false));
        result = proxy->GetExensionProxy(info, extensionProxy);
        EXPECT_EQ(result, E_IPCS);
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(E_URIS));
        result = proxy->GetExensionProxy(info, extensionProxy);
        EXPECT_EQ(result, E_URIS);
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true))
            .WillOnce(Return(false));
        EXPECT_CALL(*insMoc, Int()).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        result = proxy->GetExensionProxy(info, extensionProxy);
        EXPECT_EQ(result, ERR_OK);
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true))
            .WillOnce(Return(true));
        EXPECT_CALL(*insMoc, Int()).WillOnce(Return(E_IPCS));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        result = proxy->GetExensionProxy(info, extensionProxy);
        EXPECT_EQ(result, E_IPCS);
        EXPECT_CALL(*insMoc, Bool()).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true))
            .WillOnce(Return(true));
        EXPECT_CALL(*insMoc, Int()).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        result = proxy->GetExensionProxy(info, extensionProxy);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessServiceProxyTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-end file_access_service_proxy_GetExensionProxy_0000";
}
}