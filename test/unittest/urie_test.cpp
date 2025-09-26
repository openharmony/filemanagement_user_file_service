/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#define TEST_ACCESS private: public; protected: public
#include "uri_ext.h"
#undef TEST_ACCESS
using namespace OHOS;
using namespace OHOS::FileAccessFwk;
using namespace testing::ext;
namespace OHOS {
    namespace {
        const string EMPTY = "";
        const int PORT_NONE = -1;
    }
class UriTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<Urie> urie_ = nullptr;
};

void UriTest::SetUpTestCase(void)
{}

void UriTest::TearDownTestCase(void)
{}

void UriTest::SetUp(void)
{
    urie_ = std::make_shared<Urie>(" ");
}

void UriTest::TearDown(void)
{}

/**
 * @tc.number: Uri_GetSchemeSpecificPart_0100
 * @tc.name: GetSchemeSpecificPart
 * @tc.desc: Verify the function when the input string get string specific part.
 */
HWTEST_F(UriTest, Uri_GetSchemeSpecificPart_0100, Function | MediumTest | Level0)
{
    GTEST_LOG_(INFO) << "Uri_GetSchemeSpecificPart_0100 start";
    auto result = urie_->GetSchemeSpecificPart();
    EXPECT_EQ(result, " ");
    GTEST_LOG_(INFO) << "Uri_GetSchemeSpecificPart_0100 end";
}

/**
 * @tc.number: Uri_GetSchemeSpecificPart_0200
 * @tc.name: GetSchemeSpecificPart
 * @tc.desc: Verify the function when the input string get string specific part.
 */
HWTEST_F(UriTest, Uri_GetSchemeSpecificPart_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetSchemeSpecificPart_0200 start";
    urie_->uriString_ = "abc";
    auto result = urie_->GetSchemeSpecificPart();
    EXPECT_EQ(result, "abc");
    GTEST_LOG_(INFO) << "Uri_GetSchemeSpecificPart_0200 end";
}

/**
 * @tc.number: Uri_GetAuthority_0100
 * @tc.name: GetAuthority
 * @tc.desc: Verify the function to get authority.
 */
HWTEST_F(UriTest, Uri_GetAuthority_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetAuthority_0100 start";
    urie_->uriString_ = "abc";
    auto result = urie_->GetAuthority();
    EXPECT_TRUE(result == "");
    GTEST_LOG_(INFO) << "Uri_GetAuthority_0100 end";
}

/**
 * @tc.number: Uri_GetAuthority_0200
 * @tc.name: GetAuthority
 * @tc.desc: Verify the function to get authority.
 */
HWTEST_F(UriTest, Uri_GetAuthority_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetAuthority_0200 start";
    urie_->uriString_ = "abc";
    urie_->cachedSsi_ = 1;
    auto result = urie_->GetAuthority();
    EXPECT_TRUE(result == "");
    GTEST_LOG_(INFO) << "Uri_GetAuthority_0200 end";
}

/**
 * @tc.number: Uri_GetAuthority_0300
 * @tc.name: GetAuthority
 * @tc.desc: Verify the function to get authority.
 */
HWTEST_F(UriTest, Uri_GetAuthority_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetAuthority_0300 start";
    urie_->uriString_ = "://abc";
    auto result = urie_->GetAuthority();
    EXPECT_EQ(result, "abc");
    GTEST_LOG_(INFO) << "Uri_GetAuthority_0300 end";
}

/**
 * @tc.number: Uri_GetAuthority_0400
 * @tc.name: GetAuthority
 * @tc.desc: Verify the function to get authority.
 */
HWTEST_F(UriTest, Uri_GetAuthority_0400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetAuthority_0400 start";
    urie_->uriString_ = "";
    auto result = urie_->GetAuthority();
    EXPECT_EQ(result, EMPTY);
    GTEST_LOG_(INFO) << "Uri_GetAuthority_0400 end";
}

/**
 * @tc.number: Uri_GetHost_0100
 * @tc.name: WithAbilityName/GetAbilityName.
 * @tc.desc: Verify the function when the input authority get host.
 */
