#ifndef BIGQ_H
#define BIGQ_H

#include "Comparison.h"
#include "ComparisonEngine.h"
#include "File.h"
#include "Pipe.h"
#include <vector>

class Run {
private:
    File *tempFilePtr;
    Page *bufferPage;

    // Pointer to current and end page for this run.
    off_t currPageIndex, endPageIndex;

public:
    Record *currentRec;

    Run(File *tempFile, off_t start, off_t end);

    ~Run();

    /*
     * Updates the current parameter value to point to the current record in bufferPage and moves
     * currentRec variable to point to the next record.
     *
     * Returns 1 for success and 0 for failure.
     */
    int Next(Record *current);
};

class BigQ {
private:
    Pipe *input, *output;
    OrderMaker *sortOrder;
    int runLength;

    File *tempFile;
    const char *tempFileName = "temp.bin";

    ComparisonEngine *comp;

    std::vector<off_t> runIndexes;

public:
    BigQ(Pipe &input, Pipe &output, OrderMaker &sortOrder, int runLength);
    ~BigQ();

    void ExecuteSortPhase();

    void ExecuteMergePhase();
};

#endif //BIGQ_H