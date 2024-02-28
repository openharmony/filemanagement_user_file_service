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
#include <regex>

#include "file_info_entity.h"
#include "file_iterator_entity.h"
#include "hilog_wrapper.h"
#include "napi_file_info_exporter.h"
#include "napi_utils.h"

namespace OHOS {
namespace FileAccessFwk {
static const std::regex TRASH_RECENT_DIR_REGEX("^file://docs/storage/Users(/.+/)?\\.(Trash|Recent)(/.*)*");
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
        NError(E_GETRESULT).ThrowErr(exports_.env_);
        return false;
    }

    succ = NClass::SaveClass(exports_.env_, className, classValue);
    if (!succ) {
        NError(E_GETRESULT).ThrowErr(exports_.env_);
        return false;
    }

    return exports_.AddProp(className, classValue);
}

napi_value NapiFileIteratorExporter::Constructor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    auto fileIteratorEntity = std::make_unique<FileIteratorEntity>();
    if (fileIteratorEntity == nullptr) {
        NError(E_GETRESULT).ThrowErr(env);
        return nullptr;
    }

    if (!NClass::SetEntityFor<FileIteratorEntity>(env, funcArg.GetThisVar(), std::move(fileIteratorEntity))) {
        NError(E_GETRESULT).ThrowErr(env);
        return nullptr;
    }

    return funcArg.GetThisVar();
}

static int MakeListFileResult(napi_value &objFileInfoExporter, FileIteratorEntity *fileIteratorEntity,
    FileInfoEntity *fileInfoEntity, napi_env env, NVal &nVal, bool &isDone)
{
    SharedMemoryInfo &memInfo = fileIteratorEntity->memInfo;
    std::lock_guard<std::mutex> lock(fileIteratorEntity->entityOperateMutex);
    if (memInfo.Empty() && !memInfo.isOver) {
        int ret = SharedMemoryOperation::ExpandSharedMemory(memInfo);
        if (ret != ERR_OK) {
            return ret;
        }

        fileIteratorEntity->offset += fileIteratorEntity->currentDataCounts;
        ret = fileIteratorEntity->fileAccessHelper->ListFile(fileIteratorEntity->fileInfo,
            fileIteratorEntity->offset, fileIteratorEntity->filter, memInfo);
        if (ret != ERR_OK) {
            HILOG_ERROR("exec ListFile fail, code:%{public}d", ret);
            return ret;
        }
        fileIteratorEntity->currentDataCounts = fileIteratorEntity->memInfo.totalDataCounts;
    }

    fileInfoEntity->fileAccessHelper = fileIteratorEntity->fileAccessHelper;
    auto ret = SharedMemoryOperation::ReadFileInfo(fileInfoEntity->fileInfo, memInfo);
    if (!ret) {
        fileInfoEntity = nullptr;
        objFileInfoExporter = NVal::CreateUndefined(env).val_;
        nVal.AddProp("value", objFileInfoExporter);
        nVal.AddProp("done", NVal::CreateBool(env, true).val_);
        isDone = true;
        return ERR_OK;
    }
    nVal.AddProp("value", objFileInfoExporter);
    nVal.AddProp("done", NVal::CreateBool(env, false).val_);
    isDone = false;
    return ERR_OK;
}

static int MakeScanFileResult(napi_value &objFileInfoExporter, FileIteratorEntity *fileIteratorEntity,
    FileInfoEntity *fileInfoEntity, napi_env env, NVal &nVal, bool &isDone)
{
    std::lock_guard<std::mutex> lock(fileIteratorEntity->entityOperateMutex);
    if (fileIteratorEntity->fileInfoVec.size() == 0) {
        fileIteratorEntity->fileInfoVec.clear();
        fileIteratorEntity->offset = 0;
        fileIteratorEntity->pos = 0;
        fileInfoEntity = nullptr;
        objFileInfoExporter = NVal::CreateUndefined(env).val_;
        nVal.AddProp("value", objFileInfoExporter);
        nVal.AddProp("done", NVal::CreateBool(env, true).val_);
        isDone = true;
        return ERR_OK;
    }
    if (fileIteratorEntity->pos == MAX_COUNT) {
        fileIteratorEntity->fileInfoVec.clear();
        fileIteratorEntity->offset += MAX_COUNT;
        fileIteratorEntity->pos = 0;
        int ret = fileIteratorEntity->fileAccessHelper->ScanFile(fileIteratorEntity->fileInfo,
            fileIteratorEntity->offset, MAX_COUNT, fileIteratorEntity->filter, fileIteratorEntity->fileInfoVec);
        if (ret != ERR_OK) {
            HILOG_ERROR("exec ScanFile fail, code:%{public}d", ret);
            return ret;
        }
    }
    if (fileIteratorEntity->pos == fileIteratorEntity->fileInfoVec.size()) {
        fileIteratorEntity->fileInfoVec.clear();
        fileIteratorEntity->offset = 0;
        fileIteratorEntity->pos = 0;
        fileInfoEntity = nullptr;
        objFileInfoExporter = NVal::CreateUndefined(env).val_;
        nVal.AddProp("value", objFileInfoExporter);
        nVal.AddProp("done", NVal::CreateBool(env, true).val_);
        isDone = true;
        return ERR_OK;
    }

    fileInfoEntity->fileAccessHelper = fileIteratorEntity->fileAccessHelper;
    fileInfoEntity->fileInfo = fileIteratorEntity->fileInfoVec[fileIteratorEntity->pos];
    fileIteratorEntity->pos++;
    nVal.AddProp("value", objFileInfoExporter);
    nVal.AddProp("done", NVal::CreateBool(env, false).val_);
    isDone = false;
    return ERR_OK;
}

