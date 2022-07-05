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
#ifndef STORAGE_FILE_MANAGER_SERVICE_DEF_H
#define STORAGE_FILE_MANAGER_SERVICE_DEF_H

#include <vector>
#include <string>
#include <unordered_map>
#include "media_data_ability_const.h"
#include "media_lib_service_const.h"

namespace OHOS {
namespace FileManagerService {
enum Operation {
    GET_ROOT,
    MAKE_DIR,
    LIST_FILE,
    CREATE_FILE
};

enum Equipment {
    INTERNAL_STORAGE,
    EXTERNAL_STORAGE
};

enum VolumeState {
    UNMOUNTED = 0,
    CHECKING,
    MOUNTED,
    EJECTING
};
constexpr int64_t MAX_NUM = 200;
constexpr int32_t CODE_MASK = 0xff;
constexpr int32_t EQUIPMENT_SHIFT = 16;

const std::string FISRT_LEVEL_ALBUM = "dataability:///album";
// use to find out album in the root dir
const std::string RELATIVE_ROOT_PATH = "";

const std::string MEDIA_ROOT_PATH = "/storage/media/local/files";
const std::string IMAGE_ROOT_NAME = "image_album";
const std::string VIDEO_ROOT_NAME = "video_album";
const std::string AUDIO_ROOT_NAME = "audio_album";
const std::string FILE_ROOT_NAME = "file_folder";

const std::string ALBUM_TYPE = "album";
const std::string FILE_MIME_TYPE = "file/*";

const std::string EXTERNAL_STORAGE_URI = "dataability:///external_storage";
const std::string MOUNT_POINT_ROOT = "/mnt/";

constexpr int FILE_MEDIA_TYPE = Media::MediaType::MEDIA_TYPE_FILE;
constexpr int RESULTSET_EMPTY = 0;
constexpr int RESULTSET_ONE = 1;

const std::unordered_map<int, std::string> FILE_MIME_TYPE_MAPS = {
    {Media::MediaType::MEDIA_TYPE_IMAGE, "image/*"},
    {Media::MediaType::MEDIA_TYPE_AUDIO, "audio/*"},
    {Media::MediaType::MEDIA_TYPE_VIDEO, "video/*"},
    {Media::MediaType::MEDIA_TYPE_FILE,  "file/*"},
    {Media::MediaType::MEDIA_TYPE_ALBUM, "file/*"},
};

const std::unordered_map<int, std::string> MEDIA_TYPE_FOLDER_MAPS = {
    {Media::MediaType::MEDIA_TYPE_IMAGE, "Pictures/"},
    {Media::MediaType::MEDIA_TYPE_AUDIO, "Audios/"},
    {Media::MediaType::MEDIA_TYPE_VIDEO, "Videos/"},
    {Media::MediaType::MEDIA_TYPE_FILE,  "Documents/"},
};

const std::unordered_map<int, std::string> MEDIA_TYPE_URI_MAPS = {
    {Media::MediaType::MEDIA_TYPE_IMAGE, Media::MEDIALIBRARY_IMAGE_URI},
    {Media::MediaType::MEDIA_TYPE_AUDIO, Media::MEDIALIBRARY_AUDIO_URI},
    {Media::MediaType::MEDIA_TYPE_VIDEO, Media::MEDIALIBRARY_VIDEO_URI},
    {Media::MediaType::MEDIA_TYPE_FILE,  Media::MEDIALIBRARY_FILE_URI},
};

#define GET_COLUMN_INDEX_FROM_NAME(result, name, index)                                \
    do {                                                                               \
        if (result->GetColumnIndex(name, index) != NativeRdb::E_OK) {                  \
            ERR_LOG("NativeRdb gets %{public}s index fail", name.c_str());             \
            return false;                                                              \
        }                                                                              \
    } while (0)
} // namespace FileManagerService
} // namespace OHOS
#endif // STORAGE_FILE_MANAGER_SERVICE_DEF_H
