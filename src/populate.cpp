#include <vcflite/populate.hpp>

#include <iostream>

#include <vcflite/query.hpp>

int VCFLite::insert_meta(sqlite3 *db, const string &field, const string &id,
                         opt_str description, opt_str type, opt_str number,
                         opt_str source, opt_str version) {
  sqlite3_stmt *stmt;

  sqlite3_prepare_v2(db,
                     "INSERT INTO MetaInfo "
                     "(meta_field, meta_id, meta_description, meta_type, "
                     "meta_number, meta_source, meta_version) "
                     "VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7);",
                     -1, &stmt, nullptr);

  sqlite3_bind_text(stmt, 1, field.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, id.c_str(), -1, SQLITE_TRANSIENT);

  if (auto insert = description)
    sqlite3_bind_text(stmt, 3, insert->c_str(), -1, SQLITE_TRANSIENT);
  else
    sqlite3_bind_null(stmt, 3);

  if (auto insert = type)
    sqlite3_bind_text(stmt, 4, insert->c_str(), -1, SQLITE_TRANSIENT);
  else
    sqlite3_bind_null(stmt, 4);

  if (auto insert = number)
    sqlite3_bind_text(stmt, 5, insert->c_str(), -1, SQLITE_TRANSIENT);
  else
    sqlite3_bind_null(stmt, 5);

  if (auto insert = source)
    sqlite3_bind_text(stmt, 6, insert->c_str(), -1, SQLITE_TRANSIENT);
  else
    sqlite3_bind_null(stmt, 6);

  if (auto insert = version)
    sqlite3_bind_text(stmt, 7, insert->c_str(), -1, SQLITE_TRANSIENT);
  else
    sqlite3_bind_null(stmt, 7);

  if (sqlite3_step(stmt) != SQLITE_DONE)
    panic(db, string(sqlite3_expanded_sql(stmt)));

  return sqlite3_finalize(stmt);
}

int VCFLite::insert_meta_extra(sqlite3 *db, const std::string &field,
                               const std::string &id, std::string key,
                               std::string value) {
  sqlite3_stmt *stmt;

  sqlite3_prepare_v2(db,
                     "INSERT INTO MetaInfoExtra "
                     "(meta_field, meta_id, meta_extra_key, meta_extra_value) "
                     "VALUES (?1, ?2, ?3, ?4);",
                     -1, &stmt, nullptr);

  sqlite3_bind_text(stmt, 1, field.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, id.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, key.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 4, value.c_str(), -1, SQLITE_TRANSIENT);

  return finalize(db, stmt);
}

int VCFLite::insert_comment_proper(sqlite3 *db, const string &field,
                                   const string &value) {
  map_str data{value, ',', '=', '"'};

  if (const auto id = *data.get("ID")) {
    //    std::cerr << *id << "\n";

    opt_str description{};
    opt_str type{};
    opt_str number{};
    opt_str source{};
    opt_str version{};

    for (const auto &[key, content] : data) {
      if (!content.has_value() || content->empty())
        throw runerror{"Field '" + key + "' is missing value\n##" + field +
                       "=<" + value + ">"};
      if (key == "ID")
        continue;
      else if (key == "Description")
        description = content;
      else if (key == "Type")
        type = content;
      else if (key == "Number")
        number = content;
      else if (key == "Source")
        source = content;
      else if (key == "Version")
        version = content;
      else
        insert_meta_extra(db, field, *id, key, *content);
    }

    insert_meta(db, field, *id, description, type, number, source, version);

  } else
    throw runerror{"Comment missing 'ID' field\n" + field + "=<" + value + ">"};

  return 0;
}

int VCFLite::insert_comment_simple(sqlite3 *db, const string &field,
                                   const string &value) {
  sqlite3_stmt *stmt;

  sqlite3_prepare_v2(
      db, "INSERT INTO MetaInfo (meta_field, meta_id) values (?1, ?2);", -1,
      &stmt, nullptr);

  sqlite3_bind_text(stmt, 1, field.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, value.c_str(), -1, SQLITE_TRANSIENT);

  if (sqlite3_step(stmt) != SQLITE_DONE)
    panic(db, string(sqlite3_expanded_sql(stmt)));

  return sqlite3_finalize(stmt);
}

int VCFLite::insert_comment(sqlite3 *db, const HKL::VCF::VCFComment &comment) {
  if (comment.isProper())
    return insert_comment_proper(db, comment.getField(), comment.getValue());
  else
    return insert_comment_simple(db, comment.getField(), comment.getValue());
}

int VCFLite::insert_variant(sqlite3 *db, const string &var_chrom, int var_start,
                            int var_end, int var_length,
                            const std::string &var_ref, opt_double var_qual,
                            opt_int var_pass, int var_alleles) {
  sqlite3_stmt *stmt;

  sqlite3_prepare_v2(db,
                     "INSERT INTO Variants "
                     "(var_chrom, var_start, var_end, var_length, "
                     "var_ref, var_qual, var_pass, var_alleles) "
                     "VALUES (?1,  ?2, ?3, ?4, ?5, ?6, ?7, ?8);",
                     -1, &stmt, nullptr);

  sqlite3_bind_text(stmt, 1, var_chrom.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 2, var_start);
  sqlite3_bind_int(stmt, 3, var_end);
  sqlite3_bind_int(stmt, 4, var_length);
  sqlite3_bind_text(stmt, 5, var_ref.c_str(), -1, SQLITE_TRANSIENT);

  if (auto qual = var_qual)
    sqlite3_bind_double(stmt, 6, *qual);
  else
    sqlite3_bind_null(stmt, 6);

  if (auto pass = var_pass)
    sqlite3_bind_int(stmt, 7, *pass);
  else
    sqlite3_bind_null(stmt, 7);

  sqlite3_bind_int(stmt, 8, var_alleles);

  return finalize(db, stmt);
}

