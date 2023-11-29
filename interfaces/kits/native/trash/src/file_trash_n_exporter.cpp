/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "file_trash_n_exporter.h"

#include <ctime>

#include "access_token.h"
#include "accesstoken_kit.h"
#include "file_access_framework_errno.h"
#include "file_info.h"
#include "file_uri.h"
#include "file_util.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace Trash {
namespace {
    const std::string FILE_ACCESS_PERMISSION = "ohos.permission.FILE_ACCESS_MANAGER";
}

using namespace FileManagement::LibN;
using namespace FileManagement;
using namespace std;

static bool CheckCallingPermission(const std::string &permission)
{
    Security::AccessToken::AccessTokenID tokenCaller = IPCSkeleton::GetCallingTokenID();
    int res = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller, permission);
    if (res != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        HILOG_ERROR("FileTrashNExporter::CheckCallingPermission have no fileAccess permission");
        return false;
    }
    return true;
}

static bool GetRealPath(string &path)
{
    unique_ptr<char[]> absPath = make_unique<char[]>(PATH_MAX + 1);
    if (realpath(path.c_str(), absPath.get()) == nullptr) {
        return false;
    }
    path = absPath.get();
    return true;
}

static string GetTimeSlotFromPath(const string &path)
{
    size_t slashSize = 1;
    // 获取时间戳
    size_t trashPathPrefixPos = path.find(FileTrashNExporter::trashPath_);
    size_t expectTimeSlotStartPos = trashPathPrefixPos + FileTrashNExporter::trashPath_.length() + slashSize;
    if (expectTimeSlotStartPos >= path.length()) {
        return "";
    }
    string realFilePathWithTime = path.substr(trashPathPrefixPos + FileTrashNExporter::trashPath_.length() + slashSize);
    // 获取时间戳目录位置
    size_t trashPathWithTimePrefixPos = realFilePathWithTime.find_first_of("/");
    if (trashPathWithTimePrefixPos == string::npos) {
        HILOG_ERROR("GetTimeSlotFromPath: Invalid path = %{public}s", path.c_str());
        return "";
    }
    string timeSlot = realFilePathWithTime.substr(0, trashPathWithTimePrefixPos);
    HILOG_DEBUG("GetTimeSlotFromPath: timeSlot = %{public}s", timeSlot.c_str());
    return timeSlot;
}

static int RecursiveFunc(const string &path, vector<string> &dirents)
{
    unique_ptr<struct NameListArg, decltype(Deleter)*> pNameList = { new (nothrow) struct NameListArg, Deleter };
    if (!pNameList) {
        HILOG_ERROR("Failed to request heap memory.");
        return ENOMEM;
    }
    HILOG_DEBUG("RecursiveFunc: scandir path = %{public}s", path.c_str());
    int num = scandir(path.c_str(), &(pNameList->namelist), FilterFunc, alphasort);
    if (num < 0) {
        HILOG_ERROR("RecursiveFunc: Failed to scan dir");
        return errno;
    }
    pNameList->direntNum = num;
    string pathInRecur = path;
    for (int i = 0; i < num; i++) {
        if ((*(pNameList->namelist[i])).d_type == DT_REG) {
            dirents.emplace_back(path + '/' + pNameList->namelist[i]->d_name);
        } else if ((*(pNameList->namelist[i])).d_type == DT_DIR) {
            string pathTemp = pathInRecur;
            pathInRecur += '/' + string((*(pNameList->namelist[i])).d_name);
            // check if path include TRASH_SUB_DIR + "/", need to add it into dirents
            HILOG_DEBUG("RecursiveFunc: pathTemp = %{public}s", pathTemp.c_str());
            string timeSlot = GetTimeSlotFromPath(pathTemp);
            if (!timeSlot.empty() && pathInRecur.rfind(TRASH_SUB_DIR + timeSlot + "/") != string::npos) {
                // Only filter previous dir is TRASH_SUB_DIR
                dirents.emplace_back(pathInRecur);
            }
            int ret = RecursiveFunc(pathInRecur, dirents);
            if (ret != ERRNO_NOERR) {
                HILOG_ERROR("RecursiveFunc: Failed to recursive get all dirents for %{public}d", ret);
                return ret;
            }
            pathInRecur = pathTemp;
        }
    }
    return ERRNO_NOERR;
}

