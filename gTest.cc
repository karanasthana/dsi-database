#include <iostream>
#include "DBFile.h"
#include "BigQ.h"
#include "RelOp.h"
#include "test_a3.h"
// #include "test_a22.h"
#include "Pipe.h"
#include "DBFileSorted.h"
#include "Record.h"
#include "Schema.h"
#include "Statistics.h"

#include <fstream>
#include <gtest/gtest.h>
#include <pthread.h>

TEST(Statistics, TestGroupNameToRelationMapSize) {
	Statistics stats;
	stats.AddRel("orders",1500000);
	unordered_map<string, Relation*> groupNameToRelationMap = stats.GetGroupNameToRelationMap();
	EXPECT_EQ(1, groupNameToRelationMap.size());

    char *rels[4] = { "lineitem", "nation", "region", "parts" };
    int vals[4] = { 150000, 15, 10, 1400 };

    for (int i = 0; i < 4; i++) {
        stats.AddRel(rels[i], vals[i]);
    }

    groupNameToRelationMap = stats.GetGroupNameToRelationMap();
    EXPECT_EQ(5, groupNameToRelationMap.size());
}
TEST(Statistics, TestGetRelationAttNumCount) {
	Statistics statistics;
    char *relName = "relation";
    char *attName = "attribute";
    string relNameWithAttName = string(relName) + "." + string(attName);
    int numTuples = 1000;
    int numDistincts = 25;
    statistics.AddRel(relName, numTuples);
    statistics.AddRel("karan", 1000);
    statistics.AddRel("prateek", 1001);
    statistics.AddRel("dbi", 100);
    statistics.AddAtt(relName, attName, numDistincts);
    statistics.AddAtt("dbi", "marks", 100);


	EXPECT_EQ(numDistincts, statistics.GetRelationAttNumCount(relName, attName));
}
// TEST(DBFile, TestCreate0) {
//     DBFile dbFile;
//     ASSERT_EQ(dbFile.Create(nullptr, heap, nullptr), 0);
// }

// TEST(DBFile, TestCreate1) {
//     DBFile dbFile;
//     ASSERT_EQ(dbFile.Create("nation_2.bin", heap, nullptr), 1);

//     FILE* f = fopen("nation_2.bin", "r");
//     ASSERT_TRUE(f != nullptr);
// }

// TEST(DBFile, TestOpen0) {
//     DBFile dbFile;
//     ASSERT_EQ(dbFile.Open(nullptr), 0);
// }

// // TEST(DBFile, TestOpen1) {
// //     DBFile dbFile;
// //     ASSERT_EQ(dbFile.Open("nation.bin"), 1);

// //     FILE* f = fopen("nation.bin", "r");
// //     ASSERT_TRUE(f != nullptr);
// // }

// TEST(SortedFile, TestCreateSortedRegionFile) {
//     DBFile* dbfile=new DBFile();
  
//     char catalog_path[]="catalog";
//     char test[]="region";
    
//     Schema* testSchema=new Schema(catalog_path,test);
//     OrderMaker* myOrderMaker1=new OrderMaker(testSchema);
//     int runlen=5;
//     struct 
//     {
//         OrderMaker* o; 
//         int l;
//     } startup = {myOrderMaker1, runlen};
    
//     char* tempFilePath="region.bin";
//     int tempResult=dbfile->Create(tempFilePath, sorted, &startup);   
//     int result=dbfile->Open(tempFilePath);
//     EXPECT_EQ(1, result);
//     dbfile->Close();
//     delete dbfile;
// }

// TEST(SortedFile, TestCreateSortedNationFile) {
//     DBFile* dbfile=new DBFile();
  
//     char catalog_path[]="catalog";
//     char test[]="nation";
    
//     Schema* testSchema=new Schema(catalog_path,test);
//     OrderMaker* myOrderMaker1=new OrderMaker(testSchema);
//     int runlen=8;
//     struct 
//     {
//         OrderMaker* o; 
//         int l;
//     } startup = {myOrderMaker1, runlen};
    
