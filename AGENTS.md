# user_file_service 指引

## 项目定位

`user_file_service` 是 OpenHarmony 文件管理子系统（`filemanagement`）下的公共文件访问框架（FileAccessFramework），部件名为 `@ohos/user_file_service`，版本 3.1。该框架向下对接底层文件管理服务（medialibrary、externalFileManager），向上对接应用，提供对公共文件（媒体文件、文档文件、共享盘文件、外置存储文件）的查询、创建、删除、打开、移动、重命名等基础能力，并提供云盘同步文件夹管理（SynchronousRoot）能力。

本部件**不**直接操作底层文件系统，而是通过连接 `FileAccessExtensionAbility` 扩展能力来转发文件操作请求。扩展能力由各文件管理服务（如 medialibrary、ExternalFileManager）实现，本部件仅充当服务总线和观察者管理器。

优先按以下目录定位问题：

- `services/native/file_access_service/` — SA 5010 服务实现（`FileAccessService`），包含服务生命周期、观察者管理、扩展代理管理、云盘同步文件夹管理
- `interfaces/inner_api/file_access/` — 内部 API：`FileAccessExtAbility` 基类、`FileAccessExtStubImpl`、`FileAccessHelper`、`IFileAccessExtBase` IDL 接口
- `interfaces/kits/` — 公共 API 模块：`file.picker`、`file.recent`、`file.trash`、`file.cloudDiskManager`，以及 Taihe（ANI）和 CJ 绑定
- `frameworks/js/napi/` — NAPI 框架实现：`file.fileAccess`、`application.FileAccessExtensionAbility`、`file.fileExtensionInfo`

高频修改路径（变更影响面大，修改前须先读对应知识文档）：

- `services/native/file_access_service/src/file_access_service.cpp` — SA 生命周期、按需加载/卸载、广播注册；修改前读 `docs/knowledge/service-architecture.md`
- `interfaces/inner_api/file_access/include/file_access_ext_stub_impl.h` — 扩展能力 IPC stub 实现；修改前读 `docs/knowledge/extension-ability.md`
- `interfaces/inner_api/file_access/IFileAccessExtBase.idl` — 扩展接口 IDL 定义；修改前读 `docs/knowledge/extension-ability.md`
- `services/IFileAccessServiceBase.idl` — SA 服务接口 IDL 定义；修改前读 `docs/knowledge/service-architecture.md`
- `services/native/cloud_disk_service/src/cloud_disk_synchronous_root_manager.cpp` — 云盘同步文件夹 RDB 持久化；修改前读 `docs/knowledge/cloud-disk-sync.md`

常见任务定位：

| 任务 | 先看 |
|------|------|
| 新增文件操作接口 | `interfaces/inner_api/file_access/IFileAccessExtBase.idl` → `FileAccessExtStubImpl` → `frameworks/js/napi/file_access_ext_ability/` |
| 新增 Picker 面板类型 | `interfaces/kits/picker/` → `frameworks/js/napi/file_accesspicker/` |
| 新增观察者通知类型 | `services/NotifyType.idl` → `file_access_observer_common.h` → `file_access_service.cpp` |
| 新增云盘同步文件夹操作 | `services/native/cloud_disk_service/` → `services/rdb_adapter/` → `services/native/notify_event/` |
| 新增 Feature Flag | `filemanagement_aafwk.gni` → 对应 `#ifdef` 分支 |
| 新增错误码 | `utils/file_access_framework_errno.h` |
| 修改 SA 启动/卸载逻辑 | `services/native/file_access_service/src/file_access_service.cpp` |

> **注意**：本仓库暂无嵌套 `AGENTS.md` 或 `CLAUDE.md`，所有指导集中在本文件及 `docs/` 目录下。

## 构建和验证

构建命令须从 OpenHarmony 源码根目录执行，使用 GN 构建系统。详细构建目标、测试目标和验证规则见 `docs/03-build-test.md`。

```bash
# 从源码根目录构建整个部件
./build.sh --product-name {product} --build-target user_file_service

# 构建子目标
./build.sh --product-name {product} --build-target file_access_service
./build.sh --product-name {product} --build-target fileaccess
./build.sh --product-name {product} --build-target picker

# 单元测试
./build.sh --product-name {product} --build-target user_file_service_unit_test

# Fuzz 测试
./build.sh --product-name {product} --build-target user_file_service_fuzz_test
```