HWTEST_F(UriTest, Uri_GetHost_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetHost_0100 start";
    urie_->uriString_ = "abc";
    urie_->authority_ = "abcd";
    auto result = urie_->GetHost();
    EXPECT_EQ("abcd", result);
    GTEST_LOG_(INFO) << "Uri_GetHost_0100 end";
}

/**
 * @tc.number: Uri_GetHost_0200
 * @tc.name: WithAbilityName/GetAbilityName.
 * @tc.desc: Verify the function when the input authority get host.
 */
HWTEST_F(UriTest, Uri_GetHost_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetHost_0200 start";
    auto result = urie_->GetAuthority();
    EXPECT_TRUE(result == "");
    GTEST_LOG_(INFO) << "Uri_GetHost_0200 end";
}

/**
 * @tc.number: Uri_GetPort_0100
 * @tc.name: WithAbilityName/GetAbilityName.
 * @tc.desc: Verify the function when the input port get port.
 */
HWTEST_F(UriTest, Uri_GetPort_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetPort_0100 start";
    urie_->uriString_ = "abc";
    urie_->port_ = -1;
    auto result = urie_->GetPort();
    EXPECT_EQ(urie_->port_, result);
    GTEST_LOG_(INFO) << "Uri_GetPort_0100 end";
}

/**
 * @tc.number: Uri_GetPort_0200
 * @tc.name: WithAbilityName/GetAbilityName.
 * @tc.desc: Verify the function when the input port get port..
 */
HWTEST_F(UriTest, Uri_GetPort_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetPort_0200 start";
    auto result = urie_->GetPort();
    EXPECT_TRUE(result == -1);
    GTEST_LOG_(INFO) << "Uri_GetPort_0200 end";
}

/**
 * @tc.number: Uri_GetPort_0300
 * @tc.name: WithAbilityName/GetAbilityName.
 * @tc.desc: Verify the function when the input port get port.
 */
HWTEST_F(UriTest, Uri_GetPort_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetPort_0300 start";
    urie_->uriString_ = "://:567";
    auto result = urie_->GetPort();
    EXPECT_EQ(567, result);
    GTEST_LOG_(INFO) << "Uri_GetPort_0300 end";
}

/**
 * @tc.number: Uri_GetPort_0400
 * @tc.name: WithAbilityName/GetAbilityName.
 * @tc.desc: Verify the function when the input port get port.
 */
HWTEST_F(UriTest, Uri_GetPort_0400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetPort_0400 start";
    urie_->uriString_ = ":abc";
    urie_->port_ = 5;
    auto result = urie_->GetPort();
    EXPECT_EQ(5, result);
    GTEST_LOG_(INFO) << "Uri_GetPort_0400 end";
}

/**
 * @tc.number: Uri_GetUserInfo_0100
 * @tc.name: GetUserInfo
 * @tc.desc: Verify the function when the input userInfo get userInfo.
 */
HWTEST_F(UriTest, Uri_GetUserInfo_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetUserInfo_0100 start";
    urie_->uriString_ = "abc";
    auto result = urie_->GetUserInfo();
    EXPECT_TRUE(result == "");
    GTEST_LOG_(INFO) << "Uri_GetUserInfo_0100 end";
}

/**
 * @tc.number: Uri_GetUserInfo_0200
 * @tc.name: GetUserInfo
 * @tc.desc: Verify the function when the input userInfo get userInfo.
 */
HWTEST_F(UriTest, Uri_GetUserInfo_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetUserInfo_0200 start";
    urie_->uriString_ = "abcde@";
    urie_->userInfo_ = "abc";
    auto result = urie_->GetUserInfo();
    EXPECT_EQ(result, "abc");
    GTEST_LOG_(INFO) << "Uri_GetUserInfo_0200 end";
}

/**
 * @tc.number: Uri_GetFragment_0100
 * @tc.name: GetFragment
 * @tc.desc: Verify the function to get fragment.
 */
HWTEST_F(UriTest, Uri_GetFragment_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetFragment_0100 start";
    urie_->uriString_ = "abc";
    auto result = urie_->GetFragment();
    EXPECT_TRUE(result == "");
    GTEST_LOG_(INFO) << "Uri_GetFragment_0100 end";
}

