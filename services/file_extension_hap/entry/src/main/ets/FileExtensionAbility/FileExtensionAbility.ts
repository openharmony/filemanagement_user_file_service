/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
// @ts-nocheck
import Extension from '@ohos.application.FileAccessExtensionAbility'
import fileio from '@ohos.fileio'
import { init, findVolumeInfo, delVolumeInfo, getVolumeInfoList, path2uri } from './VolumeManager'
import { onReceiveEvent } from './Subcriber'
import fileExtensionInfo from "@ohos.file.fileExtensionInfo"
import hilog from '@ohos.hilog'
import process from '@ohos.process';

const DeviceFlag = fileExtensionInfo.DeviceFlag;
const DocumentFlag = fileExtensionInfo.DocumentFlag;
const NotifyType = fileExtensionInfo.NotifyType;
const DeviceType = fileExtensionInfo.DeviceType;
const BUNDLE_NAME = 'com.ohos.UserFile.ExternalFileManager';
const DEFAULT_MODE = 0o666;
const CREATE_FILE_FLAGS = 0o100;
const URI_SCHEME = 'datashare://';
const DOMAIN_CODE = 0x0001;
const TAG = 'ExternalFileManager';
const ERR_OK = 0;
const ERR_ERROR = -1;
const E_EXIST = 13900015;
const E_NOEXIST = 13900002;
const E_URIS = 14300002;
const E_GETRESULT = 14300004;
let callbackFun = null;

export default class FileExtAbility extends Extension {
    onCreate(want) {
        init();
        onReceiveEvent(function (data) {
            if (data.event == 'usual.event.data.VOLUME_MOUNTED') {
                if (callbackFun != null) {
                    let uri = path2uri('', data.parameters.path);
                    callbackFun(data.parameters.type, NotifyType.DEVICE_ONLINE, uri);
                }
                process.exit(0);
            } else {
                if (callbackFun != null) {
                    let uri = '';
                    let deviceType = 0;
                    let volumeInfo = findVolumeInfo(data.parameters.id);
                    if (volumeInfo) {
                        uri = volumeInfo.uri;
                        deviceType = volumeInfo.type;
                    }
                    callbackFun(deviceType, NotifyType.DEVICE_OFFLINE, uri);
                }
                delVolumeInfo(data.parameters.id);
            }
        });
    }

    registerCallback(callback) {
        callbackFun = callback;
    }

    checkUri(uri) {
        if (uri.indexOf(URI_SCHEME) == 0) {
            uri = uri.replace(URI_SCHEME, '');
            return /^\/([^\/]+\/?)+$/.test(uri);
        } else {
            hilog.error(DOMAIN_CODE, TAG, 'checkUri error, uri is ' + uri);
            return false;
        }
    }

    getPath(uri) {
        let sep = '://';
        let arr = uri.split(sep);
        if (arr.length < 2) {
            return uri;
        }
        let path = uri.replace(arr[0] + sep, '');
        if (arr[1].indexOf('/') > 0) {
            path = path.replace(arr[1].split('/')[0], '');
        }
        path = path.replace('/' + BUNDLE_NAME, '');
        if (path.charAt(path.length - 1) == '/') {
            path = path.substr(0, path.length - 1);
        }
        return path;
    }

    genNewFileUri(uri, displayName) {
        let newFileUri = uri;
        if (uri.charAt(uri.length - 1) == '/') {
            newFileUri += displayName;
        } else {
            newFileUri += '/' + displayName;
        }
        return newFileUri;
    }

    getFileName(uri) {
        let arr = uri.split('/');
        let name = arr.pop();
        if (name == '') {
            name = arr.pop();
        }
        return name;
    }

    renameUri(uri, displayName) {
        let arr = uri.split('/');
        let newFileUri = '';
        if (arr.pop() == '') {
            arr.pop();
            arr.push(displayName)
            arr.push('');
        } else {
            arr.push(displayName)
        }
        for (var index = 0; index < arr.length; index++) {
            if (arr[index] == '') {
                newFileUri += '/';
            } else if (index == arr.length - 1) {
                newFileUri += arr[index];
            } else {
                newFileUri += arr[index] + '/';
            }
        }
        return newFileUri;
    }

