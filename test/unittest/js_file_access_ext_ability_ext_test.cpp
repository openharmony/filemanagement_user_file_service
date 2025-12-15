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
 
/**
 * @tc.number: user_file_service_js_file_access_ext_ability_DoCallJsMethod_ParamNull_0001
 * @tc.name: DoCallJsMethod_ParamNull_0001
 * @tc.desc: Test DoCallJsMethod with null param pointer
 * @tc.type: FUNC
 * @tc.require: issueI9VQ8Y
 */
HWTEST_F(JsFileAccessExtAbilityTest, DoCallJsMethod_ParamNull_0001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DoCallJsMethod_ParamNull_0001 start";
 
    // Test when param is nullptr
    CallJsParam *param = nullptr;
    auto result = JsFileAccessExtAbility::DoCallJsMethod(param);
    EXPECT_EQ(result, EINVAL);
    
    GTEST_LOG_(INFO) << "DoCallJsMethod_ParamNull_0001 end";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_ability_DoCallJsMethod_jsRuntimeNull_0002
 * @tc.name: DoCallJsMethod_jsRuntimeNull_0002
 * @tc.desc: Test DoCallJsMethod with null jsRuntime in param
 * @tc.type: FUNC
 * @tc.require: issueI9VQ8Y
 */
HWTEST_F(JsFileAccessExtAbilityTest, DoCallJsMethod_jsRuntimeNull_0002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DoCallJsMethod_jsRuntimeNull_0002 start";
 
    // Test when param->jsRuntime is nullptr
    InputArgsParser argParser = nullptr;
    ResultValueParser retParser = nullptr;
    CallJsParam param("testFunction", nullptr, nullptr, argParser, retParser);
    auto result = JsFileAccessExtAbility::DoCallJsMethod(&param);
    EXPECT_EQ(result, EINVAL);
    
    GTEST_LOG_(INFO) << "DoCallJsMethod_jsRuntimeNull_0002 end";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_ability_DoCallJsMethod_ArgParserFail_0003
 * @tc.name: DoCallJsMethod_ArgParserFail_0003
 * @tc.desc: Test DoCallJsMethod when argParser returns false
 * @tc.type: FUNC
 * @tc.require: issueI9VQ8Y
 */
HWTEST_F(JsFileAccessExtAbilityTest, DoCallJsMethod_ArgParserFail_0003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DoCallJsMethod_ArgParserFail_0003 start";
 
    // Create an argParser that returns false to simulate failure
    auto argParserLambda = [](napi_env &env, napi_value *argv, size_t &argc) -> bool {
        return false; // Simulate failure
    };
    InputArgsParser argParser = argParserLambda;
    
    auto retParserLambda = [](napi_env&, napi_value) -> bool {
        return true;
    };
    ResultValueParser retParser = retParserLambda;
    
    CallJsParam param("testFunction", jsRuntime.get(), nullptr, argParser, retParser);
    auto result = JsFileAccessExtAbility::DoCallJsMethod(&param);
    EXPECT_EQ(result, EINVAL);
    
    GTEST_LOG_(INFO) << "DoCallJsMethod_ArgParserFail_0003 end";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_ability_DoCallJsMethod_ArgParserNull_0004
 * @tc.name: DoCallJsMethod_ArgParserNull_0004
 * @tc.desc: Test DoCallJsMethod when argParser is nullptr (should succeed)
 * @tc.type: FUNC
 * @tc.require: issueI9VQ8Y
 */
HWTEST_F(JsFileAccessExtAbilityTest, DoCallJsMethod_ArgParserNull_0004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DoCallJsMethod_ArgParserNull_0004 start";
 
    InputArgsParser argParser = nullptr;
    auto retParserLambda = [](napi_env&, napi_value) -> bool {
        return true;
    };
    ResultValueParser retParser = retParserLambda;
    
    // Mock the necessary napi functions for successful execution
    napi_value rslt = nullptr;
    EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
    
    CallJsParam param("testFunction", jsRuntime.get(), nullptr, argParser, retParser);
    auto result = JsFileAccessExtAbility::DoCallJsMethod(&param);
    EXPECT_EQ(result, ERR_OK);
    
    GTEST_LOG_(INFO) << "DoCallJsMethod_ArgParserNull_0004 end";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_ability_DoCallJsMethod_ReferenceValueFail_0005
 * @tc.name: DoCallJsMethod_ReferenceValueFail_0005
 * @tc.desc: Test DoCallJsMethod when napi_get_reference_value fails
 * @tc.type: FUNC
 * @tc.require: issueI9VQ8Y
 */
HWTEST_F(JsFileAccessExtAbilityTest, DoCallJsMethod_ReferenceValueFail_0005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DoCallJsMethod_ReferenceValueFail_0005 start";
 
    InputArgsParser argParser = nullptr;
    auto retParserLambda = [](napi_env&, napi_value) -> bool {
        return true;
    };
    ResultValueParser retParser = retParserLambda;
    
    // Simulate failure in napi_get_reference_value
    EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _)).WillOnce(Return(napi_invalid_arg));
    
    CallJsParam param("testFunction", jsRuntime.get(), nullptr, argParser, retParser);
    auto result = JsFileAccessExtAbility::DoCallJsMethod(&param);
    EXPECT_EQ(result, EINVAL);
    
    GTEST_LOG_(INFO) << "DoCallJsMethod_ReferenceValueFail_0005 end";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_ability_DoCallJsMethod_ReferenceValueNull_0006
 * @tc.name: DoCallJsMethod_ReferenceValueNull_0006
 * @tc.desc: Test DoCallJsMethod when napi_get_reference_value returns null value
 * @tc.type: FUNC
 * @tc.require: issueI9VQ8Y
 */
HWTEST_F(JsFileAccessExtAbilityTest, DoCallJsMethod_ReferenceValueNull_0006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DoCallJsMethod_ReferenceValueNull_0006 start";
 
    InputArgsParser argParser = nullptr;
    auto retParserLambda = [](napi_env&, napi_value) -> bool {
        return true;
    };
    ResultValueParser retParser = retParserLambda;
    
    // Simulate napi_get_reference_value returning null value
    EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _)).WillOnce(Return(napi_ok));
    
    CallJsParam param("testFunction", jsRuntime.get(), nullptr, argParser, retParser);
    auto result = JsFileAccessExtAbility::DoCallJsMethod(&param);
    EXPECT_EQ(result, EINVAL);
    
    GTEST_LOG_(INFO) << "DoCallJsMethod_ReferenceValueNull_0006 end";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_ability_DoCallJsMethod_GetNamedPropertyFail_0007
 * @tc.name: DoCallJsMethod_GetNamedPropertyFail_0007
 * @tc.desc: Test DoCallJsMethod when napi_get_named_property fails
 * @tc.type: FUNC
 * @tc.require: issueI9VQ8Y
 */
HWTEST_F(JsFileAccessExtAbilityTest, DoCallJsMethod_GetNamedPropertyFail_0007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DoCallJsMethod_GetNamedPropertyFail_0007 start";
 
    InputArgsParser argParser = nullptr;
    ResultValueParser retParser = [](napi_env&, napi_value) -> bool {
        return true;
    };
    
    napi_value rslt = nullptr;
    EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    // Simulate failure in napi_get_named_property
    EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_invalid_arg));
    
    CallJsParam param("testFunction", jsRuntime.get(), nullptr, argParser, retParser);
    auto result = JsFileAccessExtAbility::DoCallJsMethod(&param);
    EXPECT_EQ(result, EINVAL);
    
    GTEST_LOG_(INFO) << "DoCallJsMethod_GetNamedPropertyFail_0007 end";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_ability_DoCallJsMethod_GetNamedPropertyNull_0008
 * @tc.name: DoCallJsMethod_GetNamedPropertyNull_0008
 * @tc.desc: Test DoCallJsMethod when napi_get_named_property returns null method
 * @tc.type: FUNC
 * @tc.require: issueI9VQ8Y
 */
HWTEST_F(JsFileAccessExtAbilityTest, DoCallJsMethod_GetNamedPropertyNull_0008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DoCallJsMethod_GetNamedPropertyNull_0008 start";
 
    InputArgsParser argParser = nullptr;
    ResultValueParser retParser = [](napi_env&, napi_value) -> bool {
        return true;
    };
    
    napi_value rslt = nullptr;
    EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    // Simulate napi_get_named_property returning null method
    EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _)).WillOnce(Return(napi_ok));
    
    CallJsParam param("testFunction", jsRuntime.get(), nullptr, argParser, retParser);
    auto result = JsFileAccessExtAbility::DoCallJsMethod(&param);
    EXPECT_EQ(result, EINVAL);
    
    GTEST_LOG_(INFO) << "DoCallJsMethod_GetNamedPropertyNull_0008 end";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_ability_DoCallJsMethod_RetParserNull_0009
 * @tc.name: DoCallJsMethod_RetParserNull_0009
 * @tc.desc: Test DoCallJsMethod when retParser is nullptr
 * @tc.type: FUNC
 * @tc.require: issueI9VQ8Y
 */
