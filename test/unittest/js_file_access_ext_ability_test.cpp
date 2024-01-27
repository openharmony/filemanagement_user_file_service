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

int32_t FileAccessServiceProxy::OnChange(Uri uri, NotifyType notifyType)
{
    return ERR_OK;
}

int32_t FileAccessServiceProxy::GetExensionProxy(const std::shared_ptr<ConnectExtensionInfo> &,
    sptr<IFileAccessExtBase> &)
{
    return 0;
}

int32_t FileAccessServiceProxy::RegisterNotify(Uri, bool, const sptr<IFileAccessObserver> &,
    const std::shared_ptr<ConnectExtensionInfo> &)
{
    return 0;
}

int32_t FileAccessServiceProxy::UnregisterNotify(Uri, const sptr<IFileAccessObserver> &,
    const std::shared_ptr<ConnectExtensionInfo> &)
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
        auto result = ability->CallObjectMethod(nullptr, nullptr, 0);
        EXPECT_TRUE(result == nullptr);

        auto ref = make_shared<NativeReferenceMock>();
        ability->jsObj_ = ref;
        EXPECT_CALL(*ref, GetNapiValue()).WillOnce(Return(nullptr));
        result = ability->CallObjectMethod(nullptr, nullptr, 0);
        EXPECT_TRUE(result == nullptr);

        napi_value rslt;
        EXPECT_CALL(*ref, GetNapiValue()).WillOnce(Return(reinterpret_cast<napi_value>(&rslt)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        result = ability->CallObjectMethod(nullptr, nullptr, 0);
        EXPECT_TRUE(result == nullptr);

        EXPECT_CALL(*ref, GetNapiValue()).WillOnce(Return(reinterpret_cast<napi_value>(&rslt)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        result = ability->CallObjectMethod(nullptr, nullptr, 0);
        EXPECT_TRUE(result == nullptr);

        EXPECT_CALL(*ref, GetNapiValue()).WillOnce(Return(reinterpret_cast<napi_value>(&rslt)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        result = ability->CallObjectMethod(nullptr, nullptr, 0);
        EXPECT_TRUE(result == nullptr);
    } catch (...) {
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
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(-1));
        result = ability->CallJsMethod(funcNameIn, *jsRuntime, jsObj, argParser, retParser);
        EXPECT_EQ(result, EINVAL);

        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        result = ability->CallJsMethod(funcNameIn, *jsRuntime, jsObj, argParser, retParser);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
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
        string funcNameIn;
        napi_value rslt;
        NativeReference *jsObj = nullptr;
        InputArgsParser argParser = [](napi_env &env, napi_value *argv, size_t &argc) -> bool {
            return false;
        };
        ResultValueParser retParser = nullptr;
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(-1));
        auto result = ability->CallJsMethod(funcNameIn, *jsRuntime, jsObj, argParser, retParser);
        EXPECT_EQ(result, EINVAL);

        argParser = nullptr;
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(-1));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->CallJsMethod(funcNameIn, *jsRuntime, jsObj, argParser, retParser);
        EXPECT_EQ(result, EINVAL);

        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(-1));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        result = ability->CallJsMethod(funcNameIn, *jsRuntime, jsObj, argParser, retParser);
        EXPECT_EQ(result, EINVAL);

        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(-1));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        result = ability->CallJsMethod(funcNameIn, *jsRuntime, jsObj, argParser, retParser);
        EXPECT_EQ(result, EINVAL);
    } catch (...) {
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
        string funcNameIn;
        napi_value rslt;
        NativeReference *jsObj = nullptr;
        InputArgsParser argParser = nullptr;
        ResultValueParser retParser = [](napi_env&, napi_value) -> bool {
            return false;
        };
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(-1));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->CallJsMethod(funcNameIn, *jsRuntime, jsObj, argParser, retParser);
        EXPECT_EQ(result, EINVAL);

        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(-1));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<5>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        result = ability->CallJsMethod(funcNameIn, *jsRuntime, jsObj, argParser, retParser);
        EXPECT_EQ(result, EINVAL);
    } catch (...) {
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
        const Uri uri("");
        int fd = -1;

        // 模拟调用CallJsMethod失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->OpenFile(uri, 0, fd);
        EXPECT_EQ(result, EINVAL);
    } catch (...) {
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
        napi_value rslt;
        const Uri uri("");
        int fd = -1;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取nativeUri为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->OpenFile(uri, 0, fd);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取nativeFlags为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int32(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->OpenFile(uri, 0, fd);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_OpenFile_0001";
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
        napi_value rslt;
        const Uri uri("");
        int fd = -1;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取value->code失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<5>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<2>(E_IPCS), Return(napi_ok)));
        auto result = ability->OpenFile(uri, 0, fd);
        EXPECT_EQ(result, E_IPCS);

        // 模拟获取value->data为-1
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<5>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(fd), Return(napi_ok))).WillOnce(Return(napi_ok));
        result = ability->OpenFile(uri, 0, fd);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
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
        napi_value rslt;
        const Uri uri("");
        int fd = -1;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟OpenFile调用成功
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<5>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        auto result = ability->OpenFile(uri, 0, fd);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_OpenFile_0003";
}
}