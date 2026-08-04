# 扩展能力知识

本文只记录 FileAccessExtensionAbility 扩展能力模型、IFileAccessExtBase 接口、FileAccessHelper 客户端、Stub/Impl 实现、共享内存机制和文件操作 API。SA 服务架构见 `service-architecture.md`，云盘同步文件夹见 `cloud-disk-sync.md`。

## 扩展能力模型

`user_file_service` 不直接操作文件系统，而是通过 `FileAccessExtensionAbility` 扩展能力转发操作。扩展能力由各文件管理服务实现：

| 扩展实现 | BundleName | URI Alias | 实现路径 |
|----------|-----------|-----------|----------|
| ExternalFileManager | `com.ohos.UserFile.ExternalFileManager` | `docs` | `services/file_extension_hap/entry/src/main/ets/FileExtensionAbility/FileExtensionAbility.ts` |
| medialibrary | `com.ohos.medialibrary.medialibrarydata` | `media` | 外部仓（`multimedia_medialibrary_standard`） |

## 扩展能力调用链

| 阶段 | 操作 | 代码锚点 |
|------|------|----------|
| 1. 应用调用 | JS API（如 `fileAccess.openFile(uri, flags)`） | `frameworks/js/napi/file_access_module/napi_fileaccess_helper.h` `NAPI_OpenFile` |
| 2. NAPI 转发 | NAPI 模块调用 `FileAccessHelper::OpenFile()` | `interfaces/inner_api/file_access/include/file_access_helper.h:73` |
| 3. 获取代理 | `FileAccessHelper::GetProxyByUri(uri)` 获取对应扩展代理 | `file_access_helper.h:94` |
| 4. IPC 调用 | `IFileAccessExtBase::OpenFile()`（ipccode 1） | `interfaces/inner_api/file_access/IFileAccessExtBase.idl:23` |
| 5. Stub 接收 | `FileAccessExtStubImpl::OpenFile()` 接收 IPC 请求 | `interfaces/inner_api/file_access/include/file_access_ext_stub_impl.h:39` |
| 6. 调用实现 | `FileAccessExtAbility::OpenFile(uri, flags, fd)` | `interfaces/inner_api/file_access/include/file_access_ext_ability.h:44` |
| 7. JS 执行 | `ExternalFileManager` 的 `openFile()` 方法 | `services/file_extension_hap/.../FileExtensionAbility.ts` |

## IFileAccessExtBase IDL 接口码

```java
interface OHOS.FileAccessFwk.IFileAccessExtBase {
    [ipccode 1]  void OpenFile(Urie uri, int flags, FileDescriptor fd);
    [ipccode 2]  void CreateFile(Urie parent, String displayName, Urie newFile);
    [ipccode 3]  void Mkdir(Urie parent, String displayName, Urie newFile);
    [ipccode 4]  void Delete(Urie sourceFile);
    [ipccode 5]  void Move(Urie sourceFile, Urie targetParent, Urie newFile);
    [ipccode 6]  void Copy(Urie sourceUri, Urie destUri, Result[] copyResult, int retCode, boolean force);
    [ipccode 7]  void Rename(Urie sourceFile, String displayName, Urie newFile);
    [ipccode 8]  void ListFile(FileInfo fileInfo, long offset, FileFilter filter, SharedMemoryInfo meminfo);
    [ipccode 9]  void ScanFile(FileInfo fileInfo, long offset, long maxCount, FileFilter filter, FileInfo[] fileInfoVec);
    [ipccode 10] void StartWatcher(Urie uri);
    [ipccode 11] void StopWatcher(Urie uri);
    [ipccode 12] void Query(Urie uri, List<String> columns, List<String> results);
    [ipccode 13] void GetRoots(List<RootInfo> rootInfoVec);
    [ipccode 14] void Access(Urie uri, boolean isExist);
    [ipccode 15] void GetFileInfoFromUri(Urie selectFile, FileInfo fileInfo);
    [ipccode 16] void GetFileInfoFromRelativePath(String selectFile, FileInfo fileInfo);
    [ipccode 17] void CopyFile(Urie sourceUri, Urie destUri, String fileName, Urie newFileUri);
    [ipccode 18] void MoveItem(Urie sourceFile, Urie targetParent, Result[] moveResult, int retCode, boolean force);
    [ipccode 19] void MoveFile(Urie sourceFile, Urie targetParent, String fileName, Urie newFile);
}
```

