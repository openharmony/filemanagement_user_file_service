# 用户公共文件访问框架-FileAccessFramework使用指导

## 简介

用户公共文件访问框架提供公共文件管理的接口。

FileAccessFramework基于extension机制，是一个C/S架构，分为2部分。一部分运行在客户端，当前支持picker和文件管理使用（目前只支持stage模型）。另外一部分运行在server端。目前提供数据的有媒体库和ExternalFileManager。

支持能力列举如下：
* GetRoots 获取设备根目录信息，包括URI、DisplayName、flag、type等。
* listFile：获取下一级文件(夹)信息
* createFile：创建文件
* mkdir：创建目录
* delete：删除文件(夹)
* move：移动文件(夹)
* rename：重命名文件(夹)

接口详细信息见：d.ts文件地址: https://gitee.com/openharmony/interface_sdk-js/pulls/2010/
注：不同设备的文件操作需要参考对应设备的支持规格。

## 仓目录结构
仓目录结构如下:
```
/foundation/storage/user_file_service
├── figures                                     # 插图文件
├── frameworks                                  # FileAccessFramework框架层代码
│   └── innerkits                               # 内部 Native 接口
├── serivce                                     # 服务实现
│   ├── etc                                     # 一期内部接口实现
│   ├── src                                     # 一期内部接口实现
│   ├── file_extension_hap                      # ExternalFileManager实现
├── interfaces                                  # 接口代码
│   └── kits                                    # 外部 JS 接口
│       ├── js                                  # 一期napi代码实现
│       └── napi                                # 二期napi代码实现
├── BUILD.gn                                    # 编译入口
└── bundle.json                                 # 部件描述文件
```

## 使用说明
### step0:  import fileAccess from '@ohos.data.fileAccess'
### step1：创建FileAccessHelper对象
```ets
import fileAccess from '@ohos.data.fileAccess'
let want = {
    "bundleName": "com.ohos.UserFile.ExternalFileManager",
    "abilityName": "FileExtensionAbility",
}
let fileAccessHelper = fileAccess.createFileAccessHelper(this.context, want);
```
备注：目前**仅支持一对一，即链接一个数据方**，后续提供**数据提供方查询接口和一对多能力**。
### step2:  获取数据提供方支持的所有设备根目录信息


### step3: 对指定设备进行文件操作
#### listfile
```ets
let sourceUri = "fileExtension://local/data/storage/el1/bundle/storage_daemon/";
fileAccessHelper.listFile(sourceUri, displayName, (ret, data)=> {
    console.info("size " + data.length);
    for (let i = 0; i < data.length; i++) {
        console.info("name " + data[i].fileName);
    }
});
```
#### createfile
```ets
let sourceUri = "fileExtension://local/data/storage/el1/bundle/storage_daemon/";
let displayName = "file1"
fileAccessHelper.createFile(sourceUri, displayName, (ret, data)=> {
    if (ret.code >= 0 && data != "") {
        console.info("createFile success");
    } else {
        console.info("createFile fail");
    }
});
```
#### mkdir
```ets
let sourceUri = "fileExtension://local/data/storage/el1/bundle/storage_daemon/dir";
let displayName = "dir1"
fileAccessHelper.mkdir(sourceUri, displayName, (ret, data)=> {
    if(ret.code >= 0 && data ==0){
        console.info("mkdir success");
    } else {
        console.info("mkdir fail");
    }
});
```

#### delete
```ets
let targetUri = "fileExtension://local/data/storage/el1/bundle/storage_daemon/file1";
fileAccessHelper.delete(targetUri, (ret, data)=> {
    if (ret.code >= 0 && data ==0){
        console.info("delete success");
    } else {
        console.info("delete fail");
    }
});
```
#### move
```ets
let sourceUri = "fileExtension://local/data/storage/el1/bundle/storage_daemon/file1";
let targetParentUri = "fileExtension://local/data/storage/el1/bundle/storage_daemon/dir"
fileAccessHelper.move(sourceUri, targetUri, (ret, data)=> {
    if (ret.code >= 0 && data ==0){
        console.info("move success");
    } else {
        console.info("move fail");
    }
});
```
#### rename
```ets
let sourceUri = "fileExtension://local/data/storage/el1/bundle/storage_daemon/file1";
let displayName = "fileNewName"
fileAccessHelper.rename(sourceUri, displayName, (ret, data)=> {
    if (ret.code >= 0 && data ==0){
        console.info("rename success");
    } else {
        console.info("rename fail");
    }
});
```

## 后续能力支持
* 链接多个server端
* 获取所有server端的所有设备根目录信息
* 支持设备notify机制，支持业务方感知设备状态的变化
* listfile和getroots返回迭代器对象，按照迭代器方式进行使用


