#include "Heap.h"
#include "Sorted.h"
#include <iostream>
#include <fstream>

void Sorted::Read()
{
    if (mode != READ)
    {
        mode = READ;
        Merge();

        delete iPipe;
        delete oPipe;
        delete bigQ;
        iPipe = nullptr, oPipe = nullptr, bigQ = nullptr;

        queryOrder = cnfOrder = nullptr;
    }
}

void Sorted::Write()
{
    if (mode != WRITE)
    {
        mode = WRITE;

        iPipe = new Pipe(PIPE_BUFFER_SIZE);
        oPipe = new Pipe(PIPE_BUFFER_SIZE);

        bigQ = new BigQ(*iPipe, *oPipe, *sortOrder, runLength);

        queryOrder = cnfOrder = nullptr;
    }
}

void Sorted::Merge()
{
    iPipe->ShutDown();

    if (file->GetLength() > 0)
    {
        MoveFirst();
    }

    Heap *mFile = new Heap();
    mFile->Create(tempFileLoc.c_str(), heap, nullptr);

    auto *fRecord = new Record();
    bool fileRec = GetNext(*fRecord);

    auto *pRecord = new Record();
    bool pipeRec = oPipe->Remove(pRecord);

    while (fileRec && pipeRec)
    {
        if (comp->Compare(fRecord, pRecord, sortOrder) < 0)
        {
            mFile->Add(*fRecord);
            fileRec = GetNext(*fRecord);
        }
        else
        {
            mFile->Add(*pRecord);
            pipeRec = oPipe->Remove(pRecord);
        }
    }

    while (fileRec)
    {
        mFile->Add(*fRecord);
        fileRec = GetNext(*fRecord);
    }

    while (pipeRec)
    {
        mFile->Add(*pRecord);
        pipeRec = oPipe->Remove(pRecord);
    }

    delete fRecord;
    delete pRecord;

    oPipe->ShutDown();

    mFile->CloseFile();
    delete mFile;

    remove(filePath.c_str());
    rename(tempFileLoc.c_str(), filePath.c_str());
    file->Open(1, filePath.c_str());

    MoveFirst();
}

void Sorted::buildQuery(CNF &cnf)
{
    queryOrder = new OrderMaker();
    cnfOrder = new OrderMaker();

    for (int i = 0; i < sortOrder->numAtts; i++)
    {
        bool found = false;
        for (int j = 0; j < cnf.numAnds; j++)
        {
            if (cnf.orLens[j] != 1 || cnf.orList[j][0].op != Equals)
            {
                continue;
            }
            if (cnf.orList[j][0].operand1 == Literal && cnf.orList[j][0].whichAtt2 == sortOrder->whichAtts[i])
            {
                found = true;

                queryOrder->whichAtts[queryOrder->numAtts] = cnf.orList[j][0].whichAtt2;
                cnfOrder->whichAtts[cnfOrder->numAtts] = cnf.orList[j][0].whichAtt1;
            }
            else if (cnf.orList[j][0].operand2 == Literal && cnf.orList[j][0].whichAtt1 == sortOrder->whichAtts[i])
            {
                found = true;
                queryOrder->whichAtts[queryOrder->numAtts] = cnf.orList[j][0].whichAtt1;
                cnfOrder->whichAtts[cnfOrder->numAtts] = cnf.orList[j][0].whichAtt2;
            }
            else
            {
                found = false;
            }

            if (found)
            {
                queryOrder->whichTypes[queryOrder->numAtts] = sortOrder->whichTypes[i];
                cnfOrder->whichTypes[cnfOrder->numAtts] = sortOrder->whichTypes[i];

                queryOrder->numAtts++;
                cnfOrder->numAtts++;

                break;
            }
        }

        if (!found)
            break;
    }
}

Sorted::Sorted()
{
    Init();
}

Sorted::Sorted(OrderMaker *orderMaker, int runLength)
{
    this->sortOrder = orderMaker;
    this->runLength = runLength;

    Init();
}

void Sorted::Init()
{
    mode = READ;

    file = new File();

    bufferedPage = new Page();

    currPagePtr = 0;

    comp = new ComparisonEngine();

    queryOrder = cnfOrder = nullptr;
}

Sorted::~Sorted()
{
    delete file;
    delete comp;
    delete queryOrder;
}

