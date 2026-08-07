# 服务架构知识

本文只记录 FileAccessService SA 架构、生命周期、观察者管理、扩展代理管理、客户端模式和按需卸载机制。扩展能力接口和文件操作见 `extension-ability.md`，云盘同步文件夹见 `cloud-disk-sync.md`。

## SA 定位

`FileAccessService` 是 OpenHarmony SystemAbility（SA ID `5010`），进程名 `file_access_service`，运行用户 `ufs:ufs`。作为公共文件访问框架的服务总台，不直接操作文件系统，而是：

1. 管理应用与文件访问扩展能力（`FileAccessExtensionAbility`）之间的连接
2. 管理文件变更观察者（注册/注销/通知分发）
3. 管理云盘同步文件夹生命周期（注册/注销/状态变更/持久化）
4. 监听应用卸载事件并清理残留同步文件夹

## SA 生命周期

| 阶段 | 操作 | 代码锚点 |
|------|------|----------|
| 注册 | `REGISTER_SYSTEM_ABILITY_BY_ID(FileAccessService, FILE_ACCESS_SERVICE_ID, false)` | `services/native/file_access_service/src/file_access_service.cpp:38` |
| 按需启动 | 收到 `COMMON_EVENT_PACKAGE_REMOVED` 且 `persist.clouddiskmanager.workstatus` 为 `true` | `services/5010.json:11-23` |
| OnStart(reason) | 监听 `COMMON_EVENT_SERVICE_ID`，创建 `BundleObserver`，处理广播，调用 `OnStart()` | `FileAccessService::OnStart(reason)` `services/native/cloud_disk_service/src/cloud_disk_service.cpp:593` |
| OnStart() | `Init()` → `Publish(this)` → `ready_ = true` | `FileAccessService::OnStart()` `file_access_service.cpp:76` |
| Init() | 创建三个 `DeathRecipient`（Extension/Observer/App） | `FileAccessService::Init()` `file_access_service.cpp:141` |
| InitTimer() | 创建 `OnDemandTimer`（通知批量分发）和 `UnloadTimer`（空闲卸载） | `FileAccessService::InitTimer()` `file_access_service.cpp:691` |
| OnAddSystemAbility | `COMMON_EVENT_SERVICE_ID` 就绪后 `RegisterBundleBroadcast()` | `FileAccessService::OnAddSystemAbility()` `cloud_disk_service.cpp:619` |
| OnStop() | `UnRegisterBundleBroadcast()` → `ready_ = false` | `FileAccessService::OnStop()` `file_access_service.cpp:89` |
| 空闲卸载 | `UnloadTimer` 计数达 30 秒 → `IsUnused()` 为 true → `UnloadSystemAbility(5010)` | `cloud_disk_service.cpp:722-738` |

## 类职责表

| 类 | 职责 | 路径 |
|----|------|------|
| `FileAccessService` | SA 5010 主类，实现 `IFileAccessServiceBase` 接口，管理观察者、扩展代理、云盘同步、SA 生命周期 | `services/native/file_access_service/include/file_access_service.h` |
| `FileAccessServiceClient` | SA 客户端单例，懒加载 SA 代理，处理 SA 死亡重连 | `services/native/file_access_service/include/file_access_service_client.h` |
| `FileAccessServiceBaseStub` | IDL 自动生成的 IPC stub 基类（`IFileAccessServiceBase`） | IDL 生成（`services/BUILD.gn:59` `idl_gen_interface`） |
| `FileAccessServiceBaseProxy` | IDL 自动生成的 IPC proxy | IDL 生成 |
| `FileAccessExtConnection` | 扩展能力连接器（继承 `AbilityConnectionStub`），用于 SA 内部连接扩展 | `services/native/file_access_service/include/file_access_ext_connection.h:35` |
| `AgentFileAccessExtConnection` | 应用代理连接器，转发应用的扩展连接请求 | `file_access_ext_connection.h:62` |
| `BundleObserver` | 公共事件订阅者，监听 `PACKAGE_REMOVED` 触发清理 | `services/native/file_access_service/include/bundle_observer.h:33` |
| `ObserverContext` | 观察者上下文，封装 `IFileAccessObserver` 和引用计数 | `file_access_service.h:46` |
| `ObserverNode` | URI 观察节点，维护父子关系和 observer code 列表 | `file_access_service.h:89` |
| `OnDemandTimer` | 通知批量分发定时器（500ms 间隔，最多 20 次） | `file_access_service.h:123` |
| `UnloadTimer` | SA 空闲卸载定时器（1 秒间隔，30 次后触发） | `file_access_service.h:179` |
| `HolderManager<T>` | 泛型持有器模板，管理 observer 上下文的增删查 | `services/native/file_access_service/include/holder_manager.h:31` |
| `UfsAccessTokenHelper` | Access Token 工具类，权限校验、bundleName 获取、路径转换 | `services/native/file_access_service/include/ufs_access_token_helper.h:22` |

