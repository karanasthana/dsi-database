#include "Statistics.h"
#include <string>
#include <string.h>

Statistics::Statistics(){
}

Statistics::Statistics(Statistics &sc)
{
    unordered_map<string,Relation*>::iterator iter = relationMap.begin();
    while (iter != relationMap.end()) {
        Relation* cR = iter->second;
        Relation* nR = new Relation(cR->relationName, cR->numOfTuple);
        unordered_map<string, int>::iterator kvPair = cR->attributeMap.begin();
        while (kvPair != cR->attributeMap.end()) {
            nR->attributeMap[kvPair->first] = kvPair->second;
            kvPair++;
        }
        for (string jr : cR->joinedRelation) {
            if (jr.compare(cR->relationName) != 0)
                nR->joinedRelation.insert(jr);
        }
        sc.relationMap[nR->relationName] = nR;
        iter++;
    }
}

void Statistics::AddRel(char *relName, int numTuples)
{
    string sName = string(relName);
    relationMap[sName] = new Relation(sName, numTuples);
}

void Statistics::AddAtt(char *relName, char *attName, int distinctValues)
{
    string rName = string(relName);
    string aName = string(attName);
    if (relationMap.count(rName) == 0) {
        return;
    }
    if (distinctValues == -1) {
        distinctValues = relationMap[rName]->numOfTuple;
    }
    relationMap[rName]->attributeMap[aName] = distinctValues;
}

void Statistics::CopyRel(char *oName, char *nName)
{
    string oldRelName = string(oName);
    string newRelName = string(nName);
    if (relationMap.count(oldRelName) == 0)
        return;
    Relation* cR = relationMap[oldRelName];
    Relation* nR = new Relation(newRelName, cR->numOfTuple);
    unordered_map<string, int>::iterator kvPair = cR->attributeMap.begin();
    while (kvPair != cR->attributeMap.end()) {
        nR->attributeMap[newRelName + "." + kvPair->first] = kvPair->second;
        kvPair++;
    }
    for (string jr : cR->joinedRelation) {
        if (jr.compare(oldRelName) != 0)
            nR->joinedRelation.insert(jr);
    }
    relationMap[newRelName] = nR;
}

void Statistics::Read(char *from)
{
    ifstream inputStrm;
    inputStrm.open(from);
    string token;

    Relation* curRelation;
    while (inputStrm >> token) {
        if (token.compare("Relation:") == 0) {
            inputStrm >> token;
            string curRelationName = token;
            inputStrm >> token;            
            inputStrm >> token;
            inputStrm >> token;
            inputStrm >> token;
            int numOfTuple = stoi(token);
            curRelation = new Relation(curRelationName, numOfTuple);
        }
        else if (token.compare("JoinedRelation:") == 0) {
            inputStrm >> token;
            curRelation->joinedRelation.insert(token);
        }
        else if (token.compare("Attribute:") == 0) {
            inputStrm >> token;
            string curAttributeName = token;
            inputStrm >> token;
            inputStrm >> token;
            inputStrm >> token;
            inputStrm >> token;
            inputStrm >> token;
            int numOfDistinct = stoi(token);
            if (numOfDistinct == -1)
                numOfDistinct = curRelation->numOfTuple;
            curRelation->attributeMap[curAttributeName] = numOfDistinct;
            
        }
        else if (token.compare("End") == 0) {
            relationMap[curRelation->relationName] = curRelation;
        }
    }
    inputStrm.close();
}

void Statistics::Write(char *from)
{
    ofstream outputStream;
    outputStream.open(from);
    unordered_map<string, Relation*>::iterator kvPair;
    for (kvPair = relationMap.begin(); kvPair != relationMap.end(); kvPair++) {
        string curRelationName = kvPair->first;
        Relation* curRelation = kvPair->second;
        int numOfTuple = curRelation->numOfTuple;
        outputStream << "Relation: " << curRelationName << "\n"<<"Number Of Tuples: " << numOfTuple << "\n"; 

        unordered_map<string, int>* curAttributes = &(curRelation->attributeMap);
        unordered_map<string, int>::iterator kvPair2;
        for (kvPair2 = curAttributes->begin(); kvPair2 != curAttributes->end(); kvPair2++) {
            string curAttributeName = kvPair2->first;
            int numOfDistinct = kvPair2->second;
            outputStream << "Attribute: " << curAttributeName << "\n"<< "Number Of Distinct Values: " << numOfDistinct << "\n";
        }
        outputStream << "End" << "\n"<<"\n";
    }
    outputStream.close();
}



double Statistics::Estimate(struct AndList *tree, char **relationNames, int numToJoin)
{
    double res;
    res = 1.0;
    unordered_map<string,long> uniqueList;
    if(PartitionAndParse(tree,relationNames,numToJoin,uniqueList))
    {
        string subsetName="G";
        unordered_map<string,long> tval;

        int subsetSize = numToJoin;
        int i=-1;
        for(i=0;i<subsetSize;i++)
        {
            subsetName = subsetName + "," + relationNames[i];
        }
        // while(++i<subsetSize){
        //     subsetName = subsetName + "," + relationNames[i];
        // }
        i=-1;
        for(i=0;i<numToJoin;i++)
        {
            string serRes = serializeJRelations(relationMap[relationNames[i]]->joinedRelation);
            tval[serRes] = relationMap[relationNames[i]]->numOfTuple;        
        }
        // while(++i<numToJoin)
        // {
        //     string serRes = serializeJRelations(relationMap[relationNames[i]]->joinedRelation);
        //     tval[serRes] = relationMap[relationNames[i]]->numOfTuple;
        // }

        res = 1000.0;
        while(tree!= nullptr){
            res= estimateTuples(tree->left,uniqueList) * res;
            tree=tree->rightAnd;
        }
        unordered_map<string,long>::iterator ti=tval.begin();
        while(ti!=tval.end())
        {
            res*=ti->second;
            ti++;
        }
    }
    else
    {
        return -1.0;
    }
    res = res/1000.0;
    return res;
}

