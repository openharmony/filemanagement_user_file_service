# user_file_service 构建、测试与验证规则

本文记录 `user_file_service` 部件的构建入口、构建目标、测试目标和变更验证要求。工作流时序见 `docs/01-workflows.md`，验证调试命令见 `docs/02-verification.md`。

## 构建入口

构建命令须从 OpenHarmony 源码根目录执行。构建系统为 GN + Ninja，通过 `./build.sh` 驱动。

- 构建系统配置文件：`bundle.json`（部件描述）、`BUILD.gn`（顶层入口）、`filemanagement_aafwk.gni`（路径和 Feature Flag 定义）
- 部件路径：`foundation/filemanagement/user_file_service`
- 部件名：`user_file_service`，子系统：`filemanagement`

## 顶层构建目标

```bash
# 构建整个部件
./build.sh --product-name {product} --build-target user_file_service

# SA 服务共享库（libfile_access_service.z.so）
./build.sh --product-name {product} --build-target file_access_service

# SA 配置和参数文件
./build.sh --product-name {product} --build-target file_access_service.cfg
./build.sh --product-name {product} --build-target file_access_service_profile

# inner_api: 文件访问扩展能力套件
./build.sh --product-name {product} --build-target file_access_extension_ability_kit
./build.sh --product-name {product} --build-target file_access_extension_ability_module

# NAPI 模块
./build.sh --product-name {product} --build-target fileaccess
./build.sh --product-name {product} --build-target fileaccessextensionability_napi
./build.sh --product-name {product} --build-target fileextensioninfo

# Kits 公共 API 模块
./build.sh --product-name {product} --build-target picker
./build.sh --product-name {product} --build-target cj_picker_ffi
./build.sh --product-name {product} --build-target recent
./build.sh --product-name {product} --build-target trash
./build.sh --product-name {product} --build-target clouddiskmanager

# Taihe (ANI) 模块
./build.sh --product-name {product} --build-target afs_picker_taihe
./build.sh --product-name {product} --build-target cloudmanager_taihe

# ExternalFileManager HAP
./build.sh --product-name {product} --build-target user_file_manager_hap
```

## 测试目标

### 单元测试

```bash
# 构建全部单元测试（14 个测试目标）
./build.sh --product-name {product} --build-target user_file_service_unit_test

# 单独构建关键测试
./build.sh --product-name {product} --build-target external_file_access_basic_test
./build.sh --product-name {product} --build-target external_file_access_management_test
./build.sh --product-name {product} --build-target external_file_access_notify_test
./build.sh --product-name {product} --build-target file_access_ext_stub_impl_test
./build.sh --product-name {product} --build-target js_file_access_ext_ability_test
./build.sh --product-name {product} --build-target cloud_disk_service_test
./build.sh --product-name {product} --build-target cloud_disk_sync_folder_manager_test
./build.sh --product-name {product} --build-target ufs_rdb_adapter_test
```

单元测试列表（`test/unittest/BUILD.gn`）：

| 测试目标 | 测试内容 | 关键源文件 |
|----------|----------|------------|
| `external_file_access_basic_test` | 基础文件操作（OpenFile/CreateFile/Mkdir/Delete/Move/Query） | `external_file_access_test_basic.cpp`、`external_file_access_test_info.cpp` |
| `external_file_access_management_test` | 管理/操作（Copy/Rename/ListFile/ScanFile/Access/GetRoots） | `external_file_access_test_management.cpp`、`external_file_access_test_operations.cpp` |
| `external_file_access_notify_test` | 观察者注册/注销/通知分发 | `external_notify_test.cpp` |
| `abnormal_file_access_test` | 异常路径和边界用例 | `abnormal_file_access_test.cpp` |
| `file_access_ext_stub_impl_test` | `FileAccessExtStubImpl` IPC stub | `file_access_ext_stub_impl_test.cpp`、`file_access_ext_stub_impl_other_test.cpp` |
| `js_file_access_ext_ability_test` | JS `FileAccessExtAbility` 生命周期和方法 | `js_file_access_ext_ability_test.cpp` 等 5 个文件 |
| `ufs_rdb_adapter_test` | RDB 适配器 CRUD | `ufs_rdb_adapter_test.cpp` |
| `urie_test` | URI 扩展解析 | `urie_test.cpp` |
| `cloud_disk_js_manager_test` | 云盘 JS 管理器 | `cloud_disk_js_manager_test.cpp` |
| `cloud_disk_sync_folder_manager_test` | 同步文件夹管理器 | `cloud_disk_sync_folder_manager_test.cpp` |
| `cloud_disk_service_test` | 云盘服务接口 | `cloud_disk_service_test.cpp` |
| `notify_work_service_test` | 通知工作服务 | `notify_work_service_test.cpp` |
| `cloud_disk_comm_test` | `SyncFolder`/`SyncFolderExt` 序列化 | `cloud_disk_comm_test.cpp` |
| `cloud_disk_synchronous_root_manager_test` | `SynchronousRootManager` RDB 操作 | `cloud_disk_synchronous_root_manager_test.cpp` |