## DeathRecipient 架构

| 死亡监听类 | 监听对象 | 触发动作 | 位置 |
|------------|----------|----------|------|
| `ExtensionDeathRecipient` | 扩展能力代理（`IFileAccessExtBase`） | `ResetProxy()`：从 `cMap_` 清除失效代理 | `file_access_service.h:278` |
| `ObserverDeathRecipient` | 观察者代理（`IFileAccessObserver`） | `CleanRelativeObserver()`：清除该观察者的所有 URI 注册 | `file_access_service.h:288` |
| `AppDeathRecipient` | 应用连接代理（`IAbilityConnection`） | `RemoveAppProxy()`：从 `appProxyMap_` 清除失效连接 | `file_access_service.h:338` |
| `ProxyDeathRecipient`（Client） | SA 代理（`IFileAccessServiceBase`） | `InvaildInstance()`：清空客户端代理，下次调用重新加载 SA | `file_access_service_client.h:42` |

## 扩展代理缓存表

| 缓存 | 数据结构 | 用途 | 位置 |
|------|----------|------|------|
| `cMap_` | `unordered_map<string, sptr<IFileAccessExtBase>>` | 按 bundleName 缓存扩展代理 | `file_access_service.h:336` |
| `appProxyMap_` | `unordered_map<size_t, sptr<AgentFileAccessExtConnection>>` | 按连接对象地址缓存应用代理 | `file_access_service.h:350` |
| `appConnection_` | `unordered_map<size_t, sptr<IAbilityConnection>>` | 记录应用连接对象（用于断开） | `file_access_service.h:351` |
| `relationshipMap_` | `unordered_map<string, shared_ptr<ObserverNode>>` | URI → 观察节点树 | `file_access_service.h:333` |
| `obsManager_` | `HolderManager<shared_ptr<ObserverContext>>` | observer code → 观察者上下文 | `file_access_service.h:334` |

## SA 接口码枚举

```cpp
enum class FileAccessServiceInterfaceCode {
    CMD_REGISTER_NOTIFY = 0,
    CMD_UNREGISTER_NOTIFY,
    CMD_ONCHANGE,
    CMD_GET_EXTENSION_PROXY,
    CMD_CONNECT_FILE_EXT_ABILITY,
    CMD_DISCONNECT_FILE_EXT_ABILITY
};
```

来源：`services/native/file_access_service/include/file_access_service_ipc_interface_code.h:21`

## SA IDL 接口

```java
interface OHOS.FileAccessFwk.IFileAccessServiceBase {
    void RegisterNotify(Uri uri, boolean notifyForDescendants, IFileAccessObserver observer, ConnectExtensionInfo info);
    void UnregisterNotify(Uri uri, IFileAccessObserver observer, ConnectExtensionInfo info);
    void OnChange(Uri uri, NotifyType notifyType);
    void GetExtensionProxy(ConnectExtensionInfo info, IFileAccessExtBase extensionProxy);
    void ConnectFileExtAbility(Want want, IAbilityConnection connection);
    void DisConnectFileExtAbility(IAbilityConnection connection);
    void UnregisterNotifyNoObserver(Uri uri, ConnectExtensionInfo info);
    void Register(SyncFolder syncFolder);
    void Unregister(String path);
    void Active(String path);
    void Deactive(String path);
    void GetSyncFolders(SyncFolder[] syncFolders);
    void GetAllSyncFolders(SyncFolderExt[] syncFolderExts);
    void UpdateDisplayName(String path, String displayName);
    void UnregisterForSa(String path);
    void GetAllSyncFoldersForSa(SyncFolderExt[] syncFolderExts);
}
```

