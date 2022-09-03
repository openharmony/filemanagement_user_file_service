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

#include "napi_file_info_exporter.h"

#include "file_access_framework_errno.h"
#include "file_info_entity.h"
#include "file_iterator_entity.h"
#include "hilog_wrapper.h"
#include "napi_file_iterator_exporter.h"
#include "napi_utils.h"

namespace OHOS {
namespace FileAccessFwk {
bool NapiFileInfoExporter::Export()
{
    std::vector<napi_property_descriptor> props = {
        NVal::DeclareNapiFunction("listFile", ListFile),
        NVal::DeclareNapiGetter("uri", GetUri),
        NVal::DeclareNapiGetter("fileName", GetFileName),
        NVal::DeclareNapiGetter("mode", GetMode),
        NVal::DeclareNapiGetter("size", GetSize),
        NVal::DeclareNapiGetter("mtime", GetMtime),
        NVal::DeclareNapiGetter("mimeType", GetMimeType),
    };

    std::string className = GetClassName();
    bool succ = false;
    napi_value classValue = nullptr;
    std::tie(succ, classValue) = NClass::DefineClass(exports_.env_, className,
        NapiFileInfoExporter::Constructor, std::move(props));
    if (!succ) {
        NError(ERR_NULL_POINTER).ThrowErr(exports_.env_, "INNER BUG. Failed to define class NapiFileInfoExporter");
        return false;
    }

    succ = NClass::SaveClass(exports_.env_, className, classValue);
    if (!succ) {
        NError(ERR_NULL_POINTER).ThrowErr(exports_.env_, "INNER BUG. Failed to save class NapiFileInfoExporter");
        return false;
    }

    return exports_.AddProp(className, classValue);
}

napi_value NapiFileInfoExporter::Constructor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        NError(ERR_PARAM_NUMBER).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    auto fileInfoEntity = std::make_unique<FileInfoEntity>();
    if (!NClass::SetEntityFor<FileInfoEntity>(env, funcArg.GetThisVar(), std::move(fileInfoEntity))) {
        NError(ERR_NULL_POINTER).ThrowErr(env, "INNER BUG. Failed to wrap entity for obj FileInfoEntity");
        return nullptr;
    }

    return funcArg.GetThisVar();
}

napi_value NapiFileInfoExporter::ListFile(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO, NARG_CNT::ONE)) {
        NError(ERR_PARAM_NUMBER).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    FileFilter filter({}, {}, {}, 0, 0, false, false);
    if (funcArg.GetArgc() == NARG_CNT::ONE) {
        auto ret = GetFileFilterParam(NVal(env, funcArg.GetArg(NARG_POS::FIRST)), filter);
        if (ret != ERR_OK) {
            NError(ret).ThrowErr(env, "ListFile get FileFilter param fail");
            return nullptr;
        }
    }

    auto fileInfoEntity = NClass::GetEntityOf<FileInfoEntity>(env, funcArg.GetThisVar());
    if (fileInfoEntity == nullptr) {
        NError(ERR_NULL_POINTER).ThrowErr(env, "Cannot get entity of FileInfoEntity");
        return nullptr;
    }

    if (IsDirectory(fileInfoEntity->fileInfo.mode) != ERR_OK) {
        HILOG_ERROR("current FileInfo's mode error");
        return NVal::CreateUndefined(env).val_;
    }

    if (fileInfoEntity->fileAccessHelper == nullptr) {
        NError(ERR_NULL_POINTER).ThrowErr(env, "fileAccessHelper is null.");
        return nullptr;
    }

    auto objFileIteratorExporter = NClass::InstantiateClass(env, NapiFileIteratorExporter::className_, {});
    if (objFileIteratorExporter == nullptr) {
        NError(ERR_NULL_POINTER).ThrowErr(env, "Cannot instantiate class NapiFileIteratorExporter");
        return nullptr;
    }

    auto fileIteratorEntity = NClass::GetEntityOf<FileIteratorEntity>(env, objFileIteratorExporter);
    if (fileIteratorEntity == nullptr) {
        NError(ERR_NULL_POINTER).ThrowErr(env, "Cannot get the entity of FileIteratorEntity");
        return nullptr;
    }

    {
        std::lock_guard<std::mutex> lock(fileIteratorEntity->entityOperateMutex);
        fileIteratorEntity->fileAccessHelper = fileInfoEntity->fileAccessHelper;
        fileIteratorEntity->fileInfo = fileInfoEntity->fileInfo;
        fileIteratorEntity->fileInfoVec.clear();
        fileIteratorEntity->offset = 0;
        fileIteratorEntity->pos = 0;
        fileIteratorEntity->filter = std::move(filter);
        auto ret = fileInfoEntity->fileAccessHelper->ListFile(fileInfoEntity->fileInfo, fileIteratorEntity->offset,
            MAX_COUNT, fileIteratorEntity->filter, fileIteratorEntity->fileInfoVec);
        if (ret != ERR_OK) {
            NError(ret).ThrowErr(env, "exec ListFile fail");
            return nullptr;
        }
    }

    return NVal(env, objFileIteratorExporter).val_;
}

