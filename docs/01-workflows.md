# user_file_service 工作流规则

本文记录 `user_file_service` 部件的关键工作流时序。每个工作流列出前置条件、严格步骤（带代码锚点）和违反后果。稳定背景知识见 `docs/knowledge/service-architecture.md`、`docs/knowledge/extension-ability.md`、`docs/knowledge/cloud-disk-sync.md`。

## 1. SA 启动与按需加载时序

**前置条件**：系统启动或收到 `COMMON_EVENT_PACKAGE_REMOVED` 事件（且系统参数 `persist.clouddiskmanager.workstatus` 为 `true`）。

| 步骤 | 操作 | 代码锚点 |
|------|------|----------|
| 1 | SA 按需触发启动 | `5010.json` `start-on-demand.commonevent` |
| 2 | `OnStart(reason)` 监听 `COMMON_EVENT_SERVICE_ID`，初始化 `BundleObserver` 并处理广播 | `FileAccessService::OnStart(const SystemAbilityOnDemandReason&)` `services/native/cloud_disk_service/src/cloud_disk_service.cpp:593` |
| 3 | 调用无参 `OnStart()` | `FileAccessService::OnStart()` `services/native/file_access_service/src/file_access_service.cpp:76` |
| 4 | `Init()` 创建 `ExtensionDeathRecipient`、`ObserverDeathRecipient`、`AppDeathRecipient` | `FileAccessService::Init()` `services/native/file_access_service/src/file_access_service.cpp:141` |
| 5 | `InitTimer()` 创建 `OnDemandTimer`（通知批量分发）和 `UnloadTimer`（空闲卸载） | `FileAccessService::InitTimer()` `services/native/file_access_service/src/file_access_service.cpp:691` |
| 6 | `Publish(this)` 注册到 `SystemAbilityManager` | `FileAccessService::OnStart()` `services/native/file_access_service/src/file_access_service.cpp:81` |
| 7 | `ready_ = true` | 同上 `:85` |
| 8 | `OnAddSystemAbility(COMMON_EVENT_SERVICE_ID)` → `RegisterBundleBroadcast()` | `FileAccessService::OnAddSystemAbility()` `services/native/cloud_disk_service/src/cloud_disk_service.cpp:619` |

**违反后果**：若跳过步骤 4/5 直接 Publish，服务已注册但 `DeathRecipient` 和 Timer 未初始化，首次扩展连接或观察者注册时空指针崩溃。若跳过步骤 8，`BundleObserver` 未注册广播，应用卸载后无法清理同步文件夹。

## 2. SA 空闲卸载时序

**前置条件**：`UnloadTimer` 已启动（`InitTimer()` 中 `unLoadTimer_->start()`），SA 处于空闲状态。

| 步骤 | 操作 | 代码锚点 |
|------|------|----------|
| 1 | `UnloadTimer` 每秒检查，计数达 30（`UNLOAD_SA_WAIT_TIME`）时触发回调 | `UnloadTimer::start()` `services/native/file_access_service/include/file_access_service.h:194` |
| 2 | 回调中调用 `IsUnused()`：检查 `obsManager_.isEmpty()` && `IsAppProxyMapEmpty()` && `!IsCalledCountValid()` | `FileAccessService::IsUnused()` `services/native/file_access_service/src/file_access_service.cpp:678` |
| 3 | 若 `IsUnused()` 返回 true，获取 `SystemAbilityManager` | `cloud_disk_service.cpp:726` |
| 4 | `UnRegisterBundleBroadcast()` 注销包广播 | `cloud_disk_service.cpp:732` |
| 5 | `saManager->UnloadSystemAbility(FILE_ACCESS_SERVICE_ID)` 卸载 SA | `cloud_disk_service.cpp:733` |

**违反后果**：若在 `calledCount_ > 0`（有同步文件夹操作进行中）时卸载 SA，会导致操作中断、RDB 与 dfs_service 状态不一致。若未注销广播就卸载，下次按需启动时 `BundleObserver` 可能重复注册。

