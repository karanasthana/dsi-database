#ifndef DBFILE_H
#define DBFILE_H

#include "Comparison.h"
#include "GenericDBFile.h"
#include "Record.h"
#include "Schema.h"

class DBFile {
private:
    GenericDBFile *file;

public:
    DBFile();

    ~DBFile();

    int Create(const char *filePath, typeOfFile type, void *startUp);

    int Open(const char *filePath);

    int Close();

    void Load(Schema &schema, const char *loadPath);

    void MoveFirst();

    void Add(Record &addMe);

    int GetNext (Record &fetchMe);

    int GetNext (Record &fetchMe, CNF &cnf, Record &literal);
};

#endif //DBFILE_H