### Fuzz 测试

```bash
# 构建全部 fuzz 测试（64 个目标）
./build.sh --product-name {product} --build-target user_file_service_fuzz_test
```

Fuzz 测试覆盖三个 IPC 接面（`test/fuzztest/BUILD.gn`）：

| 分类 | 数量 | 测试范围 |
|------|------|----------|
| `externalfileaccess*_fuzzer` | 24 | `FileAccessExtBaseStub` IPC 命令（Access/Create/Delete/Mkdir/Move/OpenFile/Rename/ScanFile 等） |
| `fileaccessext*_fuzzer` | 26 | `FileAccessExtAbility`/Connection/Proxy（ConnectFileExtAbility/Copy/CopyFile/CreateFile 等） |
| `fileaccessservice*_fuzzer` | 12 | `FileAccessService` SA 接口（Active/Deactive/Register/Unregister/GetAllSyncFolders 等） |
| `fileinfosharedmemory*_fuzzer` | 6 | 共享内存操作（Create/Expand/Marshalling/Write） |
| `useraccess*_fuzzer` | 2 | 用户访问工具（GetUserName/IsFullMount） |

## 变更验证场景分类

| 变更类型 | 验证要求 | 最低证据 |
|----------|----------|----------|
| 修改 SA 服务逻辑（OnStart/Init/Timer/Unload） | 编译通过 + SA 生命周期单测 + 板端 SA 启动日志 | `file_access_service_test` 通过 + `hilog` 日志显示 `OnStart` |
| 修改 IDL 接口或接口码 | 编译通过 + stub/proxy 一致性 + 全部相关单测 + fuzz 测试 | `file_access_ext_stub_impl_test` + 对应 fuzzer 通过 |
| 修改文件操作逻辑（OpenFile/CreateFile 等） | 编译通过 + 基础/管理单测 + 板端文件操作验证 | `external_file_access_basic_test` + `external_file_access_management_test` 通过 |
| 修改观察者/通知逻辑 | 编译通过 + 通知单测 + 板端通知验证 | `external_file_access_notify_test` 通过 |
| 修改云盘同步文件夹逻辑 | 编译通过（开启 cloud_disk_enable）+ 云盘单测 + 板端注册/注销验证 | `cloud_disk_service_test` + `cloud_disk_sync_folder_manager_test` 通过 |
| 修改 RDB 持久化层 | 编译通过 + RDB 适配器单测 | `ufs_rdb_adapter_test` + `cloud_disk_synchronous_root_manager_test` 通过 |
| 修改 NAPI 模块（picker/recent/trash/fileaccess） | 编译通过 + 对应 NAPI 单测 | 对应 `*_test` 通过 |
| 修改 Feature Flag | 编译两种配置（开/关）均通过 | 两种 `./build.sh` 均成功 |
| 修改 BUILD.gn 或构建配置 | 编译通过 + 确认产物列表正确 | 产物 `.so`/`.hap` 文件存在 |

## 最低检查要求

每种变更类型在提交前须满足的最低检查：

