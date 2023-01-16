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
import volumeManager from '@ohos.file.volumeManager'
import fileExtensionInfo from "@ohos.file.fileExtensionInfo"
if (!globalThis.volumeInfoList) {
    globalThis.volumeInfoList = [];
}
const DeviceFlag = fileExtensionInfo.DeviceFlag;
const DeviceType = fileExtensionInfo.DeviceType;
function init() {
    globalThis.volumeInfoList = [];
    volumeManager.getAllVolumes().then((volumes) => {
        let flags = DeviceFlag.SUPPORTS_READ | DeviceFlag.SUPPORTS_WRITE;
        for (let i = 0; i < volumes.length; i++) {
            let volume = volumes[i];
            if (!volume.path) {
                continue;
            }
            let volumeInfo = {
                'volumeId': volume.id,
                'fsUuid': volume.uuid,
                'path': volume.path,
                'uri': path2uri('', volume.path),
                'displayName': volume.id,
                'deviceFlags': flags,
                'deviceType': DeviceType.DEVICE_EXTERNAL_USB
            }
            globalThis.volumeInfoList.push(volumeInfo);
        }
    });
}

function addVolumeInfo(volumeInfo) {
    globalThis.volumeInfoList.push(volumeInfo);
}

function path2uri(id, path) {
    return `datashare://${id}/com.ohos.UserFile.ExternalFileManager${path}`;
}

function findVolumeInfo(volumeId) {
    return globalThis.volumeInfoList.find((volume) => volume.volumeId == volumeId);
}

function delVolumeInfo(volumeId) {
    globalThis.volumeInfoList = globalThis.volumeInfoList.filter((volume) => volume.volumeId !== volumeId);
}

function getVolumeInfoList() {
    return globalThis.volumeInfoList;
}

export { init, addVolumeInfo, findVolumeInfo, delVolumeInfo, getVolumeInfoList, path2uri }