#include "DBFile.h"
#include "Schema.h"
#include "BigQ.h"
#include "test.h"

#include <gtest/gtest.h>
#include <fstream>
#include <iostream>

// Test to check worker thread initialization method
TEST(BigQ, TestInitializeWorkerThreadMethod)
{
    WorkerThread wt = {};
    InitWorkerThread(&wt);

    ifstream iFile;
    iFile.open(wt.bigQFileName);

    ASSERT_EQ(wt.currentRunPageNumber, 0);
    ASSERT_EQ(wt.numberOfRuns, 0);

    ASSERT_TRUE(wt.currentRunPages != NULL);
}

// Test to check cleaup method
TEST(BigQ, TestCleanUpMethod)
{
    WorkerThread wt = {};
    InitWorkerThread(&wt);
    wt.outputPipe = new Pipe(10);
    CleanUp(&wt);

    ifstream iFile;
    iFile.open(wt.bigQFileName);
    ASSERT_EQ(!iFile, true);
}

// Test to check adding records to run method
TEST(BigQ, TestAddRecordToCurrentRunMethod)
{
    WorkerThread wt = {
        .runLength = 1,
        .currentRunPageNumber = 0,
    };
    InitWorkerThread(&wt);

    DBFile dbfile;
    dbfile.Create("nation.bin", heap, NULL);
    Schema *rschema = new Schema("catalog", "nation");
    char tbl_path[100];
    sprintf(tbl_path, "%s%s.tbl", "/cise/homes/prateek.agrawal/git/dsi-database/data-files/", "nation");
    dbfile.Load(*rschema, tbl_path);

    int numRec = 0;
    Record temp;
    while (dbfile.GetNext(temp))
    {
        numRec++;
        AddRecordToCurrentRun(&wt, &temp);
    }

    int currRec = 0;
    for (int i = 0; i < wt.runLength; i++)
    {
        while (wt.currentRunPages[i].GetFirst(&temp))
        {
            currRec++;
        }
    }
    ASSERT_EQ(numRec, currRec);
    CleanUp(&wt);
    remove("nation.bin");
}

// Test to check load current run priority queue method
TEST(BigQ, TestLoadCurrentRunPriorityQueueMethod)
{
    WorkerThread wt = {
        .runLength = 1,
        .currentRunPageNumber = 0,
    };
    InitWorkerThread(&wt);

    DBFile dbfile;
    dbfile.Create("nation.bin", heap, NULL);
    Schema *rschema = new Schema("catalog", "nation");
    char tbl_path[100];
    sprintf(tbl_path, "%s%s.tbl", "/cise/homes/prateek.agrawal/git/dsi-database/data-files/", "nation");
    dbfile.Load(*rschema, tbl_path);

    int numRec = 0;
    Record temp;
    while (dbfile.GetNext(temp))
    {
        numRec++;
        AddRecordToCurrentRun(&wt, &temp);
    }

    priority_queue<Record *, vector<Record *>, RecordComparator> pq(new OrderMaker());
    LoadCurrentRunPriorityQueue(&wt, pq);

    ASSERT_EQ(numRec, pq.size());

    int currRec = 0;
    for (int i = 0; i < wt.runLength; i++)
    {
        while (wt.currentRunPages[i].GetFirst(&temp))
        {
            currRec++;
        }
    }

    ASSERT_EQ(currRec, 0);
    CleanUp(&wt);
    remove("nation.bin");
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}