/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
import hilog from '@ohos.hilog';
import fs from '@ohos.file.fs';
import type { Filter } from '@ohos.file.fs';
import fileExtensionInfo from '@ohos.file.fileExtensionInfo';
import type { Fileinfo } from './Common';
import { getPath, uriReturnObject, encodePathOfUri, DOMAIN_CODE, TAG, infosReturnObject } from './Common';

const documentFlag = fileExtensionInfo.DocumentFlag;
const ERR_OK = 0;
const E_GETRESULT = 14300004;
const APP_DATA = 'appdata';
const CURRENT_USER_PATH = '/storage/Users/currentUser';

function hasFilter(filter: Filter) : boolean {
  if (filter === null) {
    return false;
  }
  let displayNameArray = filter.displayName;
  if (displayNameArray !== null && displayNameArray.length > 0) {
    return true;
  }
  let suffixArray = filter.suffix;
  if (suffixArray !== null && suffixArray.length > 0) {
    return true;
  }
  let lastModifiedAfter = filter.lastModifiedAfter;
  if (lastModifiedAfter !== null && lastModifiedAfter >= 0) {
    return true;
  }
  let fileSizeOver = filter.fileSizeOver;
  if (fileSizeOver !== null && fileSizeOver >= 0) {
    return true;
  }
  return false;
}

function buildDisplayName(displayNameArray: string[]) : string[] {
  let displayNames : string[] = [];
  for (let i = 0; i < displayNameArray.length; i++) {
    if (displayNameArray[i].lastIndexOf('*') === -1) {
      let name = '*' + displayNameArray[i];
      displayNames.push(name);
    } else {
      displayNames.push(displayNameArray[i]);
    }
  }
  return displayNames;
}

function buildFilterOptions(filter: Filter, listNum: number, recursion: boolean) :
{recursion: boolean, listNum: number, filter: Filter} {
  let optionFilter: Filter = {};
  if (filter !== null) {
    let suffixArray = filter.suffix;
    if (suffixArray !== null && suffixArray.length > 0) {
      optionFilter.suffix = suffixArray;
    }
    let displayNameArray = filter.displayName;
    if (displayNameArray !== null && displayNameArray.length > 0) {
      optionFilter.displayName = buildDisplayName(displayNameArray);
    }
    let fileSizeOver = filter.fileSizeOver;
    if (fileSizeOver !== null && fileSizeOver >= 0) {
      optionFilter.fileSizeOver = fileSizeOver;
    }
    let lastModifiedAfter = filter.lastModifiedAfter;
    if (lastModifiedAfter !== null && lastModifiedAfter >= 0) {
      optionFilter.lastModifiedAfter = lastModifiedAfter;
    }
    let mimeType = filter.mimeType;
    if (mimeType !== null && mimeType.length > 0) {
      hilog.error(DOMAIN_CODE, TAG, 'mimeType is not supported as a filter condition');
    }
    let excludeMedia = filter.excludeMedia;
    if (excludeMedia !== null && excludeMedia === true) {
      hilog.error(DOMAIN_CODE, TAG, 'excludeMedia is not supported as a filter condition');
    }
  }
  let options = {
    'recursion': recursion,
    'listNum': listNum,
    'filter': optionFilter,
  };
  return options;
}

function buildNoFilterOptions(listNum: number, recursion: boolean) : {recursion: boolean, listNum: number} {
  let options = {
    'recursion': recursion,
    'listNum': listNum,
  };
  return options;
}

function getNewPathOrUri(prefixSection: string, filename: string) : string {
  let completeResult = prefixSection;
  if (completeResult.endsWith('/')) {
    if (filename.startsWith('/')) {
      filename = filename.substring(1, filename.length);
    }
    completeResult += filename;
  } else {
    if (!filename.startsWith('/')) {
      completeResult += '/';
    }
    completeResult += filename;
  }
  return completeResult;
}

function genNewFileName(filename: string): string {
  let newFilename = filename;
  let index = newFilename.lastIndexOf('/');
  if (index !== -1) {
    newFilename = newFilename.substring(index + 1, newFilename.length);
  }
  return newFilename;
}

function getListFileInfos(sourceFileUri: string, offset: number, count: number, filter: Filter, recursion: boolean) :
{infos: Fileinfo[], code: number} {
  let infos : Fileinfo[] = [];
  let path = getPath(sourceFileUri);
  try {
    let statPath = fs.statSync(path);
    if (!statPath.isDirectory()) {
      return infosReturnObject([], E_GETRESULT);
    }
    let options;
    let listNum = offset + count;
    if (hasFilter(filter)) {
      options = buildFilterOptions(filter, listNum, recursion);
    } else {
      options = buildNoFilterOptions(listNum, recursion);
    }
    let fileNameList = fs.listFileSync(path, options);
    for (let i = offset; i < fileNameList.length; i++) {
      if (i === listNum) {
        break;
      }
      if (path === CURRENT_USER_PATH && fileNameList[i] === APP_DATA) {
        hilog.info(DOMAIN_CODE, TAG, `filter appdata doc`);
        continue;
      }
      let mode = documentFlag.SUPPORTS_READ | documentFlag.SUPPORTS_WRITE;
      let filePath = getNewPathOrUri(path, fileNameList[i]);
      let stat = fs.statSync(filePath);
      if (stat.isDirectory()) {
        mode |= documentFlag.REPRESENTS_DIR;
      } else {
        mode |= documentFlag.REPRESENTS_FILE;
      }
      let newFileUri = getNewPathOrUri(sourceFileUri, fileNameList[i]);
      newFileUri = encodePathOfUri(newFileUri);
      infos.push({ uri: newFileUri, relativePath: filePath, fileName: genNewFileName(fileNameList[i]),
        mode: mode, size: stat.size, mtime: stat.mtime, mimeType: '' });
    }
  } catch (e) {
    hilog.error(DOMAIN_CODE, TAG, `getFileInfos error: ${e.message},code: ${e.code}`);
    return infosReturnObject([], E_GETRESULT);
  }
  return infosReturnObject(infos, ERR_OK);
}

