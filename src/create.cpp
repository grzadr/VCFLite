#include <vcflite/create.hpp>
#include <vcflite/query.hpp>

int VCFLite::Creator::init(sqlite3 *db) {
  transaction(db);

  vector<string> drop_queries{
      "DROP TABLE IF EXISTS MetaInfo;",
      "DROP TABLE IF EXISTS MetaInfoContigs;",
      "DROP TABLE IF EXISTS MetaInfoExtra;",

      "DROP TABLE IF EXISTS Variants;",
      "DROP TABLE IF EXISTS VariantsIDs;",
      "DROP TABLE IF EXISTS VariantsFilters;",
      "DROP TABLE IF EXISTS VariantsAlleles;",
      "DROP TABLE IF EXISTS VariantsInfo;",

      "DROP TABLE IF EXISTS Samples;",
      "DROP TABLE IF EXISTS SamplesInfo;",

      "DROP TABLE IF EXISTS Genotypes;",
      "DROP TABLE IF EXISTS GenotypesInfo;",
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

      "CREATE TABLE Variants ("
      "var_chrom TEXT NOT NULL COLLATE NOCASE,"
      "var_start INTEGER NOT NULL,"
      "var_end INTEGER NOT NULL,"
      "var_length INTEGER NOT NULL,"
      "var_ref TEXT NOT NULL COLLATE NOCASE,"
      "var_qual REAL DEFAULT NULL,"
      "var_pass INTEGER DEFAULT NULL,"
      "var_alleles INTEGER DEFAULT NULL,"
      "PRIMARY KEY(var_chrom, var_start, var_end)"
      ");",

      "CREATE TABLE VariantsIDs ("
      "var_chrom TEXT NOT NULL COLLATE NOCASE,"
      "var_start INTEGER NOT NULL,"
      "var_end INTEGER NOT NULL,"
      "var_id TEXT NOT NULL COLLATE NOCASE,"
      "PRIMARY KEY(var_chrom, var_start, var_end, var_id),"
      ""
      "FOREIGN KEY(var_chrom, var_start, var_end) "
      "REFERENCES Variants(var_chrom, var_start, var_end)"
      ""
      ");",

      "CREATE TABLE VariantsFilters ("
      "var_chrom TEXT NOT NULL COLLATE NOCASE,"
      "var_start INTEGER NOT NULL,"
      "var_end INTEGER NOT NULL,"
      "var_filter TEXT NOT NULL COLLATE NOCASE,"
      "PRIMARY KEY(var_filter, var_chrom, var_start, var_end),"
      ""
      "FOREIGN KEY(var_chrom, var_start, var_end) "
      "REFERENCES Variants(var_chrom, var_start, var_end)"
      ""
      ");",

      "CREATE TABLE VariantsAlleles ("
      "var_chrom TEXT NOT NULL COLLATE NOCASE,"
      "var_start INTEGER NOT NULL,"
      "var_end INTEGER NOT NULL,"
      "var_allel_id INTEGER NOT NULL,"
      "var_allel_seq TEXT NOT NULL COLLATE NOCASE,"
      "PRIMARY KEY(var_chrom, var_start, var_end, var_allel_seq),"
      ""
      "FOREIGN KEY(var_chrom, var_start, var_end) "
      "REFERENCES Variants(var_chrom, var_start, var_end)"
      ""
      ");",

      "CREATE UNIQUE INDEX idxVariantsAlleles"
      " ON VariantsAlleles(var_chrom, var_start, var_end, var_allel_seq);",

      "CREATE TABLE VariantsInfo ("
      "var_chrom TEXT NOT NULL COLLATE NOCASE,"
      "var_start INTEGER NOT NULL,"
      "var_end INTEGER NOT NULL,"
      "var_key TEXT NOT NULL COLLATE NOCASE,"
      "var_value TEXT DEFAULT NULL COLLATE NOCASE,"
      "PRIMARY KEY(var_key, var_chrom, var_start, var_end),"
      ""
      "FOREIGN KEY(var_chrom, var_start, var_end) "
      "REFERENCES Variants(var_chrom, var_start, var_end)"
      ""
      ");",

      "CREATE UNIQUE INDEX idxVariantsInfo"
      " ON VariantsInfo(var_chrom, var_start, var_end, var_key);",

      "CREATE TABLE Genotypes ("
      "var_chrom TEXT NOT NULL COLLATE NOCASE,"
      "var_start INTEGER NOT NULL,"
      "var_end INTEGER NOT NULL,"
      "genotype_sample TEXT NOT NULL COLLATE NOCASE,"
      ""
      "PRIMARY KEY(genotype_sample, var_chrom, var_start, var_end), "
      ""
      "FOREIGN KEY(var_chrom, var_start, var_end) "
      "REFERENCES Variants(var_chrom, var_start, var_end)"
      ");",

      "CREATE UNIQUE INDEX idxGenotypes"
      " ON Genotypes(var_chrom, var_start, var_end, genotype_sample);",

      "CREATE TABLE GenotypesInfo ("
      "var_chrom TEXT NOT NULL COLLATE NOCASE,"
      "var_start INTEGER NOT NULL,"
      "var_end INTEGER NOT NULL,"
      "genotype_sample TEXT NOT NULL COLLATE NOCASE,"
      "genotype_key TEXT NOT NULL COLLATE NOCASE,"
      "genotype_value TEXT DEFAULT NULL COLLATE NOCASE,"
      ""
      "PRIMARY KEY(genotype_key, genotype_sample, var_chrom, var_start, "
      "var_end), "
      ""
      "FOREIGN KEY(genotype_sample, var_chrom, var_start, var_end) "
      "REFERENCES Genotypes(genotype_sample, var_chrom, var_start, var_end)"
      ");",

      "CREATE UNIQUE INDEX idxGenotypesInfo"
      " ON GenotypesInfo(var_chrom, var_start, var_end, genotype_sample, "
      "genotype_key);",

  };

  exec(db, create_queries.begin(), create_queries.end());

  commit(db);

  return 0;
}
