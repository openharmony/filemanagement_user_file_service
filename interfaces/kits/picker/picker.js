/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
const pickerHelper = requireInternal('file.picker');

let gContext = undefined;

const PhotoViewMIMETypes = {
  IMAGE_TYPE: 'image/*',
  VIDEO_TYPE: 'video/*',
  IMAGE_VIDEO_TYPE: '*/*',
  INVALID_TYPE: ''
}

const DocumentSelectMode = {
  FILE: 0,
  FOLDER: 1,
  MIXED: 2,
};

const DocumentPickerMode = {
  DEFAULT: 0,
  DOWNLOAD: 1,
};

const ExtTypes = {
  DOWNLOAD_TYPE: 'filePicker',
};

const PickerDetailType = {
  FILE_MGR_AUTH: 'downloadAuth', 
};

const ErrCode = {
  INVALID_ARGS: 13900020,
  RESULT_ERROR: 13900042,
  NAME_TOO_LONG: 13900030,
  CONTEXT_NO_EXIST: 16000011,
}

const ERRCODE_MAP = new Map([
  [ErrCode.INVALID_ARGS, 'Invalid argument'],
  [ErrCode.RESULT_ERROR, 'Unknown error'],
  [ErrCode.NAME_TOO_LONG, 'File name too long'],
  [ErrCode.CONTEXT_NO_EXIST, 'Current ability failed to obtain context'],
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
  SAVE_ACTION_DOWNLOAD: 'ohos.want.action.DOWNLOAD',
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
  let greeceLen = 2;
  let chineseLen = 3;
  let othersLen = 4;
  for (let i = 0; i < strLen; i++) {
    let charCode = str.charCodeAt(i);
    if (charCode <= 0x007f) {
      bytesLen++;
    } else if (charCode <= 0x07ff) {
      bytesLen += greeceLen;
    } else if (charCode <= 0xffff) {
      bytesLen += chineseLen;
    } else {
      bytesLen += othersLen;
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
  } else if (args.resultCode === -1) {
    selectResult.data = new PhotoSelectResult([], undefined);
  } else {
    selectResult.error = getErr(ErrCode.RESULT_ERROR);
  }

  return selectResult;
}

async function photoPickerSelect(...args) {
  let checkPhotoArgsResult = checkArguments(args);
  if (checkPhotoArgsResult !== undefined) {
    console.log('[picker] Photo Invalid argument');
    throw checkPhotoArgsResult;
  }

  const config = parsePhotoPickerSelectOption(args);
  console.log('[picker] Photo config: ' + JSON.stringify(config));

  let photoSelectContext = undefined;
  try {
    if (this.context !== undefined) {
      photoSelectContext = this.context;
    } else {
      photoSelectContext = getContext(this);
    }
  } catch (getContextError) {
    console.error('[picker] getContext error: ' + getContextError);
    throw getErr(ErrCode.CONTEXT_NO_EXIST);
  }
  try {
    if (photoSelectContext === undefined) {
      throw getErr(ErrCode.CONTEXT_NO_EXIST);
    }
    let result = await photoSelectContext.startAbilityForResult(config, {windowMode: 0});
    console.log('[picker] photo select result: ' + JSON.stringify(result));
    const photoSelectResult = getPhotoPickerSelectResult(result);
    console.log('[picker] photoSelectResult: ' + JSON.stringify(photoSelectResult));
    if (args.length === ARGS_TWO && typeof args[ARGS_ONE] === 'function') {
      return args[ARGS_ONE](photoSelectResult.error, photoSelectResult.data);
    } else if (args.length === ARGS_ONE && typeof args[ARGS_ZERO] === 'function') {
      return args[ARGS_ZERO](photoSelectResult.error, photoSelectResult.data);
    }
    return new Promise((resolve, reject) => {
      if (photoSelectResult.data !== undefined) {
        resolve(photoSelectResult.data);
      } else {
        reject(photoSelectResult.error);
      }
    })
  } catch (error) {
    console.error('[picker] photo select error: ' + error);
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
    config.parameters.key_select_mode = option.selectMode;
    console.log('parseDocumentPickerSelectOption: ' + option.selectMode);

    if ((option.maxSelectNumber !== undefined) && option.maxSelectNumber > 0) {
      config.parameters.key_pick_num = option.maxSelectNumber;
    }
    if (option.defaultFilePathUri !== undefined) {
      config.parameters.key_pick_dir_path = option.defaultFilePathUri;
    }
    if ((option.fileSuffixFilters !== undefined) && option.fileSuffixFilters.length > 0) {
      config.parameters.key_file_suffix_filter = option.fileSuffixFilters;
    }
    if (option.authMode !== undefined) {
      config.parameters.key_auth_mode = option.authMode;
    }
  }

  console.log('[picker] document select config: ' + JSON.stringify(config));
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
      if (args.want.parameters.select_item_list) {
        selectResult.data = args.want.parameters.select_item_list;
      } else {
        selectResult.data = args.want.parameters['ability.params.stream'];
      }
    }
  } else if ((args.resultCode !== undefined && args.resultCode === -1) ||
      (args.result !== undefined && args.result === 1)) {
    selectResult.data = [];
  } else {
    selectResult.error = getErr(ErrCode.RESULT_ERROR);
  }

  console.log('[picker] document select selectResult: ' + JSON.stringify(selectResult));
  return selectResult;
}

