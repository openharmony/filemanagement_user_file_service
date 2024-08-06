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
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        auto result = ability->MoveFile(sourceFile, targetParent, fileName, newFile);
        EXPECT_EQ(result, E_GETRESULT);

        // 模拟获取dstUri为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        result = ability->MoveFile(sourceFile, targetParent, fileName, newFile);
        EXPECT_EQ(result, E_GETRESULT);

        // 模拟获取name为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
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
        EXPECT_EQ(result, E_IPCS);

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
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
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
        EXPECT_EQ(result, ERR_OK);
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
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->MoveItem(sourceFile, targetParent, moveResult, force);
        EXPECT_EQ(result, EXCEPTION);

        // 模拟获取dstNativeUri为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->MoveItem(sourceFile, targetParent, moveResult, force);
        EXPECT_EQ(result, EXCEPTION);

        // 模拟获取forceMove为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
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
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
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
        EXPECT_EQ(result, ERR_OK);
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