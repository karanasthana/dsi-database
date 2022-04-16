//
// Created by Karan & Prateek.
//
#include "DBFile.h"

#define N 3

char *dbfile_dir = "/cise/homes/karanasthana/git/data/tpch-dbgen/Project1/P1";
char *tpch_dir ="/cise/homes/karanasthana/git/tpch-dbgen";

char* values[N] = {"partsupp", "part", "supplier"};

int main(){
    char* name;
    for(int i=0; i<N; i++){
        name = values[i];
        char name_tbl[100];
        sprintf(name_tbl, "%s%s.tbl", tpch_dir, name);
        char name_bin[100];
        sprintf(name_bin, "%s.bin", name);
        Schema schema (catalog_path, name);

        DBFile dbfile;
        dbfile.Create(name_bin, heap, NULL);
        dbfile.Load(schema, name_tbl);
        dbfile.Close();
    }
}