/**
 * @tc.number: Uri_IsHierarchical_0100
 * @tc.name: IsHierarchical
 * @tc.desc: Verify the function is hierarchical.
 */
HWTEST_F(UriTest, Uri_IsHierarchical_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_IsHierarchical_0100 start";
    urie_->uriString_ = "abc";
    urie_->cachedSsi_ = std::string::npos;
    auto result = urie_->IsHierarchical();
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "Uri_IsHierarchical_0100 end";
}

/**
 * @tc.number: Uri_IsHierarchical_0200
 * @tc.name: IsHierarchical
 * @tc.desc: Verify the function is hierarchical.
 */
HWTEST_F(UriTest, Uri_IsHierarchical_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_IsHierarchical_0200 start";
    urie_->uriString_ = ":abc";
    auto result = urie_->IsHierarchical();
    EXPECT_FALSE(result);
    GTEST_LOG_(INFO) << "Uri_IsHierarchical_0200 end";
}

/**
 * @tc.number: Uri_IsAbsolute_0100
 * @tc.name: IsAbsolute
 * @tc.desc: Verify the function is absolute.
 */
HWTEST_F(UriTest, Uri_IsAbsolute_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_IsAbsolute_0100 start";
    urie_->uriString_ = "ab:c";
    auto result = urie_->IsAbsolute();
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "Uri_IsAbsolute_0100 end";
}

/**
 * @tc.number: Uri_IsAbsolute_0200
 * @tc.name: IsAbsolute
 * @tc.desc: Verify the function is absolute.
 */
HWTEST_F(UriTest, Uri_IsAbsolute_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_IsAbsolute_0200 start";
    auto result = urie_->IsAbsolute();
    EXPECT_TRUE(!result);

    urie_->uriString_ = "";
    result = urie_->IsAbsolute();
    EXPECT_TRUE(!result);
    GTEST_LOG_(INFO) << "Uri_IsAbsolute_0200 end";
}

/**
 * @tc.number: Uri_IsRelative_0100
 * @tc.name: IsRelative
 * @tc.desc: Verify the function is relative.
 */
HWTEST_F(UriTest, Uri_IsRelative_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_IsRelative_0100 start";
    urie_->uriString_ = "abc";
    urie_->cachedSsi_ = std::string::npos;
    auto result = urie_->IsRelative();
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "Uri_IsRelative_0100 end";
}

/**
 * @tc.number: Uri_IsRelative_0200
 * @tc.name: IsRelative
 * @tc.desc: Verify the function is relative.
 */
HWTEST_F(UriTest, Uri_IsRelative_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_IsRelative_0200 start";
    urie_->uriString_ = "a:bc";
    auto result = urie_->IsRelative();
    EXPECT_TRUE(!result);
    GTEST_LOG_(INFO) << "Uri_IsRelative_0200 end";
}

/**
 * @tc.number: Uri_Equals_0100
 * @tc.name: Equals
 * @tc.desc: Verify the function when the input string contains special characters.
 */
HWTEST_F(UriTest, Uri_Equals_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_Equals_0100 start";
    Parcel parcel;
    auto other = urie_->Unmarshalling(parcel);
    auto result = urie_->Equals(*other);
    EXPECT_FALSE(result);
    GTEST_LOG_(INFO) << "Uri_Equals_0100 end";
}

/**
 * @tc.number: Uri_CompareTo_0100
 * @tc.name: Equals
 * @tc.desc: Verify the function when the input string contains special characters.
 */
HWTEST_F(UriTest, Uri_CompareTo_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_CompareTo_0100 start";
    Parcel parcel;
    auto other = urie_->Unmarshalling(parcel);
    auto result = urie_->CompareTo(*other);
    EXPECT_EQ(result, 1);
    GTEST_LOG_(INFO) << "Uri_CompareTo_0100 end";
}

/**
 * @tc.number: Uri_GetScheme_0100
 * @tc.name: GetScheme
 * @tc.desc: Verify the function when the uriString_ is not empty.
 * @tc.require: issueI6415N
 */