static bool FilterTrashAndRecentDir(const std::string &uri)
{
    HILOG_INFO("FilterTrashAndRecentDir uri: %{public}s", uri.c_str());
    std::smatch matchResult;
    return std::regex_match(uri, matchResult, TRASH_RECENT_DIR_REGEX) && matchResult.length() > 0;
}

static int GetNextIterator(napi_value &objFileInfoExporter, FileIteratorEntity *fileIteratorEntity,
    FileInfoEntity *fileInfoEntity, napi_env env, NVal& retNVal)
{
    int ret = E_GETRESULT;
    bool isDone = false;
    if (fileIteratorEntity->flag == CALL_LISTFILE) {
        ret = MakeListFileResult(objFileInfoExporter, fileIteratorEntity, fileInfoEntity, env, retNVal, isDone);
    } else if (fileIteratorEntity->flag == CALL_SCANFILE) {
        ret = MakeScanFileResult(objFileInfoExporter, fileIteratorEntity, fileInfoEntity, env, retNVal, isDone);
    }
    while (!isDone && FilterTrashAndRecentDir(fileInfoEntity->fileInfo.uri)) {
        fileInfoEntity = NClass::GetEntityOf<FileInfoEntity>(env, objFileInfoExporter);
        retNVal = NVal::CreateObject(env);
        HILOG_DEBUG("TRASH_DIR or RECENT_DIR: %{public}s", fileInfoEntity->fileInfo.uri.c_str());
        if (fileIteratorEntity->flag == CALL_LISTFILE) {
            ret = MakeListFileResult(objFileInfoExporter, fileIteratorEntity, fileInfoEntity, env, retNVal, isDone);
        } else if (fileIteratorEntity->flag == CALL_SCANFILE) {
            ret = MakeScanFileResult(objFileInfoExporter, fileIteratorEntity, fileInfoEntity, env, retNVal, isDone);
        }
    }
    return ret;
}

napi_value NapiFileIteratorExporter::Next(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    auto fileIteratorEntity = NClass::GetEntityOf<FileIteratorEntity>(env, funcArg.GetThisVar());
    if (fileIteratorEntity == nullptr) {
        NError(E_GETRESULT).ThrowErr(env);
        return nullptr;
    }

    if (IsDirectory(fileIteratorEntity->fileInfo.mode) != ERR_OK) {
        HILOG_ERROR("current FileInfo's mode error");
        return NVal::CreateUndefined(env).val_;
    }

    auto objFileInfoExporter = NClass::InstantiateClass(env, NapiFileInfoExporter::className_, {});
    if (objFileInfoExporter == nullptr) {
        NError(E_GETRESULT).ThrowErr(env);
        return nullptr;
    }

    auto fileInfoEntity = NClass::GetEntityOf<FileInfoEntity>(env, objFileInfoExporter);
    if (fileInfoEntity == nullptr) {
        NError(E_GETRESULT).ThrowErr(env);
        return nullptr;
    }

    if (fileIteratorEntity->fileAccessHelper == nullptr) {
        NError(E_GETRESULT).ThrowErr(env);
        return nullptr;
    }

    auto retNVal = NVal::CreateObject(env);
    
    int ret = GetNextIterator(objFileInfoExporter, fileIteratorEntity, fileInfoEntity, env, retNVal);
    if (ret != ERR_OK) {
        NError(ret).ThrowErr(env);
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