//     char* tempFilePath="nation.bin";
//     int tempResult=dbfile->Create(tempFilePath, sorted, &startup);   
//     int result=dbfile->Open(tempFilePath);
//     EXPECT_EQ(1, result);
//     dbfile->Close();
//     delete dbfile;
// }

// TEST (SortedFile, LoadRegionFileTest)
// {
//     char catalog_path[]="catalog";
//     char region[]="region";
//     char loadPath[]="data-files/region.tbl";
    
//     Schema* testSchema=new Schema(catalog_path,region);
    
//     OrderMaker* myOrderMaker1=new OrderMaker(testSchema);
//     int runlen=8;
//     struct 
//     {
//         OrderMaker* o; 
//         int l;
//     } startup = {myOrderMaker1, runlen};
    
//     DBFile* dbfile=new DBFile();
    
//     char* tempFilePath="region.bin";
    
//     dbfile->Create(tempFilePath, sorted, &startup);
//     dbfile->Open(tempFilePath);
//     dbfile->Load(*testSchema, loadPath);
//     Page* tempPage=new Page();
//     delete tempPage;
//     delete dbfile;
// }

// TEST (SortedFile, LoadNationFileTest)
// {
//     char catalog_path[]="catalog";
//     char nation[]="nation";
//     char loadPath[]="data-files/nation.tbl";
    
//     Schema* testSchema=new Schema(catalog_path,nation);
    
//     OrderMaker* myOrderMaker1=new OrderMaker(testSchema);
//     int runlen=8;
//     struct 
//     {
//         OrderMaker* o; 
//         int l;
//     } startup = {myOrderMaker1, runlen};
    
//     DBFile* dbfile=new DBFile();
    
//     char* tempFilePath="nation.bin";
    
//     dbfile->Create(tempFilePath, sorted, &startup);
//     dbfile->Open(tempFilePath);
//     dbfile->Load(*testSchema, loadPath);
//     Page* tempPage=new Page();
//     delete tempPage;
//     delete dbfile;
// }

// TEST(SortedFile, TestToCheckSortedFileOpenNegative) {
//     char catalog_path[]="catalog";
//     char nation[]="nation";
//     char loadPath[]="data-files/nation_fake.tbl";
    
//     Schema* testSchema=new Schema(catalog_path,nation);
    
//     OrderMaker* myOrderMaker1=new OrderMaker(testSchema);
//     int runlen=8;
//     struct 
//     {
//         OrderMaker* o; 
//         int l;
//     } startup = {myOrderMaker1, runlen};
    
//     DBFile* dbfile=new DBFile();
    
//     char* tempFilePath="nation_fake.bin";
    
//     dbfile->Create(tempFilePath, sorted, &startup);
//     int a = dbfile->Open(tempFilePath);

//     ASSERT_EQ(0, a);
// }

// TEST(SortedFile, TestToCheckMetadataFileCreated) {
//     char catalog_path[]="catalog";
//     char nation[]="nation";
//     char loadPath[]="data-files/nation.tbl";
    
//     Schema* testSchema=new Schema(catalog_path,nation);
    
//     OrderMaker* myOrderMaker1=new OrderMaker(testSchema);
//     int runlen=8;
//     struct 
//     {
//         OrderMaker* o; 
//         int l;
//     } startup = {myOrderMaker1, runlen};
    
//     DBFile* dbfile=new DBFile();
    
//     char* tempFilePath="nation.bin";
    
//     dbfile->Create(tempFilePath, sorted, &startup);
//     dbfile->Open(tempFilePath);
//     dbfile->Load(*testSchema, loadPath);

//     // Check Metadata file is created.
//     ifstream fIn;
//     char metadataPath[100];
//     GetMataDataFilePath(tempFilePath, metadataPath);
//     fIn.open(metadataPath);
//     ASSERT_TRUE(fIn.is_open());
// }


// int clear_pipe (Pipe &in_pipe, Schema *schema, bool print) {
// 	Record rec;
// 	int cnt = 0;
// 	while (in_pipe.Remove (&rec)) {
// 		if (print) {
// 			rec.Print (schema);
// 		}
// 		cnt++;
// 	}
// 	return cnt;
// }

