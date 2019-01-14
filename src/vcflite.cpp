#include <iostream>

#include <agizmo/args.hpp>
#include <hkl/vcf.hpp>
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

  args.parse(argc, argv);

  VCFLite::Connector db{*args.getArg("db_path").getValue(),
                        args.getArg("create").isSet()};

  VCF::VCFReader reader(*args.getArg("vcf_file").getValue());

  while (auto ele = reader()) {
    std::visit(
        [&args, &reader](auto &arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, VCF::VCFComment>) {
          } else if constexpr (std::is_same_v<T, VCF::VCFHeader>) {
            if (const auto samples = args.getArg("samples").getValue())
              reader.provideSamples(*samples);
          } else {
          }
        },
        *ele);
  }

  return 0;
}