注意事项：
- 云盘功能默认关闭（`user_file_service_cloud_disk_enable = false`），需在产品配置中开启后才编译 `SUPPORT_CLOUD_DISK_MANAGER` 相关代码
- `file_extension_hap` 目标构建的是 `ExternalFileManager` HAP（`com.ohos.UserFile.ExternalFileManager`），需使用 `services/signature/fileextension.p7b` 签名
- 提交代码前须通过 `cfi`、`integer_overflow`、`ubsan`、`boundary_sanitize` 等安全 sanitize 检查

### 完成定义

完成定义（Definition of Done）详见 `docs/03-build-test.md` 的"完成定义"章节。最低要求：

1. 编译通过（两种 Feature Flag 配置均通过，如涉及条件编译）
2. 相关单元测试通过
3. 相关 Fuzz 测试通过（如涉及 IPC 接口）
4. 安全 sanitize 检查通过（`cfi`、`integer_overflow`、`ubsan`、`boundary_sanitize`）
5. 板端验证通过（如涉及功能变更）

### 最终报告

任务完成后须报告以下内容：

- **变更文件列表**：列出修改的源文件和头文件
- **任务类别**：SA 服务 / 扩展接口 / NAPI / 云盘 / 构建 / 其他
- **已读文档**：列出阅读的 `docs/` 文件
- **已遵守约束**：列出适用的开发约束（如分层、接口码同步、时序等）
- **验证结果**：编译/单测/Fuzz/sanitize/板端的具体结果
- **回滚顺序**：如涉及云盘同步文件夹等有回滚要求的变更，须说明回滚顺序

### 验证无法执行时的降级方案

若无法执行全部验证（如无板端环境），须：

1. 执行可执行的验证（编译、单测、Fuzz、sanitize）
2. 明确报告未执行的验证项及原因
3. 在 PR 中标注"需板端验证"并列出待验证步骤
4. 不得跳过编译和单元测试

## 知识索引

| 场景 | 先读 |
|------|------|
| 修改 SA 服务逻辑（启动/停止/按需加载/Unload 定时器） | `docs/knowledge/service-architecture.md` |
| 新增或修改文件操作接口（OpenFile/CreateFile/Delete/Move/Copy 等） | `docs/knowledge/extension-ability.md` |
| 修改 IPC 接口码或 IDL 定义 | `docs/knowledge/extension-ability.md`、`docs/knowledge/service-architecture.md` |
| 修改观察者注册/通知分发逻辑 | `docs/knowledge/service-architecture.md`、`docs/01-workflows.md` |
| 云盘同步文件夹注册/注销/状态变更 | `docs/knowledge/cloud-disk-sync.md` |
| 修改 RDB 持久化层（SynchronousRootManager） | `docs/knowledge/cloud-disk-sync.md` |
| 修改 Picker / Recent / Trash NAPI 模块 | `docs/knowledge/extension-ability.md` |
| 修改 FileAccessExtensionAbility 基类或 JS 扩展实现 | `docs/knowledge/extension-ability.md` |
| 修改构建目标或 Feature Flag | `docs/03-build-test.md` |
| 新增/修改 Fuzz 或单元测试 | `docs/03-build-test.md` |
| 验证文件操作或观察者功能 | `docs/02-verification.md` |
| 理解完整调用链和时序约束 | `docs/01-workflows.md` |

### 路径路由

| 修改路径 | 先读 |
|----------|------|
| `services/native/file_access_service/` | `docs/knowledge/service-architecture.md`、`docs/01-workflows.md` |
| `interfaces/inner_api/file_access/` | `docs/knowledge/extension-ability.md` |
| `interfaces/kits/` | `docs/knowledge/extension-ability.md`、`docs/03-build-test.md` |
| `frameworks/js/napi/` | `docs/knowledge/extension-ability.md` |
| `services/native/cloud_disk_service/` | `docs/knowledge/cloud-disk-sync.md` |
| `services/rdb_adapter/` | `docs/knowledge/cloud-disk-sync.md` |
| `services/native/notify_event/` | `docs/knowledge/cloud-disk-sync.md`、`docs/01-workflows.md` |

### 术语路由