// int clear_pipe (Pipe &in_pipe, Schema *schema, Function &func, bool print) {
// 	Record rec;
// 	int cnt = 0;
// 	double sum = 0;
// 	while (in_pipe.Remove (&rec)) {
// 		if (print) {
// 			rec.Print (schema);
// 		}
// 		int ival = 0; double dval = 0;
// 		func.Apply (rec, ival, dval);
// 		sum += (ival + dval);
// 		cnt++;
// 	}
// 	cout << " Sum: " << sum << endl;
// 	return cnt;
// }

// // int pAtts = 9;
// int psAtts = 5;
// // int liAtts = 16;
// // int oAtts = 9;
// int sAtts = 7;
// // int cAtts = 8;
// int nAtts = 4;
// // int rAtts = 3;

// void printNHyphen(int n){
// 	for(int i=0; i<n; i++){
// 		cout<<"-";
// 	}
// 	cout<<endl;
// }

// TEST(RelOp, q1) {
//     setup();

//     SelectFile SF_ps;
//     Pipe _ps(100);
// 	CNF cnf_ps;
// 	Record lit_ps;
// 	DBFile dbf_ps;

// 	char *pred_ps = "(ps_supplycost = 1.03)";
	
//     dbf_ps.Open (ps->path());
// 	get_cnf (pred_ps, ps->schema (), cnf_ps, lit_ps);
// 	SF_ps.Use_n_Pages (100);
    
// 	SF_ps.Run (dbf_ps, _ps, cnf_ps, lit_ps);
//     SF_ps.WaitUntilDone ();

// 	int cnt = clear_pipe (_ps, ps->schema (), false);
	
// 	dbf_ps.Close ();

//     ASSERT_EQ(cnt, 10);
// }


// TEST(RelOp, q2) {
//     setup();

//     SelectFile SF_p;
//     Pipe _p(100);
// 	CNF cnf_p;
// 	Record lit_p;
// 	DBFile dbf_p;

// 	char *pred_p = "(p_partkey = 100)";

//     dbf_p.Open (p->path());
// 	get_cnf (pred_p, p->schema (), cnf_p, lit_p);
// 	SF_p.Use_n_Pages (100);
    
// 	SF_p.Run (dbf_p, _p, cnf_p, lit_p);
//     SF_p.WaitUntilDone ();

// 	int cnt = clear_pipe (_p, p->schema (), false);
	
// 	dbf_p.Close ();

//     ASSERT_EQ(cnt, 1);
// }


// TEST(RelOp, q3) {
//     setup();

//     SelectFile SF_p;
//     Pipe _p(100);
// 	CNF cnf_p;
// 	Record lit_p;
// 	DBFile dbf_p;

//     char *pred_p = "(p_retailprice > 931.00) AND (p_retailprice < 931.02)";
// 	dbf_p.Open (p->path());
// 	get_cnf (pred_p, p->schema (), cnf_p, lit_p);
// 	SF_p.Use_n_Pages (100);

// 	Project P_p;
//     Pipe _out (100);
//     int keepMe[] = {0};
//     int numAttsIn = 9;
//     int numAttsOut = 1;
// 	P_p.Use_n_Pages (100);

// 	SF_p.Run (dbf_p, _p, cnf_p, lit_p);
// 	P_p.Run (_p, _out, keepMe, numAttsIn, numAttsOut);

// 	SF_p.WaitUntilDone ();
// 	P_p.WaitUntilDone ();

// 	Attribute att3[] = {IA, SA, DA};

// 	Schema out_sch ("out_sch", numAttsOut, att3);
	
// 	int cnt = clear_pipe (_out, &out_sch, false);
	
// 	dbf_p.Close ();

//     ASSERT_EQ(10, cnt);
// }

// TEST(RelOp, q4) {
//     setup();

//     SelectFile SF_s;
//     Pipe _s(100);
// 	CNF cnf_s;
// 	Record lit_s;
// 	DBFile dbf_s;

//     char *pred_s = "(s_suppkey = s_suppkey)";
//     dbf_s.Open (s->path());
// 	get_cnf (pred_s, s->schema (), cnf_s, lit_s);
// 	SF_s.Use_n_Pages (100);