- **SA 服务变更**：`file_access_service_test` + `notify_work_service_test` + `hdc shell hilog -t FileAccessFwk | grep OnStart`
- **IDL/接口码变更**：所有 fuzz 测试 + `file_access_ext_stub_impl_test` + `js_file_access_ext_ability_test`
- **文件操作变更**：`external_file_access_basic_test` + `external_file_access_management_test` + 板端通过文件管理器执行操作
- **观察者变更**：`external_file_access_notify_test` + 板端注册观察者后修改文件观察通知
- **云盘变更**：`cloud_disk_service_test` + `cloud_disk_sync_folder_manager_test` + `cloud_disk_synchronous_root_manager_test`（均需 `SUPPORT_CLOUD_DISK_MANAGER` 定义）
- **构建变更**：`./build.sh --build-target user_file_service` 成功

## 板端验证

### 环境要求

| 项目 | 要求 |
|------|------|
| 设备 | OpenHarmony 标准系统，已烧录包含本部件镜像 |
| 连接 | USB 连接，`hdc` 可用 |
| 服务 | `ExternalFileManager` HAP + `medialibrary` 服务已安装 |
| 云盘 | `user_file_service_cloud_disk_enable = true`（仅云盘验证需要） |
| 参数 | `persist.clouddiskmanager.workstatus` 为 `true`（触发 SA 按需启动） |
| 用户 | 至少一个活跃用户（`OsAccountManager` 可查询） |

### 板端验证命令

```bash
# 验证 SA 启动
hdc shell hilog -t FileAccessFwk | grep "OnStart"

# 验证文件操作
# 1. 通过文件管理器应用打开/创建/删除文件
# 2. 查看日志确认调用链
hdc shell hilog -t FileAccessFwk | grep "OpenFile\|CreateFile\|Delete"

# 验证观察者
# 1. 应用注册观察者
# 2. 修改被监听文件
# 3. 查看通知日志
hdc shell hilog -t FileAccessFwk | grep "RegisterNotify\|OnChange\|SendListNotify"

# 验证云盘同步文件夹
hdc shell param set persist.clouddiskmanager.workstatus true
# 应用调用 Register/Unregister/Active/Deactive API
hdc shell hilog -t FileAccessFwk | grep "Register\|Unregister\|Active\|Deactive\|SyncFolder"

# 验证应用卸载清理
hdc shell bm uninstall -n {bundle_name}
hdc shell hilog -t FileAccessFwk | grep "UnregisterAllByBundle\|PACKAGE_REMOVED"

# SA Dump
hdc shell hidumper -s 5010
```

### 关键日志关键字

| 场景 | 日志关键字 | 含义 |
|------|----------|------|
| SA 启动 | `OnStart`、`register to system ability manager` | SA 生命周期 |
| SA 卸载 | `UnloadSA`、`IsUnused` | 空闲卸载判断 |
| 扩展连接 | `ConnectFileExtAbility`、`OnAbilityConnectDone` | 扩展能力连接 |
| 观察者 | `RegisterNotifyImpl`、`SendListNotify`、`OnChange` | 观察者注册和通知 |
| 云盘注册 | `Register begin`、`PutSynchronousRoot`、`RegisterSyncFolder` | 同步文件夹注册 |
| 云盘注销 | `Unregister begin`、`DeleteSynchronousRoot`、`UnregisterSyncFolder` | 同步文件夹注销 |
| 应用卸载清理 | `UnregisterAllByBundle`、`HandleBundleBroadcast` | 包移除清理 |
| 权限错误 | `permission error`、`CheckCallingPermission have no fileAccess permission` | 权限校验失败 |

### PR 证据模板

```
## 验证证据

### 编译
- [ ] `./build.sh --product-name {product} --build-target {target}` 成功

### 单元测试
- [ ] `{test_target}` 通过（附测试报告）

### Fuzz 测试（如涉及 IPC 接口）
- [ ] `{fuzz_target}` 通过（附运行日志）

### 板端验证（如涉及功能变更）
- 设备型号：{device}
- 验证步骤：
  1. {step1}
  2. {step2}
- 日志证据：
  ```
  {hilog_output}
  ```

### Feature Flag 验证（如涉及条件编译）
- [ ] `user_file_service_cloud_disk_enable=true` 编译通过
- [ ] `user_file_service_cloud_disk_enable=false` 编译通过
```