HWTEST_F(UriTest, Uri_GetScheme_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetScheme_0100 start";
    string uriString = "this is uriString";
    urie_ = std::make_shared<Urie>(uriString);
    string result = urie_->GetScheme();
    string result1 = urie_->ParseScheme();
    EXPECT_EQ(result, result1);
    GTEST_LOG_(INFO) << "Uri_GetScheme_0100 end";
}

/**
 * @tc.number: Uri_GetSchemeSpecificPart_0300
 * @tc.name: GetSchemeSpecificPart
 * @tc.desc: Verify the function when the uriString_ is empty.
 * @tc.require: issueI6415N
 */
HWTEST_F(UriTest, Uri_GetSchemeSpecificPart_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetSchemeSpecificPart_0300 start";
    auto result = urie_->GetSchemeSpecificPart();
    auto result1 = urie_->GetAuthority();
    auto result2 = urie_->GetUserInfo();
    auto result5 = urie_->GetHost();
    auto result6 = urie_->ParseHost();
    auto result7 = urie_->GetPort();
    auto result8 = urie_->GetQuery();
    auto result9 = urie_->GetPath();
    auto result10 = urie_->GetFragment();
    auto result13 = urie_->IsAbsolute();
    EXPECT_EQ(result, " ");
    EXPECT_EQ(result1, EMPTY);
    EXPECT_EQ(result2, EMPTY);
    EXPECT_EQ(result5, EMPTY);
    EXPECT_EQ(result6, EMPTY);
    EXPECT_EQ(result7, PORT_NONE);
    EXPECT_EQ(result8, EMPTY);
    EXPECT_EQ(result9, " ");
    EXPECT_EQ(result10, EMPTY);
    EXPECT_EQ(result13, false);
    GTEST_LOG_(INFO) << "Uri_GetSchemeSpecificPart_0300 end";
}

/**
 * @tc.number: Uri_IsHierarchical_0300
 * @tc.name: IsHierarchical
 * @tc.desc: Verify the function when the uriString_ is empty.
 * @tc.require: issueI6415N
 */
HWTEST_F(UriTest, Uri_IsHierarchical_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_IsHierarchical_0300 start";
    urie_->uriString_ = "";
    auto result1 = urie_->IsHierarchical();
    auto result2 = urie_->IsRelative();
    EXPECT_EQ(result1, false);
    EXPECT_EQ(result2, false);
    GTEST_LOG_(INFO) << "Uri_IsHierarchical_0300 end";
}

/**
 * @tc.number: Uri_ParseUserInfo_0100
 * @tc.name: ParseUserInfo
 * @tc.desc: Verify the function when the authority is not empty.
 * @tc.require: issueI6415N
 */
HWTEST_F(UriTest, Uri_ParseUserInfo_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_ParseUserInfo_0100 start";
    string uriString = "this is uriString";
    urie_ = std::make_shared<Urie>(uriString);
    auto result = urie_->GetAuthority();
    auto result1 = urie_->ParseUserInfo();
    auto result2 = urie_->ParseAuthority();
    auto result3 = urie_->ParsePort();
    EXPECT_EQ(result, result2);
    EXPECT_EQ(result3, PORT_NONE);
    GTEST_LOG_(INFO) << "Uri_ParseUserInfo_0100 end";
}

/**
 * @tc.number: Uri_ParseQuery_0100
 * @tc.name: ParseQuery
 * @tc.desc: Verify the function when the fsi < qsi.
 * @tc.require: issueI6415N
 */
HWTEST_F(UriTest, Uri_ParseQuery_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_ParseQuery_0100 start";
    string uriString = "this is uriString";
    urie_ = std::make_shared<Urie>(uriString);
    urie_->cachedSsi_ = 1;
    urie_->cachedFsi_ = 2;
    auto result = urie_->ParseQuery();
    EXPECT_EQ(result, EMPTY);
    GTEST_LOG_(INFO) << "Uri_ParseQuery_0100 end";
}

/**
 * @tc.number: Uri_ParseQuery_0200
 * @tc.name: ParseQuery
 * @tc.desc: Verify the function when the fsi > qsi.
 * @tc.require: issueI6415N
 */
