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

#include "oper_factory.h"

#include "file_manager_service_def.h"
#include "file_oper.h"
#include "log.h"
#include "media_file_oper.h"

using namespace std;
namespace OHOS {
namespace FileManagerService {
unique_ptr<FileOper> OperFactory::GetFileOper(int equipmentId)
{
    unique_ptr<FileOper> fp;
    DEBUG_LOG("FileOper %{public}d.", equipmentId);
    switch (equipmentId) {
        case Equipment::INTERNAL_STORAGE: {
            fp = make_unique<MediaFileOper>();
            break;
        }
        default: {
            DEBUG_LOG("default FileOper %{public}d.", equipmentId);
            break;
        }
    }
    return fp;
}
} // namespace FileManagerService
} // namespace OHOS