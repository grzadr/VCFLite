#include <vcflite/connect.hpp>
#include <vcflite/populate.hpp>
#include <vcflite/select.hpp>

int VCFLite::Connector::open(const std::__cxx11::string &db_path, bool create) {
  //    sqlite3* db = sqlite.get();

  auto flag = create ? SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
                     : SQLITE_OPEN_READWRITE;

  last_result_code = sqlite3_open_v2(db_path.c_str(), &db, flag, nullptr);

  if (last_result_code)
    throw runerror{"Can't open database: " + string(sqlite3_errmsg(db)) + "\n"};

  if (create)
    creator.init(db);

  return last_result_code;
}

int VCFLite::Connector::check() {
  exec(db, "PRAGMA foreign_key_check;");
  exec(db, "PRAGMA integrity_check;");
  return 0;
}

int VCFLite::Connector::optimize() {
  std::clog << "[LOG] Optimizing database\n";
  exec(db, "PRAGMA optimize;");
  return 0;
}

int VCFLite::Connector::parseVCF(const string &vcf_file,
                                 const optional<string> &samples) {
  std::clog << "[LOG] Parsing VCF: " << vcf_file << "\n";

  VCF::VCFReader reader(vcf_file);

  auto id_variant = Select::max_variant_id(db);

  transaction(db);

  while (auto ele = reader()) {
    if (id_variant % 25000 == 0)
      std::clog << "Last inserted id: " << id_variant << "\n";

    if (std::holds_alternative<VCF::VCFComment>(*ele))
      insert_comment(db, std::get<VCF::VCFComment>(*ele));
    else if (std::holds_alternative<VCF::VCFHeader>(*ele)) {
      if (samples.has_value())
        reader.provideSamples(*samples);
    } else
      insert_record(db, ++id_variant, std::get<VCF::VCFRecord>(*ele),
                    reader.getSamplesReference(), reader.getSamplesPicked());
  }

  commit(db);

  std::clog << "[LOG] Parsing Completed\n";

  return 0;
}
