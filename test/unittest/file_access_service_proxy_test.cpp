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
#include "message_parcel_mock.h"

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

class AbilityConnectionMock : public OHOS::AAFwk::IAbilityConnection {
public:
    AbilityConnectionMock() = default;
    ~AbilityConnectionMock() = default;
public:
    void OnAbilityConnectDone(const AppExecFwk::ElementName&, const sptr<IRemoteObject>&, int) override {}
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName&, int) override {}
    MOCK_METHOD0(AsObject, sptr<IRemoteObject>());
};

class TestObject : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.FileAccessFwk.TestObject");
};

class RemoteObjectMock : public IRemoteStub<TestObject> {
public:
    RemoteObjectMock() = default;
    ~RemoteObjectMock() = default;
};

class FileAccessServiceProxyTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        Assistant::ins_ = insMoc;
        MessageParcelMock::messageParcel = msg;
        SystemAbilityManagerClient::GetInstance().systemAbilityManager_ = sptr<ISystemAbilityManager>(samgr.get());
    }
    static void TearDownTestCase()
    {
        insMoc = nullptr;
        msg = nullptr;
        samgr = nullptr;
        impl = nullptr;
        Assistant::ins_ = nullptr;
        MessageParcelMock::messageParcel = nullptr;
        SystemAbilityManagerClient::GetInstance().systemAbilityManager_ = nullptr;
    }
    void SetUp() {}
    void TearDown() {}
public:
    static inline shared_ptr<AssistantMock> insMoc = make_shared<AssistantMock>();
    static inline shared_ptr<MessageParcelMock> msg = make_shared<MessageParcelMock>();
    static inline sptr<FileAccessServiceMock> impl = sptr<FileAccessServiceMock>(new FileAccessServiceMock());
    static inline shared_ptr<ISystemAbilityManagerMock> samgr = make_shared<ISystemAbilityManagerMock>();
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
        EXPECT_TRUE(result == nullptr) << "GetInstance Faild";
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

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(false));
        auto result = proxy->OnChange(Uri(""), NotifyType::NOTIFY_ADD);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(false));
        result = proxy->OnChange(Uri(""), NotifyType::NOTIFY_ADD);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteInt32(_)).WillOnce(Return(false));
        result = proxy->OnChange(Uri(""), NotifyType::NOTIFY_ADD);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(E_URIS));
        result = proxy->OnChange(Uri(""), NotifyType::NOTIFY_ADD);
        EXPECT_EQ(result, E_URIS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*msg, ReadInt32(_)).WillOnce(Return(false));
        result = proxy->OnChange(Uri(""), NotifyType::NOTIFY_ADD);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*msg, ReadInt32(_)).WillOnce(Return(true));
        result = proxy->OnChange(Uri(""), NotifyType::NOTIFY_ADD);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteInt32(_)).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*msg, ReadInt32(_)).WillOnce(DoAll(SetArgReferee<0>(ERR_OK), Return(true)));
        result = proxy->OnChange(Uri(""), NotifyType::NOTIFY_ADD);
        EXPECT_EQ(result, ERR_OK);
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
        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(false));
        auto result = proxy->RegisterNotify(uri, notifyForDescendants, observer, info);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(false));
        result = proxy->RegisterNotify(uri, notifyForDescendants, observer, info);
        EXPECT_EQ(result, E_IPCS);

        sptr<IFileAccessObserver> obs = nullptr;
        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        result = proxy->RegisterNotify(uri, notifyForDescendants, obs, info);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*observer, AsObject()).WillOnce(Return(nullptr));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(false));
        result = proxy->RegisterNotify(uri, notifyForDescendants, observer, info);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*observer, AsObject()).WillOnce(Return(nullptr));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteBool(_)).WillOnce(Return(false));
        result = proxy->RegisterNotify(uri, notifyForDescendants, observer, info);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*observer, AsObject()).WillOnce(Return(nullptr));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteBool(_)).WillOnce(Return(true));
        result = proxy->RegisterNotify(uri, notifyForDescendants, observer, info);
        EXPECT_EQ(result, E_GETINFO);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessServiceProxyTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-end file_access_service_proxy_RegisterNotify_0000";
}

/**
 * @tc.number: user_file_service_file_access_service_proxy_RegisterNotify_0100
 * @tc.name: file_access_service_proxy_RegisterNotify_0100
 * @tc.desc: Test function of RegisterNotify interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8Y05B
 */
