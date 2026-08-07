# 云盘同步文件夹知识

本文只记录云盘同步文件夹（SynchronousRoot）的数据模型、注册/注销/状态变更流程、RDB 持久化、dfs_service 集成和事件通知。SA 服务架构见 `service-architecture.md`，扩展能力接口见 `extension-ability.md`。

## 功能概述

云盘同步文件夹（SynchronousRoot）是 `user_file_service` 提供的云盘管理能力，允许应用将本地目录注册为同步根，由 `dfs_service` 负责与云端同步。该功能受 Feature Flag `user_file_service_cloud_disk_enable` 控制（默认关闭），开启时定义 `SUPPORT_CLOUD_DISK_MANAGER`。

| 条件 | 值 | 代码锚点 |
|------|-----|----------|
| Feature Flag | `user_file_service_cloud_disk_enable = false`（默认） | `filemanagement_aafwk.gni:24` |
| C++ 宏 | `#ifdef SUPPORT_CLOUD_DISK_MANAGER` | `services/BUILD.gn:183-185` |
| 外部依赖 | `dfs_service:clouddiskservice_kit_inner` | `services/BUILD.gn:204-206` |
| 权限（SA 调用方） | `ohos.permission.ACCESS_CLOUD_DISK_INFO` | `cloud_disk_service.cpp:32` |
| 权限（应用调用方） | 无额外权限（通过 `FILE_ACCESS_MANAGER`） | — |
| 系统参数 | `persist.clouddiskmanager.workstatus` | `ufs_db_services_constants.h:48-50` |

## 注册/注销流程

| 阶段 | 操作 | 代码锚点 |
|------|------|----------|
| 1. 注册入口 | `FileAccessService::Register(syncFolder)` 加锁 `syncFolderMtx_`，`IncreaseCnt()` | `services/native/cloud_disk_service/src/cloud_disk_service.cpp:149-154` |
| 2. 获取调用方 | `UfsAccessTokenHelper::GetCallerBundleNameAndIndex(bundleName, index)` | `:157` |
| 3. 获取用户 | `UfsAccessTokenHelper::GetUserId()` | `:162` |
| 4. 校验 | `ValidateSyncFolder(syncFolder, bundleName, index, userId)` | `:163` → `:121-147` |
| 5. 写 RDB | `SynchronousRootManager::PutSynchronousRoot(syncFolder, bundleName, userId, index)` | `:170` |
| 6. 通知 dfs_service | `CloudDiskServiceManager::RegisterSyncFolder(userId, bundleName, path)` | `:175-176` |
| 7. 失败回滚 | `rootManager.DeleteSynchronousRoot(path, userId)` | `:178-180` |
| 8. 设工作状态 | `SetWorkStatus(true)` → `system::SetParameter(persist.clouddiskmanager.workstatus, ON)` | `:183` |
| 9. 发事件 | `NotifyWorkService::NotifySyncFolderEvent(syncFolderExt, REGISTER)` | `:190-193` |
| 10. 完成 | `DecreaseCnt()` | `:194` |

## 校验规则表

| 校验项 | 失败错误码 | 代码锚点 |
|--------|-----------|----------|
| `SynchronousRootManager::IsSyncFolderValid()` — 路径合法性和冲突检测 | 见 `cloud_disk_synchronous_root_manager.h:76` | `cloud_disk_service.cpp:125-130` |
| `UfsAccessTokenHelper::PathToPhysicalPath()` — 路径转换 | `E_SYNC_FOLDER_PATH_NOT_EXIST` | `cloud_disk_service.cpp:133-137` |
| `FileAccessService::IsDirectoryExists(physicalPath)` — 目录存在 | `E_SYNC_FOLDER_PATH_NOT_EXIST` | `:134` → `:59-72` |
| `UfsAccessTokenHelper::CheckPathPermission(path)` — 持久化权限 | `E_SYNC_FOLDER_PATH_UNAUTHORIZED` | `:139-142` |

## 类职责表

