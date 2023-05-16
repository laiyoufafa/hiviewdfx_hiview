/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#ifndef HIVIEW_SERVICE_ABILITY_STUB_H
#define HIVIEW_SERVICE_ABILITY_STUB_H

#include "ihiview_service_ability.h"
#include "iremote_stub.h"
#include "message_parcel.h"

namespace OHOS {
namespace HiviewDFX {
class HiviewServiceAbilityStub : public IRemoteStub<IHiviewServiceAbility> {
public:
    HiviewServiceAbilityStub() {};
    virtual ~HiviewServiceAbilityStub() {};
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    int32_t HandleListRequest(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t HandleCopyRequest(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t HandleMoveRequest(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t HandleRemoveRequest(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t HandleCopyOrMoveRequest(MessageParcel& data, MessageParcel& reply, MessageOption& option, bool isMove);
    bool IsPermissionGranted(uint32_t code);
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // IHIVIEW_SERVICE_H