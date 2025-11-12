/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
 
#include "assistant.h"
#include "extension_context.h"
#include "file_access_ext_base_stub.h"
#include "file_access_service_mock.h"
#include "file_access_service_client.h"
#include "js_file_access_ext_ability.h"
#include "js_native_api_types.h"
#include "native_reference_mock.h"
 
namespace OHOS::FileAccessFwk {
using namespace std;
using namespace testing;
using namespace testing::ext;
const int ARG_INDEX_SECOND = 2;
const int ARG_INDEX_THIRD = 3;
 
int32_t FileAccessServiceBaseStub::OnRemoteRequest(unsigned int, OHOS::MessageParcel&, OHOS::MessageParcel&,
    OHOS::MessageOption&)
{
    return 0;
}
 
int32_t FileAccessServiceBaseProxy::RegisterNotify(const Uri &, bool, const sptr<IFileAccessObserver> &,
    const ConnectExtensionInfo &)
{
    return 0;
}
 
int32_t FileAccessServiceBaseProxy::OnChange(const Uri &uri, NotifyType notifyType)
{
    return ERR_OK;
}
 
int32_t FileAccessServiceBaseProxy::GetExtensionProxy(const ConnectExtensionInfo &,
    sptr<IFileAccessExtBase> &)
{
    return 0;
}
 
int32_t FileAccessServiceBaseProxy::UnregisterNotify(const Uri &, const sptr<IFileAccessObserver> &,
    const ConnectExtensionInfo &)
{
    return 0;
}
 
int32_t FileAccessServiceBaseProxy::UnregisterNotifyNoObserver(const Uri &, const ConnectExtensionInfo &)
{
    return 0;
}
 
int32_t FileAccessServiceBaseProxy::ConnectFileExtAbility(const AAFwk::Want &,
    const sptr<AAFwk::IAbilityConnection>&)
{
    return 0;
}
 
int32_t FileAccessServiceBaseProxy::DisConnectFileExtAbility(const sptr<AAFwk::IAbilityConnection>&)
{
    return 0;
}
 
int32_t FileAccessServiceBaseProxy::Register(const SyncFolder &syncFolder)
{
    return ERR_OK;
}
 
int32_t FileAccessServiceBaseProxy::Unregister(const std::string &path)
{
    return ERR_OK;
}
 
int32_t FileAccessServiceBaseProxy::Active(const std::string &path)
{
    return ERR_OK;
}
 
int32_t FileAccessServiceBaseProxy::Deactive(const std::string &path)
{
    return ERR_OK;
}
 
int32_t FileAccessServiceBaseProxy::GetSyncFolders(std::vector<SyncFolder> &syncFolders)
{
    return ERR_OK;
}
 
int32_t FileAccessServiceBaseProxy::GetAllSyncFolders(std::vector<SyncFolderExt> &syncFolderExts)
{
    return ERR_OK;
}
 
int32_t FileAccessServiceBaseProxy::UpdateDisplayName(const std::string &path, const std::string &displayName)
{
    return ERR_OK;
}
 
int32_t FileAccessServiceBaseProxy::UnregisterForSa(const std::string &path)
{
    return ERR_OK;
}
 
int32_t FileAccessServiceBaseProxy::GetAllSyncFoldersForSa(std::vector<SyncFolderExt> &syncFolderExts)
{
    return ERR_OK;
}
 
void FileAccessExtAbility::Init(const std::shared_ptr<AbilityLocalRecord> &record,
    const std::shared_ptr<OHOSApplication> &application,
    std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    ExtensionBase<>::Init(record, application, handler, token);
}
 
int32_t FileAccessExtBaseStub::OnRemoteRequest(unsigned int, OHOS::MessageParcel&, OHOS::MessageParcel&,
    OHOS::MessageOption&)
{
    return 0;
}
 
class JsFileAccessExtAbilityLambdaTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        Assistant::ins_ = insMoc;
    }
    static void TearDownTestCase()
    {
        impl = nullptr;
        insMoc = nullptr;
        ability = nullptr;
        jsRuntime = nullptr;
        Assistant::ins_ = nullptr;
    }
    void SetUp() {}
    void TearDown() {}
