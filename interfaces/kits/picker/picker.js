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
  IMAGE_TYPE: 'image/*',
  VIDEO_TYPE: 'video/*',
  IMAGE_VIDEO_TYPE: '*/*',
  INVALID_TYPE: ''
}

const ErrCode = {
  INVALID_ARGS: 13900020,
  RESULT_ERROR: 13900042,
  NAME_TOO_LONG: 13900030,
}

const ERRCODE_MAP = new Map([
  [ErrCode.INVALID_ARGS, 'Invalid argument'],
  [ErrCode.RESULT_ERROR, 'Unknown error'],
  [ErrCode.NAME_TOO_LONG, 'File name too long'],
]);

const PHOTO_VIEW_MIME_TYPE_MAP = new Map([
  [PhotoViewMIMETypes.IMAGE_TYPE, 'FILTER_MEDIA_TYPE_IMAGE'],
  [PhotoViewMIMETypes.VIDEO_TYPE, 'FILTER_MEDIA_TYPE_VIDEO'],
  [PhotoViewMIMETypes.IMAGE_VIDEO_TYPE, 'FILTER_MEDIA_TYPE_ALL'],
]);

const ACTION = {
  SELECT_ACTION: 'ohos.want.action.OPEN_FILE',
  SELECT_ACTION_MODAL: 'ohos.want.action.OPEN_FILE_SERVICE',
  SAVE_ACTION: 'ohos.want.action.CREATE_FILE',
  SAVE_ACTION_MODAL: 'ohos.want.action.CREATE_FILE_SERVICE',
}

const CREATE_FILE_NAME_LENGTH_LIMIT = 256;
const ARGS_ZERO = 0;
const ARGS_ONE = 1;
const ARGS_TWO = 2;

/*
* UTF-8字符编码数值对应的存储长度：
* 0000 - 0x007F (eg: a~z A~Z 0~9）
* 0080 - 0x07FF (eg: 希腊字母）
* 0800 - 0xFFFF (eg: 中文）
* 其他 (eg: 平面符号）
*/
function strSizeUTF8(str) {
  let strLen = str.length;
  let bytesLen = 0;
  for (let i = 0; i < strLen; i++) {
    let charCode = str.charCodeAt(i);
    if (charCode <= 0x007f) {
      bytesLen++;
    } else if (charCode <= 0x07ff) {
      bytesLen += 2;
    } else if (charCode <= 0xffff) {
      bytesLen += 3;
    } else {
      bytesLen += 4;
    }
  }
  return bytesLen;
}

function checkArguments(args) {
  let checkArgumentsResult = undefined;

  if (args.length === ARGS_TWO && typeof args[ARGS_ONE] !== 'function') {
    checkArgumentsResult = getErr(ErrCode.INVALID_ARGS);
  }

  if (args.length > 0 && typeof args[ARGS_ZERO] === 'object') {
    let option = args[ARGS_ZERO];
    if (option.maxSelectNumber !== undefined) {
      if (option.maxSelectNumber.toString().indexOf('.') !== -1) {
        checkArgumentsResult = getErr(ErrCode.INVALID_ARGS);
      }
    }

    if (option.newFileNames !== undefined && option.newFileNames.length > 0) {
      for (let i = 0; i < option.newFileNames.length; i++) {
        let value = option.newFileNames[i];
        if (strSizeUTF8(value) >= CREATE_FILE_NAME_LENGTH_LIMIT) {
          console.log('[picker] checkArguments Invalid name: ' + value);
          checkArgumentsResult = getErr(ErrCode.NAME_TOO_LONG);
        }
      }
    }
  }

  return checkArgumentsResult;
}

function getErr(errCode) {
  return {code: errCode, message: ERRCODE_MAP.get(errCode)};
}

function parsePhotoPickerSelectOption(args) {
  let config = {
    action: 'ohos.want.action.photoPicker',
    type: 'multipleselect',
    parameters: {
      uri: 'multipleselect',
    },
  };

  if (args.length > ARGS_ZERO && typeof args[ARGS_ZERO] === 'object') {
    let option = args[ARGS_ZERO];
    if (option.maxSelectNumber && option.maxSelectNumber > 0) {
      let select = (option.maxSelectNumber === 1) ? 'singleselect' : 'multipleselect';
      config.type = select;
      config.parameters.uri = select;
      config.parameters.maxSelectCount = option.maxSelectNumber;
    }
    if (option.MIMEType && PHOTO_VIEW_MIME_TYPE_MAP.has(option.MIMEType)) {
      config.parameters.filterMediaType = PHOTO_VIEW_MIME_TYPE_MAP.get(option.MIMEType);
    }
  }

  return config;
}

function getPhotoPickerSelectResult(args) {
  let selectResult = {
    error: undefined,
    data: undefined,
  };

  if (args.resultCode === 0) {
    if (args.want && args.want.parameters) {
      let uris = args.want.parameters['select-item-list'];
      let isOrigin = args.want.parameters.isOriginal;
      selectResult.data = new PhotoSelectResult(uris, isOrigin);
    }
  } else if (result.resultCode === -1) {
    selectResult.data = new PhotoSelectResult([], undefined);
  } else {
    selectResult.error = getErr(ErrCode.RESULT_ERROR);
  }

  return selectResult;
}

