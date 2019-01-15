#include <vcflite/populate.hpp>

#include <iostream>
#include <tuple>

#include <agizmo/printable.hpp>
#include <vcflite/query.hpp>

using map_str = AGizmo::Printable::PrintableStrMap;

int VCFLite::insert_comment_proper(sqlite3 *db, const std::string &field,
                                   const std::string &value) {
  map_str data{value};

  if (const auto id = data.get("ID")) {
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db,
                       "INSERT INTO MetaInfo "
                       "(meta_field,  meta_id,  meta_description, meta_type, "
                       "meta_number, meta_source, meta_version) "
                       "VALUES (?1,  ?2, ?3, ?4, ?5, ?6, ?7);",
                       -1, &stmt, nullptr);

    for (const auto &[key, content] : map_str(value, ',', '=', '"')) {
      if (!content.has_value() || content->empty())
        throw runerror{"Field '" + key + "' is missing value\n##" + field +
                       "=<" + value + ">"};
    }
  } else
    throw runerror{"Comment missing 'ID' field\n" + field + "=<" + value + ">"};

  return 0;
}

int VCFLite::insert_comment_simple(sqlite3 *db, const std::string &field,
                                   const std::string &value) {
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
  if (comment.isProper()) {
    return insert_comment_proper(db, comment.getField(), comment.getValue());
  } else
    return insert_comment_simple(db, comment.getField(), comment.getValue());
}
