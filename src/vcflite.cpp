#include <iostream>

#include <agizmo/args.hpp>
#include <vcflite/connect.hpp>

using namespace AGizmo;
using namespace HKL;

int main(int argc, char *argv[]) {
  Args::Arguments args{};

  args.addArgument(1, "db_path", "Path to new database file",
                   Args::ValueType::String);
  args.addArgument("vcf_file", "Path to vcf file", Args::ValueType::String,
                   'v');
  args.addArgument("create", "Force to create db", Args::ValueType::Bool, 'c');
  args.addArgument("samples", "List with samples, delimetered with ','.",
                   Args::ValueType::String, 's');
  args.addArgument("optimize", "Optimize database.", Args::ValueType::Bool,
                   'o');

  args.parse(argc, argv);

  VCFLite::Connector db{*args.getArg("db_path").getValue(),
                        args.getArg("create").isSet()};

  if (const auto &vcf_file = args.getArg("vcf_file").getValue())
    db.parseVCF(*vcf_file, args.getArg("samples").getValue());

  db.check();

  if (args.getArg("optimize").isSet()) db.optimize();

  return 0;
}
