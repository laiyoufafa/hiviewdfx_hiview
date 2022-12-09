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
#include <string>
#include <vector>

#include <fcntl.h>
#include <gtest/gtest.h>
#include "sys_event.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "event.h"
#include "faultlog_database.h"
#include "faultlog_util.h"
#define private public
#include "faultlogger.h"
#undef private
#include "file_util.h"
#include "hiview_global.h"
#include "hiview_platform.h"
#include "log_analyzer.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
namespace OHOS {
namespace HiviewDFX {
constexpr char TEST_LOG_DIR[] = "/data/log/hiview/sys_event_test";
class HiviewTestContext : public HiviewContext {
public:
    std::string GetHiViewDirectory(DirectoryType type __UNUSED)
    {
        return TEST_LOG_DIR;
    }
};

class FaultloggerUnittest : public testing::Test {
public:
    void SetUp()
    {
        sleep(1);
    };
    void TearDown(){};

    std::shared_ptr<Faultlogger> CreateFaultloggerInstance() const
    {
        static std::unique_ptr<HiviewPlatform> platform = std::make_unique<HiviewPlatform>();
        auto plugin = std::make_shared<Faultlogger>();
        plugin->SetName("Faultlogger");
        plugin->SetHandle(nullptr);
        plugin->SetHiviewContext(platform.get());
        plugin->OnLoad();
        return plugin;
    }
};

/**
 * @tc.name: dumpFileListTest001
 * @tc.desc: dump with cmds, check the result
 * @tc.type: FUNC
 * @tc.require: SR000F7UQ6 AR000F83AF
 */
HWTEST_F(FaultloggerUnittest, dumpFileListTest001, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. add multiple cmds to faultlogger
     * @tc.expected: check the content size of the dump function
     */
    auto plugin = CreateFaultloggerInstance();
    auto fd = open("/data/test/testFile", O_CREAT | O_WRONLY | O_TRUNC, 770);
    if (fd < 0) {
        printf("Fail to create test result file.\n");
        return;
    }

    std::vector<std::string> cmds;
    plugin->Dump(fd, cmds);
    cmds.push_back("Faultlogger");
    plugin->Dump(fd, cmds);
    cmds.push_back("-l");
    plugin->Dump(fd, cmds);
    cmds.push_back("-f");
    plugin->Dump(fd, cmds);
    cmds.push_back("cppcrash-ModuleName-10-20201209103823");
    plugin->Dump(fd, cmds);
    cmds.push_back("-d");
    plugin->Dump(fd, cmds);
    cmds.push_back("-t");
    plugin->Dump(fd, cmds);
    cmds.push_back("20201209103823");
    plugin->Dump(fd, cmds);
    cmds.push_back("-m");
    plugin->Dump(fd, cmds);
    cmds.push_back("FAULTLOGGER");
    plugin->Dump(fd, cmds);
    close(fd);
    fd = -1;

    std::string result;
    if (FileUtil::LoadStringFromFile("/data/test/testFile", result)) {
        ASSERT_GT(result.length(), 0ul);
    } else {
        FAIL();
    }
}

/**
 * @tc.name: genCppCrashLogTest001
 * @tc.desc: create cpp crash event and send it to faultlogger
 * @tc.type: FUNC
 * @tc.require: SR000F7UQ6 AR000F4380
 */
HWTEST_F(FaultloggerUnittest, genCppCrashLogTest001, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. create a cpp crash event and pass it to faultlogger
     * @tc.expected: the calling is success and the file has been created
     */
    HiviewTestContext hiviewTestContext;
    HiviewGlobal::CreateInstance(hiviewTestContext);
    auto plugin = CreateFaultloggerInstance();
    FaultLogInfo info;
    info.time = 1607161163;
    info.id = 0;
    info.pid = 7497;
    info.faultLogType = 2;
    info.module = "com.example.myapplication";
    info.sectionMap["APPVERSION"] = "1.0";
    info.sectionMap["FAULT_MESSAGE"] = "Nullpointer";
    info.sectionMap["TRACEID"] = "0x1646145645646";
    info.sectionMap["KEY_THREAD_INFO"] = "Test Thread Info";
    info.sectionMap["REASON"] = "TestReason";
    info.sectionMap["STACKTRACE"] = "#01 xxxxxx\n#02 xxxxxx\n";
    plugin->AddFaultLog(info);
    std::string timeStr = GetFormatedTime(info.time);
    std::string fileName = "/data/log/faultlog/faultlogger/cppcrash-com.example.myapplication-0-" + timeStr;
    bool exist = FileUtil::FileExists(fileName);
    ASSERT_EQ(exist, true);
    auto size = FileUtil::GetFileSize(fileName);
    ASSERT_GT(size, 0ul);
    auto parsedInfo = plugin->GetFaultLogInfo(fileName);
    ASSERT_EQ(parsedInfo->module, "com.example.myapplication");
    FaultLogDatabase *faultLogDb = new FaultLogDatabase();
    ASSERT_EQ(faultLogDb->IsFaultExist(7497, 0, 2), false);
}

/**
 * @tc.name: SaveFaultLogInfoTest001
 * @tc.desc: Test calling SaveFaultLogInfo Func
 * @tc.type: FUNC
 */
HWTEST_F(FaultloggerUnittest, SaveFaultLogInfoTest001, testing::ext::TestSize.Level3)
{
    HiviewTestContext hiviewTestContext;
    HiviewGlobal::CreateInstance(hiviewTestContext);
    FaultLogDatabase *faultLogDb = new FaultLogDatabase();
    FaultLogInfo info;
    info.time = 1607161333; // 3 : index of timestamp
    info.pid = getpid();
    info.id = 0;
    info.faultLogType = 2;
    info.module = "FaultloggerUnittest";
    info.reason = "unittest for SaveFaultLogInfo";
    info.summary = "summary for SaveFaultLogInfo";
    info.sectionMap["APPVERSION"] = "1.0";
    info.sectionMap["FAULT_MESSAGE"] = "abort";
    info.sectionMap["TRACEID"] = "0x1646145645646";
    info.sectionMap["KEY_THREAD_INFO"] = "Test Thread Info";
    info.sectionMap["REASON"] = "TestReason";
    info.sectionMap["STACKTRACE"] = "#01 xxxxxx\n#02 xxxxxx\n";
    faultLogDb->SaveFaultLogInfo(info);

    sleep(1);
    std::list<FaultLogInfo> infoList = faultLogDb->GetFaultInfoList("FaultloggerUnittest", 0, 2, 10);
    ASSERT_EQ(infoList.size(), 0);
}

/**
 * @tc.name: FaultLogUtilTest001
 * @tc.desc: check ExtractInfoFromFileName Func
 * @tc.type: FUNC
 */
HWTEST_F(FaultloggerUnittest, FaultLogUtilTest001, testing::ext::TestSize.Level3)
{
    std::string filename = "appfreeze-com.ohos.systemui-10006-20170805172159";
    auto info = ExtractInfoFromFileName(filename);
    ASSERT_EQ(info.pid, 0);
    ASSERT_EQ(info.faultLogType, 4); // 4 : APP_FREEZE
    ASSERT_EQ(info.module, "com.ohos.systemui");
    ASSERT_EQ(info.id, 10006); // 10006 : test uid
}
} // namespace HiviewDFX
} // namespace OHOS
