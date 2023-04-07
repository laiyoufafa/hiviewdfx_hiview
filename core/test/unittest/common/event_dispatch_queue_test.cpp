/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "event_dispatch_queue_test.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;

void EventDispatchQueueTest::SetUp()
{
    /**
     * @tc.setup: create order and unordered event dispatch queue
     */
    printf("SetUp.\n");
}

void EventDispatchQueueTest::TearDown()
{
    /**
     * @tc.teardown: destroy the event dispatch queue we have created
     */
    printf("TearDown.\n");
}

std::shared_ptr<Event> EventDispatchQueueTest::CreateEvent(const std::string& name, int32_t id,
                                                           const std::string& message, Event::MessageType type)
{
    auto event = std::make_shared<Event>(name);
    event->messageType_ = type;
    event->eventId_ = id;
    event->SetValue("message", message);
    return event;
}

void ExtendEventListener::OnUnorderedEvent(const OHOS::HiviewDFX::Event& msg)
{
    printf("cur listener:%s OnUnorderedEvent eventId_:%u \n", name_.c_str(), msg.eventId_);
    unorderEventCount_++;
    auto message = msg.GetValue("message");
    processedUnorderedEvents_[message] = msg.sender_;
}

std::string ExtendEventListener::GetListenerName()
{
    return name_;
}
/**
 * @tc.name: EventDispatchQueueCreateTest001
 * @tc.desc: create and init an event dispatch queue
 * @tc.type: FUNC
 * @tc.require: AR000DPTSU
 */
HWTEST_F(EventDispatchQueueTest, EventDispatchQueueCreateTest001, TestSize.Level3)
{
    printf("EventDispatchQueueTest.\n");
    OHOS::HiviewDFX::HiviewPlatform& platform = HiviewPlatform::GetInstance();
    if (!platform.InitEnvironment("/data/test/test_data/hiview_platform_config0")) {
        printf("Fail to init environment. \n");
    }
    auto unorderQueue = std::make_shared<EventDispatchQueue>("test1", Event::ManageType::UNORDERED, &platform);
    ASSERT_EQ(false, unorderQueue->IsRunning());
    unorderQueue->Start();
    sleep(1);
    ASSERT_EQ(true, unorderQueue->IsRunning());
    unorderQueue->Stop();
    ASSERT_EQ(false, unorderQueue->IsRunning());
}
