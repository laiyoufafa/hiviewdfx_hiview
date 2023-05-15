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

#include "data_share_store.h"

#include <string>
#include <utility>
#include <vector>

#include "hilog/log.h"
#include "rdb_errno.h"
#include "rdb_helper.h"

#include "data_share_store_callback.h"
#include "file_util.h"

using namespace OHOS::HiviewDFX::SubscribeStore;

namespace OHOS {
namespace HiviewDFX {

namespace {
constexpr HiLogLabel LABEL = {LOG_CORE, 0xD002D10, "HiView-DataShareStore"};

std::string GenerateCreateSql(const std::string &table, std::vector<std::pair<std::string, std::string>> fields)
{
    std::string sql;
    sql += "CREATE TABLE IF NOT EXISTS ";
    sql += table;
    sql += "(";
    sql += "id INTEGER PRIMARY KEY AUTOINCREMENT";  // default field: id
    for (auto field : fields) {
        sql += ", ";
        sql += field.first;
        sql += " ";
        sql += field.second;
    }
    sql += ")";
    return sql;
}

std::string GenerateDropSql(const std::string &table)
{
    std::string sql;
    sql += "DROP TABLE IF EXISTS ";
    sql += table;
    return sql;
}
}  // namespace

int DataShareStoreCallback::OnCreate(NativeRdb::RdbStore &rdbStore)
{
    std::vector<std::pair<std::string, std::string>> fields = {{EventTable::FIELD_UID, SQL_INT_TYPE},
        {EventTable::FIELD_BUNDLE_NAME, SQL_TEXT_TYPE},
        {EventTable::FIELD_SUBSCRIBETIME, SQL_BIGINT_TYPE},
        {EventTable::FIELD_EVENTLIST, SQL_TEXT_TYPE}};
    std::string sql = GenerateCreateSql(EventTable::TABLE, fields);
    if (int ret = rdbStore.ExecuteSql(sql); ret != NativeRdb::E_OK) {
        HiLog::Error(LABEL, "failed to create events table, ret=%{public}d", ret);
        return ret;
    }
    return NativeRdb::E_OK;
}

int DataShareStoreCallback::OnUpgrade(NativeRdb::RdbStore &rdbStore, int oldVersion, int newVersion)
{
    HiLog::Debug(LABEL, "OnUpgrade, oldVersion=%{public}d, newVersion=%{public}d", oldVersion, newVersion);
    return NativeRdb::E_OK;
}

std::shared_ptr<NativeRdb::RdbStore> DataShareStore::GetDbStore()
{
    if (dbStore_ == nullptr) {
        std::lock_guard<std::mutex> lockGuard(dbMutex_);
        if (dbStore_ == nullptr) {
            dbStore_ = CreateDbStore();
        }
    }
    return dbStore_;
}

int DataShareStore::DropTable(const std::string &tableName)
{
    auto dbStore = GetDbStore();
    if (dbStore == nullptr) {
        HiLog::Error(LABEL, "failed to drop table %{public}s, db is null", tableName.c_str());
        return DB_FAILED;
    }
    std::string sql = GenerateDropSql(tableName);
    if (int ret = dbStore->ExecuteSql(sql); ret != NativeRdb::E_OK) {
        HiLog::Error(LABEL, "failed to drop table %{public}s, ret=%{public}d", tableName.c_str(), ret);
        return DB_FAILED;
    }
    return DB_SUCC;
}

std::shared_ptr<NativeRdb::RdbStore> DataShareStore::CreateDbStore()
{
    if (dirPath_.empty()) {
        HiLog::Error(LABEL, "failed to create db store, path is empty");
        return nullptr;
    }
    if (!FileUtil::FileExists(dirPath_) && !FileUtil::ForceCreateDirectory(dirPath_)) {
        HiLog::Error(LABEL, "failed to create database dir.");
        return nullptr;
    }
    int ret = NativeRdb::E_OK;
    NativeRdb::RdbStoreConfig config(dirPath_ + DATABASE_NAME);
    DataShareStoreCallback callback;
    auto dbStore = NativeRdb::RdbHelper::GetRdbStore(config, 1, callback, ret);
    if (ret != NativeRdb::E_OK || dbStore == nullptr) {
        HiLog::Error(LABEL, "failed to create db store, ret=%{public}d", ret);
        return nullptr;
    }
    return dbStore;
}

int DataShareStore::DestroyDbStore()
{
    if (dbStore_ == nullptr) {
        return DB_SUCC;
    }
    dbStore_ = nullptr;
    if (int ret = NativeRdb::RdbHelper::DeleteRdbStore(dirPath_ + DATABASE_NAME); ret != NativeRdb::E_OK) {
        HiLog::Error(LABEL, "failed to destroy db store, ret=%{public}d", ret);
        return DB_FAILED;
    }
    return DB_SUCC;
}

}  // namespace HiviewDFX
}  // namespace OHOS