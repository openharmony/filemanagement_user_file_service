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
 * @tc.number: user_file_service_js_file_access_ext_ability_ListFile_0000
 * @tc.name: js_file_access_ext_ability_ListFile_0000
 * @tc.desc: Test function of ListFile interface for uri、nativeOffset or nativeMaxCount failure branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ListFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ListFile_0000";
    try {
        napi_value rslt = nullptr;
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        vector<FileInfo> fileInfoVec;
        EXPECT_NE(ability, nullptr);

        // 模拟获取uri为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取nativeOffset为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int64(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取nativeMaxCount为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int64(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ListFile_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ListFile_0001
 * @tc.name: js_file_access_ext_ability_ListFile_0001
 * @tc.desc: Test function of ListFile interface for nativeFilter failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ListFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ListFile_0001";
    try {
        napi_value rslt = nullptr;
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        vector<FileInfo> fileInfoVec;
        EXPECT_NE(ability, nullptr);

        // 模拟获取nativeFilter为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int64(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_null(_, _)).WillOnce(Return(napi_ok));
        auto result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取nativeFilter成功
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int64(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_null(_, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ListFile_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ListFile_0002
 * @tc.name: js_file_access_ext_ability_ListFile_0002
 * @tc.desc: Test function of ListFile interface for nativeFilter or fileFilter.suffixArray
 *           failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ListFile_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ListFile_0002";
    try {
        napi_value rslt = nullptr;
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        filter.SetHasFilter(true);
        vector<FileInfo> fileInfoVec;
        EXPECT_NE(ability, nullptr);

        // 模拟创建nativeFilter失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int64(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_object(_, _)).WillOnce(Return(napi_ok));
        auto result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟设置fileFilter.suffixArray失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int64(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_object(_, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_array_with_length(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ListFile_0002";
}

static void CallListFileHeadSucceed(shared_ptr<AssistantMock> insMoc)
{
    napi_value rslt = nullptr;
    EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
    EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_create_int64(_, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_create_object(_, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ListFile_0003
 * @tc.name: js_file_access_ext_ability_ListFile_0003
 * @tc.desc: Test function of ListFile interface for fileFilter.displayNameArray and fileFilter.mimeTypeArray
 *           failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ListFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ListFile_0003";
    try {
        napi_value rslt = nullptr;
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        filter.SetHasFilter(true);
        vector<FileInfo> fileInfoVec;
        EXPECT_NE(ability, nullptr);

        // 模拟获取fileFilter.displayNameArray失败
        CallListFileHeadSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_create_array_with_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        auto result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取fileFilter.mimeTypeArray失败
        CallListFileHeadSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_create_array_with_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ListFile_0003";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ListFile_0004
 * @tc.name: js_file_access_ext_ability_ListFile_0004
 * @tc.desc: Test function of ListFile interface for fileFilter.nativeFileSizeOver and
 *           fileFilter.nativeLastModifiedAfter failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ListFile_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ListFile_0004";
    try {
        napi_value rslt = nullptr;
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        filter.SetHasFilter(true);
        vector<FileInfo> fileInfoVec;
        EXPECT_NE(ability, nullptr);

        // 模拟获取fileFilter.nativeFileSizeOver失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int64(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_object(_, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_array_with_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        auto result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取fileFilter.nativeLastModifiedAfter失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int64(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_object(_, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_array_with_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_double(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ListFile_0004";
}

static void CallCreateNativeValueSucceed(shared_ptr<AssistantMock> insMoc)
{
    // 模拟调用CreateNativeValue成功
    napi_value rslt = nullptr;
    EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
    EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_create_int64(_, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
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
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ListFile_0005
 * @tc.name: js_file_access_ext_ability_ListFile_0005
 * @tc.desc: Test function of ListFile interface for fileFilter.nativeExcludeMedia、fileFilter.suffixArray and
 *           fileFilter.displayNameArray failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ListFile_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ListFile_0005";
    try {
        napi_value rslt = nullptr;
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        filter.SetHasFilter(true);
        vector<FileInfo> fileInfoVec;
        EXPECT_NE(ability, nullptr);

        // 模拟获取fileFilter.nativeExcludeMedia失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int64(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_object(_, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_array_with_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_double(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟设置fileFilter.suffixArray失败
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _)).WillOnce(Return(napi_invalid_arg));
        result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟设置fileFilter.displayNameArray失败
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_invalid_arg));
        result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ListFile_0005";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ListFile_0006
 * @tc.name: js_file_access_ext_ability_ListFile_0006
 * @tc.desc: Test function of ListFile interface for fileFilter.mimeTypeArray、fileFilter.nativeFileSizeOver
 *           fileFilter.nativeLastModifiedAfter and fileFilter.nativeExcludeMedia failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ListFile_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ListFile_0006";
    try {
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        filter.SetHasFilter(true);
        vector<FileInfo> fileInfoVec;
        EXPECT_NE(ability, nullptr);

        // 模拟设置fileFilter.mimeTypeArray失败
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_invalid_arg));
        auto result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟设置fileFilter.nativeFileSizeOver失败
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_invalid_arg));
        result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟设置fileFilter.nativeLastModifiedAfter失败
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_invalid_arg));
        result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟设置fileFilter.nativeExcludeMedia失败
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_invalid_arg));
        result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ListFile_0006";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ListFile_0007
 * @tc.name: js_file_access_ext_ability_ListFile_0007
 * @tc.desc: Test function of ListFile interface for code、nativeArray failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ListFile_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ListFile_0007";
    try {
        napi_value rslt = nullptr;
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        filter.SetHasFilter(true);
        vector<FileInfo> fileInfoVec;
        EXPECT_NE(ability, nullptr);

        // 模拟获取code失败
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取nativeArray失败
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ListFile_0007";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ListFile_0008
 * @tc.name: js_file_access_ext_ability_ListFile_0008
 * @tc.desc: Test function of ListFile interface for length failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ListFile_0008, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ListFile_0008";
    try {
        napi_value rslt = nullptr;
        FileInfo fileInfo;
        FileFilter filter;
        filter.SetHasFilter(true);
        vector<FileInfo> fileInfoVec;
        EXPECT_NE(ability, nullptr);

        // 模拟获取length失败
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->ListFile(fileInfo, 0, 0, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取length成功
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->ListFile(fileInfo, 0, 0, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ListFile_0008";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ListFile_0009
 * @tc.name: js_file_access_ext_ability_ListFile_0009
 * @tc.desc: Test function of ListFile interface for CallJsMethod and value->code failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ListFile_0009, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ListFile_0009";
    try {
        int status = napi_invalid_arg;
        napi_value rslt = nullptr;
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        filter.SetHasFilter(true);
        vector<FileInfo> fileInfoVec;
        EXPECT_NE(ability, nullptr);

        // 模拟调用CallJsMethod失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, EINVAL);

        // 模拟获取value->code失败
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(status), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->ListFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, napi_invalid_arg);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ListFile_0009";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ScanFile_0000
 * @tc.name: js_file_access_ext_ability_ScanFile_0000
 * @tc.desc: Test function of ScanFile interface for uri、nativeOffset or nativeMaxCount failure branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ScanFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ScanFile_0000";
    try {
        napi_value rslt = nullptr;
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        vector<FileInfo> fileInfoVec;
        EXPECT_NE(ability, nullptr);

        // 模拟获取uri为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取nativeOffset为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int64(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取nativeMaxCount为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int64(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ScanFile_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ScanFile_0001
 * @tc.name: js_file_access_ext_ability_ScanFile_0001
 * @tc.desc: Test function of ScanFile interface for nativeFilter failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ScanFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ScanFile_0001";
    try {
        napi_value rslt = nullptr;
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        vector<FileInfo> fileInfoVec;
        EXPECT_NE(ability, nullptr);

        // 模拟获取nativeFilter为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int64(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_null(_, _)).WillOnce(Return(napi_ok));
        auto result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取nativeFilter成功
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int64(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_null(_, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ScanFile_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ScanFile_0002
 * @tc.name: js_file_access_ext_ability_ScanFile_0002
 * @tc.desc: Test function of ScanFile interface for nativeFilter or fileFilter.suffixArray
 *           failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ScanFile_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ScanFile_0002";
    try {
        napi_value rslt = nullptr;
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        filter.SetHasFilter(true);
        vector<FileInfo> fileInfoVec;
        EXPECT_NE(ability, nullptr);

        // 模拟创建nativeFilter失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int64(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_object(_, _)).WillOnce(Return(napi_ok));
        auto result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟设置fileFilter.suffixArray失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int64(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_object(_, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_array_with_length(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ScanFile_0002";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ScanFile_0003
 * @tc.name: js_file_access_ext_ability_ScanFile_0003
 * @tc.desc: Test function of ScanFile interface for fileFilter.displayNameArray and fileFilter.mimeTypeArray
 *           failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ScanFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ScanFile_0003";
    try {
        napi_value rslt = nullptr;
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        filter.SetHasFilter(true);
        vector<FileInfo> fileInfoVec;
        EXPECT_NE(ability, nullptr);

        // 模拟获取fileFilter.displayNameArray失败
        CallListFileHeadSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_create_array_with_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        auto result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取fileFilter.mimeTypeArray失败
        CallListFileHeadSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_create_array_with_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ScanFile_0003";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ScanFile_0004
 * @tc.name: js_file_access_ext_ability_ScanFile_0004
 * @tc.desc: Test function of ScanFile interface for fileFilter.nativeFileSizeOver and
 *           fileFilter.nativeLastModifiedAfter failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ScanFile_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ScanFile_0004";
    try {
        napi_value rslt = nullptr;
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        filter.SetHasFilter(true);
        vector<FileInfo> fileInfoVec;
        EXPECT_NE(ability, nullptr);

        // 模拟获取fileFilter.nativeFileSizeOver失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int64(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_object(_, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_array_with_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        auto result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取fileFilter.nativeLastModifiedAfter失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int64(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_object(_, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_array_with_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_double(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ScanFile_0004";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ScanFile_0005
 * @tc.name: js_file_access_ext_ability_ScanFile_0005
 * @tc.desc: Test function of ScanFile interface for fileFilter.nativeExcludeMedia、fileFilter.suffixArray and
 *           fileFilter.displayNameArray failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ScanFile_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ScanFile_0005";
    try {
        napi_value rslt = nullptr;
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        filter.SetHasFilter(true);
        vector<FileInfo> fileInfoVec;
        EXPECT_NE(ability, nullptr);

        // 模拟获取fileFilter.nativeExcludeMedia失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_int64(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_object(_, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIRST>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_array_with_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_double(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_boolean(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟设置fileFilter.suffixArray失败
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _)).WillOnce(Return(napi_invalid_arg));
        result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟设置fileFilter.displayNameArray失败
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_invalid_arg));
        result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ScanFile_0005";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ScanFile_0006
 * @tc.name: js_file_access_ext_ability_ScanFile_0006
 * @tc.desc: Test function of ScanFile interface for fileFilter.mimeTypeArray、fileFilter.nativeFileSizeOver
 *           fileFilter.nativeLastModifiedAfter and fileFilter.nativeExcludeMedia failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ScanFile_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ScanFile_0006";
    try {
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        filter.SetHasFilter(true);
        vector<FileInfo> fileInfoVec;
        EXPECT_NE(ability, nullptr);

        // 模拟设置fileFilter.mimeTypeArray失败
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_invalid_arg));
        auto result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟设置fileFilter.nativeFileSizeOver失败
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_invalid_arg));
        result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟设置fileFilter.nativeLastModifiedAfter失败
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_invalid_arg));
        result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟设置fileFilter.nativeExcludeMedia失败
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_invalid_arg));
        result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ScanFile_0006";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ScanFile_0007
 * @tc.name: js_file_access_ext_ability_ScanFile_0007
 * @tc.desc: Test function of ScanFile interface for code、nativeArray failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ScanFile_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ScanFile_0007";
    try {
        napi_value rslt = nullptr;
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        filter.SetHasFilter(true);
        vector<FileInfo> fileInfoVec;
        EXPECT_NE(ability, nullptr);

        // 模拟获取code失败
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取nativeArray失败
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ScanFile_0007";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ScanFile_0008
 * @tc.name: js_file_access_ext_ability_ScanFile_0008
 * @tc.desc: Test function of ScanFile interface for length failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ScanFile_0008, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ScanFile_0008";
    try {
        napi_value rslt = nullptr;
        FileInfo fileInfo;
        FileFilter filter;
        filter.SetHasFilter(true);
        vector<FileInfo> fileInfoVec;
        EXPECT_NE(ability, nullptr);

        // 模拟获取length失败
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->ScanFile(fileInfo, 0, 0, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取length成功
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->ScanFile(fileInfo, 0, 0, filter, fileInfoVec);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ScanFile_0008";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_ScanFile_0009
 * @tc.name: js_file_access_ext_ability_ScanFile_0009
 * @tc.desc: Test function of ScanFile interface for CallJsMethod and value->code failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_ScanFile_0009, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_ScanFile_0009";
    try {
        int status = napi_invalid_arg;
        napi_value rslt = nullptr;
        FileInfo fileInfo;
        int64_t offset = 0;
        int64_t maxCount = 0;
        FileFilter filter;
        filter.SetHasFilter(true);
        vector<FileInfo> fileInfoVec;
        EXPECT_NE(ability, nullptr);

        // 模拟调用CallJsMethod失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, EINVAL);

        // 模拟获取value->code失败
        CallCreateNativeValueSucceed(insMoc);
        EXPECT_CALL(*insMoc, napi_set_named_property(_, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(status), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_create_array(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _)).WillOnce(Return(napi_ok));
        result = ability->ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec);
        EXPECT_EQ(result, napi_invalid_arg);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_ScanFile_0009";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetFileInfoFromUri_0000
 * @tc.name: js_file_access_ext_ability_GetFileInfoFromUri_0000
 * @tc.desc: Test function of GetFileInfoFromUri interface for nativeUri and value->code failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetFileInfoFromUri_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetFileInfoFromUri_0000";
    try {
        napi_value rslt = nullptr;
        Uri selectFile("");
        FileInfo fileInfo;
        EXPECT_NE(ability, nullptr);

        // 模拟获取nativeUri为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->GetFileInfoFromUri(selectFile, fileInfo);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取code失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_invalid_arg));
        result = ability->GetFileInfoFromUri(selectFile, fileInfo);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetFileInfoFromUri_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetFileInfoFromUri_0001
 * @tc.name: js_file_access_ext_ability_GetFileInfoFromUri_0001
 * @tc.desc: Test function of GetFileInfoFromUri interface for fileInfo and fileInfo.uri failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetFileInfoFromUri_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetFileInfoFromUri_0001";
    try {
        napi_value rslt = nullptr;
        Uri selectFile("");
        FileInfo fileInfo;
        EXPECT_NE(ability, nullptr);

        // 模拟获取fileInfo失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->GetFileInfoFromUri(selectFile, fileInfo);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取fileInfo.uri失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        result = ability->GetFileInfoFromUri(selectFile, fileInfo);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetFileInfoFromUri_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetFileInfoFromUri_0002
 * @tc.name: js_file_access_ext_ability_GetFileInfoFromUri_0002
 * @tc.desc: Test function of GetFileInfoFromUri interface for fileInfo.relativePath and fileInfo.fileName
 *           failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetFileInfoFromUri_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetFileInfoFromUri_0002";
    try {
        napi_value rslt = nullptr;
        Uri selectFile("");
        FileInfo fileInfo;
        EXPECT_NE(ability, nullptr);

        // 模拟获取fileInfo.relativePath失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->GetFileInfoFromUri(selectFile, fileInfo);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取fileInfo.fileName失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_invalid_arg));
        result = ability->GetFileInfoFromUri(selectFile, fileInfo);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetFileInfoFromUri_0002";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetFileInfoFromUri_0003
 * @tc.name: js_file_access_ext_ability_GetFileInfoFromUri_0003
 * @tc.desc: Test function of GetFileInfoFromUri interface for fileInfo.mode failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetFileInfoFromUri_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetFileInfoFromUri_0003";
    try {
        napi_value rslt = nullptr;
        Uri selectFile("");
        FileInfo fileInfo;
        EXPECT_NE(ability, nullptr);

        // 模拟获取fileInfo.mode失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        auto result = ability->GetFileInfoFromUri(selectFile, fileInfo);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetFileInfoFromUri_0003";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetFileInfoFromUri_0004
 * @tc.name: js_file_access_ext_ability_GetFileInfoFromUri_0004
 * @tc.desc: Test function of GetFileInfoFromUri interface for fileInfo.size failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetFileInfoFromUri_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetFileInfoFromUri_0004";
    try {
        napi_value rslt = nullptr;
        Uri selectFile("");
        FileInfo fileInfo;
        EXPECT_NE(ability, nullptr);

        // 模拟获取fileInfo.size失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int64(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->GetFileInfoFromUri(selectFile, fileInfo);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetFileInfoFromUri_0004";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetFileInfoFromUri_0005
 * @tc.name: js_file_access_ext_ability_GetFileInfoFromUri_0005
 * @tc.desc: Test function of GetFileInfoFromUri interface for fileInfo.mtime failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetFileInfoFromUri_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetFileInfoFromUri_0005";
    try {
        napi_value rslt = nullptr;
        Uri selectFile("");
        FileInfo fileInfo;
        EXPECT_NE(ability, nullptr);

        // 模拟获取fileInfo.size失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int64(_, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->GetFileInfoFromUri(selectFile, fileInfo);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetFileInfoFromUri_0005";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetFileInfoFromUri_0006
 * @tc.name: js_file_access_ext_ability_GetFileInfoFromUri_0006
 * @tc.desc: Test function of GetFileInfoFromUri interface for fileInfo.mimeType failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetFileInfoFromUri_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetFileInfoFromUri_0006";
    try {
        napi_value rslt = nullptr;
        Uri selectFile("");
        FileInfo fileInfo;
        EXPECT_NE(ability, nullptr);

        // 模拟获取fileInfo.mimeType失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*insMoc, napi_get_value_int64(_, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        auto result = ability->GetFileInfoFromUri(selectFile, fileInfo);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetFileInfoFromUri_0006";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetFileInfoFromUri_0007
 * @tc.name: js_file_access_ext_ability_GetFileInfoFromUri_0007
 * @tc.desc: Test function of GetFileInfoFromUri interface for CallJsMethod and value->code failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetFileInfoFromUri_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetFileInfoFromUri_0007";
    try {
        int status = napi_invalid_arg;
        napi_value rslt = nullptr;
        Uri selectFile("");
        FileInfo fileInfo;
        EXPECT_NE(ability, nullptr);

        // 模拟调用CallJsMethod失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->GetFileInfoFromUri(selectFile, fileInfo);
        EXPECT_EQ(result, EINVAL);

        // 模拟获取value->code失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(status), Return(napi_ok))).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int64(_, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        result = ability->GetFileInfoFromUri(selectFile, fileInfo);
        EXPECT_EQ(result, napi_invalid_arg);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetFileInfoFromUri_0007";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetFileInfoFromRelativePath_0000
 * @tc.name: js_file_access_ext_ability_GetFileInfoFromRelativePath_0000
 * @tc.desc: Test function of GetFileInfoFromRelativePath interface for nativePath and value->code
 *           failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetFileInfoFromRelativePath_0000,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetFileInfoFromRelativePath_0000";
    try {
        napi_value rslt = nullptr;
        string selectFileRealtivePath;
        FileInfo fileInfo;
        EXPECT_NE(ability, nullptr);

        // 模拟获取nativePath为空
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->GetFileInfoFromRelativePath(selectFileRealtivePath, fileInfo);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取code失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_invalid_arg));
        result = ability->GetFileInfoFromRelativePath(selectFileRealtivePath, fileInfo);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetFileInfoFromRelativePath_0000";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetFileInfoFromRelativePath_0001
 * @tc.name: js_file_access_ext_ability_GetFileInfoFromRelativePath_0001
 * @tc.desc: Test function of GetFileInfoFromRelativePath interface for fileInfo and fileInfo.uri
 *           failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetFileInfoFromRelativePath_0001,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetFileInfoFromRelativePath_0001";
    try {
        napi_value rslt = nullptr;
        string selectFileRealtivePath;
        FileInfo fileInfo;
        EXPECT_NE(ability, nullptr);

        // 模拟获取fileInfo失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        auto result = ability->GetFileInfoFromRelativePath(selectFileRealtivePath, fileInfo);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取fileInfo.uri失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
        result = ability->GetFileInfoFromRelativePath(selectFileRealtivePath, fileInfo);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetFileInfoFromRelativePath_0001";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetFileInfoFromRelativePath_0002
 * @tc.name: js_file_access_ext_ability_GetFileInfoFromRelativePath_0002
 * @tc.desc: Test function of GetFileInfoFromRelativePath interface for fileInfo.relativePath and fileInfo.fileName
 *           failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetFileInfoFromRelativePath_0002,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetFileInfoFromRelativePath_0002";
    try {
        napi_value rslt = nullptr;
        string selectFileRealtivePath;
        FileInfo fileInfo;
        EXPECT_NE(ability, nullptr);

        // 模拟获取fileInfo.relativePath失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->GetFileInfoFromRelativePath(selectFileRealtivePath, fileInfo);
        EXPECT_EQ(result, ERR_OK);

        // 模拟获取fileInfo.fileName失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_invalid_arg));
        result = ability->GetFileInfoFromRelativePath(selectFileRealtivePath, fileInfo);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetFileInfoFromRelativePath_0002";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetFileInfoFromRelativePath_0003
 * @tc.name: js_file_access_ext_ability_GetFileInfoFromRelativePath_0003
 * @tc.desc: Test function of GetFileInfoFromRelativePath interface for fileInfo.mode failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetFileInfoFromRelativePath_0003,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetFileInfoFromRelativePath_0003";
    try {
        napi_value rslt = nullptr;
        string selectFileRealtivePath;
        FileInfo fileInfo;
        EXPECT_NE(ability, nullptr);

        // 模拟获取fileInfo.mode失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        auto result = ability->GetFileInfoFromRelativePath(selectFileRealtivePath, fileInfo);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetFileInfoFromRelativePath_0003";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetFileInfoFromRelativePath_0004
 * @tc.name: js_file_access_ext_ability_GetFileInfoFromRelativePath_0004
 * @tc.desc: Test function of GetFileInfoFromRelativePath interface for fileInfo.size failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetFileInfoFromRelativePath_0004,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetFileInfoFromRelativePath_0004";
    try {
        napi_value rslt = nullptr;
        string selectFileRealtivePath;
        FileInfo fileInfo;
        EXPECT_NE(ability, nullptr);

        // 模拟获取fileInfo.size失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int64(_, _, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->GetFileInfoFromRelativePath(selectFileRealtivePath, fileInfo);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetFileInfoFromRelativePath_0004";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetFileInfoFromRelativePath_0005
 * @tc.name: js_file_access_ext_ability_GetFileInfoFromRelativePath_0005
 * @tc.desc: Test function of GetFileInfoFromRelativePath interface for fileInfo.mtime failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetFileInfoFromRelativePath_0005,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetFileInfoFromRelativePath_0005";
    try {
        napi_value rslt = nullptr;
        string selectFileRealtivePath;
        FileInfo fileInfo;
        EXPECT_NE(ability, nullptr);

        // 模拟获取fileInfo.size失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int64(_, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->GetFileInfoFromRelativePath(selectFileRealtivePath, fileInfo);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetFileInfoFromRelativePath_0005";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetFileInfoFromRelativePath_0006
 * @tc.name: js_file_access_ext_ability_GetFileInfoFromRelativePath_0006
 * @tc.desc: Test function of GetFileInfoFromRelativePath interface for fileInfo.mimeType failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetFileInfoFromRelativePath_0006,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetFileInfoFromRelativePath_0006";
    try {
        napi_value rslt = nullptr;
        string selectFileRealtivePath;
        FileInfo fileInfo;
        EXPECT_NE(ability, nullptr);

        // 模拟获取fileInfo.mimeType失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*insMoc, napi_get_value_int64(_, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        auto result = ability->GetFileInfoFromRelativePath(selectFileRealtivePath, fileInfo);
        EXPECT_EQ(result, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetFileInfoFromRelativePath_0006";
}

/**
 * @tc.number: user_file_service_js_file_access_ext_ability_GetFileInfoFromRelativePath_0007
 * @tc.name: js_file_access_ext_ability_GetFileInfoFromRelativePath_0007
 * @tc.desc: Test function of GetFileInfoFromRelativePath interface for CallJsMethod and value->code
 *           failure/success branch.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 3
 * @tc.require: issuesI8ZE8T
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_ability_GetFileInfoFromRelativePath_0007,
    testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_ability_GetFileInfoFromRelativePath_0007";
    try {
        int status = napi_invalid_arg;
        napi_value rslt = nullptr;
        string selectFileRealtivePath;
        FileInfo fileInfo;
        EXPECT_NE(ability, nullptr);

        // 模拟调用CallJsMethod失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_invalid_arg));
        auto result = ability->GetFileInfoFromRelativePath(selectFileRealtivePath, fileInfo);
        EXPECT_EQ(result, EINVAL);

        // 模拟获取value->code失败
        EXPECT_CALL(*insMoc, napi_get_uv_event_loop(_, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, uv_queue_work(_, _, _, _)).WillOnce(Return(0));
        EXPECT_CALL(*insMoc, napi_create_string_utf8(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(status), Return(napi_ok))).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_string_utf8(_, _, _, _, _))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok))
            .WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_value_int64(_, _, _)).WillOnce(Return(napi_ok)).WillOnce(Return(napi_ok));
        result = ability->GetFileInfoFromRelativePath(selectFileRealtivePath, fileInfo);
        EXPECT_EQ(result, napi_invalid_arg);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_ability_GetFileInfoFromRelativePath_0007";
}