void  Statistics::Apply(struct AndList *parseTree, char *relNames[], int numToJoin)
{
    double r = Estimate(parseTree, relNames, numToJoin);
    long numTuples =(long)round(r);
    string subsetName="G";
    int i=numToJoin;
    while(i-- > 0)
    {
        subsetName = subsetName + "," + string(relNames[i]);
    }
    i=numToJoin;
    while(--i >= 0)
    {
        relationMap[relNames[i]]->joinedRelation = getJRelations(subsetName);
        relationMap[relNames[i]]->numOfTuple = numTuples;
    }
}

set<string> getJRelations(string subsetName) {
    set<string> joinedRelation;
    subsetName = subsetName + ",";
    int index = 0;
    int size = subsetName.size();
    while (subsetName.size() > 0) {
        index = subsetName.find(",");
        string sub = subsetName.substr(0, index);
        if (sub.compare("G") != 0) {
            joinedRelation.insert(sub);
        }
        subsetName.erase(0, index + 1);
    }
    return joinedRelation;
}

string serializeJRelations(set<string> joinedRelation) {
    if (joinedRelation.size() == 1)
        return *joinedRelation.begin();
    string res = "G";
    for (string rel : joinedRelation)
        res = res + "," + rel;
    return res;
}

bool Statistics::PartitionAndParse(struct AndList *tree, char *relationNames[], int sizeOfAttributesJoin,unordered_map<string,long> &uniqueList)
{
    bool res;
    res = true;
    while(!(tree==NULL || !res)){
        struct OrList *orListTop;
        orListTop=tree->left;
        while(!(orListTop==NULL || !res))
        {
            struct ComparisonOp *cmpOp = orListTop->left;
            if(!(cmpOp->left->code!=NAME || cmpOp->code!=STRING || attributeUtil(cmpOp->left->value,relationNames,sizeOfAttributesJoin,uniqueList))) {
                cout<<"\n"<< cmpOp->left->value<<" Does Not Exist";
                res=false;
            }
            if(!( cmpOp->right->code!=NAME || cmpOp->code!=STRING || attributeUtil(cmpOp->right->value,relationNames,sizeOfAttributesJoin,uniqueList))) {
                res=false;
            }
            orListTop=orListTop->rightOr;
        }
        tree=tree->rightAnd;
    }
    if(false==res) return res;
    unordered_map<string,int> tbl;
    int i=0;
    while(i<sizeOfAttributesJoin){
        string gn = serializeJRelations(relationMap[relationNames[i]]->joinedRelation);
        if(tbl.find(gn)==tbl.end()) {
            tbl[gn] = relationMap[string(relationNames[i])]->joinedRelation.size() - 1;
        }
        else
            tbl[gn]--;
        i++;
    }

    unordered_map<string,int>::iterator ti=tbl.begin();;
    while( ti!=tbl.end())
    {
        if(ti->second!=0)
        {
            res=false;
            break;
        }
        ti++;
    }
    return res;
}

bool Statistics::attributeUtil(char *v,char *relationNames[], int num,unordered_map<string,long> &uniqueList)
{
    for(int i=0; i<num; i++){
        unordered_map<string,Relation*>::iterator itr;
        itr = relationMap.find(relationNames[i]);
        if(relationMap.end() == itr){
            return false;
        }else {
            string relation = string(v);
            if(itr->second->attributeMap.end() != itr->second->attributeMap.find(relation))
            {
                uniqueList[relation]=itr->second->attributeMap.find(relation)->second;
                return true;
            }
        }
    }
    return false;
}

double Statistics::estimateTuples(struct OrList *orList, unordered_map<string,long> &uniqueList)
{
    unordered_map<string,double> selecMap;
    while(!(orList==NULL && true))
    {
        struct ComparisonOp *comp=orList->left;
        string key = string(comp->left->value);
        if(selecMap.end()==selecMap.find(key)) selecMap[key]=0.0;
        if(!(comp->code != 1 && comp->code != 2)){
            selecMap[key] = selecMap[key]+1.0/3;
        }
        else
        {
            string leftKeyVal = string(comp->left->value);
            long max_val = uniqueList[leftKeyVal];
            if(4==comp->right->code){
                string rightKeyVal = string(comp->right->value);
                if(uniqueList[rightKeyVal] > max_val)
                    max_val = uniqueList[rightKeyVal];
            }
            selecMap[key] = 1.0/max_val + selecMap[key] + 0;
        }
        orList=orList->rightOr;
    }

    double selectivity=1.0;
    unordered_map<string,double>::iterator itr;
    itr = selecMap.begin();
    while(itr!=selecMap.end()) {
        selectivity *= (1.0 - itr->second);
        itr++;
    }
    selectivity = (1.0-selectivity);
    return selectivity;
}

unordered_map<string, Relation*> Statistics::GetGroupNameToRelationMap() {
    return this->relationMap;
}

int Statistics::GetRelationAttNumCount(char *relName, char *attName) {
    string rName = string(relName);
    string aName = string(attName);
    return this->relationMap[rName]->attributeMap[aName];
}

Statistics::~Statistics(){
}