## 3. 扩展能力连接与代理获取时序

**前置条件**：SA 已就绪（`ready_ == true`），调用方持有 `ohos.permission.FILE_ACCESS_MANAGER` 权限。

| 步骤 | 操作 | 代码锚点 |
|------|------|----------|
| 1 | `GetExtensionProxy(info, extensionProxy)` 检查权限 | `FileAccessService::GetExtensionProxy()` `services/native/file_access_service/src/file_access_service.cpp:784` |
| 2 | 创建 `FileAccessExtConnection`（继承 `AbilityConnectionStub`） | `:793` |
| 3 | `fileAccessExtConnection->ConnectFileExtAbility(want, token)` 异步连接扩展 | `:802` |
| 4 | `OnAbilityConnectDone()` 收到连接回调，创建 `FileAccessExtBaseProxy` | `FileAccessExtConnection::OnAbilityConnectDone()` `services/native/file_access_service/include/file_access_ext_connection.h:40` |
| 5 | `GetFileExtProxy()` 返回代理 | `file_access_service.cpp:803` |
| 6 | 调用方通过 `ConnectExtension()` 走缓存逻辑 | `FileAccessService::ConnectExtension()` `services/native/file_access_service/src/file_access_service.cpp:187` |
| 7 | `GetBundleNameFromUri(uri, bundleName)` 从 URI 解析 bundleName | `:190` |
| 8 | `FindExtProxyByBundleName(bundleName)` 查缓存 `cMap_` | `:191` |
| 9 | 未命中则调用 `GetExtensionProxy()`，获取后 `AddDeathRecipient(extensionDeathRecipient_)` | `:198-205` |
| 10 | `AddExtProxyInfo(bundleName, extensionProxy)` 存入缓存 | `:207` |

**违反后果**：若跳过步骤 8（缓存查询）直接连接，会导致每次文件操作都重新连接扩展能力，IPC 连接泄露。若跳过步骤 9（死亡监听），扩展能力异常退出时代理无法被清理，后续操作使用失效代理。

## 4. 文件操作调用链（FileAccessHelper → Extension）

**前置条件**：应用通过 `FileAccessHelper::Creator()` 已创建 helper 实例并连接到扩展能力。

| 步骤 | 操作 | 代码锚点 |
|------|------|----------|
| 1 | 应用调用 `FileAccessHelper::OpenFile(uri, flags, fd)` | `interfaces/inner_api/file_access/include/file_access_helper.h:73` |
| 2 | `GetProxyByUri(uri)` 通过 URI 获取对应扩展代理 | `file_access_helper.h:94` |
| 3 | 代理通过 IPC 调用 `IFileAccessExtBase::OpenFile()`（ipccode 1） | `interfaces/inner_api/file_access/IFileAccessExtBase.idl:23` |
| 4 | 扩展侧 `FileAccessExtStubImpl::OpenFile()` 接收 IPC 请求 | `interfaces/inner_api/file_access/include/file_access_ext_stub_impl.h:39` |
| 5 | `GetOwner()` 获取 `FileAccessExtAbility` 实例 | `file_access_ext_stub_impl.h:64` |
| 6 | `FileAccessExtAbility::OpenFile(uri, flags, fd)` 调用具体实现 | `interfaces/inner_api/file_access/include/file_access_ext_ability.h:44` |
| 7 | JS 扩展（如 ExternalFileManager）的 `openFile()` 方法执行实际文件操作 | `services/file_extension_hap/entry/src/main/ets/FileExtensionAbility/FileExtensionAbility.ts` |

**调用链全貌**：

```
应用 JS API (NAPI)
  → FileAccessHelper (C++ inner_api)
    → IFileAccessExtBase Proxy (IPC)
      → FileAccessExtStubImpl (stub)
        → FileAccessExtAbility (C++ 基类)
          → JS FileAccessExtensionAbility (应用层实现)
```

