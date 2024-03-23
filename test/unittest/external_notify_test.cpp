/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include <cstdio>
#include <thread>
#include <unistd.h>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "accesstoken_kit.h"
#include "context_impl.h"
#include "file_access_framework_errno.h"
#include "file_access_observer_common.h"
#include "iservice_registry.h"
#include "iobserver_callback.h"
#include "nativetoken_kit.h"
#include "observer_callback_stub.h"
#include "token_setproc.h"

#define private public
#include "file_access_helper.h"
#undef private

namespace {
using namespace std;
using namespace OHOS;
using namespace FileAccessFwk;
using json = nlohmann::json;
const int ABILITY_ID = 5003;
const int INIT_THREADS_NUMBER = 4;
shared_ptr<FileAccessHelper> g_fah = nullptr;
int g_notifyEvent = -1;
int g_notifyFlag = -1;
string g_notifyUri = "";
vector<string> g_notifyUris;
const int SLEEP_TIME = 600 * 1000;
const int UID_TRANSFORM_TMP = 20000000;
const int UID_DEFAULT = 0;
const uint64_t SYSTEM_APP_MASK =  (static_cast<uint64_t>(1) << 32); // 1: Base number, 32: Left shifted bit numbers

shared_ptr<OHOS::AbilityRuntime::Context> g_context = nullptr;

void SetNativeToken()
{
    uint64_t tokenId;
    const char **perms = new const char *[2];
    perms[0] = "ohos.permission.FILE_ACCESS_MANAGER";
    perms[1] = "ohos.permission.GET_BUNDLE_INFO_PRIVILEGED";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 2,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .aplStr = "system_core",
    };

    infoInstance.processName = "SetUpTestCase";
    tokenId = GetAccessTokenId(&infoInstance);
    const uint64_t systemAppMask = (static_cast<uint64_t>(1) << 32);
    tokenId |= systemAppMask;
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    delete[] perms;
}

void SetNativeToken(const char* processName, const char*perms[], int32_t permsNum)
{
    uint64_t tokenId;
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = permsNum,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .aplStr = "system_core",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    const uint64_t systemAppMask = SYSTEM_APP_MASK;
    tokenId |= systemAppMask;
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

class FileExtensionNotifyTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        cout << "FileExtensionNotifyTest code test" << endl;
        SetNativeToken();
        auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        auto remoteObj = saManager->GetSystemAbility(ABILITY_ID);
        g_context = make_shared<OHOS::AbilityRuntime::ContextImpl>();
        g_context->SetToken(remoteObj);
        AAFwk::Want want;
        vector<AAFwk::Want> wantVec;
        setuid(UID_TRANSFORM_TMP);
        int ret = FileAccessHelper::GetRegisteredFileAccessExtAbilityInfo(wantVec);
        EXPECT_EQ(ret, OHOS::FileAccessFwk::ERR_OK);
        bool sus = false;
        for (size_t i = 0; i < wantVec.size(); i++) {
            auto element = wantVec[i].GetElement();
            if (element.GetBundleName() == "com.ohos.UserFile.ExternalFileManager" &&
                element.GetAbilityName() == "FileExtensionAbility") {
                want = wantVec[i];
                sus = true;
                break;
            }
        }
        EXPECT_TRUE(sus);
        vector<AAFwk::Want> wants{want};
        g_fah = FileAccessHelper::Creator(remoteObj, wants);
        if (g_fah == nullptr) {
            GTEST_LOG_(ERROR) << "external_file_access_test g_fah is nullptr";
            exit(1);
        }
        setuid(UID_DEFAULT);
    }
    static void TearDownTestCase()
    {
        g_fah->Release();
        g_fah = nullptr;
    };
    void SetUp(){};
    void TearDown(){};
};

class MyObserver : public ObserverCallbackStub {
public:
    MyObserver() {};
    virtual ~MyObserver() = default;
    void OnChange(NotifyMessage &notifyMessage) override;
};

void MyObserver::OnChange(NotifyMessage &notifyMessage)
{
    g_notifyEvent = static_cast<int>(notifyMessage.notifyType_);
    std::string notifyUri = notifyMessage.uris_[0];
    g_notifyUri = notifyUri;
    GTEST_LOG_(INFO) << "enter notify uri =" + notifyUri + " type =" + std::to_string(g_notifyEvent) +" uri size" +
        std::to_string(notifyMessage.uris_.size());
}

class TestObserver : public ObserverCallbackStub {
public:
    TestObserver() {};
    virtual ~TestObserver() = default;
    void OnChange(NotifyMessage &notifyMessage) override;
};

void TestObserver::OnChange(NotifyMessage &notifyMessage)
{
    g_notifyFlag = static_cast<int>(notifyMessage.notifyType_);
    std::string notifyUri = notifyMessage.uris_[0];
    g_notifyUris = notifyMessage.uris_;
    GTEST_LOG_(INFO) << "enter TestObserver uri =" + notifyUri + "type =" + std::to_string(g_notifyFlag);
}

