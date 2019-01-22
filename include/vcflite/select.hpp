#pragma once

#include <optional>
#include <stdexcept>
#include <string>

#include <vcflite/query.hpp>

using runerror = std::runtime_error;
using std::optional;
using std::string;
using opt_str = optional<string>;

#include <sqlite3.h>

namespace VCFLite::Select {

int max_variant_id(sqlite3 *db) {
  sqlite3_stmt *stmt;

  sqlite3_prepare_v2(db, "SELECT IFNULL(MAX(id_variant), 1) FROM Variants", -1,
                     &stmt, nullptr);

  step(db, stmt, SQLITE_ROW);

  int result = sqlite3_column_int(stmt, 0);

  sqlite3_finalize(stmt);

  return result;
}

}  // namespace VCFLite::Select
