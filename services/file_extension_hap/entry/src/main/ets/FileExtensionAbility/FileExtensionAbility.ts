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
import Extension from '@ohos.application.FileExtensionAbility'
import uriClass from '@ohos.uri'
import fileio from '@ohos.fileio'
import hilog from '@ohos.hilog'
import fileExtensionInfo from "@ohos.fileExtensionInfo"

const FLAG = fileExtensionInfo.FLAG;

export default class FileExtAbility extends Extension {
    onCreate(want) {
        hilog.info(0x0001, 'js server tag dsa', 'FileExtAbility onCreate, want:' + want.abilityName);
    }

    getPath(uri) {
        let uriObj = new uriClass.URI(uri);
        let path = uriObj.path;
        hilog.info(0x0001, 'js server tag dsa', 'getPath is ' + path);
        return path;
    }

    genNewFileUri(uri, displayName) {
        let arr = uri.split('/');
        let newFileUri = uri;
        if (arr.pop() == '') {
            newFileUri += displayName;
        } else {
            newFileUri += '/' + displayName;
        }
        hilog.info(0x0001, 'js server tag dsa', 'genNewFileUri is ' + newFileUri);
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
        hilog.info(0x0001, 'js server tag dsa', 'renameUri is ' + newFileUri);
        return newFileUri;
    }

    listDir(path, cb) {
        hilog.info(0x0001, 'js server tag dsa', 'listDir: ' + path);
        try {
            let stat = fileio.statSync(path);
            if (stat.isDirectory()) {
                let dir = fileio.opendirSync(path);
                let hasNextFile = true;
                while (hasNextFile) {
                    try {
                        let dirent = dir.readSync();
                        this.listDir(path + '/' + dirent.name, cb);
                    } catch (e) {
                        hilog.info(0x0001, 'js server tag dsa', 'listDir dir.readSync catch' + e);
                        hasNextFile = false;
                        cb(path);
                    }
                }
            } else {
                cb(path);
            }
        } catch (e) {
            hilog.info(0x0001, 'js server tag dsa', 'listDir catch ' + e + ' ' + path);
            cb(path);
        }
    }

    openFile(sourceFileUri, flags) {
        hilog.info(0x0001, 'js server tag dsa', 'openFile, uri:' + sourceFileUri + ',flags:' + flags);
        let fd = 0;
        try {
            let path = this.getPath(sourceFileUri);
            fd = fileio.openSync(path, flags, 0o666);
        } catch (e) {
            hilog.info(0x0001, 'js server tag dsa', 'openFile catch' + e);
            fd = -1;
        }

        return fd;
    }

    closeFile(fd, uri) {
        try {
            fileio.closeSync(fd);
            return true;
        } catch (e) {
            hilog.info(0x0001, 'js server tag dsa', 'closeFile catch' + e);
            return false;
        }
    }

    createFile(parentUri, displayName) {
        hilog.info(0x0001, 'js server tag dsa', 'createFile, uri:' + parentUri + ', displayName:' + displayName);
        try {
            let newFileUri = this.genNewFileUri(parentUri, displayName);
            let path = this.getPath(newFileUri);
            fileio.openSync(path, 0o100, 0o666);
            return newFileUri;
        } catch (e) {
            hilog.info(0x0001, 'js server tag dsa', 'createFile catch' + e);
            return '';
        }
    }

    mkdir(parentUri, displayName) {
        hilog.info(0x0001, 'js server tag dsa', 'mkdir, uri:' + parentUri + ', displayName:' + displayName);
        try {
            let newFileUri = this.genNewFileUri(parentUri, displayName);
            let path = this.getPath(newFileUri);
            fileio.mkdirSync(path);
            return newFileUri;
        } catch (e) {
            hilog.info(0x0001, 'js server tag dsa', 'mkdir catch' + e);
            return '';
        }
    }

