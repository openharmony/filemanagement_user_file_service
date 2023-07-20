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
const BUNDLE_NAME = 'docs';
const DOMAIN_CODE = 0x0001;

function getPath(uri): string {
  let sep = '://';
  let arr = uri.split(sep);
  let minLength = 2;
  if (arr.length < minLength) {
    return uri;
  }
  let path = uri.replace(arr[0] + sep, '');
  if (arr[1].indexOf('/') > 0) {
    path = path.replace(arr[1].split('/')[0], '');
  }
  path = path.replace('/' + BUNDLE_NAME, '');
  if (path.charAt(path.length - 1) === '/') {
    path = path.substr(0, path.length - 1);
  }
  return path;
}

interface Fileinfo {
  uri: string,
  fileName: string,
  mode: number,
  size: number,
  mtime: number,
  mimeType: string
}

export { getPath, BUNDLE_NAME, DOMAIN_CODE };
export type { Fileinfo };