static tuple<Uri, Uri, Uri, Uri> ReadyRegisterNotify00(Uri& parentUri,
                                                       sptr<IFileAccessObserver>& myObserver1,
                                                       sptr<IFileAccessObserver>& myObserver2,
                                                       sptr<IFileAccessObserver>& myObserver3,
                                                       sptr<IFileAccessObserver>& myObserver4)
{
    bool notifyForDescendants = false;
    Uri newDirUriTest1("");
    int result = g_fah->Mkdir(parentUri, "uri_dir1", newDirUriTest1);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri newDirUriTest2("");
    result = g_fah->Mkdir(parentUri, "uri_dir2", newDirUriTest2);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri newFileUri1("");
    result = g_fah->CreateFile(parentUri, "uri_file1", newFileUri1);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    GTEST_LOG_(INFO) <<  newFileUri1.ToString();
    Uri newFileUri2("");
    result = g_fah->CreateFile(parentUri, "uri_file2", newFileUri2);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

    result = g_fah->RegisterNotify(newDirUriTest1, true, myObserver1);
    EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    result = g_fah->RegisterNotify(newDirUriTest2, notifyForDescendants, myObserver2);
    EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    result = g_fah->RegisterNotify(newFileUri1, notifyForDescendants, myObserver3);
    EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    result = g_fah->RegisterNotify(newFileUri2, notifyForDescendants, myObserver4);
    EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);

    return {newDirUriTest1, newDirUriTest2, newFileUri1, newFileUri2};
}

static tuple<Uri, Uri> TriggerNotify00(Uri& newDirUriTest1, Uri& newDirUriTest2, Uri& newFileUri1, Uri& newFileUri2)
{
    Uri testUri("");
    int result = g_fah->CreateFile(newDirUriTest1, "testUri", testUri);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    usleep(SLEEP_TIME);
    EXPECT_EQ(g_notifyEvent, ADD_EVENT);
    EXPECT_EQ(g_notifyUri, testUri.ToString());
    Uri renameDirUri1("");
    result = g_fah->Rename(newDirUriTest1, "testDir", renameDirUri1);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    usleep(SLEEP_TIME);
    EXPECT_EQ(g_notifyEvent, MOVED_SELF);
    EXPECT_EQ(g_notifyUri, newDirUriTest1.ToString());
    result = g_fah->Delete(newDirUriTest2);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    usleep(SLEEP_TIME);
    EXPECT_EQ(g_notifyEvent, DELETE_EVENT);
    EXPECT_EQ(g_notifyUri, newDirUriTest2.ToString());

    Uri renameUri1("");
    result = g_fah->Rename(newFileUri1, "renameUri1", renameUri1);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    usleep(SLEEP_TIME);
    EXPECT_EQ(g_notifyEvent, MOVED_SELF);
    EXPECT_EQ(g_notifyUri, newFileUri1.ToString());
    result = g_fah->Delete(newFileUri2);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    usleep(SLEEP_TIME);
    EXPECT_EQ(g_notifyEvent, DELETE_EVENT);
    EXPECT_EQ(g_notifyUri, newFileUri2.ToString());

    return {renameDirUri1, renameUri1};
}

