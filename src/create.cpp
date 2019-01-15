#include <vcflite/create.hpp>
#include <vcflite/query.hpp>

int VCFLite::Creator::init(sqlite3 *db) {
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
      "id_meta INTEGER PRIMARY KEY NOT NULL,"
      "meta_field TEXT NOT NULL COLLATE NOCASE,"
      "meta_description TEXT NOT NULL COLLATE NOCASE,"
      "meta_id TEXT DEFAULT NULL COLLATE NOCASE,"
      "meta_type TEXT DEFAULT NULL COLLATE NOCASE,"
      "meta_number TEXT DEFAULT NULL COLLATE NOCASE);",

      "CREATE INDEX idxMetaInfo_id ON MetaInfo(meta_id);",

      "CREATE TABLE MetaInfoExtra("
      "id_meta INTEGER NOT NULL,"
      "meta_extra_field TEXT NOT NULL COLLATE NOCASE,"
      "meta_extra_value TEXT NOT NULL COLLATE NOCASE,"
      ""
      "PRIMARY KEY(id_meta, meta_extra_field),"
      ""
      "FOREIGN KEY(id_meta) REFERENCES MetaInfo(id_meta)"
      ");",

      "CREATE TABLE MetaInfoContigs("
      "id_contig TEXT PRIMARY KEY NOT NULL COLLATE NOCASE,"
      "contig_length INTEGER NOT NULL);"};

  exec(db, create_queries.begin(), create_queries.end());

  return 0;
}
