#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include <sqlite3.h>

namespace VCFLite {

using runerror = std::runtime_error;
using std::string;
using std::vector;

class Creator {
private:
public:
  Creator() = default;

  int init(sqlite3 *db);
  int index(sqlite3 *db);
};
} // namespace VCFLite