async function documentPickerSelect(...args) {
  let checkDocumentSelectArgsResult = checkArguments(args);
  if (checkDocumentSelectArgsResult !== undefined) {
    console.log('[picker] Document Select Invalid argument');
    throw checkDocumentSelectArgsResult;
  }

  let documentSelectContext = undefined;
  let documentSelectConfig = undefined;
  let documentSelectResult = undefined;

  try {
    if (this.context !== undefined) {
      documentSelectContext = this.context;
    } else {
      documentSelectContext = getContext(this);
    }
  } catch (getContextError) {
    console.error('[picker] getContext error: ' + getContextError);
    throw getErr(ErrCode.CONTEXT_NO_EXIST);
  }
  try {
    if (documentSelectContext === undefined) {
      throw getErr(ErrCode.CONTEXT_NO_EXIST);
    }
    documentSelectConfig = parseDocumentPickerSelectOption(args, ACTION.SELECT_ACTION_MODAL);
    documentSelectResult = await documentSelectContext.requestDialogService(documentSelectConfig);
  } catch (paramError) {
    console.error('[picker] DocumentSelect paramError: ' + JSON.stringify(paramError));
    try {
      documentSelectConfig = parseDocumentPickerSelectOption(args, ACTION.SELECT_ACTION);
      documentSelectResult = await documentSelectContext.startAbilityForResult(documentSelectConfig, {windowMode: 0});
    } catch (error) {
      console.error('[picker] DocumentSelect error: ' + error);
      return undefined;
    }
  }
  console.log('[picker] DocumentSelect result: ' + JSON.stringify(documentSelectResult));
  try {
    const selectResult = getDocumentPickerSelectResult(documentSelectResult);
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
    console.error('[picker] Result error: ' + resultError);
  }
  return undefined;
}