    delete(selectFileUri) {
        hilog.info(0x0001, 'js server tag dsa', 'delete, uri:' + selectFileUri);
        let path = this.getPath(selectFileUri);
        let code = 0;
        this.listDir(path, function (filePath) {
            try {
                fileio.unlinkSync(filePath);
            } catch (e) {
                hilog.info(0x0001, 'js server tag dsa', 'delete catch' + e);
                code = -1;
            }
        });

        return code;
    }

    move(sourceFileUri, targetParentUri) {
        hilog.info(0x0001, 'js server tag dsa', 'move, sourceFileUri:' + sourceFileUri + ', targetParentUri:' + targetParentUri);
        try {
            let displayName = this.getFileName(sourceFileUri);
            let newFileUri = this.genNewFileUri(targetParentUri, displayName);
            let oldPath = this.getPath(sourceFileUri);
            let newPath = this.getPath(newFileUri);
            fileio.renameSync(oldPath, newPath);
            return newFileUri;
        } catch (e) {
            hilog.info(0x0001, 'js server tag dsa', 'rename catch' + e);
            return '';
        }
    }

    rename(sourceFileUri, displayName) {
        hilog.info(0x0001, 'js server tag dsa', 'rename, sourceFileUri:' + sourceFileUri + ', displayName:' + displayName);
        try {
            let newFileUri = this.renameUri(sourceFileUri, displayName);
            let oldPath = this.getPath(sourceFileUri);
            let newPath = this.getPath(newFileUri);
            fileio.renameSync(oldPath, newPath);
            return newFileUri;
        } catch (e) {
            hilog.info(0x0001, 'js server tag dsa', 'rename catch' + e);
            return '';
        }
    }

    query(sourceFileUri) {
        hilog.info(0x0001, 'js server tag dsa', 'query, sourceFileUri:' + sourceFileUri);
        try {
            let path = this.getPath(sourceFileUri);
            let stat = fileio.statSync(path);
            return {
                uri: sourceFileUri,
                fileName: this.getFileName(sourceFileUri),
                mode: stat.mode,
                size: stat.size,
                mtime: stat.mtime,
                mimiType: '',
            };
        } catch (e) {
            hilog.info(0x0001, 'js server tag dsa', 'query catch' + e);
            return null;
        }
    }

    listFile(sourceFileUri) {
        hilog.info(0x0001, 'js server tag dsa', 'listFile, sourceFileUri: ' + sourceFileUri);

        let infos = [];
        try {
            let path = this.getPath(sourceFileUri);
            let dir = fileio.opendirSync(path);
            let hasNextFile = true;
            while (hasNextFile) {
                try {
                    let dirent = dir.readSync();
                    let stat = fileio.statSync(path + '/' + dirent.name);
                    infos.push({
                        uri: this.genNewFileUri(sourceFileUri, dirent.name),
                        fileName: dirent.name,
                        mode: stat.mode,
                        size: stat.size,
                        mtime: stat.mtime,
                        mimiType: '',
                    });
                } catch (e) {
                    hilog.info(0x0001, 'js server tag dsa', 'dir.readSync catch' + e);
                    hasNextFile = false;
                }
            }
        } catch (e) {
            hilog.info(0x0001, 'js server tag dsa', 'listFile catch' + e);
        }

        hilog.info(0x0001, 'js server tag dsa', 'listFile return: ' + JSON.stringify(infos));
        return infos;
    }

    getRoots() {
        hilog.info(0x0001, 'js server tag dsa', 'getRoots');
        let roots = [{
            uri: 'fileAccess:///data/storage/el1/bundle/storage_daemon',
            displayName: 'storage_daemon',
            deviceId: '',
            flags: FLAG.SUPPORTS_WRITE | FLAG.SUPPORTS_DELETE | FLAG.SUPPORTS_RENAME | FLAG.SUPPORTS_COPY
                | FLAG.SUPPORTS_MOVE | FLAG.SUPPORTS_REMOVE | FLAG.DIR_SUPPORTS_CREATE | FLAG.DIR_PREFERS_LAST_MODIFIED,
        }];
        return roots;
    }
};