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
    cout << "Constructor" << endl;

    f = new File();
    read = new Page();
    write = new Page();
    rec = new Record();
}

int DBFile::Create (const char *f_path, fType f_type, void *startup) {
    cout << "create" << endl;

    if (f_path == NULL || f_path[0] == '\0' || f_type != heap) {
        return 0;
    }

    f->Open(0, f_path);

    return 1;
}

void DBFile::Load (Schema &f_schema, const char *loadpath) {
    cout << "load" << endl;


}

int DBFile::Open (const char *f_path) {
    cout << "open" << endl;
    if (f_path == NULL || f_path[0] == '\0') {
        return 0;
    }

    f->Open(1, f_path);

    return 1;
}

void DBFile::MoveFirst () {
    cout << "move first" << endl;

}

int DBFile::Close () {
    cout << "close" << endl;
    f->Close();
    return 1;
}

void DBFile::Add (Record &rec) {
    cout << "add" << endl;
}

int DBFile::GetNext (Record &fetchme) {
    cout << "get next" << endl;
    return 1;
}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
    cout << "get next 2" << endl;
    return 1;
}
