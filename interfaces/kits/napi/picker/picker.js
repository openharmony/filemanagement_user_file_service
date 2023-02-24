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

const PhotoViewMIMETypes = {
    IMAGE_TYPE: "image/*",
    VIDEO_TYPE: "video/*",
    IMAGE_VIDEO_TYPE: "*/*",
    INVALID_TYPE: ""
}

const ErrCode = {
    INVALID_ARGS: 13900020,
    RESULT_ERROR: 13900042,
}

const ERRCODE_MAP = new Map([
    [ErrCode.INVALID_ARGS, "Invalid argument"],
    [ErrCode.RESULT_ERROR, "Unknown error"],
]);

const PHOTO_VIEW_MIME_TYPE_MAP = new Map([
    [PhotoViewMIMETypes.IMAGE_TYPE, "FILTER_MEDIA_TYPE_IMAGE"],
    [PhotoViewMIMETypes.VIDEO_TYPE, "FILTER_MEDIA_TYPE_VIDEO"],
    [PhotoViewMIMETypes.IMAGE_VIDEO_TYPE, "FILTER_MEDIA_TYPE_ALL"],
]);

const CREATE_FILE_NAME_LENGTH_LIMIT = 256;

function checkArguments(args) {
    if (args.length == 2 && typeof args[1] != "function") {
        return false;
    }

    if (args.length > 0 && typeof args[0] == 'object') {
        let option = args[0];
        if (option.maxSelectNumber != undefined) {
            if (option.maxSelectNumber.toString().indexOf(".") != -1) {
                return false;
            }
        }
        
        if (option.newFileNames != undefined && option.newFileNames.length > 0) {
            for (let i = 0; i < option.newFileNames.length; i++) {
                let value = option.newFileNames[i];
                if ((value.indexOf(".") == -1) || (value.length > CREATE_FILE_NAME_LENGTH_LIMIT)) {
                    console.log("[picker] checkArguments Invalid name: " + value);
                    return false;
                }
            }
        }
    }

    return true;
}

function getErr(errCode) {
    return {code: errCode, message: ERRCODE_MAP.get(errCode)};
}

async function photoPickerSelect() {
    if (!checkArguments(arguments)) {
        console.log("[picker] Invalid argument");
        throw Error(getErr(ErrCode.INVALID_ARGS));
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
        if (option.maxSelectNumber > 0) {
            config.parameters.uri = (option.maxSelectNumber == 1) ? "singleselect" : "multipleselect";
            config.parameters.maxSelectCount = option.maxSelectNumber;
        }
        if (option.MIMEType.length > 0 && PHOTO_VIEW_MIME_TYPE_MAP.has(option.MIMEType)) {
            config.parameters.filterMediaType = PHOTO_VIEW_MIME_TYPE_MAP.get(option.MIMEType);
        }
    }
    console.log("[picker] config: " + JSON.stringify(config));

    try {
        let result = await globalThis.abilityContext.startAbilityForResult(config, {windowMode: 1});
        console.log("[picker] result: " + JSON.stringify(result));
        let uris = result.want.parameters["select-item-list"];
        let isOrigin = result.want.parameters["isOriginal"];
        if (result.resultCode == -1) {
            result.resultCode = 0;
            uris = [];
        }
        let err = (result.resultCode == 0) ? null : getErr(ErrCode.RESULT_ERROR);
        let selectResult = (result.resultCode == 0) ? new PhotoSelectResult(uris, isOrigin) : null;
        if (arguments.length == 2 && typeof arguments[1] == "function") {
            return arguments[1](err, selectResult);
        } else if (arguments.length == 1 && typeof arguments[0] == "function") {
            return arguments[0](err, selectResult);
        }
        return new Promise((resolve, reject) => {
            if (result.resultCode == 0) {
                resolve(selectResult);
            } else {
                console.log("[picker] err: " + result.resultCode);
                reject(result.resultCode);
            }
        })
    } catch (error) {
        console.log("[picker] error: " + error);
    }
}

async function documentPickerSelect() {
    if (!checkArguments(arguments)) {
        console.log("[picker] Invalid argument");
        throw Error(getErr(ErrCode.INVALID_ARGS));
    }

    let config = {
        action: "ohos.want.action.OPEN_FILE",
        parameters: {
            'startMode': 'choose',
        }
    }
    console.log("[picker] config: " + JSON.stringify(config));

    try {
        let result = await globalThis.abilityContext.startAbilityForResult(config, {windowMode: 1});
        console.log("[picker] result: " + JSON.stringify(result));
        let uris = result.want.parameters.select_item_list;
        if (result.resultCode == -1) {
            result.resultCode = 0;
            uris = [];
        }
        let err = (result.resultCode == 0) ? null : getErr(ErrCode.RESULT_ERROR);
        let uriResult = (result.resultCode == 0) ? uris : null;
        if (arguments.length == 2 && typeof arguments[1] == "function") {
            return arguments[1](err, uriResult);
        } else if (arguments.length == 1 && typeof arguments[0] == "function") {
            return arguments[0](err, uriResult);
        }
        return new Promise((resolve, reject) => {
            if (result.resultCode == 0) {
                resolve(uris);
            } else {
                console.log("[picker] err: " + result.resultCode);
                reject(result.resultCode);
            }
        })
    } catch (error) {
        console.log("[picker] error: " + error);
    }
}

