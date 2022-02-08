/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "file_manager_service_def.h"
#include "file_manager_service_errno.h"

#include "media_asset.h"
#include "media_data_ability_const.h"
#include "medialibrary_data_ability.h"
#include "log.h"

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
    int32_t columnIndex;
    int ret = result->GetColumnIndex(Media::MEDIA_DATA_DB_FILE_PATH, columnIndex);
    if (ret != NativeRdb::E_OK) {
        ERR_LOG("NativeRdb gets MEDIA_DATA_DB_FILE_PATH index fail");
        return false;
    }
    result->GoToFirstRow();
    ret = result->GetString(columnIndex, path);
    if (ret != NativeRdb::E_OK) {
        ERR_LOG("NativeRdb gets path index fail");
        return false;
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
        ERR_LOG("invalid uri %{public}s", uriPath.c_str());
        return false;
    }
    index = uriPath.substr(pos + 1);
    if (!IsNumber(index)) {
        ERR_LOG("invalid uri %{public}s invalid id %{public}s", uriPath.c_str(), index.c_str());
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
    int ret = result->GetColumnIndex(Media::MEDIA_DATA_DB_RELATIVE_PATH, columnIndex);
    if (ret != NativeRdb::E_OK) {
        ERR_LOG("NativeRdb  gets MEDIA_DATA_DB_RELATIVE_PATH columnIndex fail");
        return false;
    }

    for (int i = 0; i < count; i++) {
        string path;
        ret = result->GetString(columnIndex, path);
        if (ret != NativeRdb::E_OK) {
            ERR_LOG("NativeRdb gets path columnIndex fail");
            return false;
        }
        auto it = find(album.begin(), album.end(), path);
        if (it == album.end()) {
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
            ROOT_PATH, ToString(Media::MediaType::MEDIA_TYPE_FILE),
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

int MediaFileUtils::DoListFile(const string &type, const string &path, int offset, int count,
    shared_ptr<NativeRdb::AbsSharedResultSet> &result)
{
    string selection;
    vector<string> selectionArgs;
    if (IsFirstLevelUriPath(path)) {
        DEBUG_LOG("IsFirstLevelUriPath ");
        CreateSelectionAndArgsFirstLevel(type, selection, selectionArgs);
    } else {
        int err = CreateSelectionAndArgsOtherLevel(type, path, selection, selectionArgs);
        if (err) {
            ERR_LOG("CreateSelectionAndArgsOtherLevel returns fail");
            return err;
        }
    }
    result = DoQuery(selection, selectionArgs);
    if (result == nullptr) {
        ERR_LOG("ListFile folder is empty");
        return E_EMPTYFOLDER;
    }
    return SUCCESS;
}

shared_ptr<NativeRdb::AbsSharedResultSet> MediaFileUtils::DoQuery(string selection, vector<string> selectionArgs)
{
    NativeRdb::DataAbilityPredicates predicates;
    predicates.SetWhereClause(selection);
    predicates.SetWhereArgs(selectionArgs);
    Media::MediaLibraryDataAbility mediaLibDb;
    mediaLibDb.InitMediaLibraryRdbStore();
    Uri uri = Uri(Media::MEDIALIBRARY_DATA_URI);
    vector<string> columns;
    return mediaLibDb.Query(uri, columns, predicates);
}

int MediaFileUtils::DoInsert(const string &name, const string &path, const string &type, string &uri)
{
    Media::ValuesBucket values;

    string albumPath;
    if (!GetAlbumPath(name, path, albumPath)) {
        ERR_LOG("path not exsit");
        return E_NOEXIST;
    }
    // album path + name ---> MEDIA_DATA_DB_FILE_PATH
    values.PutString(Media::MEDIA_DATA_DB_FILE_PATH, albumPath + "/" + name);
    values.PutString(Media::MEDIA_DATA_DB_MIME_TYPE, GetMimeType(name));
    values.PutInt(Media::MEDIA_DATA_DB_MEDIA_TYPE, GetMediaType(name));

    Uri createAsset(Media::MEDIALIBRARY_DATA_URI + "/" + Media::MEDIA_FILEOPRN + "/" +
        Media::MEDIA_FILEOPRN_CREATEASSET);
    Media::MediaLibraryDataAbility mediaLibDb;
    mediaLibDb.InitMediaLibraryRdbStore();
    int index = mediaLibDb.Insert(createAsset, values);
    if (index < 0) {
        ERR_LOG("Fail to create fail %{public}s %{public}s", name.c_str(), path.c_str());
        return E_CREATE_FAIL;
    }
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
            int ret = result->GetColumnIndex(i.first, columnIndex);
            if (ret != NativeRdb::E_OK) {
                ERR_LOG("NativeRdb returns fail");
                return false;
            }
            mediaTableMap.emplace_back(columnIndex, i.second);
        }
    }
    return true;
}

bool MediaFileUtils::PushFileInfo(shared_ptr<NativeRdb::AbsSharedResultSet> result, MessageParcel &reply)
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
    unique_ptr<FileInfo> file = make_unique<FileInfo>();
    string path = uri + "/" + id;
    file->SetPath(path);
    string type;
    result->GetString(mediaTableMap[index++].first, type);
    file->SetType(type);
    string name;
    result->GetString(mediaTableMap[index++].first, name);
    file->SetName(name);
    int64_t value;
    result->GetLong(mediaTableMap[index++].first, value);
    file->SetSize(value);
    result->GetLong(mediaTableMap[index++].first, value);
    file->SetAddedTime(value);
    result->GetLong(mediaTableMap[index++].first, value);
    file->SetModifiedTime(value);
    reply.WriteParcelable(file.get());
    return true;
}

bool MediaFileUtils::PopFileInfo(FileInfo &file, MessageParcel &reply)
{
    string path;
    string name;
    string type;
    int64_t size = 0;
    int64_t at = 0;
    int64_t mt = 0;

    reply.ReadString(path);
    reply.ReadString(type);
    reply.ReadString(name);
    reply.ReadInt64(size);
    reply.ReadInt64(at);
    reply.ReadInt64(mt);
    file = FileInfo(name, path, type);
    file.SetSize(size);
    file.SetAddedTime(at);
    file.SetModifiedTime(mt);
    return true;
}

int MediaFileUtils::GetFileInfoFromResult(shared_ptr<NativeRdb::AbsSharedResultSet> result,
    MessageParcel &reply, int res)
{
    int count = 0;
    if (res) {
        // deal with status isn't succ;
        ERR_LOG("AbsSharedResultSet status isn't succ");
        reply.WriteInt32(count);
        return res;
    }
    result->GetRowCount(count);
    reply.WriteInt32(count);
    if (count == 0) {
        ERR_LOG("AbsSharedResultSet null");
        return E_EMPTYFOLDER;
    }
    result->GoToFirstRow();
    for (int i = 0; i < count; i++) {
        PushFileInfo(result, reply);
        result->GoToNextRow();
    }
    return SUCCESS;
}
} // namespace FileManagerService
} // namespace OHOS