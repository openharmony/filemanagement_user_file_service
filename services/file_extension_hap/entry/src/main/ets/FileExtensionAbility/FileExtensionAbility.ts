/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
import Extension from '@ohos.application.FileAccessExtensionAbility';
import fs from '@ohos.file.fs';
import type { Filter } from '@ohos.file.fs';
import fileExtensionInfo from '@ohos.file.fileExtensionInfo';
import hilog from '@ohos.hilog';
import { getFileInfos } from './ListScanFileInfo';
import type { Fileinfo } from './Common';
import { getPath, BUNDLE_NAME, DOMAIN_CODE } from './Common';
const deviceFlag = fileExtensionInfo.DeviceFlag;
const documentFlag = fileExtensionInfo.DocumentFlag;
const deviceType = fileExtensionInfo.DeviceType;
const FILE_PREFIX_NAME = 'file://';

const TAG = 'ExternalFileManager';
const ERR_OK = 0;
const ERR_ERROR = -1;
const E_EXIST = 13900015;
const ERR_PERM = 13900001;
const E_NOEXIST = 13900002;
const E_URIS = 14300002;
const E_GETRESULT = 14300004;
const CREATE_EVENT_CODE = 0x00000100;
const DELETE_EVENT_CODE = 0x00000200 | 0x00000400;
const UPDATE_EVENT_CODE = 0x00000040 | 0x00000080 | 0x00000800;
const CREATE_EVENT = 0;
const DELETE_EVENT = 1;
const UPDATE_EVENT = 2;
const CONVERT_TO_HEX = 16;

// ['IN_ACCESS', 0x00000001],
// ['IN_MODIFY', 0x00000002],
// ['IN_ATTRIB', 0x00000004],
// ['IN_CLOSE_WRITE', 0x00000008],
// ['IN_CLOSE_NOWRITE', 0x00000010],
// ['IN_OPE', 0x00000020],
// ['IN_MOVED_FROM', 0x00000040],
// ['IN_MOVED_TO', 0x00000080],
// ['IN_CREATE', 0x00000100],
// ['IN_DELETE', 0x00000200],
// ['IN_DELETE_SELF', 0x00000400],
// ['IN_MOVE_SELF', 0x00000800]

enum createEvent {
  create = 100
}

enum deleteEvent {
  delete = 200,
  deleteSelf = 400
}

enum updataEvent {
  moveSelf = 800,
  moveFrom = 40,
  moveTo = 80
}

export default class FileExtAbility extends Extension {
  onCreate(want): void {
    hilog.info(DOMAIN_CODE, TAG, 'Extension init process');
  }

  encode(uri): string {
    try {
      uri = encodeURI(uri);
    } catch (e) {
      hilog.error(DOMAIN_CODE, TAG, 'The reason of encodeURI: ' + e.message + ' code: ' + e.code);
      uri = '';
    }
    return uri;
  }

  decode(uri): string {
    try {
      uri = decodeURI(uri);
    } catch (e) {
      hilog.error(DOMAIN_CODE, TAG, 'The reason of decodeURI: ' + e.message + ' code: ' + e.code);
      uri = '';
    }
    return uri;
  }

  checkUri(uri): boolean {
    try {
      if (uri.indexOf(FILE_PREFIX_NAME) === 0) {
        uri = uri.replace(FILE_PREFIX_NAME, '/');
        return true;
      } else {
        hilog.error(DOMAIN_CODE, TAG, 'checkUri error, uri is ' + uri);
        return false;
      }
    } catch (error) {
      hilog.error(DOMAIN_CODE, TAG, 'checkUri error, uri is ' + uri);
      return false;
    }
  }

  genNewFileUri(uri, displayName) {
    let newFileUri = uri;
    if (uri.charAt(uri.length - 1) === '/') {
      newFileUri += displayName;
    } else {
      newFileUri += '/' + displayName;
    }
    return newFileUri;
  }

  getFileName(uri): string {
    let arr = uri.split('/');
    let name = arr.pop();
    if (name === '') {
      name = arr.pop();
    }
    return name;
  }

  renameUri(uri, displayName): string {
    let arr = uri.split('/');
    let newFileUri = '';
    if (arr.pop() === '') {
      arr.pop();
      arr.push(displayName);
      arr.push('');
    } else {
      arr.push(displayName);
    }
    for (let index = 0; index < arr.length; index++) {
      if (arr[index] === '') {
        newFileUri += '/';
      } else if (index === arr.length - 1) {
        newFileUri += arr[index];
      } else {
        newFileUri += arr[index] + '/';
      }
    }
    return newFileUri;
  }

