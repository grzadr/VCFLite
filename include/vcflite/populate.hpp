#pragma once

#include <optional>
#include <string>

#include <sqlite3.h>

#include <hkl/vcf.hpp>

using std::string;
using opt_str = std::optional<string>;

namespace VCFLite {

int insert_meta(sqlite3* db, const string& field, const std::string& id,
                opt_str description, opt_str type, opt_str number,
                opt_str source, opt_str version);
int insert_meta_extra(sqlite3* db, const string& field, const std::string& id,
                      string key, string value);

int insert_comment_proper(sqlite3* db, const string& field,
                          const string& value);
int insert_comment_simple(sqlite3* db, const string& field,
                          const string& value);

int insert_comment(sqlite3* db, const HKL::VCF::VCFComment& comment);
}  // namespace VCFLite