function parseDocumentPickerSaveOption(args, action) {
  let config = {
    action: action,
    parameters: {
      startMode: 'save',
      pickerMode: DocumentPickerMode.DEFAULT,
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
    if (option.pickerMode === DocumentPickerMode.DOWNLOAD) {
      config.parameters.pickerMode = option.pickerMode;
      config.action = ACTION.SAVE_ACTION_DOWNLOAD;
      config.parameters.extType = ExtTypes.DOWNLOAD_TYPE;
      config.parameters.pickerType = PickerDetailType.FILE_MGR_AUTH;
    }
  }

  console.log('[picker] document save config: ' + JSON.stringify(config));
  return config;
}

function getModalPickerResult(args) {
  let saveResult = {
    error: undefined,
    data: undefined
  }
  if (args) {
    var dataArr = [];
    dataArr.push(args.uri);
    saveResult.data = dataArr;
  }
  console.log('modal picker: download saveResult: ' + JSON.stringify(saveResult));
  return saveResult;
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
      if (args.want.parameters.pick_path_return) {
        saveResult.data = args.want.parameters.pick_path_return;
      } else {
        saveResult.data = args.want.parameters['ability.params.stream'];
      }
    }
  } else if ((args.resultCode !== undefined && args.resultCode === -1) ||
      (args.result !== undefined && args.result === 1) ) {
    saveResult.data = [];
  } else {
    saveResult.error = getErr(ErrCode.RESULT_ERROR);
  }

  console.log('[picker] document saveResult: ' + JSON.stringify(saveResult));
  return saveResult;
}

function startModalPicker(context, config) {
  if (context === undefined) {
    console.log('modal picker: startModalPicker context undefined.');
    throw Error('modal picker: startModalPicker context undefined.');
  }
  if (config === undefined) {
    console.log('modal picker: startModalPicker config undefined.');
    throw Error('modal picker: startModalPicker config undefined.');
  }
  gContext = context;
  if (pickerHelper === undefined) {
    console.log('modal picker: pickerHelper undefined.')
  }
  let helper = pickerHelper.startModalPicker(gContext, config);
  if (helper === undefined) {
    console.log('modal picker: startModalPicker helper undefined.');
  }
  return helper;
}

async function modalPicker(args, context, config) {
  try {
    console.log('modal picker: config: ' + JSON.stringify(config));
    let modalSaveResult = await startModalPicker(context, config);
    const saveResult = getModalPickerResult(modalSaveResult);
    return saveResult;
  } catch (resultError) {
    console.error('modal picker: Result error: ' + resultError);
  }
}

async function documentPickerSave(...args) {
  let checkDocumentSaveArgsResult = checkArguments(args);
  if (checkDocumentSaveArgsResult !== undefined) {
    console.log('[picker] Document Save Invalid argument');
    throw checkDocumentSaveArgsResult;
  }

  let documentSaveContext = undefined;
  let documentSaveConfig = undefined;
  let documentSaveResult = undefined;
  let saveResult = undefined;

  try {
    if (this.context !== undefined) {
      documentSaveContext = this.context;
    } else {
      documentSaveContext = getContext(this);
    }
  } catch (getContextError) {
    console.error('[picker] getContext error: ' + getContextError);
    throw getErr(ErrCode.CONTEXT_NO_EXIST);
  }

  documentSaveConfig = parseDocumentPickerSaveOption(args, ACTION.SAVE_ACTION_MODAL);
  if (documentSaveConfig.parameters.pickerMode === DocumentPickerMode.DOWNLOAD) {
    console.log('modal picker: will start modal picker process. (DOWNLOAD)');
    saveResult = await modalPicker(args, documentSaveContext, documentSaveConfig);
  } else {
    try {
      if (documentSaveContext === undefined) {
        throw getErr(ErrCode.CONTEXT_NO_EXIST);
      }
      documentSaveConfig = parseDocumentPickerSaveOption(args, ACTION.SAVE_ACTION_MODAL);
      documentSaveResult = await documentSaveContext.requestDialogService(documentSaveConfig);
    } catch (paramError) {
      console.error('[picker] paramError: ' + JSON.stringify(paramError));
      try {
        documentSaveConfig = parseDocumentPickerSaveOption(args, ACTION.SAVE_ACTION);
        documentSaveResult = await documentSaveContext.startAbilityForResult(documentSaveConfig, {windowMode: 0});
      } catch (error) {
        console.error('[picker] document save error: ' + error);
        return undefined;
      }
    }
    console.log('[picker] document save result: ' + JSON.stringify(documentSaveResult));
    saveResult = getDocumentPickerSaveResult(documentSaveResult);
  }
  try {
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
    console.error('[picker] Result error: ' + resultError);
  }
  return undefined;
}

