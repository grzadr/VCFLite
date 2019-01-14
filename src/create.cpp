#include <vcflite/create.hpp>

int VCFLite::Creator::init(sqlite3 *db) {
  sqlite3_stmt *stmt;

  vector<string> drop_queries{
      "DROP TABLE IF EXISTS MetaInfo;",
      "DROP TABLE IF EXISTS MetaInfoContigs;",
      "DROP TABLE IF EXISTS MetaInfoExtra;",
      "DROP TABLE IF EXISTS Variants;",
      "DROP TABLE IF EXISTS VariantsID;",
      "DROP TABLE IF EXISTS VariantsAlt;",
      "DROP TABLE IF EXISTS VariantsFilter;",
      "DROP TABLE IF EXISTS VariantsInfo;",
  };

  for (const auto &query : drop_queries) {
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
      throw runerror{"ERROR inserting data: " + string(sqlite3_errmsg(db)) +
                     "\n"};
    }
    sqlite3_finalize(stmt);
  }

  vector<string> create_queries{
      "CREATE TABLE MetaInfo("
      "id_meta INTEGER PRIMARY KEY NOT NULL,"
      "meta_field TEXT NOT NULL COLLATE NOCASE,"
      "meta_description TEXT NOT NULL COLLATE NOCASE,"
      "meta_id TEXT DEFAULT NULL COLLATE NOCASE,"
      "meta_type TEXT DEFAULT NULL COLLATE NOCASE,"
      "meta_number TEXT DEFAULT NULL COLLATE NOCASE);",

      "CREATE TABLE MetaInfoExtra("
      "id_meta INTEGER NOT NULL,"
      "meta_extra_field TEXT NOT NULL COLLATE NOCASE,"
      "meta_extra_value TEXT NOT NULL COLLATE NOCASE,"
      "PRIMARY KEY(id_meta, meta_extra_field));",

      "CREATE TABLE MetainfoContig("
      "id_contig TEXT PRIMARY KEY NOT NULL COLLATE NOCASE,"
      "contig_length INTEGER NOT NULL);"};

  for (const auto &query : create_queries) {
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
      throw runerror{"ERROR inserting data: " + string(sqlite3_errmsg(db)) +
                     "\n"};
    }
    sqlite3_finalize(stmt);
  }

  return 0;
}