static napi_value CreateObjectArray(napi_env env, vector<FileInfo> result)
{
    uint32_t status = napi_ok;
    napi_value fileInfoResultArray = nullptr;
    status = napi_create_array_with_length(env, result.size(), &fileInfoResultArray);
    if (status != napi_ok) {
        HILOG_ERROR("Create napi array fail");
        return nullptr;
    }

    for (size_t i = 0; i < result.size(); i++) {
        FileInfo &tmpResult = result.at(i);
        napi_value resultVal;
        status |= napi_create_object(env, &resultVal);
        napi_value tmpVal;
        status |= napi_create_string_utf8(env, tmpResult.uri.c_str(), tmpResult.uri.length(), &tmpVal);
        status |= napi_set_named_property(env, resultVal, "uri", tmpVal);
        status |= napi_create_string_utf8(env, tmpResult.srcPath.c_str(), tmpResult.srcPath.length(), &tmpVal);
        status |= napi_set_named_property(env, resultVal, "srcPath", tmpVal);
        status |= napi_create_string_utf8(env, tmpResult.fileName.c_str(), tmpResult.fileName.length(), &tmpVal);
        status |= napi_set_named_property(env, resultVal, "fileName", tmpVal);
        status |= napi_create_int64(env, tmpResult.mode, &tmpVal);
        status |= napi_set_named_property(env, resultVal, "mode", tmpVal);
        status |= napi_create_int64(env, tmpResult.mode, &tmpVal);
        status |= napi_set_named_property(env, resultVal, "mode", tmpVal);
        status |= napi_create_int64(env, tmpResult.size, &tmpVal);
        status |= napi_set_named_property(env, resultVal, "size", tmpVal);
        status |= napi_create_int64(env, tmpResult.mtime, &tmpVal);
        status |= napi_set_named_property(env, resultVal, "mtime", tmpVal);
        status |= napi_create_int64(env, tmpResult.ctime, &tmpVal);
        status |= napi_set_named_property(env, resultVal, "ctime", tmpVal);
        status |= napi_set_element(env, fileInfoResultArray, i, resultVal);
        if (status != napi_ok) {
            HILOG_ERROR("Create CopyResult object error");
            return nullptr;
        }
    }
    return fileInfoResultArray;
}

static string FindSourceFilePath(const string &path)
{
    HILOG_INFO("FindSourceFilePath: curFilePath = %{public}s", path.c_str());
    size_t slashSize = 1;
    // 获取/trash目录位置
    size_t trashPathPrefixPos = path.find(FileTrashNExporter::trashPath_);
    if (trashPathPrefixPos == string::npos) {
        HILOG_ERROR("FindSourceFilePath: Invalid Path No Trash Path");
        return "";
    }
    size_t timeSLotStartPos = trashPathPrefixPos + FileTrashNExporter::trashPath_.length() + slashSize;
    string realFilePathWithTime = path.substr(timeSLotStartPos);
    // 获取时间戳目录位置
    size_t trashPathWithTimePrefixPos = realFilePathWithTime.find_first_of("/");
    if (trashPathWithTimePrefixPos == string::npos) {
        HILOG_ERROR("FindSourceFilePath: : Invalid Path No timestamp");
        return "";
    }
    // 获取时间戳
    string timeSlot = realFilePathWithTime.substr(0, trashPathWithTimePrefixPos);
    string realFilePath = realFilePathWithTime.substr(trashPathWithTimePrefixPos + slashSize);
    size_t pos = realFilePath.rfind(TRASH_SUB_DIR + timeSlot + "/");
    if (pos == string::npos) {
        HILOG_ERROR("FindSourceFilePath: : Invalid Path No Trash Sub Path");
        return "";
    }
    string realFilePathPrefix = realFilePath.substr(0, pos);
    string realFileName = realFilePath.substr(pos + TRASH_SUB_DIR.length() +
        timeSlot.length() + slashSize);
    realFilePath = "/" + realFilePathPrefix + realFileName;
    HILOG_INFO("FindSourceFilePath: realFilePath After = %{public}s", realFilePath.c_str());
    return realFilePath;
}

