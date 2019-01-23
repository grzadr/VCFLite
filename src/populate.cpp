#include <vcflite/populate.hpp>

#include <iostream>

#include <vcflite/query.hpp>
#include <vcflite/select.hpp>

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

int VCFLite::insert_variant(sqlite3 *db, const int id_variant,
                            const string &variant_chrom, int variant_start,
                            int variant_end, int variant_length,
                            const string &variant_ref, opt_double variant_qual,
                            opt_int variant_pass, int variant_alleles) {
  sqlite3_stmt *stmt;

  sqlite3_prepare_v2(db,
                     "INSERT INTO Variants "
                     "(id_variant, variant_chrom, variant_start, variant_end, "
                     "variant_length, variant_ref, variant_qual, variant_pass, "
                     "variant_alleles) "
                     "VALUES (?1,  ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9);",
                     -1, &stmt, nullptr);

  int index = 0;

  sqlite3_bind_int(stmt, ++index, id_variant);
  sqlite3_bind_text(stmt, ++index, variant_chrom.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, ++index, variant_start);
  sqlite3_bind_int(stmt, ++index, variant_end);
  sqlite3_bind_int(stmt, ++index, variant_length);
  sqlite3_bind_text(stmt, ++index, variant_ref.c_str(), -1, SQLITE_TRANSIENT);

  if (auto qual = variant_qual)
    sqlite3_bind_double(stmt, ++index, *qual);
  else
    sqlite3_bind_null(stmt, ++index);

  if (auto pass = variant_pass)
    sqlite3_bind_int(stmt, ++index, *pass);
  else
    sqlite3_bind_null(stmt, ++index);

  sqlite3_bind_int(stmt, ++index, variant_alleles);

  return finalize(db, stmt);
}

int VCFLite::insert_variant_ids(sqlite3 *db, const int id_variant,
                                const vector<string> ids) {
  sqlite3_stmt *stmt;

  const string query =
      "INSERT INTO VariantsIDs "
      "(id_variant, variant_id) "
      "VALUES (?1,  ?2);";

  for (const auto &id : ids) {
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id_variant);
    sqlite3_bind_text(stmt, 2, id.c_str(), -1, SQLITE_TRANSIENT);
    finalize(db, stmt);
  }

  return SQLITE_OK;
}

int VCFLite::insert_variant_filters(sqlite3 *db, const int id_variant,
                                    const opt_vec_str &variant_filters) {
  if (const auto &filters = variant_filters) {
    sqlite3_stmt *stmt;

    const string query =
        "INSERT INTO VariantsFilters "
        "(id_variant, variant_filter) "
        "VALUES (?1, ?2);";

    for (const auto &filter : *filters) {
      sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

      sqlite3_bind_int(stmt, 1, id_variant);
      sqlite3_bind_text(stmt, 2, filter.c_str(), -1, SQLITE_TRANSIENT);

      finalize(db, stmt);
    }
  }
  return SQLITE_OK;
}

int VCFLite::insert_variant_alleles(sqlite3 *db, const int id_variant,
                                    const string &variant_ref,
                                    const vec_str &variant_alt) {
  sqlite3_stmt *stmt;

  const string query =
      "INSERT INTO VariantsAlleles "
      "(id_variant, variant_allele_id, variant_allele_seq) "
      "VALUES (?1, ?2, ?3);";

  int allele_id = 0;

  sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

  sqlite3_bind_int(stmt, 1, id_variant);
  sqlite3_bind_int(stmt, 2, allele_id);
  sqlite3_bind_text(stmt, 3, variant_ref.c_str(), -1, SQLITE_TRANSIENT);

  finalize(db, stmt);

  for (const auto &alt : variant_alt) {
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    sqlite3_bind_int(stmt, 1, id_variant);
    sqlite3_bind_int(stmt, 2, ++allele_id);
    sqlite3_bind_text(stmt, 3, alt.c_str(), -1, SQLITE_TRANSIENT);

    finalize(db, stmt);
  }

  return SQLITE_OK;
}

int VCFLite::insert_variant_info(sqlite3 *db, const int id_variant,
                                 const map_str &variant_info) {
  sqlite3_stmt *stmt;
  const string query =
      "INSERT INTO VariantsInfo "
      "(id_variant, variant_key, variant_value) "
      "VALUES (?1, ?2, ?3);";

  for (const auto &[variant_key, value] : variant_info) {
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    sqlite3_bind_int(stmt, 1, id_variant);
    sqlite3_bind_text(stmt, 2, variant_key.c_str(), -1, SQLITE_TRANSIENT);
    if (auto variant_value = value) {
      sqlite3_bind_text(stmt, 3, variant_value->c_str(), -1, SQLITE_TRANSIENT);
    } else
      sqlite3_bind_null(stmt, 3);

    finalize(db, stmt);
  }

  return SQLITE_OK;
}