/**
 * @tc.number: user_file_service_external_file_access_notify_0000
 * @tc.name: external_file_access_notify_0000
 * @tc.desc: Test function of RegisterNotify and UnregisterNotify interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionNotifyTest, external_file_access_notify_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-begin external_file_access_notify_0000";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        sptr<IFileAccessObserver> myObserver1 = new (std::nothrow) MyObserver();
        sptr<IFileAccessObserver> myObserver2 = new (std::nothrow) MyObserver();
        sptr<IFileAccessObserver> myObserver3 = new (std::nothrow) MyObserver();
        sptr<IFileAccessObserver> myObserver4 = new (std::nothrow) MyObserver();
        Uri parentUri(info[1].uri);
        auto [newDirUriTest1, newDirUriTest2, newFileUri1, newFileUri2] =
            ReadyRegisterNotify00(parentUri, myObserver1, myObserver2, myObserver3, myObserver4);

        auto [renameDirUri1, renameUri1] = TriggerNotify00(newDirUriTest1, newDirUriTest2, newFileUri1, newFileUri2);

        sleep(1);
        result = g_fah->UnregisterNotify(newDirUriTest1, myObserver1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->UnregisterNotify(newDirUriTest2, myObserver2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->UnregisterNotify(newFileUri1, myObserver3);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->UnregisterNotify(newFileUri2, myObserver4);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(renameDirUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(renameUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_notify_0000 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-end external_file_access_notify_0000";
}

static tuple<Uri, Uri, Uri> ReadyRegisterNotify01(Uri& parentUri, sptr<IFileAccessObserver> myObserver1,
    sptr<IFileAccessObserver> myObserver2, sptr<IFileAccessObserver> myObserver3)
{
    bool notifyForDescendants1 = true;
    bool notifyForDescendants2 = false;
    Uri newFileUri1("");
    int result = g_fah->CreateFile(parentUri, "uri_file1", newFileUri1);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri newFileUri2("");
    result = g_fah->CreateFile(parentUri, "uri_file2", newFileUri2);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri newFileUri3("");
    result = g_fah->CreateFile(parentUri, "uri_file3", newFileUri3);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

    result = g_fah->RegisterNotify(newFileUri1, notifyForDescendants1, myObserver1);
    EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    result = g_fah->RegisterNotify(newFileUri2, notifyForDescendants1, myObserver2);
    EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    result = g_fah->RegisterNotify(newFileUri3, notifyForDescendants2, myObserver3);
    EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    return {newFileUri1, newFileUri2, newFileUri3};
}

/**
 * @tc.number: user_file_service_external_file_access_notify_0001
 * @tc.name: external_file_access_notify_0001
 * @tc.desc: Test function of RegisterNotify and UnregisterNotify interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionNotifyTest, external_file_access_notify_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-begin external_file_access_notify_0001";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        sptr<IFileAccessObserver> myObserver1 = new (std::nothrow) MyObserver();
        sptr<IFileAccessObserver> myObserver2 = new (std::nothrow) MyObserver();
        sptr<IFileAccessObserver> myObserver3 = new (std::nothrow) MyObserver();
        Uri parentUri(info[1].uri);
        auto [newFileUri1, newFileUri2, newFileUri3] =
            ReadyRegisterNotify01(parentUri, myObserver1, myObserver2, myObserver3);

        Uri renameFileUri1("");
        result = g_fah->Rename(newFileUri1, "renamefile", renameFileUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        usleep(SLEEP_TIME);
        EXPECT_EQ(g_notifyEvent, MOVED_SELF);
        EXPECT_EQ(g_notifyUri, newFileUri1.ToString());

        result = g_fah->Delete(newFileUri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        usleep(SLEEP_TIME);
        EXPECT_EQ(g_notifyEvent, DELETE_EVENT);
        EXPECT_EQ(g_notifyUri, newFileUri2.ToString());
        result = g_fah->Delete(newFileUri3);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        usleep(SLEEP_TIME);
        EXPECT_EQ(g_notifyEvent, DELETE_EVENT);
        EXPECT_EQ(g_notifyUri, newFileUri3.ToString());

        sleep(1);
        result = g_fah->UnregisterNotify(newFileUri1, myObserver1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->UnregisterNotify(newFileUri2, myObserver2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->UnregisterNotify(newFileUri3, myObserver3);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(renameFileUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_notify_0001 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-end external_file_access_notify_0001";
}

static tuple<Uri, Uri, Uri, Uri> ReadyRegisterNotify02(Uri& parentUri)
{
    Uri newDirUriTest1("");
    int result = g_fah->Mkdir(parentUri, "uri_dir1", newDirUriTest1);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri newDirUriTest2("");
    result = g_fah->Mkdir(parentUri, "uri_dir2", newDirUriTest2);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri newFileUri1("");
    result = g_fah->CreateFile(parentUri, "uri_file1", newFileUri1);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri newFileUri2("");
    result = g_fah->CreateFile(parentUri, "uri_file2", newFileUri2);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

    return {newDirUriTest1, newDirUriTest2, newFileUri1, newFileUri2};
}

static tuple<Uri, Uri> TriggerNotify02(Uri& newDirUriTest1, Uri& newDirUriTest2, Uri& newFileUri1, Uri& newFileUri2)
{
    const int tm = 2;
    Uri testFile("");
    int result = g_fah->CreateFile(newDirUriTest1, "test_file", testFile);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    usleep(SLEEP_TIME);
    Uri renameDirUri1("");
    result = g_fah->Rename(newDirUriTest1, "renameDir", renameDirUri1);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    usleep(SLEEP_TIME);
    EXPECT_EQ(g_notifyEvent, MOVED_SELF);
    EXPECT_EQ(g_notifyUri, newDirUriTest1.ToString());
    result = g_fah->Delete(newDirUriTest2);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    usleep(SLEEP_TIME);
    EXPECT_EQ(g_notifyEvent, DELETE_EVENT);
    EXPECT_EQ(g_notifyUri, newDirUriTest2.ToString());
    Uri renameFileUri1("");
    result = g_fah->Rename(newFileUri1, "renamefile", renameFileUri1);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    usleep(SLEEP_TIME);
    EXPECT_EQ(g_notifyEvent, MOVED_SELF);
    EXPECT_EQ(g_notifyUri, newFileUri1.ToString());
    result = g_fah->Delete(newFileUri2);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    usleep(SLEEP_TIME);
    EXPECT_EQ(g_notifyEvent, DELETE_EVENT);
    EXPECT_EQ(g_notifyUri, newFileUri2.ToString());
    sleep(tm);
    return {renameDirUri1, renameFileUri1};
}
/**
 * @tc.number: user_file_service_external_file_access_notify_0002
 * @tc.name: external_file_access_notify_0002
 * @tc.desc: Test function of RegisterNotify and UnregisterNotify interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionNotifyTest, external_file_access_notify_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-begin external_file_access_notify_0002";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        bool notifyForDescendants = true;
        vector<sptr<IFileAccessObserver>> observers;
        const int len = 12;
        const int group = 3;
        for (int i = 0; i < len; i++) {
            observers.emplace_back(new (std::nothrow) MyObserver());
        }
        Uri parentUri(info[1].uri);
        auto [newDirUriTest1, newDirUriTest2, newFileUri1, newFileUri2] = ReadyRegisterNotify02(parentUri);

        for (int i = 0; i < group; i++) {
            result = g_fah->RegisterNotify(newDirUriTest1, notifyForDescendants, observers[i]);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
            result = g_fah->RegisterNotify(newDirUriTest2, notifyForDescendants, observers[3 + i]);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
            result = g_fah->RegisterNotify(newFileUri1, notifyForDescendants, observers[6 + i]);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
            result = g_fah->RegisterNotify(newFileUri2, notifyForDescendants, observers[9 + i]);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }

        auto [renameDirUri1, renameFileUri1] =
            TriggerNotify02(newDirUriTest1, newDirUriTest2, newFileUri1, newFileUri2);

        for (int i = 0; i < group; i++) {
            result = g_fah->UnregisterNotify(newDirUriTest1, observers[i]);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
            result = g_fah->UnregisterNotify(newDirUriTest2, observers[3 + i]);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
            result = g_fah->UnregisterNotify(newFileUri1, observers[6 + i]);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
            result = g_fah->UnregisterNotify(newFileUri2, observers[9 + i]);
            EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        }
        result = g_fah->Delete(renameDirUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(renameFileUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_notify_0002 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-end external_file_access_notify_0002";
}

/**
 * @tc.number: user_file_service_external_file_access_notify_0003
 * @tc.name: external_file_access_notify_0003
 * @tc.desc: Test function of RegisterNotify and UnregisterNotify interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionNotifyTest, external_file_access_notify_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-begin external_file_access_notify_0003";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        bool notifyForDescendants = true;
        sptr<IFileAccessObserver> myObserver1 = new (std::nothrow) MyObserver();
        sptr<IFileAccessObserver> myObserver2 = new (std::nothrow) MyObserver();
        Uri parentUri(info[1].uri);
        Uri newDirUriTest1("");
        result = g_fah->Mkdir(parentUri, "uri_dir1", newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newFileUri1("");
        result = g_fah->CreateFile(parentUri, "uri_file1", newFileUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->RegisterNotify(newDirUriTest1, notifyForDescendants, myObserver1);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->RegisterNotify(newDirUriTest1, notifyForDescendants, myObserver1);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);

        result = g_fah->RegisterNotify(newFileUri1, notifyForDescendants, myObserver2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->RegisterNotify(newFileUri1, notifyForDescendants, myObserver2);
        EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->UnregisterNotify(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->UnregisterNotify(newFileUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newDirUriTest1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newFileUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_notify_0003 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-end external_file_access_notify_0003";
}

static tuple<Uri, Uri, Uri, Uri> ReadyRegisterNotify05(Uri& parentUri, sptr<IFileAccessObserver>& myObserver1)
{
    bool notifyForDescendants = true;
    Uri uri_dir("");
    int result = g_fah->Mkdir(parentUri, "uri_dir123", uri_dir);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri uri_dirSub1("");
    result = g_fah->Mkdir(uri_dir, "uri_dirSub1", uri_dirSub1);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    result = g_fah->RegisterNotify(uri_dir, notifyForDescendants, myObserver1);
    EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    result = g_fah->RegisterNotify(uri_dirSub1, notifyForDescendants, myObserver1);
    EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    Uri uri_dirSub2("");
    result = g_fah->Mkdir(uri_dir, "uri_dirSub2", uri_dirSub2);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    usleep(SLEEP_TIME);
    EXPECT_EQ(g_notifyEvent, ADD_EVENT);
    EXPECT_EQ(g_notifyUri, uri_dirSub2.ToString());
    result = g_fah->RegisterNotify(uri_dirSub2, notifyForDescendants, myObserver1);
    EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    Uri renameDirUri1("");
    result = g_fah->Rename(uri_dirSub2, "renameDir1", renameDirUri1);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    usleep(SLEEP_TIME);
    return {uri_dir, uri_dirSub1, uri_dirSub2, renameDirUri1};
}

/**
 * @tc.number: user_file_service_external_file_access_notify_0005
 * @tc.name: external_file_access_notify_0005
 * @tc.desc: Test function of RegisterNotify and UnregisterNotify interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionNotifyTest, external_file_access_notify_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-begin external_file_access_notify_0005";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        sptr<IFileAccessObserver> myObserver1 = new (std::nothrow) MyObserver();
        Uri parentUri(info[1].uri);
        auto [uri_dir, uri_dirSub1, uri_dirSub2, renameDirUri1] = ReadyRegisterNotify05(parentUri, myObserver1);
        if (g_notifyEvent != MOVED_TO) {
            if (g_notifyEvent != MOVED_SELF) {
                EXPECT_EQ(g_notifyEvent, MOVED_FROM);
                EXPECT_EQ(g_notifyUri, uri_dirSub2.ToString());
            } else {
                EXPECT_EQ(g_notifyEvent, MOVED_SELF);
                EXPECT_EQ(g_notifyUri, uri_dirSub2.ToString());
            }
        } else {
            EXPECT_EQ(g_notifyEvent, MOVED_TO);
            EXPECT_EQ(g_notifyUri, renameDirUri1.ToString());
        }
        result = g_fah->Delete(uri_dirSub1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        usleep(SLEEP_TIME);
        EXPECT_EQ(g_notifyEvent, DELETE_EVENT);
        EXPECT_EQ(g_notifyUri, uri_dirSub1.ToString());

        sleep(2);
        result = g_fah->UnregisterNotify(uri_dir, myObserver1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(renameDirUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(uri_dir);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        sleep(1);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_notify_0005 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-end external_file_access_notify_0005";
}

/**
 * @tc.number: user_file_service_external_file_access_notify_0006
 * @tc.name: external_file_access_notify_0006
 * @tc.desc: Duplicate UnregisterNotify failed.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionNotifyTest, external_file_access_notify_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-begin external_file_access_notify_0006";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        bool notifyForDescendants = true;
        sptr<IFileAccessObserver> myObserver1 = new (std::nothrow) MyObserver();
        Uri parentUri(info[1].uri);
        Uri newFileUri1("");
        result = g_fah->CreateFile(parentUri, "uri_file", newFileUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->RegisterNotify(newFileUri1, notifyForDescendants, myObserver1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newFileUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        usleep(SLEEP_TIME);
        EXPECT_EQ(g_notifyEvent, DELETE_EVENT);
        EXPECT_EQ(g_notifyUri, newFileUri1.ToString());
        sleep(1);
        result = g_fah->UnregisterNotify(newFileUri1, myObserver1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->UnregisterNotify(newFileUri1, myObserver1);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_notify_0006 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-end external_file_access_notify_0006";
}

/**
 * @tc.number: user_file_service_external_file_access_notify_0007
 * @tc.name: external_file_access_notify_0007
 * @tc.desc: Test function of RegisterNotify and UnregisterNotify failed when callback is Invalid.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionNotifyTest, external_file_access_notify_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-begin external_file_access_notify_0007";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        sptr<IFileAccessObserver> myObserver1 = nullptr;
        bool notifyForDescendants = true;
        Uri parentUri(info[1].uri);
        Uri newFileUri1("");
        result = g_fah->CreateFile(parentUri, "uri_file007", newFileUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->RegisterNotify(newFileUri1, notifyForDescendants, myObserver1);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->UnregisterNotify(newFileUri1, myObserver1);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newFileUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_notify_0007 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-end external_file_access_notify_0007";
}

/**
 * @tc.number: user_file_service_external_file_access_notify_0008
 * @tc.name: external_file_access_notify_0008
 * @tc.desc: Test function of UnregisterNotify failed when uri is Invalid.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionNotifyTest, external_file_access_notify_0008, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-begin external_file_access_notify_0008";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        sptr<IFileAccessObserver> myObserver1 = new (std::nothrow) MyObserver();
        Uri newFileUri1("*/&%");
        bool notifyForDescendants = true;
        result = g_fah->RegisterNotify(newFileUri1, notifyForDescendants, myObserver1);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->UnregisterNotify(newFileUri1, myObserver1);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_notify_0008 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-end external_file_access_notify_0008";
}

