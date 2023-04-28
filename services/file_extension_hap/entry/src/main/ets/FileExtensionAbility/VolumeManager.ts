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
// @ts-nocheck
import volumeManager from '@ohos.file.volumeManager';
import fileExtensionInfo from '@ohos.file.fileExtensionInfo';
import fileuri from '@ohos.file.fileuri';
import hilog from '@ohos.hilog';
if (!globalThis.volumeInfoList) {
  globalThis.volumeInfoList = [];
}
const deviceFlag = fileExtensionInfo.DeviceFlag;
const deviceType = fileExtensionInfo.DeviceType;
const DOMAIN_CODE = 0x0001;
const TAG = 'ExternalFileManager';
function init(): void {
  globalThis.volumeInfoList = [];
  volumeManager.getAllVolumes(function(error, volumes) {
    if (error) {
      hilog.error(DOMAIN_CODE, TAG, 'getAllVolumes failed error message = ' + error.message);
      return;
    }
    let flags = deviceFlag.SUPPORTS_READ | deviceFlag.SUPPORTS_WRITE;
    for (let i = 0; i < volumes.length; i++) {
      let volume = volumes[i];
      if (!volume.path) {
        continue;
      }
      let volumeInfo = {
        'volumeId': volume.id,
        'fsUuid': volume.uuid,
        'path': volume.path,
        'uri': path2uri(volume.path),
        'displayName': volume.id,
        'deviceFlags': flags,
        'deviceType': deviceType.DEVICE_EXTERNAL_USB
      };
      globalThis.volumeInfoList.push(volumeInfo);
    }
  });
}

function addVolumeInfo(volumeInfo): void {
  globalThis.volumeInfoList.push(volumeInfo);
}

function path2uri(path): string {
  let uri = fileuri.getUriFromPath(path);
  return uri;
}

function findVolumeInfo(volumeId) {
  return globalThis.volumeInfoList.find((volume) => volume.volumeId === volumeId);
}

function delVolumeInfo(volumeId): void {
  globalThis.volumeInfoList = globalThis.volumeInfoList.filter((volume) => volume.volumeId !== volumeId);
}

function getVolumeInfoList() {
  return globalThis.volumeInfoList;
}

export { init, addVolumeInfo, findVolumeInfo, delVolumeInfo, getVolumeInfoList, path2uri };