// 	Sum T;
// 	Pipe _out (1);
// 	Function func;
// 	char *str_sum = "(s_acctbal * 1.10)";
// 	get_cnf (str_sum, s->schema (), func);
// 	func.Print ();
// 	T.Use_n_Pages (1);
// 	SF_s.Run (dbf_s, _s, cnf_s, lit_s);
// 	T.Run (_s, _out, func);

// 	SF_s.WaitUntilDone ();
// 	T.WaitUntilDone ();

// 	Schema out_sch ("out_sch", 1, &DA);

//     Record rec;	
//     _out.Remove(&rec);
    
//     int cnt = clear_pipe (_out, &out_sch, true);

// 	dbf_s.Close ();

//     ASSERT_EQ(1, rec.GetAttrCount()); // the record is supposed to have only s_acctbal
// }


// // SELECT * FROM nation WHERE n_nationkey<10
// // expected number of records in output = 10
// // TEST(RelOp, NumberOfRecordsTest){
// // 	setup();

// // 	SelectFile SF_n;
// //     cout << "gtest line 65" << '\n';
// // 	Pipe _n(100);
// //     cout << "gtest line 67" << '\n';
// // 	CNF cnf_n;
// //     cout << "gtest line 69" << '\n';
// // 	Record lit_n;
// //     cout << "gtest line 71" << '\n';
// // 	DBFile dbf_n;
// //     cout << "gtest line 73" << '\n';
	
// // 	char *pred_n = "(n_nationkey < 10)";
// //     cout << "gtest line 76" << '\n';
// // 	dbf_n.Open (n->path());
// //     cout << "gtest line 78" << '\n';
// // 	get_cnf (pred_n, n->schema (), cnf_n, lit_n);
// //     cout << "gtest line 80" << '\n';
// // 	SF_n.Use_n_Pages (100);
// //     cout << "gtest line 82" << '\n';

// // 	SF_n.Run (dbf_n, _n, cnf_n, lit_n);
// //     cout << "gtest line 85" << '\n';
// // 	SF_n.WaitUntilDone ();
// //     cout << "gtest line 87" << '\n';

// // 	int cnt = clear_pipe (_n, n->schema (), false);
// //     cout << "gtest line 90" << '\n';
// // 	dbf_n.Close ();
// //     cout << "gtest line 92" << '\n';
// // 	cleanup();
// //     cout << "gtest line 94" << '\n';

// // 	ASSERT_EQ(cnt, 10);
// // }

// // // SELECT n_nationkey, n_name FROM nation
// // TEST(ProjectTest, NumberOfAttributesTest){
// // 	setup();

// // 	SelectFile SF_n;
// // 	Pipe _n(100);
// // 	CNF cnf_n;
// // 	Record lit_n;
// // 	DBFile dbf_n;

// // 	char *pred_n = "(n_nationkey = 0)";
// // 	dbf_n.Open (n->path());
// // 	get_cnf (pred_n, n->schema (), cnf_n, lit_n);
// // 	SF_n.Use_n_Pages (100);

// // 	Project P_n;
// // 	Pipe _out (100);
// // 	int keepMe[] = {0,1};
// // 	int numAttsIn = nAtts;
// // 	int numAttsOut = 2;
// // 	P_n.Use_n_Pages (100);

// // 	SF_n.Run (dbf_n, _n, cnf_n, lit_n);
// // 	P_n.Run (_n, _out, keepMe, numAttsIn, numAttsOut);

// // 	SF_n.WaitUntilDone ();
// // 	P_n.WaitUntilDone ();

// // 	Attribute att2[] = {IA, SA};
// // 	Schema out_sch ("out_sch", numAttsOut, att2);

// // 	Record rec;	
// // 	_out.Remove(&rec);
// // 	cout<<"First projected record is"<<endl;
// // 	printNHyphen(200);
// // 	rec.Print(&out_sch);
// // 	printNHyphen(200);

// // 	dbf_n.Close ();
// // 	cleanup();
// // 	ASSERT_EQ(2, rec.GetAttrCount()); // rec must have 2 attributes only
// // }


// // // SELECT SUM(n_nationkey) FROM nation
// // // expected sum = 300 (Formula: n*(n+1)/2 here n=24)
// // TEST(SumTest, SumCalculationTest){
// // 	setup();

