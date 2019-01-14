#include <iostream>
#include <memory>

#include <sqlite3.h>

#include <agizmo/args.hpp>

using namespace AGizmo;

int main(int argc, char *argv[]) {
  Args::Arguments args{};

  args.addArgument(1, "db_path", "Path to new database file",
                   Args::ValueType::String);
  args.addArgument(2, "vcf_file", "Path to vcf file", Args::ValueType::String);

  args.parse(argc, argv);

  std::cerr << "Hello world!" << '\n';

  //  std::unique_ptr<sqlite3> db{};

  sqlite3 *db;
  //  char *zErrMsg = 0;
  int rc;

  rc = sqlite3_open(args.getArg("db_path").getValue().value().c_str(), &db);

  if (rc) {
    std::cerr << "Can't open database: " << sqlite3_errmsg(db) << "\n";
    return rc;
  } else
    std::cerr << "Opened database successfully\n";

  sqlite3_close(db);

  return 0;
}