HWTEST_F(FileAccessServiceProxyTest, file_access_service_proxy_RegisterNotify_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-begin file_access_service_proxy_RegisterNotify_0100";
    try {
        shared_ptr<FileAccessServiceProxy> proxy = make_shared<FileAccessServiceProxy>(impl);

        Uri uri("");
        bool notifyForDescendants = false;
        shared_ptr<ConnectExtensionInfo> info = make_shared<ConnectExtensionInfo>();
        sptr<IFileAccessObserverMock> observer = sptr<IFileAccessObserverMock>(new IFileAccessObserverMock());
        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true)).WillOnce(Return(false));
        EXPECT_CALL(*observer, AsObject()).WillOnce(Return(nullptr));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteBool(_)).WillOnce(Return(true));
        auto result = proxy->RegisterNotify(uri, notifyForDescendants, observer, info);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*observer, AsObject()).WillOnce(Return(nullptr));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(true))
            .WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteBool(_)).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(E_URIS));
        result = proxy->RegisterNotify(uri, notifyForDescendants, observer, info);
        EXPECT_EQ(result, E_URIS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*observer, AsObject()).WillOnce(Return(nullptr));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(true))
            .WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteBool(_)).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*msg, ReadInt32(_)).WillOnce(Return(false));
        result = proxy->RegisterNotify(uri, notifyForDescendants, observer, info);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessServiceProxyTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-end file_access_service_proxy_RegisterNotify_0100";
}

/**
 * @tc.number: user_file_service_file_access_service_proxy_RegisterNotify_0200
 * @tc.name: file_access_service_proxy_RegisterNotify_0200
 * @tc.desc: Test function of RegisterNotify interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8Y05B
 */
HWTEST_F(FileAccessServiceProxyTest, file_access_service_proxy_RegisterNotify_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-begin file_access_service_proxy_RegisterNotify_0100";
    try {
        shared_ptr<FileAccessServiceProxy> proxy = make_shared<FileAccessServiceProxy>(impl);

        Uri uri("");
        bool notifyForDescendants = false;
        shared_ptr<ConnectExtensionInfo> info = make_shared<ConnectExtensionInfo>();
        sptr<IFileAccessObserverMock> observer = sptr<IFileAccessObserverMock>(new IFileAccessObserverMock());
        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*observer, AsObject()).WillOnce(Return(nullptr));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(true))
            .WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteBool(_)).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*msg, ReadInt32(_)).WillOnce(Return(true));
        auto result = proxy->RegisterNotify(uri, notifyForDescendants, observer, info);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*observer, AsObject()).WillOnce(Return(nullptr));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(true))
            .WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteBool(_)).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*msg, ReadInt32(_)).WillOnce(DoAll(SetArgReferee<0>(ERR_OK), Return(true)));
        result = proxy->RegisterNotify(uri, notifyForDescendants, observer, info);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessServiceProxyTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-end file_access_service_proxy_RegisterNotify_0200";
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
        EXPECT_CALL(*msg, ReadInt32(_)).WillOnce(Return(false));
        result = proxy->UnregisterNotifyInternal(data);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*msg, ReadInt32(_)).WillOnce(Return(true));
        result = proxy->UnregisterNotifyInternal(data);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*msg, ReadInt32(_)).WillOnce(DoAll(SetArgReferee<0>(ERR_OK), Return(true)));
        result = proxy->UnregisterNotifyInternal(data);
        EXPECT_EQ(result, ERR_OK);
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
        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(false));
        auto result = proxy->UnregisterNotify(uri, observer, info);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(false));
        result = proxy->UnregisterNotify(uri, observer, info);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteBool(_)).WillOnce(Return(false));
        result = proxy->UnregisterNotify(uri, observer, info);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteBool(_)).WillOnce(Return(true));
        result = proxy->UnregisterNotify(uri, observer, info);
        EXPECT_EQ(result, E_GETINFO);

        observer = sptr<IFileAccessObserverMock>(new IFileAccessObserverMock());
        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteBool(_)).WillOnce(Return(false));
        result = proxy->UnregisterNotify(uri, observer, info);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteBool(_)).WillOnce(Return(true));
        EXPECT_CALL(*observer, AsObject()).WillOnce(Return(nullptr));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(false));
        result = proxy->UnregisterNotify(uri, observer, info);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteBool(_)).WillOnce(Return(true));
        EXPECT_CALL(*observer, AsObject()).WillOnce(Return(nullptr));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(true));
        result = proxy->UnregisterNotify(uri, observer, info);
        EXPECT_EQ(result, E_GETINFO);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessServiceProxyTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-end file_access_service_proxy_UnregisterNotify_0000";
}

/**
 * @tc.number: user_file_service_file_access_service_proxy_ConnectFileExtAbility_0000
 * @tc.name: file_access_service_proxy_ConnectFileExtAbility_0000
 * @tc.desc: Test function of ConnectFileExtAbility interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8Y05B
 */