  recurseDir(path, cb): void {
    try {
      let stat = fs.statSync(path);
      if (stat.isDirectory()) {
        let fileName = fs.listFileSync(path);
        for (let fileLen = 0; fileLen < fileName.length; fileLen++) {
          stat = fs.statSync(path + '/' + fileName[fileLen]);
          if (stat.isDirectory()) {
            this.recurseDir(path + '/' + fileName[fileLen], cb);
          } else {
            cb(path + '/' + fileName[fileLen], false);
          }
        }
      } else {
        cb(path, false);
      }
    } catch (e) {
      hilog.error(DOMAIN_CODE, TAG, 'recurseDir error ' + e.message);
      cb(path, true);
    }
  }

  isCrossDeviceLink(sourceFileUri, targetParentUri): boolean {
    let roots = this.getRoots().roots;
    for (let index = 0; index < roots.length; index++) {
      let uri = roots[index].uri;
      if (sourceFileUri.indexOf(uri) === 0 && targetParentUri.indexOf(uri) === 0) {
        return false;
      }
    }
    return true;
  }

  openFile(sourceFileUri, flags): {number, number} {
    sourceFileUri = this.decode(sourceFileUri);
    if (sourceFileUri === '') {
      return {
        fd: ERR_ERROR,
        code: E_URIS,
      };
    }
    if (!this.checkUri(sourceFileUri)) {
      return {
        fd: ERR_ERROR,
        code: E_URIS,
      };
    }
    try {
      let path = getPath(sourceFileUri);
      let file = fs.openSync(path, flags);
      return {
        fd: file.fd,
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

  createFile(parentUri, displayName): {string, number} {
    parentUri = this.decode(parentUri);
    if (parentUri === '') {
      return {
        uri: '',
        code: E_URIS
      };
    }
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
      let path = getPath(newFileUri);
      let file = fs.openSync(path, fs.OpenMode.CREATE);
      fs.closeSync(file);
      newFileUri = encodeURI(newFileUri);
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

  mkdir(parentUri, displayName): {string, number} {
    parentUri = this.decode(parentUri);
    if (parentUri === '') {
      return {
        uri: '',
        code: E_URIS
      };
    }
    if (!this.checkUri(parentUri)) {
      return {
        uri: '',
        code: E_URIS,
      };
    }
    try {
      let newFileUri = this.genNewFileUri(parentUri, displayName);
      let path = getPath(newFileUri);

      fs.mkdirSync(path);
      newFileUri = encodeURI(newFileUri);
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

  delete(selectFileUri): number {
    selectFileUri = this.decode(selectFileUri);
    if (selectFileUri === '') {
      return {
        uri: '',
        code: E_URIS
      };
    }
    if (!this.checkUri(selectFileUri)) {
      return E_URIS;
    }
    let path = getPath(selectFileUri);
    let code = ERR_OK;
    try {
      let stat = fs.statSync(path);
      if (stat.isDirectory()) {
        fs.rmdirSync(path);
      } else {
        fs.unlinkSync(path);
      }
    } catch (e) {
      hilog.error(DOMAIN_CODE, TAG, 'deleteFile error ' + e.message);
      return e.code;
    }
    return code;
  }

  move(sourceFileUri, targetParentUri): {string, number} {
    sourceFileUri = this.decode(sourceFileUri);
    targetParentUri = this.decode(targetParentUri);
    if (sourceFileUri === '' || targetParentUri === '') {
      return {
        uri: '',
        code: E_URIS,
      };
    }
    if (!this.checkUri(sourceFileUri) || !this.checkUri(targetParentUri)) {
      return {
        uri: '',
        code: E_URIS,
      };
    }
    let displayName = this.getFileName(sourceFileUri);
    let newFileUri = this.genNewFileUri(targetParentUri, displayName);
    let oldPath = getPath(sourceFileUri);
    let newPath = getPath(newFileUri);
    newFileUri = this.encode(newFileUri);
    if (newFileUri === '') {
      return {
        uri: '',
        code: E_URIS,
      };
    }
    if (oldPath === newPath) {
      // move to the same directory
      return {
        uri: newFileUri,
        code: ERR_OK,
      };
    } else if (newPath.indexOf(oldPath) === 0) {
      // move to a subdirectory of the source directory
      return {
        uri: '',
        code: E_GETRESULT,
      };
    }
    try {
      // The source file does not exist or the destination is not a directory
      let isAccess = fs.accessSync(oldPath);
      if (!isAccess) {
        return {
          uri: '',
          code: E_GETRESULT,
        };
      }
      let stat = fs.statSync(getPath(targetParentUri));
      if (!stat || !stat.isDirectory()) {
        return {
          uri: '',
          code: E_GETRESULT,
        };
      }
      // If not across devices, use fs.renameSync to move
      if (!this.isCrossDeviceLink(sourceFileUri, targetParentUri)) {
        fs.renameSync(oldPath, newPath);
        return {
          uri: newFileUri,
          code: ERR_OK,
        };
      }
      //Cross device move not currently supported
      return {
          uri: '',
          code: ERR_PERM,
      };
    } catch (e) {
      hilog.error(DOMAIN_CODE, TAG, 'move error ' + e.message);
      return {
        uri: '',
        code: e.code,
      };
    }
  }

  rename(sourceFileUri, displayName): {string, number} {
    sourceFileUri = this.decode(sourceFileUri);
    if (sourceFileUri === '') {
      return {
        uri: '',
        code: E_URIS,
      };
    }

    if (!this.checkUri(sourceFileUri)) {
      return {
        uri: '',
        code: E_URIS,
      };
    }
    try {
      let newFileUri = this.renameUri(sourceFileUri, displayName);
      let oldPath = getPath(sourceFileUri);
      let newPath = getPath(newFileUri);
      fs.renameSync(oldPath, newPath);
      newFileUri = encodeURI(newFileUri);
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

  access(sourceFileUri): {boolean, number} {
    sourceFileUri = this.decode(sourceFileUri);
    if (sourceFileUri === '') {
      return {
        uri: '',
        code: E_URIS,
      };
    }
    if (!this.checkUri(sourceFileUri)) {
      hilog.error(DOMAIN_CODE, TAG, 'access checkUri fail');
      return {
        isExist: false,
        code: E_URIS,
      };
    }
    let isAccess = false;
    try {
      let path = getPath(sourceFileUri);
      isAccess = fs.accessSync(path);
      if (!isAccess) {
        return {
          isExist: false,
          code: ERR_OK,
        };
      }
    } catch (e) {
      hilog.error(DOMAIN_CODE, TAG, 'access error ' + e.message);
      return {
        isExist: false,
        code: e.code,
      };
    }
    return {
      isExist: true,
      code: ERR_OK,
    };
  }

  listFile(sourceFileUri: string, offset: number, count: number, filter: Filter) :
  {infos: Fileinfo[], code: number} {
    let infos : Fileinfo[] = [];
    sourceFileUri = this.decode(sourceFileUri);
    if (sourceFileUri === '') {
      return {
        infos: [],
        code: E_URIS,
      };
    }
    if (!this.checkUri(sourceFileUri)) {
      return {
        infos: [],
        code: E_URIS,
      };
    }
    return getFileInfos(sourceFileUri, offset, count, filter, false);
  }

  scanFile(sourceFileUri: string, offset: number, count: number, filter: Filter) :
  {infos: Fileinfo[], code: number} {
    let infos : Fileinfo[] = [];
    sourceFileUri = this.decode(sourceFileUri);
    if (sourceFileUri === '') {
      return {
        infos: [],
        code: E_URIS,
      };
    }
    if (!this.checkUri(sourceFileUri)) {
      return {
        infos: [],
        code: E_URIS,
      };
    }

    return getFileInfos(sourceFileUri, offset, count, filter, true);
  }

  getFileInfoFromUri(selectFileUri) {
    selectFileUri = this.decode(selectFileUri);
    if (selectFileUri === '') {
      return {
        uri: '',
        code: E_URIS,
      };
    }

    if (!this.checkUri(selectFileUri)) {
      return {
        fileInfo: {},
        code: E_URIS,
      };
    }
    let fileInfo = {};
    try {
      let path = getPath(selectFileUri);
      let fileName = this.getFileName(path);
      let stat = fs.statSync(path);
      let mode = documentFlag.SUPPORTS_READ | documentFlag.SUPPORTS_WRITE;
      if (stat.isDirectory()) {
        mode |= documentFlag.REPRESENTS_DIR;
      } else {
        mode |= documentFlag.REPRESENTS_FILE;
      }
      selectFileUri = encodeURI(selectFileUri);
      fileInfo = {
        uri: selectFileUri,
        relativePath: path,
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

  volumePath2uri(path): string {
    return `file://docs/storage/External/${path}`;
  }

  getRoots() {
    let roots = [
      {
        uri: 'file://docs/storage/local/Documents',
        displayName: 'Documents',
        deviceType: deviceType.DEVICE_LOCAL_DISK,
        deviceFlags: deviceFlag.SUPPORTS_READ | deviceFlag.SUPPORTS_WRITE,
      },
      {
        uri: 'file://docs/storage/local/Download',
        displayName: 'Download',
        deviceType: deviceType.DEVICE_LOCAL_DISK,
        deviceFlags: deviceFlag.SUPPORTS_READ | deviceFlag.SUPPORTS_WRITE,
      },
      {
        uri: 'file://docs/storage/Share',
        displayName: 'shared_disk',
        deviceType: deviceType.DEVICE_SHARED_DISK,
        deviceFlags: deviceFlag.SUPPORTS_READ | deviceFlag.SUPPORTS_WRITE,
      },
    ];
    try {
      let rootPath = '/storage/External';
      let volumeInfoList = [];
      let volumeName = fs.listFileSync(rootPath);
      hilog.info(DOMAIN_CODE, TAG, 'listFileSync-result: ' + volumeName);
      for (let i = 0; i < volumeName.length; i++) {
        let volumeInfo = {
          uri: this.volumePath2uri(volumeName[i]),
          displayName: volumeName[i],
          deviceType: deviceType.DEVICE_EXTERNAL_USB,
          deviceFlags: deviceFlag.SUPPORTS_READ | deviceFlag.SUPPORTS_WRITE,
        };
        hilog.info(DOMAIN_CODE, TAG, 'volumeInfo-uri: ' + volumeInfo.uri);
        volumeInfoList.push(volumeInfo);
      }
      roots = roots.concat(volumeInfoList);
      return {
        roots: roots,
        code: ERR_OK,
      };
    } catch (e) {
      hilog.error(DOMAIN_CODE, TAG, 'getRoots errorcode: ' + e.code, ' message: ' + e.message);
      return {
        roots: [],
        code: e.code,
      };
    }
  }

  getNormalResult(dirPath, column, queryResults): void {
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

  getResultFromStat(dirPath, column, stat, queryResults): void {
    if (column === 'size' && stat.isDirectory()) {
      let size = 0;
      this.recurseDir(dirPath, function (filePath, isDirectory) {
        if (!isDirectory) {
          let fileStat = fs.statSync(filePath);
          size += fileStat.size;
        }
      });
      queryResults.push(String(size));
    } else {
      queryResults.push(String(stat[column]));
    }
  }

  query(uri, columns): {[], number} {
    uri = this.decode(uri);
    if (uri === '') {
      return {
        uri: '',
        code: E_URIS,
      };
    }

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
      let dirPath = getPath(uri);
      let stat = fs.statSync(dirPath);
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

  startWatcher(uri, callback): number {
    uri = this.decode(uri);
    if (uri === '') {
      return {
        code: E_URIS,
      };
    }
    if (!this.checkUri(uri)) {
      return E_URIS;
    }
    let watchPath = getPath(uri);
    try {
      let watcher = fs.createWatcher(watchPath, CREATE_EVENT_CODE | DELETE_EVENT_CODE | UPDATE_EVENT_CODE, (data) => {
        try {
          let notifyEvent = (data.event).toString(CONVERT_TO_HEX);
          if (notifyEvent in createEvent) {
            notifyEvent = CREATE_EVENT;
          }

          if (notifyEvent in deleteEvent) {
            notifyEvent = DELETE_EVENT;
          }

          if (notifyEvent in updataEvent) {
            notifyEvent = UPDATE_EVENT;
          }
          let watchUri = FILE_PREFIX_NAME + BUNDLE_NAME + watchPath;
          watchUri = this.encode(watchUri);
          callback(watchUri, notifyEvent);
        } catch (error) {
          hilog.error(DOMAIN_CODE, TAG, 'onchange error ' + error.message);
          return E_GETRESULT;
        }
      });
      observerMap.set(uri, watcher);
      watcher.start();
    } catch (e) {
      hilog.error(DOMAIN_CODE, TAG, 'startWatcher error ' + e.message);
      return E_GETRESULT;
    }
    return ERR_OK;
  }

  stopWatcher(uri): number {
    uri = this.decode(uri);
    if (uri === '') {
      return {
        code: E_URIS,
      };
    }

    if (!this.checkUri(uri)) {
      return E_URIS;
    }
    try {
      let watcher = observerMap.get(uri);
      watcher.stop();
      observerMap.delete(uri);
    } catch (e) {
      hilog.error(DOMAIN_CODE, TAG, 'stopWatcher error ' + e.message);
      return E_GETRESULT;
    }
    return ERR_OK;
  }
};