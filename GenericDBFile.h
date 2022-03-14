#ifndef GENERICDBFILE_H
#define GENERICDBFILE_H

typedef enum {
    heap,
    sorted,
    tree
} typeOfFile;

// Generic database type interface.
class GenericDBFile {
public:
    GenericDBFile() = default;

    virtual ~GenericDBFile() = default;

    virtual int Create(const char *filePath, typeOfFile type, void *startUp) = 0;

    virtual int Open(const char *filePath) = 0;

    virtual int Close() = 0;

    virtual void Load(Schema &schema, const char *loadPath) = 0;

    virtual void MoveFirst() = 0;

    virtual void Add(Record &addMe) = 0;

    virtual int GetNext (Record &fetchMe) = 0;

    virtual int GetNext (Record &fetchMe, CNF &cnf, Record &literal) = 0;
};

#endif //GENERICDBFILE_H