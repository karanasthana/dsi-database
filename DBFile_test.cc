#include "DBFile.h"
#include "test.h"

#include <gtest/gtest.h>

// Test case to Create a new file
TEST(DBFile, Create) { 
    DBFile dbFile;

    ASSERT_EQ(dbFile.Create(NULL, heap, NULL), 0);

    dbFile.Create("gtest.bin", heap, NULL);
    FILE* f = fopen("gtest.bin", "r");

    ASSERT_TRUE(f != NULL);
}

// Test case to Create and Open a new file
TEST(DBFile, Open) {
    DBFile dbFile;

    ASSERT_EQ(dbFile.Open(NULL), 0);
    dbFile.Create("gtest.bin", heap, NULL);
    ASSERT_EQ(dbFile.Open("gtest.bin"), 1);
    FILE* f = fopen("gtest.bin", "r");

    ASSERT_TRUE(f != NULL);
}

// Test case to Close the file
TEST(DBFile, Close) {
    DBFile dbFile;
    dbFile.Create("gtest.bin", heap, NULL);
    dbFile.Open("gtest.bin");
    ASSERT_EQ(dbFile.Close(), 1);
}

// Test case to Load and open the file
TEST(DBFile, load) {
    DBFile dbfile;
    dbfile.Load (*(new Schema ("catalog", nation)), "/cise/homes/prateek.agrawal/git/dsi-database/data-files/nation.tbl");
	dbfile.Open ("nation.bin");
	dbfile.MoveFirst ();

	Record temp;

    ASSERT_EQ(dbfile.GetNext (temp), 1);

}

// Test case to GetNext record from the file
TEST(DBFile, GetNext) {
    DBFile dbfile;
	dbfile.Open ("nation.bin");
	dbfile.MoveFirst ();

	Record temp;

	int counter = 0;
    ASSERT_EQ(dbfile.GetNext (temp), 1);

}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}