int VCFLite::insert_variant_ids(sqlite3 *db, const string &var_chrom,
                                int var_start, int var_end,
                                const vector<string> ids) {
  sqlite3_stmt *stmt;

  const string query =
      "INSERT INTO VariantsIDs "
      "(var_chrom, var_start, var_end, var_id) "
      "VALUES (?1,  ?2, ?3, ?4);";

  for (const auto &id : ids) {
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, var_chrom.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, var_start);
    sqlite3_bind_int(stmt, 3, var_end);
    sqlite3_bind_text(stmt, 4, id.c_str(), -1, SQLITE_TRANSIENT);
    finalize(db, stmt);
  }

  return SQLITE_OK;
}

int VCFLite::insert_variant_filters(sqlite3 *db, const std::string &var_chrom,
                                    int var_start, int var_end,
                                    const opt_vec_str &var_filters) {
  if (const auto &filters = var_filters) {
    sqlite3_stmt *stmt;

    const string query =
        "INSERT INTO VariantsFilters "
        "(var_chrom, var_start, var_end, var_filter) "
        "VALUES (?1,  ?2, ?3, ?4);";

    for (const auto &filter : *filters) {
      sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

      sqlite3_bind_text(stmt, 1, var_chrom.c_str(), -1, SQLITE_TRANSIENT);
      sqlite3_bind_int(stmt, 2, var_start);
      sqlite3_bind_int(stmt, 3, var_end);
      sqlite3_bind_text(stmt, 4, filter.c_str(), -1, SQLITE_TRANSIENT);

      finalize(db, stmt);
    }
  }
  return SQLITE_OK;
}

int VCFLite::insert_variant_alleles(sqlite3 *db, const std::string &var_chrom,
                                    int var_start, int var_end,
                                    const std::string &var_ref,
                                    const vec_str &var_alt) {
  sqlite3_stmt *stmt;

  const string query =
      "INSERT INTO VariantsAlleles "
      "(var_chrom, var_start, var_end, var_allel_id, var_allel_seq) "
      "VALUES (?1,  ?2, ?3, ?4, ?5);";

  int allel_id = 0;

  sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

  sqlite3_bind_text(stmt, 1, var_chrom.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 2, var_start);
  sqlite3_bind_int(stmt, 3, var_end);
  sqlite3_bind_int(stmt, 4, allel_id);
  sqlite3_bind_text(stmt, 5, var_ref.c_str(), -1, SQLITE_TRANSIENT);

  finalize(db, stmt);

  for (const auto &alt : var_alt) {
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, var_chrom.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, var_start);
    sqlite3_bind_int(stmt, 3, var_end);
    sqlite3_bind_int(stmt, 4, ++allel_id);
    sqlite3_bind_text(stmt, 5, alt.c_str(), -1, SQLITE_TRANSIENT);

    finalize(db, stmt);
  }

  return SQLITE_OK;
}

int VCFLite::insert_variant_info(sqlite3 *db, const std::string &var_chrom,
                                 int var_start, int var_end,
                                 const map_str &var_info) {
  sqlite3_stmt *stmt;
  const string query =
      "INSERT INTO VariantsInfo "
      "(var_chrom, var_start, var_end, var_key, var_value) "
      "VALUES (?1,  ?2, ?3, ?4, ?5);";

  for (const auto &[var_key, value] : var_info) {
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, var_chrom.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, var_start);
    sqlite3_bind_int(stmt, 3, var_end);
    sqlite3_bind_text(stmt, 4, var_key.c_str(), -1, SQLITE_TRANSIENT);
    if (auto var_value = value) {
      sqlite3_bind_text(stmt, 5, var_value->c_str(), -1, SQLITE_TRANSIENT);
    } else
      sqlite3_bind_null(stmt, 5);

    finalize(db, stmt);
  }

  return SQLITE_OK;
}

int VCFLite::insert_record(sqlite3 *db, const HKL::VCF::VCFRecord &record,
                           const vector<string> &samples_reference,
                           const vector<int> &picked_samples) {
  insert_variant(db, record.getChrom(), record.getStart(), record.getEnd(),
                 record.getLength(), record.getRef(), record.getQual(),
                 record.getPass(), record.getAlleles());

  if (auto ids = record.getIDs(); !ids.empty())
    insert_variant_ids(db, record.getChrom(), record.getStart(),
                       record.getEnd(), ids);

  insert_variant_filters(db, record.getChrom(), record.getStart(),
                         record.getEnd(), record.getFilters());

  insert_variant_alleles(db, record.getChrom(), record.getStart(),
                         record.getEnd(), record.getRef(), record.getAlt());

  insert_variant_info(db, record.getChrom(), record.getStart(), record.getEnd(),
                      record.getInfo());

  return 0;
}