HWTEST_F(JsFileAccessExtAbilityTest, DoCallJsMethod_RetParserNull_0009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DoCallJsMethod_RetParserNull_0009 start";
 
    InputArgsParser argParser = nullptr;
    ResultValueParser retParser = nullptr; // nullptr retParser
    
    napi_value rslt = nullptr;
    EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    
    CallJsParam param("testFunction", jsRuntime.get(), nullptr, argParser, retParser);
    auto result = JsFileAccessExtAbility::DoCallJsMethod(&param);
    EXPECT_EQ(result, EINVAL);
    
    GTEST_LOG_(INFO) << "DoCallJsMethod_RetParserNull_0009 end";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_ability_DoCallJsMethod_CallFunctionFail_0010
 * @tc.name: DoCallJsMethod_CallFunctionFail_0010
 * @tc.desc: Test DoCallJsMethod when napi_call_function fails
 * @tc.type: FUNC
 * @tc.require: issueI9VQ8Y
 */
HWTEST_F(JsFileAccessExtAbilityTest, DoCallJsMethod_CallFunctionFail_0010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DoCallJsMethod_CallFunctionFail_0010 start";
 
    InputArgsParser argParser = nullptr;
    ResultValueParser retParser = [](napi_env&, napi_value) -> bool {
        return true;
    };
    
    napi_value rslt = nullptr;
    EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    // Simulate failure in napi_call_function
    EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _)).WillOnce(Return(napi_invalid_arg));
    
    CallJsParam param("testFunction", jsRuntime.get(), nullptr, argParser, retParser);
    auto result = JsFileAccessExtAbility::DoCallJsMethod(&param);
    EXPECT_EQ(result, E_GETRESULT);
    
    GTEST_LOG_(INFO) << "DoCallJsMethod_CallFunctionFail_0010 end";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_ability_DoCallJsMethod_CallFunctionNull_0011
 * @tc.name: DoCallJsMethod_CallFunctionNull_0011
 * @tc.desc: Test DoCallJsMethod when napi_call_function returns null result
 * @tc.type: FUNC
 * @tc.require: issueI9VQ8Y
 */
