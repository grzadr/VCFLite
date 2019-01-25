#pragma once

#include <iostream>
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

inline int max_variant_id(sqlite3 *db) {
  sqlite3_stmt *stmt;

  sqlite3_prepare_v2(db, "SELECT IFNULL(MAX(id_variant), 1) FROM Variants", -1,
                     &stmt, nullptr);

  step(db, stmt, SQLITE_ROW);

  int result = sqlite3_column_int(stmt, 0);

  sqlite3_finalize(stmt);

  return result;
}

inline int phased_variant_id(sqlite3 *db, const int id_variant,
                             const int phased_pos) {
  sqlite3_stmt *stmt;

  std::cerr << id_variant << " " << phased_pos << "\n";

  sqlite3_prepare_v2(db,
                     "SELECT id_variant "
                     "FROM Variants "
                     "WHERE variant_chrom = "
                     "(SELECT variant_chrom "
                     "FROM Variants WHERE id_variant = ?1) AND "
                     "variant_start = ?2",
                     -1, &stmt, nullptr);

  sqlite3_bind_int(stmt, 1, id_variant);
  sqlite3_bind_int(stmt, 2, phased_pos);

  step(db, stmt, SQLITE_ROW);

  int result = sqlite3_column_int(stmt, 0);

  sqlite3_finalize(stmt);

  return result;
}

}  // namespace VCFLite::Select
