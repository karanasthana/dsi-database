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

int main(int argc, char **argv){
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}