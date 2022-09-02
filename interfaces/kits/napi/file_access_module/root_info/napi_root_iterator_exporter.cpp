/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "napi_root_iterator_exporter.h"

#include "hilog_wrapper.h"
#include "napi_root_info_exporter.h"
#include "root_info_entity.h"
#include "root_iterator_entity.h"

namespace OHOS {
namespace FileAccessFwk {
bool NapiRootIteratorExporter::Export()
{
    std::vector<napi_property_descriptor> props = {
        NVal::DeclareNapiFunction("next", Next),
    };

    std::string className = GetClassName();
    bool succ = false;
    napi_value classValue = nullptr;
    std::tie(succ, classValue) = NClass::DefineClass(exports_.env_, className,
        NapiRootIteratorExporter::Constructor, std::move(props));
    if (!succ) {
        NError(ERR_NULL_POINTER).ThrowErr(exports_.env_,
            "INNER BUG. Failed to define class NapiRootIteratorExporter");
        return false;
    }

    succ = NClass::SaveClass(exports_.env_, className, classValue);
    if (!succ) {
        NError(ERR_NULL_POINTER).ThrowErr(exports_.env_, "INNER BUG. Failed to save class NapiRootIteratorExporter");
        return false;
    }

    return exports_.AddProp(className, classValue);
}

napi_value NapiRootIteratorExporter::Constructor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        NError(ERR_PARAM_NUMBER).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    auto rootIteratorEntity = std::make_unique<RootIteratorEntity>();
    if (!NClass::SetEntityFor<RootIteratorEntity>(env, funcArg.GetThisVar(), std::move(rootIteratorEntity))) {
        NError(ERR_NULL_POINTER).ThrowErr(env, "INNER BUG. Failed to wrap entity for obj FileIteratorEntity");
        return nullptr;
    }

    return funcArg.GetThisVar();
}

napi_value NapiRootIteratorExporter::Next(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        NError(ERR_PARAM_NUMBER).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    napi_value thisVar = funcArg.GetThisVar();
    auto iterEntity = NClass::GetEntityOf<RootIteratorEntity>(env, thisVar);
    if (iterEntity == nullptr) {
        NError(ERR_NULL_POINTER).ThrowErr(env, "Cannot get entity of RootIteratorEntity");
        return nullptr;
    }

    napi_value objRootExporter = NClass::InstantiateClass(env, NapiRootInfoExporter::className_, {});
    if (objRootExporter == nullptr) {
        NError(ERR_NULL_POINTER).ThrowErr(env, "Cannot instantiate class NapiRootInfoExporter");
        return nullptr;
    }

    auto rootEntity = NClass::GetEntityOf<RootInfoEntity>(env, objRootExporter);
    if (rootEntity == nullptr) {
        NError(ERR_NULL_POINTER).ThrowErr(env, "Cannot get the entity of RootInfoEntity");
        return nullptr;
    }

    auto retNVal = NVal::CreateObject(env);
    bool done = true;
    {
        std::lock_guard<std::mutex> lock(iterEntity->entityOperateMutex);
        auto len = (int64_t)iterEntity->devVec.size();
        rootEntity->fileAccessHelper = iterEntity->fileAccessHelper;
        if (iterEntity->pos < len) {
            rootEntity->rootInfo = iterEntity->devVec[iterEntity->pos];
            iterEntity->pos++;
            done = (iterEntity->pos == len);
        } else {
            iterEntity->pos++;
            rootEntity = nullptr;
            objRootExporter = NVal::CreateUndefined(env).val_;
            done = true;
        }
        retNVal.AddProp("value", objRootExporter);
        retNVal.AddProp("done", NVal::CreateBool(env, done).val_);
    }

    return retNVal.val_;
}

std::string NapiRootIteratorExporter::GetClassName()
{
    return NapiRootIteratorExporter::className_;
}
} // namespace FileAccessFwk
} // namespace OHOS