| 术语/缩写 | 含义 | 先读 |
|-----------|------|------|
| SA 5010 | `FileAccessService` 的 SystemAbility ID | `docs/knowledge/service-architecture.md` |
| FileAccessExtensionAbility | 文件访问扩展能力基类，由 medialibrary/ExternalFileManager 实现 | `docs/knowledge/extension-ability.md` |
| SynchronousRoot / SyncFolder | 云盘同步文件夹，通过 RDB 持久化、dfs_service 协同 | `docs/knowledge/cloud-disk-sync.md` |
| cMap_ | SA 内部扩展代理缓存（`unordered_map<string, sptr<IFileAccessExtBase>>`） | `docs/knowledge/service-architecture.md` |
| obsManager_ | SA 内部观察者管理器，维护 URI→Observer 映射 | `docs/knowledge/service-architecture.md` |
| OnDemandTimer / UnloadTimer | SA 按需加载/卸载定时器，使用 `wptr` 弱引用 | `docs/knowledge/service-architecture.md` |
| DeathRecipient | IPC 死亡监听对象，用于扩展代理和观察者的生命周期联动 | `docs/knowledge/service-architecture.md` |
| Sequenceable | IDL 序列化声明，须保持 `Marshalling`/`Unmarshalling` 与 IDL 一致 | `docs/knowledge/extension-ability.md` |
| `SUPPORT_CLOUD_DISK_MANAGER` | 云盘功能条件编译宏，由 `user_file_service_cloud_disk_enable` 控制 | `docs/knowledge/cloud-disk-sync.md` |
| `SANDBOX_MANAGER` | 沙箱管理器条件编译宏，由 `ufs_sandbox_manarer` 控制 | `docs/03-build-test.md` |
| UfsAccessTokenHelper | 权限校验工具类，封装 `AccessToken` 权限检查 | `docs/knowledge/service-architecture.md` |

### 编辑前声明

修改代码前，须先在本任务中声明：

1. **任务类别**：属于 SA 服务 / 扩展接口 / NAPI / 云盘 / 构建 / 其他中的哪一类
2. **已读文档**：根据知识索引和路径路由，列出已阅读的 `docs/` 文件
3. **已知约束**：列出在开发约束中找到的相关约束（如分层、接口码同步、时序等）

未声明以上三项前，不应开始编辑代码。

## 开发约束

### 架构约束

1. **分层约束**：`interfaces/kits/`（公共 API） → `frameworks/js/napi/`（NAPI 实现） → `interfaces/inner_api/file_access/`（内部 API：`FileAccessHelper`） → `services/native/file_access_service/`（SA 5010 服务）。公共 API 不允许直接调用 SA 服务，必须经过 `FileAccessHelper` 间接访问。违反会导致 IPC 边界混乱和权限校验缺失。

2. **SA 单例约束**：`FileAccessService` 是 SystemAbility（SA ID 5010），通过 `REGISTER_SYSTEM_ABILITY_BY_ID` 注册为单例。新增 SA 相关逻辑须在 `FileAccessService::OnStart()` 或 `OnStart(reason)` 中初始化。违反会导致服务未就绪时被调用产生空指针。

3. **扩展代理缓存约束**：SA 通过 `cMap_`（`unordered_map<string, sptr<IFileAccessExtBase>>`）按 bundleName 缓存扩展代理。新增文件操作须通过 `ConnectExtension()` 获取代理，不能绕过缓存直接连接。违反会导致重复连接扩展能力、泄露 IPC 连接。

4. **Feature Flag 集中约束**：所有 Feature Flag 须在 `filemanagement_aafwk.gni` 的 `declare_args()` 中集中声明（`user_file_service_cloud_disk_enable`、`picker_udmf_enabled`、`ufs_sandbox_manarer`）。C++ 代码中通过 `#ifdef SUPPORT_CLOUD_DISK_MANAGER` / `#ifdef SANDBOX_MANAGER` 条件编译。不允许在源文件中硬编码功能开关。违反会导致功能无法在产品级别统一配置。

5. **云盘同步文件夹操作须加锁**：所有 SyncFolder 操作（Register/Unregister/Active/Deactive）须持有 `syncFolderMtx_` 锁。违反会导致并发写入 RDB 数据不一致。

### Clean Code 规范

1. **作用域约束**：内部实现细节（如 `RegisterNotifyImpl`、`OperateObsNode`、`CleanAllNotifyImpl`）须声明为 `private`，仅暴露 IDL 接口方法为 `protected`/`public`。违反会导致外部模块直接调用内部实现，绕过权限校验。

2. **RAII / 资源守护**：所有 `mutex` 须使用 `std::lock_guard<std::mutex>` 管理；`DeathRecipient` 须使用 `sptr` 持有；Timer 须在析构中 `Shutdown()`。违反会导致死锁或资源泄露。

3. **IPC 返回值检查**：所有 IPC 调用返回值须检查 `ERR_OK`，扩展代理获取后须检查 `nullptr`。示例：`GetExtensionProxy()` 返回后须检查 `extensionProxy == nullptr`。违反会导致空指针解引用。