HWTEST_F(UriTest, Uri_ParseQuery_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_ParseQuery_0200 start";
    urie_ = std::make_shared<Urie>();
    urie_->uriString_ = "?";
    auto result = urie_->ParseQuery();
    EXPECT_EQ(result, EMPTY);

    urie_->uriString_ = "test?#test";
    urie_->cachedSsi_ = 0;
    urie_->cachedFsi_ = 1;
    result = urie_->ParseQuery();
    EXPECT_EQ(result, EMPTY);

    urie_->uriString_ = "?test";
    urie_->cachedSsi_ = 2;
    urie_->cachedFsi_ = 1;
    result = urie_->ParseQuery();
    EXPECT_EQ(result, EMPTY);
    GTEST_LOG_(INFO) << "Uri_ParseQuery_0200 end";
}

/**
 * @tc.number: Uri_ParseQuery_0300
 * @tc.name: ParseQuery
 * @tc.desc: Verify the function.
 * @tc.require: issueI6415N
 */
HWTEST_F(UriTest, Uri_ParseQuery_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_ParseQuery_0300 start";
    string uriString = "this is uriString";
    urie_ = std::make_shared<Urie>(uriString);
    urie_->cachedSsi_ = 3;
    urie_->cachedFsi_ = 2;
    auto result = urie_->ParseQuery();
    EXPECT_EQ(result, EMPTY);
    GTEST_LOG_(INFO) << "Uri_ParseQuery_0300 end";
}

/**
 * @tc.number: Uri_ParsePath_0100
 * @tc.name: ParsePath
 * @tc.desc: Verify the function when the fsi = uriString.length.
 * @tc.require: issueI6415N
 */
HWTEST_F(UriTest, Uri_ParsePath_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_ParsePath_0100 start";
    string uriString = "uriString";
    urie_ = std::make_shared<Urie>(uriString);
    urie_->cachedSsi_ = 8;
    auto result = urie_->ParsePath();
    EXPECT_EQ(result, EMPTY);
    GTEST_LOG_(INFO) << "Uri_ParsePath_0100 end";
}

/**
 * @tc.number: Uri_ParsePath_0200
 * @tc.name: ParsePath
 * @tc.desc: Verify the ParsePath function.
 * @tc.require: issueI6415N
 */
HWTEST_F(UriTest, Uri_ParsePath_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_ParsePath_0200 start";
    string uriString = "uriString";
    urie_ = std::make_shared<Urie>(uriString);
    urie_->cachedSsi_ = 2;
    auto result = urie_->ParsePath();
    EXPECT_EQ(result, EMPTY);

    urie_->uriString_ = ":/";
    urie_->cachedSsi_ = NOT_FOUND;
    result = urie_->ParsePath();
    EXPECT_EQ(result, "/");
    GTEST_LOG_(INFO) << "Uri_ParsePath_0200 end";
}

/**
 * @tc.number: Uri_GetHost_0300
 * @tc.name: WithAbilityName/GetAbilityName.
 * @tc.desc: Verify the GetHost function.
 */
HWTEST_F(UriTest, Uri_GetHost_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetHost_0300 start";
    urie_ = std::make_shared<Urie>();
    urie_->uriString_ = "";
    auto result = urie_->GetHost();
    EXPECT_EQ(result, EMPTY);

    urie_->uriString_ = "test";
    urie_->host_ = "test";
    result = urie_->GetHost();
    EXPECT_EQ(result, "test");
    GTEST_LOG_(INFO) << "Uri_GetHost_0300 end";
}

/**
 * @tc.number: Uri_ParsePath_0300
 * @tc.name: ParsePath
 * @tc.desc: Verify the ParsePath function.
 * @tc.require: issueI6415N
 */
