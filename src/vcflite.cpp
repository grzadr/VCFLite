#include <iostream>

#include <agizmo/args.hpp>
#include <vcflite/connect.hpp>
#include <agizmo/logging.hpp>

using namespace AGizmo;
using namespace HKL;

int main(int argc, char *argv[]) {
  Args::Arguments args{};

  args.addPositional("db_path", "Path to new database file", true);
  args.addMulti("vcf", "Path to vcf file", 'v');
  args.addArgument("samples", "List with samples, delimetered with ','.", 's');
  args.addSwitch("create", "Force to create db", 'c');
  args.addSwitch("optimize", "Optimize database.", 'o');
  args.addSwitch("check", "Check database integrity", 'e');
  args.addSwitch("index", "Index database", 'i');
  args.addSwitch("disable-foreign",
                 "Disable foreign key check during population process. "
                 "WARNING: may result in broken database.");

  if (args.parse(argc, argv))
    return 1;

  Logging::Timer total_elapsed;

  VCFLite::Connector db{*args.getValue("db_path"), args.isSet("create"),
                        args.isSet("disable-foreign")};

  if (args.isSet("vcf")){
    int total_records = 0;

    Logging::Timer insert_elapsed;

    for (const auto &vcf_file : args.getIterable("vcf")) {
      total_records += db.parseVCF(vcf_file, args.getValue("samples"));
    }

    insert_elapsed.mark();
    std::clog << "[LOG] Inserted " << total_records
              << " in " << insert_elapsed << std::endl;
  }

  if (args.isSet("index"))
    db.index();

  if (args.isSet("optimize"))
    db.optimize();

  if (args.isSet("check"))
    db.check();

  total_elapsed.mark();
  std::clog << "[LOG] Operations completed in " << total_elapsed << "\n";

  return 0;
}