来源：`interfaces/inner_api/file_access/IFileAccessExtBase.idl:22`

## 类职责表

| 类 | 职责 | 路径 |
|----|------|------|
| `FileAccessHelper` | 客户端门面，管理扩展连接，提供文件操作 API（OpenFile/CreateFile/Delete/Move/Copy/Rename/ListFile/ScanFile/Query/Access/RegisterNotify 等） | `interfaces/inner_api/file_access/include/file_access_helper.h:56` |
| `FileAccessExtAbility` | 扩展能力 C++ 基类，继承 `ExtensionBase<>`，定义虚函数接口供 JS 实现 | `interfaces/inner_api/file_access/include/file_access_ext_ability.h:32` |
| `FileAccessExtStubImpl` | IPC stub 实现，转发 IPC 请求到 `FileAccessExtAbility` | `interfaces/inner_api/file_access/include/file_access_ext_stub_impl.h:31` |
| `AppFileAccessExtConnection` | 应用侧扩展连接器（继承 `AbilityConnectionStub`），管理连接和死亡监听 | `interfaces/inner_api/file_access/include/app_file_access_ext_connection.h:31` |
| `FileAccessExtConnection` | SA 内部扩展连接器 | `services/native/file_access_service/include/file_access_ext_connection.h:35` |
| `Urie` | URI 扩展类（继承 `OHOS::Uri`），支持 IPC 序列化 | `interfaces/inner_api/file_access/include/uri_ext.h:42` |
| `SharedMemoryInfo` | 共享内存信息载体，用于 `ListFile` 批量传输 | `interfaces/kits/js/src/common/file_info_shared_memory.h` |
| `SharedMemoryOperation` | 共享内存操作工具类（创建/扩展/销毁/读写） | 同上 |
| `FileInfo` | 文件信息结构体（uri/relativePath/fileName/mode/size/mtime/mimeType） | `interfaces/inner_api/file_access/include/file_access_extension_info.h:99` |
| `RootInfo` | 设备根信息结构体（deviceType/uri/relativePath/displayName/deviceFlags） | `file_access_extension_info.h:167` |
| `ConnectExtensionInfo` | 连接扩展信息（Want + token） | `file_access_extension_info.h:303` |
| `Result` | 文件操作结果（sourceUri/destUri/errCode/errMsg） | `file_access_extension_info.h:242` |
| `FileFilter` | 文件过滤器（suffix/displayName/mimeType/fileSizeOver/lastModifiedAfter/excludeMedia） | `interfaces/kits/js/src/common/file_filter.h` |

## FileAccessHelper 创建方式

| 方法 | 签名 | 用途 |
|------|------|------|
| Creator（全量连接） | `Creator(context) → pair<helper, int>` | 连接所有已注册的文件访问扩展能力 | `file_access_helper.h:62` |
| Creator（指定 Want） | `Creator(context, wants) → pair<helper, int>` | 连接指定扩展能力 | `file_access_helper.h:65` |
| Creator（Token） | `Creator(token, wants) → helper` | 以 Token 方式连接（ExtensionAbility 内部使用） | `file_access_helper.h:67` |

## 文件操作 API 表