public:
    static inline shared_ptr<AssistantMock> insMoc = make_shared<AssistantMock>();
    static inline unique_ptr<JsRuntime> jsRuntime = make_unique<JsRuntime>();
    static inline shared_ptr<JsFileAccessExtAbility> ability = make_shared<JsFileAccessExtAbility>(*jsRuntime);
    static inline sptr<FileAccessServiceMock> impl = sptr<FileAccessServiceMock>(new FileAccessServiceMock());
    static inline napi_env env = reinterpret_cast<napi_env>(&(jsRuntime->GetNativeEngine()));
};
 
sptr<IFileAccessServiceBase> FileAccessServiceClient::GetInstance()
{
    return iface_cast<IFileAccessServiceBase>(JsFileAccessExtAbilityLambdaTest::impl);
}
 
int JsFileAccessExtAbility::CallJsMethod(const std::string &funcName, JsRuntime &jsRuntime, NativeReference *jsObj,
    InputArgsParser argParser, ResultValueParser retParser)
{
    HILOG_ERROR("IN test CallJsMethod");
    auto &nativeEngine = jsRuntime.GetNativeEngine();
    auto env = reinterpret_cast<napi_env>(&nativeEngine);
    size_t argc = 0;
    napi_value argv[5] = { nullptr };
    HandleEscape handleEscape(jsRuntime);
    napi_value result = nullptr;
    if (argParser) {
        auto ret = argParser(env, argv, argc);
        HILOG_ERROR("argParser ret %{public}d", ret);
    }
    if (retParser) {
        auto ret = retParser(env, handleEscape.Escape(result));
        HILOG_ERROR("retParser ret %{public}d", ret);
    }
    return ERR_OK;
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_ability_OpenFile_0003
 * @tc.name: js_file_access_ext_ability_OpenFile_0003
 * @tc.desc: Test function of OpenFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_OpenFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_OpenFile_0003";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        const Uri uri("");
        int fd = -1;
        ability->jsObj_ = make_shared<NativeReferenceMock>();
 
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        auto result = ability->OpenFile(uri, 0, fd);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_OpenFile_0003";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_OpenFile_0004
 * @tc.name: js_file_access_ext_ability_OpenFile_0004
 * @tc.desc: Test function of OpenFile interface for SUCCESS with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_OpenFile_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_OpenFile_0004";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        const Uri uri("");
        int fd = -1;
        ability->jsObj_ = make_shared<NativeReferenceMock>();
 
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        auto result = ability->OpenFile(uri, 0, fd);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_OpenFile_0004";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_OpenFile_0005
 * @tc.name: js_file_access_ext_ability_OpenFile_0005
 * @tc.desc: Test function of OpenFile interface for SUCCESS with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_OpenFile_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_OpenFile_0005";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        const Uri uri("");
        int fd = -1;
        ability->jsObj_ = make_shared<NativeReferenceMock>();
 
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int32(_, _, _))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        auto result = ability->OpenFile(uri, 0, fd);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_OpenFile_0005";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_OpenFile_0006
 * @tc.name: js_file_access_ext_ability_OpenFile_0006
 * @tc.desc: Test function of OpenFile interface for SUCCESS with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_OpenFile_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_OpenFile_0006";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        const Uri uri("");
        int fd = -1;
        ability->jsObj_ = make_shared<NativeReferenceMock>();
 
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_int32(_, _, _))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        auto result = ability->OpenFile(uri, 0, fd);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_OpenFile_0006";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_OpenFile_0007
 * @tc.name: js_file_access_ext_ability_OpenFile_0007
 * @tc.desc: Test function of OpenFile interface when napi_get_value_int32 returns invalid argument.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_OpenFile_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_OpenFile_0007";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        const Uri uri("");
        int fd = -1;
        ability->jsObj_ = make_shared<NativeReferenceMock>();
 
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->OpenFile(uri, 0, fd);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_OpenFile_0007";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_OpenFile_0008
 * @tc.name: js_file_access_ext_ability_OpenFile_0008
 * @tc.desc: Test function of OpenFile interface with custom return code.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_OpenFile_0008, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_OpenFile_0008";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        const Uri uri("");
        int fd = -1;
        ability->jsObj_ = make_shared<NativeReferenceMock>();
        int retcode = 5;

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<2>(retcode), Return(napi_ok)));
        auto result = ability->OpenFile(uri, 0, fd);
        EXPECT_EQ(result, retcode);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_OpenFile_0008";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CreateFile_0004
 * @tc.name: js_file_access_ext_ability_CreateFile_0004
 * @tc.desc: Test function of CreateFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_CreateFile_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_CreateFile_0004";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        string path = "test";
        Uri parent("");
        string displayName("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();
 
        // 模拟CreateFile调用成功
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->CreateFile(parent, displayName, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_CreateFile_0004";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CreateFile_0005
 * @tc.name: js_file_access_ext_ability_CreateFile_0005
 * @tc.desc: Test function of CreateFile interface for SUCCESS with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_CreateFile_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_CreateFile_0005";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        string path = "test";
        Uri parent("");
        string displayName("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->CreateFile(parent, displayName, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_CreateFile_0005";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CreateFile_0006
 * @tc.name: js_file_access_ext_ability_CreateFile_0006
 * @tc.desc: Test function of CreateFile interface for SUCCESS with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_CreateFile_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_CreateFile_0006";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        string path = "test";
        Uri parent("");
        string displayName("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->CreateFile(parent, displayName, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_CreateFile_0006";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CreateFile_0007
 * @tc.name: js_file_access_ext_ability_CreateFile_0007
 * @tc.desc: Test function of CreateFile interface for SUCCESS with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_CreateFile_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_CreateFile_0007";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        string path = "test";
        Uri parent("");
        string displayName("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->CreateFile(parent, displayName, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_CreateFile_0007";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CreateFile_0008
 * @tc.name: js_file_access_ext_ability_CreateFile_0008
 * @tc.desc: Test function of CreateFile interface when napi_get_value_int32 returns invalid argument.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_CreateFile_0008, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_CreateFile_0008";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        string path = "test";
        Uri parent("");
        string displayName("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->CreateFile(parent, displayName, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_CreateFile_0008";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Mkdir_0004
 * @tc.name: js_file_access_ext_ability_Mkdir_0004
 * @tc.desc: Test function of Mkdir interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_Mkdir_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_Mkdir_0004";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        string path = "test";
        Uri parent("");
        string displayName("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();
 
 
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->Mkdir(parent, displayName, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_Mkdir_0004";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Mkdir_0005
 * @tc.name: js_file_access_ext_ability_Mkdir_0005
 * @tc.desc: Test function of Mkdir interface for SUCCESS with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_Mkdir_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_Mkdir_0005";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        string path = "test";
        Uri parent("");
        string displayName("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->Mkdir(parent, displayName, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_Mkdir_0005";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Mkdir_0006
 * @tc.name: js_file_access_ext_ability_Mkdir_0006
 * @tc.desc: Test function of Mkdir interface for SUCCESS with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_Mkdir_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_Mkdir_0006";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        string path = "test";
        Uri parent("");
        string displayName("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->Mkdir(parent, displayName, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_Mkdir_0006";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Mkdir_0007
 * @tc.name: js_file_access_ext_ability_Mkdir_0007
 * @tc.desc: Test function of Mkdir interface for SUCCESS with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_Mkdir_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_Mkdir_0007";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        string path = "test";
        Uri parent("");
        string displayName("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->Mkdir(parent, displayName, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_Mkdir_0007";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Mkdir_0008
 * @tc.name: js_file_access_ext_ability_Mkdir_0008
 * @tc.desc: Test function of Mkdir interface when napi_get_value_int32 returns invalid argument.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_Mkdir_0008, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_Mkdir_0008";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        string path = "test";
        Uri parent("");
        string displayName("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->Mkdir(parent, displayName, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_Mkdir_0008";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Delete_0002
 * @tc.name: js_file_access_ext_ability_Delete_0002
 * @tc.desc: Test function of Delete interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_Delete_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_Delete_0002";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();
 
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(ERR_OK), Return(napi_invalid_arg)));
        auto result = ability->Delete(sourceFile);
        EXPECT_EQ(result, ERR_OK);
 
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(ERR_OK), Return(napi_ok)));
        result = ability->Delete(sourceFile);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_Delete_0002";
}
 
 
/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Move_0003
 * @tc.name: js_file_access_ext_ability_Move_0003
 * @tc.desc: Test function of Move interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_Move_0004_1, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_Move_0004_1";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        string path = "test";
        Uri sourceFile("");
        Uri targetParent("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->Move(sourceFile, targetParent, newFile);
        EXPECT_EQ(result, E_GETRESULT);

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->Move(sourceFile, targetParent, newFile);
        EXPECT_EQ(result, E_GETRESULT);

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->Move(sourceFile, targetParent, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_Move_0004_1";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Move_0004_2
 * @tc.name: js_file_access_ext_ability_Move_0004_2
 * @tc.desc: Test function of Move interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_Move_0004_2, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_Move_0004_2";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        string path = "test";
        Uri sourceFile("");
        Uri targetParent("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->Move(sourceFile, targetParent, newFile);
        EXPECT_EQ(result, E_GETRESULT);

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_invalid_arg));
        result = ability->Move(sourceFile, targetParent, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_Move_0004_2";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Copy_0008
 * @tc.name: js_file_access_ext_ability_Copy_0007
 * @tc.desc: Test function of Copy interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_Copy_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_Copy_0007";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        vector<Result> copyResult;
        bool force = false;
        int err = -1;
        ability->jsObj_ = make_shared<NativeReferenceMock>();
 
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        auto result = ability->Copy(sourceUri, destUri, copyResult, force);
        EXPECT_EQ(result, err);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_Copy_0007";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Copy_0009
 * @tc.name: js_file_access_ext_ability_Copy_0009
 * @tc.desc: Test function of Copy interface for ERROR with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_Copy_0009, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_Copy_0009";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        vector<Result> copyResult;
        bool force = false;
        int err = -1;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        auto result = ability->Copy(sourceUri, destUri, copyResult, force);
        EXPECT_EQ(result, err);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_Copy_0009";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Copy_0010
 * @tc.name: js_file_access_ext_ability_Copy_0010
 * @tc.desc: Test function of Copy interface for ERROR with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_Copy_0010, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_Copy_0010";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        vector<Result> copyResult;
        bool force = false;
        int err = -1;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        auto result = ability->Copy(sourceUri, destUri, copyResult, force);
        EXPECT_EQ(result, err);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_Copy_0010";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Copy_0011
 * @tc.name: js_file_access_ext_ability_Copy_0011
 * @tc.desc: Test function of Copy interface for ERROR with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_Copy_0011, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_Copy_0011";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        vector<Result> copyResult;
        bool force = false;
        int err = -1;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        auto result = ability->Copy(sourceUri, destUri, copyResult, force);
        EXPECT_EQ(result, err);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_Copy_0011";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Copy_0012
 * @tc.name: js_file_access_ext_ability_Copy_0012
 * @tc.desc: Test function of Copy interface for ERROR with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_Copy_0012, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_Copy_0012";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        vector<Result> copyResult;
        bool force = false;
        int err = -1;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _))
            .WillOnce(Return(napi_ok));
        auto result = ability->Copy(sourceUri, destUri, copyResult, force);
        EXPECT_EQ(result, err);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_Copy_0012";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Copy_0013
 * @tc.name: js_file_access_ext_ability_Copy_0013
 * @tc.desc: Test function of Copy interface for ERROR with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_Copy_0013, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_Copy_0013";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        vector<Result> copyResult;
        bool force = false;
        int err = -1;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _))
            .WillOnce(Return(napi_ok));
        auto result = ability->Copy(sourceUri, destUri, copyResult, force);
        EXPECT_EQ(result, err);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_Copy_0013";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Copy_0014
 * @tc.name: js_file_access_ext_ability_Copy_0014
 * @tc.desc: Test function of Copy interface for ERROR with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_Copy_0014, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_Copy_0014";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        vector<Result> copyResult;
        bool force = false;
        int err = -1;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _))
            .WillOnce(Return(napi_ok));
        auto result = ability->Copy(sourceUri, destUri, copyResult, force);
        EXPECT_EQ(result, err);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_Copy_0014";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CopyFile_0008
 * @tc.name: js_file_access_ext_ability_CopyFile_0008
 * @tc.desc: Test function of CopyFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_CopyFile_0008, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_CopyFile_0008";
    try {
        EXPECT_NE(ability, nullptr);
        string path = "test";
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        string fileName;
        Uri newFileUri("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();
 
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->CopyFile(sourceUri, destUri, fileName, newFileUri);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_CopyFile_0008";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CopyFile_0009
 * @tc.name: js_file_access_ext_ability_CopyFile_0009
 * @tc.desc: Test function of CopyFile interface for SUCCESS with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_CopyFile_0009, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_CopyFile_0009";
    try {
        EXPECT_NE(ability, nullptr);
        string path = "test";
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        string fileName;
        Uri newFileUri("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->CopyFile(sourceUri, destUri, fileName, newFileUri);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_CopyFile_0009";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CopyFile_0010
 * @tc.name: js_file_access_ext_ability_CopyFile_0010
 * @tc.desc: Test function of CopyFile interface for SUCCESS with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_CopyFile_0010, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_CopyFile_0010";
    try {
        EXPECT_NE(ability, nullptr);
        string path = "test";
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        string fileName;
        Uri newFileUri("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->CopyFile(sourceUri, destUri, fileName, newFileUri);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_CopyFile_0010";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CopyFile_0011
 * @tc.name: js_file_access_ext_ability_CopyFile_0011
 * @tc.desc: Test function of CopyFile interface for SUCCESS with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_CopyFile_0011, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_CopyFile_0011";
    try {
        EXPECT_NE(ability, nullptr);
        string path = "test";
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        string fileName;
        Uri newFileUri("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->CopyFile(sourceUri, destUri, fileName, newFileUri);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_CopyFile_0011";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CopyFile_0012
 * @tc.name: js_file_access_ext_ability_CopyFile_0012
 * @tc.desc: Test function of CopyFile interface for SUCCESS with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_CopyFile_0012, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_CopyFile_0012";
    try {
        EXPECT_NE(ability, nullptr);
        string path = "test";
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        string fileName;
        Uri newFileUri("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->CopyFile(sourceUri, destUri, fileName, newFileUri);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_CopyFile_0012";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CopyFile_0013
 * @tc.name: js_file_access_ext_ability_CopyFile_0013
 * @tc.desc: Test function of CopyFile interface for SUCCESS with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_CopyFile_0013, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_CopyFile_0013";
    try {
        EXPECT_NE(ability, nullptr);
        string path = "test";
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        string fileName;
        Uri newFileUri("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->CopyFile(sourceUri, destUri, fileName, newFileUri);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_CopyFile_0013";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CopyFile_0014
 * @tc.name: js_file_access_ext_ability_CopyFile_0014
 * @tc.desc: Test function of CopyFile interface for SUCCESS with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_CopyFile_0014, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_CopyFile_0014";
    try {
        EXPECT_NE(ability, nullptr);
        string path = "test";
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        string fileName;
        Uri newFileUri("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->CopyFile(sourceUri, destUri, fileName, newFileUri);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_CopyFile_0014";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CopyFile_0015
 * @tc.name: js_file_access_ext_ability_CopyFile_0015
 * @tc.desc: Test function of CopyFile interface for SUCCESS with different mock setup.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityLambdaTest, js_file_access_ext_ability_CopyFile_0015, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-begin js_file_access_ext_ability_CopyFile_0015";
    try {
        EXPECT_NE(ability, nullptr);
        string path = "test";
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        string fileName;
        Uri newFileUri("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->CopyFile(sourceUri, destUri, fileName, newFileUri);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityLambdaTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityLambdaTest-end js_file_access_ext_ability_CopyFile_0015";
}
