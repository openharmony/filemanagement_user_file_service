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

#include "uri_ext.h"
#include <iostream>
#include <vector>
#include <regex>
#include "hilog/log.h"
#include "string_ex.h"

using std::string;
using std::regex;
using OHOS::HiviewDFX::HiLog;

namespace OHOS {
namespace FileAccessFwk {
// 默认构造函数实现
Urie::Urie() : OHOS::Uri("")
{
    cachedSsi_ = NOT_FOUND;
    cachedFsi_ = NOT_FOUND;
    port_ = NOT_CALCULATED;
}

// 接受字符串参数的构造函数实现
Urie::Urie(const std::string& uriString) : OHOS::Uri(uriString)
{
    cachedSsi_ = NOT_FOUND;
    cachedFsi_ = NOT_FOUND;
    port_ = NOT_CALCULATED;

    if (uriString.empty()) {
        return;
    }

    uriString_ = uriString;
    scheme_ = NOT_CACHED;
    ssp_ = NOT_CACHED;
    authority_ = NOT_CACHED;
    host_ = NOT_CACHED;
    userInfo_ = NOT_CACHED;
    query_ = NOT_CACHED;
    path_ = NOT_CACHED;
    fragment_ = NOT_CACHED;

    if (!CheckScheme()) {
        uriString_ = EMPTY;
        std::cerr << "URI Scheme wrong" << std::endl;  // 简单的错误输出，可替换为合适的日志记录方式
    }
}

// 析构函数实现
Urie::~Urie() {
}

bool Urie::CheckScheme()
{
    scheme_ = ParseScheme();
    if (scheme_.empty()) {
        return true;
    }
    regex schemeRegex("[a-zA-Z][a-zA-Z|\\d|\\+|\\-|.]*$");
    if (!regex_match(scheme_, schemeRegex)) {
        return false;
    }
    return true;
}

string Urie::GetScheme()
{
    if (uriString_.empty()) {
        return EMPTY;
    }

    if (scheme_ == NOT_CACHED) {
        scheme_ = ParseScheme();
    }
    return scheme_;
}

string Urie::ParseScheme()
{
    size_t ssi = FindSchemeSeparator();
    return (ssi == NOT_FOUND) ? EMPTY : uriString_.substr(0, ssi);
}

string Urie::GetSchemeSpecificPart()
{
    if (uriString_.empty()) {
        return EMPTY;
    }

    return (ssp_ == NOT_CACHED) ? (ssp_ = ParseSsp()) : ssp_;
}

string Urie::ParseSsp()
{
    size_t ssi = FindSchemeSeparator();
    size_t fsi = FindFragmentSeparator();

    size_t start = (ssi == NOT_FOUND) ? 0 : (ssi + 1);
    size_t end = (fsi == NOT_FOUND) ? uriString_.size() : fsi;

    // Return everything between ssi and fsi.
    string ssp = EMPTY;
    if (end > start) {
        ssp = uriString_.substr(start, end - start);
    }

    return ssp;
}

string Urie::GetAuthority()
{
    if (uriString_.empty()) {
        return EMPTY;
    }

    if (authority_ == NOT_CACHED) {
        authority_ = ParseAuthority();
    }
    return authority_;
}

string Urie::ParseAuthority()
{
    size_t ssi = FindSchemeSeparator();
    if (ssi == NOT_FOUND) {
        return EMPTY;
    }

    size_t length = uriString_.length();
    // If "//" follows the scheme separator, we have an authority.
    if ((length > (ssi + POS_INC_MORE)) && (uriString_.at(ssi + POS_INC) == LEFT_SEPARATOR) &&
        (uriString_.at(ssi + POS_INC_MORE) == LEFT_SEPARATOR)) {
        // Look for the start of the path, query, or fragment, or the end of the string.
        size_t start = ssi + POS_INC_AGAIN;
        size_t end = start;

        while (end < length) {
            char ch = uriString_.at(end);
            if ((ch == LEFT_SEPARATOR) || (ch == RIGHT_SEPARATOR) || (ch == QUERY_FLAG) ||
                (ch == SCHEME_FRAGMENT)) {
                break;
            }

            end++;
        }

        return uriString_.substr(start, end - start);
    } else {
        return EMPTY;
    }
}

string Urie::GetUserInfo()
{
    if (uriString_.empty()) {
        return EMPTY;
    }

    if (userInfo_ == NOT_CACHED) {
        userInfo_ = ParseUserInfo();
    }
    return userInfo_;
}

string Urie::ParseUserInfo()
{
    string authority = GetAuthority();
    if (authority.empty()) {
        return EMPTY;
    }

    size_t end = authority.find_last_of(USER_HOST_SEPARATOR);
    return (end == NOT_FOUND) ? EMPTY : authority.substr(0, end);
}

string Urie::GetHost()
{
    if (uriString_.empty()) {
        return EMPTY;
    }

    if (host_ == NOT_CACHED) {
        host_ = ParseHost();
    }
    return host_;
}

string Urie::ParseHost()
{
    string authority = GetAuthority();
    if (authority.empty()) {
        return EMPTY;
    }

    // Parse out user info and then port.
    size_t userInfoSeparator = authority.find_last_of(USER_HOST_SEPARATOR);
    size_t start = (userInfoSeparator == NOT_FOUND) ? 0 : (userInfoSeparator + 1);
    size_t portSeparator = authority.find_first_of(PORT_SEPARATOR, start);
    size_t end = (portSeparator == NOT_FOUND) ? authority.size() : portSeparator;

    string host = EMPTY;
    if (start < end) {
        host = authority.substr(start, end - start);
    }

    return host;
}

int Urie::GetPort()
{
    if (uriString_.empty()) {
        return PORT_NONE;
    }

    if (port_ == NOT_CALCULATED) {
        port_ = ParsePort();
    }
    return port_;
}

int Urie::ParsePort()
{
    string authority = GetAuthority();
    if (authority.empty()) {
        return PORT_NONE;
    }

    // Make sure we look for the port separtor *after* the user info separator.
    size_t userInfoSeparator = authority.find_last_of(USER_HOST_SEPARATOR);
    size_t start = (userInfoSeparator == NOT_FOUND) ? 0 : (userInfoSeparator + 1);
    size_t portSeparator = authority.find_first_of(PORT_SEPARATOR, start);
    if (portSeparator == NOT_FOUND) {
        return PORT_NONE;
    }

    start = portSeparator + 1;
    string portString = authority.substr(start);

    int value = PORT_NONE;
    return StrToInt(portString, value) ? value : PORT_NONE;
}

string Urie::GetQuery()
{
    if (uriString_.empty()) {
        return EMPTY;
    }

    if (query_ == NOT_CACHED) {
        query_ = ParseQuery();
    }
    return query_;
}

string Urie::ParseQuery()
{
    size_t ssi = FindSchemeSeparator();
    if (ssi == NOT_FOUND) {
        ssi = 0;
    }
    size_t qsi = uriString_.find_first_of(QUERY_FLAG, ssi);
    if (qsi == NOT_FOUND) {
        return EMPTY;
    }

    size_t start = qsi + 1;
    size_t fsi = FindFragmentSeparator();
    if (fsi == NOT_FOUND) {
        return uriString_.substr(start);
    }

    if (fsi < qsi) {
        // Invalid.
        return EMPTY;
    }

    return uriString_.substr(start, fsi - start);
}

string Urie::GetPath()
{
    if (uriString_.empty()) {
        return EMPTY;
    }

    if (path_ == NOT_CACHED) {
        path_ = ParsePath();
    }
    return path_;
}

void Urie::GetPathSegments(std::vector<std::string>& segments)
{
    if (uriString_.empty()) {
        return;
    }
    if (path_ == NOT_CACHED) {
        path_ = ParsePath();
    }

    size_t previous = 0;
    size_t current;
    while ((current = path_.find(LEFT_SEPARATOR, previous)) != std::string::npos) {
        if (previous < current) {
            segments.emplace_back(path_.substr(previous, current - previous));
        }
        previous = current + POS_INC;
    }
    // Add in the final path segment.
    if (previous < path_.length()) {
        segments.emplace_back(path_.substr(previous));
    }
}

string Urie::ParsePath()
{
    size_t ssi = FindSchemeSeparator();
    // If the URI is absolute.
    if (ssi != NOT_FOUND) {
        // Is there anything after the ':'?
        if ((ssi + 1) == uriString_.length()) {
            // Opaque URI.
            return EMPTY;
        }

        // A '/' after the ':' means this is hierarchical.
        if (uriString_.at(ssi + 1) != LEFT_SEPARATOR) {
            // Opaque URI.
            return EMPTY;
        }
    } else {
        // All relative URIs are hierarchical.
    }

    return ParsePath(ssi);
}

string Urie::ParsePath(size_t ssi)
{
    size_t length = uriString_.length();

    // Find start of path.
    size_t pathStart = (ssi == NOT_FOUND) ? 0 : (ssi + POS_INC);
    if ((length > (pathStart + POS_INC)) && (uriString_.at(pathStart) == LEFT_SEPARATOR) &&
        (uriString_.at(pathStart + POS_INC) == LEFT_SEPARATOR)) {
        // Skip over authority to path.
        pathStart += POS_INC_MORE;

        while (pathStart < length) {
            char ch = uriString_.at(pathStart);
            if ((ch == QUERY_FLAG) || (ch == SCHEME_FRAGMENT)) {
                return EMPTY;
            }

            if ((ch == LEFT_SEPARATOR) || (ch == RIGHT_SEPARATOR)) {
                break;
            }

            pathStart++;
        }
    }

    // Find end of path.
    size_t pathEnd = pathStart;
    while (pathEnd < length) {
        char ch = uriString_.at(pathEnd);
        if ((ch == QUERY_FLAG) || (ch == SCHEME_FRAGMENT)) {
            break;
        }

        pathEnd++;
    }

    return uriString_.substr(pathStart, pathEnd - pathStart);
}

string Urie::GetFragment()
{
    if (uriString_.empty()) {
        return EMPTY;
    }

    if (fragment_ == NOT_CACHED) {
        fragment_ = ParseFragment();
    }
    return fragment_;
}

string Urie::ParseFragment()
{
    size_t fsi = FindFragmentSeparator();
    return (fsi == NOT_FOUND) ? EMPTY : uriString_.substr(fsi + 1);
}

size_t Urie::FindSchemeSeparator()
{
    if (cachedSsi_ == NOT_FOUND) {
        cachedSsi_ = uriString_.find_first_of(SCHEME_SEPARATOR);
    }
    return cachedSsi_;
}

size_t Urie::FindFragmentSeparator()
{
    if (cachedFsi_ == NOT_FOUND) {
        cachedFsi_ = uriString_.find_first_of(SCHEME_FRAGMENT, FindSchemeSeparator());
    }
    return cachedFsi_;
}

bool Urie::IsHierarchical()
{
    if (uriString_.empty()) {
        return false;
    }

    size_t ssi = FindSchemeSeparator();
    if (ssi == NOT_FOUND) {
        // All relative URIs are hierarchical.
        return true;
    }

    if (uriString_.length() == (ssi + 1)) {
        // No ssp.
        return false;
    }

    // If the ssp starts with a '/', this is hierarchical.
    return (uriString_.at(ssi + 1) == LEFT_SEPARATOR);
}

bool Urie::IsAbsolute()
{
    if (uriString_.empty()) {
        return false;
    }

    return !IsRelative();
}

bool Urie::IsRelative()
{
    if (uriString_.empty()) {
        return false;
    }

    // Note: We return true if the index is 0
    return FindSchemeSeparator() == NOT_FOUND;
}

bool Urie::Equals(const Urie& other) const
{
    return uriString_ == other.ToString();
}

int Urie::CompareTo(const Urie& other) const
{
    return uriString_.compare(other.ToString());
}

string Urie::ToString() const
{
    return uriString_;
}

bool Urie::operator==(const Urie& other) const
{
    return uriString_ == other.ToString();
}

bool Urie::Marshalling(Parcel& parcel) const
{
    if (IsAsciiString(uriString_)) {
        return parcel.WriteString16(Str8ToStr16(uriString_));
    }

    HILOG_IMPL(LOG_CORE, LOG_ERROR, 0xD001305,  "URIe", "uriString_ is not ascii string");
    return false;
}

Urie* Urie::Unmarshalling(Parcel& parcel)
{
    return new Urie(Str16ToStr8(parcel.ReadString16()));
}
}
}