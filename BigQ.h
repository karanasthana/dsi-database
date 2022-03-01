#ifndef BIGQ_H
#define BIGQ_H

#include <pthread.h>
#include <iostream>
#include <queue>
#include "Pipe.h"
#include "File.h"
#include "Record.h"

using namespace std;

struct WorkerThread {
    Pipe *inputPipe;
    Pipe *outputPipe;
    OrderMaker *sortOrder;
    int runLength;

    File bigQFile;
    char bigQFileName[100];
    int numberOfRuns;

    Page *currentRunPages;
    int currentRunPageNumber;

    bool isOverflow;
};

struct PriorityQueueStruct {
    Page *page;
    Record *head;

    int currentPageNumber;
    int maxPageNumberOfCurrentRun;
};

struct RecordComparator {
    OrderMaker *sortOrder;

    RecordComparator(OrderMaker *sortorder) {
        this->sortOrder = sortorder;
    }

    bool operator()(Record *lhs, Record *rhs) {
        ComparisonEngine cmp;
        return cmp.Compare(lhs, rhs, this->sortOrder) > 0;
    }

    bool operator()(const PriorityQueueStruct &lhs, const PriorityQueueStruct &rhs) {
        ComparisonEngine cmp;
        return cmp.Compare(lhs.head, rhs.head, this->sortOrder) > 0;
    }
};


void *TPMMS(void *threadData);

void InitWorkerThread(WorkerThread *threadData);

void RunGeneration(WorkerThread *threadData);

int AddRecordToCurrentRun(WorkerThread *threadData, Record *nextRecord);

void CreateRun(WorkerThread *workerThread);

void SortAndStoreCurrentRun(WorkerThread *workerThread);

void LoadCurrentRunPriorityQueue(WorkerThread *workerThread,priority_queue<Record *, vector<Record *>, RecordComparator> &pq);

void WritePriorityQueueContentToBigQFile(WorkerThread *workerThread,priority_queue<Record *, vector<Record *>, RecordComparator> &pq);

void ClearCurrentRun(WorkerThread *workerThread);

void MergeRuns(WorkerThread *workerThread);

void LoadMergeRunPriorityQueue(WorkerThread *workerThread,
                               priority_queue<PriorityQueueStruct, vector<PriorityQueueStruct>, RecordComparator> &pq);

void LoadOutputPipeWithPriorityQueueData(WorkerThread *workerThread,
                                         priority_queue<PriorityQueueStruct, vector<PriorityQueueStruct>, RecordComparator> &pq);

void CleanUp(WorkerThread *workerThread);

class BigQ {
private:
    pthread_t workerThread;

public:
    BigQ(Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen);

    ~BigQ();
};

#endif BIGQ_H