#include <vcflite/create.hpp>
#include <vcflite/query.hpp>

int VCFLite::Creator::init(sqlite3 *db) {
  transaction(db);

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

  exec(db, drop_queries.begin(), drop_queries.end());

  vector<string> create_queries{
      "CREATE TABLE MetaInfo("
      "meta_field TEXT NOT NULL COLLATE NOCASE,"
      "meta_id TEXT DEFAULT NULL COLLATE NOCASE,"
      "meta_description TEXT DEFAULT NULL COLLATE NOCASE,"
      "meta_type TEXT DEFAULT NULL COLLATE NOCASE,"
      "meta_number TEXT DEFAULT NULL COLLATE NOCASE,"
      "meta_source TEXT DEFAULT NULL COLLATE NOCASE,"
      "meta_version TEXT DEFAULT NULL COLLATE NOCASE,"
      ""
      "PRIMARY KEY(meta_field, meta_id)"
      ");",

      "CREATE INDEX idxMetaInfo_id ON MetaInfo(meta_id);",

      "CREATE TABLE MetaInfoExtra("
      "meta_field TEXT NOT NULL COLLATE NOCASE,"
      "meta_id TEXT DEFAULT NULL COLLATE NOCASE,"
      "meta_extra_key TEXT NOT NULL COLLATE NOCASE,"
      "meta_extra_value TEXT NOT NULL COLLATE NOCASE,"
      ""
      "PRIMARY KEY(meta_extra_key, meta_field, meta_id),"
      ""
      "FOREIGN KEY(meta_field, meta_id) "
      "REFERENCES MetaInfo(meta_field, meta_id)"
      ""
      ");",

      "CREATE INDEX idxMetaInfoExtra_id ON MetaInfo(meta_field, meta_id);",

      //      "CREATE TABLE MetaInfoContigs("
      //      "id_contig TEXT PRIMARY KEY NOT NULL COLLATE NOCASE,"
      //      "contig_length INTEGER NOT NULL"
      //      ");"
  };

  exec(db, create_queries.begin(), create_queries.end());

  commit(db);

  return 0;
}