static bool Mkdirs(const string &path, bool isDir, string &newRecoveredPath)
{
    HILOG_INFO("Mkdirs: path = %{public}s", path.c_str());
    string recoveredPath = path;
    string folderName = "";
    size_t lastPos = 0;
    if (recoveredPath.length() == 0) {
        return false;
    }
    // if argument uri is dir, then add "/"
    if (isDir) {
        recoveredPath = recoveredPath + "/";
    }

    for (size_t i = 1; i < recoveredPath.length(); ++i) {
        if (recoveredPath[i] != '/') {
            continue;
        }
        recoveredPath[i] = '\0';
        folderName = recoveredPath.substr(lastPos + 1, i);
        lastPos = i;
        auto [isExist, ret] = Access(recoveredPath);
        if (!isExist && !Mkdir(recoveredPath)) {
            HILOG_ERROR("Mkdirs fail for %{public}s ", recoveredPath.c_str());
            return false;
        }
        recoveredPath[i] = '/';
    }
    return true;
}

static string GenerateNewFileNameWithSuffix(const string &destFile, int32_t distinctSuffixIndex,
    const string &newPrefix, const string &newSuffix)
{
    // 处理存在后缀名的文件
    auto [isExist, ret] = Access(destFile);
    if (isExist) {
        distinctSuffixIndex += 1;
        string newDestFile = newPrefix + to_string(distinctSuffixIndex) + newSuffix;
        return GenerateNewFileNameWithSuffix(newDestFile, distinctSuffixIndex, newPrefix, newSuffix);
    } else if (ret == ERRNO_NOERR) {
        HILOG_DEBUG("GenerateNewFileNameWithSuffix: destFile = %{public}s", destFile.c_str());
        return destFile;
    }
    return "";
}

static string GenerateNewFileNameNoSuffix(const string &destFile, int32_t distinctSuffixIndex, const string &newPrefix)
{
    // 处理不存在后缀名的文件
    auto [isExist, ret] = Access(destFile);
    if (isExist) {
        distinctSuffixIndex += 1;
        string newDestFile = newPrefix + to_string(distinctSuffixIndex);
        return GenerateNewFileNameNoSuffix(newDestFile, distinctSuffixIndex, newPrefix);
    } else if (ret == ERRNO_NOERR) {
        HILOG_DEBUG("GenerateNewFileNameNoSuffix: destFile = %{public}s", destFile.c_str());
        return destFile;
    }
    return "";
}

static int MoveFile(const string &srcFile, const string &destFile)
{
    // 判断目的文件是否存在
    auto [isExist, ret] = Access(destFile);
    if (isExist) {
        // 存在同名文件，需要加上数字后缀区分
        // 获取文件前一级目录'/' 位置，从这个位置出发寻找文件后缀分隔符'.'
        size_t slashPos = destFile.find_last_of("/");
        if (slashPos == string::npos) {
            HILOG_ERROR("MoveFile: : Invalid Path");
            return EINVAL;
        }
        // 识别文件后缀分隔符，找最后一个
        size_t suffixPos = destFile.find_last_of('.');
        if (suffixPos < slashPos) {
            // 识别的文件后缀分隔符必须在文件部分
            suffixPos = string::npos;
        }
        string newDestFile = destFile;
        int32_t distinctSuffixIndex = 1;
        if (suffixPos == string::npos) {
            string newPrefix = newDestFile + " ";
            newDestFile = newPrefix + to_string(distinctSuffixIndex);
            newDestFile = GenerateNewFileNameNoSuffix(newDestFile, distinctSuffixIndex, newPrefix);
        } else {
            string newPrefix = destFile.substr(0, suffixPos) + " ";
            string newSuffix =  destFile.substr(suffixPos);
            HILOG_DEBUG("MoveFile: newPrefix = %{public}s", newPrefix.c_str());
            HILOG_DEBUG("MoveFile: newSuffix = %{public}s", newSuffix.c_str());
            // 查看加上数字后缀后文件是否已经存在，若存在，需要重新获取
            newDestFile = GenerateNewFileNameWithSuffix(newPrefix + to_string(distinctSuffixIndex) + newSuffix,
                distinctSuffixIndex, newPrefix, newSuffix);
        }
        HILOG_INFO("MoveFile: newDestFile = %{public}s", newDestFile.c_str());
        return RenameFile(srcFile, newDestFile);
    } else if (ret == ERRNO_NOERR) {
        return RenameFile(srcFile, destFile);
    }
    HILOG_ERROR("MoveFile: : Invalid Path");
    return EINVAL;
}