async function photoPickerSelect(...args) {
  let checkArgsResult = checkArguments(args);
  if (checkArgsResult !== undefined) {
    console.log('[picker] Invalid argument');
    throw checkArgsResult;
  }

  const config = parsePhotoPickerSelectOption(args);
  console.log('[picker] config: ' + JSON.stringify(config));

  try {
    let context = getContext(this);
    let result = await context.startAbilityForResult(config, {windowMode: 1});
    console.log('[picker] result: ' + JSON.stringify(result));
    const selectResult = getPhotoPickerSelectResult(result);
    console.log('[picker] selectResult: ' + JSON.stringify(selectResult));
    if (args.length === ARGS_TWO && typeof args[ARGS_ONE] === 'function') {
      return args[ARGS_ONE](selectResult.error, selectResult.data);
    } else if (args.length === ARGS_ONE && typeof args[ARGS_ZERO] === 'function') {
      return args[ARGS_ZERO](selectResult.error, selectResult.data);
    }
    return new Promise((resolve, reject) => {
      if (selectResult.data !== undefined) {
        resolve(selectResult.data);
      } else {
        reject(selectResult.error);
      }
    })
  } catch (error) {
    console.log('[picker] error: ' + error);
  }
  return undefined;
}

function parseDocumentPickerSelectOption(args, action) {
  let config = {
    action: action,
    parameters: {
      startMode: 'choose',
    }
  };

  if (args.length > ARGS_ZERO && typeof args[ARGS_ZERO] === 'object') {
    let option = args[ARGS_ZERO];

    if ((option.maxSelectNumber !== undefined) && option.maxSelectNumber > 0) {
      config.parameters.key_pick_num = option.maxSelectNumber;
    }
    if (option.defaultFilePathUri !== undefined) {
      config.parameters.key_pick_dir_path = option.defaultFilePathUri;
    }
    if ((option.fileSuffixFilters !== undefined) && option.fileSuffixFilters.length > 0) {
      config.parameters.key_file_suffix_filter = option.fileSuffixFilters;
    }
  }

  console.log('[picker] Select config: ' + JSON.stringify(config));
  return config;
}

function getDocumentPickerSelectResult(args) {
  let selectResult = {
    error: undefined,
    data: undefined
  };
  // 0:success
  // -1:Non modal cancel
  // 1:Modal cancel
  // ResultCode is a non modal return code.
  // Result is the return code of the modality.
  if ((args.resultCode !== undefined && args.resultCode === 0) || (args.result !== undefined && args.result === 0)) {
    if (args.want && args.want.parameters) {
      selectResult.data = args.want.parameters.select_item_list;
    }
  } else if ((args.resultCode !== undefined && args.resultCode === -1) || (args.result !== undefined && args.result === 1)) {
    selectResult.data = [];
  } else {
    selectResult.error = getErr(ErrCode.RESULT_ERROR);
  }

  console.log('[picker] Select selectResult: ' + JSON.stringify(selectResult));
  return selectResult;
}

async function documentPickerSelect(...args) {
  let checkArgsResult = checkArguments(args);
  if (checkArgsResult !== undefined) {
    console.log('[picker] Select Invalid argument');
    throw checkArgsResult;
  }

  let context = undefined;
  let config = undefined;
  let result = undefined;

  try {
    context = getContext(this);
  } catch (getContextError) {
    console.log('[picker] getContext error: ' + getContextError);
    return undefined;
  }
  try {
    config = parseDocumentPickerSelectOption(args, ACTION.SELECT_ACTION_MODAL);
    result = await context.requestDialogService(config);
  } catch (paramError) {
    console.log('[picker] Select paramError: ' + JSON.stringify(paramError));
    try {
      config = parseDocumentPickerSelectOption(args, ACTION.SELECT_ACTION);
      result = await context.startAbilityForResult(config, {windowMode: 0});
    } catch (error) {
      console.log('[picker] Select error: ' + error);
      return undefined;
    }
  }
  console.log('[picker] Select result: ' + JSON.stringify(result));
  try {
    const selectResult = getDocumentPickerSelectResult(result);
    if (args.length === ARGS_TWO && typeof args[ARGS_ONE] === 'function') {
      return args[ARGS_ONE](selectResult.error, selectResult.data);
    } else if (args.length === ARGS_ONE && typeof args[ARGS_ZERO] === 'function') {
      return args[ARGS_ZERO](selectResult.error, selectResult.data);
    }
    return new Promise((resolve, reject) => {
      if (selectResult.data !== undefined) {
        resolve(selectResult.data);
      } else {
        reject(selectResult.error);
      }
    })
  } catch (resultError) {
    console.log('[picker] Result error: ' + resultError);
  }
  return undefined;
}

