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

#include "assistant.h"
#include "extension_context.h"
#include "file_access_ext_stub.h"
#include "file_access_service_mock.h"
#include "file_access_service_proxy.h"
#include "js_file_access_ext_ability.h"
#include "js_native_api_types.h"
#include "native_reference_mock.h"

#include "js_file_access_ext_ability.cpp"

namespace OHOS::FileAccessFwk {
using namespace std;
using namespace testing;
using namespace testing::ext;
const int ARG_INDEX_FIRST = 1;
const int ARG_INDEX_SECOND = 2;
const int ARG_INDEX_THIRD = 3;
const int ARG_INDEX_FOUR = 4;
const int ARG_INDEX_FIFTH = 5;

FileAccessServiceStub::FileAccessServiceStub() {}

FileAccessServiceStub::~FileAccessServiceStub() {}

int32_t FileAccessServiceStub::OnRemoteRequest(unsigned int, OHOS::MessageParcel&, OHOS::MessageParcel&,
    OHOS::MessageOption&)
{
    return 0;
}

int32_t FileAccessServiceProxy::RegisterNotify(Uri, bool, const sptr<IFileAccessObserver> &,
    const std::shared_ptr<ConnectExtensionInfo> &)
{
    return 0;
}

int32_t FileAccessServiceProxy::OnChange(Uri uri, NotifyType notifyType)
{
    return ERR_OK;
}

int32_t FileAccessServiceProxy::GetExtensionProxy(const std::shared_ptr<ConnectExtensionInfo> &,
    sptr<IFileAccessExtBase> &)
{
    return 0;
}

int32_t FileAccessServiceProxy::UnregisterNotify(Uri, const sptr<IFileAccessObserver> &,
    const std::shared_ptr<ConnectExtensionInfo> &)
{
    return 0;
}

int32_t FileAccessServiceProxy::ConnectFileExtAbility(const AAFwk::Want &,
    const sptr<AAFwk::IAbilityConnection>&)
{
    return 0;
}

int32_t FileAccessServiceProxy::DisConnectFileExtAbility(const sptr<AAFwk::IAbilityConnection>&)
{
    return 0;
}

void FileAccessExtAbility::Init(const std::shared_ptr<AbilityLocalRecord> &record,
    const std::shared_ptr<OHOSApplication> &application,
    std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    ExtensionBase<>::Init(record, application, handler, token);
}

FileAccessExtStub::FileAccessExtStub() {}

FileAccessExtStub::~FileAccessExtStub() {}

int32_t FileAccessExtStub::OnRemoteRequest(unsigned int, OHOS::MessageParcel&, OHOS::MessageParcel&,
    OHOS::MessageOption&)
{
    return 0;
}

class JsFileAccessExtAbilityTest : public testing::Test {
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

sptr<FileAccessServiceProxy> FileAccessServiceProxy::GetInstance()
{
    return iface_cast<FileAccessServiceProxy>(JsFileAccessExtAbilityTest::impl);
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CallObjectMethod_0000
 * @tc.name: js_file_access_ext_ability_CallObjectMethod_0000
 * @tc.desc: Test function of CallObjectMethod interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_CallObjectMethod_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_CallObjectMethod_0000";
    try {
        EXPECT_NE(ability, nullptr);
        auto result = ability->CallObjectMethod(nullptr, nullptr, 0);
        EXPECT_TRUE(result == nullptr);

        auto ref = make_shared<NativeReferenceMock>();
        ability->jsObj_ = ref;
        EXPECT_CALL(*ref, GetNapiValue()).WillOnce(Return(nullptr));
        result = ability->CallObjectMethod(nullptr, nullptr, 0);
        EXPECT_TRUE(result == nullptr);

        napi_value rslt = nullptr;
        EXPECT_CALL(*ref, GetNapiValue()).WillOnce(Return(reinterpret_cast<napi_value>(&rslt)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        result = ability->CallObjectMethod(nullptr, nullptr, 0);
        EXPECT_TRUE(result == nullptr);

        EXPECT_CALL(*ref, GetNapiValue()).WillOnce(Return(reinterpret_cast<napi_value>(&rslt)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        result = ability->CallObjectMethod(nullptr, nullptr, 0);
        EXPECT_TRUE(result == nullptr);

        EXPECT_CALL(*ref, GetNapiValue()).WillOnce(Return(reinterpret_cast<napi_value>(&rslt)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        result = ability->CallObjectMethod(nullptr, nullptr, 0);
        EXPECT_TRUE(result == nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_CallObjectMethod_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CallJsMethod_0000
 * @tc.name: js_file_access_ext_ability_CallJsMethod_0000
 * @tc.desc: Test function of CallJsMethod interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_CallJsMethod_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_CallJsMethod_0000";
    try {
        EXPECT_NE(ability, nullptr);
        string funcNameIn;
        NativeReference *jsObj = nullptr;
        InputArgsParser argParser = [](napi_env &env, napi_value *argv, size_t &argc) -> bool {
            return false;
        };
        ResultValueParser retParser = nullptr;
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->CallJsMethod(funcNameIn, *jsRuntime, jsObj, argParser, retParser);
        EXPECT_EQ(result, EINVAL);

        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        result = ability->CallJsMethod(funcNameIn, *jsRuntime, jsObj, argParser, retParser);
        EXPECT_EQ(result, EINVAL);

        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->CallJsMethod(funcNameIn, *jsRuntime, jsObj, argParser, retParser);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_CallJsMethod_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_DoCallJsMethod_0000
 * @tc.name: js_file_access_ext_ability_DoCallJsMethod_0000
 * @tc.desc: Test function of DoCallJsMethod interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_DoCallJsMethod_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_DoCallJsMethod_0000";
    try {
        EXPECT_NE(ability, nullptr);
        string funcNameIn;
        napi_value rslt = nullptr;
        NativeReference *jsObj = nullptr;
        InputArgsParser argParser = [](napi_env &env, napi_value *argv, size_t &argc) -> bool {
            return false;
        };
        ResultValueParser retParser = nullptr;
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->CallJsMethod(funcNameIn, *jsRuntime, jsObj, argParser, retParser);
        EXPECT_EQ(result, EINVAL);

        argParser = nullptr;
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->CallJsMethod(funcNameIn, *jsRuntime, jsObj, argParser, retParser);
        EXPECT_EQ(result, EINVAL);

        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        result = ability->CallJsMethod(funcNameIn, *jsRuntime, jsObj, argParser, retParser);
        EXPECT_EQ(result, EINVAL);

        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        result = ability->CallJsMethod(funcNameIn, *jsRuntime, jsObj, argParser, retParser);
        EXPECT_EQ(result, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_DoCallJsMethod_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_DoCallJsMethod_0001
 * @tc.name: js_file_access_ext_ability_DoCallJsMethod_0001
 * @tc.desc: Test function of DoCallJsMethod interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_DoCallJsMethod_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_DoCallJsMethod_0001";
    try {
        EXPECT_NE(ability, nullptr);
        string funcNameIn;
        napi_value rslt = nullptr;
        NativeReference *jsObj = nullptr;
        InputArgsParser argParser = nullptr;
        ResultValueParser retParser = [](napi_env&, napi_value) -> bool {
            return false;
        };
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->CallJsMethod(funcNameIn, *jsRuntime, jsObj, argParser, retParser);
        EXPECT_EQ(result, EINVAL);

        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        result = ability->CallJsMethod(funcNameIn, *jsRuntime, jsObj, argParser, retParser);
        EXPECT_EQ(result, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_DoCallJsMethod_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_OpenFile_0000
 * @tc.name: js_file_access_ext_ability_OpenFile_0000
 * @tc.desc: Test function of OpenFile interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_OpenFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_OpenFile_0000";
    try {
        EXPECT_NE(ability, nullptr);
        const Uri uri("");
        int fd = -1;

        // 模拟调用CallJsMethod失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->OpenFile(uri, 0, fd);
        EXPECT_EQ(result, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_OpenFile_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_OpenFile_0001
 * @tc.name: js_file_access_ext_ability_OpenFile_0001
 * @tc.desc: Test function of OpenFile interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_OpenFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_OpenFile_0001";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        const Uri uri("");
        int fd = -1;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取nativeUri为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->OpenFile(uri, 0, fd);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取nativeFlags为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int32(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->OpenFile(uri, 0, fd);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_OpenFile_0001";
}

static void MockNapiCalls(shared_ptr<AssistantMock> insMoc, napi_value rslt)
{
    EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
    EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
    EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
        .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
    EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
}

static void MockNapiFunctionCalls(shared_ptr<AssistantMock> insMoc, const string& path, napi_value rslt)
{
    EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
    EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
    EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
        .WillOnce(Return(napi_ok))
        .WillOnce(Return(napi_ok));
    EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FOUR>(path.length()), Return(napi_ok)))
        .WillOnce(DoAll(SetArrayArgument<ARG_INDEX_SECOND>(path.begin(), path.end()), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
}

static void MockNapiFunctionCallsForCopy(shared_ptr<AssistantMock> insMoc, napi_value rslt)
{
    EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
    EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
    EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
}

static void MockNapiFunctionCallsForCopyFile(shared_ptr<AssistantMock> insMoc, const string& path, napi_value rslt)
{
    EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
    EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
    EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
        .WillOnce(Return(napi_ok))
        .WillOnce(Return(napi_ok));
    EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_OpenFile_0002
 * @tc.name: js_file_access_ext_ability_OpenFile_0002
 * @tc.desc: Test function of OpenFile interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_OpenFile_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_OpenFile_0002";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        const Uri uri("");
        int fd = -1;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取value->code失败
        MockNapiCalls(insMoc, rslt);
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(E_IPCS), Return(napi_ok)));
        auto result = ability->OpenFile(uri, 0, fd);
        EXPECT_EQ(result, E_IPCS);

        // 模拟获取value->data为-1
        MockNapiCalls(insMoc, rslt);
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(fd), Return(napi_ok))).WillOnce(Return(napi_ok));
        result = ability->OpenFile(uri, 0, fd);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_OpenFile_0002";
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
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_OpenFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_OpenFile_0003";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        const Uri uri("");
        int fd = -1;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟OpenFile调用成功
        MockNapiCalls(insMoc, rslt);
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        auto result = ability->OpenFile(uri, 0, fd);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_OpenFile_0003";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CreateFile_0000
 * @tc.name: js_file_access_ext_ability_CreateFile_0000
 * @tc.desc: Test function of CreateFile interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_CreateFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_CreateFile_0000";
    try {
        Uri parent("");
        string displayName("");
        Uri newFile("");
        EXPECT_NE(ability, nullptr);

        // 模拟调用CallJsMethod失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->CreateFile(parent, displayName, newFile);
        EXPECT_EQ(result, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_CreateFile_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CreateFile_0001
 * @tc.name: js_file_access_ext_ability_CreateFile_0001
 * @tc.desc: Test function of CreateFile interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_CreateFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_CreateFile_0001";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri parent("");
        string displayName("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取nativeParent为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        auto result = ability->CreateFile(parent, displayName, newFile);
        EXPECT_EQ(result, E_GETRESULT);

        // 模拟获取nativeDisplayName为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        result = ability->CreateFile(parent, displayName, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_CreateFile_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CreateFile_0002
 * @tc.name: js_file_access_ext_ability_CreateFile_0002
 * @tc.desc: Test function of CreateFile interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_CreateFile_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_CreateFile_0002";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri parent("");
        string displayName("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取value->code失败
        MockNapiCalls(insMoc, rslt);
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillRepeatedly(
            DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(E_IPCS), Return(napi_ok)));
        auto result = ability->CreateFile(parent, displayName, newFile);
        EXPECT_EQ(result, E_IPCS);

        // 模拟获取value->data为-1
        MockNapiCalls(insMoc, rslt);
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillRepeatedly(
            DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(Return(napi_ok));
        result = ability->CreateFile(parent, displayName, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_CreateFile_0002";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CreateFile_0003
 * @tc.name: js_file_access_ext_ability_CreateFile_0003
 * @tc.desc: Test function of CreateFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_CreateFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_CreateFile_0003";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        string path = "test";
        Uri parent("");
        string displayName("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟CreateFile调用成功
        MockNapiFunctionCalls(insMoc, path, rslt);
        auto result = ability->CreateFile(parent, displayName, newFile);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_CreateFile_0003";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Mkdir_0000
 * @tc.name: js_file_access_ext_ability_Mkdir_0000
 * @tc.desc: Test function of Mkdir interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Mkdir_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Mkdir_0000";
    try {
        EXPECT_NE(ability, nullptr);
        Uri parent("");
        string displayName("");
        Uri newFile("");

        // 模拟调用CallJsMethod失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->Mkdir(parent, displayName, newFile);
        EXPECT_EQ(result, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Mkdir_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Mkdir_0001
 * @tc.name: js_file_access_ext_ability_Mkdir_0001
 * @tc.desc: Test function of Mkdir interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Mkdir_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Mkdir_0001";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri parent("");
        string displayName("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取nativeParent为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        auto result = ability->Mkdir(parent, displayName, newFile);
        EXPECT_EQ(result, E_GETRESULT);

        // 模拟获取nativeDisplayName为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        result = ability->Mkdir(parent, displayName, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Mkdir_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Mkdir_0002
 * @tc.name: js_file_access_ext_ability_Mkdir_0002
 * @tc.desc: Test function of Mkdir interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Mkdir_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Mkdir_0002";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri parent("");
        string displayName("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取value->code失败
        MockNapiCalls(insMoc, rslt);
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillRepeatedly(
            DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(E_IPCS), Return(napi_ok)));
        auto result = ability->Mkdir(parent, displayName, newFile);
        EXPECT_EQ(result, E_IPCS);

        // 模拟获取value->data为-1
        MockNapiCalls(insMoc, rslt);
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillRepeatedly(
            DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->Mkdir(parent, displayName, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Mkdir_0002";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Mkdir_0003
 * @tc.name: js_file_access_ext_ability_Mkdir_0003
 * @tc.desc: Test function of Mkdir interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Mkdir_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Mkdir_0003";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        string path = "test";
        Uri parent("");
        string displayName("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟Mkdir调用成功
        MockNapiFunctionCalls(insMoc, path, rslt);
        auto result = ability->Mkdir(parent, displayName, newFile);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Mkdir_0003";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Delete_0000
 * @tc.name: js_file_access_ext_ability_Delete_0000
 * @tc.desc: Test function of Delete interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Delete_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Delete_0000";
    try {
        EXPECT_NE(ability, nullptr);
        Uri sourceFile("");

        // 模拟调用CallJsMethod失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->Delete(sourceFile);
        EXPECT_EQ(result, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Delete_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Delete_0001
 * @tc.name: js_file_access_ext_ability_Delete_0001
 * @tc.desc: Test function of Delete interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Delete_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Delete_0001";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取nativeUri为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->Delete(sourceFile);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取nativeDisplayName为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(E_IPCS), Return(napi_invalid_arg)));
        result = ability->Delete(sourceFile);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Delete_0001";
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
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Delete_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Delete_0002";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟Delete调用成功
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(ERR_OK), Return(napi_ok)));
        auto result = ability->Delete(sourceFile);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Delete_0002";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Move_0000
 * @tc.name: js_file_access_ext_ability_Move_0000
 * @tc.desc: Test function of Move interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Move_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Move_0000";
    try {
        EXPECT_NE(ability, nullptr);
        Uri sourceFile("");
        Uri targetParent("");
        Uri newFile("");

        // 模拟调用CallJsMethod失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->Move(sourceFile, targetParent, newFile);
        EXPECT_EQ(result, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Move_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Move_0001
 * @tc.name: js_file_access_ext_ability_Move_0001
 * @tc.desc: Test function of Move interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Move_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Move_0001";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceFile("");
        Uri targetParent("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取srcUri为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        auto result = ability->Move(sourceFile, targetParent, newFile);
        EXPECT_EQ(result, E_GETRESULT);

        // 模拟获取dstUri为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        result = ability->Move(sourceFile, targetParent, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Move_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Move_0002
 * @tc.name: js_file_access_ext_ability_Move_0002
 * @tc.desc: Test function of Move interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Move_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Move_0002";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceFile("");
        Uri targetParent("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取value->code失败
        MockNapiCalls(insMoc, rslt);
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillRepeatedly(
            DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(E_IPCS), Return(napi_ok)));
        auto result = ability->Move(sourceFile, targetParent, newFile);
        EXPECT_EQ(result, E_IPCS);

        // 模拟获取value->data为-1
        MockNapiCalls(insMoc, rslt);
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillRepeatedly(
            DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(Return(napi_ok));
        result = ability->Move(sourceFile, targetParent, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Move_0002";
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
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Move_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Move_0003";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        string path = "test";
        Uri sourceFile("");
        Uri targetParent("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟Move调用成功
        MockNapiFunctionCalls(insMoc, path, rslt);
        auto result = ability->Move(sourceFile, targetParent, newFile);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Move_0003";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Copy_0000
 * @tc.name: js_file_access_ext_ability_Copy_0000
 * @tc.desc: Test function of Copy interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Copy_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Copy_0000";
    try {
        EXPECT_NE(ability, nullptr);
        Uri sourceUri("");
        Uri destUri("");
        vector<Result> copyResult;
        bool force = false;

        // 模拟调用CallJsMethod失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->Copy(sourceUri, destUri, copyResult, force);
        EXPECT_EQ(result, EXCEPTION);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Copy_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Copy_0001
 * @tc.name: js_file_access_ext_ability_Copy_0001
 * @tc.desc: Test function of Copy interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Copy_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Copy_0001";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        vector<Result> copyResult;
        bool force = false;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取srcNativeUri为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->Copy(sourceUri, destUri, copyResult, force);
        EXPECT_EQ(result, EXCEPTION);

        // 模拟获取dstNativeUri为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->Copy(sourceUri, destUri, copyResult, force);
        EXPECT_EQ(result, EXCEPTION);

        // 模拟获取forceCopy为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->Copy(sourceUri, destUri, copyResult, force);
        EXPECT_EQ(result, EXCEPTION);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Copy_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Copy_0002
 * @tc.name: js_file_access_ext_ability_Copy_0002
 * @tc.desc: Test function of Copy interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Copy_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Copy_0002";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        vector<Result> copyResult;
        bool force = false;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取nativeValue失败
        MockNapiFunctionCallsForCopy(insMoc, rslt);
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->Copy(sourceUri, destUri, copyResult, force);
        EXPECT_EQ(result, EXCEPTION);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Copy_0002";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Copy_0003
 * @tc.name: js_file_access_ext_ability_Copy_0003
 * @tc.desc: Test function of Copy interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Copy_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Copy_0003";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        vector<Result> copyResult;
        bool force = false;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取copyRet失败
        MockNapiFunctionCallsForCopy(insMoc, rslt);
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->Copy(sourceUri, destUri, copyResult, force);
        EXPECT_EQ(result, EXCEPTION);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Copy_0003";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Copy_0004
 * @tc.name: js_file_access_ext_ability_Copy_0004
 * @tc.desc: Test function of Copy interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Copy_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Copy_0004";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        vector<Result> copyResult;
        bool force = false;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取copyRet等于ERR_OK
        MockNapiFunctionCallsForCopy(insMoc, rslt);
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(ERR_OK), Return(napi_ok)));
        auto result = ability->Copy(sourceUri, destUri, copyResult, force);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Copy_0004";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Copy_0005
 * @tc.name: js_file_access_ext_ability_Copy_0005
 * @tc.desc: Test function of Copy interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Copy_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Copy_0005";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        vector<Result> copyResult;
        bool force = false;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取nativeArray为空
        MockNapiFunctionCallsForCopy(insMoc, rslt);
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(E_IPCS), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        auto result = ability->Copy(sourceUri, destUri, copyResult, force);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Copy_0005";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Copy_0006
 * @tc.name: js_file_access_ext_ability_Copy_0006
 * @tc.desc: Test function of Copy interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Copy_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Copy_0006";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        vector<Result> copyResult;
        bool force = false;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取length失败
        MockNapiFunctionCallsForCopy(insMoc, rslt);
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(E_IPCS), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_array(_, _))
            .WillRepeatedly(DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_value>(&rslt)),
                Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->Copy(sourceUri, destUri, copyResult, force);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Copy_0006";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Copy_0007
 * @tc.name: js_file_access_ext_ability_Copy_0007
 * @tc.desc: Test function of Copy interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Copy_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Copy_0007";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        vector<Result> copyResult;
        bool force = false;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取nativeResult失败
        MockNapiFunctionCallsForCopy(insMoc, rslt);
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(E_IPCS), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_array(_, _))
            .WillRepeatedly(DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_value>(&rslt)),
                Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(1), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_element(_, _, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->Copy(sourceUri, destUri, copyResult, force);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Copy_0007";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CopyFile_0000
 * @tc.name: js_file_access_ext_ability_CopyFile_0000
 * @tc.desc: Test function of CopyFile interface for CallJsMethod ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_CopyFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_CopyFile_0000";
    try {
        EXPECT_NE(ability, nullptr);
        Uri sourceUri("");
        Uri destUri("");
        string fileName;
        Uri newFileUri("");

        // 模拟调用CallJsMethod失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->CopyFile(sourceUri, destUri, fileName, newFileUri);
        EXPECT_EQ(result, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_CopyFile_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CopyFile_0001
 * @tc.name: js_file_access_ext_ability_CopyFile_0001
 * @tc.desc: Test function of CopyFile interface for ERROR, the reason is that one of srcNativeUri、dstNativeUri and
 *           fileNativeName is empty.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_CopyFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_CopyFile_0001";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        string fileName;
        Uri newFileUri("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取srcNativeUri为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        auto result = ability->CopyFile(sourceUri, destUri, fileName, newFileUri);
        EXPECT_EQ(result, E_GETRESULT);

        // 模拟获取dstNativeUri为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        result = ability->CopyFile(sourceUri, destUri, fileName, newFileUri);
        EXPECT_EQ(result, E_GETRESULT);

        // 模拟获取fileNativeName为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        result = ability->CopyFile(sourceUri, destUri, fileName, newFileUri);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_CopyFile_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CopyFile_0002
 * @tc.name: js_file_access_ext_ability_CopyFile_0002
 * @tc.desc: Test function of CopyFile interface for setting reserve size ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_CopyFile_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_CopyFile_0002";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        string fileName;
        Uri newFileUri("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟为reserve获取的size失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->CopyFile(sourceUri, destUri, fileName, newFileUri);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_CopyFile_0002";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CopyFile_0003
 * @tc.name: js_file_access_ext_ability_CopyFile_0003
 * @tc.desc: Test function of CopyFile interface for setting resize size ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_CopyFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_CopyFile_0003";
    try {
        EXPECT_NE(ability, nullptr);
        string path = "test";
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        string fileName;
        Uri newFileUri("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟为resize获取的size失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FOUR>(path.length()), Return(napi_ok)))
            .WillOnce(Return(napi_invalid_arg));
        auto result = ability->CopyFile(sourceUri, destUri, fileName, newFileUri);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_CopyFile_0003";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CopyFile_0004
 * @tc.name: js_file_access_ext_ability_CopyFile_0004
 * @tc.desc: Test function of CopyFile interface for getting uri succeed but setting code error.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_CopyFile_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_CopyFile_0004";
    try {
        EXPECT_NE(ability, nullptr);
        string path = "test";
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        string fileName;
        Uri newFileUri("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取uri成功但value->code失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FOUR>(path.length()), Return(napi_ok)))
            .WillOnce(DoAll(SetArrayArgument<ARG_INDEX_SECOND>(path.begin(), path.end()), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(E_IPCS), Return(napi_invalid_arg)));
        auto result = ability->CopyFile(sourceUri, destUri, fileName, newFileUri);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_CopyFile_0004";
}


/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CopyFile_0005
 * @tc.name: js_file_access_ext_ability_CopyFile_0005
 * @tc.desc: Test function of CopyFile interface for GetUriAndCodeFromJs SUCCEED.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_CopyFile_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_CopyFile_0005";
    try {
        EXPECT_NE(ability, nullptr);
        string path = "test";
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        string fileName;
        Uri newFileUri("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟GetUriAndCodeFromJs成功
        MockNapiFunctionCallsForCopyFile(insMoc, path, rslt);
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FOUR>(path.length()), Return(napi_ok)))
            .WillOnce(DoAll(SetArrayArgument<ARG_INDEX_SECOND>(path.begin(), path.end()), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(E_IPCS), Return(napi_ok)));
        auto result = ability->CopyFile(sourceUri, destUri, fileName, newFileUri);
        EXPECT_EQ(result, E_IPCS);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_CopyFile_0005";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CopyFile_0006
 * @tc.name: js_file_access_ext_ability_CopyFile_0006
 * @tc.desc: Test function of CopyFile interface for ERROR, the reason is that one of value->code and value->data fail.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_CopyFile_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_CopyFile_0006";
    try {
        EXPECT_NE(ability, nullptr);
        string path = "test";
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        string fileName;
        Uri newFileUri("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取value->code失败
        MockNapiFunctionCallsForCopyFile(insMoc, path, rslt);
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(E_IPCS), Return(napi_ok)));
        auto result = ability->CopyFile(sourceUri, destUri, fileName, newFileUri);
        EXPECT_EQ(result, E_IPCS);

        // 模拟获取value->data为-1
        MockNapiFunctionCallsForCopyFile(insMoc, path, rslt);
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->CopyFile(sourceUri, destUri, fileName, newFileUri);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_CopyFile_0006";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_CopyFile_0007
 * @tc.name: js_file_access_ext_ability_CopyFile_0007
 * @tc.desc: Test function of CopyFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_CopyFile_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_CopyFile_0007";
    try {
        EXPECT_NE(ability, nullptr);
        string path = "test";
        napi_value rslt = nullptr;
        Uri sourceUri("");
        Uri destUri("");
        string fileName;
        Uri newFileUri("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟CopyFile调用成功
        MockNapiFunctionCallsForCopyFile(insMoc, path, rslt);
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FOUR>(path.length()), Return(napi_ok)))
            .WillOnce(DoAll(SetArrayArgument<ARG_INDEX_SECOND>(path.begin(), path.end()), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->CopyFile(sourceUri, destUri, fileName, newFileUri);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_CopyFile_0007";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Rename_0000
 * @tc.name: js_file_access_ext_ability_Rename_0000
 * @tc.desc: Test function of Rename interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Rename_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Rename_0000";
    try {
        EXPECT_NE(ability, nullptr);
        Uri sourceFile("");
        string displayName("");
        Uri newFile("");

        // 模拟调用CallJsMethod失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->Rename(sourceFile, displayName, newFile);
        EXPECT_EQ(result, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Rename_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Rename_0001
 * @tc.name: js_file_access_ext_ability_Rename_0001
 * @tc.desc: Test function of Rename interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Rename_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Rename_0001";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceFile("");
        string displayName("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取nativeSourceFile为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        auto result = ability->Rename(sourceFile, displayName, newFile);
        EXPECT_EQ(result, E_GETRESULT);

        // 模拟获取nativeDisplayName为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        result = ability->Rename(sourceFile, displayName, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Rename_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Rename_0002
 * @tc.name: js_file_access_ext_ability_Rename_0002
 * @tc.desc: Test function of Rename interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Rename_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Rename_0002";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceFile("");
        string displayName("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取value->code失败
        MockNapiCalls(insMoc, rslt);
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillRepeatedly(
            DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(E_IPCS), Return(napi_ok)));
        auto result = ability->Rename(sourceFile, displayName, newFile);
        EXPECT_EQ(result, E_IPCS);

        // 模拟获取value->data为-1
        MockNapiCalls(insMoc, rslt);
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillRepeatedly(
            DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->Rename(sourceFile, displayName, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Rename_0002";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Rename_0003
 * @tc.name: js_file_access_ext_ability_Rename_0003
 * @tc.desc: Test function of Rename interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Rename_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Rename_0003";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        string path = "test";
        Uri sourceFile("");
        string displayName("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟Rename调用成功
        MockNapiFunctionCalls(insMoc, path, rslt);
        auto result = ability->Rename(sourceFile, displayName, newFile);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Rename_0003";
}

#include "js_file_access_ext_ability_ex_test.cpp"
#include "js_file_access_ext_ability_other_test.cpp"
}