static string RecurCheckIfOnlyContentInDir(const string &path, size_t trashWithTimePos, const string &trashWithTimePath)
{
    HILOG_INFO("RecurCheckIfOnlyContentInDir: path = %{public}s", path.c_str());
    size_t slashPos = path.find_last_of("/");
    if (slashPos <= trashWithTimePos) {
        HILOG_DEBUG("RecurCheckIfOnlyContentInDir: slashPos = %{public}zu", slashPos);
        return trashWithTimePath;
    }
    string parentPath = path.substr(0, slashPos);
    HILOG_DEBUG("RecurCheckIfOnlyContentInDir: parentPath = %{public}s", parentPath.c_str());
    int num = ScanDir(parentPath);
    HILOG_DEBUG("RecurCheckIfOnlyContentInDir: num = %{public}d", num);
    if (num > 1) {
        // 同一时间戳目录下存在多个删除项，则不论是还原后的删除还是彻底删除，仅需删除该项
        HILOG_DEBUG("RecurCheckIfOnlyContentInDir: find other items in current dir");
        return path;
    } else if (num == 1) {
        // 需要向上一层目录判断
        return RecurCheckIfOnlyContentInDir(parentPath, trashWithTimePos, trashWithTimePath);
    } else {
        HILOG_ERROR("RecurCheckIfOnlyContentInDir: invalid path = %{public}s", path.c_str());
    }
    return nullptr;
}

static string GetToDeletePath(const string &toDeletePath, napi_env env)
{
    HILOG_INFO("GetToDeletePath: toDeletePath = %{public}s", toDeletePath.c_str());
    // 判断是否为有效回收站路径
    size_t slashSize = 1;
    // 获取/Trash目录位置
    size_t trashPathPrefixPos = toDeletePath.find(FileTrashNExporter::trashPath_);
    if (trashPathPrefixPos == string::npos ||
        trashPathPrefixPos + FileTrashNExporter::trashPath_.length() + slashSize >= toDeletePath.length()) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    string realFilePathWithTime = toDeletePath.substr(trashPathPrefixPos + FileTrashNExporter::trashPath_.length() + slashSize);
    // 获取时间戳目录位置
    size_t trashPathWithTimePrefixPos = realFilePathWithTime.find_first_of("/");
    size_t realTimeDirPos = trashPathPrefixPos + FileTrashNExporter::trashPath_.length() +
        slashSize + trashPathWithTimePrefixPos;
    // 回收站下一级的时间戳目录
    string trashWithTimePath = toDeletePath.substr(0, realTimeDirPos);

    // 从待删除目录开始向内层遍历父目录，判断父目录是否仅有一个子目录，如果是则继续向前查找，直到时间戳目录为止；
    // 如果不是仅有一个子目录，则待删除目录即子目录
    return RecurCheckIfOnlyContentInDir(toDeletePath, realTimeDirPos, trashWithTimePath);
}