int Sorted::Create(const char *fp, typeOfFile type, void *startUp)
{
    filePath = string(fp);

    size_t index = filePath.find_last_of("/\\");
    tempFileLoc = filePath.substr(0, index + 1);
    tempFileLoc.append("merge.bin");

    typedef struct
    {
        OrderMaker *sortOrder;
        int runLength;
    } * Args;
    Args args = (Args)startUp;
    sortOrder = args->sortOrder;
    runLength = args->runLength;

    file->Open(0, fp);

    return 1;
}

int Sorted::Open(const char *fp)
{
    this->filePath = string(fp);

    size_t index = filePath.find_last_of("/\\");
    tempFileLoc = filePath.substr(0, index + 1);
    tempFileLoc.append("merge.bin");

    file->Open(1, filePath.c_str());
    return 1;
}

int Sorted::Close()
{
    string metadataFilePath(filePath);
    metadataFilePath.append(".metadata");

    ofstream metadataFile;
    metadataFile.open(metadataFilePath.c_str());
    if (!metadataFile.is_open())
    {
        cout << "SORTED : Not able to create meta data file " << metadataFilePath << '\n';
        exit(1);
    }

    metadataFile << FILE_TYPE_SORTED << "\n";
    metadataFile << runLength << "\n";
    metadataFile << sortOrder->ToString() << "\n";
    metadataFile.close();

    Read();

    return file->Close();
}

void Sorted::Load(Schema &schema, const char *loadPath)
{
    Write();

    int count = 0;

    FILE *tblFile = fopen(loadPath, "r");
    if (tblFile != nullptr)
    {
        Record next;
        while (next.SuckNextRecord(&schema, tblFile))
        {
            iPipe->Insert(&next);
            count++;
        }
        fclose(tblFile);
    }
    else
    {
        cout << "Not able to open file " << tblFile << '\n';
        exit(1);
    }

    // cout << "Loaded " << count << " records from " << loadPath << '\n';
}

void Sorted::MoveFirst()
{
    Read();

    currPagePtr = 0;
    bufferedPage->EmptyItOut();
    if (file->GetLength() > 0)
    {
        file->GetPage(bufferedPage, currPagePtr++);
    }
}

void Sorted::Add(Record &addMe)
{
    Write();
    iPipe->Insert(&addMe);
}

int Sorted::GetNext(Record &fetchMe)
{
    Read();

    if (!bufferedPage->GetFirst(&fetchMe))
    {
        if (currPagePtr < file->GetLength() - 1)
        {
            file->GetPage(bufferedPage, currPagePtr++);
            bufferedPage->GetFirst(&fetchMe);
        }
        else
            return 0;
    }
    return 1;
}

int Sorted::GetNext(Record &fetchMe, CNF &cnf, Record &literal)
{
    Read();

    bool isCnfUpdated = false;
    if (queryOrder == nullptr || cnfOrder == nullptr)
    {
        isCnfUpdated = true;
        buildQuery(cnf);
    }

    if (queryOrder->numAtts > 0)
    {
        return binarySearchNext(fetchMe, cnf, literal, isCnfUpdated);
    }
    else
    {
        return linearSearchNext(fetchMe, cnf, literal);
    }
}

int Sorted::binarySearchNext(Record &fetchMe, CNF &cnf, Record &literal, bool isCnfUpdated)
{
    if (isCnfUpdated)
    {
        off_t index = performBinarySearch(currPagePtr, file->GetLength() - 2, literal);
        if (index == -1)
        {
            return 0;
        }
        else
        {
            currPagePtr = index;
            bufferedPage->EmptyItOut();
            file->GetPage(bufferedPage, currPagePtr++);
        }
    }

    while (GetNext(fetchMe))
    {
        if (comp->Compare(&fetchMe, queryOrder, &literal, cnfOrder) == 0 && comp->Compare(&fetchMe, &literal, &cnf))
        {
            return 1;
        }
    }

    return 0;
}

off_t Sorted::performBinarySearch(off_t low, off_t high, Record &literal)
{
    if (low == high)
        return low;

    off_t mid = (low + high) / 2;

    Page tempPage;
    file->GetPage(&tempPage, mid);

    Record tempRec;
    tempPage.GetFirst(&tempRec);

    if (comp->Compare(&tempRec, queryOrder, &literal, cnfOrder) < 0)
    {
        return performBinarySearch(mid + 1, high, literal);
    }

    return performBinarySearch(low, mid - 1, literal);
}

int Sorted::linearSearchNext(Record &fetchMe, CNF &cnf, Record &literal)
{
    while (true)
    {
        int hasRecord = GetNext(fetchMe);
        if (!hasRecord)
            return 0;

        if (comp->Compare(&fetchMe, &literal, &cnf))
        {
            return 1;
        }
    }
}