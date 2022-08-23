/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "time_util.h"

#include <sys/time.h>
#include <unistd.h>
#include <chrono>

namespace OHOS {
namespace HiviewDFX {
namespace TimeUtil {
time_t StrToTimeStamp(const std::string& tmStr, const std::string& format)
{
    std::string stTime = tmStr;
    struct tm tmFormat { 0 };
    strptime(stTime.c_str(), format.c_str(), &tmFormat);
    tmFormat.tm_isdst = -1;
    return mktime(&tmFormat);
}

uint64_t GenerateTimestamp()
{
    struct timeval now;
    if (gettimeofday(&now, nullptr) == -1) {
        return 0;
    }
    return (now.tv_sec * SEC_TO_MICROSEC + now.tv_usec);
}

void Sleep(unsigned int seconds)
{
    sleep(seconds);
}

int GetMillSecOfSec()
{
    auto now = std::chrono::system_clock::now();
    auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return millisecs.count() % SEC_TO_MILLISEC;
}

uint64_t GetMilliseconds()
{
    auto now = std::chrono::system_clock::now();
    auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return millisecs.count();
}

std::string TimestampFormatToDate(time_t timeStamp, const std::string& format)
{
    char date[MAX_TIME_BUFF] = {0};
    struct tm result {};
    if (localtime_r(&timeStamp, &result) != nullptr) {
        strftime(date, MAX_TIME_BUFF, format.c_str(), &result);
    }
    return std::string(date);
}

std::string GetFomattedTime(unsigned int origin)
{
    unsigned int dec = 10;
    std::string timeRet {static_cast<char>('0' + (origin / dec % dec))}; // add num at tens place
    timeRet += static_cast<char>('0' + origin % dec); // add num at ones place
    return timeRet;
}

std::string GetTimeZone()
{
    struct timeval currentTime;
    if (gettimeofday(&currentTime, nullptr) != 0) {
        return "";
    }
    time_t systemSeconds = currentTime.tv_sec;
    struct tm tmLocal;
    if (localtime_r(&systemSeconds, &tmLocal) == nullptr) {
        return "";
    }
    struct tm timeUtc;
    if (gmtime_r(&systemSeconds, &timeUtc) == nullptr) {
        return "";
    }
    time_t offset = mktime(&tmLocal) - mktime(&timeUtc);
    unsigned int secsPerHour = 3600;
    unsigned int hour = static_cast<unsigned int>(std::abs(offset)) / secsPerHour;
    unsigned int timeZoneUpper = 12; // max time zone is 12
    if (hour > timeZoneUpper) {
        hour = 0;
    }
    unsigned int secsPerMin = 60;
    unsigned int min = (static_cast<unsigned int>(std::abs(offset)) % secsPerHour) / secsPerMin;
    std::string timeZone {(offset < 0) ? "-" : "+"};
    timeZone += GetFomattedTime(hour);
    timeZone += GetFomattedTime(min);
    return timeZone;
}

int64_t Get0ClockStampMs()
{
    time_t now = std::time(nullptr);
    int64_t zero = now;
    struct tm *l = std::localtime(&now);
    if (l != nullptr) {
        l->tm_hour = 0;
        l->tm_min = 0;
        l->tm_sec = 0;
        zero = std::mktime(l) * SEC_TO_MILLISEC;  // time is 00:00:00
    }
    return zero;
}

uint64_t GetSteadyClockTimeMs()
{
    auto now = std::chrono::steady_clock::now();
    auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return millisecs.count();
}

TimeCalculator::TimeCalculator(std::shared_ptr<uint64_t>& timePtr)
{
    this->time_ = timePtr;
    this->startTime_ = GenerateTimestamp();
    this->endTime_ = 0;
}

TimeCalculator::~TimeCalculator()
{
    this->endTime_ = GenerateTimestamp();
    if (this->time_ != nullptr && this->endTime_ > this->startTime_) {
        *(this->time_) += this->endTime_ - this->startTime_;
    }
}
} // namespace TimeUtil
} // namespace HiviewDFX
} // namespace OHOS
