#pragma once

#include <optional>
#include <string>
#include <vector>

#include <sqlite3.h>

#include <agizmo/printable.hpp>
#include <hkl/vcf.hpp>

using std::string;
using opt_str = std::optional<string>;
using opt_double = std::optional<double>;
using opt_int = std::optional<int>;

using std::vector;
using vec_str = std::vector<string>;
using opt_vec_str = std::optional<vec_str>;

using map_str = AGizmo::Printable::PrintableStrMap;

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

int insert_variant(sqlite3* db, const int id_variant,
                   const string& variant_chrom, int variant_start,
                   int variant_end, int variant_length,
                   const string& variant_ref, opt_double variant_qual,
                   opt_int variant_pass, int variant_alleles);

int insert_variant_ids(sqlite3* db, const int id_variant,
                       const vector<string> ids);

int insert_variant_filters(sqlite3* db, const int id_variant,
                           const opt_vec_str& variant_filters);

int insert_variant_alleles(sqlite3* db, const int id_variant,
                           const string& variant_ref,
                           const vec_str& variant_alt);

int insert_variant_info(sqlite3* db, const int id_variant,
                        const map_str& variant_info);

int insert_variant_genotypes(sqlite3* db, const int id_variant,
                             const vector<HKL::VCF::VCFGenotype> genotypes,
                             const vector<string>& samples_reference,
                             const vector<int>& samples_picked);

int insert_record(sqlite3* db, const int new_id,
                  const HKL::VCF::VCFRecord& record,
                  const vector<string>& samples_reference,
                  const vector<int>& samples_picked);
}  // namespace VCFLite
