#include <algorithm>
#include <queue>

#include "BigQ.h"

Run::Run(File *bigQFile, off_t start, off_t end)
{
    tempFilePtr = bigQFile;

    currPageIndex = start;
    endPageIndex = end;

    bufferPage = new Page();
    tempFilePtr->GetPage(bufferPage, currPageIndex++);

    currentRec = new Record();
    bufferPage->GetFirst(currentRec);
}

int Run::Next(Record *current)
{
    current->Consume(currentRec);
    if (!bufferPage->GetFirst(currentRec))
    {
        if (currPageIndex < endPageIndex)
        {
            bufferPage->EmptyItOut();
            tempFilePtr->GetPage(bufferPage, currPageIndex++);
            bufferPage->GetFirst(currentRec);
        }
        else
        {
            return 0;
        }
    }

    return 1;
}

Run::~Run()
{
    delete tempFilePtr;
    delete bufferPage;
    delete currentRec;
}

// Worker thread.
void *Worker(void *bigQ)
{
    auto *myBigQ = (BigQ *)bigQ;
    myBigQ->ExecuteSort();
    myBigQ->ExecuteMerge();
    return nullptr;
}

BigQ::BigQ(Pipe &input, Pipe &output, OrderMaker &sortOrder, int runLength)
{
    this->input = &input;
    this->output = &output;
    this->sortOrder = &sortOrder;
    this->runLength = runLength;

    bigQFile = new File();
    bigQFile->Open(0, tempFileName);

    comp = new ComparisonEngine();

    pthread_t worker;
    pthread_create(&worker, nullptr, Worker, (void *)this);
}

void BigQ::ExecuteSort()
{
    int currentSize = 0, capacity = runLength * PAGE_SIZE;

    Page tempPage;
    off_t pageIndex = 0;

    Record tempRec;
    Record *copyRec;

    vector<Record *> records;

    while (input->Remove(&tempRec))
    {
        copyRec = new Record();
        copyRec->Consume(&tempRec);

        currentSize += copyRec->getRecordSize();

        if (currentSize <= capacity)
        {
            records.push_back(copyRec);
        }
        else
        {
            sort(records.begin(), records.end(), [this](Record *left, Record *right)
                 { return comp->Compare(left, right, sortOrder) < 0; });

            for (auto &rec : records)
            {
                if (!tempPage.Append(rec))
                {
                    bigQFile->AddPage(&tempPage, pageIndex++);
                    tempPage.EmptyItOut();
                    tempPage.Append(rec);
                }
            }

            if (tempPage.GetNumberOfRecs() > 0)
            {
                bigQFile->AddPage(&tempPage, pageIndex++);
                tempPage.EmptyItOut();
            }

            runIndexes.push_back(pageIndex);

            for (auto &rec : records)
                delete rec;
            records.clear();
            records.push_back(copyRec);
            currentSize = sizeof(int) + copyRec->getRecordSize();
        }
    }

    sort(records.begin(), records.end(), [this](Record *left, Record *right)
         { return comp->Compare(left, right, sortOrder) < 0; });

    for (auto &rec : records)
    {
        if (!tempPage.Append(rec))
        {
            bigQFile->AddPage(&tempPage, pageIndex++);
            tempPage.EmptyItOut();
            tempPage.Append(rec);
        }
    }

    if (tempPage.GetNumberOfRecs() > 0)
    {
        bigQFile->AddPage(&tempPage, pageIndex++);
        tempPage.EmptyItOut();
    }

    runIndexes.push_back(pageIndex);

    for (auto &rec : records)
        delete rec;
    records.clear();
}

void BigQ::ExecuteMerge()
{
    auto comparator = [this](Run *left, Run *right)
    {
        return comp->Compare(left->currentRec, right->currentRec, sortOrder) >= 0;
    };

    priority_queue<Run *, vector<Run *>, decltype(comparator)> PQ(comparator);

    off_t prev = 0;
    for (auto &index : runIndexes)
    {
        PQ.push(new Run(bigQFile, prev, index));
        prev = index;
    }

    Record tempRec;
    Run *tempRun;
    while (!PQ.empty())
    {
        tempRun = PQ.top();
        PQ.pop();

        if (tempRun->Next(&tempRec))
        {
            PQ.push(tempRun);
        }

        output->Insert(&tempRec);
    }
    delete tempRun;

    output->ShutDown();

    bigQFile->Close();
    remove(tempFileName);
}

BigQ::~BigQ()
{
    delete bigQFile;
    delete comp;
}