## Feature Flag

| Flag | 默认值 | 说明 |
|------|--------|------|
| `user_file_service_cloud_disk_enable` | `false` | 云盘同步文件夹功能开关。开启时定义 `SUPPORT_CLOUD_DISK_MANAGER`，编译云盘相关代码（Register/Unregister/Active/Deactive/GetSyncFolders/GetAllSyncFolders 等），并依赖 `dfs_service:clouddiskservice_kit_inner`。定义在 `filemanagement_aafwk.gni:24` |
| `picker_udmf_enabled` | `true` | Picker UDMF 集成开关。当 `distributeddatamgr_udmf` 部件存在时为 `true`，控制 Picker 模块是否支持 UDMF 数据插入。定义在 `filemanagement_aafwk.gni:20` |
| `ufs_sandbox_manarer` | `false` | 沙箱管理器集成开关。当 `accesscontrol_sandbox_manager` 部件存在时为 `true`，定义 `SANDBOX_MANAGER` 和 `ABILITY_RUNTIME_FEATURE_SANDBOXMANAGER`，依赖 `sandbox_manager:libsandbox_manager_sdk` 和 `ability_runtime:uri_permission_mgr`。定义在 `filemanagement_aafwk.gni:25-29` |

## 常见误用

| 误用 | 后果 | 修正 |
|------|------|------|
| 在 `file_access_service` 子目录直接运行 `./build.sh` | 找不到 GN 配置，编译失败 | 须从 OpenHarmony 源码根目录执行 `./build.sh` |
| 开启 `user_file_service_cloud_disk_enable` 后未同步开启 `dfs_service` 部件 | 编译失败，找不到 `clouddiskservice_kit_inner` 依赖 | 产品配置中同时启用 `dfs_service` 部件 |
| 修改 IDL 后未重新生成 stub/proxy | IPC 编解码不匹配，`E_IPCS` 错误 | 执行 `./build.sh` 时 `idl_gen_interface` 会自动重新生成 |
| 在 NAPI 模块中直接调用 SA IPC | 绕过 `FileAccessHelper`，权限校验和连接管理缺失 | 通过 `FileAccessHelper::Creator()` 创建 helper 后操作 |
| 单元测试中未设置 `defines = ["private=public"]` | 无法访问类的 private 成员 | 在测试 BUILD.gn 中添加 `defines = ["private=public"]` |
| 云盘单测未定义 `SUPPORT_CLOUD_DISK_MANAGER` | 云盘代码被条件编译排除，测试覆盖率不足 | 在测试 BUILD.gn 中添加 `defines = ["SUPPORT_CLOUD_DISK_MANAGER"]` |
| Fuzz 测试未使用 `UserFileServiceTokenMock` 设置 Token | 权限校验失败，Fuzz 无法进入目标代码 | 在 Fuzz 入口调用 `UserFileServiceTokenMock::SetSaToken()` 等 |
| 修改 `ExternalFileManager` HAP 后未使用正确签名安装 | HAP 安装失败，签名不匹配 | 使用 `services/signature/fileextension.p7b` 签名 |
| 测试中未设置 `use_exceptions = true` | NAPI/异常相关测试编译失败 | 在测试 BUILD.gn 中添加 `use_exceptions = true` |

## 完成定义

一个变更视为完成（Definition of Done），须满足以下全部条件：

1. 代码编译通过（两种 Feature Flag 配置均通过，如涉及条件编译）
2. 相关单元测试全部通过
3. 相关 Fuzz 测试全部通过（如涉及 IPC 接口）
4. 安全 sanitize 检查通过（`cfi`、`integer_overflow`、`ubsan`、`boundary_sanitize`）
5. 板端验证通过（如涉及功能变更）
6. 日志隐私合规（敏感信息使用 `%{private}`）
7. 无新增禁止关键字（见 Knowledge-SKILL 合规检查列表中的禁用词清单，须通过 `grep` 扫描零输出）
8. 代码锚点（类名/方法名/路径）与源码一致
9. PR 包含验证证据（编译/测试/板端日志）
