#include "BigQ.h"

BigQ::BigQ(Pipe &in, Pipe &out, OrderMaker &sortOrder, int runlen) {
    // Initialize worker thread data.
    WorkerThread *wt = new WorkerThread();
    wt->iPipe = &in;
    wt->oPipe = &out;
    wt->sortOrder = &sortOrder;
    wt->runLength = runlen;

    // Create the worker thread.
    pthread_create(&workerThread, NULL, TPMMS, (void *) wt);
}

BigQ::~BigQ() {
    //destructor
}

void *TPMMS(void *threadData) {
    WorkerThread *workerThread = (WorkerThread *) threadData;
    InitWorkerThread(workerThread);

    // Phase-1 (Generate sorted runs of runLength pages)
    RunGeneration(workerThread);

    // Phase-2 (Merge sorted runs)
    MergeRuns(workerThread);

    CleanUp(workerThread);
    return nullptr;
}

void InitWorkerThread(WorkerThread *threadData) {
    // Create buffer page array to store current runs' records
    Page *currPages = new(std::nothrow) Page[threadData->runLength];
    if (currPages == NULL) {
        cerr << "ERROR occured: Current Run Pages is null!!!\n";
        exit(1);
    }

    threadData->currPages = currPages;
    threadData->currPageNum = 0;
    threadData->numberOfRuns = 0;
    threadData->isOverflow = false;

    // Create temporary file for storing runs.
    sprintf(threadData->bigQFileName, "%d.bin", pthread_self());
    threadData->bigQFile.Open(0, threadData->bigQFileName);
}

void RunGeneration(WorkerThread *threadData) {
    Record *nextRecord = new Record();

    while (threadData->iPipe->Remove(nextRecord)) {
        if (!AddRecordToCurrentRun(threadData, nextRecord)) {
            CreateRun(threadData);
            AddRecordToCurrentRun(threadData, nextRecord);
        }
    }
    do {
        CreateRun(threadData);
    } while (threadData->isOverflow);
}

int AddRecordToCurrentRun(WorkerThread *threadData, Record *nextRecord) {
    Page *currentRunPage = &threadData->currPages[threadData->currPageNum];
    if (!currentRunPage->Append(nextRecord)) {
        if (threadData->currPageNum + 1 == threadData->runLength) {
            return 0;
        } else {
            currentRunPage = &threadData->currPages[++threadData->currPageNum];
        }
        currentRunPage->Append(nextRecord);
    }
    return 1;
}

void CreateRun(WorkerThread *workerThread) {
    SortAndStoreCurrentRun(workerThread);
    ClearCurrentRun(workerThread);
}

void SortAndStoreCurrentRun(WorkerThread *workerThread) {
    // Using priority queue for in memory sort.
    priority_queue<Record *, vector<Record *>, RecordComparator> pq(workerThread->sortOrder);
    LoadCurrentRunPriorityQueue(workerThread, pq);

    // Get the records from the priority queue and add sorted pages of records in the BigQFile.
    WritePriorityQueueContentToBigQFile(workerThread, pq);
}

// Put all the current run's record to the priority queue.
void LoadCurrentRunPriorityQueue(WorkerThread *workerThread,
                                 priority_queue<Record *, vector<Record *>, RecordComparator> &pq) {
    for (int i = 0; i <= workerThread->currPageNum; i++) {
        Page *currentRunPage = &workerThread->currPages[i];
        Record *temp = new Record();
        while (currentRunPage->GetFirst(temp)) {
            pq.push(temp);
            temp = new Record();
        }
        currentRunPage->EmptyItOut();
    }
}

