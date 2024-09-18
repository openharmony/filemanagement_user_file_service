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
const BUNDLE_NAME = 'docs';
const DOMAIN_CODE = 0x0001;
const SLICE_PREFIX_URI = 12;
const TAG = 'ExternalFileManager';
const FILE_PREFIX_NAME = 'file://';

function checkUri(uri: string): boolean {
  try {
    if (uri?.indexOf(FILE_PREFIX_NAME) === 0) {
      hilog.info(DOMAIN_CODE, TAG, 'uri is ' + uri);
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

function getPath(uri): string {
  let sep = '://';
  let arr = uri.split(sep);
  let minLength = 2;
  if (arr.length < minLength) {
    hilog.error(DOMAIN_CODE, TAG, 'getPath-parameter-uri format exception, uri is' + uri);
    return '';
  }
  let path = uri.replace(arr[0] + sep, '');
  if (arr[1].indexOf('/') > 0 && arr[1].split('/')[0] === BUNDLE_NAME) {
    path = path.replace(arr[1].split('/')[0], '');
  } else {
    hilog.error(DOMAIN_CODE, TAG, 'getPath-parameter-uri format exception, uri is ' + uri);
    return '';
  }

  if (path.charAt(path.length - 1) === '/') {
    path = path.substr(0, path.length - 1);
  }
  hilog.info(DOMAIN_CODE, TAG, 'getPath after ' + path);
  return path;
}

interface Fileinfo {
  uri: string,
  relativePath: string,
  fileName: string,
  mode: number,
  size: number,
  mtime: number,
  mimeType: string
}

function uriReturnObject(uri: string, code: number) {
  return {
    uri: uri,
    code: code
  };
}

function infosReturnObject(infos: Fileinfo[], code: number): {infos: Fileinfo[], code: number} {
  return {
    infos: infos,
    code: code
  };
}

function fdReturnObject(fd: number, code: number): {fd: number, code: number} {
  return {
    fd: fd,
    code: code
  };
}

function boolReturnObject(isExist: boolean, code: number): {isExist: boolean, code: number} {
  return {
    isExist: isExist,
    code: code
  };
}

function fileinfoReturnObject(fileInfo: object, code: number): {fileInfo: object, code: number} {
  return {
    fileInfo: fileInfo,
    code: code
  };
}

function resultsResultObject(results: object, code: number): {results: object, code: number} {
  return {
    results: results,
    code: code
  };
}

function rootsReturnObject(roots: object, code: number): {roots: object, code: number} {
  return {
    roots: roots,
    code: code
  };
}

function encodePathOfUri(uri): string {  
  try {
    let suffixUri = uri.slice(SLICE_PREFIX_URI, uri.length);
    let prefixUri = uri.slice(0, SLICE_PREFIX_URI); 
    uri = prefixUri.concat(encodeURIComponent(suffixUri).replace(/%2F/g, '/'));
  } catch (e) {
    hilog.error(DOMAIN_CODE, TAG, 'The reason of encodeURIComponent: ' + e.message + ' code: ' + e.code);
    uri = '';
  }
  return uri;
}
 
function decodeUri(uri): string {
  try {
    uri = decodeURIComponent(uri);
  } catch (e) {
    hilog.error(DOMAIN_CODE, TAG, 'The reason of decodeURIComponent: ' + e.message + ' code: ' + e.code);
    uri = '';
  }
  return uri;
}

export { 
  getPath, checkUri, encodePathOfUri, decodeUri, uriReturnObject, infosReturnObject, fdReturnObject, boolReturnObject, resultsResultObject,
  fileinfoReturnObject, rootsReturnObject, BUNDLE_NAME, DOMAIN_CODE, FILE_PREFIX_NAME, TAG 
};

export type { Fileinfo };