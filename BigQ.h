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
    Pipe *iPipe;
    Pipe *oPipe;
    OrderMaker *sortOrder;
    int runLength;

    File bigQFile;
    char bigQFileName[100];
    int numberOfRuns;

    Page *currPages;
    int currPageNum;

    bool isOverflow;
};

struct PriorityQueueStruct {
    Page *page;
    Record *head;

    int pqCurrPageNum;
    int pqMaxCurrRunPageNum;
};

struct RecordComparator {
    OrderMaker *sortOrder;

    RecordComparator(OrderMaker *sortorder) {
        this->sortOrder = sortorder;
    }

    bool operator()(Record *left, Record *right) {
        ComparisonEngine cmp;
        return cmp.Compare(left, right, this->sortOrder) > 0;
    }

    bool operator()(const PriorityQueueStruct &left, const PriorityQueueStruct &right) {
        ComparisonEngine cmp;
        return cmp.Compare(left.head, right.head, this->sortOrder) > 0;
    }
};


void *TPMMS(void *tData);

void InitWorkerThread(WorkerThread *tData);

void GenerateRun(WorkerThread *tData);

int AddRecToCurrRun(WorkerThread *tData, Record *nextRecord);

void CreateRun(WorkerThread *workerThread);

void CustomSortAndWrite(WorkerThread *workerThread);

void PutInPQ(WorkerThread *workerThread,priority_queue<Record *, vector<Record *>, RecordComparator> &pq);

void WriteFromPQ(WorkerThread *workerThread,priority_queue<Record *, vector<Record *>, RecordComparator> &pq);

void SetupNewRun(WorkerThread *workerThread);

void MergeRuns(WorkerThread *workerThread);

void LoadMergeRunPriorityQueue(WorkerThread *workerThread,
                               priority_queue<PriorityQueueStruct, vector<PriorityQueueStruct>, RecordComparator> &pq);

void LoadoPipeWithPriorityQueueData(WorkerThread *workerThread,
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