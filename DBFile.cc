#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"
#include "Defs.h"
#include <string.h>
#include <iostream>

// stub file .. replace it with your own DBFile.cc

DBFile::DBFile () {
    cout << "Constructor";
}

int DBFile::Create (const char *f_path, fType f_type, void *startup) {
    cout << "create";
    return 1;
}

void DBFile::Load (Schema &f_schema, const char *loadpath) {
    cout << "load";
}

int DBFile::Open (const char *f_path) {
    cout << "open";
    return 1;
}

void DBFile::MoveFirst () {
    cout << "move first";
}

int DBFile::Close () {
    cout << "close";
    return 1;
}

void DBFile::Add (Record &rec) {
    cout << "add";
}

int DBFile::GetNext (Record &fetchme) {
    cout << "get next";
    return 1;
}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
    cout << "get next 2";
    return 1;
}
