#include <vcflite/populate.hpp>

#include <iostream>

#include <agizmo/printable.hpp>
#include <vcflite/query.hpp>

using map_str = AGizmo::Printable::PrintableStrMap;

int VCFLite::insert_meta(sqlite3 *db, const string &field, const string &id,
                         opt_str description, opt_str type, opt_str number,
                         opt_str source, opt_str version) {
  sqlite3_stmt *stmt;

  sqlite3_prepare_v2(db,
                     "INSERT INTO MetaInfo "
                     "(meta_field,  meta_id,  meta_description, meta_type, "
                     "meta_number, meta_source, meta_version) "
                     "VALUES (?1,  ?2, ?3, ?4, ?5, ?6, ?7);",
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

  sqlite3_prepare_v2(
      db,
      "INSERT INTO MetaInfoExtra "
      "(meta_field,  meta_id,  meta_extra_key, meta_extra_value) "
      "VALUES (?1,  ?2, ?3, ?4);",
      -1, &stmt, nullptr);

  sqlite3_bind_text(stmt, 1, field.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, id.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, key.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 4, value.c_str(), -1, SQLITE_TRANSIENT);

  if (sqlite3_step(stmt) != SQLITE_DONE)
    panic(db, string(sqlite3_expanded_sql(stmt)));

  return sqlite3_finalize(stmt);
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
