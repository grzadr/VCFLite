#pragma once

#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

#include <sqlite3.h>

#include <vcflite/create.hpp>
#include <vcflite/query.hpp>

#include <hkl/vcf.hpp>

using namespace HKL;

using runerror = std::runtime_error;
using std::optional;
using std::string;

namespace VCFLite {

class Connector {
private:
  sqlite3 *db;
  int last_result_code = 0;
  Creator creator{};

public:
  int open(const string &db_path, bool create = true,
           bool disable_foreign = false);

  Connector() = delete;

  Connector(const string &db_path, bool create = true,
            bool disable_foreign = false) {
    open(db_path, create, disable_foreign);
  }

  int close() { return sqlite3_close_v2(db); }

  ~Connector() {
    if (close() != SQLITE_OK)
      std::cerr << "Database didn't close!\n";
  }

  int check();
  int optimize();
  int index();

  int parseVCF(const string &vcf_file, const optional<string> &samples);

}; // Connector

} // namespace VCFLite
