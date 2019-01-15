#pragma once

#include <stdexcept>
#include <string>

using runerror = std::runtime_error;
using std::string;

#include <sqlite3.h>

namespace VCFLite {

[[noreturn]] inline void panic(sqlite3 *db, const string &query) {
  throw runerror{"\nError in exec()\n  Query: " + query +
                 "\nMessage: " + string(sqlite3_errmsg(db)) + "\n"};
}

inline int exec(sqlite3 *db, const string &query) {
  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, query.c_str(), static_cast<int>(query.size()),
                         &stmt, nullptr) != SQLITE_OK)
    panic(db, query);

  if (auto rc = sqlite3_step(stmt);
      rc != SQLITE_DONE && rc != SQLITE_ROW && rc != SQLITE_OK)
    panic(db, query);

  return sqlite3_finalize(stmt);
}

template <class It>
inline int exec(sqlite3 *db, It begin, const It end) {
  sqlite3_stmt *stmt;

  for (It t = begin; t < end; ++t) {
    if (sqlite3_prepare_v2(db, (*t).c_str(), static_cast<int>((*t).size()),
                           &stmt, nullptr) != SQLITE_OK)
      panic(db, *t);
    if (auto rc = sqlite3_step(stmt);
        rc != SQLITE_DONE && rc != SQLITE_ROW && rc != SQLITE_OK)
      panic(db, *t);
  }

  return sqlite3_finalize(stmt);
}

inline int transaction(sqlite3 *db) { return exec(db, "BEGIN TRANSACTION"); }
inline int commit(sqlite3 *db) { return exec(db, "COMMIT TRANSACTION"); }

}  // namespace VCFLite
