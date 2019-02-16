#include <iostream>

#include <agizmo/args.hpp>
#include <vcflite/connect.hpp>

using namespace AGizmo;
using namespace HKL;

int main(int argc, char *argv[]) {
  Args::NewArguments args{};

  args.addPositional("db_path", "Path to new database file");
  args.addObligatory("vcf_file", "Path to vcf file", 'v');
  args.allowMultipleValues("vcf_file");
  args.addArgument("samples", "List with samples, delimetered with ','.", 's');
  args.addSwitch("create", "Force to create db", 'c');
  args.addSwitch("optimize", "Optimize database.", 'o');
  args.addSwitch("check", "Check database integrity", 'e');
  args.addSwitch("index", "Index database", 'i');
  args.addSwitch("disable-foreign",
                 "Disable foreign key check during population process. "
                 "WARNING: may result in broken database.");

  args.parse(argc, argv);

  return 0;

  //  VCFLite::Connector db{*args.getArg("db_path").getValue(),
  //                        args.getArg("create").isSet(),
  //                        args.getArg("disable-foreign").isSet()};

  //  if (const auto &vcf_file = args.getArg("vcf_file").getValue())
  //    db.parseVCF(*vcf_file, args.getArg("samples").getValue());

  //  if (args.getArg("check").isSet())
  //    db.check();

  //  if (args.getArg("index").isSet())
  //    db.index();

  //  if (args.getArg("optimize").isSet())
  //    db.optimize();

  //  return 0;
}
