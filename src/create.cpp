#include <iostream>

#include <vcflite/create.hpp>
#include <vcflite/query.hpp>

int VCFLite::Creator::init(sqlite3 *db) {
  std::clog << "[LOG] Initializing new database\n";

  transaction(db);

  std::clog << "[LOG] Dropping old tables\n";

  vector<string> drop_queries{
      "DROP TABLE IF EXISTS MetaInfoExtra;",
      "DROP TABLE IF EXISTS MetaInfo;",

      "DROP TABLE IF EXISTS GenotypesPhase;",
      "DROP TABLE IF EXISTS GenotypesInfo;",
      "DROP TABLE IF EXISTS GenotypesAlleles;",
      "DROP TABLE IF EXISTS Genotypes;",

      "DROP TABLE IF EXISTS VariantsIDs;",
      "DROP TABLE IF EXISTS VariantsFilters;",
      "DROP TABLE IF EXISTS VariantsAlleles;",
      "DROP TABLE IF EXISTS VariantsInfo;",
      "DROP TABLE IF EXISTS Variants;",
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
      "variant_filters INTEGER DEFAULT NULL,"
      "variant_alleles INTEGER DEFAULT NULL,"
      "variant_idxs INTEGER NOT NULL"
      ");",

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
      "PRIMARY KEY(id_variant, variant_key),"
      ""
      "FOREIGN KEY(id_variant) REFERENCES Variants(id_variant)"
      ");",

      "CREATE TABLE Genotypes ("
      "id_variant INTEGER NOT NULL,"
      "genotype_sample TEXT NOT NULL COLLATE NOCASE,"
      "genotype_gt TEXT DEFAULT NULL COLLATE NOCASE,"
      "genotype_gt_decoded TEXT DEFAULT NULL COLLATE NOCASE,"
      "genotype_dp INTEGER DEFAULT NULL COLLATE NOCASE,"
      "genotype_phased BOOLEAN NOT NULL,"
      ""
      "PRIMARY KEY(id_variant, genotype_sample),"
      ""
      "FOREIGN KEY(id_variant) REFERENCES Variants(id_variant)"
      ");",

      "CREATE TABLE GenotypesAlleles ("
      "id_variant INTEGER NOT NULL,"
      "genotype_sample TEXT NOT NULL COLLATE NOCASE,"
      "genotype_position INTEGER DEFAULT NULL,"
      "genotype_sequence TEXT DEFAULT NULL COLLATE NOCASE,"
      "variant_allele_id INTEGER DEFAULT NULL,"
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

  };

  exec(db, create_queries.begin(), create_queries.end());

  std::clog << "[LOG] Completed\n"
            << "[LOG] Commiting changes";

  commit(db);

  std::clog << "[LOG] Committed!\n";

  return 0;
}

int VCFLite::Creator::index(sqlite3 *db) {
  std::clog << "[LOG] Indexing database\n";

  transaction(db);

  std::clog << "[LOG] Dropping old indexes\n";

  vector<string> drop_queries{
      "DROP INDEX IF EXISTS idxVariantsPos;",
      "DROP INDEX IF EXISTS idxVariantsFilt;",
      "DROP INDEX IF EXISTS idxVariantsAlleles;",
      "DROP INDEX IF EXISTS idxVariantsIdxs;",
      "DROP INDEX IF EXISTS idxVariantsIDsID",
      "DROP INDEX IF EXISTS idxVariantsFiltersID",
      "DROP INDEX IF EXISTS idxVariantsInfo",
      "DROP INDEX IF EXISTS idxGenotypesDP",
      "DROP INDEX IF EXISTS idxGenotypesID",
      "DROP INDEX IF EXISTS idxGenotypesAllelesID",
      "DROP INDEX IF EXISTS idxGenotypesInfo",
  };

  exec(db, drop_queries.begin(), drop_queries.end());

  std::clog << "[LOG] Completed\n";

  std::clog << "[LOG] Creating new indexes\n";

  vector<string> create_queries{
      "CREATE INDEX idxVariantsPos"
      " ON Variants(variant_chrom, variant_start, variant_end);",
      "CREATE INDEX idxVariantsFilt ON Variants(variant_filters);",
      "CREATE INDEX idxVariantsAlleles ON Variants(variant_alleles);",
      "CREATE INDEX idxVariantsIdxs ON Variants(variant_idxs);",

      "CREATE INDEX idxVariantsIDsID ON VariantsIDs(id_variant);",

      "CREATE INDEX idxVariantsFiltersID ON VariantsFilters(id_variant);",

      "CREATE INDEX idxVariantsInfo"
      " ON VariantsInfo(variant_key, variant_value);",

      "CREATE INDEX idxGenotypesDP ON Genotypes(genotype_dp);",
      "CREATE INDEX idxGenotypesSample ON Genotypes(genotype_sample);",

      "CREATE INDEX idxGenotypesAllelesID "
      "ON GenotypesAlleles(id_variant, variant_allele_id);",

      "CREATE INDEX idxGenotypesInfo"
      " ON GenotypesInfo(genotype_key, genotype_value);",
  };

  exec(db, create_queries.begin(), create_queries.end());

  std::clog << "[LOG] Index creation completed\n";

  commit(db);

  std::clog << "[LOG] Commiting changes\n";

  return 0;
}
