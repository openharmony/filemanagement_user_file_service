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
#include "file_info.h"
#include "log.h"
using namespace std;

namespace OHOS {
namespace FileManagerService  {
bool FileInfo::Marshalling(Parcel &parcel) const
{
    parcel.WriteString(path_);
    parcel.WriteString(name_);
    parcel.WriteString(type_);
    parcel.WriteInt64(size_);
    parcel.WriteInt64(addedTime_);
    parcel.WriteInt64(modifiedTime_);
    return true;
}
FileInfo* FileInfo::Unmarshalling(Parcel &parcel)
{
    auto *obj = new (std::nothrow) FileInfo();
    if (obj == nullptr) {
        ERR_LOG("Unmarshalling fail");
    }
    obj->path_ = parcel.ReadString();
    obj->name_ = parcel.ReadString();
    obj->type_ = parcel.ReadString();
    obj->size_ = parcel.ReadInt64();
    obj->addedTime_ = parcel.ReadInt64();
    obj->modifiedTime_ = parcel.ReadInt64();
    return obj;
}
} // namespace FileManagerService
} // namespace OHOS