HWTEST_F(FileAccessServiceProxyTest, file_access_service_proxy_ConnectFileExtAbility_0000,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-begin file_access_service_proxy_ConnectFileExtAbility_0000";
    try {
        shared_ptr<FileAccessServiceProxy> proxy = make_shared<FileAccessServiceProxy>(impl);
        AAFwk::Want want;
        sptr<AbilityConnectionMock> connection = nullptr;

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(false));
        auto result = proxy->ConnectFileExtAbility(want, connection);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(false));
        result = proxy->ConnectFileExtAbility(want, connection);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        result = proxy->ConnectFileExtAbility(want, connection);
        EXPECT_EQ(result, E_GETINFO);

        connection = sptr<AbilityConnectionMock>(new AbilityConnectionMock());
        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*connection, AsObject()).WillOnce(Return(nullptr));
        result = proxy->ConnectFileExtAbility(want, connection);
        EXPECT_EQ(result, E_GETINFO);

        auto obj = sptr(new (std::nothrow) RemoteObjectMock());
        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*connection, AsObject()).WillOnce(Return(obj)).WillOnce(Return(obj));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(false));
        result = proxy->ConnectFileExtAbility(want, connection);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessServiceProxyTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-end file_access_service_proxy_ConnectFileExtAbility_0000";
}

/**
 * @tc.number: user_file_service_file_access_service_proxy_ConnectFileExtAbility_0100
 * @tc.name: file_access_service_proxy_ConnectFileExtAbility_0100
 * @tc.desc: Test function of ConnectFileExtAbility interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8Y05B
 */
HWTEST_F(FileAccessServiceProxyTest, file_access_service_proxy_ConnectFileExtAbility_0100,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-begin file_access_service_proxy_ConnectFileExtAbility_0000";
    try {
        shared_ptr<FileAccessServiceProxy> proxy = make_shared<FileAccessServiceProxy>(impl);
        AAFwk::Want want;
        sptr<AbilityConnectionMock> connection = sptr<AbilityConnectionMock>(new AbilityConnectionMock());
        auto obj = sptr(new (std::nothrow) RemoteObjectMock());

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*connection, AsObject()).WillOnce(Return(obj)).WillOnce(Return(nullptr));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(E_IPCS));
        auto result = proxy->ConnectFileExtAbility(want, connection);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*connection, AsObject()).WillOnce(Return(obj)).WillOnce(Return(nullptr));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*msg, ReadInt32(_)).WillOnce(DoAll(Return(false)));
        result = proxy->ConnectFileExtAbility(want, connection);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*connection, AsObject()).WillOnce(Return(obj)).WillOnce(Return(nullptr));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*msg, ReadInt32(_)).WillOnce(DoAll(SetArgReferee<0>(E_IPCS), Return(true)));
        result = proxy->ConnectFileExtAbility(want, connection);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*connection, AsObject()).WillOnce(Return(obj)).WillOnce(Return(nullptr));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*msg, ReadInt32(_)).WillOnce(DoAll(SetArgReferee<0>(ERR_OK), Return(true)));
        result = proxy->ConnectFileExtAbility(want, connection);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessServiceProxyTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-end file_access_service_proxy_ConnectFileExtAbility_0100";
}

/**
 * @tc.number: user_file_service_file_access_service_proxy_DisConnectFileExtAbility_0000
 * @tc.name: file_access_service_proxy_DisConnectFileExtAbility_0000
 * @tc.desc: Test function of DisConnectFileExtAbility interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8Y05B
 */
HWTEST_F(FileAccessServiceProxyTest, file_access_service_proxy_DisConnectFileExtAbility_0000,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-begin file_access_service_proxy_DisConnectFileExtAbility_0000";
    try {
        shared_ptr<FileAccessServiceProxy> proxy = make_shared<FileAccessServiceProxy>(impl);
        sptr<AbilityConnectionMock> connection = nullptr;

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(false));
        auto result = proxy->DisConnectFileExtAbility(connection);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        result = proxy->DisConnectFileExtAbility(connection);
        EXPECT_EQ(result, E_GETINFO);

        connection = sptr<AbilityConnectionMock>(new AbilityConnectionMock());
        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*connection, AsObject()).WillOnce(Return(nullptr));
        result = proxy->DisConnectFileExtAbility(connection);
        EXPECT_EQ(result, E_GETINFO);

        auto obj = sptr(new (std::nothrow) RemoteObjectMock());
        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*connection, AsObject()).WillOnce(Return(obj)).WillOnce(Return(nullptr));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(false));
        result = proxy->DisConnectFileExtAbility(connection);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*connection, AsObject()).WillOnce(Return(obj)).WillOnce(Return(nullptr));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(E_IPCS));
        result = proxy->DisConnectFileExtAbility(connection);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessServiceProxyTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-end file_access_service_proxy_DisConnectFileExtAbility_0000";
}

