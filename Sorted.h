#ifndef SORTED_H
#define SORTED_H

#include "BigQ.h"
#include "GenericDBFile.h"
#include "File.h"

class Sorted : public GenericDBFile
{
private:
    enum Mode
    {
        READ,
        WRITE
    } mode;

    File *file;
    string filePath, tempFileLoc;

    BigQ *bigQ;

    Pipe *iPipe, *oPipe;

    Page *bufferedPage;
    off_t currPagePtr;

    ComparisonEngine *comp;

    OrderMaker *sortOrder;
    int runLength;

    OrderMaker *queryOrder, *cnfOrder;

    void Init();

    void Read();
    void Write();

    void Merge();

    void buildQuery(CNF &cnf);

    off_t performBinarySearch(off_t low, off_t high, Record &literal);

    int binarySearchNext(Record &fetchMe, CNF &cnf, Record &literal, bool isCnfUpdated);

    int linearSearchNext(Record &fetchMe, CNF &cnf, Record &literal);

public:
    Sorted();
    Sorted(OrderMaker *orderMaker, int runLength);

    ~Sorted() override;

    int Create(const char *fp, typeOfFile type, void *startUp) override;

    int Open(const char *fp) override;

    int Close() override;

    void Load(Schema &schema, const char *loadPath) override;

    void MoveFirst() override;

    void Add(Record &addMe) override;

    int GetNext(Record &fetchMe) override;

    int GetNext(Record &fetchMe, CNF &cnf, Record &literal) override;
};

#endif // SORTED_H