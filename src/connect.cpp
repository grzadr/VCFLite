#include <vcflite/connect.hpp>
#include <vcflite/populate.hpp>

int VCFLite::Connector::open(const std::__cxx11::string &db_path, bool create) {
  //    sqlite3* db = sqlite.get();

  auto flag = create ? SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
                     : SQLITE_OPEN_READWRITE;

  last_result_code = sqlite3_open_v2(db_path.c_str(), &db, flag, nullptr);

  if (last_result_code)
    throw runerror{"Can't open database: " + string(sqlite3_errmsg(db)) + "\n"};

  if (create) creator.init(db);

  return last_result_code;
}

int VCFLite::Connector::parseVCF(const string &vcf_file,
                                 const optional<string> &samples) {
  VCF::VCFReader reader(vcf_file);

  transaction(db);

  while (auto ele = reader()) {
    if (std::holds_alternative<VCF::VCFComment>(*ele)) {
      insert_comment(db, std::get<VCF::VCFComment>(*ele));
    } else if (std::holds_alternative<VCF::VCFHeader>(*ele)) {
      if (samples.has_value()) reader.provideSamples(*samples);
      break;
    } else {
    }

    //    break;
  }

  commit(db);

  return 0;
}