function parseDocumentPickerSaveOption(args, action) {
  let config = {
    action: action,
    parameters: {
      startMode: 'save',
    }
  };

  if (args.length > ARGS_ZERO && typeof args[ARGS_ZERO] === 'object') {
    let option = args[ARGS_ZERO];
    if ((option.newFileNames !== undefined) && option.newFileNames.length > 0) {
      config.parameters.key_pick_file_name = option.newFileNames;
      config.parameters.saveFile = option.newFileNames[0];
    }
    
    if (option.defaultFilePathUri !== undefined) {
      config.parameters.key_pick_dir_path = option.defaultFilePathUri;
    }
    if ((option.fileSuffixChoices !== undefined) && option.fileSuffixChoices.length > 0) {
      config.parameters.key_file_suffix_choices = option.fileSuffixChoices;
    }
  }

  console.log('[picker] Save config: ' + JSON.stringify(config));
  return config;
}

function getDocumentPickerSaveResult(args) {
  let saveResult = {
    error: undefined,
    data: undefined
  };

  // 0:success
  // -1:Non modal cancel
  // 1:Modal cancel
  // ResultCode is a non modal return code.
  // Result is the return code of the modality.
  if ((args.resultCode !== undefined && args.resultCode === 0) || (args.result !== undefined && args.result === 0)) {
    if (args.want && args.want.parameters) {
      saveResult.data = args.want.parameters.pick_path_return;
    }
  } else if ((args.resultCode !== undefined && args.resultCode === -1) || (args.result !== undefined && args.result === 1)) {
    saveResult.data = [];
  } else {
    saveResult.error = getErr(ErrCode.RESULT_ERROR);
  }

  console.log('[picker] Save saveResult: ' + JSON.stringify(saveResult));
  return saveResult;
}

async function documentPickerSave(...args) {
  let checkArgsResult = checkArguments(args);
  if (checkArgsResult !== undefined) {
    console.log('[picker] Invalid argument');
    throw checkArgsResult;
  }

  let context = undefined;
  let config = undefined;
  let result = undefined;

  try {
    context = getContext(this);
  } catch (getContextError) {
    console.log('[picker] getContext error: ' + getContextError);
    return undefined;
  }
  try {
    config = parseDocumentPickerSaveOption(args, ACTION.SAVE_ACTION_MODAL);
    result = await context.requestDialogService(config);
  } catch (paramError) {
    console.log('[picker] paramError: ' + JSON.stringify(paramError));
    try {
      config = parseDocumentPickerSaveOption(args, ACTION.SAVE_ACTION);
      result = await context.startAbilityForResult(config, {windowMode: 0});
    } catch (error) {
      console.log('[picker] error: ' + error);
      return undefined;
    }
  }
  console.log('[picker] Save result: ' + JSON.stringify(result));
  try {
    const saveResult = getDocumentPickerSaveResult(result);
    if (args.length === ARGS_TWO && typeof args[ARGS_ONE] === 'function') {
      return args[ARGS_ONE](saveResult.error, saveResult.data);
    } else if (args.length === ARGS_ONE && typeof args[ARGS_ZERO] === 'function') {
      return args[ARGS_ZERO](saveResult.error, saveResult.data);
    }
    return new Promise((resolve, reject) => {
      if (saveResult.data !== undefined) {
        resolve(saveResult.data);
      } else {
        reject(saveResult.error);
      }
    })
  } catch (resultError) {
    console.log('[picker] Result error: ' + resultError);
  }
  return undefined;
}

async function audioPickerSelect(...args) {
  let checkArgsResult = checkArguments(args);
  if (checkArgsResult !== undefined) {
    console.log('[picker] Invalid argument');
    throw checkArgsResult;
  }

  const config = parseDocumentPickerSelectOption(args, ACTION.SELECT_ACTION);
  console.log('[picker] config: ' + JSON.stringify(config));

  try {
    let context = getContext(this);
    let result = await context.startAbilityForResult(config, {windowMode: 0});
    console.log('[picker] result: ' + JSON.stringify(result));
    const selectResult = getDocumentPickerSelectResult(result);
    console.log('[picker] selectResult: ' + JSON.stringify(selectResult));
    if (args.length === ARGS_TWO && typeof args[ARGS_ONE] === 'function') {
      return args[ARGS_ONE](selectResult.error, selectResult.data);
    } else if (args.length === ARGS_ONE && typeof args[ARGS_ZERO] === 'function') {
      return args[ARGS_ZERO](selectResult.error, selectResult.data);
    }
    return new Promise((resolve, reject) => {
      if (selectResult.data !== undefined) {
        resolve(selectResult.data);
      } else {
        reject(selectResult.error);
      }
    })
  } catch (error) {
    console.log('[picker] error: ' + error);
  }
  return undefined;
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
  this.newFileNames = undefined;
}

function DocumentSelectOptions() {
  this.defaultFilePathUri = undefined;
  this.fileSuffixFilters = undefined;
  this.maxSelectNumber = undefined;
}

function DocumentSaveOptions() {
  this.newFileNames = undefined;
  this.defaultFilePathUri = undefined;
  this.fileSuffixChoices = undefined;
}

function AudioSelectOptions() {}

function AudioSaveOptions() {
  this.newFileNames = undefined;
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