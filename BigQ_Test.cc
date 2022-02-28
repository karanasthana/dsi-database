#include "DBFile.h"
#include "Schema.h"
#include "BigQ.h"
#include "test.h"

#include <gtest/gtest.h>
#include <fstream>
#include <iostream>

// Test to check worker thread initialization method
TEST(BigQ, TestInitializeWorkerThreadDataMethod)
{
    WorkerThreadData wtd = {};
    InitializeWorkerThreadData(&wtd);

    ifstream iFile;
    iFile.open(wtd.bigQFileName);

    ASSERT_EQ(wtd.currentRunPageNumber, 0);
    ASSERT_EQ(wtd.numberOfRuns, 0);

    ASSERT_TRUE(wtd.currentRunPages != NULL);
}

// Test to check cleaup method
TEST(BigQ, TestCleanUpMethod)
{
    WorkerThreadData wtd = {};
    InitializeWorkerThreadData(&wtd);
    wtd.outputPipe = new Pipe(10);
    CleanUp(&wtd);

    ifstream iFile;
    iFile.open(wtd.bigQFileName);
    ASSERT_EQ(!iFile, true);
}

// Test to check adding records to run method
TEST(BigQ, TestAddRecordToCurrentRunMethod)
{
    WorkerThreadData wtd = {
        .runLength = 1,
        .currentRunPageNumber = 0,
    };
    InitializeWorkerThreadData(&wtd);

    DBFile dbfile;
    dbfile.Create("nation.bin", heap, NULL);
    Schema *rschema = new Schema("catalog", "nation");
    char tbl_path[100];
    sprintf(tbl_path, "%s%s.tbl", "/cise/homes/karanasthana/git/dbi-project-impl/data-files/", "nation");
    dbfile.Load(*rschema, tbl_path);

    int numRec = 0;
    Record temp;
    while (dbfile.GetNext(temp))
    {
        numRec++;
        AddRecordToCurrentRun(&wtd, &temp);
    }

    int currRec = 0;
    for (int i = 0; i < wtd.runLength; i++)
    {
        while (wtd.currentRunPages[i].GetFirst(&temp))
        {
            currRec++;
        }
    }
    ASSERT_EQ(numRec, currRec);
    CleanUp(&wtd);
    remove("nation.bin");
}

// Test to check load current run priority queue method
TEST(BigQ, TestLoadCurrentRunPriorityQueueMethod)
{
    WorkerThreadData wtd = {
        .runLength = 1,
        .currentRunPageNumber = 0,
    };
    InitializeWorkerThreadData(&wtd);

    DBFile dbfile;
    dbfile.Create("nation.bin", heap, NULL);
    Schema *rschema = new Schema("catalog", "nation");
    char tbl_path[100];
    sprintf(tbl_path, "%s%s.tbl", "/cise/homes/karanasthana/git/dbi-project-impl/data-files/", "nation");
    dbfile.Load(*rschema, tbl_path);

    int numRec = 0;
    Record temp;
    while (dbfile.GetNext(temp))
    {
        numRec++;
        AddRecordToCurrentRun(&wtd, &temp);
    }

    priority_queue<Record *, vector<Record *>, RecordComparator> pq(new OrderMaker());
    LoadCurrentRunPriorityQueue(&wtd, pq);

    ASSERT_EQ(numRec, pq.size());

    int currRec = 0;
    for (int i = 0; i < wtd.runLength; i++)
    {
        while (wtd.currentRunPages[i].GetFirst(&temp))
        {
            currRec++;
        }
    }

    ASSERT_EQ(currRec, 0);
    CleanUp(&wtd);
    remove("nation.bin");
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}