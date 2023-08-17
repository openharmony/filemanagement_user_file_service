/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FILE_ACCESS_SERVICE_H
#define FILE_ACCESS_SERVICE_H

#include <atomic>
#include <memory>
#include <mutex>
#include <system_ability.h>
#include <unordered_map>
#include <vector>

#include "file_access_service_stub.h"
#include "holder_manager.h"
#include "iremote_object.h"
#include "uri.h"

namespace OHOS {
namespace FileAccessFwk {

class ObserverContext {
    public:
        ObserverContext(sptr<IFileAccessObserver> obs): obs_(obs) {}
        virtual ~ObserverContext() = default;
        sptr<IFileAccessObserver> obs_ = nullptr;
        void Ref()
        {
            if (ref_ == std::numeric_limits<int32_t>::max()) {
                HILOG_ERROR("Ref is over max");
                return;
            }
            ref_++;
        }

        void UnRef()
        {
            if (ref_ == 0) {
                HILOG_ERROR("Ref is already zero");
                return;
            }
            ref_--;
        }

        bool IsValid()
        {
            return ref_ > 0;
        }

        bool EqualTo(std::shared_ptr<ObserverContext> observerContext)
        {
            return obs_->AsObject() == observerContext->obs_->AsObject();
        }

    private:
        std::atomic<int32_t> ref_;
};

class ObserverNode {
    public:
        ObserverNode(const bool needChildNote): needChildNote_(needChildNote) {}
        virtual ~ObserverNode() = default;
        std::shared_ptr<ObserverNode> parent_;
        std::vector<std::shared_ptr<ObserverNode>> children_;
        std::vector<uint32_t> obsCodeList_;
        bool needChildNote_;
};

class FileAccessService final : public SystemAbility, public FileAccessServiceStub {
    DECLARE_SYSTEM_ABILITY(FileAccessService)

public:
    static sptr<FileAccessService> GetInstance();
    virtual ~FileAccessService() = default;

    virtual void OnStart() override;
    virtual void OnStop() override;
    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;

public:
    int32_t RegisterNotify(Uri uri, bool notifyForDescendants, const sptr<IFileAccessObserver> &observer) override;
    int32_t UnregisterNotify(Uri uri, const sptr<IFileAccessObserver> &observer) override;
    int32_t CleanAllNotify(Uri uri) override;
    int32_t OnChange(Uri uri, NotifyType notifyType) override;

private:
    void SendListNotify(const std::vector<uint32_t> list, NotifyMessage &notifyMessage);
    void RemoveRelations(std::string &uriStr, std::shared_ptr<ObserverNode> obsNode);
    int FindUri(const std::string &uriStr, std::shared_ptr<ObserverNode> &outObsNode);
    FileAccessService();
    bool IsServiceReady() const;
    static sptr<FileAccessService> instance_;
    bool ready_ = false;
    static std::mutex mutex_;
    std::mutex nodeMutex_;
    std::unordered_map<std::string, std::shared_ptr<ObserverNode>> relationshipMap_;
    HolderManager<std::shared_ptr<ObserverContext>> obsManager_;
};
} // namespace FileAccessFwk
} // namespace OHOS
#endif // FILE_ACCESS_SERVICE_H