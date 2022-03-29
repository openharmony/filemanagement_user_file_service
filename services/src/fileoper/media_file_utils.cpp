/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "media_file_utils.h"

#include <algorithm>

#include "data_ability_predicates.h"
#include "file_manager_service_def.h"
#include "file_manager_service_errno.h"
#include "log.h"
#include "media_asset.h"
#include "media_data_ability_const.h"
#include "rdb_errno.h"
#include "values_bucket.h"

using namespace std;
namespace OHOS {
namespace FileManagerService {
bool GetPathFromResult(shared_ptr<NativeRdb::AbsSharedResultSet> result, string &path)
{
    int count = 0;
    result->GetRowCount(count);
    if (count == RESULTSET_EMPTY) {
        ERR_LOG("AbsSharedResultSet null");
        return false;
    }
    int32_t columnIndex = 0;
    GET_COLUMN_INDEX_FROM_NAME(result, Media::MEDIA_DATA_DB_FILE_PATH, columnIndex);
    result->GoToFirstRow();
    int ret = result->GetString(columnIndex, path);
    if (ret != NativeRdb::E_OK) {
        ERR_LOG("NativeRdb gets path index fail");
        return false;
    }
    GET_COLUMN_INDEX_FROM_NAME(result, Media::MEDIA_DATA_DB_RELATIVE_PATH, columnIndex);
    string relativePath;
    ret = result->GetString(columnIndex, relativePath);
    if (ret != NativeRdb::E_OK) {
        relativePath = "";
        DEBUG_LOG("NativeRdb gets relative path is null %{public}d", columnIndex);
    }
    // get relative path from absolute path
    string::size_type pos = path.find_last_of('/');
    if (pos != string::npos) {
        path = relativePath + path.substr(pos + 1) + "/";
    }
    return true;
}

bool IsNumber(const string &str)
{
    if (str.length() == 0) {
        ERR_LOG("IsNumber input is empty ");
        return false;
    }

    for (char const &c : str) {
        if (isdigit(c) == 0) {
            ERR_LOG("Index is not a number");
            return false;
        }
    }

    return true;
}

bool GetPathID(const string &uriPath, string &index)
{
    string::size_type pos = uriPath.find_last_of('/');
    if (pos == string::npos) {
        ERR_LOG("invalid uri %{private}s", uriPath.c_str());
        return false;
    }
    index = uriPath.substr(pos + 1);
    if (!IsNumber(index)) {
        ERR_LOG("invalid uri %{private}s invalid id %{public}s", uriPath.c_str(), index.c_str());
        return false;
    }
    return true;
}

int GetMediaType(const string &name)
{
    int mediaType = Media::MediaAsset::GetMediaType(name);
    if (FILE_MIME_TYPE_MAPS.count(mediaType) == 0) {
        ERR_LOG("invalid mediaType %{public}d", mediaType);
        return FILE_MEDIA_TYPE;
    }
    return mediaType;
}

string GetMimeType(const string &name)
{
    int mediaType = GetMediaType(name);
    if (FILE_MIME_TYPE_MAPS.count(mediaType) == 0) {
        ERR_LOG("invalid mediaType %{public}d", mediaType);
        return FILE_MIME_TYPE;
    }
    return FILE_MIME_TYPE_MAPS.at(mediaType);
}

bool GetPathFromAlbumPath(const string &albumUri, string &path)
{
    string id;
    if (!GetPathID(albumUri, id)) {
        ERR_LOG("GetPathID fails");
        return false;
    }
    string selection = Media::MEDIA_DATA_DB_ID + " LIKE ? ";
    vector<string> selectionArgs = {id};
    shared_ptr<NativeRdb::AbsSharedResultSet> result = MediaFileUtils::DoQuery(selection, selectionArgs);
    if (result == nullptr) {
        ERR_LOG("AbsSharedResultSet null");
        return false;
    }
    return GetPathFromResult(result, path);
}

string GetType(string type)
{
    unordered_map<string, int> typeMap = {
        {"image", Media::MediaType::MEDIA_TYPE_IMAGE},
        {"video", Media::MediaType::MEDIA_TYPE_VIDEO},
        {"audio", Media::MediaType::MEDIA_TYPE_AUDIO}
    };
    if (typeMap.count(type) == 0) {
        // type is wrong
        ERR_LOG("Type %{public}s", type.c_str());
        return "";
    }
    return ToString(typeMap[type]);
}

bool IsFirstLevelUriPath(const string &path)
{
    // check whether path is first level uri
    return path == FISRT_LEVEL_ALBUM;
}

bool GetAlbumFromResult(shared_ptr<NativeRdb::AbsSharedResultSet> &result, vector<string> &album)
{
    int count = 0;
    result->GetRowCount(count);
    result->GoToFirstRow();
    int32_t columnIndex;
    GET_COLUMN_INDEX_FROM_NAME(result, Media::MEDIA_DATA_DB_RELATIVE_PATH, columnIndex);
    for (int i = 0; i < count; i++) {
        string path;
        if (result->GetString(columnIndex, path) != NativeRdb::E_OK) {
            ERR_LOG("NativeRdb gets path columnIndex fail");
            return false;
        }
        path = MEDIA_ROOT_PATH + "/" + path.substr(0, path.size() - 1);
        // add into ablum if not in album
        if (find(album.begin(), album.end(), path) == album.end()) {
            DEBUG_LOG(" // add into ablum path %{public}s", path.c_str());
            album.emplace_back(path);
        }
        result->GoToNextRow();
    }
    return true;
}

vector<string> FindAlbumByType(string type)
{
    // find out the first level Album
    // first find out file by type
    // then get the album
    string selection = Media::MEDIA_DATA_DB_MEDIA_TYPE + " LIKE ?";
    vector<string> selectionArgs = {type};
    shared_ptr<NativeRdb::AbsSharedResultSet> result = MediaFileUtils::DoQuery(selection, selectionArgs);
    vector<string> album;
    if (result == nullptr) {
        ERR_LOG("query album type returns fail");
        return album;
    }
    GetAlbumFromResult(result, album);
    return album;
}

/* listfile
 * ----find "file" type
 * --------first level view----
 * --------selection MEDIA_DATA_DB_RELATIVE_PATH == root_path &&
 * (MEDIA_DATA_DB_MEDIA_TYPE == "file" || MEDIA_DATA_DB_MEDIA_TYPE == "album")
 * --------other level view ----
 * --------selection MEDIA_DATA_DB_RELATIVE_PATH == uri.MEDIA_DATA_DB_FILE_PATH &&
 * (MEDIA_DATA_DB_MEDIA_TYPE == "file" || MEDIA_DATA_DB_MEDIA_TYPE == "album")
 *
 * ----find "image"/"audio"/"video" type
 * --------first level view----
 * --------find out all album with type file
 * --------selection MEDIA_DATA_DB_MEDIA_TYPE == type
 * --------Get the relative path ----> Album path
 * --------selection MEDIA_DATA_DB_FILE_PATH == Album path1 || selection MEDIA_DATA_DB_FILE_PATH == Album path2 || ...
 * --------second level view ----
 * --------selection MEDIA_DATA_DB_RELATIVE_PATH == uri.MEDIA_DATA_DB_FILE_PATH && MEDIA_DATA_DB_MEDIA_TYPE == type
 */
int CreateSelectionAndArgsFirstLevel(const string &type, string &selection, vector<string> &selectionArgs)
{
    if (type == "file") {
        selection = Media::MEDIA_DATA_DB_RELATIVE_PATH + " LIKE ? AND (" + Media::MEDIA_DATA_DB_MEDIA_TYPE;
        selection += " LIKE ? OR " + Media::MEDIA_DATA_DB_MEDIA_TYPE + " LIKE ? )";
        selectionArgs = {
            RELATIVE_ROOT_PATH, ToString(Media::MediaType::MEDIA_TYPE_FILE),
            ToString(Media::MediaType::MEDIA_TYPE_ALBUM)
        };
    } else {
        selectionArgs = FindAlbumByType(GetType(type));
        selection = Media::MEDIA_DATA_DB_FILE_PATH + " LIKE ?";
        if (selectionArgs.size() > 1) {
            for (uint32_t i = 1; i < selectionArgs.size(); i++) {
                selection += " OR " + Media::MEDIA_DATA_DB_FILE_PATH + " LIKE ?";
            }
        }
    }
    return SUCCESS;
}

int CreateSelectionAndArgsOtherLevel(const string &type, const string &albumUri, string &selection,
    vector<string> &selectionArgs)
{
    // get the album path from the album uri
    string albumPath;
    if (!GetPathFromAlbumPath(albumUri, albumPath)) {
        ERR_LOG("path not exsit");
        return E_NOEXIST;
    }
    if (type == "file") {
        selection = Media::MEDIA_DATA_DB_RELATIVE_PATH + " LIKE ? AND (" + Media::MEDIA_DATA_DB_MEDIA_TYPE;
        selection += " LIKE ? OR " + Media::MEDIA_DATA_DB_MEDIA_TYPE + " LIKE ? )";
        selectionArgs = {
            albumPath, ToString(Media::MediaType::MEDIA_TYPE_FILE),
            ToString(Media::MediaType::MEDIA_TYPE_ALBUM)
        };
    } else {
        selection = Media::MEDIA_DATA_DB_RELATIVE_PATH + " LIKE ? AND " + Media::MEDIA_DATA_DB_MEDIA_TYPE + " LIKE ?";
        selectionArgs = { albumPath, GetType(type) };
    }
    return SUCCESS;
}

bool GetAlbumPath(const string &name, const string &path, string &albumPath)
{
    if (IsFirstLevelUriPath(path)) {
        int mediaType = GetMediaType(name);
        albumPath = (MEDIA_TYPE_FOLDER_MAPS.count(mediaType) == 0) ? MEDIA_TYPE_FOLDER_MAPS.at(FILE_MEDIA_TYPE) :
            MEDIA_TYPE_FOLDER_MAPS.at(mediaType);
        return true;
    }
    return GetPathFromAlbumPath(path, albumPath);
}

static void ShowSelecArgs(const string &selection, const vector<string> &selectionArgs)
{
    DEBUG_LOG("selection %{public}s ", selection.c_str());
    for (auto s : selectionArgs) {
        DEBUG_LOG("selectionArgs %{public}s", s.c_str());
    }
}

int MediaFileUtils::DoListFile(const string &type, const string &path, int offset, int count,
    shared_ptr<NativeRdb::AbsSharedResultSet> &result)
{
    string selection;
    vector<string> selectionArgs;
    if (IsFirstLevelUriPath(path)) {
        DEBUG_LOG("IsFirstLevelUriPath");
        CreateSelectionAndArgsFirstLevel(type, selection, selectionArgs);
    } else {
        int err = CreateSelectionAndArgsOtherLevel(type, path, selection, selectionArgs);
        if (err) {
            ERR_LOG("CreateSelectionAndArgsOtherLevel returns fail");
            return err;
        }
    }
    result = DoQuery(selection, selectionArgs, offset, count);
    if (result == nullptr) {
        ERR_LOG("ListFile folder is empty");
        return E_EMPTYFOLDER;
    }
    return SUCCESS;
}

shared_ptr<NativeRdb::AbsSharedResultSet> MediaFileUtils::DoQuery(const string &selection,
    const vector<string> &selectionArgs)
{
    return DoQuery(selection, selectionArgs, 0, MAX_NUM);
}

shared_ptr<NativeRdb::AbsSharedResultSet> MediaFileUtils::DoQuery(const string &selection,
    const vector<string> &selectionArgs, int offset, int count)
{
    ShowSelecArgs(selection, selectionArgs);
    NativeRdb::DataAbilityPredicates predicates;
    predicates.SetWhereClause(selection);
    predicates.SetWhereArgs(selectionArgs);
    predicates.SetOrder("date_taken DESC LIMIT " + ToString(offset) + "," + ToString(count));
    DEBUG_LOG("limit %{public}d, offset %{public}d", count, offset);
    Uri uri = Uri(Media::MEDIALIBRARY_DATA_URI);
    vector<string> columns;
    return abilityHelper->Query(uri, columns, predicates);
}

int MediaFileUtils::DoInsert(const string &name, const string &path, const string &type, string &uri)
{
    NativeRdb::ValuesBucket values;
    string albumPath;
    if (!GetAlbumPath(name, path, albumPath)) {
        ERR_LOG("path not exsit");
        return E_NOEXIST;
    }
    values.PutString(Media::MEDIA_DATA_DB_RELATIVE_PATH, albumPath);
    values.PutString(Media::MEDIA_DATA_DB_NAME, name);
    values.PutString(Media::MEDIA_DATA_DB_MIME_TYPE, GetMimeType(name));
    values.PutInt(Media::MEDIA_DATA_DB_MEDIA_TYPE, GetMediaType(name));
    Uri createAsset(Media::MEDIALIBRARY_DATA_URI + "/" + Media::MEDIA_FILEOPRN + "/" +
        Media::MEDIA_FILEOPRN_CREATEASSET);
    int index = abilityHelper->Insert(createAsset, values);
    if (index < 0) {
        ERR_LOG("Fail to create fail file %{public}s uri %{private}s album %{public}s", name.c_str(),
            path.c_str(), albumPath.c_str());
        return E_CREATE_FAIL;
    }
    // use file id concatenate head as uri
    uri = (MEDIA_TYPE_URI_MAPS.count(GetMediaType(name)) == 0) ? MEDIA_TYPE_URI_MAPS.at(FILE_MEDIA_TYPE) :
        MEDIA_TYPE_URI_MAPS.at(GetMediaType(name));
    uri += "/" + to_string(index);

    return SUCCESS;
}

bool MediaFileUtils::InitMediaTableColIndexMap(shared_ptr<NativeRdb::AbsSharedResultSet> result)
{
    if (mediaTableMap.size() == 0) {
        DEBUG_LOG("init mediaTableMap");
        vector<pair<string, string>> mediaData = {
            {Media::MEDIA_DATA_DB_ID, "string"},
            {Media::MEDIA_DATA_DB_URI, "string"},
            {Media::MEDIA_DATA_DB_MEDIA_TYPE, "string"},
            {Media::MEDIA_DATA_DB_NAME, "string"},
            {Media::MEDIA_DATA_DB_SIZE, "int"},
            {Media::MEDIA_DATA_DB_DATE_ADDED, "int"},
            {Media::MEDIA_DATA_DB_DATE_MODIFIED, "int"}
        };
        for (auto i : mediaData) {
            int columnIndex = 0;
            GET_COLUMN_INDEX_FROM_NAME(result, i.first, columnIndex);
            mediaTableMap.emplace_back(columnIndex, i.second);
        }
    }
    return true;
}

bool MediaFileUtils::GetFileInfo(shared_ptr<NativeRdb::AbsSharedResultSet> result,
    shared_ptr<FileInfo> &fileInfo)
{
    if (!InitMediaTableColIndexMap(result)) {
        ERR_LOG("InitMediaTableColIndexMap returns fail");
        return false;
    }
    int index = 0;
    string id;
    result->GetString(mediaTableMap[index++].first, id);
    string uri;
    result->GetString(mediaTableMap[index++].first, uri);

    string path = uri + "/" + id;
    fileInfo->SetPath(path);
    string type;
    result->GetString(mediaTableMap[index++].first, type);
    fileInfo->SetType(type);
    string name;
    result->GetString(mediaTableMap[index++].first, name);
    fileInfo->SetName(name);
    int64_t value;
    result->GetLong(mediaTableMap[index++].first, value);
    fileInfo->SetSize(value);
    result->GetLong(mediaTableMap[index++].first, value);
    fileInfo->SetAddedTime(value);
    result->GetLong(mediaTableMap[index++].first, value);
    fileInfo->SetModifiedTime(value);
    return true;
}

int MediaFileUtils::GetFileInfoFromResult(shared_ptr<NativeRdb::AbsSharedResultSet> result,
    std::vector<shared_ptr<FileInfo>> &fileList)
{
    int count = 0;
    result->GetRowCount(count);
    if (count == 0) {
        ERR_LOG("AbsSharedResultSet null");
        return E_EMPTYFOLDER;
    }
    result->GoToFirstRow();
    for (int i = 0; i < count; i++) {
        shared_ptr<FileInfo> fileInfo = make_shared<FileInfo>();
        GetFileInfo(result, fileInfo);
        fileList.push_back(fileInfo);
        result->GoToNextRow();
    }
    return SUCCESS;
}

bool MediaFileUtils::InitHelper(sptr<IRemoteObject> obj)
{
    if (abilityHelper == nullptr) {
        abilityHelper =  AppExecFwk::DataAbilityHelper::Creator(obj, make_shared<Uri>(Media::MEDIALIBRARY_DATA_URI));
        if (abilityHelper == nullptr) {
            DEBUG_LOG("get %{private}s helper fail", Media::MEDIALIBRARY_DATA_URI.c_str());
            return false;
        }
    }
    return true;
}

int MediaFileUtils::DoGetRoot(const std::string &name, const std::string &path,
    std::vector<shared_ptr<FileInfo>> &fileList)
{
    shared_ptr<FileInfo> image = make_shared<FileInfo>(IMAGE_ROOT_NAME, FISRT_LEVEL_ALBUM, ALBUM_TYPE);
    fileList.emplace_back(image);
    shared_ptr<FileInfo> video = make_shared<FileInfo>(VIDEO_ROOT_NAME, FISRT_LEVEL_ALBUM, ALBUM_TYPE);
    fileList.emplace_back(video);
    shared_ptr<FileInfo> audio = make_shared<FileInfo>(AUDIO_ROOT_NAME, FISRT_LEVEL_ALBUM, ALBUM_TYPE);
    fileList.emplace_back(audio);
    shared_ptr<FileInfo> file = make_shared<FileInfo>(FILE_ROOT_NAME, FISRT_LEVEL_ALBUM, ALBUM_TYPE);
    fileList.emplace_back(file);
    return SUCCESS;
}
} // namespace FileManagerService
} // namespace OHOS