    recurseDir(path, cb) {
        try {
            let stat = fileio.statSync(path);
            if (stat.isDirectory()) {
                let dir = fileio.opendirSync(path);
                let hasNextFile = true;
                cb(path, true, hasNextFile);
                while (hasNextFile) {
                    try {
                        let dirent = dir.readSync();
                        this.recurseDir(path + '/' + dirent.name, cb);
                    } catch (e) {
                        hasNextFile = false;
                        cb(path, true, hasNextFile);
                    }
                }
                dir.closeSync();
            } else {
                cb(path, false);
            }
        } catch (e) {
            hilog.error(DOMAIN_CODE, TAG, 'recurseDir error ' + e.message);
            cb(path, true);
        }
    }

    isCrossDeviceLink(sourceFileUri, targetParentUri) {
        let roots = this.getRoots().roots;
        for (let index = 0; index < roots.length; index++) {
            let uri = roots[index].uri;
            if (sourceFileUri.indexOf(uri) == 0 && targetParentUri.indexOf(uri) == 0) {
                return false;
            }
        }
        return true;
    }

    openFile(sourceFileUri, flags) {
        if (!this.checkUri(sourceFileUri)) {
            return {
                fd: ERR_ERROR,
                code: E_URIS,
            };
        }
        let fd = 0;
        try {
            let path = this.getPath(sourceFileUri);
            fd = fileio.openSync(path, flags, DEFAULT_MODE);
            return {
                fd: fd,
                code: ERR_OK,
            };
        } catch (e) {
            hilog.error(DOMAIN_CODE, TAG, 'openFile error ' + e.message);
            return {
                fd: ERR_ERROR,
                code: e.code,
            };
        }
    }

    createFile(parentUri, displayName) {
        if (!this.checkUri(parentUri)) {
            return {
                uri: '',
                code: E_URIS,
            };
        }
        try {
            let newFileUri = this.genNewFileUri(parentUri, displayName);
            if (this.access(newFileUri).isExist) {
                return {
                    uri: '',
                    code: E_EXIST,
                };
            }
            let path = this.getPath(newFileUri);
            let fd = fileio.openSync(path, CREATE_FILE_FLAGS, DEFAULT_MODE);
            fileio.closeSync(fd);
            return {
                uri: newFileUri,
                code: ERR_OK,
            };
        } catch (e) {
            hilog.error(DOMAIN_CODE, TAG, 'createFile error ' + e.message);
            return {
                uri: '',
                code: e.code,
            };
        }
    }

    mkdir(parentUri, displayName) {
        if (!this.checkUri(parentUri)) {
            return {
                uri: '',
                code: E_URIS,
            };
        }
        try {
            let newFileUri = this.genNewFileUri(parentUri, displayName);
            let path = this.getPath(newFileUri);
            fileio.mkdirSync(path);
            return {
                uri: newFileUri,
                code: ERR_OK,
            };
        } catch (e) {
            hilog.error(DOMAIN_CODE, TAG, 'mkdir error ' + e.message);
            return {
                uri: '',
                code: e.code,
            };
        }
    }

    delete(selectFileUri) {
        if (!this.checkUri(selectFileUri)) {
            return E_URIS;
        }
        let path = this.getPath(selectFileUri);
        let code = ERR_OK;
        this.recurseDir(path, function (filePath, isDirectory, hasNextFile) {
            try {
                if (isDirectory) {
                    if (!hasNextFile) {
                        fileio.rmdirSync(filePath);
                    }
                } else {
                    fileio.unlinkSync(filePath);
                }
            } catch (e) {
                hilog.error(DOMAIN_CODE, TAG, 'delete error ' + e.message);
                // At present, the master libn has modified the interface exception throwing mechanism
                // and the exception has no code attribute, which will lead to the failure of some faf use cases.
                // In the later stage, the new file_api interfaces will be merged and modified in a unified way.
                code = E_GETRESULT;
            }
        });
        return code;
    }

