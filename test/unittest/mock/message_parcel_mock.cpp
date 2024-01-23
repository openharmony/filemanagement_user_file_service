/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "assistant.h"
#include "message_parcel.h"
#include "hilog_wrapper.h"

namespace OHOS {
using namespace OHOS::FileAccessFwk;

Parcelable::Parcelable() : Parcelable(false)
{}

Parcelable::Parcelable(bool asRemote)
{
    asRemote_ = asRemote;
    behavior_ = 0;
}

bool MessageParcel::WriteInterfaceToken(std::u16string name)
{
    return Assistant::ins_->Bool();
}

bool Parcel::WriteParcelable(const Parcelable *object)
{
    return Assistant::ins_->Bool();
}

bool Parcel::WriteInt32(int32_t value)
{
    return Assistant::ins_->Bool();
}

int32_t Parcel::ReadInt32()
{
    return Assistant::ins_->Bool();
}

bool Parcel::ReadInt32(int32_t &value)
{
    value = Assistant::ins_->Int();
    return Assistant::ins_->Bool();
}

bool Parcel::WriteRemoteObject(const Parcelable *object)
{
    return Assistant::ins_->Bool();
}

bool MessageParcel::WriteRemoteObject(const sptr<IRemoteObject> &object)
{
    return Assistant::ins_->Bool();
}

bool Parcel::WriteBool(bool value)
{
    return Assistant::ins_->Bool();
}
} // namespace OHOS