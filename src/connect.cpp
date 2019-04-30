#include <vcflite/connect.hpp>
#include <vcflite/populate.hpp>
#include <vcflite/query.hpp>
#include <vcflite/select.hpp>

#include <agizmo/logging.hpp>

using namespace AGizmo;

int VCFLite::Connector::open(const std::string &db_path, bool create,
                             bool disable_foreign) {

  auto flag = create ? SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
                     : SQLITE_OPEN_READWRITE;

  last_result_code = sqlite3_open_v2(db_path.c_str(), &db, flag, nullptr);

  if (last_result_code)
    throw runerror{"Can't open database: " + string(sqlite3_errmsg(db)) + "\n"};

  exec(db, "PRAGMA encoding = 'UTF-8';");
  exec(db, "PRAGMA journal_mode = MEMORY;");
  exec(db, "PRAGMA locking_mode = EXCLUSIVE;");

  if (disable_foreign)
    exec(db, "PRAGMA foreign_keys = 0;");
  else
    exec(db, "PRAGMA foreign_keys = 1;");

  if (create)
    creator.init(db);

  return last_result_code;
}

int VCFLite::Connector::check() {
  std::clog << "[LOG] Checking database\n";
  Logging::Timer elapsed;

  exec(db, "PRAGMA foreign_key_check;");
  exec(db, "PRAGMA integrity_check;");

  elapsed.mark();
  std::clog << "[LOG] Database check completed in " << elapsed << std::endl;

  return SQLITE_OK;
}

int VCFLite::Connector::optimize() {
  std::clog << "[LOG] Optimizing database\n";
  Logging::Timer elapsed;

  exec(db, "PRAGMA optimize;");

  elapsed.mark();
  std::clog << "[LOG] Database optimized in " << elapsed << std::endl;

  return SQLITE_OK;
}

int VCFLite::Connector::index() {
  std::clog << "[LOG] Indexing database" << std::endl;
  Logging::Timer elapsed;

  creator.index(db);

  elapsed.mark();
  std::clog << "[LOG] Database indexed in " << elapsed << std::endl;

  return last_result_code;
}

int VCFLite::Connector::parseVCF(const string &vcf_file,
                                 const vector<string> &samples, opt_int limit) {
  std::clog << "[LOG] Parsing VCF: " << vcf_file << "\n";

  VCF::VCFReader reader(vcf_file);

  const auto initial_id_variant = Select::max_variant_id(db);
  auto id_variant = initial_id_variant + 1;
  int total_parsed = 0;

  Logging::Timer total_time;
  Logging::Timer elapsed;

  transaction(db);

  std::cerr << limit.value_or(-1) << "\n";

  while (auto ele = reader()) {
    if (limit && total_parsed >= *limit)
      break;
    if (++total_parsed % 5000 == 0)
      std::clog << "Parsed " << total_parsed << " lines\n";

    if (std::holds_alternative<VCF::VCFComment>(*ele))
      insert_comment(db, vcf_file, std::get<VCF::VCFComment>(*ele));
    else if (std::holds_alternative<VCF::VCFHeader>(*ele)) {
      reader.provideSamples(samples);
    } else
      insert_record(db, vcf_file, id_variant++, std::get<VCF::VCFRecord>(*ele),
                    reader.getSamplesReference(), reader.getSamplesPicked());
  }

  elapsed.mark();

  std::clog << "[LOG] Completed VCF: " << vcf_file
            << ";Inserted : " << id_variant - initial_id_variant
            << " records in " << elapsed << "\n[LOG] Commiting changes\n";

  elapsed.reset();

  commit(db);

  elapsed.mark();

  std::clog << "[LOG] Commited in " << elapsed << "\n";
  total_time.mark();

  std::clog << "[LOG] Parsing Completed in " << total_time << "\n";

  return total_parsed;
}