void WritePriorityQueueContentToBigQFile(WorkerThread *workerThread,
                                         priority_queue<Record *, vector<Record *>, RecordComparator> &pq) {
    workerThread->isOverflow = false;
    Page *bufferPage = new Page();
    // Get next empty page offset of BigQFile.
    off_t currentPageIndexOfBigQFile =
            workerThread->bigQFile.GetLength() - 2 < 0 ? 0 : workerThread->bigQFile.GetLength() - 1;
    off_t maxRunPageNumber = currentPageIndexOfBigQFile + workerThread->runLength - 1;
    int currPageNumOfOverflowRecords = 0;

    while (!pq.empty()) {
        // Overflow condition
        if (currentPageIndexOfBigQFile > maxRunPageNumber) {
            if (!(&workerThread->currPages[currPageNumOfOverflowRecords])->Append(pq.top())) {
                currPageNumOfOverflowRecords++;
            } else {
                pq.pop();
            }
        }
            // If the current buffer is full.
        else if (!bufferPage->Append(pq.top())) {
            workerThread->bigQFile.AddPage(bufferPage, currentPageIndexOfBigQFile++);
            bufferPage->EmptyItOut();
        } else {
            pq.pop();
        }
    }

    // If data is not over flow, store the last page in the BigQFile.
    if (currentPageIndexOfBigQFile <= maxRunPageNumber) {
        workerThread->bigQFile.AddPage(bufferPage, currentPageIndexOfBigQFile);
    } else {
        workerThread->isOverflow = true;
    }
}

void ClearCurrentRun(WorkerThread *workerThread) {
    workerThread->numberOfRuns++;
    workerThread->currPageNum = 0;
}

void MergeRuns(struct WorkerThread *workerThread) {
    priority_queue<PriorityQueueStruct, vector<PriorityQueueStruct>, RecordComparator> pq(workerThread->sortOrder);

    LoadMergeRunPriorityQueue(workerThread, pq);

    LoadoPipeWithPriorityQueueData(workerThread, pq);
}

void LoadMergeRunPriorityQueue(WorkerThread *workerThread,
                               priority_queue<PriorityQueueStruct,
                                       vector<PriorityQueueStruct>,
                                       RecordComparator> &pq) {
    PriorityQueueStruct *priorityQueueStruct = new PriorityQueueStruct[workerThread->numberOfRuns];
    for (int i = 0; i < workerThread->numberOfRuns; i++) {
        priorityQueueStruct[i].currentPageNumber = i * workerThread->runLength;
        priorityQueueStruct[i].maxPageNumberOfCurrentRun =
                std::min((off_t) priorityQueueStruct[i].currentPageNumber + workerThread->runLength - 1,
                         workerThread->bigQFile.GetLength() - 2);

        priorityQueueStruct[i].page = new Page();
        workerThread->bigQFile.GetPage(priorityQueueStruct[i].page, priorityQueueStruct[i].currentPageNumber);
        priorityQueueStruct[i].head = new Record();
        priorityQueueStruct[i].page->GetFirst(priorityQueueStruct[i].head);
        pq.push(priorityQueueStruct[i]);
    }
}

void LoadoPipeWithPriorityQueueData(WorkerThread *workerThread,
                                         priority_queue<PriorityQueueStruct,
                                                 vector<PriorityQueueStruct>, RecordComparator> &pq) {
    while (!pq.empty()) {
        PriorityQueueStruct temp = pq.top();
        pq.pop();
        workerThread->oPipe->Insert(temp.head);
        if (!temp.page->GetFirst(temp.head)) {
            if (temp.currentPageNumber + 1 <= temp.maxPageNumberOfCurrentRun) {
                temp.currentPageNumber++;
                workerThread->bigQFile.GetPage(temp.page, temp.currentPageNumber);
                temp.page->GetFirst(temp.head);
                pq.push(temp);
            }
        } else {
            pq.push(temp);
        }
    }
}

void CleanUp(WorkerThread *workerThread) {
    // Close and delete the file.

    workerThread->bigQFile.Close();
    remove(workerThread->bigQFileName);

    delete[] workerThread->currPages;

    // Shut down output pipe.
    if (workerThread->oPipe) {
        workerThread->oPipe->ShutDown();
    }
}