// // 	SelectFile SF_n;
// // 	Pipe _n(100);
// // 	CNF cnf_n;
// // 	Record lit_n;
// // 	DBFile dbf_n;
// // 	char *pred_n = "(n_nationkey = n_nationkey)";
	
// // 	dbf_n.Open (n->path());
// // 	get_cnf (pred_n, n->schema (), cnf_n, lit_n);
// // 	SF_n.Use_n_Pages (100);

// // 	Sum T;
// // 	Pipe _out (1);
// // 	Function func;
// // 	char *str_sum = "(n_nationkey)";
// // 	get_cnf (str_sum, n->schema (), func);
// // 	func.Print ();
// // 	T.Use_n_Pages (1);
// // 	SF_n.Run (dbf_n, _n, cnf_n, lit_n);
// // 	T.Run (_n, _out, func);

// // 	SF_n.WaitUntilDone ();
// // 	T.WaitUntilDone ();

// // 	Schema out_sch ("out_sch", 1, &IA);

// // 	Record rec;
// // 	_out.Remove (&rec);
// // 	cout<<"Sum record is"<<endl;
// // 	printNHyphen(200);
// // 	rec.Print(&out_sch);
// // 	printNHyphen(200);
// // 	int pointer = ((int *) rec.bits)[1];
// // 	int *outputSum = (int *) &(rec.bits[pointer]);


// // 	dbf_n.Close ();

// // 	ASSERT_EQ(*outputSum, 300); // check if outputsum is equal to the expected sum of 300
// // }

// // // join supplier and partsupp
// // // expected attributes in each record=7+5=12
// // TEST(JoinTest, NumberOfAttributesTest){
// // 	setup();

// // 	SelectFile SF_s;
// // 	Pipe _s(100);
// // 	CNF cnf_s;
// // 	Record lit_s;
// // 	DBFile dbf_s;
// // 	char *pred_s = "(s_suppkey = s_suppkey)";

// // 	dbf_s.Open (s->path());
// // 	get_cnf (pred_s, s->schema (), cnf_s, lit_s);
// // 	SF_s.Use_n_Pages (100);
// // 	SF_s.Run (dbf_s, _s, cnf_s, lit_s); // 10k recs qualified


// // 	SelectFile SF_ps;
// // 	Pipe _ps(100);
// // 	CNF cnf_ps;
// // 	Record lit_ps;
// // 	DBFile dbf_ps;
// // 	char *pred_ps = "(ps_suppkey = ps_suppkey)";
// // 	dbf_ps.Open (ps->path());
// // 	get_cnf (pred_ps, ps->schema (), cnf_ps, lit_ps);
// // 	SF_ps.Use_n_Pages (100);
	
// // 	Join J;
// // 	Pipe _s_ps (100);
// // 	CNF cnf_p_ps;
// // 	Record lit_p_ps;
// // 	get_cnf ("(s_suppkey = ps_suppkey)", s->schema(), ps->schema(), cnf_p_ps, lit_p_ps);

// // 	int outAtts = sAtts + psAtts;
// // 	Attribute ps_supplycost = {"ps_supplycost", Double};
// // 	Attribute joinatt[] = {IA,SA,SA,IA,SA,DA,SA, IA,IA,IA,ps_supplycost,SA};
// // 	Schema join_sch ("join_sch", outAtts, joinatt);

// // 	SF_ps.Run (dbf_ps, _ps, cnf_ps, lit_ps); // 161 recs qualified
// // 	J.Run (_s, _ps, _s_ps, cnf_p_ps, lit_p_ps);

// // 	SF_ps.WaitUntilDone ();

// // 	Record rec;
// // 	_s_ps.Remove(&rec);
// // 	cout<<"First joined record is"<<endl;
// // 	printNHyphen(200);
// // 	rec.Print(&join_sch);
// // 	printNHyphen(200);
// // 	ASSERT_EQ(sAtts+psAtts, rec.GetAttrCount()); // number of attributes in a joined record should be equal to sAtts+psAtts=12
// // }

int main(int argc, char **argv){
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}