HWTEST_F(UriTest, Uri_ParsePath_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_ParsePath_0300 start";
    urie_ = std::make_shared<Urie>();
    urie_->uriString_ = "";
    auto result = urie_->ParsePath(NOT_FOUND);
    EXPECT_EQ(result, EMPTY);

    urie_->uriString_ = "/";
    result = urie_->ParsePath(NOT_FOUND);
    EXPECT_EQ(result, "/");

    urie_->uriString_ = "//";
    result = urie_->ParsePath(NOT_FOUND);
    EXPECT_EQ(result, EMPTY);

    urie_->uriString_ = "//?";
    result = urie_->ParsePath(NOT_FOUND);
    EXPECT_EQ(result, EMPTY);

    urie_->uriString_ = "//#";
    result = urie_->ParsePath(NOT_FOUND);
    EXPECT_EQ(result, EMPTY);

    urie_->uriString_ = "///";
    result = urie_->ParsePath(NOT_FOUND);
    EXPECT_EQ(result, "/");

    urie_->uriString_ = "//\\";
    result = urie_->ParsePath(NOT_FOUND);
    EXPECT_EQ(result, "\\");

    urie_->uriString_ = "//a";
    result = urie_->ParsePath(NOT_FOUND);
    EXPECT_EQ(result, EMPTY);
    GTEST_LOG_(INFO) << "Uri_ParsePath_0300 end";
}

/**
 * @tc.number: Uri_GetScheme_0200
 * @tc.name: GetScheme
 * @tc.desc: Verify the GetScheme function.
 */
HWTEST_F(UriTest, Uri_GetScheme_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetScheme_0200 start";
    urie_ = std::make_shared<Urie>("");
    auto result = urie_->GetScheme();
    EXPECT_EQ(result, EMPTY);

    urie_->uriString_ = "test";
    urie_->scheme_ = NOT_CACHED;
    result = urie_->GetScheme();
    EXPECT_EQ(result, EMPTY);
    GTEST_LOG_(INFO) << "Uri_GetScheme_0200 end";
}

/**
 * @tc.number: Uri_GetSchemeSpecificPart_0400
 * @tc.name: GetSchemeSpecificPart
 * @tc.desc: Verify the GetSchemeSpecificPart function.
 */
HWTEST_F(UriTest, Uri_GetSchemeSpecificPart_0400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetSchemeSpecificPart_0400 start";
    urie_ = std::make_shared<Urie>("http:");
    auto result = urie_->GetSchemeSpecificPart();
    EXPECT_EQ(result, EMPTY);

    urie_->uriString_ = "";
    result = urie_->GetSchemeSpecificPart();
    EXPECT_EQ(result, EMPTY);
    GTEST_LOG_(INFO) << "Uri_GetSchemeSpecificPart_0400 end";
}

/**
 * @tc.number: Uri_ParseSsp_0100
 * @tc.name: ParseSsp
 * @tc.desc: Verify the ParseSsp function.
 */
HWTEST_F(UriTest, Uri_ParseSsp_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_ParseSsp_0100 start";
    urie_ = std::make_shared<Urie>();
    urie_->uriString_ = "";
    urie_->cachedSsi_ = 0;
    urie_->cachedFsi_ = 0;
    auto result = urie_->ParseSsp();
    EXPECT_EQ(result, EMPTY);
    GTEST_LOG_(INFO) << "Uri_ParseSsp_0100 end";
}

/**
 * @tc.number: Uri_ParseUserInfo_0200
 * @tc.name: ParseUserInfo
 * @tc.desc: Verify the ParseUserInfo function.
 */
HWTEST_F(UriTest, Uri_ParseUserInfo_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_ParseUserInfo_0200 start";
    urie_ = std::make_shared<Urie>();
    urie_->uriString_ = "test";
    urie_->authority_ = "test";
    auto result = urie_->ParseUserInfo();
    EXPECT_EQ(result, EMPTY);
    GTEST_LOG_(INFO) << "Uri_ParseUserInfo_0200 end";
}

/**
 * @tc.number: Uri_GetPort_0500
 * @tc.name: GetPort
 * @tc.desc: Verify the GetPort function.
 */
HWTEST_F(UriTest, Uri_GetPort_0500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetPort_0500 start";
    urie_ = std::make_shared<Urie>();
    urie_->uriString_ = "";
    auto result = urie_->GetPort();
    EXPECT_EQ(result, PORT_NONE);
    GTEST_LOG_(INFO) << "Uri_GetPort_0500 end";
}

/**
 * @tc.number: Uri_ParsePort_0100
 * @tc.name: ParsePort
 * @tc.desc: Verify the ParsePort function.
 */