    move(sourceFileUri, targetParentUri) {
        if (!this.checkUri(sourceFileUri) || !this.checkUri(targetParentUri)) {
            return {
                uri: '',
                code: E_URIS,
            };
        }
        let displayName = this.getFileName(sourceFileUri);
        let newFileUri = this.genNewFileUri(targetParentUri, displayName);
        let oldPath = this.getPath(sourceFileUri);
        let newPath = this.getPath(newFileUri);
        if (oldPath == newPath) {
            // move to the same directory
            return {
                uri: newFileUri,
                code: ERR_OK,
            };
        } else if (newPath.indexOf(oldPath) == 0) {
            // move to a subdirectory of the source directory
            return {
                uri: '',
                code: E_GETRESULT,
            };
        }
        try {
            // The source file does not exist or the destination is not a directory
            fileio.accessSync(oldPath);
            let stat = fileio.statSync(this.getPath(targetParentUri));
            if (!stat || !stat.isDirectory()) {
                return {
                    uri: '',
                    code: E_GETRESULT,
                };
            }
            // If not across devices, use fileio.renameSync to move
            if (!this.isCrossDeviceLink(sourceFileUri, targetParentUri)) {
                fileio.renameSync(oldPath, newPath);
                return {
                    uri: newFileUri,
                    code: ERR_OK,
                };
            }
        } catch (e) {
            hilog.error(DOMAIN_CODE, TAG, 'move error ' + e.message);
            return {
                uri: '',
                code: e.code,
            };
        }
        /**
         * Recursive source directory
         * If it is a directory, create a new directory first and then delete the source directory.
         * If it is a file, copy the file first and then delete the source file.
         * The source directory will be deleted after the sub files are deleted
         */
        this.recurseDir(oldPath, function (filePath, isDirectory, hasNextFile) {
            try {
                let newFilePath = filePath.replace(oldPath, newPath);
                if (isDirectory) {
                    if (hasNextFile) {
                        try {
                            // If the target directory already has a directory with the same name, it will not be created
                            fileio.accessSync(newFilePath);
                        } catch (e) {
                            fileio.mkdirSync(newFilePath);
                        }
                    } else {
                        fileio.rmdirSync(filePath);
                    }
                } else {
                    fileio.copyFileSync(filePath, newFilePath);
                    fileio.unlinkSync(filePath);
                }
            } catch (e) {
                hilog.error(DOMAIN_CODE, TAG, 'move error ' + e.message);
                return {
                    uri: '',
                    code: e.code,
                };
            }
        });
        return {
            uri: newFileUri,
            code: ERR_OK,
        };
    }

    rename(sourceFileUri, displayName) {
        if (!this.checkUri(sourceFileUri)) {
            return {
                uri: '',
                code: E_URIS,
            };
        }
        try {
            let newFileUri = this.renameUri(sourceFileUri, displayName);
            let oldPath = this.getPath(sourceFileUri);
            let newPath = this.getPath(newFileUri);
            fileio.renameSync(oldPath, newPath);
            return {
                uri: newFileUri,
                code: ERR_OK,
            };
        } catch (e) {
            hilog.error(DOMAIN_CODE, TAG, 'rename error ' + e.message);
            return {
                uri: '',
                code: e.code,
            };
        }
    }

    access(sourceFileUri) {
        if (!this.checkUri(sourceFileUri)) {
            hilog.error(DOMAIN_CODE, TAG, 'access checkUri fail');
            return {
                isExist: false,
                code: E_URIS,
            };
        }
        try {
            let path = this.getPath(sourceFileUri);
            fileio.accessSync(path);
        } catch (e) {
            hilog.error(DOMAIN_CODE, TAG, 'access error ' + e.message);
            if (e.message == 'No such file or directory') {
                return {
                    isExist: false,
                    code: ERR_OK,
                };
            }
            return {
                isExist: false,
                code: e.code,
            }
        }
        return {
            isExist: true,
            code: ERR_OK,
        };
    }

    listFile(sourceFileUri, offset, count, filter) {
        if (!this.checkUri(sourceFileUri)) {
            return {
                infos: [],
                code: E_URIS,
            };
        }
        let infos = [];
        try {
            let path = this.getPath(sourceFileUri);
            let dir = fileio.opendirSync(path);
            let hasNextFile = true;
            let i = 0;
            while (hasNextFile) {
                try {
                    let dirent = dir.readSync();
                    let stat = fileio.statSync(path + '/' + dirent.name);
                    let mode = DocumentFlag.SUPPORTS_READ | DocumentFlag.SUPPORTS_WRITE;
                    if (stat.isDirectory()) {
                        mode |= DocumentFlag.REPRESENTS_DIR;
                    } else {
                        mode |= DocumentFlag.REPRESENTS_FILE;
                    }

                    if (offset > i) {
                        i++;
                        continue;
                    }

                    infos.push({
                        uri: this.genNewFileUri(sourceFileUri, dirent.name),
                        fileName: dirent.name,
                        mode: mode,
                        size: stat.size,
                        mtime: stat.mtime,
                        mimeType: '',
                    });

                    i++;
                    if (i == (offset + count)) {
                        hasNextFile = false;
                        break;
                    }
                } catch (e) {
                    hasNextFile = false;
                }
            }
            dir.closeSync();
        } catch (e) {
            hilog.error(DOMAIN_CODE, TAG, 'listFile error ' + e.message);
            return {
                infos: [],
                // At present, the master libn has modified the interface exception throwing mechanism
                // and the exception has no code attribute, which will lead to the failure of some faf use cases.
                // In the later stage, the new file_api interfaces will be merged and modified in a unified way.
                code: E_GETRESULT,
            };
        }
        return {
            infos: infos,
            code: ERR_OK,
        };
    }

