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
import Extension from '@ohos.application.FileExtensionAbility'

export default class FileExtAbility extends Extension {
    onCreate(want) {
        console.log('js server tag dsa FileExtAbility onCreate, want:' + want.abilityName);
        console.log('js server tag dsa FileExtAbility this.context:' + this.context);
    }

    openFile(uri, mode) {
        console.log('js server tag dsa openFile, uri:' + uri + ',mode:' + mode);
        return 2;
    }

    closeFile(fd, uri) {
        console.log('js server tag dsa delete, fd:' + fd + ",uri: " + uri);
        return 268;
    }

    createFile(parentUri, displayName) {
        console.log('js server tag dsa CreateFile, parentUri:' + parentUri + ',displayName:' + displayName );
        return "filetest://fileext.share/temp/test/CreateFile001.txt";
    }

    mkdir(parentUri, displayName) {
        console.log('js server tag dsa mkdir, parentUri:' + parentUri + ',displayName:' + displayName);
        return "filetest://fileext.share/temp/test/Mkdir001";
    }

    delete(sourceFileUri) {
        console.log('js server tag dsa delete, sourceFileUri:' + sourceFileUri);
        return 132;
    }

    move(sourceFileUri, targetParentUri) {
        console.log('js server tag dsa move, sourceFileUri:' + sourceFileUri + ',targetParentUri:' + targetParentUri);
        return "filetest://fileext.share/temp/test/move001.xl";
    }

    rename(sourceFileUri, displayName) {
        console.log('js server tag dsa rename, sourceFileUri:' + sourceFileUri + ',displayName:' + displayName);
        return "filetest://fileext.share/temp/test/rename001.ttt";
    }
};