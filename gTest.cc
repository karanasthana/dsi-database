#include <iostream>
#include <fstream>
//include the google test dependencies
#include <gtest/gtest.h>
//declare the function(s) that you are testing
#include "Pipe.h"
#include "Sorted.h"
#include "Record.h"
#include "DBFile.h"
#include "Schema.h"
#include "BigQ.h"
#include "test.h"

TEST(DBFile, TestCreate0) {
    DBFile dbFile;
    ASSERT_EQ(dbFile.Create(nullptr, heap, nullptr), 0);
}

TEST(DBFile, TestCreate1) {
    DBFile dbFile;
    ASSERT_EQ(dbFile.Create("nation_2.bin", heap, nullptr), 1);

    FILE* f = fopen("nation_2.bin", "r");
    ASSERT_TRUE(f != nullptr);
}

TEST(DBFile, TestOpen0) {
    DBFile dbFile;
    ASSERT_EQ(dbFile.Open(nullptr), 0);
}

TEST(DBFile, TestOpen1) {
    DBFile dbFile;
    ASSERT_EQ(dbFile.Open("nation.bin"), 1);

    FILE* f = fopen("nation.bin", "r");
    ASSERT_TRUE(f != nullptr);
}

TEST(SortedFile, TestCreateSortedRegionFile) {
    DBFile* dbfile=new DBFile();
  
    char catalog_path[]="catalog";
    char test[]="region";
    
    Schema* testSchema=new Schema(catalog_path,test);
    OrderMaker* myOrderMaker1=new OrderMaker(testSchema);
    int runlen=5;
    struct 
    {
        OrderMaker* o; 
        int l;
    } startup = {myOrderMaker1, runlen};
    
    char* tempFilePath="region.bin";
    int tempResult=dbfile->Create(tempFilePath, sorted, &startup);   
    int result=dbfile->Open(tempFilePath);
    EXPECT_EQ(1, result);
    dbfile->Close();
    delete dbfile;
}

TEST(SortedFile, TestCreateSortedNationFile) {
    DBFile* dbfile=new DBFile();
  
    char catalog_path[]="catalog";
    char test[]="nation";
    
    Schema* testSchema=new Schema(catalog_path,test);
    OrderMaker* myOrderMaker1=new OrderMaker(testSchema);
    int runlen=8;
    struct 
    {
        OrderMaker* o; 
        int l;
    } startup = {myOrderMaker1, runlen};
    
    char* tempFilePath="nation.bin";
    int tempResult=dbfile->Create(tempFilePath, sorted, &startup);   
    int result=dbfile->Open(tempFilePath);
    EXPECT_EQ(1, result);
    dbfile->Close();
    delete dbfile;
}

TEST (SortedFile, LoadRegionFileTest)
{
    char catalog_path[]="catalog";
    char region[]="region";
    char loadPath[]="data-files/region.tbl";
    
    Schema* testSchema=new Schema(catalog_path,region);
    
    OrderMaker* myOrderMaker1=new OrderMaker(testSchema);
    int runlen=8;
    struct 
    {
        OrderMaker* o; 
        int l;
    } startup = {myOrderMaker1, runlen};
    
    DBFile* dbfile=new DBFile();
    
    char* tempFilePath="region.bin";
    
    dbfile->Create(tempFilePath, sorted, &startup);
    dbfile->Open(tempFilePath);
    dbfile->Load(*testSchema, loadPath);
    Page* tempPage=new Page();
    delete tempPage;
    delete dbfile;
}

TEST (SortedFile, LoadNationFileTest)
{
    char catalog_path[]="catalog";
    char nation[]="nation";
    char loadPath[]="data-files/nation.tbl";
    
    Schema* testSchema=new Schema(catalog_path,nation);
    
    OrderMaker* myOrderMaker1=new OrderMaker(testSchema);
    int runlen=8;
    struct 
    {
        OrderMaker* o; 
        int l;
    } startup = {myOrderMaker1, runlen};
    
    DBFile* dbfile=new DBFile();
    
    char* tempFilePath="nation.bin";
    
    dbfile->Create(tempFilePath, sorted, &startup);
    dbfile->Open(tempFilePath);
    dbfile->Load(*testSchema, loadPath);
    Page* tempPage=new Page();
    delete tempPage;
    delete dbfile;
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}