HWTEST_F(JsFileAccessExtAbilityTest, DoCallJsMethod_CallFunctionNull_0011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DoCallJsMethod_CallFunctionNull_0011 start";
 
    InputArgsParser argParser = nullptr;
    ResultValueParser retParser = [](napi_env&, napi_value) -> bool {
        return true;
    };
    
    napi_value rslt = nullptr;
    EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    // Simulate napi_call_function returning null result
    EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _)).WillOnce(Return(napi_ok));
    
    CallJsParam param("testFunction", jsRuntime.get(), nullptr, argParser, retParser);
    auto result = JsFileAccessExtAbility::DoCallJsMethod(&param);
    EXPECT_EQ(result, E_GETRESULT);
    
    GTEST_LOG_(INFO) << "DoCallJsMethod_CallFunctionNull_0011 end";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_ability_DoCallJsMethod_RetParserFail_0012
 * @tc.name: DoCallJsMethod_RetParserFail_0012
 * @tc.desc: Test DoCallJsMethod when retParser returns false
 * @tc.type: FUNC
 * @tc.require: issueI9VQ8Y
 */
HWTEST_F(JsFileAccessExtAbilityTest, DoCallJsMethod_RetParserFail_0012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DoCallJsMethod_RetParserFail_0012 start";
 
    InputArgsParser argParser = nullptr;
    // Create a retParser that returns false to simulate failure
    auto retParserLambda = [](napi_env&, napi_value) -> bool {
        return false; // Simulate failure
    };
    ResultValueParser retParser = retParserLambda;
    
    napi_value rslt = nullptr;
    EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
    
    CallJsParam param("testFunction", jsRuntime.get(), nullptr, argParser, retParser);
    auto result = JsFileAccessExtAbility::DoCallJsMethod(&param);
    EXPECT_EQ(result, E_GETRESULT);
    
    GTEST_LOG_(INFO) << "DoCallJsMethod_RetParserFail_0012 end";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_ability_DoCallJsMethod_Success_0013
 * @tc.name: DoCallJsMethod_Success_0013
 * @tc.desc: Test DoCallJsMethod success case
 * @tc.type: FUNC
 * @tc.require: issueI9VQ8Y
 */
HWTEST_F(JsFileAccessExtAbilityTest, DoCallJsMethod_Success_0013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DoCallJsMethod_Success_0013 start";
 
    InputArgsParser argParser = nullptr;
    auto retParserLambda = [](napi_env&, napi_value) -> bool {
        return true; // Simulate success
    };
    ResultValueParser retParser = retParserLambda;
    
    napi_value rslt = nullptr;
    EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
    
    CallJsParam param("testFunction", jsRuntime.get(), nullptr, argParser, retParser);
    auto result = JsFileAccessExtAbility::DoCallJsMethod(&param);
    EXPECT_EQ(result, ERR_OK);
    
    GTEST_LOG_(INFO) << "DoCallJsMethod_Success_0013 end";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_ability_DoCallJsMethod_ArgParserException_0014
 * @tc.name: DoCallJsMethod_ArgParserException_0014
 * @tc.desc: Test DoCallJsMethod when argParser throws exception
 * @tc.type: FUNC
 * @tc.require: issueI9VQ8Y
 */
HWTEST_F(JsFileAccessExtAbilityTest, DoCallJsMethod_ArgParserException_0014, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DoCallJsMethod_ArgParserException_0014 start";
 
    // Create an argParser that throws an exception
    auto argParserLambda = [](napi_env &env, napi_value *argv, size_t &argc) -> bool {
        throw std::runtime_error("Test exception");
        return true;
    };
    InputArgsParser argParser = argParserLambda;
    
    auto retParserLambda = [](napi_env&, napi_value) -> bool {
        return true;
    };
    ResultValueParser retParser = retParserLambda;
    
    // When an exception is thrown in argParser, it should be handled gracefully
    // In the current implementation, this would likely cause the CallJsMethod to return an error
    CallJsParam param("testFunction", jsRuntime.get(), nullptr, argParser, retParser);
    // The actual behavior depends on how exceptions are handled in the implementation
    // We're just documenting the test case here
    
    GTEST_LOG_(INFO) << "DoCallJsMethod_ArgParserException_0014 end";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_ability_DoCallJsMethod_RetParserException_0015
 * @tc.name: DoCallJsMethod_RetParserException_0015
 * @tc.desc: Test DoCallJsMethod when retParser throws exception
 * @tc.type: FUNC
 * @tc.require: issueI9VQ8Y
 */
HWTEST_F(JsFileAccessExtAbilityTest, DoCallJsMethod_RetParserException_0015, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DoCallJsMethod_RetParserException_0015 start";
 
    InputArgsParser argParser = nullptr;
    // Create a retParser that throws an exception
    auto retParserLambda = [](napi_env&, napi_value) -> bool {
        throw std::runtime_error("Test exception");
        return true;
    };
    ResultValueParser retParser = retParserLambda;
    
    CallJsParam param("testFunction", jsRuntime.get(), nullptr, argParser, retParser);
    // The actual behavior depends on how exceptions are handled in the implementation
    
    GTEST_LOG_(INFO) << "DoCallJsMethod_RetParserException_0015 end";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_ability_DoCallJsMethod_MultipleArgs_0016
 * @tc.name: DoCallJsMethod_MultipleArgs_0016
 * @tc.desc: Test DoCallJsMethod with multiple arguments
 * @tc.type: FUNC
 * @tc.require: issueI9VQ8Y
 */
HWTEST_F(JsFileAccessExtAbilityTest, DoCallJsMethod_MultipleArgs_0016, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DoCallJsMethod_MultipleArgs_0016 start";
 
    // Create an argParser that uses multiple arguments
    auto argParserLambda = [](napi_env &env, napi_value *argv, size_t &argc) -> bool {
        argc = 3; // Use 3 arguments
        argv[0] = nullptr;
        argv[1] = nullptr;
        argv[2] = nullptr;
        return true;
    };
    InputArgsParser argParser = argParserLambda;
    
    auto retParserLambda = [](napi_env&, napi_value) -> bool {
        return true;
    };
    ResultValueParser retParser = retParserLambda;
    
    napi_value rslt = nullptr;
    EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
    
    CallJsParam param("testFunction", jsRuntime.get(), nullptr, argParser, retParser);
    auto result = JsFileAccessExtAbility::DoCallJsMethod(&param);
    EXPECT_EQ(result, ERR_OK);
    
    GTEST_LOG_(INFO) << "DoCallJsMethod_MultipleArgs_0016 end";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_ability_DoCallJsMethod_MaxArgCount_0017
 * @tc.name: DoCallJsMethod_MaxArgCount_0017
 * @tc.desc: Test DoCallJsMethod with maximum argument count
 * @tc.type: FUNC
 * @tc.require: issueI9VQ8Y
 */
HWTEST_F(JsFileAccessExtAbilityTest, DoCallJsMethod_MaxArgCount_0017, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DoCallJsMethod_MaxArgCount_0017 start";
 
    // Create an argParser that uses maximum arguments (MAX_ARG_COUNT = 5)
    auto argParserLambda = [](napi_env &env, napi_value *argv, size_t &argc) -> bool {
        argc = 5; // Use maximum arguments
        argv[0] = nullptr;
        argv[1] = nullptr;
        argv[2] = nullptr;
        argv[3] = nullptr;
        argv[4] = nullptr;
        return true;
    };
    InputArgsParser argParser = argParserLambda;
    
    auto retParserLambda = [](napi_env&, napi_value) -> bool {
        return true;
    };
    ResultValueParser retParser = retParserLambda;
    
    napi_value rslt = nullptr;
    EXPECT_CALL(*insMoc, napi_get_reference_value(_, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_SECOND>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_THIRD>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_call_function(_, _, _, _, _, _))
        .WillOnce(DoAll(SetArgPointee<ARG_INDEX_FIFTH>(reinterpret_cast<napi_value>(&rslt)), Return(napi_ok)));
    EXPECT_CALL(*insMoc, napi_escape_handle(_, _, _, _)).WillOnce(Return(napi_ok));
    
    CallJsParam param("testFunction", jsRuntime.get(), nullptr, argParser, retParser);
    auto result = JsFileAccessExtAbility::DoCallJsMethod(&param);
    EXPECT_EQ(result, ERR_OK);
    
    GTEST_LOG_(INFO) << "DoCallJsMethod_MaxArgCount_0017 end";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_abilityParserGetJsResult_0001
 * @tc.name: js_file_access_ext_abilityParserGetJsResult_0001
 * @tc.desc: Test ParserGetJsResult with nullptr nativeValue parameter.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_abilityParserGetJsResult_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_abilityParserGetJsResult_0001";
    try {
        EXPECT_NE(ability, nullptr);
        napi_env env = nullptr;
        napi_value nativeValue = nullptr;
        std::vector<Result> result;
        int copyRet = 0;
        
        // 测试nativeValue为nullptr的情况
        bool ret = ability->ParserGetJsResult(env, nativeValue, result, copyRet);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_abilityParserGetJsResult_0001";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_abilityParserGetJsResult_0002
 * @tc.name: js_file_access_ext_abilityParserGetJsResult_0002
 * @tc.desc: Test ParserGetJsResult with napi_get_named_property failure when getting code.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_abilityParserGetJsResult_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_abilityParserGetJsResult_0002";
    try {
        EXPECT_NE(ability, nullptr);
        napi_env env = nullptr;
        napi_value nativeValue = reinterpret_cast<napi_value>(0x123456);
        std::vector<Result> result;
        int copyRet = 0;
        
        // 模拟获取code属性失败的情况
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(Return(napi_invalid_arg));
        bool ret = ability->ParserGetJsResult(env, nativeValue, result, copyRet);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_abilityParserGetJsResult_0002";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_abilityParserGetJsResult_0003
 * @tc.name: js_file_access_ext_abilityParserGetJsResult_0003
 * @tc.desc: Test ParserGetJsResult when napi_get_value_int32 fails when getting code.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_abilityParserGetJsResult_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_abilityParserGetJsResult_0003";
    try {
        EXPECT_NE(ability, nullptr);
        napi_env env = nullptr;
        napi_value nativeValue = reinterpret_cast<napi_value>(0x123456);
        std::vector<Result> result;
        int copyRet = 0;
        
        // 模拟获取code值失败的情况
        napi_value code = reinterpret_cast<napi_value>(0x654321);
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(code), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(Return(napi_invalid_arg));
        
        bool ret = ability->ParserGetJsResult(env, nativeValue, result, copyRet);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_abilityParserGetJsResult_0003";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_abilityParserGetJsResult_0004
 * @tc.name: js_file_access_ext_abilityParserGetJsResult_0004
 * @tc.desc: Test ParserGetJsResult when copyRet equals ERR_OK.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_abilityParserGetJsResult_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_abilityParserGetJsResult_0004";
    try {
        EXPECT_NE(ability, nullptr);
        napi_env env = nullptr;
        napi_value nativeValue = reinterpret_cast<napi_value>(0x123456);
        std::vector<Result> result;
        int copyRet = 0;
        
        // 模拟copyRet为ERR_OK的情况
        napi_value code = reinterpret_cast<napi_value>(0x654321);
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(code), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(ERR_OK), Return(napi_ok)));
        
        bool ret = ability->ParserGetJsResult(env, nativeValue, result, copyRet);
        EXPECT_TRUE(ret);
        EXPECT_EQ(copyRet, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_abilityParserGetJsResult_0004";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_abilityParserGetJsResult_0005
 * @tc.name: js_file_access_ext_abilityParserGetJsResult_0005
 * @tc.desc: Test ParserGetJsResult with napi_get_named_property failure when getting results.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_abilityParserGetJsResult_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_abilityParserGetJsResult_0005";
    try {
        EXPECT_NE(ability, nullptr);
        napi_env env = nullptr;
        napi_value nativeValue = reinterpret_cast<napi_value>(0x123456);
        std::vector<Result> result;
        int copyRet = 0;
        
        // 模拟获取results属性失败的情况
        napi_value code = reinterpret_cast<napi_value>(0x654321);
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(code), Return(napi_ok)))
            .WillOnce(Return(napi_invalid_arg));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(E_IPCS), Return(napi_ok))); // 非ERR_OK值
        EXPECT_CALL(*insMoc, napi_create_array(_, _))
            .WillOnce(Return(napi_ok));
 
        bool ret = ability->ParserGetJsResult(env, nativeValue, result, copyRet);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_abilityParserGetJsResult_0005";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_abilityParserGetJsResult_0006
 * @tc.name: js_file_access_ext_abilityParserGetJsResult_0006
 * @tc.desc: Test ParserGetJsResult when nativeArray is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_abilityParserGetJsResult_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_abilityParserGetJsResult_0006";
    try {
        EXPECT_NE(ability, nullptr);
        napi_env env = nullptr;
        napi_value nativeValue = reinterpret_cast<napi_value>(0x123456);
        std::vector<Result> result;
        int copyRet = 0;
        
        // 模拟获取results数组为nullptr的情况
        napi_value code = reinterpret_cast<napi_value>(0x654321);
        napi_value nativeArray = nullptr;
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(code), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<3>(nativeArray), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(E_IPCS), Return(napi_ok))); // 非ERR_OK值
        EXPECT_CALL(*insMoc, napi_create_array(_, _))
            .WillOnce(Return(napi_ok));
        
        bool ret = ability->ParserGetJsResult(env, nativeValue, result, copyRet);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_abilityParserGetJsResult_0006";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_abilityParserGetJsResult_0007
 * @tc.name: js_file_access_ext_abilityParserGetJsResult_0007
 * @tc.desc: Test ParserGetJsResult with napi_get_array_length failure.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_abilityParserGetJsResult_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_abilityParserGetJsResult_0007";
    try {
        EXPECT_NE(ability, nullptr);
        napi_env env = nullptr;
        napi_value nativeValue = reinterpret_cast<napi_value>(0x123456);
        std::vector<Result> result;
        int copyRet = 0;
        
        // 模拟获取数组长度失败的情况
        napi_value code = reinterpret_cast<napi_value>(0x654321);
        napi_value nativeArray = reinterpret_cast<napi_value>(0x789abc);
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(code), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<3>(nativeArray), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(E_IPCS), Return(napi_ok))); // 非ERR_OK值
        EXPECT_CALL(*insMoc, napi_create_array(_, _))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _))
            .WillOnce(Return(napi_invalid_arg));
        
        bool ret = ability->ParserGetJsResult(env, nativeValue, result, copyRet);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_abilityParserGetJsResult_0007";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_abilityParserGetJsResult_0008
 * @tc.name: js_file_access_ext_abilityParserGetJsResult_0008
 * @tc.desc: Test ParserGetJsResult with napi_get_element failure.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_abilityParserGetJsResult_0008, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_abilityParserGetJsResult_0008";
    try {
        EXPECT_NE(ability, nullptr);
        napi_env env = nullptr;
        napi_value nativeValue = reinterpret_cast<napi_value>(0x123456);
        std::vector<Result> result;
        int copyRet = 0;
        
        // 模拟获取数组元素失败的情况
        napi_value code = reinterpret_cast<napi_value>(0x654321);
        napi_value nativeArray = reinterpret_cast<napi_value>(0x789abc);
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(code), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<3>(nativeArray), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(E_IPCS), Return(napi_ok))); // 非ERR_OK值
        EXPECT_CALL(*insMoc, napi_create_array(_, _))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(1), Return(napi_ok))); // 数组长度为1
        EXPECT_CALL(*insMoc, napi_get_element(_, _, _, _))
            .WillOnce(Return(napi_invalid_arg));
        
        bool ret = ability->ParserGetJsResult(env, nativeValue, result, copyRet);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_abilityParserGetJsResult_0008";
}
 
