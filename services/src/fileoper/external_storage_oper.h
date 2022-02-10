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
#ifndef STORAGE_SERIVCES_EXTERNAL_STORAGE_OPER_H
#define STORAGE_SERIVCES_EXTERNAL_STORAGE_OPER_H

#include <string>
#include "cmd_options.h"
#include "file_oper.h"
namespace OHOS {
namespace FileManagerService {
class ExternalStorageOper : public FileOper {
public:
    ExternalStorageOper() = default;
    virtual ~ExternalStorageOper() = default;
    int OperProcess(uint32_t code, MessageParcel &data, MessageParcel &reply) const override;
private:
    int CreateFile(const std::string &uri, const std::string &name, MessageParcel &reply) const;
    int ListFile(const std::string &type, const std::string &uri, const CmdOptions &option,
        MessageParcel &reply) const;
    int GetRoot(const std::string &name, const std::string &path, MessageParcel &reply) const;
};
} // namespace FileManagerService
} // namespace OHOS
#endif // STORAGE_SERIVCES_EXTERNAL_STORAGE_OPER_H