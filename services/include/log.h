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
#ifndef STORAGE_SERIVCE_INCLUDE_LOG_H
#define STORAGE_SERIVCE_INCLUDE_LOG_H

#include <stdio.h>
#include "hilog/log.h"

#undef LOG_DOMAIN
#undef LOG_TAG
#define LOG_DOMAIN 0xD00430A
#define LOG_TAG "FileManagerment:FMS"

#define DEBUG_LOG(fmt, args...) \
    HILOG_DEBUG(LOG_CORE, "{%{public}s():%{public}d} " fmt, __FUNCTION__, __LINE__, ##args)
#define ERR_LOG(fmt, args...)  \
    HILOG_ERROR(LOG_CORE, "{%{public}s():%{public}d} " fmt, __FUNCTION__, __LINE__, ##args)
#define WARNING_LOG(fmt, args...)  \
    HILOG_WARN(LOG_CORE, "{%{public}s():%{public}d} " fmt, __FUNCTION__, __LINE__, ##args)
#define INFO_LOG(fmt, args...)  \
    HILOG_INFO(LOG_CORE, "{%{public}s():%{public}d} " fmt, __FUNCTION__, __LINE__, ##args)
#define FATAL_LOG(fmt, args...)  \
    HILOG_FATAL(LOG_CORE, "{%{public}s():%{public}d} " fmt, __FUNCTION__, __LINE__, ##args)
#endif // STORAGE_SERIVCE_INCLUDE_LOG_H