| 类 | 职责 | 路径 |
|----|------|------|
| `FileAccessService` | SA 层入口，实现 Register/Unregister/Active/Deactive/GetSyncFolders/GetAllSyncFolders/UpdateDisplayName/UnregisterForSa/GetAllSyncFoldersForSa | `services/native/file_access_service/include/file_access_service.h` |
| `SynchronousRootManager` | 单例，管理同步文件夹的 RDB 持久化（增删改查） | `services/native/file_access_service/include/cloud_disk_synchronous_root_manager.h:41` |
| `RdbAdapter` | RDB 操作实现（继承 `IRdbAdapter`），封装 `NativeRdb::RdbStore` | `services/rdb_adapter/include/ufs_rdb_adapter.h:52` |
| `OpenCallback` | RDB 打开回调，建表和建索引 | `ufs_rdb_adapter.h:74` |
| `CloudDiskSyncFolderManager` | dfs_service 侧同步文件夹管理抽象接口 | `interfaces/inner_api/cloud_disk_kit_inner/include/cloud_disk_sync_folder_manager.h:22` |
| `CloudDiskSyncFolderManagerImpl` | dfs_service 侧实现（转发到 `CloudDiskServiceManager`） | `interfaces/inner_api/cloud_disk_kit_inner/include/cloud_disk_sync_folder_manager_impl.h:22` |
| `CloudDiskServiceManager` | dfs_service 云盘服务管理器（外部依赖） | `dfs_service:clouddiskservice_kit_inner`（外部仓） |
| `NotifyWorkService` | 事件通知服务（单例），发送同步文件夹变更事件 | `services/native/notify_event/include/notify_work_service.h:21` |
| `UfsAccessTokenHelper` | 权限校验、bundleName/index 获取、userId 获取、路径转换 | `services/native/file_access_service/include/ufs_access_token_helper.h:22` |
| `BundleObserver` | 包移除事件监听，触发 `UnregisterAllByBundle()` 清理 | `services/native/file_access_service/include/bundle_observer.h:33` |

## 数据模型

### SyncFolder 结构体

```cpp
struct SyncFolder : public OHOS::Parcelable {
    std::string path_;
    State state_ { State::INACTIVE };
    uint32_t displayNameResId_ { 0 };
    std::string displayName_;
};
```

来源：`interfaces/inner_api/cloud_disk_kit_inner/include/cloud_disk_comm.h:33`

### SyncFolderExt 结构体

```cpp
struct SyncFolderExt : SyncFolder {
    std::string bundleName_;
};
```

来源：`cloud_disk_comm.h:47`

### State 枚举

```cpp
enum class State {
    INACTIVE,
    ACTIVE,
    MAX_VALUE
};
```

来源：`cloud_disk_comm.h:27`

## NotifyWorkService 事件类型

```cpp
enum class EventType {
    REGISTER,
    UNREGISTER,
    ACTIVE,
    INACTIVE,
    UPDATE,
    MAX_EVENT_TYPE
};
```

来源：`services/native/notify_event/include/notify_work_service.h:23`

## RDB 常量表

| 常量 | 用途 | 代码锚点 |
|------|------|----------|
| `SYNCHRONOUS_ROOT_DATA_RDB_PATH` | RDB 数据库路径 | `ufs_db_services_constants.h:35` |
| `SYNCHRONOUS_ROOT_DATABASE_NAME` | 数据库名 | `:36` |
| `SYNCHRONOUS_ROOT_TABLE` | 表名 | `:43` |
| `CREATE_SYNCHRONOUS_ROOT_TABLE_SQL` | 建表 SQL | `:37` |
| `PATH` | 列名：路径 | `:28` |
| `STATE` | 列名：状态 | `:29` |
| `DISPLAY_NAME_RES_ID` | 列名：显示名资源 ID | `:30` |
| `BUNDLENAME` | 列名：包名 | `:31` |
| `CLOUD_DISK_DISPLAY_NAME` | 列名：云盘显示名 | `:32` |
| `USERID` | 列名：用户 ID | `:33` |
| `INDEX` | 列名：应用实例索引 | `:34` |
| `SYNCFOLDER_WORK_STATUS_KEY` | 系统参数键 `persist.clouddiskmanager.workstatus` | `:48` |
| `SYNCFOLDER_WORK_STATUS_ON` | 工作状态开 `true` | `:49` |
| `SYNCFOLDER_WORK_STATUS_OFF` | 工作状态关 `false` | `:50` |
| `RDB_INIT_MAX_TIMES` | RDB 初始化最大重试次数（30） | `:25` |
| `RDB_INIT_INTERVAL_TIME` | RDB 初始化重试间隔（100000 微秒） | `:26` |

