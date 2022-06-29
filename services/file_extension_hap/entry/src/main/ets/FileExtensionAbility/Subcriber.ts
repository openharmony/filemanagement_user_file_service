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
import CommonEvent from '@ohos.commonevent'
async function onReceiveEvent(callBack) {
    var subscribeInfo = {
        events: [
            "usual.event.data.VOLUME_REMOVED",
            "usual.event.data.VOLUME_UNMOUNTED",
            "usual.event.data.VOLUME_MOUNTED",
            "usual.event.data.VOLUME_BAD_REMOVAL",
            "usual.event.data.VOLUME_EJECT"
        ]
    };
    let subscriber = await CommonEvent.createSubscriber(subscribeInfo);
    CommonEvent.subscribe(subscriber, function (err, data) {
        callBack(data)
    })
}

export { onReceiveEvent }