static bool GenerateFileInfoEntity(FileInfo& fileInfoEntity, string filterDirent, string timeSlot)
{
    string realFilePath = FindSourceFilePath(filterDirent);
    size_t lastSlashPos = filterDirent.find_last_of("/");
    if (lastSlashPos == string::npos) {
        HILOG_ERROR("GenerateFileInfoEntity: invalid path");
        return false;
    }
    string fileName = filterDirent.substr(lastSlashPos + 1);

    AppFileService::ModuleFileUri::FileUri fileUri(filterDirent);
    fileInfoEntity.uri = fileUri.ToString();
    fileInfoEntity.srcPath = realFilePath;
    fileInfoEntity.fileName = fileName;

    size_t uMode = SUPPORTS_READ | SUPPORTS_WRITE;
    StatEntity statEntity;
    if (GetStat(filterDirent, statEntity)) {
        bool check = (statEntity.stat_.st_mode & S_IFMT) == S_IFDIR;
        if (check) {
            uMode |= REPRESENTS_DIR;
        } else {
            uMode |= REPRESENTS_FILE;
        }
        HILOG_DEBUG("ListFile: After filter mode  = %{public}zu", uMode);

        fileInfoEntity.mode = static_cast<int32_t>(uMode);
        fileInfoEntity.size = static_cast<int64_t>(statEntity.stat_.st_size);
        fileInfoEntity.mtime = static_cast<int64_t>(statEntity.stat_.st_mtim.tv_sec);

        try {
            fileInfoEntity.ctime = static_cast<int64_t>(atoll(timeSlot.c_str()) / SECOND_TO_MILLISECOND);
        } catch (...) {
            HILOG_ERROR("GenerateFileInfoEntity: invalid timeSlot = %{public}s", timeSlot.c_str());
            return false;
        }
    }
    return true;
}

