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
import { getPath, uriReturnObject, DOMAIN_CODE, TAG, infosReturnObject } from './Common';

const documentFlag = fileExtensionInfo.DocumentFlag;
const ERR_OK = 0;
const E_GETRESULT = 14300004;

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

function getFileInfos(sourceFileUri: string, offset: number, count: number, filter: Filter, recursion: boolean) :
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
    for (let i = 0; i < fileNameList.length; i++) {
      if (offset > i) {
        continue;
      }
      if (i === listNum) {
        break;
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
      newFileUri = encodeURI(newFileUri);
      infos.push({
        uri: newFileUri,
        relativePath: filePath,
        fileName: genNewFileName(fileNameList[i]),
        mode: mode,
        size: stat.size,
        mtime: stat.mtime,
        mimeType: '',
      });
    }
  } catch (e) {
    hilog.error(DOMAIN_CODE, TAG, `getFileInfos error: ${e.message},code: ${e.code}`);
    return infosReturnObject([], E_GETRESULT);
  }
  return infosReturnObject(infos, ERR_OK);
}
export { getFileInfos };