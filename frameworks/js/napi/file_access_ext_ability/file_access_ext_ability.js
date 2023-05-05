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

class FileAccessExtensionAbility {
  onCreate(want) {
  }

  openFile(sourceFileUri, flags) {
    return {
      fd: -1,
      code: ERR_ERROR,
    };
  }

  createFile(parentUri, displayName) {
    return {
      uri: "",
      code: ERR_ERROR,
    };
  }

  mkdir(parentUri, displayName) {
    return {
      uri: "",
      code: ERR_ERROR,
    };
  }

  delete(selectFileUri) {
    return {
      code: ERR_ERROR,
    };
  }

  move(sourceFileUri, targetParentUri) {
    return {
      uri: '',
      code: ERR_ERROR,
    };
  }

  rename(sourceFileUri, displayName) {
    return {
      uri: '',
      code: ERR_ERROR,
    };
  }

  listFile(sourceFileUri, offset, count, filter) {
    return {
      infos: [],
      code: ERR_ERROR,
    };
  }

  getRoots() {
    return {
      roots: [],
      code: ERR_ERROR,
    };
  }

  access(sourceFileUri) {
    return {
      isExist: false,
      code: ERR_ERROR,
    };
  }

  getFileInfoFromUri(selectFileUri) {
    return {
      fileInfo: [],
      code: ERR_ERROR,
    };
  }
}

export default FileAccessExtensionAbility