/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef FILE_ACCESS_URI_EXT_H
#define FILE_ACCESS_URI_EXT_H

#include <string>
#include "uri.h"

namespace OHOS {
namespace FileAccessFwk {
namespace {
    const std::string NOT_CACHED = "NOT VALID";
    const std::string EMPTY = "";
    const std::size_t NOT_FOUND = std::string::npos;
    const int NOT_CALCULATED = -2;
    const int PORT_NONE = -1;
    const char SCHEME_SEPARATOR = ':';
    const char SCHEME_FRAGMENT = '#';
    const char LEFT_SEPARATOR = '/';
    const char RIGHT_SEPARATOR = '\\';
    const char QUERY_FLAG = '?';
    const char USER_HOST_SEPARATOR = '@';
    const char PORT_SEPARATOR = ':';
    const std::size_t POS_INC = 1;
    const std::size_t POS_INC_MORE = 2;
    const std::size_t POS_INC_AGAIN = 3;
}  // namespace

class Urie : public OHOS::Uri {
public:
    Urie();
    Urie(const std::string& uriString);
    ~Urie();

    OHOS::Uri ConvertToUri(Urie &urie);
    std::string GetScheme();
    std::string GetSchemeSpecificPart();
    std::string GetAuthority();
    std::string GetHost();
    int GetPort();
    std::string GetUserInfo();
    std::string GetQuery();
    std::string GetPath();
    void GetPathSegments(std::vector<std::string>& segments);
    std::string GetFragment();
    bool IsHierarchical();
    bool IsAbsolute();
    bool IsRelative();
    bool Equals(const Urie& other) const;
    int CompareTo(const Urie& other) const;
    std::string ToString() const;
    bool operator==(const Urie& other) const;
    virtual bool Marshalling(Parcel& parcel) const override;
    static Urie* Unmarshalling(Parcel& parcel);

private:
    bool CheckScheme();
    std::string ParseScheme();
    std::string ParseSsp();
    std::string ParseAuthority();
    std::string ParseUserInfo();
    std::string ParseHost();
    int ParsePort();
    std::string ParsePath(std::size_t ssi);
    std::string ParsePath();
    std::string ParseQuery();
    std::string ParseFragment();
    std::size_t FindSchemeSeparator();
    std::size_t FindFragmentSeparator();

    std::string uriString_;
    std::string scheme_;
    std::string ssp_;
    std::string authority_;
    std::string host_;
    int port_;
    std::string userInfo_;
    std::string query_;
    std::string path_;
    std::string fragment_;
    std::size_t cachedSsi_;
    std::size_t cachedFsi_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif  // FILE_ACCESS_URI_EXT_H