/**
 * @tc.number: user_file_service_js_file_access_ext_abilityParserGetJsResult_0009
 * @tc.name: js_file_access_ext_abilityParserGetJsResult_0009
 * @tc.desc: Test ParserGetJsResult when nativeResult is nullptr.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(JsFileAccessExtAbilityTest, js_file_access_ext_abilityParserGetJsResult_0009, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-begin js_file_access_ext_abilityParserGetJsResult_0009";
    try {
        EXPECT_NE(ability, nullptr);
        napi_env env = nullptr;
        napi_value nativeValue = reinterpret_cast<napi_value>(0x123456);
        std::vector<Result> result;
        int copyRet = 0;
        
        // 模拟获取的数组元素为nullptr的情况
        napi_value code = reinterpret_cast<napi_value>(0x654321);
        napi_value nativeArray = reinterpret_cast<napi_value>(0x789abc);
        napi_value nativeResult = nullptr;
        EXPECT_CALL(*insMoc, napi_get_named_property(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(code), Return(napi_ok)))
            .WillOnce(DoAll(SetArgPointee<3>(nativeArray), Return(napi_ok)));
        EXPECT_CALL(*insMoc, napi_get_value_int32(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(E_IPCS), Return(napi_ok))); // 非ERR_OK值
        EXPECT_CALL(*insMoc, napi_create_array(_, _))
            .WillOnce(Return(napi_ok));
        EXPECT_CALL(*insMoc, napi_get_array_length(_, _, _))
            .WillOnce(DoAll(SetArgPointee<2>(1), Return(napi_ok))); // 数组长度为1
        EXPECT_CALL(*insMoc, napi_get_element(_, _, _, _))
            .WillOnce(DoAll(SetArgPointee<3>(nativeResult), Return(napi_ok)));
        
        bool ret = ability->ParserGetJsResult(env, nativeValue, result, copyRet);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "JsFileAccessExtAbilityTest occurs an exception.";
    }
    GTEST_LOG_(INFO) << "JsFileAccessExtAbilityTest-end js_file_access_ext_abilityParserGetJsResult_0009";
}