来源：`services/IFileAccessServiceBase.idl:25`

## 观察者回调接口

```java
[callback] interface OHOS.FileAccessFwk.IFileAccessObserver {
    void OnChange(NotifyMessage notifyMessage);
}
```

来源：`services/IFileAccessObserver.idl:17`

## NotifyType 枚举

```
enum NotifyType {
    NOTIFY_ADD = 0,
    NOTIFY_DELETE,
    NOTIFY_MOVE_TO,
    NOTIFY_MOVE_FROM,
    NOTIFY_MOVE_SELE,
    NOTIFY_DEVICE_ONLINE,
    NOTIFY_DEVICE_OFFLINE
};
```

来源：`services/NotifyType.idl:16`

## 线程安全表

| 锁 | 保护对象 | 位置 |
|----|----------|------|
| `mutex_`（静态） | `ConnectExtension()` 中扩展代理获取和 `cMap_` 写入 | `file_access_service.h:331` |
| `nodeMutex_` | `relationshipMap_`（URI 观察节点树） | `file_access_service.h:332` |
| `mapMutex_` | `cMap_` 读写和 `ResetProxy()` | `file_access_service.h:335` |
| `appProxyMutex_` | `appProxyMap_` 和 `appConnection_` | `file_access_service.h:349` |
| `calledMutex_` | `calledCount_`（调用计数） | `file_access_service.h:354` |
| `syncFolderMtx_` | 同步文件夹操作（Register/Unregister 等） | `file_access_service.h:348` |
| `observerMux_` | `BundleObserver` 创建和广播注册 | `file_access_service.h:243` |
| `timerMutex_`（OnDemandTimer） | `isTimerStart_` 和 timer 启动 | `file_access_service.h:176` |
| `obsCodeMutex_`（ObserverNode） | `obsCodeList_` | `file_access_service.h:95` |
| `mapMutex_`（ObserverContext） | `notifyMap_` | `file_access_service.h:84` |
| `holderMutex_`（HolderManager） | `holder_` map | `holder_manager.h:126` |
| `rdbAdapterMtx_`（RdbAdapter） | `store_` RDB 操作 | `ufs_rdb_adapter.h:71` |
| `aclMutex_`（SynchronousRootManager） | ACL 相关操作 | `cloud_disk_synchronous_root_manager.h:91` |
| `rdbMutex_`（SynchronousRootManager） | RDB 读写 | `cloud_disk_synchronous_root_manager.h:92` |

## URI BundleName 映射表

| URI 前缀 | Alias | BundleName | 代码锚点 |
|----------|-------|------------|----------|
| `file://media/...` | `media` | `com.ohos.medialibrary.medialibrarydata` | `file_access_helper.h:46-47` |
| `file://docs/...` | `docs` | `com.ohos.UserFile.ExternalFileManager` | `file_access_helper.h:48-49` |

## 设备类型枚举

```cpp
constexpr int32_t DEVICE_LOCAL_DISK = 1;            // 本地磁盘
constexpr int32_t DEVICE_SHARED_DISK = 2;           // 多用户共享盘
constexpr int32_t DEVICE_SHARED_TERMINAL = 3;       // 分布式组网终端
constexpr int32_t DEVICE_NETWORK_NEIGHBORHOODS = 4;  // 网络邻居设备
constexpr int32_t DEVICE_EXTERNAL_MTP = 5;          // MTP 设备
constexpr int32_t DEVICE_EXTERNAL_USB = 6;           // USB 设备
constexpr int32_t DEVICE_EXTERNAL_CLOUD = 7;         // 云盘设备
```

来源：`interfaces/inner_api/file_access/include/file_access_extension_info.h:49-55`

## SA 配置文件