async function audioPickerSelect(...args) {
  let checkAudioArgsResult = checkArguments(args);
  if (checkAudioArgsResult !== undefined) {
    console.log('[picker] Audio Invalid argument');
    throw checkAudioArgsResult;
  }

  const audioSelectConfig = parseDocumentPickerSelectOption(args, ACTION.SELECT_ACTION);
  console.log('[picker] audio select config: ' + JSON.stringify(audioSelectConfig));

  let audioSelectContext = undefined;
  try {
    if (this.context !== undefined) {
      audioSelectContext = this.context;
    } else {
      audioSelectContext = getContext(this);
    }
  } catch (getContextError) {
    console.error('[picker] getContext error: ' + getContextError);
    throw getErr(ErrCode.CONTEXT_NO_EXIST);
  }
  try {
    if (audioSelectContext === undefined) {
      throw getErr(ErrCode.CONTEXT_NO_EXIST);
    }
    let result = await audioSelectContext.startAbilityForResult(audioSelectConfig, {windowMode: 0});
    console.log('[picker] audio select result: ' + JSON.stringify(result));
    const audioSelectResult = getDocumentPickerSelectResult(result);
    console.log('[picker] documentSelectResult: ' + JSON.stringify(audioSelectResult));
    if (args.length === ARGS_TWO && typeof args[ARGS_ONE] === 'function') { 
      return args[ARGS_ONE](audioSelectResult.error, audioSelectResult.data);
    } else if (args.length === ARGS_ONE && typeof args[ARGS_ZERO] === 'function') {
      return args[ARGS_ZERO](audioSelectResult.error, audioSelectResult.data);
    }
    return new Promise((resolve, reject) => {
      if (audioSelectResult.data !== undefined) {
        resolve(audioSelectResult.data);
      } else {
        reject(audioSelectResult.error);
      }
    })
  } catch (error) {
    console.error('[picker] audio select error: ' + error);
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
  this.selectMode = DocumentSelectMode.FILE;
}

function DocumentSaveOptions() {
  this.newFileNames = undefined;
  this.defaultFilePathUri = undefined;
  this.fileSuffixChoices = undefined;
  this.pickerMode = DocumentPickerMode.DEFAULT;
}

function AudioSelectOptions() {}

function AudioSaveOptions() {
  this.newFileNames = undefined;
}

function ParseContext(args)
{
  if (args.length > ARGS_ONE || args.length < ARGS_ZERO || typeof args[ARGS_ZERO] !== 'object') {
    return undefined;
  }
  return args[ARGS_ZERO];
}

function PhotoViewPicker(...args) {
  this.select = photoPickerSelect;
  this.save = documentPickerSave;
  this.context = ParseContext(args);
}

function DocumentViewPicker(...args) {
  this.select = documentPickerSelect;
  this.save = documentPickerSave;
  this.context = ParseContext(args);
}

function AudioViewPicker(...args) {
  this.select = audioPickerSelect;
  this.save = documentPickerSave;
  this.context = ParseContext(args);
}

export default {
  startModalPicker,
  ExtTypes : ExtTypes,
  PickerDetailType: PickerDetailType,
  PhotoViewMIMETypes : PhotoViewMIMETypes,
  PhotoSelectOptions : PhotoSelectOptions,
  PhotoSelectResult : PhotoSelectResult,
  PhotoSaveOptions : PhotoSaveOptions,
  DocumentSelectMode : DocumentSelectMode,
  DocumentPickerMode : DocumentPickerMode,
  DocumentSelectOptions : DocumentSelectOptions,
  DocumentSaveOptions : DocumentSaveOptions,
  AudioSelectOptions : AudioSelectOptions,
  AudioSaveOptions : AudioSaveOptions,
  PhotoViewPicker : PhotoViewPicker,
  DocumentViewPicker: DocumentViewPicker,
  AudioViewPicker : AudioViewPicker,
}