| 操作 | FileAccessHelper 方法 | IFileAccessExtBase ipccode | FileAccessExtAbility 虚函数 |
|------|----------------------|---------------------------|---------------------------|
| 打开文件 | `OpenFile(uri, flags, fd)` | 1 | `OpenFile(uri, flags, fd)` |
| 创建文件 | `CreateFile(parent, displayName, newFile)` | 2 | `CreateFile(parent, displayName, newFile)` |
| 创建目录 | `Mkdir(parent, displayName, newDir)` | 3 | `Mkdir(parent, displayName, newFile)` |
| 删除 | `Delete(selectFile)` | 4 | `Delete(sourceFile)` |
| 移动 | `Move(sourceFile, targetParent, newFile)` | 5 | `Move(sourceFile, targetParent, newFile)` |
| 复制 | `Copy(sourceUri, destUri, copyResult, force)` | 6 | `Copy(sourceUri, destUri, copyResult, force)` |
| 复制文件 | `CopyFile(sourceUri, destUri, fileName, newFileUri)` | 17 | `CopyFile(sourceUri, destUri, fileName, newFileUri)` |
| 重命名 | `Rename(sourceFile, displayName, newFile)` | 7 | `Rename(sourceFile, displayName, newFile)` |
| 列举文件 | `ListFile(fileInfo, offset, filter, memInfo)` | 8 | `ListFile(fileInfo, offset, maxCount, filter, fileInfoVec)` |
| 扫描文件 | `ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec)` | 9 | `ScanFile(fileInfo, offset, maxCount, filter, fileInfoVec)` |
| 查询 | `Query(uri, metaJson)` | 12 | `Query(uri, columns, results)` |
| 获取根 | `GetRoots(rootInfoVec)` | 13 | `GetRoots(rootInfoVec)` |
| 访问检查 | `Access(uri, isExist)` | 14 | `Access(uri, isExist)` |
| URI 获取信息 | `GetFileInfoFromUri(selectFile, fileInfo)` | 15 | `GetFileInfoFromUri(selectFile, fileInfo)` |
| 相对路径获取信息 | `GetFileInfoFromRelativePath(selectFile, fileInfo)` | 16 | `GetFileInfoFromRelativePath(selectFile, fileInfo)` |
| 开始监听 | — | 10 | `StartWatcher(uri)` |
| 停止监听 | — | 11 | `StopWatcher(uri)` |
| 移动项 | `MoveItem(sourceFile, targetParent, moveResult, force)` | 18 | `MoveItem(sourceFile, targetParent, moveResult, force)` |
| 移动文件 | `MoveFile(sourceFile, targetParent, fileName, newFile)` | 19 | `MoveFile(sourceFile, targetParent, fileName, newFile)` |

## NAPI 模块表

| NAPI 模块名 | 安装路径 | 源码路径 | 说明 |
|-------------|----------|----------|------|
| `file.fileAccess` | `module/file` | `frameworks/js/napi/file_access_module/` | 文件访问 JS API（OpenFile/CreateFile/Delete/Move/Copy/Rename/ListFile/Query/Access/RegisterObserver 等） |
| `application.FileAccessExtensionAbility` | `module/application` | `frameworks/js/napi/file_access_ext_ability/` | 扩展能力基类，供开发者继承 |
| `file.fileExtensionInfo` | `module/file` | `frameworks/js/napi/file_extension_info_module/` | 枚举和常量（DeviceType/DeviceFlag/DocumentFlag/NotifyType 等） |
| `file.picker` | `module/file` | `interfaces/kits/picker/` | 文件选择器（PhotoViewPicker/DocumentViewPicker/AudioViewPicker） |
| `file.recent` | `module/file` | `interfaces/kits/native/recent/` | 最近文件管理（AddRecentFile/RemoveRecentFile/ListRecentFile） |
| `file.trash` | `module/file` | `interfaces/kits/native/trash/` | 回收站（ListFile/Recover/CompletelyDelete） |
| `file.cloudDiskManager` | `module/file` | `interfaces/kits/native/clouddiskmanager/` | 云盘管理（GetAllSyncFolders） |

## 共享内存参数表