static FileInfoEntity *GetFileInfoEntity(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        NError(ERR_PARAM_NUMBER).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    auto fileInfoEntity = NClass::GetEntityOf<FileInfoEntity>(env, funcArg.GetThisVar());
    if (fileInfoEntity == nullptr) {
        NError(ERR_NULL_POINTER).ThrowErr(env, "Cannot get entity of FileInfoEntity");
        return nullptr;
    }

    return fileInfoEntity;
}

napi_value NapiFileInfoExporter::GetUri(napi_env env, napi_callback_info info)
{
    auto fileInfoEntity = GetFileInfoEntity(env, info);
    if (fileInfoEntity == nullptr) {
        HILOG_ERROR("Failed to get entity of FileInfoEntity");
        return NVal::CreateUndefined(env).val_;
    }

    return NVal::CreateUTF8String(env, fileInfoEntity->fileInfo.uri).val_;
}

napi_value NapiFileInfoExporter::GetFileName(napi_env env, napi_callback_info info)
{
    auto fileInfoEntity = GetFileInfoEntity(env, info);
    if (fileInfoEntity == nullptr) {
        HILOG_ERROR("Failed to get entity of FileInfoEntity");
        return NVal::CreateUndefined(env).val_;
    }

    return NVal::CreateUTF8String(env, fileInfoEntity->fileInfo.fileName).val_;
}

napi_value NapiFileInfoExporter::GetMode(napi_env env, napi_callback_info info)
{
    auto fileInfoEntity = GetFileInfoEntity(env, info);
    if (fileInfoEntity == nullptr) {
        HILOG_ERROR("Failed to get entity of FileInfoEntity");
        return NVal::CreateUndefined(env).val_;
    }

    return NVal::CreateInt32(env, fileInfoEntity->fileInfo.mode).val_;
}

napi_value NapiFileInfoExporter::GetSize(napi_env env, napi_callback_info info)
{
    auto fileInfoEntity = GetFileInfoEntity(env, info);
    if (fileInfoEntity == nullptr) {
        HILOG_ERROR("Failed to get entity of FileInfoEntity");
        return NVal::CreateUndefined(env).val_;
    }

    return NVal::CreateInt64(env, fileInfoEntity->fileInfo.size).val_;
}

napi_value NapiFileInfoExporter::GetMtime(napi_env env, napi_callback_info info)
{
    auto fileInfoEntity = GetFileInfoEntity(env, info);
    if (fileInfoEntity == nullptr) {
        HILOG_ERROR("Failed to get entity of FileInfoEntity");
        return NVal::CreateUndefined(env).val_;
    }

    return NVal::CreateInt64(env, fileInfoEntity->fileInfo.mtime).val_;
}

napi_value NapiFileInfoExporter::GetMimeType(napi_env env, napi_callback_info info)
{
    auto fileInfoEntity = GetFileInfoEntity(env, info);
    if (fileInfoEntity == nullptr) {
        HILOG_ERROR("Failed to get entity of FileInfoEntity");
        return NVal::CreateUndefined(env).val_;
    }

    return NVal::CreateUTF8String(env, fileInfoEntity->fileInfo.mimeType).val_;
}

std::string NapiFileInfoExporter::GetClassName()
{
    return NapiFileInfoExporter::className_;
}
} // namespace FileAccessFwk
} // namespace OHOS