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
    comp = new ComparisonEngine();
}

DBFile::~DBFile() {
    delete(f);
    delete(read);
    delete(write);
    delete(rec);
    delete(comp);
}

int DBFile::Create (const char *f_path, fType f_type, void *startup) {
    cout << "create" << endl;

    if (f_path == NULL || f_path[0] == '\0' || f_type != heap) {
        return 0;
    }

    cout << "error checking in create done " << endl;

    f->Open(0, f_path);
    
    hasFileEnded = true;
    hasRecordsLeft = false;
    // write_index = 1;
    wIndex = 1;
    rIndex = 1;

    cout << "variable initialization done in create " << endl;

    cout << "windex - 51 " << wIndex << endl;

    return 1;
}

void DBFile::Load (Schema &f_schema, const char *loadpath) {
    cout << "load" << endl;
    cout << "write index  - 56 " << wIndex << endl;

    FILE *tableFile = NULL;
    tableFile = fopen(loadpath, "r");
    if(tableFile == NULL) {
        cerr << "error trying to open file: " << loadpath << endl;
    }

    cout << "write index  - 64 " << wIndex << endl;
    Record *temp = new Record();

    while (temp->SuckNextRecord (&f_schema, tableFile) == 1) {
        // rec.Print (&f_schema);
        Add(*(temp));
    }
    cout << "write index  - 77 " << wIndex << endl;
    delete temp;

    cout << "write index  - 80 " << wIndex << endl;

    fclose(tableFile);

    cout << "write index  - 84 " << wIndex << endl;
}

int DBFile::Open (const char *f_path) {
    cout << "open" << endl;
    if (f_path == NULL || f_path[0] == '\0') {
        return 0;
    }

    f->Open(1, f_path);

    hasFileEnded = false;
    rIndex = 1;

    return 1;
}

void DBFile::MoveFirst () {
    cout << "move first" << endl;

    // check if this has to be 0 or 1.
    f->GetPage(read, 1);
    read->GetFirst(rec);

    return;
}

int DBFile::Close () {
    cout << "close" << endl;

    if (hasRecordsLeft) {
        cout << "Adding Page now from close with wIndex as " << wIndex << endl;
        WriteToFile();
    }

    hasFileEnded = true;

    f->Close();
    return 1;
}

void DBFile::Add (Record &rec) {
    cout << "add" << endl;

    hasRecordsLeft = true;
    
    if(write->getCurrentPageSize() + rec.getRecordSize() > PAGE_SIZE) {
        cout << "Adding Page now " << endl;
        WriteToFile();
    }

    int appendResult = write->Append(&rec);
    cout << "appending to write file had the result --> " << appendResult << endl;
    if (appendResult == 0) {
        cerr << "DBFile::AppendToPage - Error appending new record to the page." << endl;
    }
    return;
}

int DBFile::GetNext (Record &fetchme) {
    cout << "get next" << endl;

    if(!hasFileEnded) {
        fetchme.Copy(rec);
        if (!read->GetFirst(rec)) {
            if (++rIndex < f->GetLength () - 1) {
                f->GetPage(read, rIndex);
                read->GetFirst(rec);
            } else {
                hasFileEnded = true;
            }
        }
        return 1;
    }
    return 0;
}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
    cout << "get next 2" << endl;

    if(hasFileEnded) {
        return 0;
    }
    
    // getNextRecordThatSatisfies
    while (1) {
        int getNextResult = GetNext(fetchme);
        if (!getNextResult) {
            return 0;
        }
        int result = comp->Compare (&fetchme, &literal, &cnf);
        if (result == 1) {
            return 1;
        }
        cout << "Checking for condition" << endl;
    }
}

void DBFile::WriteToFile() {
    f->AddPage(write, wIndex++);
    cout << "Add Page complete!";
    write->EmptyItOut();
}
