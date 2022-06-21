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

class FileExtensionAbility {
    onCreate(want) {
        console.log('js c++ tag dsa onCreate, want:' + want.abilityName);
    }

    openFile(uri, mode) {
        console.log('js c++ tag dsa openFile, uri:' + uri + ',flags: ' + flags);
        return 0;
    }

    createFile(parentUri, displayName) {
        console.log('js c++ tag dsa CreateFile, parentUri:' + parentUri + ',displayName:' + displayName );
        return "filetest://fileext.share/temp/test/CreateFile000.txt";
    }

    mkdir(parentUri, displayName) {
        console.log('js c++ tag dsa mkdir, parentUri:' + parentUri + ',displayName:' + displayName);
        return "filetest://fileext.share/temp/test/Mkdir000";
    }

    delete(sourceFileUri) {
        console.log('js c++ tag dsa delete, sourceFileUri:' + sourceFileUri);
        return 0;
    }

    move(sourceFileUri, targetParentUri) {
        console.log('js c++ tag dsa move, sourceFileUri:' + sourceFileUri + ',targetParentUri:' + targetParentUri);
        return "filetest://fileext.share/temp/test/move000.xl";
    }
    
    rename(sourceFileUri, displayName) {
        console.log('js c++ tag dsa rename, sourceFileUri:' + sourceFileUri + ',displayName:' + displayName);
        return "filetest://fileext.share/temp/test/rename000.ttt";
    }

    listFile(sourceFileUri) {
        let infos = [];
        return infos;
    }

    getRoots() {
        let roots = [];
        return roots;
    }
}

export default FileExtensionAbility