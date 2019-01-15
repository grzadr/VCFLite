#pragma once

#include <string>

#include <sqlite3.h>

#include <hkl/vcf.hpp>

using std::string;

namespace VCFLite {

int insert_meta(const string& field);

int insert_comment_proper(sqlite3* db, const string& field,
                          const string& value);
int insert_comment_simple(sqlite3* db, const string& field,
                          const string& value);

int insert_comment(sqlite3* db, const HKL::VCF::VCFComment& comment);
}  // namespace VCFLite
