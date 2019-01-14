#include <vcflite/connect.hpp>

int VCFLite::Connector::open(const std::__cxx11::string &db_path, bool create) {
  //    sqlite3* db = sqlite.get();

  auto flag = create ? SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
                     : SQLITE_OPEN_READWRITE;

  last_result_code = sqlite3_open_v2(db_path.c_str(), &db, flag, NULL);

  if (last_result_code)
    throw runerror{"Can't open database: " + string(sqlite3_errmsg(db)) + "\n"};

  if (create) creator.init(db);

  return last_result_code;
}
