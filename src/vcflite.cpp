#include <iostream>
#include <optional>

#include <agizmo/args.hpp>
#include <agizmo/logging.hpp>
#include <vcflite/connect.hpp>

using namespace AGizmo;
using namespace HKL;

using std::cerr;
using opt_int = std::optional<int>;

int main(int argc, char *argv[]) {
  Args::Arguments args{"VCFLite", "Converts VCF files into SQLite databases.",
                       "19-04-30"};

  args.addPositional("db_path", "Path to a database file", true);
  args.addMulti("vcf", "Path to vcf file", 'v');

  args.addArgument("samples", "List with samples, delimetered with ','", 's');
  args.enableAppend("samples", ',');

  args.addSwitch("build", "Force to build db", 'b');
  args.addSwitch("optimize", "Optimize database", 'o');
  args.addSwitch("check", "Check database integrity", 'c');
  args.addSwitch("index", "Index database", 'i');
  args.addSwitch("disable-foreign",
                 "Disable foreign key check during population process. "
                 "WARNING: may result in broken database");
  args.addArgument("limit", "Positive number of rows to process", 'l');

  if (args.parse(argc, argv))
    return 1;

  Logging::Timer total_elapsed;

  VCFLite::Connector db{*args.getValue("db_path"), args.isSet("build"),
                        args.isSet("disable-foreign")};

  if (args.isSet("vcf")) {
    int total_records = 0;

    Logging::Timer insert_elapsed;

    const auto samples = args.getIterable("samples");

    if (!samples.empty())
      cerr << "Selected samples: "
           << StringCompose::str_join(samples.begin(), samples.end(), " - ")
           << "\n";

    opt_int limit;
    if (const auto temp_limit = args.getValue("limit")) {
      limit = StringFormat::str_to_int(*temp_limit);
      if (!limit.has_value())
        throw runerror{"Limit must be a positive number, not '" + *temp_limit +
                       "'"};
    }

    for (const auto &vcf_file : args.getIterable("vcf")) {
      if (limit && limit < 1)
        break;
      total_records += db.parseVCF(vcf_file, samples, limit);
      cerr << total_records << "\n";
      if (limit)
        *limit -= total_records;
    }

    insert_elapsed.mark();
    std::clog << "[LOG] Inserted " << total_records << " in " << insert_elapsed
              << std::endl;
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
