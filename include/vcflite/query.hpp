#pragma once

#include <stdexcept>
#include <string>

using runerror = std::runtime_error;
using std::string;

#include <sqlite3.h>

namespace VCFLite {

[[noreturn]] void panic(sqlite3 *db, const string &query) {
  throw runerror{"Error Executing Query:\n" + query + "\n" +
                 string(sqlite3_errmsg(db)) + "\n"};
}

int exec(sqlite3 *db, const string &query) {
  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, query.c_str(), static_cast<int>(query.size()),
                         &stmt, nullptr) != SQLITE_OK)
    panic(db, query);

  if (sqlite3_step(stmt) != SQLITE_DONE) panic(db, query);

  return sqlite3_finalize(stmt);
}

template <class It>
int exec(sqlite3 *db, It begin, const It end) {
  sqlite3_stmt *stmt;

  for (It t = begin; t < end; ++t) {
    if (sqlite3_prepare_v2(db, (*t).c_str(), static_cast<int>((*t).size()),
                           &stmt, nullptr) != SQLITE_OK)
      panic(db, *t);
    if (sqlite3_step(stmt) != SQLITE_DONE) panic(db, *t);
  }

  return sqlite3_finalize(stmt);
}

}  // namespace VCFLite