4. **日志隐私**：敏感信息（如 path、code、userId）须使用 `%{private}` 格式说明符，非敏感信息使用 `%{public}`。日志域 `0xD00430A`，标签 `FileAccessFwk`（见 `utils/hilog_wrapper.h`）。违反会导致隐私泄露。

5. **错误码复用**：须复用 `utils/file_access_framework_errno.h` 中定义的错误码（如 `E_PERMISSION`、`E_CONNECT`、`E_INVALID_PARAM`）。不允许自定义新的错误码常量，须在该头文件中集中添加。违反会导致错误码碎片化，调用方无法统一处理。

6. **Access Token 校验**：所有 SA 接口入口须调用 `CheckCallingPermission(FILE_ACCESS_PERMISSION)` 或 `UfsAccessTokenHelper::CheckCallerPermission()`。违反会导致越权访问。

7. **异步生命周期**：`OnDemandTimer` 和 `UnloadTimer` 回调中使用 `wptr<FileAccessService>` 弱引用，回调中须先 `promote()` 并检查 `nullptr`。违反会导致 SA 已销毁后回调访问悬垂指针。

8. **BUILD 边界**：新增源文件须同时加入 `services/BUILD.gn` 的 `sources` 列表和 `interfaces/inner_api/file_access/BUILD.gn`（如属于 inner_api）。新增头文件须加入 `include_dirs`。违反会导致编译失败或头文件不可见。

9. **格式合规**：遵循仓库现有 `.clang-format` 风格，使用 4 空格缩进，命名空间不增加缩进。

### IPC 和接口码约束

1. **SA 服务接口码**：修改 `services/IFileAccessServiceBase.idl` 中的接口时，须同步更新 `services/native/file_access_service/include/file_access_service_ipc_interface_code.h` 中的 `FileAccessServiceInterfaceCode` 枚举（`CMD_REGISTER_NOTIFY` = 0 至 `CMD_DISCONNECT_FILE_EXT_ABILITY` = 5）。IDL 由 `idl_gen_interface` 自动生成 stub/proxy 代码。违反会导致 IPC 编解码不匹配，通信失败。

2. **扩展接口码**：修改 `interfaces/inner_api/file_access/IFileAccessExtBase.idl` 中的接口时，`[ipccode N]` 编号必须连续且不重复（当前 1–19）。须同步更新：
   - `FileAccessExtBaseStub` / `FileAccessExtBaseProxy`（IDL 自动生成）
   - `FileAccessExtStubImpl`（`interfaces/inner_api/file_access/include/file_access_ext_stub_impl.h`）
   - `FileAccessExtAbility`（`interfaces/inner_api/file_access/include/file_access_ext_ability.h`）
   - JS 扩展实现（`frameworks/js/napi/file_access_ext_ability/file_access_ext_ability.js`）
   - `ExternalFileManager` HAP（`services/file_extension_hap/entry/src/main/ets/FileExtensionAbility/FileExtensionAbility.ts`）
   - 单元测试 mock（`test/unittest/mock/`）
   - Fuzz 测试（`test/fuzztest/`）
   
   违反会导致 IPC 命令码不匹配，扩展能力调用失败。

3. **回调接口**：修改 `IFileAccessObserver.idl` 中的 `OnChange` 方法时，须同步更新 `NotifyMessage` 结构（`interfaces/inner_api/file_access/include/file_access_observer_common.h`）和 `NotifyType` 枚举（`services/NotifyType.idl` 和 `file_access_observer_common.h`）。违反会导致通知数据反序列化失败。

4. **Sequenceable 类型**：IDL 中 `sequenceable` 声明的类型（`Uri`、`ConnectExtensionInfo`、`SyncFolder`、`SyncFolderExt`、`FileInfo`、`RootInfo`、`Result`、`FileFilter`、`SharedMemoryInfo`、`Urie`）须保持 `Marshalling`/`Unmarshalling` 实现与 IDL 声明一致。修改结构体字段须同步更新对应的序列化方法。违反会导致 IPC 序列化失败。

### 时序约束

1. **SA 启动时序**：`OnStart(reason)` → `AddSystemAbilityListener(COMMON_EVENT_SERVICE_ID)` → `Init()` → `InitTimer()` → `Publish(this)` → `ready_ = true` → `OnAddSystemAbility()` → `RegisterBundleBroadcast()`。必须先初始化 DeathRecipient 和 Timer，再 Publish。违反会导致服务已注册但内部对象未初始化，调用时空指针崩溃。

