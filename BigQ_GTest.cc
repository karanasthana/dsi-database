// #include "DBFile.h"
// #include "Schema.h"
// #include "BigQ.h"
// #include "test.h"

// #include <gtest/gtest.h>
// #include <fstream>
// #include <iostream>

// // Test to check Initialization of worker
// TEST(BigQ, TestInitialization)
// {
//     WorkerThread wt = {};
//     InitWorkerThread(&wt);

//     ifstream file;
//     file.open(wt.bigQFileName);

//     ASSERT_EQ(wt.currPageNum, 0);
//     ASSERT_EQ(wt.numberOfRuns, 0);

//     ASSERT_TRUE(wt.currPages != NULL);
// }

// // Test to check cleaup
// TEST(BigQ, TestCleanup)
// {
//     WorkerThread wt = {};
//     InitWorkerThread(&wt);
//     wt.oPipe = new Pipe(10);
//     CleanUp(&wt);

//     ifstream file;
//     file.open(wt.bigQFileName);
//     ASSERT_EQ(!file, true);
// }

// // Test to check adding records to run
// TEST(BigQ, TestAddRecordsCurrRun)
// {
//     WorkerThread wt = {
//         .runLength = 1,
//         .currPageNum = 0,
//     };
//     InitWorkerThread(&wt);

//     DBFile dbfile;
//     dbfile.Create("nation_3.bin", heap, NULL);
//     Schema *schema = new Schema("catalog", "nation");
//     char path[100];
//     sprintf(path, "%s%s.tbl", "/cise/homes/prateek.agrawal/git/dsi-database/data-files/", "nation");
//     dbfile.Load(*schema, path);

//     int numRec = 0;
//     Record temp;
//     while (dbfile.GetNext(temp))
//     {
//         numRec++;
//         AddRecToCurrRun(&wt, &temp);
//     }

//     int currRec = 0;
//     int i=0;
//     while(i<wt.runLength)
//     {
//         while (wt.currPages[i].GetFirst(&temp))
//         {
//             currRec++;
//         }
//         i++;
//     }

//     ASSERT_EQ(numRec, currRec);
//     CleanUp(&wt);
//     remove("nation_3.bin");
// }

// // Test to check load current run priority queue method
// TEST(BigQ, TestPutInPQ)
// {
//     WorkerThread wt = {
//         .runLength = 1,
//         .currPageNum = 0,
//     };
//     InitWorkerThread(&wt);

//     DBFile dbfile;
//     dbfile.Create("nation_2.bin", heap, NULL);
//     Schema *schema = new Schema("catalog", "nation");
//     char path[100];
//     sprintf(path, "%s%s.tbl", "/cise/homes/prateek.agrawal/git/dsi-database/data-files/", "nation");
//     dbfile.Load(*schema, path);

//     int numRec = 0;
//     Record temp;
//     while (dbfile.GetNext(temp))
//     {
//         numRec++;
//         AddRecToCurrRun(&wt, &temp);
//     }

//     priority_queue<Record *, vector<Record *>, RecordComparator> pq(new OrderMaker());
//     PutInPQ(&wt, pq);

//     ASSERT_EQ(numRec, pq.size());

//     int currRec = 0;
//     int i=0;
//     while(i<wt.runLength)
//     {
//         while (wt.currPages[i].GetFirst(&temp))
//         {
//             currRec++;
//         }
//         i++;
//     }

//     ASSERT_EQ(currRec, 0);
//     CleanUp(&wt);
//     remove("nation_2.bin");
// }

// int main(int argc, char **argv)
// {
//     testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }