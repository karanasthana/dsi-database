#ifndef STATISTICS_
#define STATISTICS_
#include "ParseTree.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unordered_map>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <map>
#include <sstream>

using namespace std;

class Relation{

    
public:
    unordered_map<string, int> attributeMap;
    int numOfTuple;
    string relationName;
    set<string> joinedRelation;
    Relation(string relationName, int numOfTuple) {
        this->numOfTuple = numOfTuple;
        this->relationName = relationName;
        joinedRelation.insert(relationName);
    }
    ~Relation(){
        attributeMap.clear();
        joinedRelation.clear();
    }
};


class Statistics
{
private:

public:
	Statistics();
	Statistics(Statistics &copyMe);
	~Statistics();

    unordered_map<string, Relation*> relationMap;
    bool PartitionAndParse(struct AndList *parseTree, char *subsetNames[], int numToJoin, unordered_map<string,long> &uniqvallist);
    bool attributeUtil(char *value,char *subsetNames[], int numToJoin,unordered_map<string,long> &uniqvallist);
    double estimateTuples(struct OrList *orList, unordered_map<string,long> &uniqvallist);
    void AddRel(char *relName, int numTuples);
	void AddAtt(char *relName, char *attName, int numDistincts);
	void CopyRel(char *oldName, char *newName);
	
	void Read(char *fromWhere);
	void Write(char *fromWhere);

	void  Apply(struct AndList *parseTree, char *relNames[], int numToJoin);
	double Estimate(struct AndList *parseTree, char **relNames, int numToJoin);
    unordered_map<string, Relation*> GetGroupNameToRelationMap();
    int GetRelationAttNumCount(char *relName, char *attName);

};

set<string> getJRelations(string subsetName);

string serializeJRelations(set<string> joinedRelation);

#endif