## 云盘错误码表

| 错误码 | 名称 | 含义 | 代码锚点 |
|--------|------|------|----------|
| 34400001 | `E_INVALID_PARAM` | 无效参数 | `utils/file_access_framework_errno.h:55` |
| 34400002 | `E_SYNC_FOLDER_PATH_UNAUTHORIZED` | 同步文件夹路径未授权 | `:56` |
| 34400003 | `E_IPC_FAILED` | IPC 错误 | `:57` |
| 34400004 | `E_SYNC_FOLDER_LIMIT_EXCEEDED` | 同步文件夹数量超限 | `:58` |
| 34400005 | `E_SYNC_FOLDER_CONFLICT_SELF` | 与自身应用同步文件夹冲突 | `:59` |
| 34400006 | `E_SYNC_FOLDER_CONFLICT_OTHER` | 与其他应用同步文件夹冲突 | `:60` |
| 34400007 | `E_REGISTER_SYNC_FOLDER_FAILED` | 注册同步文件夹失败 | `:61` |
| 34400008 | `E_SYNC_FOLDER_NOT_REGISTERED` | 同步文件夹未注册 | `:62` |
| 34400009 | `E_REMOVE_SYNC_FOLDER_FAILED` | 移除同步文件夹失败 | `:63` |
| 34400010 | `E_SYNC_FOLDER_PATH_NOT_EXIST` | 同步文件夹路径不存在 | `:64` |
| 34400011 | `E_LISTENER_NOT_REGISTERED` | 监听器未注册 | `:65` |
| 34400012 | `E_LISTENER_ALREADY_REGISTERED` | 监听器已注册 | `:66` |
| 34400013 | `E_INVALID_CHANGE_SEQUENCE` | 无效变更序列 | `:67` |
| 34400014 | `E_TRY_AGAIN` | 请重试 | `:68` |
| 34400015 | `E_SYSTEM_RESTRICTED` | 系统限制 | `:69` |
| 801 | `E_NOT_SUPPORT` | 设备不支持（功能未开启） | `:50` |

## SA 接口与调用方权限对照表

| SA 接口方法 | 调用方权限 | 特殊条件 | 代码锚点 |
|-------------|-----------|----------|----------|
| `Register(syncFolder)` | 应用通过 `FILE_ACCESS_MANAGER` | `SUPPORT_CLOUD_DISK_MANAGER` | `cloud_disk_service.cpp:149` |
| `Unregister(path)` | 应用通过 `FILE_ACCESS_MANAGER` | 同上 | `:201` |
| `Active(path)` | 应用通过 `FILE_ACCESS_MANAGER` | 同上 | `:374` |
| `Deactive(path)` | 应用通过 `FILE_ACCESS_MANAGER` | 同上 | `:388` |
| `GetSyncFolders(syncFolders)` | 应用通过 `FILE_ACCESS_MANAGER` | 同上 | `:402` |
| `UpdateDisplayName(path, displayName)` | 应用通过 `FILE_ACCESS_MANAGER` | 同上 | `:458` |
| `GetAllSyncFolders(syncFolderExts)` | `ACCESS_CLOUD_DISK_INFO` | 同上 | `:431-440` |
| `UnregisterForSa(path)` | `ACCESS_CLOUD_DISK_INFO` | 同上 | `:513-519` |
| `GetAllSyncFoldersForSa(syncFolderExts)` | `ACCESS_CLOUD_DISK_INFO` | 同上 | `:564-569` |

## 调用计数机制