**违反后果**：若跳过 `FileAccessHelper` 直接操作代理，无法获取正确的扩展连接（helper 管理连接生命周期）。若修改 IDL 接口码但未同步 stub/proxy，IPC 编解码不匹配导致 `E_IPCS` 错误。

## 5. 观察者注册与通知分发时序

**前置条件**：SA 已就绪，扩展能力已连接，调用方持有 `FILE_ACCESS_MANAGER` 权限。

### 5.1 注册观察者

| 步骤 | 操作 | 代码锚点 |
|------|------|----------|
| 1 | `RegisterNotify(uri, notifyForDescendants, observer, info)` 权限校验 | `services/native/file_access_service/src/file_access_service.cpp:315` |
| 2 | `convertUris(uri, uris)` 将 `file://docs` 展开为各设备根 URI | `:302-313` |
| 3 | 遍历 uris，调用 `RegisterNotifyImpl()` | `:327-334` |
| 4 | `obsManager_.getId()` 查找 observer 是否已注册 | `:397-399` |
| 5 | 未注册则 `obsContext->Ref()` → `obsManager_.save(obsContext)` → `AddDeathRecipient(observerDeathRecipient_)` | `:402-408` |
| 6 | 已注册则 `object->Ref()` 增加引用计数 | `:411-416` |
| 7 | `OperateObsNode()` 在 `relationshipMap_` 中创建/更新 `ObserverNode` | `:337-387` |
| 8 | `ConnectExtension(uri, info)` 获取扩展代理 | `:364` |
| 9 | `extensionProxy->StartWatcher(uriConvert)` 通知扩展开始监听 | `:369` |
| 10 | 建立父子关系（`IsParentUri` / `IsChildUri`），插入 `relationshipMap_` | `:373-384` |

**违反后果**：若先调用 `StartWatcher` 再注册 observer 到 `obsManager_`，扩展发出通知时 `OnChange` 无法找到 observer 上下文，通知丢失。若跳过死亡监听（步骤 5），observer 进程退出后 `obsManager_` 中残留失效引用。

### 5.2 通知分发

| 步骤 | 操作 | 代码锚点 |
|------|------|----------|
| 1 | 扩展能力调用 `OnChange(uri, notifyType)` | `services/native/file_access_service/src/file_access_service.cpp:632` |
| 2 | 权限校验，解析 URI | `:634-647` |
| 3 | `FindUri(uriStr, node)` 在 `relationshipMap_` 中查找 | `:650` |
| 4 | 未找到则查父路径 `parentUri` | `:651-656` |
| 5 | 父节点 `needChildNote_` 为 true 时向父的观察者列表分发 | `:665` |
| 6 | `SendListNotify(uris, notifyType, node->obsCodeList_)` | `:669` |
| 7 | `onDemandTimer_->start()` 启动批量通知定时器 | `:597` |
| 8 | 遍历 obsCodeList，将 URI 攒入 `context->notifyMap_` | `:613-629` |
| 9 | 达到 `NOTIFY_MAX_NUM`（32）条时立即回调 `obs_->OnChange(notifyMessage)` | `:620-628` |
| 10 | 未达阈值则等 `OnDemandTimer`（500ms 间隔，最多 20 次）批量回调 | `InitTimer()` `:693-720` |

**违反后果**：若跳过批量机制直接通知，高频文件变更会产生大量 IPC 调用，影响性能。若 `OnDemandTimer` 未启动，通知积压在 `notifyMap_` 中无法发出。

### 5.3 注销观察者