| 参数 | 值 | 说明 | 代码锚点 |
|------|-----|------|----------|
| `DEFAULT_CAPACITY_200KB` | `200 * 1024` | 默认共享内存容量 | `interfaces/kits/js/src/common/file_info_shared_memory.h` |
| `MAX_CAPACITY_2MB` | `2 * 1024 * 1024` | 最大共享内存容量 | 同上 |
| `MAX_COUNTNUM` | `1000` | `NotifyMessage` 最大 URI 数量 | `interfaces/inner_api/file_access/include/file_access_observer_common.h:28` |
| `NOTIFY_MAX_NUM` | `32` | 单批通知最大 URI 数量 | `services/native/file_access_service/src/file_access_service.cpp:44` |
| `NOTIFY_TIME_INTERVAL` | `500`（ms） | 批量通知定时器间隔 | `file_access_service.cpp:45` |
| `MAX_WAIT_TIME` | `20` | 批量通知最大等待次数 | `file_access_service.cpp:46` |
| `UNLOAD_SA_WAIT_TIME` | `30` | SA 空闲卸载等待秒数 | `file_access_service.cpp:48` |

## 文件标志枚举

```cpp
// 设备能力标志
const uint32_t DEVICE_FLAG_SUPPORTS_READ = 1;
const uint32_t DEVICE_FLAG_SUPPORTS_WRITE = 1 << 1;

// 文件/目录能力标志
const uint32_t DOCUMENT_FLAG_REPRESENTS_FILE = 1;
const uint32_t DOCUMENT_FLAG_REPRESENTS_DIR = 1 << 1;
const uint32_t DOCUMENT_FLAG_SUPPORTS_READ = 1 << 2;
const uint32_t DOCUMENT_FLAG_SUPPORTS_WRITE = 1 << 3;

// 文件描述符打开模式
constexpr int32_t READ = 0;
constexpr int32_t WRITE = 1;
constexpr int32_t WRITE_READ = 2;
```

来源：`interfaces/inner_api/file_access/include/file_access_extension_info.h:60-76`

## 文件属性列名表

| 列名 | 常量 | 类型 | 代码锚点 |
|------|------|------|----------|
| 显示名称 | `DISPLAY_NAME` | STRING | `file_access_extension_info.h:33` |
| 相对路径 | `RELATIVE_PATH` | STRING | `:34` |
| 文件大小 | `FILE_SIZE` | INT64 | `:35` |
| 修改时间 | `DATE_MODIFIED` | INT64 | `:36` |
| 添加时间 | `DATE_ADDED` | INT64 | `:37` |
| 图片高度 | `HEIGHT` | INT32 | `:39` |
| 图片宽度 | `WIDTH` | INT32 | `:40` |
| 媒体时长 | `DURATION` | INT32 | `:42` |

## URI 路径格式表

| 路径格式 | 说明 | 示例 |
|----------|------|------|
| `file://docs` | 所有设备根 URI | `file://docs` |
| `file://docs/storage/External` | 外置存储根 URI | `file://docs/storage/External` |
| `file://media/...` | 媒体文件路径 | `file://media/Photo/IMG_001.jpg` |
| `file://docs/...` | 文档文件路径 | `file://docs/storage/External/Documents/test.txt` |

## 线程安全表（扩展连接）

| 锁 | 保护对象 | 位置 |
|----|----------|------|
| `connectLockInfo_.mutex`（FileAccessExtConnection） | 连接条件变量 `isReady` | `file_access_ext_connection.h:51` |
| `connectLockInfo_.mutex`（AppFileAccessExtConnection） | 连接条件变量 `isReady` | `app_file_access_ext_connection.h:48` |
| `mutex_`（静态，两个 Connection 类） | 连接操作互斥 | `file_access_ext_connection.h:56`、`app_file_access_ext_connection.h:54` |
| `deathRecipientMutex_`（AppFileAccessExtConnection） | `callerDeathRecipient_` | `app_file_access_ext_connection.h:57` |
| `proxyMutex_`（AppFileAccessExtConnection） | `fileExtProxy_` | `app_file_access_ext_connection.h:58` |

## 常见误用