HWTEST_F(UriTest, Uri_ParsePort_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_ParsePort_0100 start";
    urie_ = std::make_shared<Urie>();
    urie_->authority_ = "test";
    urie_->uriString_ = "test";
    auto result = urie_->ParsePort();
    EXPECT_EQ(result, PORT_NONE);
    GTEST_LOG_(INFO) << "Uri_ParsePort_0100 end";
}

/**
 * @tc.number: Uri_GetQuery_0100
 * @tc.name: GetQuery
 * @tc.desc: Verify the GetQuery function.
 */
HWTEST_F(UriTest, Uri_GetQuery_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetQuery_0100 start";
    urie_ = std::make_shared<Urie>();
    urie_->uriString_ = "";
    auto result = urie_->GetQuery();
    EXPECT_EQ(result, EMPTY);

    urie_->uriString_ = "test";
    urie_->query_ = "";
    result = urie_->GetQuery();
    EXPECT_EQ(result, EMPTY);
    GTEST_LOG_(INFO) << "Uri_GetQuery_0100 end";
}

/**
 * @tc.number: Uri_GetPath_0100
 * @tc.name: GetPath
 * @tc.desc: Verify the GetPath function.
 */
HWTEST_F(UriTest, Uri_GetPath_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetPath_0100 start";
    urie_ = std::make_shared<Urie>();
    urie_->uriString_ = "";
    auto result = urie_->GetPath();
    EXPECT_EQ(result, EMPTY);

    urie_->uriString_ = "test";
    urie_->path_ = "test";
    result = urie_->GetPath();
    EXPECT_EQ(result, "test");
    GTEST_LOG_(INFO) << "Uri_GetPath_0100 end";
}

/**
 * @tc.number: Uri_GetPathSegments_0100
 * @tc.name: GetPathSegments
 * @tc.desc: Verify the function when path is empty.
 */
HWTEST_F(UriTest, Uri_GetPathSegments_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetPathSegments_0100 start";
    urie_ = std::make_shared<Urie>();
    urie_->uriString_ = "";
    std::vector<std::string> segments;
    urie_->GetPathSegments(segments);
    EXPECT_EQ(segments.size(), 0);

    urie_->uriString_ = ":/a/b/c/d";
    urie_->path_ = NOT_CACHED;
    urie_->GetPathSegments(segments);
    EXPECT_EQ(segments.size(), 4); // 4: vector size

    urie_->uriString_ = ":/a/b/c/d/";
    urie_->path_ = NOT_CACHED;
    urie_->GetPathSegments(segments);
    EXPECT_EQ(segments.size(), 8); // 8: vector size
    GTEST_LOG_(INFO) << "Uri_GetPathSegments_0100 end";
}

/**
 * @tc.number: Uri_GetPathSegments_0200
 * @tc.name: GetPathSegments
 * @tc.desc: Verify the GetPathSegments function.
 */
HWTEST_F(UriTest, Uri_GetPathSegments_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetPathSegments_0200 start";
    urie_ = std::make_shared<Urie>();
    urie_->uriString_ = ":/a/b/c/d";
    urie_->path_ = "/a/b/c/d";
    std::vector<std::string> segments;
    urie_->GetPathSegments(segments);
    EXPECT_EQ(segments.size(), 4); // 4: vector size

    urie_->uriString_ = ":/a/b/c/d/";
    urie_->path_ = "/a/b/c/d/";
    urie_->GetPathSegments(segments);
    EXPECT_EQ(segments.size(), 8); // 8: vector size
    GTEST_LOG_(INFO) << "Uri_GetPathSegments_0200 end";
}

/**
 * @tc.number: Uri_GetFragment_0200
 * @tc.name: GetFragment
 * @tc.desc: Verify the GetFragment function.
 */
HWTEST_F(UriTest, Uri_GetFragment_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetFragment_0200 start";
    urie_ = std::make_shared<Urie>("http:path#fragment");
    auto result = urie_->GetFragment();
    EXPECT_EQ(result, "fragment");

    urie_->uriString_ = "";
    result = urie_->GetFragment();
    EXPECT_EQ(result, EMPTY);

    urie_->uriString_ = "test";
    urie_->fragment_ = "";
    result = urie_->GetFragment();
    EXPECT_EQ(result, EMPTY);
    GTEST_LOG_(INFO) << "Uri_GetFragment_0200 end";
}

