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
const TAG = 'ExternalFileManager';
const FILE_PREFIX_NAME = 'file://';

function checkUri(uri): boolean {
  try {
    if (uri.indexOf(FILE_PREFIX_NAME) === 0) {
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
  hilog.info(DOMAIN_CODE, TAG, 'getPath before0 ' + uri);
  let sep = '://';
  let arr = uri.split(sep);
  let minLength = 2;
  if (arr.length < minLength) {
    hilog.error(DOMAIN_CODE, TAG, 'getPath-parameter-uri format exception, uri is' + uri);
    return '';
  }
  let path = uri.replace(arr[0] + sep, '');
  if (arr[1].indexOf('/') > 0 && arr[1].split('/')[0] == BUNDLE_NAME) {
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

export { getPath, checkUri, BUNDLE_NAME, DOMAIN_CODE, FILE_PREFIX_NAME, TAG };
export type { Fileinfo };