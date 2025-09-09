/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#ifndef FILE_ACCESS_FRAMEWORK_ERRNO_H
#define FILE_ACCESS_FRAMEWORK_ERRNO_H

#include "errors.h"

namespace OHOS {
namespace FileAccessFwk {

const int ERR_EXIST = 13900015;
const int ERR_NOMEM = 13900011;
const int ERR_PERM = 13900001;
const int ERR_URI = 13900002;
const int ERR_NOT_DIR = 13900018;
const int ERR_IS_DIR = 13900019;
const int BASE_OFFSET = 14300001;
const int CLOUD_DISK_BASE_OFFSET = 34400001;
enum {
    ERR_OK = 0,
    E_IPCS = BASE_OFFSET,                   // IPC error
    E_URIS,                                 // Invalid uri
    E_GETINFO,                              // Fail to get fileextension info
    E_GETRESULT,                            // Get wrong result
    E_REGISTER,                             // Fail to register notification
    E_REMOVE,                               // Fail to remove notification
    E_INIT_NOTIFY_AGENT,                    // Fail to init notification agent
    E_NOTIFY,                               // Fail to notify agent
    E_CONNECT,                              // Fail to connect file access extension ability
    E_COUNT,                                // Too many records
    E_CALLBACK_AND_URI_HAS_NOT_RELATIONS,   // Uri and callback do not has relations, can not unregister
    E_CALLBACK_IS_NOT_REGISTER,             // CallBack is not registered, can not unregister
    E_CAN_NOT_FIND_URI,                     // Can not find registered uri
    E_DO_NOT_HAVE_PARENT,                   // Do not have parent uri in observerNode
    E_LOAD_SA,                              // load SA failed
    E_PERMISSION = 201,                     // Permission verification failed
    E_PERMISSION_SYS,                       // is not system app

    E_NOT_SUPPORT = 801,
    E_SERVICE_DIED = 29189,

    // cloud disk errcode
    E_INVALID_PARAM = CLOUD_DISK_BASE_OFFSET,       // Invalid parameter
    E_PERSIST_SYNC_FOLDER_FAILED,                   // Failed to persist Sync folder
    E_IPC_FAILED,                                   // IPC error
    E_SYNC_FOLDER_LIMIT_EXCEEDED,                   // Sync folder limit exceeded
    E_SYNC_FOLDER_CONFLICT_SELF,                    // Conflict with own app's Sync folder
    E_SYNC_FOLDER_CONFLICT_OTHER,                   // Conflict with other app's Sync folder
    E_CREATE_SYNC_FOLDER_FAILED,                    // Failed to create Sync folder
    E_SYNC_FOLDER_NOT_REGISTERED,                   // Sync folder not registered
    E_REMOVE_SYNC_FOLDER_FAILED,                    // Failed to remove Sync folder
    E_UPDATE_SYNC_FOLDER_FAILED,                    // Failed to update Sync folder properties
    E_SYNC_FOLDER_NOT_EXIST,                        // Sync folder not exist
    E_INTERNAL_ERROR = CLOUD_DISK_BASE_OFFSET + 15, // Internal error 15: offset
    E_SYSTEM_RESTRICTED,                            // System restricted
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_FRAMEWORK_ERRNO_H