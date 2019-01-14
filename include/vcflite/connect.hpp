#pragma once

#include <iostream>
#include <stdexcept>
#include <string>

#include <sqlite3.h>

#include <vcflite/create.hpp>

using runerror = std::runtime_error;
using std::string;

namespace VCFLite {

class Connector {
 private:
  sqlite3* db;
  //  std::unique_ptr<sqlite3> sqlite;
  int last_result_code = 0;
  Creator creator{};

 public:
  int open(const string& db_path, bool create = true);

  Connector() = delete;

  Connector(const string& db_path, bool create = true) {
    open(db_path, create);
  }

  int close() {
    //    if (sqlite) {
    //      last_result_code = sqlite3_close_v2(sqlite.get());
    //      sqlite = nullptr;
    //      return last_result_code;
    //    } else
    //      return SQLITE_OK;
    return sqlite3_close_v2(db);
  }

  ~Connector() {
    if (close() != SQLITE_OK) std::cerr << "Database didn't close!\n";
  }
};  // Connector

}  // namespace VCFLite
