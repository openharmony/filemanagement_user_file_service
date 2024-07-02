/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef INTERFACES_KITS_NATIVE_PICKER_N_EXPOTER_H
#define INTERFACES_KITS_NATIVE_PICKER_N_EXPOTER_H

#include <dirent.h>
#include <thread>
#include "data_ability_helper.h"
#include "data_ability_observer_stub.h"
#include "data_ability_predicates.h"
#include "filemgmt_libn.h"
#include "picker_napi_utils.h"
#include "napi_base_context.h"
#include "napi_common_want.h"


namespace OHOS {
namespace Picker {
using namespace FileManagement::LibN;
using namespace std;

struct NameListArg {
    struct dirent** namelist = { nullptr };
    int direntNum = 0;
};

struct PickerCallBack {
    bool ready = false;
    int32_t resultCode;
    OHOS::AAFwk::Want want;
};

struct PickerAsyncContext {
    napi_async_work work;
    napi_deferred deferred;
    napi_ref callbackRef;
    size_t argc;
    napi_value argv[NAPI_ARGC_MAX];
    std::shared_ptr<PickerCallBack> pickerCallBack;
};

class PickerNExporter final : public FileManagement::LibN::NExporter {
public:
    inline static const std::string className_ = "Picker";
    static napi_value StartModalPicker(napi_env env, napi_callback_info info);
    static napi_value MakeResultWithPickerCallBack(napi_env env, std::shared_ptr<PickerCallBack> pickerCallBack);
    bool Export() override;
    std::string GetClassName() override;
    PickerNExporter(napi_env env, napi_value exports);
    ~PickerNExporter() override;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // INTERFACES_KITS_NATIVE_PICKER_N_EXPOTER_H