/**
 * @tc.number: user_file_service_external_file_access_notify_0009
 * @tc.name: external_file_access_notify_0009
 * @tc.desc: Test function of RegisterNotify and UnregisterNotify interface for SUCCESS when the file is Chinese name.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionNotifyTest, external_file_access_notify_0009, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-begin external_file_access_notify_0009";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        bool notifyForDescendants = true;
        sptr<IFileAccessObserver> myObserver1 = new (std::nothrow) MyObserver();
        Uri parentUri(info[1].uri);
        Uri newFileUri1("");
        result = g_fah->CreateFile(parentUri, "测试文件", newFileUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->RegisterNotify(newFileUri1, notifyForDescendants, myObserver1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newFileUri1);
        usleep(SLEEP_TIME);
        EXPECT_EQ(g_notifyEvent, DELETE_EVENT);
        EXPECT_EQ(g_notifyUri, newFileUri1.ToString());
        sleep(1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->UnregisterNotify(newFileUri1, myObserver1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_notify_0009 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-end external_file_access_notify_0009";
}

/**
 * @tc.number: user_file_service_external_file_access_notify_0010
 * @tc.name: external_file_access_notify_0010
 * @tc.desc: Test function of UnregisterNotify interface for failed when the Uri and callback do not match.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionNotifyTest, external_file_access_notify_0010, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-begin external_file_access_notify_0010";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        bool notifyForDescendants = true;
        sptr<IFileAccessObserver> myObserver1 = new (std::nothrow) MyObserver();
        sptr<IFileAccessObserver> myObserver2 = new (std::nothrow) MyObserver();
        Uri parentUri(info[1].uri);
        Uri newFileUri1("");
        result = g_fah->CreateFile(parentUri, "uri_file1", newFileUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri newFileUri2("");
        result = g_fah->CreateFile(parentUri, "uri_file2", newFileUri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->RegisterNotify(newFileUri1, notifyForDescendants, myObserver1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->RegisterNotify(newFileUri2, notifyForDescendants, myObserver2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = g_fah->UnregisterNotify(newFileUri1, myObserver2);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->UnregisterNotify(newFileUri1, myObserver1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->UnregisterNotify(newFileUri2, myObserver2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newFileUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newFileUri2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_notify_0010 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-end external_file_access_notify_0010";
}

static tuple<Uri, Uri> ReadyRegisterNotify11(Uri& parentUri, Uri &newFileDir1, sptr<IFileAccessObserver>& myObserver1,
    sptr<IFileAccessObserver>& myObserver2)
{
    const int tm = SLEEP_TIME * 2;
    bool notifyForDescendants1 = true;
    int result = g_fah->Mkdir(parentUri, "uri_dir11", newFileDir1);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri newFileUri1("");
    result = g_fah->CreateFile(newFileDir1, "uri_file11", newFileUri1);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    sleep(1);
    result = g_fah->RegisterNotify(newFileDir1, notifyForDescendants1, myObserver1);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    result = g_fah->RegisterNotify(newFileUri1, notifyForDescendants1, myObserver2);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    Uri renameFileUri1("");
    result = g_fah->Rename(newFileUri1, "renamefile1", renameFileUri1);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    usleep(tm);

    return {newFileUri1, renameFileUri1};
}

/**
 * @tc.number: user_file_service_external_file_access_notify_0011
 * @tc.name: external_file_access_notify_0011
 * @tc.desc: Test function of RegisterNotify interface for SUCCESS when set the notifyForDescendants to true and
 * @tc.desc: then set to false
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionNotifyTest, external_file_access_notify_0011, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-begin external_file_access_notify_0011";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        bool notifyForDescendants2 = false;
        sptr<IFileAccessObserver> myObserver1 = new (std::nothrow) TestObserver();
        sptr<IFileAccessObserver> myObserver2 = new (std::nothrow) MyObserver();
        Uri parentUri(info[1].uri);
        Uri newFileDir1("");
        auto [newFileUri1, renameFileUri1] =
            ReadyRegisterNotify11(parentUri, newFileDir1, myObserver1, myObserver2);
        usleep(SLEEP_TIME * 2);
        if (g_notifyEvent != MOVED_TO) {
            if (g_notifyEvent != MOVED_SELF) {
                EXPECT_EQ(g_notifyEvent, MOVED_FROM);
                EXPECT_EQ(g_notifyUri, newFileUri1.ToString());
            } else {
                EXPECT_EQ(g_notifyEvent, MOVED_SELF);
                EXPECT_EQ(g_notifyUri, newFileUri1.ToString());
            }
        } else {
            EXPECT_EQ(g_notifyEvent, MOVED_TO);
            EXPECT_EQ(g_notifyUri, renameFileUri1.ToString());
        }
        result = g_fah->RegisterNotify(newFileDir1, notifyForDescendants2, myObserver1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        sleep(2);
        result = g_fah->Delete(renameFileUri1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        usleep(SLEEP_TIME);
        EXPECT_EQ(g_notifyEvent, DELETE_EVENT);
        EXPECT_EQ(g_notifyUri, newFileUri1.ToString());
        EXPECT_NE(g_notifyFlag, DELETE_EVENT);

        sleep(1);
        result = g_fah->UnregisterNotify(newFileDir1, myObserver1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->UnregisterNotify(newFileUri1, myObserver2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->Delete(newFileDir1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_notify_0011 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-end external_file_access_notify_0011";
}

static void RegisterDirNotify(Uri parentUri, std::string dirName, IFileAccessObserver *observer)
{
    Uri newDirUriTest("");
    bool notifyForDescendants = true;
    int result = g_fah->Mkdir(parentUri, dirName, newDirUriTest);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    sptr<IFileAccessObserver> myObserver(observer);
    result = g_fah->RegisterNotify(newDirUriTest, notifyForDescendants, myObserver);
    EXPECT_GE(result, OHOS::FileAccessFwk::ERR_OK);
    result = g_fah->Delete(newDirUriTest);
    EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    usleep(SLEEP_TIME);
    EXPECT_EQ(g_notifyEvent, DELETE_EVENT);
    EXPECT_EQ(g_notifyUri, newDirUriTest.ToString());
    sleep(2);
    g_fah->UnregisterNotify(newDirUriTest, myObserver);
}

/**
 * @tc.number: user_file_service_external_file_access_notify_0012
 * @tc.name: external_file_access_notify_0012
 * @tc.desc: Test function of RegisterNotify and UnregisterNotify interface for SUCCESS which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionNotifyTest, external_file_access_notify_00012, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-begin external_file_access_notify_00012";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[1].uri);
        Uri newFileUri1("");
        GTEST_LOG_(INFO) << parentUri.ToString();
        sptr<IFileAccessObserver> myObserver1 = new (std::nothrow) MyObserver();
        for (int i = 0; i < INIT_THREADS_NUMBER; i++) {
            std::thread execthread1(RegisterDirNotify, parentUri, "WatcherTest", myObserver1.GetRefPtr());
            execthread1.join();
        }
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_notify_00012 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-end external_file_access_notify_00012";
}

/**
 * @tc.number: user_file_service_external_file_access_notify_0013
 * @tc.name: external_file_access_notify_0013
 * @tc.desc: Test function of RegisterNotify and UnregisterNotify interface for SUCCESS which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionNotifyTest, external_file_access_notify_00013, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-begin external_file_access_notify_00013";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[1].uri);
        GTEST_LOG_(INFO) << parentUri.ToString();
        sptr<IFileAccessObserver> myObserver1 = new (std::nothrow) MyObserver();
        std::thread execthread1(RegisterDirNotify, parentUri, "WatcherTest1", myObserver1.GetRefPtr());
        execthread1.join();
        std::thread execthread2(RegisterDirNotify, parentUri, "WatcherTest2", myObserver1.GetRefPtr());
        execthread2.join();
        std::thread execthread3(RegisterDirNotify, parentUri, "WatcherTest3", myObserver1.GetRefPtr());
        execthread3.join();
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_notify_00013 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-end external_file_access_notify_00013";
}

/**
 * @tc.number: user_file_service_external_file_access_notify_0014
 * @tc.name: external_file_access_notify_0014
 * @tc.desc: Test function of RegisterNotify and UnregisterNotify interface for SUCCESS which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionNotifyTest, external_file_access_notify_00014, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-begin external_file_access_notify_00014";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[1].uri);
        GTEST_LOG_(INFO) << parentUri.ToString();
        sptr<IFileAccessObserver> myObserver1 = new (std::nothrow) MyObserver();
        sptr<IFileAccessObserver> myObserver2 = new (std::nothrow) MyObserver();
        sptr<IFileAccessObserver> myObserver3 = new (std::nothrow) MyObserver();

        std::thread execthread1(RegisterDirNotify, parentUri, "WatcherTest", myObserver1.GetRefPtr());
        execthread1.join();
        sleep(1);
        std::thread execthread2(RegisterDirNotify, parentUri, "WatcherTest", myObserver2.GetRefPtr());
        execthread2.join();
        sleep(1);
        std::thread execthread3(RegisterDirNotify, parentUri, "WatcherTest", myObserver3.GetRefPtr());
        execthread3.join();
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_notify_00014 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-end external_file_access_notify_00014";
}

/**
 * @tc.number: user_file_service_external_file_access_notify_0015
 * @tc.name: external_file_access_notify_0015
 * @tc.desc: Test function of RegisterNotify and UnregisterNotify interface for SUCCESS which Concurrent.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionNotifyTest, external_file_access_notify_00015, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-begin external_file_access_notify_00015";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri parentUri(info[1].uri);
        GTEST_LOG_(INFO) << parentUri.ToString();
        sptr<IFileAccessObserver> myObserver1 = new (std::nothrow) MyObserver();
        sptr<IFileAccessObserver> myObserver2 = new (std::nothrow) MyObserver();
        sptr<IFileAccessObserver> myObserver3 = new (std::nothrow) MyObserver();

        std::thread execthread1(RegisterDirNotify, parentUri, "WatcherTest1", myObserver1.GetRefPtr());
        execthread1.join();
        std::thread execthread2(RegisterDirNotify, parentUri, "WatcherTest2", myObserver2.GetRefPtr());
        execthread2.join();
        std::thread execthread3(RegisterDirNotify, parentUri, "WatcherTest3", myObserver3.GetRefPtr());
        execthread3.join();
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_notify_00015 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-end external_file_access_notify_00015";
}

/**
 * @tc.number: user_file_service_external_file_access_notify_0016
 * @tc.name: external_file_access_notify_0016
 * @tc.desc: Test UnregisterNotify all callbacks related to the current uri
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionNotifyTest, external_file_access_notify_0016, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-begin external_file_access_notify_0016";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        bool notifyForDescendants = true;
        sptr<IFileAccessObserver> myObserver1 = new (std::nothrow) MyObserver();
        sptr<IFileAccessObserver> myObserver2 = new (std::nothrow) MyObserver();
        sptr<IFileAccessObserver> myObserver3 = new (std::nothrow) MyObserver();
        Uri parentUri(info[1].uri);
        Uri newFileDir1("");
        result = g_fah->Mkdir(parentUri, "uri_dir0016", newFileDir1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->RegisterNotify(newFileDir1, notifyForDescendants, myObserver1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->RegisterNotify(newFileDir1, notifyForDescendants, myObserver2);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->RegisterNotify(newFileDir1, notifyForDescendants, myObserver3);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = g_fah->UnregisterNotify(newFileDir1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->UnregisterNotify(newFileDir1, myObserver1);
        EXPECT_NE(result, OHOS::FileAccessFwk::ERR_OK);

        result = g_fah->Delete(newFileDir1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_notify_0016 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-end external_file_access_notify_0016";
}

/**
 * @tc.number: user_file_service_external_file_access_notify_0017
 * @tc.name: external_file_access_notify_0017
 * @tc.desc: Test event changes exceeding 500ms trigger callback
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionNotifyTest, external_file_access_notify_0017, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-begin external_file_access_notify_0017";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        bool notifyForDescendants = true;
        sptr<IFileAccessObserver> myObserver1 = new (std::nothrow) MyObserver();
        Uri parentUri(info[1].uri);
        Uri newFileDir1("");
        result = g_fah->Mkdir(parentUri, "uri_dir", newFileDir1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->RegisterNotify(newFileDir1, notifyForDescendants, myObserver1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri fileUri("");
        result = g_fah->CreateFile(newFileDir1, "uri_file", fileUri);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        usleep(SLEEP_TIME / 10);
        EXPECT_NE(g_notifyEvent, ADD_EVENT);
        usleep(SLEEP_TIME);
        EXPECT_EQ(g_notifyEvent, ADD_EVENT);

        result = g_fah->UnregisterNotify(newFileDir1, myObserver1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = g_fah->Delete(newFileDir1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_notify_0017 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-end external_file_access_notify_0017";
}

/**
 * @tc.number: user_file_service_external_file_access_notify_0018
 * @tc.name: external_file_access_notify_0018
 * @tc.desc: Test event changes More than 32 notifications callback
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionNotifyTest, external_file_access_notify_0018, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-begin external_file_access_notify_0018";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        bool notifyForDescendants = true;
        sptr<IFileAccessObserver> myObserver1 = new (std::nothrow) TestObserver();
        Uri parentUri(info[1].uri);
        Uri newFileDir1("");
        result = g_fah->Mkdir(parentUri, "uri_dir", newFileDir1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->RegisterNotify(newFileDir1, notifyForDescendants, myObserver1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri fileUri("");
        for (int i = 0; i < 64; i++) {
            result = g_fah->CreateFile(newFileDir1, "uri_file" + to_string(i), fileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
        sleep(1);
        EXPECT_LE(g_notifyUris.size(), 32);
        g_notifyUris.clear();
        result = g_fah->UnregisterNotify(newFileDir1, myObserver1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = g_fah->Delete(newFileDir1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_notify_0018 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-end external_file_access_notify_0018";
}

/**
 * @tc.number: user_file_service_external_file_access_notify_0019
 * @tc.name: external_file_access_notify_0019
 * @tc.desc: Test event changes More than one notifications callback
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0386
 */
