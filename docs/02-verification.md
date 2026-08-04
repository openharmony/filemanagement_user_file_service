# user_file_service 验证与调试规则

本文记录 `user_file_service` 部件的验证环境前置条件、验证命令、日志收集方法和环境注意事项。工作流时序见 `docs/01-workflows.md`，构建测试规则见 `docs/03-build-test.md`。

## 环境前置条件

- OpenHarmony 标准系统设备（`adapted_system_type: standard`），已通过编译并烧录包含 `user_file_service` 部件的镜像
- 设备已通过 USB 连接并可用 `hdc` 命令访问
- 测试文件操作需设备上已安装 `ExternalFileManager` HAP（`com.ohos.UserFile.ExternalFileManager`）和 `medialibrary` 服务
- 测试云盘同步文件夹功能需产品配置中开启 `user_file_service_cloud_disk_enable = true`
- 测试 Picker 需设备具备屏幕（Picker 使用模态 UI 窗口）
- 测试多用户场景需设备已创建多用户（`os_account` 子系统就绪）
- 测试应用卸载清理需设备已安装含同步文件夹的应用

## 验证命令

```bash
# 查看 SA 5010 是否已注册
hdc shell hilog -t FileAccessFwk | grep -i "OnStart"
hdc shell sa_main -h | grep 5010

# 查看 file_access_service 进程状态
hdc shell ps -ef | grep file_access_service

# 手动触发 SA 按需加载（通过安装/卸载应用触发 PACKAGE_REMOVED 广播）
hdc shell bm install -p /path/to/app.hap
hdc shell bm uninstall -n com.example.testapp

# 查看系统参数（云盘工作状态）
hdc shell param get persist.clouddiskmanager.workstatus

# 设置系统参数（开启云盘工作状态以触发 SA 按需启动）
hdc shell param set persist.clouddiskmanager.workstatus true

# 查看 SA 数据库目录
hdc shell ls -la /data/service/el1/public/database/ufs_db/

# 查看 RDB 数据库文件
hdc shell ls -la /data/service/el1/public/database/ufs_db/*.db

# 通过 dump 查看服务状态（SA dump_level = 1）
hdc shell hidumper -s 5010

# 验证文件操作（通过文件管理器应用或 hilog 观察调用）
# 观察者注册/通知验证需通过应用触发文件变更后查看日志

# 检查权限声明
hdc shell bm dump -n com.ohos.UserFile.ExternalFileManager | grep -A5 permission
```

## 日志标签

| 服务/模块 | 日志标签 | 日志域 |
|-----------|----------|--------|
| FileAccessFramework（全局） | `FileAccessFwk` | `0xD00430A` |
| 服务 IDL 生成代码 | `filemanagement`（BUILD.gn `log_tag`） | `0xD00430A` |
| Hitrace 标签 | `HITRACE_TAG_FILEMANAGEMENT` | — |

## 日志收集

```bash
# 收集 FileAccessFwk 相关日志（实时）
hdc shell hilog -t FileAccessFwk

# 收集日志并过滤特定操作
hdc shell hilog -t FileAccessFwk | grep -i "RegisterNotify\|OnChange\|ConnectExtension"

# 收集云盘同步文件夹相关日志
hdc shell hilog -t FileAccessFwk | grep -i "Register\|Unregister\|Active\|Deactive\|SyncFolder"

# 收集 SA 生命周期日志
hdc shell hilog -t FileAccessFwk | grep -i "OnStart\|OnStop\|OnAddSystemAbility\|UnloadSA\|IsUnused"

# 收集扩展连接日志
hdc shell hilog -t FileAccessFwk | grep -i "ConnectFileExtAbility\|GetExtensionProxy\|OnAbilityConnectDone"

# 导出完整日志到文件
hdc shell hilog > ufs_hilog.txt

# Hitrace 抓取
hdc shell hitrace --trace_clock mono --buffer_size 2048 --overwrite HITRACE_TAG_FILEMANAGEMENT > ufs_trace.ftrace
```

## 环境注意事项

1. **SA 按需启动**：`file_access_service` SA 默认按需启动（`ondemand: true`），仅在收到 `PACKAGE_REMOVED` 广播且 `persist.clouddiskmanager.workstatus` 为 `true` 时启动。验证文件操作需先通过应用调用触发 SA 加载。

2. **SA 空闲卸载**：SA 在无观察者、无 App 代理连接、无同步文件夹操作时，30 秒后自动卸载。验证长流程操作时注意 SA 可能在中间卸载。查看 `IsUnused` 日志确认卸载判断。

3. **云盘功能默认关闭**：`user_file_service_cloud_disk_enable` 默认为 `false`。云盘相关 API（Register/Unregister/Active/Deactive/GetSyncFolders/GetAllSyncFolders）返回 `E_NOT_SUPPORT`（801）。需在产品配置中开启。

4. **扩展能力依赖**：文件操作依赖 `ExternalFileManager` HAP 和 `medialibrary` 服务已安装。若未安装，`ConnectExtension()` 返回 `nullptr`，操作返回 `E_CONNECT`。

5. **数据库目录权限**：SA 运行在 `ufs:ufs` 用户下，数据库路径 `/data/service/el1/public/database/ufs_db/`。若目录不存在或权限不正确，`SynchronousRootManager::Init()` 失败。初始化脚本见 `services/file_access_service.cfg`。

6. **广播注册时机**：`BundleObserver` 在 `COMMON_EVENT_SERVICE_ID` 就绪后通过 `OnAddSystemAbility` 回调中注册。若 `CES` 未就绪时 SA 启动，广播注册会延迟到 `CES` 就绪。查看 `RegisterBundleBroadcast` 日志确认。

7. **多用户场景**：云盘同步文件夹操作使用 `UfsAccessTokenHelper::GetUserId()` 获取当前活跃用户 ID。切换用户后需确认 SA 使用的是正确的 userId。`GetCurrentUserId()` 通过 `OsAccountManager::QueryActiveOsAccountIds()` 获取。

8. **URI 格式约定**：
   - 设备根 URI：`file://docs`
   - 外置存储根 URI：`file://docs/storage/External`
   - 媒体文件 URI：`file://media/...`（alias `media` → bundleName `com.ohos.medialibrary.medialibrarydata`）
   - 文档文件 URI：`file://docs/...`（alias `docs` → bundleName `com.ohos.UserFile.ExternalFileManager`）

9. **共享内存限制**：`ListFile` 使用共享内存批量传输 `FileInfo`，默认容量 200KB，最大 2MB（`DEFAULT_CAPACITY_200KB` / `MAX_CAPACITY_2MB`）。超出上限时分批返回，通过 `isOver` 标志判断。

10. **Full Mount 模式**：`user_access_common_utils.h` 中 `IsFullMountEnable()` 读取系统参数 `const.filemanager.full_mount.enable` 判断是否为全挂载模式，影响文件路径解析行为。
