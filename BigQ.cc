#include "BigQ.h"

BigQ::BigQ(Pipe &in, Pipe &out, OrderMaker &sortOrder, int runlen) {
    // Initialize worker thread object with default values.
    WorkerThread *wt = new WorkerThread();
    wt->iPipe = &in;
    wt->oPipe = &out;
    wt->sortOrder = &sortOrder;
    wt->runLength = runlen;

    // Creating pthread with the worker thread.
    pthread_create(&workerThread, NULL, TPMMS, (void *) wt);
}

BigQ::~BigQ() {
    //destructor
}

//Implementation of Two Pass Multiway Merge Sort Algorithm
void *TPMMS(void *tData) {
    WorkerThread *worker = (WorkerThread *) tData;
    InitWorkerThread(worker);

    // Generating sorted runs of runLength pages
    GenerateRun(worker);

    //Merging sorted runs
    MergeRuns(worker);

    //Cleaning the worker object
    CleanUp(worker);

    //Note: return nullptr else segmentation fault (IMP)
    return nullptr;
}

//Initializing the worker thread with relevant data
void InitWorkerThread(WorkerThread *tData) {
    // Create buffer page array to store current runs' records
    Page *currPages = new(std::nothrow) Page[tData->runLength];
    if (currPages == NULL) {
        cerr << "ERROR occured: Current Run Pages is null!!!\n";
        exit(1);
    }

    tData->currPages = currPages;
    tData->currPageNum = 0;
    tData->numberOfRuns = 0;
    tData->isOverflow = false;

    // Create temporary file for storing runs.
    sprintf(tData->bigQFileName, "%d.bin", pthread_self());
    tData->bigQFile.Open(0, tData->bigQFileName);
}

void GenerateRun(WorkerThread *tData) {
    Record *next = new Record();

    while (tData->iPipe->Remove(next)) {
        if (!AddRecToCurrRun(tData, next)) {
            CustomSortAndWrite(tData);
            SetupNewRun(tData);
            AddRecToCurrRun(tData, next);
        }
    }
    do {
        CustomSortAndWrite(tData);
        SetupNewRun(tData);
    } while (tData->isOverflow);
}

int AddRecToCurrRun(WorkerThread *tData, Record *next) {
    Page *runPage = &tData->currPages[tData->currPageNum];
    if (!runPage->Append(next)) {
        if (tData->currPageNum + 1 == tData->runLength) {
            return 0;
        } else {
            runPage = &tData->currPages[++tData->currPageNum];
        }
        runPage->Append(next);
    }
    return 1;
}

void CustomSortAndWrite(WorkerThread *worker) {
    // Using priority queue for in memory sort.
    priority_queue<Record *, vector<Record *>, RecordComparator> pq(worker->sortOrder);
    PutInPQ(worker, pq);

    // Get the records from the priority queue and add sorted pages of records in the BigQFile.
    WriteFromPQ(worker, pq);
}

// Put all the current run's record to the priority queue.
void PutInPQ(WorkerThread *worker,
                                 priority_queue<Record *, vector<Record *>, RecordComparator> &pq) {

    int i=0;
    int pageNum= worker->currPageNum;
    while(i<=pageNum)
    {
        Page *currPage = &worker->currPages[i]; 
        Record *temp = new Record();
        while (currPage->GetFirst(temp)) {
            pq.push(temp);
            temp = new Record();
        }
        currPage->EmptyItOut();
        i++;
    }
}

void WriteFromPQ(WorkerThread *worker,
                                         priority_queue<Record *, vector<Record *>, RecordComparator> &pq) {
    worker->isOverflow = false;
    Page *buff = new Page();
    // Get next empty page offset of BigQFile.
    off_t currPageIdxInFile =
            worker->bigQFile.GetLength() - 2 < 0 ? 0 : worker->bigQFile.GetLength() - 1;
    off_t maxPageNum = currPageIdxInFile + worker->runLength - 1;
    int pageNumOfExtraRec = 0;

    while (!pq.empty()) {
        // Overflow condition
        if (currPageIdxInFile > maxPageNum) {
            if (!(&worker->currPages[pageNumOfExtraRec])->Append(pq.top())) {
                pageNumOfExtraRec++;
            } else {
                pq.pop();
            }
        }
            // If the current buffer is full.
        else if (!buff->Append(pq.top())) {
            worker->bigQFile.AddPage(buff, currPageIdxInFile++);
            buff->EmptyItOut();
        } else {
            pq.pop();
        }
    }

    // If data is not over flow, store the last page in the BigQFile.
    if (currPageIdxInFile <= maxPageNum) {
        worker->bigQFile.AddPage(buff, currPageIdxInFile);
    } else {
        worker->isOverflow = true;
    }
}

void SetupNewRun(WorkerThread *worker) {
    worker->numberOfRuns++;
    worker->currPageNum = 0;
}

void MergeRuns(struct WorkerThread *worker) {
    priority_queue<PriorityQueueStruct, vector<PriorityQueueStruct>, RecordComparator> pq(worker->sortOrder);

    PQLoadMergeRun(worker, pq);

    PQLoadOPipe(worker, pq);
}

void PQLoadMergeRun(WorkerThread *worker,
                               priority_queue<PriorityQueueStruct,
                                       vector<PriorityQueueStruct>,
                                       RecordComparator> &pq) {
    PriorityQueueStruct *pQStruct = new PriorityQueueStruct[worker->numberOfRuns];
    int i=0;
    int numRuns= worker->numberOfRuns;
    while(i<numRuns)
    {
        pQStruct[i].pqCurrPageNum = i * worker->runLength;
        pQStruct[i].pqMaxCurrRunPageNum =
                std::min((off_t) pQStruct[i].pqCurrPageNum + worker->runLength - 1,
                         worker->bigQFile.GetLength() - 2);

        pQStruct[i].page = new Page();
        worker->bigQFile.GetPage(pQStruct[i].page, pQStruct[i].pqCurrPageNum);
        pQStruct[i].head = new Record();
        pQStruct[i].page->GetFirst(pQStruct[i].head);
        pq.push(pQStruct[i]);
        i++;
    }
}

void PQLoadOPipe(WorkerThread *worker,
                                         priority_queue<PriorityQueueStruct,
                                                 vector<PriorityQueueStruct>, RecordComparator> &pq) {
    while (!pq.empty()) {
        PriorityQueueStruct temp = pq.top();
        pq.pop();
        worker->oPipe->Insert(temp.head);
        if (!temp.page->GetFirst(temp.head)) {
            if (temp.pqCurrPageNum + 1 <= temp.pqMaxCurrRunPageNum) {
                temp.pqCurrPageNum++;
                worker->bigQFile.GetPage(temp.page, temp.pqCurrPageNum);
                temp.page->GetFirst(temp.head);
                pq.push(temp);
            }
        } else {
            pq.push(temp);
        }
    }
}

void CleanUp(WorkerThread *worker) {
    // Close and delete the file.

    worker->bigQFile.Close();
    remove(worker->bigQFileName);

    delete[] worker->currPages;

    // Shut down output pipe.
    if (worker->oPipe) {
        worker->oPipe->ShutDown();
    }
}