2. **SA 按需卸载时序**：`IsUnused()` 返回 true（`obsManager_` 为空且 `appProxyMap_` 为空且 `calledCount_` 为 0） → `UnloadTimer` 计数达 `UNLOAD_SA_WAIT_TIME`（30 秒） → `UnRegisterBundleBroadcast()` → `saManager->UnloadSystemAbility(FILE_ACCESS_SERVICE_ID)`。违反卸载前置条件会导致 SA 被误卸载，正在进行的文件操作中断。

3. **扩展连接时序**：`ConnectExtension(uri, info)` → `GetBundleNameFromUri(uri, bundleName)` → `FindExtProxyByBundleName(bundleName)`（查缓存）→ 未命中则 `GetExtensionProxy(info, extensionProxy)` → `AddDeathRecipient(extensionDeathRecipient_)` → `AddExtProxyInfo(bundleName, extensionProxy)`。必须先查缓存再连接，连接后必须加死亡监听再存入缓存。违反会导致重复连接或代理泄露。

4. **观察者注册时序**：`RegisterNotify(uri, observer, info)` → `CheckCallingPermission()` → `convertUris(uri, uris)` → 遍历 uris `RegisterNotifyImpl()` → `obsManager_.getId()`（查重）→ 未注册则 `obsContext->Ref()` → `obsManager_.save(obsContext)` → `AddDeathRecipient(observerDeathRecipient_)` → `OperateObsNode()` → `ConnectExtension()` → `extensionProxy->StartWatcher(uriConvert)` → 建立父子关系 → `relationshipMap_.insert()`。必须先注册观察者到 `obsManager_` 再连接扩展发 StartWatcher。违反会导致扩展已开始监听但观察者上下文未保存，通知丢失。

5. **云盘同步文件夹注册时序**：`Register(syncFolder)` → `IncreaseCnt()` → `UfsAccessTokenHelper::GetCallerBundleNameAndIndex()` → `ValidateSyncFolder()`（路径校验 + 权限校验）→ `SynchronousRootManager::PutSynchronousRoot()`（写 RDB）→ `CloudDiskServiceManager::RegisterSyncFolder()`（通知 dfs_service）→ `SetWorkStatus(true)`（设系统参数）→ `NotifyWorkService::NotifySyncFolderEvent(REGISTER)`。若 dfs_service 注册失败，须回滚 RDB（`DeleteSynchronousRoot`）。违反回滚顺序会导致 RDB 与 dfs_service 状态不一致。

6. **应用卸载清理时序**：收到 `COMMON_EVENT_PACKAGE_REMOVED` → `BundleObserver::OnReceiveEvent()` → `HandleBundleBroadcast()` → `UnregisterAllByBundle(bundleName, userId, index)` → 遍历该应用所有 SyncFolder → `DeleteSynchronousRoot()` → `CloudDiskServiceManager::UnregisterSyncFolder()` → `NotifySyncFolderEvent(UNREGISTER)`。必须先删 RDB 再通知 dfs_service，否则 dfs_service 侧仍认为文件夹有效。违反会导致卸载后残留同步文件夹记录。

### 升级约束（Ask before）

以下变更须在修改前请求对应团队审查，不得自行合入：

1. **公共 API 签名变更**：修改 `interfaces/kits/` 下的公共 API 签名、参数或返回值 → 须请求 **API 兼容性审查**，确认向前兼容或走废弃流程。

2. **IDL 接口变更**：修改 `services/IFileAccessServiceBase.idl` 或 `interfaces/inner_api/file_access/IFileAccessExtBase.idl` 的接口码或方法定义 → 须请求 **IPC 协议兼容性审查**，确认所有调用方（medialibrary、ExternalFileManager、JS 扩展）已同步更新。

3. **权限模型变更**：修改 `UfsAccessTokenHelper` 的权限校验逻辑、新增/删除 `CheckCallingPermission` 调用、修改 `FILE_ACCESS_PERMISSION` 权限声明 → 须请求 **安全审查**。

4. **RDB 模式或序列化格式变更**：修改 `SynchronousRootManager` 的表结构、修改 IDL `sequenceable` 类型字段 → 须请求 **数据兼容性审查**，确认跨版本升级不丢数据。

5. **Feature Flag 默认值变更**：修改 `filemanagement_aafwk.gni` 中 `declare_args()` 的默认值 → 须请求 **产品配置审查**，确认不影响存量产品构建。

6. **SA 生命周期变更**：修改 `FileAccessService::OnStart()` / `OnStop()` / `UnloadTimer` 逻辑 → 须请求 **子系统负责人审查**，确认不影响 SA 按需加载/卸载行为。