| 步骤 | 操作 | 代码锚点 |
|------|------|----------|
| 1 | `UnregisterNotify(uri, observer, info)` 权限校验 | `file_access_service.cpp:504` |
| 2 | `UnregisterNotifyImpl()` 查找 URI 节点和 observer code | `:540-567` |
| 3 | `obsNode->FindAndRmObsCodeByCode(code)` 从节点移除 code | `:568` |
| 4 | `object->UnRef()` 减引用，引用为 0 则 `obsManager_.release(code)` | `:578-582` |
| 5 | 若节点仍有其他 observer（`CheckObsCodeListNotEmpty()`），不停止监听 | `:584` |
| 6 | 若节点无 observer，`RmUriObsNodeRelations()` → `extensionProxy->StopWatcher()` → `RemoveRelations()` | `:588` → `:811-828` |
| 7 | 若 SA 空闲，`unLoadTimer_->reset()` 重置卸载计时 | `:522` |

**违反后果**：若跳过步骤 5 直接 StopWatcher，同一 URI 上其他 observer 的通知将中断。若未 `UnRef`，observer 引用计数泄露，`obsManager_` 无法释放。

## 6. 云盘同步文件夹注册/注销时序

**前置条件**：`user_file_service_cloud_disk_enable = true`（`SUPPORT_CLOUD_DISK_MANAGER` 已定义），SA 已就绪，调用方持有 `ohos.permission.ACCESS_CLOUD_DISK_INFO`（`GetAllSyncFolders` / `UnregisterForSa` / `GetAllSyncFoldersForSa`）或为普通应用（`Register` / `Unregister` / `Active` / `Deactive`）。

### 6.1 注册同步文件夹

| 步骤 | 操作 | 代码锚点 |
|------|------|----------|
| 1 | `Register(syncFolder)` 加锁 `syncFolderMtx_` | `services/native/cloud_disk_service/src/cloud_disk_service.cpp:149-151` |
| 2 | `IncreaseCnt()` 增加调用计数 | `:154` |
| 3 | `UfsAccessTokenHelper::GetCallerBundleNameAndIndex()` 获取调用方信息 | `:157` |
| 4 | `ValidateSyncFolder()` 校验路径存在性和持久化权限 | `:163` → `:121-147` |
| 5 | `SynchronousRootManager::PutSynchronousRoot()` 写入 RDB | `:170` |
| 6 | `CloudDiskServiceManager::RegisterSyncFolder()` 通知 dfs_service | `:175-176` |
| 7 | 若步骤 6 失败，回滚 RDB：`rootManager.DeleteSynchronousRoot()` | `:178-180` |
| 8 | `SetWorkStatus(true)` 设置系统参数 `persist.clouddiskmanager.workstatus` | `:183` |
| 9 | `NotifyWorkService::NotifySyncFolderEvent(REGISTER)` 发送事件通知 | `:190-193` |
| 10 | `DecreaseCnt()` 减少调用计数 | `:194` |

**违反后果**：若步骤 6 失败未执行步骤 7 回滚，RDB 中存在记录但 dfs_service 侧无对应文件夹，状态不一致。若跳过步骤 4 权限校验，非授权应用可注册任意路径为同步文件夹。若未 `IncreaseCnt` / `DecreaseCnt`，SA 卸载判断 `IsCalledCountValid()` 不准，可能在操作中途卸载。

### 6.2 注销同步文件夹

| 步骤 | 操作 | 代码锚点 |
|------|------|----------|
| 1 | `Unregister(path)` 加锁 `syncFolderMtx_`，`IncreaseCnt()` | `cloud_disk_service.cpp:201-206` |
| 2 | `GetCallerBundleNameAndIndex()` + `GetUserId()` | `:209-214` |
| 3 | `IsSyncFolderInTable(path, syncFolderExts, userId)` 查 RDB | `:216` |
| 4 | 校验 `bundleName` 匹配（仅允许注册者注销） | `:233` |
| 5 | `DoUnregister()` → `CloudDiskServiceManager::UnregisterSyncFolder()` | `:254-255` |
| 6 | `rootManager.DeleteSynchronousRoot(path, userId)` 删 RDB | `:261` |
| 7 | 若 `rootManager.GetRootCount() == 0` 则 `SetWorkStatus(false)` | `:265-266` |
| 8 | `NotifySyncFolderEvent(UNREGISTER)` | `:268-269` |
| 9 | `DecreaseCnt()` | `:243` |

