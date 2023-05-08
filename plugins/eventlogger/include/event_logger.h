/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#ifndef HIVIEW_PLUGIN_EVENT_LOG_COLLECTOR_H
#define HIVIEW_PLUGIN_EVENT_LOG_COLLECTOR_H

#include <ctime>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>

#include "event.h"
#include "event_loop.h"
#include "log_store_ex.h"
#include "logger.h"
#include "plugin.h"
#include "sys_event.h"

#include "event_logger_config.h"
#include "event_thread_pool.h"
namespace OHOS {
namespace HiviewDFX {
struct BinderInfo {
    int client;
    int server;
    unsigned long wait;
};

class EventLogger : public Plugin, public FileDescriptorEventCallback  {
public:
    EventLogger() : logStore_(std::make_unique<LogStoreEx>(LOGGER_EVENT_LOG_PATH, true)),
        startTime_(time(nullptr)),
        inotifyFd_(0) {};
    ~EventLogger() {};
    bool OnEvent(std::shared_ptr<Event> &event) override;
    void OnLoad() override;
    void OnUnload() override;
    bool IsInterestedPipelineEvent(std::shared_ptr<Event> event) override;
    bool OnFileDescriptorEvent(int fd, int type) override;
    int32_t GetPollFd() override;
    int32_t GetPollType() override;
private:
    static const inline std::string LOGGER_EVENT_LOG_PATH = "/data/log/eventlog";
    static const inline std::string MONITOR_STACK_LOG_PATH = "/data/log/faultlog/temp";
    static const inline std::string MONITOR_STACK_FLIE_NAME[] = {
        "jsstack",
    };
    static const inline std::string MONITOR_LOG_PATH[] = {
        MONITOR_STACK_LOG_PATH,
    };
    static constexpr int EVENT_MAX_ID = 1000000;
    static constexpr int MAX_FILE_NUM = 500;
    static constexpr int MAX_FOLDER_SIZE = 50 * 1024 * 1024;

    std::unique_ptr<LogStoreEx> logStore_;
    uint64_t startTime_;
    std::unordered_map<std::string, std::time_t> eventTagTime_;
    int inotifyFd_;
    std::unordered_map<int, std::string> fileMap_;
    std::unordered_map<std::string, EventLoggerConfig::EventLoggerConfigData> eventLoggerConfig_;
    std::unordered_set<std::string> hitraceSet_;
    std::mutex hitraceSetMutex_;
    int const maxEventPoolCount = 5;
    std::unique_ptr<EventThreadPool> eventPool_;
    std::mutex intervalMutex_;

    void StartLogCollect(std::shared_ptr<SysEvent> event);
    int Getfile(std::shared_ptr<SysEvent> event, std::string& logFile);
    bool JudgmentRateLimiting(std::shared_ptr<SysEvent> event);
    bool WriteCommonHead(int fd, std::shared_ptr<SysEvent> event);
    bool UpdateDB(std::shared_ptr<SysEvent> event, std::string logFile);
    void CreateAndPublishEvent(std::string& dirPath, std::string& fileName);
    bool HitraceCatcher(int64_t& beginTime, const std::string& hitraceTime,
        const std::string& fullTracePath, std::shared_ptr<SysEvent> event);
    std::string GetHitraceName(int64_t& beginTime, std::string& hitraceTime);
    bool DetectionHiTraceMap(const std::string& name);
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIVIEW_PLUGIN_EVENT_LOG_COLLECTOR_H
