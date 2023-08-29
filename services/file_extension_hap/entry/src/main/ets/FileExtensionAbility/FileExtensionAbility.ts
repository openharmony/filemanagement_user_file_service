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
import { getPath, checkUri, BUNDLE_NAME, DOMAIN_CODE, FILE_PREFIX_NAME, TAG } from './Common';
const deviceFlag = fileExtensionInfo.DeviceFlag;
const documentFlag = fileExtensionInfo.DocumentFlag;
const deviceType = fileExtensionInfo.DeviceType;
const FILEOVERWRITE = 1;
const THROWEXCEPTION = 0;

const ERR_OK = 0;
const ERR_ERROR = -1;
const COPY_EXCEPTION = -1;
const COPY_NOEXCEPTION = -2;
const E_PERM = 13900001;
const E_NOEXIST = 13900002;
const E_FAULT = 13900013;
const E_EXIST = 13900015;
const E_INVAL = 13900020;
const E_URIS = 14300002;
const E_GETRESULT = 14300004;
const CREATE_EVENT_CODE = 0x00000100;
const IN_DELETE_EVENT_CODE = 0x00000200;
const DELETE_SELF_EVENT_CODE = 0x00000400;
const MOVE_TO_CODE = 0x00000080;
const MOVED_FROM_CODE = 0x00000040;
const MOVE_SELF_CODE = 0x00000800;
const MOVE_MODLE_CODE = 3;
const CREATE_EVENT = 0;
const DELETE_EVENT = 1;
const MOVED_TO = 2;
const MOVED_FROM = 3;
const MOVED_SELF = 4;
const TRASH_PATH = '/storage/.Trash/Users/100/';
const TRASH_SUB_FODER = '/oh_trash_content';
let observerMap = new Map();
let watcherCountMap = new Map();
let eventMap = new Map([
  [CREATE_EVENT_CODE, CREATE_EVENT],
  [IN_DELETE_EVENT_CODE, DELETE_EVENT],
  [DELETE_SELF_EVENT_CODE, DELETE_EVENT],
  [MOVE_TO_CODE, MOVED_TO],
  [MOVED_FROM_CODE, MOVED_FROM],
  [MOVE_SELF_CODE, MOVED_SELF]
]);

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
    if (!checkUri(sourceFileUri)) {
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
    if (!checkUri(parentUri)) {
      return {
        uri: '',
        code: E_URIS,
      };
    }
    try {
      hilog.info(DOMAIN_CODE, TAG, 'createFile, uri is ' + parentUri);
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
    if (!checkUri(parentUri)) {
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

  mkdirs(path): void {
    if (path.length > 0) {
      // argument uri is dir, add '/'
      path = path + '/';
      for (let i = 1; i < path.length; ++i) {
        if (path.charAt(i) === '/') {
          let subDir = path.substring(0, i);
          hilog.info(DOMAIN_CODE, TAG, 'mkdirs: subDir path = ' + subDir);
          try {
            let isAccess = fs.accessSync(subDir);
            if (!isAccess) {
              fs.mkdirSync(subDir);
            }
          } catch (e) {
            hilog.error(DOMAIN_CODE, TAG, 'mkdirs error ' + e.message);
          }
        }
      }
    }
  }

  deleteToTrash(path): number {
    hilog.info(DOMAIN_CODE, TAG, 'deleteToTrash: path:' + path);
    let code = ERR_OK;
    let pathLen = path.length;
    if (path.charAt(pathLen - 1) === '/') {
      path = path.substring(0, pathLen - 1);
    }
    // 取最后一级和前一层目录
    let posLastSlash = path.lastIndexOf('/');
    if (posLastSlash === -1) {
      hilog.error(DOMAIN_CODE, TAG, 'Mkdirs: invalid uri');
      return E_URIS;
    }

    let selectPathOnly = path.substring(0, posLastSlash);
    // 获取时间戳
    let curTime = new Date().getTime();
    // 拼接新路径
    let currentTrashParentPath = TRASH_PATH + curTime + selectPathOnly + TRASH_SUB_FODER + curTime;
    hilog.info(DOMAIN_CODE, TAG, 'deleteToTrash: currentTrashParentPath:' + currentTrashParentPath);
    // 创建回收站目录
    this.mkdirs(currentTrashParentPath);
    try {
      let stat = fs.statSync(path);
      if (!stat.isDirectory()) {
        let selectFileOnly = path.substring(posLastSlash);
        hilog.info(DOMAIN_CODE, TAG, 'deleteToTrash: selectFileOnly:' + selectFileOnly);
        let newFileName = currentTrashParentPath + selectFileOnly;
        hilog.info(DOMAIN_CODE, TAG, 'deleteToTrash: newFileName:' + newFileName);
        // 移动文件
        fs.moveFileSync(path, newFileName, 0);
      } else {
        // 待删除文件夹
        let toDeleteDir = path.substring(posLastSlash);
        // 移动待删除文件夹
        fs.moveDirSync(selectPathOnly + toDeleteDir, currentTrashParentPath, MOVE_MODLE_CODE);
        if (fs.accessSync(path)) {
          fs.rmdirSync(path);
        }
      }
    } catch (e) {
      hilog.error(DOMAIN_CODE, TAG, 'deleteToTrash error ' + e.message);
      return e.code;
    }
    return code;
  }

  delete(selectFileUri): number {
    selectFileUri = this.decode(selectFileUri);
    if (selectFileUri === '') {
      return E_URIS;
    }
    if (!checkUri(selectFileUri)) {
      return E_URIS;
    }
    let path = getPath(selectFileUri);

    hilog.info(DOMAIN_CODE, TAG, 'Delete: path = ' + path);
    return this.deleteToTrash(path);
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
    if (!checkUri(sourceFileUri) || !checkUri(targetParentUri)) {
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
    } else if (newPath.match(new RegExp('^' + oldPath + '(/|$)'))) {
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

      let statOld = fs.statSync(oldPath);
      if (!statOld) {
        return {
          uri: '',
          code: E_GETRESULT,
        }
      }

      // isDir
      if (statOld.isDirectory()) {
        fs.moveDirSync(oldPath, getPath(targetParentUri), MOVE_MODLE_CODE);
        return {
          uri: newFileUri,
          code: ERR_OK,
        };
      }
      // when targetFile is exist, delete it
      let isAccessNewPath = fs.accessSync(newPath);
      if (isAccessNewPath) {
        fs.unlinkSync(newPath);
      }
      fs.moveFileSync(oldPath, newPath, 0);

      return {
        uri: newFileUri,
        code: ERR_OK,
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

    if (!checkUri(sourceFileUri)) {
      return {
        uri: '',
        code: E_URIS,
      };
    }
    try {
      let newFileUri = this.renameUri(sourceFileUri, displayName);
      let oldPath = getPath(sourceFileUri);
      let newPath = getPath(newFileUri);
      let isAccess = fs.accessSync(newPath);
      if (isAccess) {
        return {
          uri: '',
          code: E_EXIST,
        };
      }
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

  getCopyReturnValue(sourceUri, destUri, errCode, errMsg, ret): {[], number} {
    let copyResult = [
      {
        sourceUri: sourceUri,
        destUri: destUri,
        errCode: errCode,
        errMsg: errMsg,
      },
    ];
    return {
      results: copyResult,
      code: ret,
    };
  }

  checkCopyArguments(sourceFileUri, targetParentUri): {[], number} {
    if (!checkUri(sourceFileUri) || !checkUri(targetParentUri)) {
      hilog.error(DOMAIN_CODE, TAG, 'check arguments error, invalid arguments');
      return this.getCopyReturnValue(sourceFileUri, targetParentUri, E_URIS, '', COPY_EXCEPTION);
    }

    let displayName = this.getFileName(sourceFileUri);
    let newFileOrDirUri = this.genNewFileUri(targetParentUri, displayName);
    let oldPath = getPath(sourceFileUri);
    let newPath = getPath(newFileOrDirUri);
    if (oldPath === newPath) {
      hilog.error(DOMAIN_CODE, TAG, 'the source and target files are the same file');
      return this.getCopyReturnValue(sourceFileUri, targetParentUri, E_INVAL, '', COPY_NOEXCEPTION);
    } else if (newPath.indexOf(oldPath) === 0 && newPath.charAt(oldPath.length) === '/') {
      hilog.error(DOMAIN_CODE, TAG, 'copy to a subdirectory of the source directory');
      return this.getCopyReturnValue(sourceFileUri, targetParentUri, E_FAULT, '', COPY_EXCEPTION);
    }

    try {
      let isExist = fs.accessSync(oldPath);
      if (!isExist) {
        hilog.error(DOMAIN_CODE, TAG, 'source uri is not exist, invalid arguments');
        return this.getCopyReturnValue(sourceFileUri, '', E_INVAL, '', COPY_NOEXCEPTION);
      }

      let stat = fs.statSync(getPath(targetParentUri));
      if (!stat || !stat.isDirectory()) {
        hilog.error(DOMAIN_CODE, TAG, 'target is not directory, invalid arguments');
        return this.getCopyReturnValue('', targetParentUri, E_INVAL, '', COPY_NOEXCEPTION);
      }
    } catch (e) {
      hilog.error(DOMAIN_CODE, TAG, 'copy error ' + e.message);
      return this.getCopyReturnValue(sourceFileUri, targetParentUri, e.code, '', COPY_EXCEPTION);
    }
    return {
      results: [],
      code: ERR_OK,
    };
  }

  processCopyReturnValue(ret, copyRet): void {
    if (ret.code === COPY_EXCEPTION) {
      copyRet = ret;
    }
    if (ret.code === COPY_NOEXCEPTION) {
      for (let index in ret.results) {
        copyRet.results.push(ret.results[index]);
      }
      copyRet.code = ret.code;
    }
  }

  copyFile(sourceFilePath, newFilePath): {[], number} {
    let copyRet = {
      results: [],
      code: ERR_OK,
    };

    try {
      let isExist = fs.accessSync(newFilePath);
      if (isExist) {
        fs.unlinkSync(newFilePath);
      }
      fs.copyFileSync(sourceFilePath, newFilePath);
    } catch (err) {
      hilog.error(DOMAIN_CODE, TAG,
        'copyFileSync failed with error message: ' + err.message + ', error code: ' + err.code);
      return this.getCopyReturnValue(this.relativePath2uri(sourceFilePath), '', err.code, err.message, COPY_EXCEPTION);
    }
    return copyRet;
  }

  copyDirectory(sourceFilePath, targetFilePath, mode): {[], number} {
    let copyRet = {
      results: [],
      code: ERR_OK,
    };
    try {
      fs.copyDirSync(sourceFilePath, targetFilePath, mode);
    } catch (err) {
      if (err.code === E_EXIST) {
        for (let i = 0; i < err.data.length; i++) {
          hilog.error(DOMAIN_CODE, TAG,
            'copy directory failed with conflicting files: ' + err.data[i].srcFile + ' ' + err.data[i].destFile);
          let ret = this.getCopyReturnValue(
            this.relativePath2uri(err.data[i].srcFile), this.relativePath2uri(err.data[i].destFile),
            err.code, err.message, COPY_NOEXCEPTION);
          this.processCopyReturnValue(ret, copyRet);
        }
        return copyRet;
      }
      hilog.error(DOMAIN_CODE, TAG,
        'copy directory failed with error message: ' + err.message + ', error code: ' + err.code);
      return this.getCopyReturnValue(
        this.relativePath2uri(sourceFilePath), this.relativePath2uri(targetFilePath),
        err.code, err.message, COPY_EXCEPTION);
    }
    return copyRet;
  }

  copy(sourceFileUri, targetParentUri, force): {[], number} {
    let checkRet = this.checkCopyArguments(sourceFileUri, targetParentUri);
    if (checkRet.code !== ERR_OK) {
      return checkRet;
    }

    let sourceFilePath = getPath(sourceFileUri);
    let targetFilePath = getPath(targetParentUri);
    let displayName = this.getFileName(sourceFileUri);
    let newFileOrDirUri = this.genNewFileUri(targetParentUri, displayName);
    let newFilePath = getPath(newFileOrDirUri);

    let stat = fs.statSync(sourceFilePath);
    if (stat.isFile()) {
      let isExist = fs.accessSync(newFilePath);
      if (isExist && force === false) {
        return this.getCopyReturnValue(sourceFileUri, newFileOrDirUri, E_EXIST, '', COPY_NOEXCEPTION);
      }
      return this.copyFile(sourceFilePath, newFilePath);
    } else if (stat.isDirectory()) {
      let mode = force ? FILEOVERWRITE : THROWEXCEPTION;
      let copyRet = this.copyDirectory(sourceFilePath, targetFilePath, mode);
      return copyRet;
    } else {
      hilog.error(DOMAIN_CODE, TAG, 'the copy operation is not permitted');
      return this.getCopyReturnValue(sourceFileUri, targetParentUri, E_PERM, '', COPY_EXCEPTION);
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
    if (!checkUri(sourceFileUri)) {
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
    if (!checkUri(sourceFileUri)) {
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
    if (!checkUri(sourceFileUri)) {
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

    if (!checkUri(selectFileUri)) {
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

  checkRelativePath(selectFileRelativePath): boolean {
    try {
      // Processing format: The first character is '/'
      if (selectFileRelativePath !== undefined && selectFileRelativePath.indexOf('/') === 0) {
        hilog.info(DOMAIN_CODE, TAG, 'checkRelativePath-path is ' + selectFileRelativePath);
        return true;
      } else {
        hilog.error(DOMAIN_CODE, TAG, 'checkRelativePath error, path is ' + selectFileRelativePath);
        return false;
      }
    } catch (error) {
      hilog.error(DOMAIN_CODE, TAG, 'checkRelativePath error, path is ' + selectFileRelativePath);
      return false;
    }
  }

  /*
   * selectFileRelativePath formate： /storage/Users/currentUser/filename
   */
  getFileInfoFromRelativePath(selectFileRelativePath): {fileInfo:object, code:number} {
    let fileInfo = {};
    if (!this.checkRelativePath(selectFileRelativePath)) {
      return {
        fileInfo: {},
        code: E_INVAL,
      };
    }
    try {
      // Processing format: Delete the last '/'
      if (selectFileRelativePath.charAt(selectFileRelativePath.length - 1) === '/') {
        selectFileRelativePath = selectFileRelativePath.substr(0, selectFileRelativePath.length - 1);
      }
      let fileName = this.getFileName(selectFileRelativePath);
      let stat = fs.statSync(selectFileRelativePath);
      let mode = documentFlag.SUPPORTS_READ | documentFlag.SUPPORTS_WRITE;
      if (stat.isDirectory()) {
        mode |= documentFlag.REPRESENTS_DIR;
      } else {
        mode |= documentFlag.REPRESENTS_FILE;
      }
      let selectFileUri = encodeURI(this.relativePath2uri(selectFileRelativePath));
      fileInfo = {
        uri: selectFileUri,
        relativePath: selectFileRelativePath,
        fileName: fileName,
        mode: mode,
        size: stat.size,
        mtime: stat.mtime,
        mimeType: '',
      };
    } catch (e) {
      hilog.error(DOMAIN_CODE, TAG, 'getFileInfoFromRelativePath error ' + e.message);
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

  relativePath2uri(path): string {
    return `file://docs${path}`;
  }

  volumePath2uri(path): string {
    return `file://docs/storage/External/${path}`;
  }

  getRoots() {
    let roots = [
      {
        uri: 'file://docs/storage/Users/currentUser',
        displayName: 'currentUser',
        relativePath: '/storage/Users/currentUser',
        deviceType: deviceType.DEVICE_LOCAL_DISK,
        deviceFlags: deviceFlag.SUPPORTS_READ | deviceFlag.SUPPORTS_WRITE,
      },
      {
        uri: 'file://docs/storage/Share',
        displayName: 'shared_disk',
        relativePath: '/storage/Share',
        deviceType: deviceType.DEVICE_SHARED_DISK,
        deviceFlags: deviceFlag.SUPPORTS_READ | deviceFlag.SUPPORTS_WRITE,
      }
    ];
    try {
      let rootPath = '/storage/External';
      let volumeInfoList = [];
      let volumeName = fs.listFileSync(rootPath);
      for (let i = 0; i < volumeName.length; i++) {
        let volumeInfo = {
          uri: this.volumePath2uri(volumeName[i]),
          displayName: volumeName[i],
          relativePath: '/storage/External/' + volumeName[i],
          deviceType: deviceType.DEVICE_EXTERNAL_USB,
          deviceFlags: deviceFlag.SUPPORTS_READ | deviceFlag.SUPPORTS_WRITE,
        };
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
      queryResults.push(dirPath);
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

    if (!checkUri(uri)) {
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
    if (!checkUri(uri)) {
      return E_URIS;
    }
    let watchPath = getPath(uri);
    try {
      if (!observerMap.has(uri)) {
        let watcher = fs.createWatcher(watchPath, CREATE_EVENT_CODE | IN_DELETE_EVENT_CODE | DELETE_SELF_EVENT_CODE |
          MOVE_TO_CODE | MOVED_FROM_CODE | MOVE_SELF_CODE, (data) => {
          try {
            let eventCode = -1;
            eventMap.forEach((value, key) => {
              if (data.event & key) {
                eventCode = value;
              }
            });
            let targetUri = FILE_PREFIX_NAME + BUNDLE_NAME + data.fileName;
            targetUri = this.encode(targetUri);
            if (eventCode >= 0) {
              callback(targetUri, eventCode);
            } else {
              hilog.info(DOMAIN_CODE, TAG, 'eventCode =' + data.event);
            }
          } catch (error) {
            hilog.error(DOMAIN_CODE, TAG, 'onchange error ' + error.message);
          }
        });
        watcher.start();
        observerMap.set(uri, watcher);
        watcherCountMap.set(uri, 1);
      } else {
        let temp = watcherCountMap.get(uri) + 1;
        watcherCountMap.set(uri, temp);
      }
    } catch (e) {
      hilog.error(DOMAIN_CODE, TAG, 'startWatcher error ' + e.message);
      return E_GETRESULT;
    }
    return ERR_OK;
  }

  stopWatcher(uri, isUnregisterAll): number {
    uri = this.decode(uri);
    if (uri === '') {
      return {
        code: E_URIS,
      };
    }

    if (!checkUri(uri)) {
      return E_URIS;
    }
    try {
      if (!watcherCountMap.has(uri)) {
        return E_GETRESULT;
      }
      if (isUnregisterAll) {
        watcherCountMap.set(uri, 0);
      } else {
        let temp = watcherCountMap.get(uri) - 1;
        watcherCountMap.set(uri, temp);
      }
      if (watcherCountMap.get(uri) === 0) {
        watcherCountMap.delete(uri);
        let watcher = observerMap.get(uri);
        if (typeof watcher !== undefined) {
          watcher.stop();
          observerMap.delete(uri);
        }
      }
    } catch (e) {
      hilog.error(DOMAIN_CODE, TAG, 'stopWatcher error ' + e.message);
      return E_GETRESULT;
    }
    return ERR_OK;
  }
};
