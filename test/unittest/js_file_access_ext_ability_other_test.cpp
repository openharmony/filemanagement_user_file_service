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

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_MoveFile_0000
 * @tc.name: js_file_access_ext_ability_MoveFile_0000
 * @tc.desc: Test function of MoveFile interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_MoveFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_MoveFile_0000";
    try {
        EXPECT_NE(ability, nullptr);
        Uri sourceFile("");
        Uri targetParent("");
        string fileName("");
        Uri newFile("");

        // 模拟调用CallJsMethod失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->MoveFile(sourceFile, targetParent, fileName, newFile);
        EXPECT_EQ(result, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_MoveFile_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_MoveFile_0001
 * @tc.name: js_file_access_ext_ability_MoveFile_0001
 * @tc.desc: Test function of MoveFile interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_MoveFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_MoveFile_0001";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceFile("");
        Uri targetParent("");
        string fileName("");
        Uri newFile("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取srcUri为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        auto result = ability->MoveFile(sourceFile, targetParent, fileName, newFile);
        EXPECT_EQ(result, E_GETRESULT);

        // 模拟获取dstUri为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        result = ability->MoveFile(sourceFile, targetParent, fileName, newFile);
        EXPECT_EQ(result, E_GETRESULT);

        // 模拟获取name为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        result = ability->MoveFile(sourceFile, targetParent, fileName, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_MoveFile_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_MoveFile_0002
 * @tc.name: js_file_access_ext_ability_MoveFile_0002
 * @tc.desc: Test function of MoveFile interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_MoveFile_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_MoveFile_0002";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceFile("");
        Uri targetParent("");
        string fileName("");
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
        auto result = ability->MoveFile(sourceFile, targetParent, fileName, newFile);
        EXPECT_EQ(result, E_GETRESULT);

        // 模拟获取value->data为-1
        MockNapiCalls(insMoc, rslt);
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillRepeatedly(
            DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(Return(napi_ok));
        result = ability->MoveFile(sourceFile, targetParent, fileName, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_MoveFile_0002";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_MoveFile_0003
 * @tc.name: js_file_access_ext_ability_MoveFile_0003
 * @tc.desc: Test function of MoveFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_MoveFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_MoveFile_0003";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceFile("");
        Uri targetParent("");
        string fileName("");
        Uri newFile("");
        string path = "test";
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟MoveFile调用成功
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
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FOUR>(path.length()), Return(napi_ok)))
            .WillOnce(DoAll(SetArrayArgument<ARG_INDEX_SECOND>(path.begin(), path.end()), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->MoveFile(sourceFile, targetParent, fileName, newFile);
        EXPECT_EQ(result, E_GETRESULT);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_MoveFile_0003";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_MoveItem_0000
 * @tc.name: js_file_access_ext_ability_MoveItem_0000
 * @tc.desc: Test function of MoveItem interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_MoveItem_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_MoveItem_0000";
    try {
        EXPECT_NE(ability, nullptr);
        Uri sourceFile("");
        Uri targetParent("");
        vector<Result> moveResult;
        bool force = false;

        // 模拟调用CallJsMethod失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->MoveItem(sourceFile, targetParent, moveResult, force);
        EXPECT_EQ(result, EXCEPTION);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_MoveItem_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_MoveItem_0001
 * @tc.name: js_file_access_ext_ability_MoveItem_0001
 * @tc.desc: Test function of MoveItem interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_MoveItem_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_MoveItem_0001";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceFile("");
        Uri targetParent("");
        vector<Result> moveResult;
        bool force = false;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取srcNativeUri为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->MoveItem(sourceFile, targetParent, moveResult, force);
        EXPECT_EQ(result, EXCEPTION);

        // 模拟获取dstNativeUri为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->MoveItem(sourceFile, targetParent, moveResult, force);
        EXPECT_EQ(result, EXCEPTION);

        // 模拟获取forceMove为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->MoveItem(sourceFile, targetParent, moveResult, force);
        EXPECT_EQ(result, EXCEPTION);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_MoveItem_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_MoveItem_0002
 * @tc.name: js_file_access_ext_ability_MoveItem_0002
 * @tc.desc: Test function of MoveItem interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_MoveItem_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_MoveItem_0002";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri sourceFile("");
        Uri targetParent("");
        vector<Result> moveResult;
        bool force = false;
        string path = "test";
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟MoveItem调用成功
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(ERR_OK), Return(napi_ok)));
        auto result = ability->MoveItem(sourceFile, targetParent, moveResult, force);
        EXPECT_EQ(result, ERR_INVALID);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_MoveItem_0002";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetRootInfo_0000
 * @tc.name: js_file_access_ext_ability_GetRootInfo_0000
 * @tc.desc: Test function of GetRootInfo interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetRootInfo_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetRootInfo_0000";
    try {
        EXPECT_NE(ability, nullptr);
        napi_env env = nullptr;
        napi_value nativeRootInfo = {};
        RootInfo rootInfo;

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->GetRootInfo(env, nativeRootInfo, rootInfo);
        EXPECT_EQ(result, napi_generic_failure);

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        result = ability->GetRootInfo(env, nativeRootInfo, rootInfo);
        EXPECT_EQ(result, napi_generic_failure);

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_invalid_arg));
        result = ability->GetRootInfo(env, nativeRootInfo, rootInfo);
        EXPECT_EQ(result, napi_generic_failure);

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_invalid_arg));
        result = ability->GetRootInfo(env, nativeRootInfo, rootInfo);
        EXPECT_EQ(result, napi_generic_failure);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetRootInfo_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetRootInfo_0001
 * @tc.name: js_file_access_ext_ability_GetRootInfo_0001
 * @tc.desc: Test function of GetRootInfo interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetRootInfo_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetRootInfo_0001";
    try {
        EXPECT_NE(ability, nullptr);
        napi_env env = nullptr;
        napi_value nativeRootInfo = {};
        RootInfo rootInfo;

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        auto result = ability->GetRootInfo(env, nativeRootInfo, rootInfo);
        EXPECT_EQ(result, napi_generic_failure);

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        result = ability->GetRootInfo(env, nativeRootInfo, rootInfo);
        EXPECT_EQ(result, napi_ok);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetRootInfo_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ConstructQueryArg_0001
 * @tc.name: js_file_access_ext_ability_ConstructQueryArg_0001
 * @tc.desc: Test function of ConstructQueryArg interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ConstructQueryArg_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ConstructQueryArg_0001";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        napi_env env = nullptr;
        napi_value argv[2] = {};
        size_t argc = 0;
        Uri uri("");
        vector<string> columns;

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->ConstructQueryArg(env, argv, argc, uri, columns);
        EXPECT_EQ(result, napi_generic_failure);

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_array_with_length(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->ConstructQueryArg(env, argv, argc, uri, columns);
        EXPECT_EQ(result, napi_generic_failure);

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_array_with_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        result = ability->ConstructQueryArg(env, argv, argc, uri, columns);
        EXPECT_EQ(result, napi_ok);

        columns.emplace_back("test");
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_array_with_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        result = ability->ConstructQueryArg(env, argv, argc, uri, columns);
        EXPECT_EQ(result, napi_generic_failure);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ConstructQueryArg_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_StartWatcher_0000
 * @tc.name: js_file_access_ext_ability_StartWatcher_0000
 * @tc.desc: Test function of StartWatcher interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_StartWatcher_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_StartWatcher_0000";
    try {
        EXPECT_NE(ability, nullptr);
        Uri uri("");

        // 模拟调用CallJsMethod失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->StartWatcher(uri);
        EXPECT_EQ(result, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_StartWatcher_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_StartWatcher_0001
 * @tc.name: js_file_access_ext_ability_StartWatcher_0001
 * @tc.desc: Test function of StartWatcher interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_StartWatcher_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_StartWatcher_0001";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri uri("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取nativeUri为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->StartWatcher(uri);
        EXPECT_EQ(result, false);

        // 模拟创建FuncCallback失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        result = ability->StartWatcher(uri);
        EXPECT_EQ(result, false);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_StartWatcher_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_StartWatcher_0002
 * @tc.name: js_file_access_ext_ability_StartWatcher_0002
 * @tc.desc: Test function of StartWatcher interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_StartWatcher_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_StartWatcher_0002";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri uri("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取ret失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(E_IPCS), Return(napi_ok)));
        auto result = ability->StartWatcher(uri);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_StartWatcher_0002";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_StartWatcher_0003
 * @tc.name: js_file_access_ext_ability_StartWatcher_0003
 * @tc.desc: Test function of StartWatcher interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_StartWatcher_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_StartWatcher_0003";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri uri("");
        string path = "test";
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟StartWatcher调用成功
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
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
        auto result = ability->StartWatcher(uri);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_StartWatcher_0003";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_StopWatcher_0000
 * @tc.name: js_file_access_ext_ability_StopWatcher_0000
 * @tc.desc: Test function of StopWatcher interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_StopWatcher_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_StopWatcher_0000";
    try {
        EXPECT_NE(ability, nullptr);
        Uri uri("");

        // 模拟调用CallJsMethod失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->StopWatcher(uri);
        EXPECT_EQ(result, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_StopWatcher_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_StopWatcher_0001
 * @tc.name: js_file_access_ext_ability_StopWatcher_0001
 * @tc.desc: Test function of StopWatcher interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_StopWatcher_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_StopWatcher_0001";
    try {
        EXPECT_NE(ability, nullptr);
        Uri uri("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取nativeUri为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->StopWatcher(uri);
        EXPECT_EQ(result, false);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_StopWatcher_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_StopWatcher_0002
 * @tc.name: js_file_access_ext_ability_StopWatcher_0002
 * @tc.desc: Test function of StopWatcher interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_StopWatcher_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_StopWatcher_0002";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri uri("");
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取ret失败
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
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(E_IPCS), Return(napi_ok)));
        auto result = ability->StopWatcher(uri);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_StopWatcher_0002";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_StopWatcher_0003
 * @tc.name: js_file_access_ext_ability_StopWatcher_0003
 * @tc.desc: Test function of StopWatcher interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_StopWatcher_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_StopWatcher_0003";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri uri("");
        string path = "test";
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟StopWatcher调用成功
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
        auto result = ability->StopWatcher(uri);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_StopWatcher_0003";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_FuncCallback_0000
 * @tc.name: js_file_access_ext_ability_FuncCallback_0000
 * @tc.desc: Test function of FuncCallback interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_FuncCallback_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_FuncCallback_0000";
    try {
        EXPECT_NE(ability, nullptr);
        int value = 0;
        napi_env env = nullptr;
        napi_callback_info info = nullptr;

        auto result = ability->FuncCallback(env, info);
        EXPECT_TRUE(result == nullptr);

        env = reinterpret_cast<napi_env>(&value);
        EXPECT_CALL(*insMoc, napi_get_undefined(_, _)).WillOnce(Return(napi_ok));
        result = ability->FuncCallback(env, info);
        EXPECT_TRUE(result == nullptr);

        info = reinterpret_cast<napi_callback_info>(&value);
        EXPECT_CALL(*insMoc, napi_get_cb_info(_, _, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*insMoc, napi_get_undefined(_, _)).WillOnce(Return(napi_ok));
        result = ability->FuncCallback(env, info);
        EXPECT_TRUE(result == nullptr);

        EXPECT_CALL(*insMoc, napi_get_cb_info(_, _, _, _, _, _))
           .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>((0)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_undefined(_, _)).WillOnce(Return(napi_ok));
        result = ability->FuncCallback(env, info);
        EXPECT_TRUE(result == nullptr);

        EXPECT_CALL(*insMoc, napi_get_cb_info(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*insMoc, napi_get_undefined(_, _)).WillOnce(Return(napi_ok));
        result = ability->FuncCallback(env, info);
        EXPECT_TRUE(result == nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_FuncCallback_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ParserQueryFileJsResult_0000
 * @tc.name: js_file_access_ext_ability_ParserQueryFileJsResult_0000
 * @tc.desc: Test function of ParserQueryFileJsResult interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ParserQueryFileJsResult_0000,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ParserQueryFileJsResult_0000";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        napi_env env = nullptr;
        napi_value nativeValue = {};
        JsFileAccessExtAbility::Value<vector<string>> value;

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->ParserQueryFileJsResult(env, nativeValue, value);
        EXPECT_FALSE(result);

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        result = ability->ParserQueryFileJsResult(env, nativeValue, value);
        EXPECT_FALSE(result);

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _)).WillOnce(Return(napi_invalid_arg));
        result = ability->ParserQueryFileJsResult(env, nativeValue, value);
        EXPECT_FALSE(result);

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->ParserQueryFileJsResult(env, nativeValue, value);
        EXPECT_TRUE(result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ParserQueryFileJsResult_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ParserQueryFileJsResult_0001
 * @tc.name: js_file_access_ext_ability_ParserQueryFileJsResult_0001
 * @tc.desc: Test function of ParserQueryFileJsResult interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ParserQueryFileJsResult_0001,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ParserQueryFileJsResult_0001";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        napi_env env = nullptr;
        napi_value nativeValue = {};
        JsFileAccessExtAbility::Value<vector<string>> value;

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(1), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_element(_, _, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->ParserQueryFileJsResult(env, nativeValue, value);
        EXPECT_FALSE(result);

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(1), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_element(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        result = ability->ParserQueryFileJsResult(env, nativeValue, value);
        EXPECT_FALSE(result);

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(1), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_element(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        result = ability->ParserQueryFileJsResult(env, nativeValue, value);
        EXPECT_TRUE(result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ParserQueryFileJsResult_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ParserGetRootsJsResult_0000
 * @tc.name: js_file_access_ext_ability_ParserGetRootsJsResult_0000
 * @tc.desc: Test function of ParserGetRootsJsResult interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ParserGetRootsJsResult_0000,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ParserGetRootsJsResult_0000";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        napi_env env = nullptr;
        napi_value nativeValue = nullptr;
        JsFileAccessExtAbility::Value<vector<RootInfo>> value;

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->ParserGetRootsJsResult(env, nativeValue, value);
        EXPECT_FALSE(result);

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        result = ability->ParserGetRootsJsResult(env, nativeValue, value);
        EXPECT_FALSE(result);

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _)).WillOnce(Return(napi_invalid_arg));
        result = ability->ParserGetRootsJsResult(env, nativeValue, value);
        EXPECT_FALSE(result);

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->ParserGetRootsJsResult(env, nativeValue, value);
        EXPECT_TRUE(result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ParserGetRootsJsResult_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ParserGetRootsJsResult_0001
 * @tc.name: js_file_access_ext_ability_ParserGetRootsJsResult_0001
 * @tc.desc: Test function of ParserGetRootsJsResult interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ParserGetRootsJsResult_0001,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ParserGetRootsJsResult_0001";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        napi_env env = nullptr;
        napi_value nativeValue = nullptr;
        JsFileAccessExtAbility::Value<vector<RootInfo>> value;

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(1), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_element(_, _, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->ParserGetRootsJsResult(env, nativeValue, value);
        EXPECT_FALSE(result);

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(1), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_element(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        result = ability->ParserGetRootsJsResult(env, nativeValue, value);
        EXPECT_FALSE(result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ParserGetRootsJsResult_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ParserGetRootsJsResult_0002
 * @tc.name: js_file_access_ext_ability_ParserGetRootsJsResult_0002
 * @tc.desc: Test function of ParserGetRootsJsResult interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ParserGetRootsJsResult_0002,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ParserGetRootsJsResult_0001";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        napi_env env = nullptr;
        napi_value nativeValue = nullptr;
        JsFileAccessExtAbility::Value<vector<RootInfo>> value;

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(1), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_element(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        auto result = ability->ParserGetRootsJsResult(env, nativeValue, value);
        EXPECT_TRUE(result);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ParserGetRootsJsResult_0002";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetRoots_0000
 * @tc.name: js_file_access_ext_ability_GetRoots_0000
 * @tc.desc: Test function of GetRoots interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetRoots_0000, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetRoots_0000";
    try {
        EXPECT_NE(ability, nullptr);
        vector<RootInfo> rootInfoVec;

        // 模拟调用CallJsMethod失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->GetRoots(rootInfoVec);
        EXPECT_EQ(result, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetRoots_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetRoots_0002
 * @tc.name: js_file_access_ext_ability_GetRoots_0002
 * @tc.desc: Test function of GetRoots interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetRoots_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetRoots_0002";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        vector<RootInfo> rootInfoVec;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟获取value->code失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillRepeatedly(
            DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(E_IPCS), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->GetRoots(rootInfoVec);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetRoots_0002";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetRoots_0003
 * @tc.name: js_file_access_ext_ability_GetRoots_0003
 * @tc.desc: Test function of GetRoots interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetRoots_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetRoots_0003";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        string path = "test";
        vector<RootInfo> rootInfoVec;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟GetRoots调用成功
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillRepeatedly(
            DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(ERR_OK), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _)).WillOnce(Return(napi_ok));
            auto result = ability->GetRoots(rootInfoVec);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetRoots_0003";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Access_0000
 * @tc.name: js_file_access_ext_ability_Access_0000
 * @tc.desc: Test function of Access interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Access_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Access_0000";
    try {
        EXPECT_NE(ability, nullptr);
        Uri uri("");
        bool isExist = false;

        // 模拟调用CallJsMethod失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->Access(uri, isExist);
        EXPECT_EQ(result, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Access_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Access_0001
 * @tc.name: js_file_access_ext_ability_Access_0001
 * @tc.desc: Test function of Access interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Access_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Access_0001";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri uri("");
        bool isExist = false;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_bool(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->Access(uri, isExist);
        EXPECT_EQ(result, ERR_OK);

        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_bool(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_invalid_arg));
        result = ability->Access(uri, isExist);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Access_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Access_0002
 * @tc.name: js_file_access_ext_ability_Access_0002
 * @tc.desc: Test function of Access interface for ERROR.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Access_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Access_0002";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri uri("");
        bool isExist = false;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_bool(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->Access(uri, isExist);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取value->code失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_bool(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(E_IPCS), Return(napi_ok)));
        result = ability->Access(uri, isExist);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Access_0002";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_Access_0003
 * @tc.name: js_file_access_ext_ability_Access_0003
 * @tc.desc: Test function of Access interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_Access_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_Access_0003";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        Uri uri("");
        bool isExist = false;
        ability->jsObj_ = make_shared<NativeReferenceMock>();

        // 模拟Access调用成功
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_send_event(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_bool(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(ERR_OK), Return(napi_ok)));
        auto result = ability->Access(uri, isExist);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_Access_0003";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetResultByJs_0000
 * @tc.name: js_file_access_ext_ability_GetResultByJs_0000
 * @tc.desc: Test function of GetResultByJs interface for success and failure branches.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetResultByJs_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetResultByJs_0000";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        napi_env env = nullptr;
        napi_value nativeResult = nullptr;
        Result result;
        int ret = NOEXCEPTION;

        EXPECT_FALSE(GetResultByJs(env, nativeResult, result, ret));

        nativeResult = reinterpret_cast<napi_value>(&rslt);
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_FALSE(GetResultByJs(env, nativeResult, result, ret));

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_invalid_arg));
        EXPECT_FALSE(GetResultByJs(env, nativeResult, result, ret));

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_FALSE(GetResultByJs(env, nativeResult, result, ret));

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_TRUE(GetResultByJs(env, nativeResult, result, ret));

        ret = EXCEPTION;
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_invalid_arg));
        EXPECT_FALSE(GetResultByJs(env, nativeResult, result, ret));

        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_TRUE(GetResultByJs(env, nativeResult, result, ret));

        ret = MAX_ARG_COUNT;
        EXPECT_TRUE(GetResultByJs(env, nativeResult, result, ret));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetResultByJs_0000";
}

static void InvokeCreateNativeValueSucceed(shared_ptr<AssistantMock> insMoc)
{
    // 模拟调用CreateNativeValue成功
    napi_value rslt = nullptr;
    EXPECT_CALL(*insMoc, napi_create_array_with_length(_, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_create_int64(_, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_create_double(_, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_MakeJsNativeFileFilter_0000
 * @tc.name: js_file_access_ext_ability_MakeJsNativeFileFilter_0000
 * @tc.desc: Test function of MakeJsNativeFileFilter interface for success and failure branches.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_MakeJsNativeFileFilter_0000,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_MakeJsNativeFileFilter_0000";
    try {
        EXPECT_NE(ability, nullptr);
        napi_env env = nullptr;
        FileFilter filter;
        napi_value nativeFilter = nullptr;

        EXPECT_CALL(*insMoc, napi_create_array_with_length(_, _, _)).WillOnce(Return(napi_ok));
        auto ret = ability->MakeJsNativeFileFilter(env, filter, nativeFilter);
        EXPECT_EQ(ret, E_GETRESULT);

        InvokeCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _)).WillOnce(Return(napi_invalid_arg));
        ret = ability->MakeJsNativeFileFilter(env, filter, nativeFilter);
        EXPECT_EQ(ret, EINVAL);

        InvokeCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_invalid_arg));
        ret = ability->MakeJsNativeFileFilter(env, filter, nativeFilter);
        EXPECT_EQ(ret, EINVAL);

        InvokeCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_invalid_arg));
        ret = ability->MakeJsNativeFileFilter(env, filter, nativeFilter);
        EXPECT_EQ(ret, EINVAL);

        InvokeCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_invalid_arg));
        ret = ability->MakeJsNativeFileFilter(env, filter, nativeFilter);
        EXPECT_EQ(ret, EINVAL);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_MakeJsNativeFileFilter_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_MakeJsNativeFileFilter_0001
 * @tc.name: js_file_access_ext_ability_MakeJsNativeFileFilter_0001
 * @tc.desc: Test function of MakeJsNativeFileFilter interface for success and failure branches.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_MakeJsNativeFileFilter_0001,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_MakeJsNativeFileFilter_0001";
    try {
        EXPECT_NE(ability, nullptr);
        napi_env env = nullptr;
        FileFilter filter;
        napi_value nativeFilter = nullptr;

        InvokeCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_invalid_arg));
        auto ret = ability->MakeJsNativeFileFilter(env, filter, nativeFilter);
        EXPECT_EQ(ret, EINVAL);

        InvokeCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_invalid_arg));
        ret = ability->MakeJsNativeFileFilter(env, filter, nativeFilter);
        EXPECT_EQ(ret, EINVAL);

        InvokeCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        ret = ability->MakeJsNativeFileFilter(env, filter, nativeFilter);
        EXPECT_EQ(ret, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_MakeJsNativeFileFilter_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_BuildFileInfoNumParam_0000
 * @tc.name: js_file_access_ext_ability_BuildFileInfoNumParam_0000
 * @tc.desc: Test function of BuildFileInfoNumParam interface for success and failure branches.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_BuildFileInfoNumParam_0000,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_BuildFileInfoNumParam_0000";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        napi_env env = nullptr;
        FileFilter filter;
        FileInfoNumParam param { "", filter, false };
        napi_value argv;
        size_t argc;

        filter.hasFilter_ = true;
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_object(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(ability->BuildFileInfoNumParam(env, param, &argv, argc));

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_object(_, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_array_with_length(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(ability->BuildFileInfoNumParam(env, param, &argv, argc));

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_int64(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_object(_, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_array_with_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_double(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_FALSE(ability->BuildFileInfoNumParam(env, param, &argv, argc));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_BuildFileInfoNumParam_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_BuildFileInfoNumParam_0001
 * @tc.name: js_file_access_ext_ability_BuildFileInfoNumParam_0001
 * @tc.desc: Test function of BuildFileInfoNumParam interface for success and failure branches.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_BuildFileInfoNumParam_0001,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_BuildFileInfoNumParam_0001";
    try {
        EXPECT_NE(ability, nullptr);
        napi_value rslt = nullptr;
        FileFilter filter;
        FileInfoNumParam param { "", filter, false };
        napi_value argv[3];
        size_t argc;

        filter.hasFilter_ = false;
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_null(_, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(ability->BuildFileInfoNumParam(env, param, argv, argc));

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_null(_, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(ability->BuildFileInfoNumParam(env, param, argv, argc));

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_null(_, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_FALSE(ability->BuildFileInfoNumParam(env, param, argv, argc));

        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_null(_, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_TRUE(ability->BuildFileInfoNumParam(env, param, argv, argc));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_BuildFileInfoNumParam_0001";
}