    getFileInfoFromUri(selectFileUri) {
        if (!this.checkUri(selectFileUri)) {
            return {
                fileInfo: {},
                code: E_URIS,
            };
        }
        let fileInfo = {};
        try {
            let path = this.getPath(selectFileUri);
            let fileName = this.getFileName(path);
            let stat = fileio.statSync(path)
            let mode = DocumentFlag.SUPPORTS_READ | DocumentFlag.SUPPORTS_WRITE;
            if (stat.isDirectory()) {
                mode |= DocumentFlag.REPRESENTS_DIR;
            } else {
                mode |= DocumentFlag.REPRESENTS_FILE;
            }
            fileInfo = {
                uri: selectFileUri,
                fileName: fileName,
                mode: mode,
                size: stat.size,
                mtime: stat.mtime,
                mimeType: '',
            };
        } catch (e) {
            hilog.error(DOMAIN_CODE, TAG, 'getFileInfoFromUri error ' + e.message);
            return {
                fileInfo: {},
                code: e.code,
            };
        }
        return {
            fileInfo: fileInfo,
            code: ERR_OK,
        };
    }

    getRoots() {
        let roots = [
            {
                uri: 'datashare:///com.ohos.UserFile.ExternalFileManager/data/storage/el1/bundle/storage_daemon',
                displayName: 'shared_disk',
                deviceType: DeviceType.DEVICE_SHARED_DISK,
                deviceFlags: DeviceFlag.SUPPORTS_READ | DeviceFlag.SUPPORTS_WRITE,
            },
        ];
        roots = roots.concat(getVolumeInfoList());
        return {
            roots: roots,
            code: ERR_OK,
        };
    }

    getNormalResult(dirPath, column, queryResults) {
        if (column === 'display_name') {
            let index = dirPath.lastIndexOf('/');
            let target = dirPath.substring(index + 1, );
            queryResults.push(String(target));
        } else if (column === 'relative_path') {
            let index = dirPath.lastIndexOf('/');
            let target = dirPath.substring(0, index + 1);
            queryResults.push(target);
        } else {
            queryResults.push('');
        }
    }

    getResultFromStat(dirPath, column, stat, queryResults) {
        if (column === 'size' && stat.isDirectory()) {
            let size = 0;
            this.recurseDir(dirPath, function (filePath, isDirectory, hasNextFile) {
                if (isDirectory && !hasNextFile) {
                    return;
                }

                if (!isDirectory) {
                    let fileStat = fileio.statSync(filePath);
                    size += fileStat.size;
                }
            });
            queryResults.push(String(size));
        } else {
            queryResults.push(String(stat[column]));
        }
    }

    query(uri, columns) {
        if (!this.checkUri(uri)) {
            return {
                results: [],
                code: E_URIS,
            };
        }

        if (!this.access(uri).isExist) {
            return {
                results: [],
                code: E_NOEXIST,
            };
        }

        let queryResults = [];
        try {
            let dirPath = this.getPath(uri);
            let stat = fileio.statSync(dirPath);
            for (let index in columns) {
                let column = columns[index];
                if (column in stat) {
                    this.getResultFromStat(dirPath, column, stat, queryResults);
                } else {
                    this.getNormalResult(dirPath, column, queryResults);
                }
            }
        } catch (e) {
            hilog.error(DOMAIN_CODE, TAG, 'query error ' + e.message);
            return {
                results: [],
                code: E_GETRESULT,
            };
        }
        return {
            results: queryResults,
            code: ERR_OK,
        };
    }
};