async function documentPickerSave() {
    if (!checkArguments(arguments)) {
        console.log("[picker] Invalid argument");
        throw Error({code: 13900020, message: "Invalid argument"});
    }

    let config = {
        action: "ohos.want.action.CREATE_FILE",
        parameters: {
            'startMode': 'save',
        }
    }
    if (arguments.length > 0 && typeof arguments[0] == 'object') {
        let option = arguments[0];
        if (option.newFileNames.length > 0) {
            config.parameters.key_pick_file_name = JSON.stringify(option.newFileNames);
            config.parameters.saveFile = option.newFileNames[0];
        }
    }
    console.log("[picker] config: " + JSON.stringify(config));

    try {
        let result = await globalThis.abilityContext.startAbilityForResult(config, {windowMode: 1});
        console.log("[picker] result: " + JSON.stringify(result));
        let uris = result.want["parameters"].pick_path_return;
        if (result.resultCode == -1) {
            result.resultCode = 0;
            uris = [];
        }
        let err = (result.resultCode == 0) ? null : getErr(ErrCode.RESULT_ERROR);
        let uriResult = (result.resultCode == 0) ? uris : null;
        if (arguments.length == 2 && typeof arguments[1] == "function") {
            return arguments[1](err, uriResult);
        } else if (arguments.length == 1 && typeof arguments[0] == "function") {
            return arguments[0](err, uriResult);
        }
        return new Promise((resolve, reject) => {
            if (result.resultCode == 0) {
                resolve(uris);
            } else {
                console.log("[picker] err: " + result.resultCode);
                reject(result.resultCode);
            }
        })
    } catch (error) {
        console.log("[picker] error: " + error);
    }
}

async function audioPickerSelect() {
    if (!checkArguments(arguments)) {
        console.log("[picker] Invalid argument");
        throw Error({code: 13900020, message: "Invalid argument"});
    }

    let config = {
        action: "ohos.want.action.OPEN_FILE",
        parameters: {
            'startMode': 'choose',
        }
    }
    console.log("[picker] config: " + JSON.stringify(config));

    try {
        let result = await globalThis.abilityContext.startAbilityForResult(config, {windowMode: 1});
        console.log("[picker] result: " + JSON.stringify(result));
        let uris = result.want.parameters.select_item_list;
        if (result.resultCode == -1) {
            result.resultCode = 0;
            uris = [];
        }
        let err = (result.resultCode == 0) ? null : getErr(ErrCode.RESULT_ERROR);
        let uriResult = (result.resultCode == 0) ? uris : null;
        if (arguments.length == 2 && typeof arguments[1] == "function") {
            return arguments[1](err, uriResult);
        } else if (arguments.length == 1 && typeof arguments[0] == "function") {
            return arguments[0](err, uriResult);
        }
        return new Promise((resolve, reject) => {
            if (result.resultCode == 0) {
                resolve(uris);
            } else {
                console.log("[picker] err: " + result.resultCode);
                reject(result.resultCode);
            }
        })
    } catch (error) {
        console.log("[picker] error: " + error);
    }
}

function PhotoSelectOptions() {
    this.MIMEType = PhotoViewMIMETypes.INVALID_TYPE;
    this.maxSelectNumber = -1;
}

function PhotoSelectResult(uris, isOriginalPhoto) {
    this.photoUris = uris;
    this.isOriginalPhoto = isOriginalPhoto;
}

function PhotoSaveOptions() {
    this.newFileNames = [];
}

function DocumentSelectOptions() {}

function DocumentSaveOptions() {
    this.newFileNames = [];
}

function AudioSelectOptions() {}

function AudioSaveOptions() {
    this.newFileNames = [];
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
    PhotoViewMIMETypes : PhotoViewMIMETypes,
    PhotoSelectOptions : PhotoSelectOptions,
    PhotoSelectResult : PhotoSelectResult,
    PhotoSaveOptions : PhotoSaveOptions,
    DocumentSelectOptions : DocumentSelectOptions,
    DocumentSaveOptions : DocumentSaveOptions,
    AudioSelectOptions : AudioSelectOptions,
    AudioSaveOptions : AudioSaveOptions,
    PhotoViewPicker : PhotoViewPicker,
    DocumentViewPicker: DocumentViewPicker,
    AudioViewPicker : AudioViewPicker,
}