**违反后果**：若先删 RDB 再通知 dfs_service（步骤 5/6 顺序反转），dfs_service 注销时查不到记录可能报错。若跳过步骤 4 bundleName 校验，应用 A 可注销应用 B 的同步文件夹。

## 7. 应用卸载时同步文件夹清理时序

**前置条件**：SA 收到 `COMMON_EVENT_PACKAGE_REMOVED` 广播，且系统参数 `persist.clouddiskmanager.workstatus` 为 `true`。

| 步骤 | 操作 | 代码锚点 |
|------|------|----------|
| 1 | `BundleObserver::OnReceiveEvent()` 接收包移除事件 | `services/native/file_access_service/include/bundle_observer.h:45` |
| 2 | `HandleBundleBroadcast()` 解析事件数据 | `bundle_observer.h:38` |
| 3 | 提取 `bundleName`、`userId`、`appIndex` | `BundleObserver::GetValueByKey()` / `GetNumberFromString()` |
| 4 | `FileAccessService::UnregisterAllByBundle(bundleName, userId, index)` | `services/native/cloud_disk_service/src/cloud_disk_service.cpp:278` |
| 5 | `SynchronousRootManager::GetRootInfosByUserAndBundle()` 查询该应用所有 SyncFolder | `:287` |
| 6 | 遍历每个 SyncFolder：`DeleteSynchronousRoot()` 删 RDB | `:293` |
| 7 | `CloudDiskServiceManager::UnregisterSyncFolder()` 通知 dfs_service | `:298-299` |
| 8 | `NotifySyncFolderEvent(UNREGISTER)` 发送事件 | `:310-311` |
| 9 | 若 `GetRootCount() == 0` 则 `SetWorkStatus(false)` | `:315-316` |

**违反后果**：若先通知 dfs_service 再删 RDB（步骤 6/7 顺序反转），dfs_service 注销后 RDB 仍残留记录。若跳过 `appIndex`，多开场景下可能误删其他应用实例的同步文件夹。若 SA 未按需启动（`workstatus` 不为 `true`），应用卸载后残留 SyncFolder 无人清理。

## 8. App 代理连接管理时序

**前置条件**：应用通过 `ConnectFileExtAbility(want, connection)` 请求直连扩展能力。

| 步骤 | 操作 | 代码锚点 |
|------|------|----------|
| 1 | `ConnectFileExtAbility(want, connection)` 权限校验 | `services/native/file_access_service/src/file_access_service.cpp:742` |
| 2 | 创建 `AgentFileAccessExtConnection(connection, weak_this)` | `:755-756` |
| 3 | `fileAccessExtConnection->ConnectFileExtAbility(want)` 异步连接 | `:762` |
| 4 | `AddAppProxy(connection, fileAccessExtConnection)` 存入 `appProxyMap_` | `:763` → `:847-869` |
| 5 | `connection->AsObject()->AddDeathRecipient(appDeathRecipient_)` | `:865` |
| 6 | 断开时 `DisConnectFileExtAbility()` → `DisconnectAppProxy()` → `RemoveAppProxy()` | `:767-781` → `:889-901` → `:871-887` |
| 7 | 应用进程死亡 `AppDeathRecipient::OnRemoteDied()` → `RemoveAppProxy()` | `:903-919` |

**违反后果**：若跳过步骤 5 死亡监听，应用崩溃后 `appProxyMap_` 残留失效连接，SA 无法卸载（`IsAppProxyMapEmpty()` 返回 false）。若 `appProxyMap_` 已有同 key 的代理时重复添加（步骤 4），旧连接泄露。
