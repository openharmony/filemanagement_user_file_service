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
#ifndef STORAGE_SERVICES_INCLUDE_ERRNO_H
#define STORAGE_SERVICES_INCLUDE_ERRNO_H
namespace OHOS {
namespace FileManagerService {
constexpr int32_t SUCCESS = 0;
constexpr int32_t FAIL = -1;                  // internal Error
constexpr int32_t E_NOEXIST = -2;             // file not exist
constexpr int32_t E_EMPTYFOLDER = -3;         // folder empty
constexpr int32_t E_INVALID_OPERCODE = -4;    // not valid oper code
constexpr int32_t E_CREATE_FAIL = -5;         // create file fail
} // namespace FileManagerService
} // namespace OHOS
#endif // STORAGE_SERVICES_INCLUDE_ERRNO_H
