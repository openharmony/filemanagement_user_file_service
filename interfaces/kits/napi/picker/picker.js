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

const PHOTO_VIEW_MIME_TYPE_MAP = new Map([
    ["image/*", "FILTER_MEDIA_TYPE_IMAGE"],
    ["video/*", "FILTER_MEDIA_TYPE_VIDEO"],
    ["*/*", "FILTER_MEDIA_TYPE_ALL"],
]);

async function photoPickerSelect() {
    if (arguments.length == 2 && typeof arguments[1] != "function") {
        console.log("[picker] photoPickerSelect callback invalid");
        throw Error("invalid callback");
    }

    let config = {
        bundleName: "com.ohos.photos",
        abilityName: "com.ohos.photos.MainAbility",
        parameters: {
            'uri': "multipleselect",
        },
    }
    if (arguments.length > 0 && typeof arguments[0] == 'object') {
        let option = arguments[0];
        if (option.maxSelectNumber != undefined) {
            config.parameters.uri = (option.maxSelectNumber == 1) ? "singleselect" : "multipleselect";
            config.parameters.maxSelectCount = option.maxSelectNumber;
        }
        if (option.MIMEType != undefined && PHOTO_VIEW_MIME_TYPE_MAP.has(option.MIMEType)) {
            config.parameters.filterMediaType = PHOTO_VIEW_MIME_TYPE_MAP.get(option.MIMEType);
        }
    }
    console.log("[picker] photoPickerSelect config: " + JSON.stringify(config));
    
    try {
        let result = await globalThis.abilityContext.startAbilityForResult(config, {windowMode: 1});
        console.log("[picker] photoPickerSelect result: " + JSON.stringify(result));
        let uri = result.want.parameters["select-item-list"];
        let isOrigin = result.want.parameters["isOriginal"];
        if (arguments.length == 2 && typeof arguments[1] == "function") {
            return arguments[1](result.resultCode, uri, isOrigin);
        } else if (arguments.length == 1 && typeof arguments[0] == "function") {
            return arguments[0](result.resultCode, uri, isOrigin);
        }
        return new Promise((resolve, reject) => {
            if (result.resultCode == 0) {
                resolve({photoUris: uri, isOriginalPhoto: isOrigin});
            } else {
                console.log("[picker] photoPickerSelect err: " + result.resultCode);
                reject(result.resultCode);
            }
        })
    } catch (error) {
        console.log("[picker] photoPickerSelect error: " + error);
    }
}

async function documentPickerSelect() {
    if (arguments.length == 2 && typeof arguments[1] != "function") {
        console.log("[picker] documentPickerSelect callback invalid");
        throw Error("invalid callback");
    }

    let config = {
        action: "ohos.want.action.OPEN_FILE",
        parameters: {
            'startMode': 'choose',
        }
    }
    console.log("[picker] documentPickerSelect config: " + JSON.stringify(config));
    
    try {
        let result = await globalThis.abilityContext.startAbilityForResult(config, {windowMode: 1});
        console.log("[picker] documentPickerSelect result: " + JSON.stringify(result));
        let uri = result.want.parameters.select_item_list;
        if (arguments.length == 2 && typeof arguments[1] == "function") {
            return arguments[1](result.resultCode, uri);
        } else if (arguments.length == 1 && typeof arguments[0] == "function") {
            return arguments[0](result.resultCode, uri);
        }
        return new Promise((resolve, reject) => {
            if (result.resultCode == 0) {
                resolve(uri);
            } else {
                console.log("[picker] documentPickerSelect err: " + result.resultCode);
                reject(result.resultCode);
            }
        })
    } catch (error) {
        console.log("[picker] documentPickerSelect error: " + error);
    }
}

async function documentPickerSave() {
    if (arguments.length == 2 && typeof arguments[1] != "function") {
        console.log("[picker] documentPickerSave callback invalid");
        throw Error("invalid callback");
    }

    let config = {
        action: "ohos.want.action.CREATE_FILE",
        parameters: {
            'startMode': 'save',
        }
    }
    if (arguments.length > 0 && typeof arguments[0] == 'object') {
        let option = arguments[0];
        if (option.newFileNames != undefined) {
            config.parameters.key_pick_file_name = JSON.stringify(option.newFileNames);
            config.parameters.saveFile = option.newFileNames[0];
        }
    }
    console.log("[picker] documentPickerSave config: " + JSON.stringify(config));
    
    try {
        let result = await globalThis.abilityContext.startAbilityForResult(config, {windowMode: 1});
        console.log("[picker] documentPickerSave result: " + JSON.stringify(result));
        let uri = result.want["parameters"].pick_path_return;
        if (arguments.length == 2 && typeof arguments[1] == "function") {
            return arguments[1](result.resultCode, uri);
        } else if (arguments.length == 1 && typeof arguments[0] == "function") {
            return arguments[0](result.resultCode, uri);
        }
        return new Promise((resolve, reject) => {
            if (result.resultCode == 0) {
                resolve(uri);
            } else {
                console.log("[picker] documentPickerSave err: " + result.resultCode);
                reject(result.resultCode);
            }
        })
    } catch (error) {
        console.log("[picker] documentPickerSave error: " + error);
    }
}

async function audioPickerSelect() {
    if (arguments.length == 2 && typeof arguments[1] != "function") {
        console.log("[picker] audioPickerSelect callback invalid");
        throw Error("invalid callback");
    }

    let config = {
        action: "ohos.want.action.OPEN_FILE",
        parameters: {
            'startMode': 'choose',
            'key_pick_location': JSON.stringify([0]),
        }
    }
    if (arguments.length > 0 && typeof arguments[0] == 'object') {
        let option = arguments[0];
        if (option.maxSelectNumber != undefined) {
            config.parameters.key_pick_num = option.maxSelectNumber;
        }
    }
    console.log("[picker] audioPickerSelect config: " + JSON.stringify(config));
    
    try {
        let result = await globalThis.abilityContext.startAbilityForResult(config, {windowMode: 1});
        console.log("[picker] audioPickerSelect result: " + JSON.stringify(result));
        let uri = result.want.parameters.select_item_list;
        if (arguments.length == 2 && typeof arguments[1] == "function") {
            return arguments[1](result.resultCode, uri);
        } else if (arguments.length == 1 && typeof arguments[0] == "function") {
            return arguments[0](result.resultCode, uri);
        }
        return new Promise((resolve, reject) => {
            if (result.resultCode == 0) {
                resolve(uri);
            } else {
                console.log("[picker] audioPickerSelect err: " + result.resultCode);
                reject(result.resultCode);
            }
        })
    } catch (error) {
        console.log("[picker] audioPickerSelect error: " + error);
    }
}

function PhotoViewPicker() {
    this.select = photoPickerSelect;
    this.save = documentPickerSave;
}
  
function DocumentViewPicker() {
    this.select = documentPickerSelect;
    this.save = documentPickerSave;
}

function AudioViewPicker() {
    this.select = audioPickerSelect;
    this.save = documentPickerSave;
}

export default {
    PhotoViewPicker : PhotoViewPicker,
    DocumentViewPicker: DocumentViewPicker,
    AudioViewPicker : AudioViewPicker,
}