/**
 * @tc.number: user_file_service_file_access_service_proxy_DisConnectFileExtAbility_0100
 * @tc.name: file_access_service_proxy_DisConnectFileExtAbility_0100
 * @tc.desc: Test function of DisConnectFileExtAbility interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8Y05B
 */
HWTEST_F(FileAccessServiceProxyTest, file_access_service_proxy_DisConnectFileExtAbility_0100,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-begin file_access_service_proxy_DisConnectFileExtAbility_0100";
    try {
        shared_ptr<FileAccessServiceProxy> proxy = make_shared<FileAccessServiceProxy>(impl);
        sptr<AbilityConnectionMock> connection = sptr<AbilityConnectionMock>(new AbilityConnectionMock());
        auto obj = sptr(new (std::nothrow) RemoteObjectMock());

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*connection, AsObject()).WillOnce(Return(obj)).WillOnce(Return(nullptr));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*msg, ReadInt32(_)).WillOnce(DoAll(Return(false)));
        auto result = proxy->DisConnectFileExtAbility(connection);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*connection, AsObject()).WillOnce(Return(obj)).WillOnce(Return(nullptr));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*msg, ReadInt32(_)).WillOnce(DoAll(SetArgReferee<0>(E_IPCS), Return(true)));
        result = proxy->DisConnectFileExtAbility(connection);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*connection, AsObject()).WillOnce(Return(obj)).WillOnce(Return(nullptr));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*msg, ReadInt32(_)).WillOnce(DoAll(SetArgReferee<0>(ERR_OK), Return(true)));
        result = proxy->DisConnectFileExtAbility(connection);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessServiceProxyTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-end file_access_service_proxy_DisConnectFileExtAbility_0100";
}

/**
 * @tc.number: user_file_service_file_access_service_proxy_UnregisterNotify_0100
 * @tc.name: file_access_service_proxy_UnregisterNotify_0100
 * @tc.desc: Test function of UnregisterNotify interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8Y05B
 */
HWTEST_F(FileAccessServiceProxyTest, file_access_service_proxy_UnregisterNotify_0100,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-begin file_access_service_proxy_UnregisterNotify_0100";
    try {
        Uri uri("");
        sptr<IFileAccessObserverMock> observer = nullptr;
        shared_ptr<FileAccessServiceProxy> proxy = make_shared<FileAccessServiceProxy>(impl);
        shared_ptr<ConnectExtensionInfo> info = make_shared<ConnectExtensionInfo>();

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true)).WillOnce(Return(false));
        EXPECT_CALL(*msg, WriteBool(_)).WillOnce(Return(true));
        auto result = proxy->UnregisterNotify(uri, observer, info);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteBool(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(E_IPCS));
        result = proxy->UnregisterNotify(uri, observer, info);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessServiceProxyTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-end file_access_service_proxy_UnregisterNotify_0100";
}

/**
 * @tc.number: user_file_service_file_access_service_proxy_GetExensionProxy_0000
 * @tc.name: file_access_service_proxy_GetExensionProxy_0000
 * @tc.desc: Test function of GetExtensionProxy interface for ERROR.
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

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(false));
        auto result = proxy->GetExtensionProxy(info, extensionProxy);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        result = proxy->GetExtensionProxy(info, extensionProxy);
        EXPECT_EQ(result, E_GETINFO);

        info = make_shared<ConnectExtensionInfo>();
        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(false));
        result = proxy->GetExtensionProxy(info, extensionProxy);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(E_URIS));
        result = proxy->GetExtensionProxy(info, extensionProxy);
        EXPECT_EQ(result, E_URIS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*msg, ReadInt32(_)).WillOnce(Return(false));
        result = proxy->GetExtensionProxy(info, extensionProxy);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*msg, ReadInt32(_)).WillOnce(Return(true));
        result = proxy->GetExtensionProxy(info, extensionProxy);
        EXPECT_EQ(result, E_IPCS);

        EXPECT_CALL(*msg, WriteInterfaceToken(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteParcelable(_)).WillOnce(Return(true));
        EXPECT_CALL(*msg, WriteRemoteObject(An<const sptr<IRemoteObject>&>())).WillOnce(Return(true));
        EXPECT_CALL(*impl, SendRequest(_, _, _, _)).WillOnce(Return(ERR_OK));
        EXPECT_CALL(*msg, ReadInt32(_)).WillOnce(DoAll(SetArgReferee<0>(ERR_OK), Return(true)));
        EXPECT_CALL(*msg, ReadRemoteObject()).WillOnce(Return(nullptr));
        result = proxy->GetExtensionProxy(info, extensionProxy);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        GTEST_LOG_(ERROR) << "FileAccessServiceProxyTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileAccessServiceProxyTest-end file_access_service_proxy_GetExensionProxy_0000";
}
}