| 操作 | 增/减 | 代码锚点 |
|------|-------|----------|
| `Register` | `IncreaseCnt()` → `DecreaseCnt()` | `cloud_disk_service.cpp:154, 194` |
| `Unregister` | `IncreaseCnt()` → `DecreaseCnt()` | `:206, 243` |
| `Active` | `IncreaseCnt()` → `DecreaseCnt()` | `:378, 381` |
| `Deactive` | `IncreaseCnt()` → `DecreaseCnt()` | `:392, 395` |
| `GetSyncFolders` | `IncreaseCnt()` → `DecreaseCnt()` | `:406, 424` |
| `GetAllSyncFolders` | `IncreaseCnt()` → `DecreaseCnt()` | `:435, 451` |
| `UpdateDisplayName` | `IncreaseCnt()` → `DecreaseCnt()` | `:462, 506` |
| `UnregisterForSa` | `IncreaseCnt()` → `DecreaseCnt()` | `:517, 557` |
| `GetAllSyncFoldersForSa` | `IncreaseCnt()` → `DecreaseCnt()` | `:568, 586` |

`calledCount_` 用于 `IsUnused()` 判断 SA 是否可卸载。`IncreaseCnt()` / `DecreaseCnt()` 在 `cloud_disk_service.cpp:36-58` 实现。

## 工作状态参数机制

| 操作 | 参数值 | 代码锚点 |
|------|--------|----------|
| `Register` 成功且 `GetRootCount() > 0` | `persist.clouddiskmanager.workstatus = true` | `cloud_disk_service.cpp:183` → `SetWorkStatus(true)` `:86-100` |
| `Unregister` / `DoUnregister` 且 `GetRootCount() == 0` | `persist.clouddiskmanager.workstatus = false` | `:265-266` → `SetWorkStatus(false)` |
| `UnregisterAllByBundle` 且 `GetRootCount() == 0` | `persist.clouddiskmanager.workstatus = false` | `:315-316` |
| `UnregisterForSa` 且 `GetRootCount() == 0` | `persist.clouddiskmanager.workstatus = false` | `:538-539` |

该参数同时作为 SA 按需启动条件（`5010.json:18-20`）和 SA Dump 判断依据。

## 线程安全表

| 锁 | 保护对象 | 位置 |
|----|----------|------|
| `syncFolderMtx_` | 所有 SyncFolder 操作（Register/Unregister/Active/Deactive/Changestate 等） | `file_access_service.h:348` |
| `calledMutex_` | `calledCount_` 调用计数 | `file_access_service.h:354` |
| `rdbAdapterMtx_`（RdbAdapter） | `store_` RDB 操作 | `ufs_rdb_adapter.h:71` |
| `rdbStoreMtx_`（OpenCallback） | RDB Store 创建/升级 | `ufs_rdb_adapter.h:81` |
| `aclMutex_`（SynchronousRootManager） | ACL 相关操作 | `cloud_disk_synchronous_root_manager.h:91` |
| `rdbMutex_`（SynchronousRootManager） | RDB 读写 | `cloud_disk_synchronous_root_manager.h:92` |
| `observerMux_`（FileAccessService） | `BundleObserver` 创建和广播注册 | `file_access_service.h:243` |

## 常见误用

| 误用 | 后果 | 修正 |
|------|------|------|
| 在 `user_file_service_cloud_disk_enable = false` 时调用云盘接口 | 返回 `E_NOT_SUPPORT`（801），功能不可用 | 在产品配置中开启 `user_file_service_cloud_disk_enable = true` |
| `Register` 失败后不回滚 RDB | RDB 中存在记录但 dfs_service 侧无对应文件夹 | 失败时调用 `rootManager.DeleteSynchronousRoot()` 回滚（见 `cloud_disk_service.cpp:178-180`） |
| `Unregister` 时先删 RDB 再通知 dfs_service | dfs_service 注销时查不到记录可能报错 | 先调用 `CloudDiskServiceManager::UnregisterSyncFolder()` 再 `DeleteSynchronousRoot()` |
| `SetWorkStatus` 中直接 `SetParameter` 而不检查当前值 | 状态翻转，SA 被误卸载或无法按需启动 | 先 `GetParameter` 检查当前值，仅在期望值匹配时才 `SetParameter`（见 `:95-99`） |
| `UnregisterAllByBundle` 中不遍历所有 SyncFolder | 应用多开场景下残留同步文件夹 | 遍历 `GetRootInfosByUserAndBundle()` 返回的所有文件夹（见 `:292-314`） |
| `Changestate` 不校验 bundleName | 应用 A 可修改应用 B 的文件夹状态 | 在 `Changestate` 中校验 `bundleName` 匹配（通过 `IsSyncFolderInTable` 返回的数据校验） |
| `ValidateSyncFolder` 跳过 `CheckPathPermission` | 未持久化授权的路径被注册为同步根 | 必须调用 `UfsAccessTokenHelper::CheckPathPermission(path)`（见 `:139`） |
| `Register` 中不调用 `IncreaseCnt` / `DecreaseCnt` | `IsUnused()` 判断错误，SA 可能在操作中途卸载 | 操作前后成对调用 `IncreaseCnt()` / `DecreaseCnt()` |

