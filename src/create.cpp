#include <iostream>

#include <vcflite/create.hpp>
#include <vcflite/query.hpp>

int VCFLite::Creator::init(sqlite3 *db) {
  std::clog << "[LOG] Initializing new database\n";

  transaction(db);

  std::clog << "[LOG] Dropping old tables\n";

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
      "DROP TABLE IF EXISTS GenotypesAlleles;",
      "DROP TABLE IF EXISTS GenotypesPhase;",
  };

  exec(db, drop_queries.begin(), drop_queries.end());

  std::clog << "[LOG] Completed\n";

  std::clog << "[LOG] Creating new tables and indexes\n";

  vector<string> create_queries{
      "CREATE TABLE MetaInfo("
      "meta_file TEXT NOT NULL COLLATE NOCASE,"
      "meta_field TEXT NOT NULL COLLATE NOCASE,"
      "meta_id TEXT DEFAULT NULL COLLATE NOCASE,"
      "meta_description TEXT DEFAULT NULL COLLATE NOCASE,"
      "meta_type TEXT DEFAULT NULL COLLATE NOCASE,"
      "meta_number TEXT DEFAULT NULL COLLATE NOCASE,"
      "meta_source TEXT DEFAULT NULL COLLATE NOCASE,"
      "meta_version TEXT DEFAULT NULL COLLATE NOCASE,"
      ""
      "PRIMARY KEY(meta_file, meta_field, meta_id)"
      ");",

      "CREATE TABLE MetaInfoExtra("
      "meta_file TEXT NOT NULL COLLATE NOCASE,"
      "meta_field TEXT NOT NULL COLLATE NOCASE,"
      "meta_id TEXT DEFAULT NULL COLLATE NOCASE,"
      "meta_extra_key TEXT NOT NULL COLLATE NOCASE,"
      "meta_extra_value TEXT NOT NULL COLLATE NOCASE,"
      ""
      "PRIMARY KEY(meta_extra_key, meta_file, meta_field, meta_id),"
      ""
      "FOREIGN KEY(meta_file, meta_field, meta_id) "
      "REFERENCES MetaInfo(meta_file, meta_field, meta_id)"
      ""
      ");",

      "CREATE TABLE Variants ("
      "id_variant INTEGER PRIMARY KEY NOT NULL,"
      "variant_file TEXT NOT NULL COLLATE NOCASE,"
      "variant_chrom TEXT NOT NULL COLLATE NOCASE,"
      "variant_start INTEGER NOT NULL,"
      "variant_end INTEGER NOT NULL,"
      "variant_length INTEGER NOT NULL,"
      "variant_ref TEXT NOT NULL COLLATE NOCASE,"
      "variant_qual REAL DEFAULT NULL,"
      "variant_pass INTEGER DEFAULT NULL,"
      "variant_alleles INTEGER DEFAULT NULL"
      ");",

      "CREATE INDEX idxVariantsPos "
      "ON Variants(variant_chrom, variant_start, variant_end);",
      "CREATE INDEX idxVariantsPass "
      "ON Variants(variant_pass);",
      "CREATE INDEX idxVariantsAlleles "
      "ON Variants(variant_alleles);",

      "CREATE TABLE VariantsIDs ("
      "id_variant INTEGER NOT NULL,"
      "variant_idx TEXT NOT NULL COLLATE NOCASE,"
      ""
      "PRIMARY KEY(variant_idx, id_variant),"
      ""
      "FOREIGN KEY(id_variant) REFERENCES Variants(id_variant)"
      ");",

      "CREATE TABLE VariantsFilters ("
      "id_variant INTEGER NOT NULL,"
      "variant_filter TEXT NOT NULL COLLATE NOCASE,"
      "PRIMARY KEY(variant_filter, id_variant),"
      ""
      "FOREIGN KEY(id_variant) REFERENCES Variants(id_variant)"
      ");",

      "CREATE TABLE VariantsAlleles ("
      "id_variant INTEGER NOT NULL,"
      "variant_allele_id INTEGER NOT NULL,"
      "variant_allele_seq TEXT NOT NULL COLLATE NOCASE,"
      ""
      "PRIMARY KEY(id_variant, variant_allele_id),"
      ""
      "FOREIGN KEY(id_variant) REFERENCES Variants(id_variant)"
      ");",

      "CREATE TABLE VariantsInfo ("
      "id_variant INTEGER NOT NULL,"
      "variant_key TEXT NOT NULL COLLATE NOCASE,"
      "variant_value TEXT DEFAULT NULL COLLATE NOCASE,"
      ""
      "PRIMARY KEY(variant_key, id_variant),"
      ""
      "FOREIGN KEY(id_variant) REFERENCES Variants(id_variant)"
      ");",

      "CREATE INDEX idxVariantsInfo"
      " ON VariantsInfo(variant_key, variant_value);",

      "CREATE TABLE Genotypes ("
      "id_variant INTEGER NOT NULL,"
      "genotype_sample TEXT NOT NULL COLLATE NOCASE,"
      "genotype_gt TEXT DEFAULT NULL COLLATE NOCASE,"
      "genotype_dp INTEGER DEFAULT NULL COLLATE NOCASE,"
      ""
      "PRIMARY KEY(genotype_sample, id_variant),"
      ""
      "FOREIGN KEY(id_variant) REFERENCES Variants(id_variant)"
      ");",

      "CREATE INDEX idxGenotypesDP ON Genotypes(genotype_dp);",
      "CREATE INDEX idxGenotypesID ON Genotypes(id_variant);",

      "CREATE TABLE GenotypesAlleles ("
      "id_variant INTEGER NOT NULL,"
      "genotype_sample TEXT NOT NULL COLLATE NOCASE,"
      "genotype_position INTEGER NOT NULL COLLATE NOCASE,"
      "variant_allele_id INTEGER NOT NULL COLLATE NOCASE,"
      ""
      "PRIMARY KEY(id_variant, genotype_sample, genotype_position), "
      ""
      "FOREIGN KEY(id_variant, genotype_sample) "
      "REFERENCES Genotypes(id_variant, genotype_sample),"
      "FOREIGN KEY(id_variant, variant_allele_id) "
      "REFERENCES VariantsAlleles(id_variant, variant_allele_id)"
      ""
      ");",

      "CREATE TABLE GenotypesPhase ("
      "id_variant INTEGER NOT NULL,"
      "genotype_sample TEXT NOT NULL COLLATE NOCASE,"
      "genotype_phased_id TEXT DEFAULT NULL COLLATE NOCASE,"
      "genotype_phased_gt TEXT DEFAULT NULL COLLATE NOCASE,"
      ""
      "PRIMARY KEY(id_variant, genotype_sample, genotype_phased_id), "
      ""
      "FOREIGN KEY(id_variant, genotype_sample) "
      "REFERENCES Genotypes(id_variant, genotype_sample)"
      ""
      ");",

      "CREATE TABLE GenotypesInfo ("
      "id_variant INTEGER NOT NULL,"
      "genotype_sample TEXT NOT NULL COLLATE NOCASE,"
      "genotype_key TEXT NOT NULL COLLATE NOCASE,"
      "genotype_value TEXT DEFAULT NULL COLLATE NOCASE,"
      ""
      "PRIMARY KEY(id_variant, genotype_key, genotype_sample), "
      ""
      "FOREIGN KEY(id_variant, genotype_sample) "
      "REFERENCES Genotypes(id_variant, genotype_sample)"
      ");",

      "CREATE INDEX idxGenotypesInfo"
      " ON GenotypesInfo(genotype_key, genotype_value);",

  };

  exec(db, create_queries.begin(), create_queries.end());

  std::clog << "[LOG] Completed\n";

  commit(db);

  std::clog << "[LOG] Commiting changes\n";

  return 0;
}
