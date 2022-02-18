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
#ifndef STORAGE_SERVICES_MEDIA_FILE_OPER_H
#define STORAGE_SERVICES_MEDIA_FILE_OPER_H

#include <string>
#include "file_oper.h"
namespace OHOS {
namespace FileManagerService {
class MediaFileOper : public FileOper {
public:
    MediaFileOper() = default;
    virtual ~MediaFileOper() = default;
    int OperProcess(uint32_t code, MessageParcel &data, MessageParcel &reply) const override;
private:
    int CreateFile(const std::string &name, const std::string &path, MessageParcel &reply) const;
    int ListFile(const std::string &type, const std::string &path, int offset, int count, MessageParcel &data) const;
    int Mkdir(const std::string &name, const std::string &path) const;
};
} // namespace FileManagerService
} // namespace OHOS
#endif // STORAGE_SERVICES_MEDIA_FILE_OPER_H