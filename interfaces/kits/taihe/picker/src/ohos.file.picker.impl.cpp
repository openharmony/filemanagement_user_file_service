/*
 * Copyright (C) 2025-2026 Huawei Device Co., Ltd.
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
#include "ani.h"
#include "taihe/runtime.hpp"
#include "taihe/platform/ani.hpp"
#include "ohos.file.picker.proj.hpp"
#include "ohos.file.picker.impl.hpp"
#include "stdexcept"

#include "taihe_modal_ui_callback.h"
#include "modal_ui_extension_config.h"
#include "ability_context.h"
#include "context.h"
#include "ability.h"
// #ifdef HAS_ACE_ENGINE_PART
#include "ui_content.h"
// #endif
#include "ui_extension_context.h"
#include "want.h"
// #ifdef UDMF_ENABLED
#include "unified_data.h"
#include "udmf_client.h"
#include "unified_record.h"
#include "unified_meta.h"
#include "unified_types.h"
// #endif

#include "ani_window.h"
#include "ani_base_context.h"
#include "array_wrapper.h"
#include "int_wrapper.h"
#include "bool_wrapper.h"
#include "byte_wrapper.h"
#include "double_wrapper.h"
#include "float_wrapper.h"
#include "long_wrapper.h"
#include "short_wrapper.h"
#include "user_object_wrapper.h"
#include "zchar_wrapper.h"
#include <memory>
#include <thread>
#include <chrono>
#include "hilog_wrapper.h"

#define ABILITY_WANT_PARAMS_UIEXTENSIONTARGETTYPE "ability.want.params.uiExtensionTargetType"
#define WAIT_TIME_MS 100
static const std::string PHOTO_URIS_KEY = "photoUris";
const char *ABILITY_WANT_PARAMS_UDKEY = "ability.want.params.udKey";

namespace {
    // To be implemented.
    using namespace std;
    using ModalUICallback = OHOS::Picker::ModalUICallback;
    using AniPickerCallBack = OHOS::Picker::AniPickerCallBack;
    using AniPickerAsyncContext = OHOS::Picker::AniPickerAsyncContext;
    using AniInsertAsyncContext = OHOS::Picker::AniInsertAsyncContext;
    using DocumentSelectOptions = ::ohos::file::picker::DocumentSelectOptions;
    using DocumentSaveOptions = ::ohos::file::picker::DocumentSaveOptions;
    using DocumentSelectMode = ::ohos::file::picker::DocumentSelectMode;
    using MergeTypeMode = ::ohos::file::picker::MergeTypeMode;
    using AudioSelectOptions = ::ohos::file::picker::AudioSelectOptions;
    using AudioSaveOptions = ::ohos::file::picker::AudioSaveOptions;
    using DocumentPickerMode = ::ohos::file::picker::DocumentPickerMode;
    using Want = OHOS::AAFwk::Want;
    using WantParams = OHOS::AAFwk::WantParams;
    using UIContent = OHOS::Ace::UIContent;
    using String = OHOS::AAFwk::String;
    using Array = OHOS::AAFwk::Array;
    using Integer = OHOS::AAFwk::Integer;
    using Boolean = OHOS::AAFwk::Boolean;
    using UIExtensionContext = OHOS::AbilityRuntime::UIExtensionContext;

    void setAudioPickerSaveParameters(const AudioSaveOptions& option, WantParams& params,
        OHOS::sptr<OHOS::AAFwk::IArray>& ao)
    {
        params.SetParam("key_pick_file_name", ao);
        auto itTemp = option.newFileNames.value().begin();
        std::string saveFile(*itTemp);
        params.SetParam("saveFile", String::Box(saveFile));
    }

    void WaitForPickerReady(const std::shared_ptr<AniPickerAsyncContext>& context)
    {
        if (!context || !context->pickerCallBack) {
            return;
        }
        while (!context->pickerCallBack->ready) {
            std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME_MS));
        }
    }

    OHOS::Ace::UIContent *GetUIContent(ani_env *env, const ani_ref &contextRef)
    {
        HILOG_INFO("Begin");
        if (env == nullptr) {
            HILOG_ERROR("env == nullptr");
            return nullptr;
        }

        ani_boolean isValue = false;
        ani_boolean isUndefined = false;
        env->Reference_IsNullishValue(contextRef, &isValue);
        env->Reference_IsUndefined(contextRef, &isUndefined);
        if (isUndefined || isValue) {
            HILOG_ERROR("context is undefined or Null");
            return nullptr;
        }

        ani_boolean isStageMode = ANI_FALSE;
        ani_status status = OHOS::AbilityRuntime::IsStageContext(env,
            reinterpret_cast<ani_object>(contextRef), isStageMode);
        if (status != ANI_OK || !isStageMode) {
            HILOG_ERROR("[picker]: is not StageMode context");
            ::taihe::set_business_error(-1, "[picker]: is not StageMode context");
            return nullptr;
        }

        auto context = OHOS::AbilityRuntime::GetStageModeContext(env, reinterpret_cast<ani_object>(contextRef));
        if (context == nullptr) {
            ::taihe::set_business_error(-1, "[picker]: Failed to get native stage context instance");
            return nullptr;
        }
        auto abilityContext = OHOS::AbilityRuntime::Context::ConvertTo<OHOS::AbilityRuntime::AbilityContext>(context);
        if (abilityContext == nullptr) {
            auto uiExtensionContext = OHOS::AbilityRuntime::Context::ConvertTo<UIExtensionContext>(context);
            if (uiExtensionContext == nullptr) {
                ::taihe::set_business_error(-1, "[picker]: Fail to convert to abilityContext or uiExtensionContext");
                return nullptr;
            }
            return uiExtensionContext->GetUIContent();
        }
        return abilityContext->GetUIContent();
    }

    void GetCustomShowingWindow(ani_env *env, const ani_ref &windowRef, OHOS::sptr<OHOS::Rosen::Window> &window)
    {
        HILOG_INFO("Begin");
        if (env == nullptr) {
            HILOG_ERROR("env == nullptr");
            return;
        }
        ani_boolean isValue = false;
        ani_boolean isUndefined = false;
        env->Reference_IsNullishValue(windowRef, &isValue);
        env->Reference_IsUndefined(windowRef, &isUndefined);
        if (isUndefined || isValue) {
            HILOG_ERROR("window is undefined or Null");
            return;
        }
        OHOS::Rosen::AniWindow *ani_window = OHOS::Rosen::AniWindow::GetWindowObjectFromEnv(env,
            reinterpret_cast<ani_object>(windowRef));
        if (ani_window == nullptr) {
            HILOG_ERROR("GetWindowObjectFromEnv failed");
            ::taihe::set_business_error(-1, "GetWindowObjectFromEnv");
            return;
        }
        ani_object property = ani_window->GetWindowPropertiesSync(env);
        ani_string name;
        ani_status status = env->Object_GetPropertyByName_Ref(property, "name", reinterpret_cast<ani_ref *>(&name));
        if (status != ANI_OK) {
            HILOG_ERROR("Object_GetPropertyByName_Ref failed, status = %{public}d", status);
            ::taihe::set_business_error(-1, "Get Window name failed");
            return;
        }
        
        ani_size len;
        env->String_GetUTF8Size(name, &len);
        char *nameUtf8 = new char[len + 1];
        env->String_GetUTF8(name, nameUtf8, len + 1, &len);
        auto customWindow = OHOS::Rosen::Window::Find(nameUtf8);
        if (!customWindow) {
            delete[] nameUtf8;
            return;
        }
        window = customWindow;
        delete[] nameUtf8;
    }

    std::shared_ptr<AniPickerAsyncContext> StartPickerExtension(ani_vm *vm, const ani_ref &contextRef,
                                                                const ani_ref &windowRef, Want &request)
    {
        HILOG_INFO("Begin");

        if (vm == nullptr) {
            HILOG_ERROR("vm == nullptr");
            return nullptr;
        }

        ani_env* env = nullptr;
        ani_options aniArgs {0, nullptr};
        if (ANI_OK != vm->AttachCurrentThread(&aniArgs, ANI_VERSION_1, &env)) {
            env = nullptr;
            if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
                HILOG_ERROR("env == nullptr");
                return nullptr;
            }
        }

        if (env == nullptr) {
            HILOG_ERROR("env == nullptr");
            if (ANI_OK != vm->DetachCurrentThread()) {
                HILOG_ERROR("DetachCurrentThread failed");
            }
            return nullptr;
        }

        OHOS::sptr<OHOS::Rosen::Window> tempWindow = nullptr;
        UIContent *uiContent = nullptr;
        std::shared_ptr<AniPickerAsyncContext> asyncContext = std::make_shared<AniPickerAsyncContext>();
        GetCustomShowingWindow(env, windowRef, tempWindow);
        if (tempWindow) {
            uiContent = tempWindow->GetUIContent();
        } else {
            uiContent = GetUIContent(env, contextRef);
        }

        if (uiContent == nullptr) {
            if (ANI_OK != vm->DetachCurrentThread()) {
                HILOG_ERROR("DetachCurrentThread failed");
            }
            return nullptr;
        }

        std::string targetType = request.GetStringParam("extType");
        std::string pickerType;
        if (request.GetParams().HasParam("pickerType")) {
            pickerType = request.GetStringParam("pickerType");
        }
        request.SetParam(ABILITY_WANT_PARAMS_UIEXTENSIONTARGETTYPE, targetType);
        std::string uriUdkey = request.GetStringParam("ability_params_udkey");
        request.SetParam(ABILITY_WANT_PARAMS_UDKEY, uriUdkey);
        asyncContext->pickerCallBack = make_shared<AniPickerCallBack>();
        auto callback = std::make_shared<ModalUICallback>(uiContent, asyncContext->pickerCallBack);
        OHOS::Ace::ModalUIExtensionCallbacks extensionCallback = {
            .onRelease = std::bind(&ModalUICallback::OnRelease, callback, std::placeholders::_1),
            .onResult = std::bind(&ModalUICallback::OnResultForModal, callback, std::placeholders::_1,
                                  std::placeholders::_2),
            .onReceive = std::bind(&ModalUICallback::OnReceive, callback, std::placeholders::_1),
            .onError = std::bind(&ModalUICallback::OnError, callback, std::placeholders::_1, std::placeholders::_2,
                                 std::placeholders::_3),
            .onDestroy = std::bind(&ModalUICallback::OnDestroy, callback),
        };
        OHOS::Ace::ModalUIExtensionConfig config;
        int sessionId = uiContent->CreateModalUIExtension(request, extensionCallback, config);
        if (sessionId == 0) {
            if (ANI_OK != vm->DetachCurrentThread()) {
                HILOG_ERROR("DetachCurrentThread failed");
            }
            return nullptr;
        }
        callback->SetSessionId(sessionId);
        if (ANI_OK != vm->DetachCurrentThread()) {
            HILOG_ERROR("DetachCurrentThread failed");
        }
        return asyncContext;
    }

    class DocumentViewPickerImpl {
    public:
        DocumentViewPickerImpl()
            : context_(nullptr), window_(nullptr), suffixIndex_(0), vm_(nullptr)
        {
            HILOG_INFO("Begin");
        }

        explicit DocumentViewPickerImpl(uintptr_t context)
            : context_(nullptr), window_(nullptr), suffixIndex_(0), vm_(nullptr)
        {
            HILOG_INFO("Begin");
            taihe::env_guard guard;
            ani_env *env = guard.get_env();

            if (env == nullptr) {
                HILOG_ERROR("env == nullptr");
                return;
            }

            env->GetVM(&vm_);
            if (vm_ == nullptr) {
                HILOG_ERROR("vm == nullptr");
                return;
            }

            ani_object objContext = reinterpret_cast<ani_object>(context);
            ani_ref refContext = static_cast<ani_ref>(objContext);
            ani_boolean isValue = false;
            ani_boolean isUndefined = false;
            env->Reference_IsNullishValue(refContext, &isValue);
            env->Reference_IsUndefined(refContext, &isUndefined);
            if (isUndefined || isValue) {
                HILOG_ERROR("context is Undefined or  Null");
                return;
            }
            if (env->GlobalReference_Create(refContext, &context_) != ANI_OK) {
                HILOG_ERROR("context  global reference error");
                ::taihe::set_business_error(-1, "context  global reference error");
                return;
            }
            if (env->GetUndefined(&window_) != ANI_OK) {
                HILOG_ERROR("window  GetUndefined error");
                ::taihe::set_business_error(-1, "window  GetUndefined error");
            }
        }

        DocumentViewPickerImpl(uintptr_t context, uintptr_t window)
            : context_(nullptr), window_(nullptr), suffixIndex_(0), vm_(nullptr)
        {
            HILOG_INFO("Begin");
            taihe::env_guard guard;
            ani_env *env = guard.get_env();

            if (env == nullptr) {
                HILOG_ERROR("env == nullptr");
                return;
            }

            env->GetVM(&vm_);
            if (vm_ == nullptr) {
                HILOG_ERROR("vm == nullptr");
                return;
            }

            ani_object objContext = reinterpret_cast<ani_object>(context);
            ani_ref refContext = static_cast<ani_ref>(objContext);
            ani_boolean isValue = false;
            ani_boolean isUndefined = false;
            env->Reference_IsNullishValue(refContext, &isValue);
            env->Reference_IsUndefined(refContext, &isUndefined);
            if (isUndefined || isValue) {
                HILOG_ERROR("context is Undefined or  Null");
                return;
            }

            if (env->GlobalReference_Create(refContext, &context_) != ANI_OK) {
                HILOG_ERROR("context  global reference error");
                ::taihe::set_business_error(-1, "context  global reference error");
                return;
            }

            ani_object objWindow = reinterpret_cast<ani_object>(window);
            ani_ref refWindow = static_cast<ani_ref>(objWindow);
            isValue = false;
            isUndefined = false;
            env->Reference_IsNullishValue(refWindow, &isValue);
            env->Reference_IsUndefined(refWindow, &isUndefined);
            if (isUndefined || isValue) {
                HILOG_ERROR("window is Undefined or  Null");
                return;
            }

            if (env->GlobalReference_Create(refWindow, &window_) != ANI_OK) {
                HILOG_ERROR("window  global reference error");
                ::taihe::set_business_error(-1, "window  global reference error");
                return;
            }
        }

        ~DocumentViewPickerImpl()
        {
            HILOG_INFO("Begin");
            taihe::env_guard guard;
            ani_env *env = guard.get_env();
            if (env == nullptr) {
                HILOG_ERROR("env == nullptr");
                return;
            }
            env->GlobalReference_Delete(context_);
            env->GlobalReference_Delete(window_);
        }

        ::taihe::array<::taihe::string> SelectSync(::taihe::optional_view<DocumentSelectOptions> option)
        {
            HILOG_INFO("Begin");
            OHOS::sptr<OHOS::Rosen::Window> tempWindow = nullptr;
            Want request;
            request.SetAction(std::string("ohos.want.action.OPEN_FILE_SERVICE"));
            WantParams &parameters = const_cast<WantParams &>(request.GetParams());
            parameters.SetParam("startMode", String::Box(std::string("choose")));
            parameters.SetParam("extType", String::Box(std::string("filePicker")));
            parameters.SetParam("pickerType", String::Box(std::string("select")));

            if (option.has_value()) {
                DocumentSelectOptions aniOption = option.value();
                if (aniOption.defaultFilePathUri.has_value()) {
                    parameters.SetParam("key_pick_dir_path",
                                        String::Box(std::string(aniOption.defaultFilePathUri.value())));
                }
                if (aniOption.fileSuffixFilters.has_value()) {
                    uint32_t length = aniOption.fileSuffixFilters.value().size();
                    int i = 0;
                    OHOS::sptr<OHOS::AAFwk::IArray> ao = OHOS::sptr<OHOS::AAFwk::Array>::MakeSptr(length,
                        OHOS::AAFwk::g_IID_IString);
                    for (auto it = aniOption.fileSuffixFilters.value().begin();
                         it != aniOption.fileSuffixFilters.value().end(); ++it) {
                        ao->Set(i, OHOS::AAFwk::String::Box(std::string(*it)));
                        i++;
                    }
                    parameters.SetParam("key_file_suffix_filter", ao);
                }
                if (aniOption.maxSelectNumber.has_value()) {
                    parameters.SetParam("key_pick_num", Integer::Box(aniOption.maxSelectNumber.value()));
                }
                if (aniOption.selectMode.has_value()) {
                    parameters.SetParam("key_select_mode", Integer::Box(aniOption.selectMode.value()));
                }
                if (aniOption.authMode.has_value()) {
                    parameters.SetParam("key_auth_mode", Boolean::Box(aniOption.authMode.value()));
                }
                if (aniOption.multiAuthMode.has_value()) {
                    parameters.SetParam("key_mult_auth_mode", Boolean::Box(aniOption.multiAuthMode.value()));
                }
                if (aniOption.multiUriArray.has_value()) {
                    std::unique_ptr<AniInsertAsyncContext> asyncContext = std::make_unique<AniInsertAsyncContext>();
                    std::vector<::taihe::string> uriVec;
                    OHOS::UDMF::CustomOption option = {.intention = OHOS::UDMF::Intention::UD_INTENTION_PICKER};
                    OHOS::UDMF::UnifiedData unifiedData;
                    for (auto it = aniOption.multiUriArray.value().begin();
                         it != aniOption.multiUriArray.value().end(); ++it) {
                        uriVec.push_back(*it);
                        std::shared_ptr<OHOS::UDMF::Object> fileObj = std::make_shared<OHOS::UDMF::Object>();
                        fileObj->value_[OHOS::UDMF::UNIFORM_DATA_TYPE] = "general.file-uri";
                        fileObj->value_[OHOS::UDMF::FILE_URI_PARAM] = std::string(*it);
                        fileObj->value_[OHOS::UDMF::FILE_TYPE] = "general.file";
                        std::shared_ptr<OHOS::UDMF::UnifiedRecord> file =
                            std::make_shared<OHOS::UDMF::UnifiedRecord>(OHOS::UDMF::UDType::FILE_URI, fileObj);
                        unifiedData.AddRecord(file);
                    }
                    OHOS::UDMF::UdmfClient::GetInstance().SetData(option, unifiedData, asyncContext->udKey);
                    parameters.SetParam("ability_params_udkey", String::Box(asyncContext->udKey));
                }
            }

            std::shared_ptr<AniPickerAsyncContext> aniPickerAsyncContext = nullptr;
            auto task = [this, &request, &aniPickerAsyncContext]() {
                aniPickerAsyncContext = StartPickerExtension(this->vm_, this->context_, this->window_, request);
                WaitForPickerReady(aniPickerAsyncContext);
            };
            std::thread taskThread(task);
            taskThread.join();

            std::vector<std::string> list;
            if (aniPickerAsyncContext) {
                if (aniPickerAsyncContext->pickerCallBack) {
                    if (aniPickerAsyncContext->pickerCallBack->want.GetParams().HasParam("ability.params.stream")) {
                        list = aniPickerAsyncContext->pickerCallBack->want.GetStringArrayParam("ability.params.stream");
                    } else if (aniPickerAsyncContext->pickerCallBack->want.GetParams().HasParam("ability.params.udkey")) {
                        list = aniPickerAsyncContext->pickerCallBack->want.GetStringArrayParam("ability.params.udkey");
                    }
                }
            }

            std::vector<::taihe::string> taiheList;
            for (auto it = list.begin(); it != list.end(); ++it) {
                taiheList.push_back(::taihe::string(*it));
            }
            ::taihe::array<::taihe::string> uriNullArray(taiheList);
            return uriNullArray;
        }

        ::taihe::array<::taihe::string> SelectSync1(::ohos::file::picker::DocumentSelectOptions const &option)
        {
            HILOG_INFO("Begin");
            Want request;
            request.SetAction(std::string("ohos.want.action.OPEN_FILE_SERVICE"));
            WantParams &parameters = const_cast<WantParams &>(request.GetParams());
            parameters.SetParam("startMode", String::Box(std::string("choose")));
            parameters.SetParam("extType", String::Box(std::string("filePicker")));
            parameters.SetParam("pickerType", String::Box(std::string("select")));

            if (option.defaultFilePathUri.has_value()) {
                parameters.SetParam("key_pick_dir_path", String::Box(std::string(option.defaultFilePathUri.value())));
            }
            if (option.fileSuffixFilters.has_value()) {
                uint32_t length = option.fileSuffixFilters.value().size();
                int i = 0;
                OHOS::sptr<OHOS::AAFwk::IArray> ao = OHOS::sptr<OHOS::AAFwk::Array>::MakeSptr(length,
                    OHOS::AAFwk::g_IID_IString);
                for (auto it = option.fileSuffixFilters.value().begin();
                     it != option.fileSuffixFilters.value().end(); ++it) {
                    ao->Set(i, String::Box(std::string(*it)));
                    i++;
                }
                parameters.SetParam("key_file_suffix_filter", ao);
            }
            if (option.maxSelectNumber.has_value()) {
                parameters.SetParam("key_pick_num", Integer::Box(option.maxSelectNumber.value()));
            }
            if (option.selectMode.has_value()) {
                parameters.SetParam("key_select_mode", Integer::Box(option.selectMode.value()));
            }
            if (option.authMode.has_value()) {
                parameters.SetParam("key_auth_mode", Boolean::Box(option.authMode.value()));
            }
            if (option.multiAuthMode.has_value()) {
                parameters.SetParam("key_mult_auth_mode", Boolean::Box(option.multiAuthMode.value()));
            }
            if (option.multiUriArray.has_value()) {
                std::unique_ptr<AniInsertAsyncContext> asyncContext = std::make_unique<AniInsertAsyncContext>();
                std::vector<::taihe::string> uriVec;
                OHOS::UDMF::CustomOption customOption = {.intention = OHOS::UDMF::Intention::UD_INTENTION_PICKER};
                OHOS::UDMF::UnifiedData unifiedData;
                for (auto it = option.multiUriArray.value().begin(); it != option.multiUriArray.value().end(); ++it) {
                    uriVec.push_back(*it);
                    std::shared_ptr<OHOS::UDMF::Object> fileObj = std::make_shared<OHOS::UDMF::Object>();
                    fileObj->value_[OHOS::UDMF::UNIFORM_DATA_TYPE] = "general.file-uri";
                    fileObj->value_[OHOS::UDMF::FILE_URI_PARAM] = std::string(*it);
                    fileObj->value_[OHOS::UDMF::FILE_TYPE] = "general.file";
                    std::shared_ptr<OHOS::UDMF::UnifiedRecord> file =
                        std::make_shared<OHOS::UDMF::UnifiedRecord>(OHOS::UDMF::UDType::FILE_URI, fileObj);
                    unifiedData.AddRecord(file);
                }
                OHOS::UDMF::UdmfClient::GetInstance().SetData(customOption, unifiedData, asyncContext->udKey);
                parameters.SetParam("ability_params_udkey", String::Box(asyncContext->udKey));
            }
            std::shared_ptr<AniPickerAsyncContext> aniPickerAsyncContext = nullptr;
            auto task = [this, &request, &aniPickerAsyncContext]() {
                aniPickerAsyncContext = StartPickerExtension(this->vm_, this->context_, this->window_, request);
                WaitForPickerReady(aniPickerAsyncContext);
            };
            std::thread taskThread(task);
            taskThread.join();

            std::vector<std::string> list;
            if (aniPickerAsyncContext) {
                if (aniPickerAsyncContext->pickerCallBack) {
                    if (aniPickerAsyncContext->pickerCallBack->want.GetParams().HasParam("ability.params.stream")) {
                        list = aniPickerAsyncContext->pickerCallBack->want.GetStringArrayParam("ability.params.stream");
                    } else if (aniPickerAsyncContext->pickerCallBack->want.GetParams().HasParam("ability.params.udkey")) {
                        list = aniPickerAsyncContext->pickerCallBack->want.GetStringArrayParam("ability.params.udkey");
                    }
                }
            }

            std::vector<::taihe::string> taiheList;
            for (auto it = list.begin(); it != list.end(); ++it) {
                taiheList.push_back(::taihe::string(*it));
            }
            ::taihe::array<::taihe::string> uriNullArray(taiheList);
            return uriNullArray;
        }

        ::taihe::array<::taihe::string> SelectSync2()
        {
            HILOG_INFO("Begin");
            Want request;
            request.SetAction(std::string("ohos.want.action.OPEN_FILE_SERVICE"));
            WantParams &parameters = const_cast<WantParams &>(request.GetParams());
            parameters.SetParam("startMode", String::Box(std::string("choose")));
            parameters.SetParam("extType", String::Box(std::string("filePicker")));
            parameters.SetParam("pickerType", String::Box(std::string("select")));

            std::shared_ptr<AniPickerAsyncContext> aniPickerAsyncContext = nullptr;
            auto task = [this, &request, &aniPickerAsyncContext]() {
                aniPickerAsyncContext = StartPickerExtension(this->vm_, this->context_, this->window_, request);
                WaitForPickerReady(aniPickerAsyncContext);
            };
            std::thread taskThread(task);
            taskThread.join();

            std::vector<std::string> list;
            if (aniPickerAsyncContext) {
                if (aniPickerAsyncContext->pickerCallBack) {
                    if (aniPickerAsyncContext->pickerCallBack->want.GetParams().HasParam("ability.params.stream")) {
                        list = aniPickerAsyncContext->pickerCallBack->want.GetStringArrayParam("ability.params.stream");
                    } else if (aniPickerAsyncContext->pickerCallBack->want.GetParams().HasParam("ability.params.udkey")) {
                        list = aniPickerAsyncContext->pickerCallBack->want.GetStringArrayParam("ability.params.udkey");
                    }
                }
            }

            std::vector<::taihe::string> taiheList;
            for (auto it = list.begin(); it != list.end(); ++it) {
                taiheList.push_back(::taihe::string(*it));
            }
            ::taihe::array<::taihe::string> uriNullArray(taiheList);
            return uriNullArray;
        }

        ::taihe::array<::taihe::string> SaveSync(::taihe::optional_view<DocumentSaveOptions> option)
        {
            HILOG_INFO("Begin");
            OHOS::sptr<OHOS::Rosen::Window> tempWindow = nullptr;
            Want request;
            request.SetAction(std::string("ohos.want.action.CREATE_FILE_SERVICE"));
            WantParams &parameters = const_cast<WantParams &>(request.GetParams());
            parameters.SetParam("startMode", String::Box(std::string("save")));
            parameters.SetParam("pickerMode", Integer::Box(0));
            parameters.SetParam("extType", String::Box(std::string("filePicker")));
            parameters.SetParam("pickerType", String::Box(std::string("save")));
            parameters.SetParam("autoCreateEmptyFile", Boolean::Box(true));

            if (option.has_value()) {
                DocumentSaveOptions aniOption = option.value();
                if (aniOption.defaultFilePathUri.has_value()) {
                    parameters.SetParam("key_pick_dir_path",
                                        String::Box(std::string(aniOption.defaultFilePathUri.value())));
                }
                if (aniOption.newFileNames.has_value()) {
                    uint32_t length = aniOption.newFileNames.value().size();
                    OHOS::sptr<OHOS::AAFwk::IArray> ao = OHOS::sptr<OHOS::AAFwk::Array>::MakeSptr(length,
                        OHOS::AAFwk::g_IID_IString);
                    int i = 0;
                    for (auto it = aniOption.newFileNames.value().begin();
                         it != aniOption.newFileNames.value().end(); ++it) {
                        ao->Set(i, OHOS::AAFwk::String::Box(std::string(*it)));
                        i++;
                    }
                    parameters.SetParam("key_pick_file_name", ao);
                    auto itTemp = aniOption.newFileNames.value().begin();
                    std::string saveFile(*itTemp);
                    parameters.SetParam("saveFile", String::Box(saveFile));
                }
                if (aniOption.fileSuffixChoices.has_value()) {
                    uint32_t length = aniOption.fileSuffixChoices.value().size();
                    OHOS::sptr<OHOS::AAFwk::IArray> ao = OHOS::sptr<OHOS::AAFwk::Array>::MakeSptr(length,
                        OHOS::AAFwk::g_IID_IString);
                    int i = 0;
                    for (auto it = aniOption.fileSuffixChoices.value().begin();
                         it != aniOption.fileSuffixChoices.value().end(); ++it) {
                        ao->Set(i, String::Box(std::string(*it)));
                        i++;
                    }
                    parameters.SetParam("key_file_suffix_choices", ao);
                }
                if (aniOption.pickerMode.has_value()) {
                    if (aniOption.pickerMode.value() == 1) {
                        parameters.SetParam("pickerMode", Integer::Box((int)aniOption.pickerMode.value()));
                        parameters.SetParam("pickerType", String::Box(std::string("downloadAuth")));
                    }
                }
            }

            std::shared_ptr<AniPickerAsyncContext> aniPickerAsyncContext = nullptr;
            auto task = [this, &request, &aniPickerAsyncContext]() {
                aniPickerAsyncContext = StartPickerExtension(this->vm_, this->context_, this->window_, request);
                WaitForPickerReady(aniPickerAsyncContext);
            };
            std::thread taskThread(task);
            taskThread.join();

            std::vector<std::string> list;
            if (aniPickerAsyncContext) {
                if (aniPickerAsyncContext->pickerCallBack) {
                    if (aniPickerAsyncContext->pickerCallBack->want.GetParams().HasParam("ability.params.stream")) {
                        list = aniPickerAsyncContext->pickerCallBack->want.GetStringArrayParam("ability.params.stream");
                        if (aniPickerAsyncContext->pickerCallBack->want.GetParams().HasParam("userSuffixIndex")) {
                            int32_t index = 0;
                            index = aniPickerAsyncContext->pickerCallBack->want.GetIntParam("userSuffixIndex", 0);
                            if (index >= 0) {
                                suffixIndex_ = index;
                            }
                        }
                    }
                }
            }

            std::vector<::taihe::string> taiheList;
            for (auto it = list.begin(); it != list.end(); ++it) {
                taiheList.push_back(::taihe::string(*it));
            }
            ::taihe::array<::taihe::string> uriNullArray(taiheList);
            return uriNullArray;
        }

        ::taihe::array<::taihe::string> SaveSync1(::ohos::file::picker::DocumentSaveOptions const &option)
        {
            HILOG_INFO("Begin");
            OHOS::sptr<OHOS::Rosen::Window> tempWindow = nullptr;
            Want request;
            request.SetAction(std::string("ohos.want.action.CREATE_FILE_SERVICE"));
            WantParams &parameters = const_cast<WantParams &>(request.GetParams());
            parameters.SetParam("startMode", String::Box(std::string("save")));
            parameters.SetParam("pickerMode", Integer::Box(0));
            parameters.SetParam("extType", String::Box(std::string("filePicker")));
            parameters.SetParam("pickerType", String::Box(std::string("save")));
            parameters.SetParam("autoCreateEmptyFile", Boolean::Box(true));

            if (option.defaultFilePathUri.has_value()) {
                parameters.SetParam("key_pick_dir_path", String::Box(std::string(option.defaultFilePathUri.value())));
            }
            if (option.newFileNames.has_value()) {
                uint32_t length = option.newFileNames.value().size();
                OHOS::sptr<OHOS::AAFwk::IArray> ao = OHOS::sptr<OHOS::AAFwk::Array>::MakeSptr(length,
                    OHOS::AAFwk::g_IID_IString);
                int i = 0;
                for (auto it = option.newFileNames.value().begin();
                     it != option.newFileNames.value().end(); ++it) {
                    ao->Set(i, String::Box(std::string(*it)));
                    i++;
                }
                parameters.SetParam("key_pick_file_name", ao);
                auto itTemp = option.newFileNames.value().begin();
                std::string saveFile(*itTemp);
                parameters.SetParam("saveFile", String::Box(saveFile));
            }
            if (option.fileSuffixChoices.has_value()) {
                uint32_t length = option.fileSuffixChoices.value().size();
                OHOS::sptr<OHOS::AAFwk::IArray> ao = OHOS::sptr<OHOS::AAFwk::Array>::MakeSptr(length,
                    OHOS::AAFwk::g_IID_IString);
                int i = 0;
                for (auto it = option.fileSuffixChoices.value().begin();
                     it != option.fileSuffixChoices.value().end(); ++it) {
                    ao->Set(i, String::Box(std::string(*it)));
                    i++;
                }
                parameters.SetParam("key_file_suffix_choices", ao);
            }
            if (option.pickerMode.has_value()) {
                if (option.pickerMode.value() == 1) {
                    parameters.SetParam("pickerMode", Integer::Box(option.pickerMode.value()));
                    parameters.SetParam("pickerType", String::Box(std::string("downloadAuth")));
                }
            }

            std::shared_ptr<AniPickerAsyncContext> aniPickerAsyncContext = nullptr;
            auto task = [this, &request, &aniPickerAsyncContext]() {
                aniPickerAsyncContext = StartPickerExtension(this->vm_, this->context_, this->window_, request);
                WaitForPickerReady(aniPickerAsyncContext);
            };
            std::thread taskThread(task);
            taskThread.join();

            std::vector<std::string> list;
            if (aniPickerAsyncContext) {
                if (aniPickerAsyncContext->pickerCallBack) {
                    if (aniPickerAsyncContext->pickerCallBack->want.GetParams().HasParam("ability.params.stream")) {
                        list = aniPickerAsyncContext->pickerCallBack->want.GetStringArrayParam("ability.params.stream");
                        if (aniPickerAsyncContext->pickerCallBack->want.GetParams().HasParam("userSuffixIndex")) {
                            int32_t index = 0;
                            index = aniPickerAsyncContext->pickerCallBack->want.GetIntParam("userSuffixIndex", 0);
                            if (index >= 0) {
                                suffixIndex_ = index;
                            }
                        }
                    }
                }
            }

            std::vector<::taihe::string> taiheList;
            for (auto it = list.begin(); it != list.end(); ++it) {
                taiheList.push_back(::taihe::string(*it));
            }
            ::taihe::array<::taihe::string> uriNullArray(taiheList);
            return uriNullArray;
        }

        ::taihe::array<::taihe::string> SaveSync2()
        {
            HILOG_INFO("Begin");
            OHOS::sptr<OHOS::Rosen::Window> tempWindow = nullptr;
            Want request;
            request.SetAction(std::string("ohos.want.action.CREATE_FILE_SERVICE"));
            WantParams &parameters = const_cast<WantParams &>(request.GetParams());
            parameters.SetParam("startMode", String::Box(std::string("save")));
            parameters.SetParam("pickerMode", Integer::Box(0));
            parameters.SetParam("extType", String::Box(std::string("filePicker")));
            parameters.SetParam("pickerType", String::Box(std::string("save")));
            parameters.SetParam("autoCreateEmptyFile", Boolean::Box(true));

            std::shared_ptr<AniPickerAsyncContext> aniPickerAsyncContext = nullptr;
            auto task = [this, &request, &aniPickerAsyncContext]() {
                aniPickerAsyncContext = StartPickerExtension(this->vm_, this->context_, this->window_, request);
                WaitForPickerReady(aniPickerAsyncContext);
            };
            std::thread taskThread(task);
            taskThread.join();

            std::vector<std::string> list;
            if (aniPickerAsyncContext) {
                if (aniPickerAsyncContext->pickerCallBack) {
                    if (aniPickerAsyncContext->pickerCallBack->want.GetParams().HasParam("ability.params.stream")) {
                        list = aniPickerAsyncContext->pickerCallBack->want.GetStringArrayParam("ability.params.stream");
                        if (aniPickerAsyncContext->pickerCallBack->want.GetParams().HasParam("userSuffixIndex")) {
                            int32_t index = 0;
                            index = aniPickerAsyncContext->pickerCallBack->want.GetIntParam("userSuffixIndex", 0);
                            if (index >= 0) {
                                suffixIndex_ = index;
                            }
                        }
                    }
                }
            }

            std::vector<::taihe::string> taiheList;
            for (auto it = list.begin(); it != list.end(); ++it) {
                taiheList.push_back(::taihe::string(*it));
            }
            ::taihe::array<::taihe::string> uriNullArray(taiheList);
            return uriNullArray;
        }

        int32_t getSelectedIndex()
        {
            int32_t index = 0;
            index = suffixIndex_;
            suffixIndex_ = -1;
            return index;
        }

    public:
        ani_ref context_;
        ani_ref window_;
        int32_t suffixIndex_;
        ani_vm *vm_ = nullptr;
    };

    class AudioViewPickerImpl {
    public:
        AudioViewPickerImpl()
            : context_(nullptr), vm_(nullptr) {}

        AudioViewPickerImpl(uintptr_t context)
            : context_(nullptr), vm_(nullptr)
        {
            HILOG_INFO("Begin");
            taihe::env_guard guard;
            ani_env *env = guard.get_env();

            if (env == nullptr) {
                HILOG_ERROR("env == nullptr");
                return;
            }

            env->GetVM(&vm_);
            if (vm_ == nullptr) {
                HILOG_ERROR("vm == nullptr");
                return;
            }

            ani_object objContext = reinterpret_cast<ani_object>(context);
            ani_ref refContext = static_cast<ani_ref>(objContext);
            ani_boolean isValue = false;
            ani_boolean isUndefined = false;
            env->Reference_IsNullishValue(refContext, &isValue);
            env->Reference_IsUndefined(refContext, &isUndefined);
            if (isUndefined || isValue) {
                HILOG_ERROR("context is Undefined or  Null");
                return;
            }
            if (env->GlobalReference_Create(refContext, &context_) != ANI_OK) {
                HILOG_ERROR("context  global reference error");
                ::taihe::set_business_error(-1, "context  global reference error");
                return;
            }
        }

        ~AudioViewPickerImpl()
        {
            HILOG_INFO("Begin");
            taihe::env_guard guard;
            ani_env *env = guard.get_env();
            if (env == nullptr) {
                HILOG_ERROR("env == nullptr");
                return;
            }
            env->GlobalReference_Delete(context_);
        }

        ::taihe::array<::taihe::string> SelectSync(::taihe::optional_view<AudioSelectOptions> option)
        {
            HILOG_INFO("Begin");
            taihe::env_guard guard;
            ani_env *env = guard.get_env();
            Want request;
            request.SetAction(std::string("ohos.want.action.OPEN_FILE"));
            WantParams &parameters = const_cast<WantParams &>(request.GetParams());
            parameters.SetParam("extType", String::Box(std::string("audioPicker")));
            if (option.has_value()) {
                AudioSelectOptions aniOption = option.value();
                if (aniOption.maxSelectNumber.has_value()) {
                    parameters.SetParam("key_pick_num", Integer::Box(aniOption.maxSelectNumber.value()));
                }
            }

            ani_ref undefineRef;
            if (env->GetUndefined(&undefineRef) != ANI_OK) {
                ::taihe::set_business_error(-1, "context  GetUndefined error");
            }

            std::shared_ptr<AniPickerAsyncContext> aniPickerAsyncContext = nullptr;
            auto task = [this, &undefineRef, &request, &aniPickerAsyncContext]() {
                aniPickerAsyncContext = StartPickerExtension(this->vm_, this->context_, undefineRef, request);
                WaitForPickerReady(aniPickerAsyncContext);
            };
            std::thread taskThread(task);
            taskThread.join();

            std::vector<std::string> list;
            if (aniPickerAsyncContext) {
                if (aniPickerAsyncContext->pickerCallBack) {
                    if (aniPickerAsyncContext->pickerCallBack->want.GetParams().HasParam("uriArr")) {
                        list = aniPickerAsyncContext->pickerCallBack->want.GetStringArrayParam("uriArr");
                    }
                }
            }

            std::vector<::taihe::string> taiheList;
            for (auto it = list.begin(); it != list.end(); ++it) {
                taiheList.push_back(::taihe::string(*it));
            }
            ::taihe::array<::taihe::string> uriNullArray(taiheList);
            return uriNullArray;
        }

        ::taihe::array<::taihe::string> SelectSync1(::ohos::file::picker::AudioSelectOptions const &option)
        {
            HILOG_INFO("Begin");
            taihe::env_guard guard;
            ani_env *env = guard.get_env();
            Want request;
            request.SetAction(std::string("ohos.want.action.OPEN_FILE"));
            WantParams &parameters = const_cast<WantParams &>(request.GetParams());
            parameters.SetParam("extType", String::Box(std::string("audioPicker")));
            if (option.maxSelectNumber.has_value()) {
                parameters.SetParam("key_pick_num", Integer::Box(option.maxSelectNumber.value()));
            }
            ani_ref undefineRef;
            if (env->GetUndefined(&undefineRef) != ANI_OK) {
                ::taihe::set_business_error(-1, "context  GetUndefined error");
            }

            std::shared_ptr<AniPickerAsyncContext> aniPickerAsyncContext = nullptr;
            auto task = [this, &undefineRef, &request, &aniPickerAsyncContext]() {
                aniPickerAsyncContext = StartPickerExtension(this->vm_, this->context_, undefineRef, request);
                WaitForPickerReady(aniPickerAsyncContext);
            };
            std::thread taskThread(task);
            taskThread.join();

            std::vector<std::string> list;
            if (aniPickerAsyncContext) {
                if (aniPickerAsyncContext->pickerCallBack) {
                    if (aniPickerAsyncContext->pickerCallBack->want.GetParams().HasParam("uriArr")) {
                        list = aniPickerAsyncContext->pickerCallBack->want.GetStringArrayParam("uriArr");
                    }
                }
            }

            std::vector<::taihe::string> taiheList;
            for (auto it = list.begin(); it != list.end(); ++it) {
                taiheList.push_back(::taihe::string(*it));
            }
            ::taihe::array<::taihe::string> uriNullArray(taiheList);
            return uriNullArray;
        }

        ::taihe::array<::taihe::string> SelectSync2()
        {
            HILOG_INFO("Begin");
            taihe::env_guard guard;
            ani_env *env = guard.get_env();
            Want request;
            request.SetAction(std::string("ohos.want.action.OPEN_FILE"));
            WantParams &parameters = const_cast<WantParams &>(request.GetParams());
            parameters.SetParam("extType", String::Box(std::string("audioPicker")));

            ani_ref undefineRef;
            if (env->GetUndefined(&undefineRef) != ANI_OK) {
                ::taihe::set_business_error(-1, "context  GetUndefined error");
            }

            std::shared_ptr<AniPickerAsyncContext> aniPickerAsyncContext = nullptr;
            auto task = [this, &undefineRef, &request, &aniPickerAsyncContext]() {
                aniPickerAsyncContext = StartPickerExtension(this->vm_, this->context_, undefineRef, request);
                WaitForPickerReady(aniPickerAsyncContext);
            };
            std::thread taskThread(task);
            taskThread.join();

            std::vector<std::string> list;
            if (aniPickerAsyncContext) {
                if (aniPickerAsyncContext->pickerCallBack) {
                    if (aniPickerAsyncContext->pickerCallBack->want.GetParams().HasParam("uriArr")) {
                        list = aniPickerAsyncContext->pickerCallBack->want.GetStringArrayParam("uriArr");
                    }
                }
            }

            std::vector<::taihe::string> taiheList;
            for (auto it = list.begin(); it != list.end(); ++it) {
                taiheList.push_back(::taihe::string(*it));
            }
            ::taihe::array<::taihe::string> uriNullArray(taiheList);
            return uriNullArray;
        }

        ::taihe::array<::taihe::string> SaveSync(::taihe::optional_view<::ohos::file::picker::AudioSaveOptions> option)
        {
            HILOG_INFO("Begin");
            taihe::env_guard guard;
            ani_env *env = guard.get_env();
            Want request;
            request.SetAction(std::string("ohos.want.action.CREATE_FILE_SERVICE"));
            WantParams &parameters = const_cast<WantParams &>(request.GetParams());
            parameters.SetParam("startMode", String::Box(std::string("save")));
            parameters.SetParam("pickerMode", Integer::Box(0));
            parameters.SetParam("extType", String::Box(std::string("filePicker")));
            parameters.SetParam("pickerType", String::Box(std::string("save")));
            parameters.SetParam("autoCreateEmptyFile", Boolean::Box(true));
            if (option.has_value()) {
                AudioSaveOptions audioSaveOptions = option.value();
                if (audioSaveOptions.newFileNames.has_value()) {
                    uint32_t length = audioSaveOptions.newFileNames.value().size();
                    OHOS::sptr<OHOS::AAFwk::IArray> ao = OHOS::sptr<OHOS::AAFwk::Array>::MakeSptr(length,
                        OHOS::AAFwk::g_IID_IString);
                    int i = 0;
                    for (auto it = audioSaveOptions.newFileNames.value().begin();
                         it != audioSaveOptions.newFileNames.value().end(); ++it) {
                        ao->Set(i, String::Box(std::string(*it)));
                        i++;
                    }
                    setAudioPickerSaveParameters(audioSaveOptions, parameters, ao);
                }
            }
            ani_ref undefineRef;
            if (env->GetUndefined(&undefineRef) != ANI_OK) {
                ::taihe::set_business_error(-1, "context  GetUndefined error");
            }
            std::shared_ptr<AniPickerAsyncContext> aniPickerAsyncContext = nullptr;
            auto task = [this, &undefineRef, &request, &aniPickerAsyncContext]() {
                aniPickerAsyncContext = StartPickerExtension(this->vm_, this->context_, undefineRef, request);
                WaitForPickerReady(aniPickerAsyncContext);
            };
            std::thread taskThread(task);
            taskThread.join();
            std::vector<std::string> list;
            if (aniPickerAsyncContext && aniPickerAsyncContext->pickerCallBack) {
                if (aniPickerAsyncContext->pickerCallBack->want.GetParams().HasParam("ability.params.stream")) {
                    list = aniPickerAsyncContext->pickerCallBack->want.GetStringArrayParam("ability.params.stream");
                }
            }
            std::vector<::taihe::string> taiheList;
            for (auto it = list.begin(); it != list.end(); ++it) {
                taiheList.push_back(::taihe::string(*it));
            }
            ::taihe::array<::taihe::string> uriNullArray(taiheList);
            return uriNullArray;
        }

        ::taihe::array<::taihe::string> SaveSync1(::ohos::file::picker::AudioSaveOptions const &option)
        {
            HILOG_INFO("Begin");
            taihe::env_guard guard;
            ani_env *env = guard.get_env();
            Want request;
            request.SetAction(std::string("ohos.want.action.CREATE_FILE_SERVICE"));
            WantParams &parameters = const_cast<WantParams &>(request.GetParams());
            parameters.SetParam("startMode", String::Box(std::string("save")));
            parameters.SetParam("pickerMode", Integer::Box(0));
            parameters.SetParam("extType", String::Box(std::string("filePicker")));
            parameters.SetParam("pickerType", String::Box(std::string("save")));
            parameters.SetParam("autoCreateEmptyFile", Boolean::Box(true));

            if (option.newFileNames.has_value()) {
                uint32_t length = option.newFileNames.value().size();
                OHOS::sptr<OHOS::AAFwk::IArray> ao = OHOS::sptr<OHOS::AAFwk::Array>::MakeSptr(length,
                    OHOS::AAFwk::g_IID_IString);
                int i = 0;
                for (auto it = option.newFileNames.value().begin(); it != option.newFileNames.value().end(); ++it) {
                    ao->Set(i, String::Box(std::string(*it)));
                    i++;
                }
                setAudioPickerSaveParameters(option, parameters, ao);
            }

            ani_ref undefineRef;
            if (env->GetUndefined(&undefineRef) != ANI_OK) {
                ::taihe::set_business_error(-1, "context  GetUndefined error");
            }

            std::shared_ptr<AniPickerAsyncContext> aniPickerAsyncContext = nullptr;
            auto task = [this, &undefineRef, &request, &aniPickerAsyncContext]() {
                aniPickerAsyncContext = StartPickerExtension(this->vm_, this->context_, undefineRef, request);
                WaitForPickerReady(aniPickerAsyncContext);
            };
            std::thread taskThread(task);
            taskThread.join();

            std::vector<std::string> list;
            if (aniPickerAsyncContext) {
                if (aniPickerAsyncContext->pickerCallBack) {
                    if (aniPickerAsyncContext->pickerCallBack->want.GetParams().HasParam("ability.params.stream")) {
                        list = aniPickerAsyncContext->pickerCallBack->want.GetStringArrayParam("ability.params.stream");
                    }
                }
            }

            std::vector<::taihe::string> taiheList;
            for (auto it = list.begin(); it != list.end(); ++it) {
                taiheList.push_back(::taihe::string(*it));
            }
            ::taihe::array<::taihe::string> uriNullArray(taiheList);
            return uriNullArray;
        }

        ::taihe::array<::taihe::string> SaveSync2()
        {
            HILOG_INFO("Begin");
            taihe::env_guard guard;
            ani_env *env = guard.get_env();
            Want request;
            request.SetAction(std::string("ohos.want.action.CREATE_FILE_SERVICE"));
            WantParams &parameters = const_cast<WantParams &>(request.GetParams());
            parameters.SetParam("startMode", String::Box(std::string("save")));
            parameters.SetParam("pickerMode", Integer::Box(0));
            parameters.SetParam("extType", String::Box(std::string("filePicker")));
            parameters.SetParam("pickerType", String::Box(std::string("save")));
            parameters.SetParam("autoCreateEmptyFile", Boolean::Box(true));

            ani_ref undefineRef;
            if (env->GetUndefined(&undefineRef) != ANI_OK) {
                ::taihe::set_business_error(-1, "context  GetUndefined error");
            }

            std::shared_ptr<AniPickerAsyncContext> aniPickerAsyncContext = nullptr;
            auto task = [this, &undefineRef, &request, &aniPickerAsyncContext]() {
                aniPickerAsyncContext = StartPickerExtension(this->vm_, this->context_, undefineRef, request);
                WaitForPickerReady(aniPickerAsyncContext);
            };
            std::thread taskThread(task);
            taskThread.join();

            std::vector<std::string> list;
            if (aniPickerAsyncContext) {
                if (aniPickerAsyncContext->pickerCallBack) {
                    if (aniPickerAsyncContext->pickerCallBack->want.GetParams().HasParam("ability.params.stream")) {
                        list = aniPickerAsyncContext->pickerCallBack->want.GetStringArrayParam("ability.params.stream");
                    }
                }
            }

            std::vector<::taihe::string> taiheList;
            for (auto it = list.begin(); it != list.end(); ++it) {
                taiheList.push_back(::taihe::string(*it));
            }
            ::taihe::array<::taihe::string> uriNullArray(taiheList);
            return uriNullArray;
        }

    public:
        ani_ref context_;
        ani_vm *vm_ = nullptr;
    };

    ::ohos::file::picker::DocumentViewPicker createDocumentViewPicker1()
    {
        // The parameters in the make_holder function should be of the same type
        // as the parameters in the constructor of the actual implementation class.
        HILOG_INFO("Begin");
        return taihe::make_holder<DocumentViewPickerImpl, ::ohos::file::picker::DocumentViewPicker>();
    }

    ::ohos::file::picker::DocumentViewPicker createDocumentViewPicker2(uintptr_t context)
    {
        // The parameters in the make_holder function should be of the same type
        // as the parameters in the constructor of the actual implementation class.
        HILOG_INFO("Begin");
        return taihe::make_holder<DocumentViewPickerImpl, ::ohos::file::picker::DocumentViewPicker>(context);
    }

    ::ohos::file::picker::DocumentViewPicker createDocumentViewPicker3(uintptr_t context, uintptr_t window)
    {
        // The parameters in the make_holder function should be of the same type
        // as the parameters in the constructor of the actual implementation class.
        HILOG_INFO("Begin");
        return taihe::make_holder<DocumentViewPickerImpl, ::ohos::file::picker::DocumentViewPicker>(context, window);
    }

    ::ohos::file::picker::AudioViewPicker createAudioViewPicker1()
    {
        // The parameters in the make_holder function should be of the same type
        // as the parameters in the constructor of the actual implementation class.
        HILOG_INFO("Begin");
        return taihe::make_holder<AudioViewPickerImpl, ::ohos::file::picker::AudioViewPicker>();
    }

    ::ohos::file::picker::AudioViewPicker createAudioViewPicker2(uintptr_t context)
    {
        // The parameters in the make_holder function should be of the same type
        // as the parameters in the constructor of the actual implementation class.
        HILOG_INFO("Begin");
        return taihe::make_holder<AudioViewPickerImpl, ::ohos::file::picker::AudioViewPicker>(context);
    }
} // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_createDocumentViewPicker1(createDocumentViewPicker1);
TH_EXPORT_CPP_API_createDocumentViewPicker2(createDocumentViewPicker2);
TH_EXPORT_CPP_API_createDocumentViewPicker3(createDocumentViewPicker3);
TH_EXPORT_CPP_API_createAudioViewPicker1(createAudioViewPicker1);
TH_EXPORT_CPP_API_createAudioViewPicker2(createAudioViewPicker2);
// NOLINTEND