HWTEST_F(FileExtensionNotifyTest, external_file_access_notify_0019, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-begin external_file_access_notify_0019";
    try {
        vector<RootInfo> info;
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        bool notifyForDescendants = true;
        sptr<IFileAccessObserver> myObserver1 = new (std::nothrow) TestObserver();
        Uri parentUri(info[1].uri);
        Uri newFileDir1("");
        result = g_fah->Mkdir(parentUri, "uri_dir", newFileDir1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        result = g_fah->RegisterNotify(newFileDir1, notifyForDescendants, myObserver1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        Uri fileUri("");
        for (int i = 0; i < 10; i++) {
            result = g_fah->CreateFile(newFileDir1, "uri_file" + to_string(i), fileUri);
            EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
        }
        usleep(SLEEP_TIME);
        EXPECT_GT(g_notifyUris.size(), 1);
        g_notifyUris.clear();
        result = g_fah->UnregisterNotify(newFileDir1, myObserver1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        result = g_fah->Delete(newFileDir1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);
    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_notify_0019 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-end external_file_access_notify_0019";
}

HWTEST_F(FileExtensionNotifyTest, external_file_access_notify_0020, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-begin external_file_access_notify_0020";
    try {
        g_notifyEvent = -1;
        vector<RootInfo> info;
        const char* perms[] = {
            "ohos.permission.GET_BUNDLE_INFO_PRIVILEGED"
        };
        SetNativeToken("SetUpTestCase", perms, sizeof(perms) / sizeof(perms[0]));
        int result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::E_PERMISSION);
        SetNativeToken();
        result = g_fah->GetRoots(info);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        bool notifyForDescendants = true;
        sptr<IFileAccessObserver> myObserver1 = new (std::nothrow) MyObserver();
        Uri parentUri(info[1].uri);
        Uri newFileDir1("");
        SetNativeToken("SetUpTestCase", perms, sizeof(perms) / sizeof(perms[0]));
        result = g_fah->Mkdir(parentUri, "uri_dir", newFileDir1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::E_PERMISSION);
        SetNativeToken();
        result = g_fah->Mkdir(parentUri, "uri_dir", newFileDir1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

        SetNativeToken("SetUpTestCase", perms, sizeof(perms) / sizeof(perms[0]));
        result = g_fah->RegisterNotify(newFileDir1, notifyForDescendants, myObserver1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::E_PERMISSION);

        result = g_fah->UnregisterNotify(newFileDir1, myObserver1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::E_PERMISSION);

        result = g_fah->Delete(newFileDir1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::E_PERMISSION);

        SetNativeToken();
        result = g_fah->Delete(newFileDir1);
        EXPECT_EQ(result, OHOS::FileAccessFwk::ERR_OK);

    } catch (...) {
        GTEST_LOG_(ERROR) << "external_file_access_notify_0020 occurs an exception.";
    }
    GTEST_LOG_(INFO) << "FileExtensionNotifyTest-end external_file_access_notify_0020";
}
} // namespace