napi_value FileTrashNExporter::ListFile(napi_env env, napi_callback_info info)
{
    if (!CheckCallingPermission(FILE_ACCESS_PERMISSION)) {
        HILOG_ERROR("permission error");
        NError(E_PERMISSION).ThrowErr(env);
        return nullptr;
    }

    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        HILOG_ERROR("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    vector<string> dirents;
    unique_ptr<struct NameListArg, decltype(Deleter)*> pNameList = { new (nothrow) struct NameListArg, Deleter };
    if (!pNameList) {
        NError(ENOMEM).ThrowErr(env);
        HILOG_ERROR("Failed to request heap memory.");
        return nullptr;
    }
    int ret = RecursiveFunc(FileTrashNExporter::trashPath_, dirents);
    if (ret != ERRNO_NOERR) {
        NError(ENOMEM).ThrowErr(env);
        HILOG_ERROR("ListFile: Failed to recursive all Trash items path");
        return nullptr;
    }

    vector<FileInfo> fileInfoList;
    size_t slashSize = 1;
    for (size_t j = 0; j < dirents.size(); j++) {
        string dirent = dirents[j];
        string timeSlot = GetTimeSlotFromPath(dirent);
        if (timeSlot.empty()) {
            continue;
        }
        // Only filter previous dir is TRASH_SUB_DIR
        size_t pos = dirent.find(TRASH_SUB_DIR + timeSlot + "/");
        if (pos != string::npos) {
            string trashSubDir = TRASH_SUB_DIR + timeSlot;
            FileInfo info;
            if ((dirent.find("/", pos + trashSubDir.length() + slashSize) == string::npos) &&
                GenerateFileInfoEntity(info, dirent, timeSlot)) {
                fileInfoList.emplace_back(info);
            }
        }
    }
    return CreateObjectArray(env, fileInfoList);
}

static napi_value RecoverFile(napi_env env, const string &filePath)
{
    string sourceFilePath = FindSourceFilePath(filePath);
    HILOG_INFO("RecoverFile: sourceFilePath = %{public}s", sourceFilePath.c_str());
    string newDestPath = sourceFilePath;
    if (newDestPath.length() == 0 || !Mkdirs(sourceFilePath, false, newDestPath)) {
        HILOG_ERROR("RecoverFile: Mkdirs failed");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }

    int moveRet = MoveFile(filePath, newDestPath);
    if (moveRet != ERRNO_NOERR) {
        HILOG_ERROR("RecoverFile: MoveFile failed");
        NError(moveRet).ThrowErr(env);
        return nullptr;
    }

    // 文件已被移动，则如果前一层目录包含其他内容，则直接返回；
    // 如果不包含，则需要一层层向父目录回退判断对应目录是否需要删除
    size_t slashPos = filePath.find_last_of("/");
    string parentPath = filePath.substr(0, slashPos);
    int num = ScanDir(parentPath);
    if (num == 0) {
        auto err = RmDirent(GetToDeletePath(parentPath, env));
        if (err) {
            err.ThrowErr(env);
            return nullptr;
        }
    }
    return NVal::CreateUndefined(env).val_;
}

static int RecoverFilePart(vector<string> filePathList, map<string, string> dirPath2UpdatedNameMap)
{
    // 处理文件
    for (size_t j = 0; j < filePathList.size(); j++) {
        string filePath = filePathList[j];
        HILOG_INFO("RecoverFilePart: filePath  = %{public}s", filePath.c_str());
        string sourceFilePath = FindSourceFilePath(filePath);
        HILOG_INFO("RecoverFilePart: sourceFilePath  = %{public}s", sourceFilePath.c_str());

        size_t lastSlashPos = sourceFilePath.find_last_of("/");
        string fileName = sourceFilePath.substr(lastSlashPos + 1);
        string sourceFilePathOnly = sourceFilePath.substr(0, lastSlashPos);
        map<string, string>::iterator iter = dirPath2UpdatedNameMap.find(sourceFilePathOnly);
        if (iter != dirPath2UpdatedNameMap.end()) {
            sourceFilePath = iter->second + "/" + fileName;
        }
        int moveRet = MoveFile(filePath, sourceFilePath);
        if (moveRet != ERRNO_NOERR) {
            return moveRet;
        }
    }
    return ERRNO_NOERR;
}

static map<string, string> MakeAndFindUpdateNameDir(vector<string> filterDirPathList)
{
    map<string, string> dirPath2UpdatedNameMap;
    for (size_t j = 0; j < filterDirPathList.size(); j++) {
        string dirPath = filterDirPathList[j];
        string sourceFilePath = FindSourceFilePath(dirPath);
        HILOG_DEBUG("MakeAndFindUpdateNameDir: sourceFilePath  = %{public}s", sourceFilePath.c_str());
        string newDestPath = sourceFilePath;
        if (Mkdirs(sourceFilePath, true, newDestPath)) {
            HILOG_DEBUG("MakeAndFindUpdateNameDir: newDestPath  = %{public}s", newDestPath.c_str());
            if (newDestPath != sourceFilePath) {
                dirPath2UpdatedNameMap.insert(make_pair(sourceFilePath, newDestPath));
            }
        }
    }
    return dirPath2UpdatedNameMap;
}

static napi_value RecoverDir(napi_env env, const string &dirPath)
{
    vector<string> dirents;
    unique_ptr<struct NameListArg, decltype(Deleter)*> pNameList = { new (nothrow) struct NameListArg, Deleter };
    if (!pNameList) {
        HILOG_ERROR("RecoverDir: Failed to request heap memory.");
        return nullptr;
    }
    int ret = RecursiveFunc(dirPath, dirents);
    if (ret != ERRNO_NOERR) {
        HILOG_ERROR("RecoverDir: Failed to Recursive Dir.");
        return nullptr;
    }
    dirents.emplace_back(dirPath);

    // 区分目录和文件
    vector<string> dirPathList;
    vector<string> filePathList;
    for (size_t j = 0; j < dirents.size(); j++) {
        string dirent = dirents[j];
        if (CheckDir(dirent)) {
            dirPathList.emplace_back(dirent);
        } else {
            filePathList.emplace_back(dirent);
        }
    }

    // 新建目录并获取因为存在同名目录修改过名称的目录
    map<string, string> dirPath2UpdatedNameMap = MakeAndFindUpdateNameDir(dirPathList);

    // 处理文件部分
    auto retRecoveFilePart = RecoverFilePart(filePathList, dirPath2UpdatedNameMap);
    if (retRecoveFilePart != ERRNO_NOERR) {
        NError(retRecoveFilePart).ThrowErr(env);
        HILOG_ERROR("RecoverFilePart: Failed to Recover File in Dir.");
        return nullptr;
    }
    
    // 删除目录
    auto err = RmDirent(GetToDeletePath(dirPath, env));
    if (err) {
        err.ThrowErr(env);
        return nullptr;
    }

    return NVal::CreateUndefined(env).val_;
}

napi_value FileTrashNExporter::Recover(napi_env env, napi_callback_info info)
{
    if (!CheckCallingPermission(FILE_ACCESS_PERMISSION)) {
        HILOG_ERROR("permission error");
        NError(E_PERMISSION).ThrowErr(env);
        return nullptr;
    }

    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        HILOG_ERROR("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    bool succ = false;
    unique_ptr<char[]> uriPtr;
    tie(succ, uriPtr, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    if (!succ) {
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    string uriStr = uriPtr.get();
    HILOG_DEBUG("Recover: uriPtr.get()  = %{public}s", uriStr.c_str());

    // 获取沙箱目录地址
    AppFileService::ModuleFileUri::FileUri fileUri(uriStr);
    string path = fileUri.GetPath();
    // 判断绝对路径
    if (!GetRealPath(path)) {
        NError(EINVAL).ThrowErr(env);
        HILOG_ERROR("Recover: Invalid Path");
        return nullptr;
    }
    HILOG_DEBUG("Recover: path  = %{public}s", path.c_str());

    // 判断是否是回收站路径
    if (path.find(FileTrashNExporter::trashPath_) == string::npos) {
        NError(EINVAL).ThrowErr(env);
        HILOG_ERROR("Recover: path  = %{public}s is not Trash path", path.c_str());
        return nullptr;
    }

    // 判断路径是否存在
    auto [isExist, ret] = Access(path);
    if (!isExist) {
        NError(EINVAL).ThrowErr(env);
        HILOG_ERROR("Recover: Path is not exist");
        return nullptr;
    }

    if (!CheckDir(path)) {
        return RecoverFile(env, path);
    }
    return RecoverDir(env, path);
}

napi_value FileTrashNExporter::CompletelyDelete(napi_env env, napi_callback_info info)
{
    if (!CheckCallingPermission(FILE_ACCESS_PERMISSION)) {
        HILOG_ERROR("permission error");
        NError(E_PERMISSION).ThrowErr(env);
        return nullptr;
    }

    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        HILOG_ERROR("Number of arguments unmatched");
        NError(EINVAL).ThrowErr(env);
        return nullptr;
    }
    bool succ = false;
    unique_ptr<char[]> uriPtr;
    tie(succ, uriPtr, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    if (!succ) {
        NError(EINVAL).ThrowErr(env);
        HILOG_ERROR("Recover: Invalid arguments");
        return nullptr;
    }

    string uriStr = uriPtr.get();
    HILOG_DEBUG("Recover: uriPtr.get()  = %{public}s", uriStr.c_str());

    // 获取沙箱目录地址
    AppFileService::ModuleFileUri::FileUri fileUri(uriStr);
    string path = fileUri.GetPath();
    // 判断绝对路径
    if (!GetRealPath(path)) {
        NError(EINVAL).ThrowErr(env);
        HILOG_ERROR("Recover: Invalid Path");
        return nullptr;
    }
    HILOG_DEBUG("Recover: path  = %{public}s", path.c_str());

    // 判断是否是回收站路径
    if (path.find(FileTrashNExporter::trashPath_) == string::npos) {
        NError(EINVAL).ThrowErr(env);
        HILOG_ERROR("Recover: path  = %{public}s is not Trash path", path.c_str());
        return nullptr;
    }

    // 判断路径是否存在
    auto [isExist, ret] = Access(path);
    if (!isExist) {
        NError(EINVAL).ThrowErr(env);
        HILOG_ERROR("Recover: Path is not exist");
        return nullptr;
    }

    // 删除目录
    auto err = RmDirent(GetToDeletePath(path, env));
    if (err) {
        err.ThrowErr(env);
        return nullptr;
    }
    return NVal::CreateUndefined(env).val_;
}

bool FileTrashNExporter::Export()
{
    return exports_.AddProp({
        NVal::DeclareNapiFunction("listFile", ListFile),
        NVal::DeclareNapiFunction("recover", Recover),
        NVal::DeclareNapiFunction("completelyDelete", CompletelyDelete)
    });
}

string FileTrashNExporter::GetClassName()
{
    return FileTrashNExporter::className_;
}

FileTrashNExporter::FileTrashNExporter(napi_env env, napi_value exports) : NExporter(env, exports) {}

FileTrashNExporter::~FileTrashNExporter() {}
} // namespace Trash
} // namespace OHOS