function getSubUriList(path: string, listNum: number, filter: Filter) : string[]
{
  let dirOptions = {
    'recursion': false
  };
  let fileOption = hasFilter(filter) ?
      buildFilterOptions(filter, listNum, false) : buildNoFilterOptions(listNum, false);
  let dirTmpResult = fs.listFileSync(path, dirOptions).filter(item => item !== APP_DATA).map(function(item) {
    return CURRENT_USER_PATH + '/' + item;
  });
  let fileResult = fs.listFileSync(path, fileOption).filter(item => item !== APP_DATA).map(function(item) {
    return CURRENT_USER_PATH + '/' + item;
  });
  let dirResult : string[] = [];
  for (let i = 0; i < dirTmpResult.length; ++i) {
    if (fs.statSync(dirTmpResult[i]).isDirectory()) {
      dirResult.push(dirTmpResult[i]);
    }
  }
  return Array.from(new Set(fileResult.concat(dirResult)));
}

function getSubFileInfos(
  changeData: {
    options: {recursion: boolean, listNum: number, filter?: Filter},
    tempOffset: number,
    listNumCnt: number
  }, needInfo: {subPath: string, count: number, isRootPath: boolean, sourceFileUri: string}): Fileinfo[] {
  let infos: Fileinfo[] = [];
  let tmpStat = fs.statSync(needInfo.subPath);
  let bIsDct = tmpStat.isDirectory();
  let fileNameList = bIsDct ? fs.listFileSync(needInfo.subPath, changeData.options) :
      [needInfo.subPath.substring(CURRENT_USER_PATH.length)];
  let subPath = needInfo.subPath;
  if (needInfo.isRootPath) {
    subPath = bIsDct ? subPath.substring(CURRENT_USER_PATH.length) : '';
  }
  let listLen = fileNameList.length;
  if (changeData.tempOffset >= listLen) {
    changeData.tempOffset -= listLen;
    return infos;
  }
  for (let j = changeData.tempOffset; j < fileNameList.length; ++j, ++changeData.listNumCnt) {
    if (changeData.listNumCnt === needInfo.count) {
      break;
    }
    let mode = documentFlag.SUPPORTS_READ | documentFlag.SUPPORTS_WRITE;
    let filePath = getNewPathOrUri(needInfo.subPath, fileNameList[j]);
    let stat = bIsDct ? fs.statSync(filePath) : tmpStat;
    mode |= (bIsDct | stat.isDirectory()) ? documentFlag.REPRESENTS_DIR : documentFlag.REPRESENTS_FILE;
    let newFileUri = getNewPathOrUri(
      needInfo.isRootPath ? needInfo.sourceFileUri + subPath : needInfo.sourceFileUri, fileNameList[j]);
    newFileUri = encodePathOfUri(newFileUri);
    infos.push({ uri: newFileUri, relativePath: filePath, fileName: genNewFileName(fileNameList[j]),
      mode: mode, size: stat.size, mtime: stat.mtime, mimeType: '' });
  }
  changeData.tempOffset = 0;
  changeData.options.listNum = needInfo.count - changeData.listNumCnt;
  return infos;
}

function getScanFileInfos(sourceFileUri: string, offset: number, count: number, filter: Filter, recursion: boolean) :
{infos: Fileinfo[], code: number} {
  let infos : Fileinfo[] = [];
  let path = getPath(sourceFileUri);
  try {
    let statPath = fs.statSync(path);
    if (!statPath.isDirectory()) {
      return infosReturnObject([], E_GETRESULT);
    }
    let listNum = offset + count;
    let isRootPath = (path === CURRENT_USER_PATH);
    let listInfo = (isRootPath) ? getSubUriList(path, listNum, filter) : [path];
    let changeData = {
      options: hasFilter(filter) ?
        buildFilterOptions(filter, listNum, recursion) : buildNoFilterOptions(listNum, recursion),
      tempOffset: offset,
      listNumCnt: 0
    };
    for (let i = 0; i < listInfo.length; ++i) {
      const needInfo = {
        subPath: listInfo[i],
        count: count,
        isRootPath: isRootPath,
        sourceFileUri: sourceFileUri
      };
      let subFileRes = getSubFileInfos(changeData, needInfo);
      infos.push(...subFileRes);
      if (changeData.options.listNum <= 0) {
        break;
      }
    }
  } catch (e) {
    hilog.error(DOMAIN_CODE, TAG, `getFileInfos error: ${e.message},code: ${e.code}`);
    return infosReturnObject([], E_GETRESULT);
  }
  return infosReturnObject(infos, ERR_OK);
}
export { getListFileInfos, getScanFileInfos, buildFilterOptions, buildNoFilterOptions, hasFilter };