## 反模式/修改前检查

- 新增云盘 SA 接口必须先在 `services/IFileAccessServiceBase.idl` 中声明，由 `idl_gen_interface` 自动生成
- 新增云盘 SA 接口必须包裹在 `#ifdef SUPPORT_CLOUD_DISK_MANAGER` / `#endif` 中，否则在 `user_file_service_cloud_disk_enable = false` 时编译失败
- 新增云盘 SA 接口必须成对调用 `IncreaseCnt()` / `DecreaseCnt()` 以确保 SA 不在操作中途卸载
- 新增 `SynchronousRootManager` 方法必须通过 `IRdbAdapter` 接口操作 RDB，不允许直接使用 `NativeRdb::RdbStore`
- 修改 `SyncFolder` / `SyncFolderExt` 结构体字段必须同步更新 `Marshalling` / `ReadFromParcel` / `Unmarshalling`
- 修改 RDB 表结构（列名/索引）必须同步更新 `ufs_db_services_constants.h` 中的 SQL 常量和 `OpenCallback::CreateTable()` / `CreateUniqueIndex()`
- 新增 `NotifyWorkService::EventType` 必须在 `notify_work_service.h` 枚举中添加，并在 `NotifySyncFolderEvent()` 中处理
- 修改 `SetWorkStatus` 逻辑必须考虑 SA 按需启动条件（`5010.json` 中 `persist.clouddiskmanager.workstatus` 判断）
- 不允许在 `ValidateSyncFolder` 中省略任何一项校验（路径合法性、物理路径存在、持久化权限）
- 新增 `BundleObserver` 处理逻辑必须在 `OnReceiveEvent()` 中解析 `bundleName`、`userId`、`appIndex` 后调用 `UnregisterAllByBundle()`

## 测试指引

- 云盘服务测试：`test/unittest/cloud_disk_service_test.cpp` — 测试 `FileAccessService` 云盘接口
- 同步文件夹管理器测试：`test/unittest/cloud_disk_sync_folder_manager_test.cpp` — 测试 `SynchronousRootManager`
- 同步根管理器测试：`test/unittest/cloud_disk_synchronous_root_manager_test.cpp` — 测试 RDB 持久化
- 云盘通信测试：`test/unittest/cloud_disk_comm_test.cpp` — 测试 `SyncFolder`/`SyncFolderExt` 序列化
- 云盘 JS 管理器测试：`test/unittest/cloud_disk_js_manager_test.cpp` — 测试 `CloudDiskJSManager`
- 通知服务测试：`test/unittest/notify_work_service_test.cpp` — 测试 `NotifyWorkService`
- RDB 适配器测试：`test/unittest/ufs_rdb_adapter_test.cpp` — 测试 `RdbAdapter`
- 云盘 Fuzz 测试：`test/fuzztest/fileaccessservice*_fuzzer/` — Active/Deactive/Register/Unregister/GetAllSyncFolders 等 12 个
- RDB Mock：`test/unittest/mock/rdb_store_mock.h`
- 测试须定义 `SUPPORT_CLOUD_DISK_MANAGER`（见 `test/unittest/BUILD.gn` 中云盘测试目标）
- Token Mock：`test/fuzztest/privacy_comm/` — `UserFileServiceTokenMock::SetSaToken()` / `SetHapToken()` / `SetFileManagerToken()`