int VCFLite::insert_variant_genotypes(
    sqlite3 *db, const int id_variant,
    const vector<HKL::VCF::VCFGenotype> genotypes,
    const vector<string> &samples_reference,
    const vector<int> &samples_picked) {
  sqlite3_stmt *stmt;
  const string query =
      "INSERT INTO Genotypes "
      "(id_variant, genotype_sample, genotype_gt, "
      "genotype_dp, genotype_phased_id_variant, genotype_phased_gt) "
      "VALUES (?1,  ?2, ?3, ?4, ?5, ?6);";

  for (const auto &i : samples_picked) {
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    const auto genotype_sample = samples_reference.begin() + i;
    const auto genotype_record = *(genotypes.begin() + i);

    sqlite3_bind_int(stmt, 1, id_variant);
    sqlite3_bind_text(stmt, 2, genotype_sample->c_str(), -1, SQLITE_TRANSIENT);

    if (const auto gt = genotype_record.getGT()) {
      sqlite3_bind_text(stmt, 3, gt->c_str(), -1, SQLITE_TRANSIENT);

      if (const auto gt = genotype_record.get("DP");
          gt.has_value() && *gt != ".")
        sqlite3_bind_int(stmt, 4, std::stoi((*gt)->c_str()));
      else
        sqlite3_bind_null(stmt, 4);

      finalize(db, stmt);

      const string query_genotype_alleles =
          "INSERT INTO GenotypesAlleles "
          "(id_variant, genotype_sample, genotype_position, variant_allele_id) "
          "VALUES (?1,  ?2, ?3, ?4);";

      for (const auto &allele : genotype_record.getAlleles()) {
        sqlite3_prepare_v2(db, query_genotype_alleles.c_str(), -1, &stmt,
                           nullptr);

        sqlite3_bind_int(stmt, 1, id_variant);
        sqlite3_bind_text(stmt, 2, genotype_sample->c_str(), -1,
                          SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, allele.getPosition());

        if (const auto &gt = allele.getAllele())
          sqlite3_bind_int(stmt, 4, *gt);
        else
          sqlite3_bind_null(stmt, 4);

        finalize(db, stmt);
      }
    } else {
      sqlite3_bind_null(stmt, 3);
      sqlite3_bind_null(stmt, 4);
    }

    if (genotype_record.isPhased()) {
      sqlite3_bind_text(stmt, 7, genotype_record.getPhasedGT()->c_str(), -1,
                        SQLITE_TRANSIENT);
      auto phased_id_variant = Select::phased_variant_id(
          db, id_variant, *genotype_record.getPhasedPosition());
      sqlite3_bind_int(stmt, 8, phased_id_variant);
    } else {
      sqlite3_bind_null(stmt, 7);
      sqlite3_bind_null(stmt, 8);
    }

    const string query_genotypeinfo =
        "INSERT INTO GenotypesInfo "
        "(id_variant, genotype_sample, genotype_key, genotype_value) "
        "VALUES (?1,  ?2, ?3, ?4);";

    for (const auto &[key, value] : genotype_record) {
      sqlite3_prepare_v2(db, query_genotypeinfo.c_str(), -1, &stmt, nullptr);

      const auto genotype_sample = samples_reference.begin() + i;

      sqlite3_bind_int(stmt, 1, id_variant);
      sqlite3_bind_text(stmt, 2, genotype_sample->c_str(), -1,
                        SQLITE_TRANSIENT);
      sqlite3_bind_text(stmt, 3, key.c_str(), -1, SQLITE_TRANSIENT);

      if (const auto genotype_value = value)
        sqlite3_bind_text(stmt, 4, genotype_value->c_str(), -1,
                          SQLITE_TRANSIENT);
      else
        sqlite3_bind_null(stmt, 4);

      finalize(db, stmt);
    }
  }

  return SQLITE_OK;
}

int VCFLite::insert_record(sqlite3 *db, const int id_variant,
                           const HKL::VCF::VCFRecord &record,
                           const vector<string> &samples_reference,
                           const vector<int> &samples_picked) {
  insert_variant(db, id_variant, record.getChrom(), record.getStart(),
                 record.getEnd(), record.getLength(), record.getRef(),
                 record.getQual(), record.getPass(), record.getAlleles());

  if (auto ids = record.getIDs(); !ids.empty())
    insert_variant_ids(db, id_variant, ids);

  insert_variant_filters(db, id_variant, record.getFilters());

  insert_variant_alleles(db, id_variant, record.getRef(), record.getAlt());

  insert_variant_info(db, id_variant, record.getInfo());

  if (auto var_genotypes = record.getGenotypes();
      !var_genotypes.empty() && !samples_picked.empty())
    insert_variant_genotypes(db, id_variant, var_genotypes, samples_reference,
                             samples_picked);

  return SQLITE_OK;
}