| 误用 | 后果 | 修正 |
|------|------|------|
| 修改 IDL `[ipccode N]` 编号但不更新 `FileAccessExtStubImpl` | IPC stub 分发到错误方法 | 同步更新 `file_access_ext_stub_impl.h` 中所有方法声明 |
| 新增 IDL 接口但不更新 JS 扩展基类 `file_access_ext_ability.js` | JS 扩展无默认实现，调用报错 | 在 `file_access_ext_ability.js` 中添加默认 stub |
| `FileAccessHelper::Creator()` 后不调用 `Release()` | 扩展连接泄露 | 使用 `shared_ptr` 管理 helper 生命周期，析构自动 `Release()` |
| `ListFile` 中共享内存超 2MB 未分批 | 内存分配失败或截断 | 使用 `SharedMemoryOperation::ExpandSharedMemory` 扩容，超上限时通过 `isOver` 分批 |
| 修改 `FileInfo` 结构体字段但不更新 `Marshalling`/`Unmarshalling` | IPC 序列化数据不匹配 | 同步更新 `ReadFromParcel` 和 `Marshalling` 方法 |
| 在 `FileAccessExtStubImpl` 中不做 `CheckCallingPermission` | 无权限进程可调用扩展 | 每个 stub 方法入口检查权限 |
| 新增 NAPI 方法不在 `native_fileaccess_module.cpp::Init()` 中注册 | JS 侧找不到方法 | 在 `Init()` 中调用注册函数 |
| `file_access_ext_ability.js` 中默认返回 `ERR_ERROR` 而未在子类覆写 | 扩展能力操作返回错误 | 开发者须在子类覆写需要的方法 |

## 反模式/修改前检查

- 新增文件操作接口必须先在 `IFileAccessExtBase.idl` 中声明 `[ipccode N]`，编号须连续且不与现有编号冲突
- 新增接口后必须同步更新链路上所有 6 处：Stub（IDL 生成）、StubImpl、ExtAbility 基类、JS 基类、ExternalFileManager HAP、测试
- 修改 `FileInfo`/`RootInfo`/`Result`/`ConnectExtensionInfo` 等 Parcelable 类须同步更新 `Marshalling` 和 `Unmarshalling`
- 新增 NAPI 模块须在 `bundle.json` 的 `fwk_group` 或 `service_group` 中添加构建目标
- 不允许在 `FileAccessHelper` 中直接持有 `IFileAccessExtBase` 裸指针，须通过 `sptr` 管理
- `FileAccessExtAbility::Create()` 根据 `Runtime` 类型创建 JS 或 C++ 实例，新增运行时类型须在此处处理
- 共享内存操作须配对（`CreateSharedMemory` → `MapSharedMemory` → `WriteFileInfos` → `DestroySharedMemory`），泄露会导致 fd 耗尽
- `Urie` 类用于扩展间 IPC 传输 URI（支持序列化），与 `OHOS::Uri` 之间通过 `ConvertToUri()` 转换

## 测试指引

- 扩展 stub 测试：`test/unittest/file_access_ext_stub_impl_test.cpp`、`file_access_ext_stub_impl_other_test.cpp` — 测试 `FileAccessExtStubImpl`
- JS 扩展能力测试：`test/unittest/js_file_access_ext_ability_test.cpp` 等 5 个测试文件 — 测试 `FileAccessExtAbility`
- 文件操作测试：`test/unittest/external_file_access_test_basic.cpp`、`external_file_access_test_management.cpp`、`external_file_access_test_operations.cpp` — 测试 `FileAccessHelper`
- 通知测试：`test/unittest/external_notify_test.cpp` — 测试观察者
- URI 测试：`test/unittest/urie_test.cpp` — 测试 `Urie`
- Mock 文件：`test/unittest/mock/file_access_ext_ability_mock.h`、`file_access_service_mock.h`
- Fuzz 测试：`test/fuzztest/externalfileaccess*_fuzzer/`（24 个）、`fileaccessext*_fuzzer/`（26 个）