| 配置项 | 值 | 路径 |
|--------|-----|------|
| SA ID | 5010 | `services/5010.json:5` |
| 进程名 | `file_access_service` | `services/5010.json:2` |
| 库路径 | `libfile_access_service.z.so` | `services/5010.json:6` |
| 按需创建 | `false`（`run-on-create`） | `services/5010.json:7` |
| 按需启动 | `COMMON_EVENT_PACKAGE_REMOVED` + `persist.clouddiskmanager.workstatus == true` | `services/5010.json:11-23` |
| 缓存公共事件 | `true` | `services/5010.json:10` |
| UID/GID | `ufs:ufs` | `services/file_access_service.cfg:13-14` |
| SELinux | `u:r:file_access_service:s0` | `services/file_access_service.cfg:19` |
| 数据库目录 | `/data/service/el1/public/database/ufs_db` | `services/file_access_service.cfg:5-6` |
| 权限 | `FILE_ACCESS_MANAGER`、`CONNECT_FILE_ACCESS_EXTENSION`、`ACCESS_CLOUD_DISK_INFO`、`CHECK_SANDBOX_POLICY` | `services/file_access_service.cfg:20-25` |

## 常见误用

| 误用 | 后果 | 修正 |
|------|------|------|
| 在 `OnStart()` 中直接注册广播（不等 `COMMON_EVENT_SERVICE_ID` 就绪） | `CommonEventManager::Subscribe` 可能失败 | 在 `OnAddSystemAbility(COMMON_EVENT_SERVICE_ID)` 回调中注册 |
| `ConnectExtension()` 不加锁直接操作 `cMap_` | 并发场景下 `cMap_` 数据竞争 | 使用 `mutex_` 或 `mapMutex_` 保护 |
| `UnloadTimer` 回调中直接使用 `this` | SA 已销毁后访问悬垂指针 | 使用 `wptr<FileAccessService>` 弱引用，回调中 `promote()` 后检查 |
| 修改 IDL 后未更新 `FileAccessServiceInterfaceCode` 枚举 | IPC 命令码不匹配，通信失败 | 同步更新 `file_access_service_ipc_interface_code.h` |
| `OnChange` 中直接回调 `obs_->OnChange()` 不走批量机制 | 高频文件变更产生大量 IPC，性能下降 | 通过 `SendListNotify()` 攒批，由 `OnDemandTimer` 批量回调 |
| 在 `IsUnused()` 返回 true 后仍操作 `obsManager_` | SA 正在卸载，数据可能已被清理 | 通过 `calledCount_` 和锁确保操作期间 SA 不卸载 |
| `ObserverNode` 父子关系不维护（不调用 `RemoveRelations`） | URI 节点泄露，`relationshipMap_` 持续增长 | 注销时调用 `RemoveRelations()` 清理父子引用 |

## 反模式/修改前检查

- 新增 SA 接口方法必须先在 `services/IFileAccessServiceBase.idl` 中声明，由 `idl_gen_interface` 自动生成 stub/proxy
- 新增 SA 接口方法必须同步更新 `file_access_service_ipc_interface_code.h` 枚举
- 不允许直接调用 `RegisterNotifyImpl` / `OperateObsNode` 等 private 方法，必须通过 IDL 生成的 public/protected 方法
- 新增 `DeathRecipient` 必须在 `Init()` 中创建，在 `OnStart()` 之前
- 修改 `UnloadTimer` 的 `UNLOAD_SA_WAIT_TIME`（30 秒）或 `ONE_SECOND`（1 秒）须评估对 SA 生命周期的影响
- 修改 `OnDemandTimer` 的 `NOTIFY_TIME_INTERVAL`（500ms）或 `MAX_WAIT_TIME`（20 次）须评估对通知实时性的影响
- 新增 URI alias 映射必须更新 `file_access_helper.h` 和 `file_access_service.cpp::GetBundleNameFromUri()`
- 不允许在 `FileAccessService` 构造函数中做重操作（仅 `handleBroadCastThreadPool_.Start(THREAD_NUM)`），重操作在 `Init()` / `OnStart()` 中

## 测试指引

- SA 服务测试：`test/unittest/file_access_service_test.cpp` — 测试 `FileAccessService` 各接口
- 通知服务测试：`test/unittest/notify_work_service_test.cpp` — 测试 `NotifyWorkService`
- SA 接口 Fuzz：`test/fuzztest/fileaccessservice*_fuzzer/` — 12 个 SA 接口 fuzz 目标
- Mock 文件：`test/unittest/mock/file_access_service_mock.h` — SA mock
- Token Mock：`test/fuzztest/privacy_comm/` — `UserFileServiceTokenMock` 设置测试 Token
