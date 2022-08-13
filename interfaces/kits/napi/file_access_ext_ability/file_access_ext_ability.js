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

    registerCallback(callback) {
    }

    openFile(uri, mode) {
        return 0;
    }

    createFile(parentUri, displayName) {
        return "";
    }

    mkdir(parentUri, displayName) {
        return "";
    }

    delete(sourceFileUri) {
        return 0;
    }

    move(sourceFileUri, targetParentUri) {
        return "";
    }

    rename(sourceFileUri, displayName) {
        return "";
    }

    listFile(sourceFileUri) {
        let infos = [];
        return infos;
    }

    getRoots() {
        let roots = [];
        return roots;
    }

    isFileExist() {
        return true;
    }
}

export default FileAccessExtensionAbility