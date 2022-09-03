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

#include "napi_file_iterator_exporter.h"

#include <cinttypes>

#include "file_info_entity.h"
#include "file_iterator_entity.h"
#include "hilog_wrapper.h"
#include "napi_file_info_exporter.h"
#include "napi_utils.h"

namespace OHOS {
namespace FileAccessFwk {
bool NapiFileIteratorExporter::Export()
{
    std::vector<napi_property_descriptor> props = {
        NVal::DeclareNapiFunction("next", Next),
    };

    std::string className = GetClassName();
    bool succ = false;
    napi_value classValue = nullptr;
    std::tie(succ, classValue) = NClass::DefineClass(exports_.env_, className,
        NapiFileIteratorExporter::Constructor, std::move(props));
    if (!succ) {
        NError(ERR_NULL_POINTER).ThrowErr(exports_.env_, "INNER BUG. Failed to define class NapiFileIteratorExporter");
        return false;
    }

    succ = NClass::SaveClass(exports_.env_, className, classValue);
    if (!succ) {
        NError(ERR_NULL_POINTER).ThrowErr(exports_.env_, "INNER BUG. Failed to save class NapiFileIteratorExporter");
        return false;
    }

    return exports_.AddProp(className, classValue);
}

napi_value NapiFileIteratorExporter::Constructor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        NError(ERR_PARAM_NUMBER).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    auto fileIteratorEntity = std::make_unique<FileIteratorEntity>();
    if (fileIteratorEntity == nullptr) {
        NError(ERR_NULL_POINTER).ThrowErr(env, "New obj FileIteratorEntity fail");
        return nullptr;
    }

    if (!NClass::SetEntityFor<FileIteratorEntity>(env, funcArg.GetThisVar(), std::move(fileIteratorEntity))) {
        NError(ERR_NULL_POINTER).ThrowErr(env, "INNER BUG. Failed to wrap entity for obj FileIteratorEntity");
        return nullptr;
    }

    return funcArg.GetThisVar();
}

static int MakeResult(napi_value objFileInfoExporter, FileIteratorEntity *fileIteratorEntity,
    FileInfoEntity *fileInfoEntity, napi_env env, NVal &nVal)
{
    std::lock_guard<std::mutex> lock(fileIteratorEntity->entityOperateMutex);
    bool done = true;
    if (fileIteratorEntity->fileInfoVec.size() == 0) {
        nVal.AddProp("value", NVal::CreateUndefined(env).val_);
        nVal.AddProp("done", NVal::CreateBool(env, done).val_);
        return ERR_OK;
    }

    if (fileIteratorEntity->pos == fileIteratorEntity->fileInfoVec.size()) {
        HILOG_ERROR("pos out of index.");
        return ERR_INVALID_RESULT;
    }

    fileInfoEntity->fileAccessHelper = fileIteratorEntity->fileAccessHelper;
    fileInfoEntity->fileInfo = fileIteratorEntity->fileInfoVec[fileIteratorEntity->pos];
    fileIteratorEntity->pos++;
    if (fileIteratorEntity->pos == MAX_COUNT) {
        fileIteratorEntity->fileInfoVec.clear();
        fileIteratorEntity->offset += MAX_COUNT;
        fileIteratorEntity->pos = 0;
        int ret = fileIteratorEntity->fileAccessHelper->ListFile(fileIteratorEntity->fileInfo,
            fileIteratorEntity->offset, MAX_COUNT, fileIteratorEntity->fileInfoVec);
        if (ret != ERR_OK) {
            HILOG_ERROR("exec ListFile fail, code:%{public}d", ret);
            return ret;
        }
    } else if (fileIteratorEntity->pos == fileIteratorEntity->fileInfoVec.size()) {
        fileIteratorEntity->fileInfoVec.clear();
        fileIteratorEntity->pos = 0;
        fileIteratorEntity->offset = 0;
    }

    done = (fileIteratorEntity->pos == fileIteratorEntity->fileInfoVec.size());
    nVal.AddProp("value", objFileInfoExporter);
    nVal.AddProp("done", NVal::CreateBool(env, done).val_);
    return ERR_OK;
}

napi_value NapiFileIteratorExporter::Next(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        NError(ERR_PARAM_NUMBER).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    auto fileIteratorEntity = NClass::GetEntityOf<FileIteratorEntity>(env, funcArg.GetThisVar());
    if (fileIteratorEntity == nullptr) {
        NError(ERR_NULL_POINTER).ThrowErr(env, "Cannot get entity of FileIteratorEntity");
        return nullptr;
    }

    if (IsDirectory(fileIteratorEntity->fileInfo.mode) != ERR_OK) {
        HILOG_ERROR("current FileInfo's mode error");
        return NVal::CreateUndefined(env).val_;
    }

    auto objFileInfoExporter = NClass::InstantiateClass(env, NapiFileInfoExporter::className_, {});
    if (objFileInfoExporter == nullptr) {
        NError(ERR_NULL_POINTER).ThrowErr(env, "Cannot instantiate class NapiFileInfoExporter");
        return nullptr;
    }

    auto fileInfoEntity = NClass::GetEntityOf<FileInfoEntity>(env, objFileInfoExporter);
    if (fileInfoEntity == nullptr) {
        NError(ERR_NULL_POINTER).ThrowErr(env, "Cannot get the entity of FileInfoEntity");
        return nullptr;
    }

    if (fileIteratorEntity->fileAccessHelper == nullptr) {
        NError(ERR_NULL_POINTER).ThrowErr(env, "fileAccessHelper is null.");
        return nullptr;
    }

    auto retNVal = NVal::CreateObject(env);
    int ret = MakeResult(objFileInfoExporter, fileIteratorEntity, fileInfoEntity, env, retNVal);
    if (ret != ERR_OK) {
        NError(ret).ThrowErr(env, "MakeResult fail.");
        return nullptr;
    }

    return retNVal.val_;
}

std::string NapiFileIteratorExporter::GetClassName()
{
    return NapiFileIteratorExporter::className_;
}
} // namespace FileAccessFwk
} // namespace OHOS