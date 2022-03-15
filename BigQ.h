#ifndef BIGQ_H
#define BIGQ_H

#include "Comparison.h"
#include "ComparisonEngine.h"
#include "File.h"
#include "Pipe.h"
#include <vector>

class Run
{
private:
    File *tempFilePtr;
    Page *bufferPage;

    off_t currPageIndex, endPageIndex;

public:
    Record *currentRec;

    Run(File *bigQFile, off_t start, off_t end);

    ~Run();

    int Next(Record *current);
};

class BigQ
{
private:
    Pipe *input, *output;
    OrderMaker *sortOrder;
    int runLength;

    File *bigQFile;
    const char *tempFileName = "temp.bin";

    ComparisonEngine *comp;

    std::vector<off_t> runIndexes;

public:
    BigQ(Pipe &input, Pipe &output, OrderMaker &sortOrder, int runLength);
    ~BigQ();

    void ExecuteSort();

    void ExecuteMerge();
};

#endif // BIGQ_H