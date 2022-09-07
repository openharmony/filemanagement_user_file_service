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

#include "napi_utils.h"

#include <cinttypes>
#include <string>
#include <vector>

#include "file_access_extension_info.h"
#include "file_access_framework_errno.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace FileAccessFwk {
int IsDirectory(const int64_t mode)
{
    if ((mode & DOCUMENT_FLAG_REPRESENTS_DIR) != DOCUMENT_FLAG_REPRESENTS_DIR) {
        HILOG_ERROR("current FileInfo is not dir");
        return ERR_INVALID_PARAM;
    }

    if ((mode & DOCUMENT_FLAG_REPRESENTS_FILE) == DOCUMENT_FLAG_REPRESENTS_FILE) {
        HILOG_ERROR("file mode(%{public}" PRId64 ") is incorrect", mode);
        return ERR_INVALID_PARAM;
    }

    return ERR_OK;
}

int GetFileFilterParam(const NVal &argv, FileFilter &filter)
{
    bool ret = false;
    filter.SetHasFilter(false);
    if (argv.HasProp("suffix")) {
        std::vector<std::string> filter_suffix;
        std::tie(ret, filter_suffix, std::ignore) = argv.GetProp("suffix").ToStringArray();
        if (!ret) {
            HILOG_ERROR("FileFilter get suffix param fail.");
            return ERR_INVALID_PARAM;
        }

        filter.SetSuffix(filter_suffix);
        filter.SetHasFilter(true);
    } else {
        return ERR_INVALID_PARAM;
    }

    if (argv.HasProp("display_name")) {
        std::vector<std::string> display_name;
        std::tie(ret, display_name, std::ignore) = argv.GetProp("display_name").ToStringArray();
        if (!ret) {
            HILOG_ERROR("FileFilter get display_name param fail.");
            return ERR_INVALID_PARAM;
        }
        filter.SetDisplayName(display_name);
        filter.SetHasFilter(true);

    }

    if (argv.HasProp("mime_type")) {
        std::vector<std::string> mime_type;
        std::tie(ret, mime_type, std::ignore) = argv.GetProp("mime_type").ToStringArray();
        if (!ret) {
            HILOG_ERROR("FileFilter get mime_type param fail.");
            return ERR_INVALID_PARAM;
        }

        filter.SetMimeType(mime_type);
        filter.SetHasFilter(true);
    }

    if (argv.HasProp("file_size_over")) {
        int64_t file_size_over;
        std::tie(ret, file_size_over) = argv.GetProp("file_size_over").ToInt64();
        if (!ret) {
            HILOG_ERROR("FileFilter get file_size_over param fail.");
            return ERR_INVALID_PARAM;
        }

        filter.SetFileSizeOver(file_size_over);
        filter.SetHasFilter(true);
    }

    if (argv.HasProp("last_modified_after")) {
        double last_modified_after;
        std::tie(ret, last_modified_after) = argv.GetProp("last_modified_after").ToDouble();
        if (!ret) {
            HILOG_ERROR("FileFilter get last_modified_after param fail.");
            return ERR_INVALID_PARAM;
        }

        filter.SetLastModifiedAfter(last_modified_after);
        filter.SetHasFilter(true);
    }

    if (argv.HasProp("exclude_media")) {
        bool exclude_media;
        std::tie(ret, exclude_media) = argv.GetProp("exclude_media").ToBool();
        if (!ret) {
            HILOG_ERROR("FileFilter get exclude_media param fail.");
            return ERR_INVALID_PARAM;
        }

        filter.SetExcludeMedia(exclude_media);
        filter.SetHasFilter(true);
    }

    if (!filter.GetHasFilter()) {
        HILOG_ERROR("FileFilter must have one property.");
        return ERR_INVALID_PARAM;
    }
    return ERR_OK;
}
} // namespace FileAccessFwk
} // namespace OHOS