/**
 * @tc.number: Uri_FindFragmentSeparator_0100
 * @tc.name: FindFragmentSeparator
 * @tc.desc: Verify the FindFragmentSeparator function.
 */
HWTEST_F(UriTest, Uri_FindFragmentSeparator_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_FindFragmentSeparator_0100 start";
    urie_ = std::make_shared<Urie>("http:path");
    urie_->cachedFsi_ = 0;
    auto result = urie_->FindFragmentSeparator();
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << "Uri_FindFragmentSeparator_0100 end";
}

/**
 * @tc.number: Uri_Marshalling_0100
 * @tc.name: Marshalling
 * @tc.desc: Verify the Marshalling function.
 */
HWTEST_F(UriTest, Uri_Marshalling_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_Marshalling_0100 start";
    urie_ = std::make_shared<Urie>();
    urie_->uriString_ = "café";
    Parcel parcel;
    auto result = urie_->Marshalling(parcel);
    EXPECT_EQ(result, false);
    GTEST_LOG_(INFO) << "Uri_Marshalling_0100 end";
}

/**
 * @tc.number: Uri_GetUserInfo_0300
 * @tc.name: GetUserInfo
 * @tc.desc: Verify the GetUserInfo function.
 */
HWTEST_F(UriTest, Uri_GetUserInfo_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_GetUserInfo_0300 start";
    urie_ = std::make_shared<Urie>();
    urie_->uriString_ = "";
    auto result = urie_->GetUserInfo();
    EXPECT_EQ(result, EMPTY);
    GTEST_LOG_(INFO) << "Uri_GetUserInfo_0300 end";
}

/**
 * @tc.number: Uri_ParseAuthority_0100
 * @tc.name: ParseAuthority
 * @tc.desc: Verify the ParseAuthority function.
 */
HWTEST_F(UriTest, Uri_ParseAuthority_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_ParseAuthority_0100 start";
    urie_ = std::make_shared<Urie>();
    urie_->uriString_ = "";
    urie_->cachedSsi_ = 0;
    auto result = urie_->ParseAuthority();
    EXPECT_EQ(result, EMPTY);
    GTEST_LOG_(INFO) << "Uri_ParseAuthority_0100 end";
}

/**
 * @tc.number: Uri_ParseHost_0100
 * @tc.name: ParseHost
 * @tc.desc: Verify the ParseAuthority function.
 */
HWTEST_F(UriTest, Uri_ParseHost_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_ParseHost_0100 start";
    urie_ = std::make_shared<Urie>();
    urie_->uriString_ = "///test";
    urie_->authority_ = "test@";
    auto result = urie_->ParseAuthority();
    EXPECT_EQ(result, EMPTY);

    urie_->uriString_ = "//\\test";
    result = urie_->ParseAuthority();
    EXPECT_EQ(result, EMPTY);

    urie_->uriString_ = "//?test";
    result = urie_->ParseAuthority();
    EXPECT_EQ(result, EMPTY);

    urie_->uriString_ = "//#test";
    result = urie_->ParseAuthority();
    EXPECT_EQ(result, EMPTY);
    GTEST_LOG_(INFO) << "Uri_ParseHost_0100 end";
}

/**
 * @tc.number: Uri_CheckScheme_0100
 * @tc.name: CheckScheme
 * @tc.desc: Verify the CheckScheme function.
 */
HWTEST_F(UriTest, Uri_CheckScheme_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_CheckScheme_0100 start";
    urie_ = std::make_shared<Urie>();
    urie_->uriString_ = "123:";
    auto result = urie_->CheckScheme();
    EXPECT_EQ(result, false);
    GTEST_LOG_(INFO) << "Uri_CheckScheme_0100 end";
}

/**
 * @tc.number: Uri_Constructor_0100
 * @tc.name: Constructor
 * @tc.desc: Verify the Constructor function.
 */
HWTEST_F(UriTest, Uri_Constructor_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Uri_Constructor_0100 start";
    urie_ = std::make_shared<Urie>("123:");
    EXPECT_EQ(urie_->uriString_, EMPTY);
    GTEST_LOG_(INFO) << "Uri_Constructor_0100 end";
}
}