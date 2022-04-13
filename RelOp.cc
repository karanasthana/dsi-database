#include "RelOp.h"
#include <iostream>
#include "BigQ.h"

typedef struct {
    DBFile &inputF;
    Pipe &outputP;
    CNF &cnfOp;
    Record &literal;
} w1;
void* wM1(void* arg){
    w1* workerArg = (w1*) arg;
    Record rec;
    while(workerArg->inputF.GetNext(rec, workerArg->cnfOp, workerArg->literal)){
        workerArg->outputP.Insert(&rec);
    }
    workerArg->outputP.ShutDown();
    return NULL;
}

void SelectFile::Run (DBFile &inputF, Pipe &outputP, CNF &cnfOp, Record &literal) {
    w1* workerArg = new w1{inputF, outputP, cnfOp, literal};
    pthread_create(&worker, NULL, wM1, (void*) workerArg);
}

void SelectFile::WaitUntilDone () {
	pthread_join (worker, NULL);
}

void SelectFile::Use_n_Pages (int runlen) {

}

typedef struct {
    Pipe &inPipe;
    Pipe &outputP;
    CNF &cnfOp;
    Record &literal;
} w2;
void* wM2(void*arg){
    ComparisonEngine comp;
    w2* workerArg = (w2*) arg;
    Record rec;
    while(workerArg->inPipe.Remove(&rec)){
        if(comp.Compare(&rec, &workerArg->literal, &workerArg->cnfOp)){
            workerArg->outputP.Insert(&rec);
        }
    }
    workerArg->outputP.ShutDown();
    return NULL;
}

void SelectPipe::Run (Pipe &inPipe, Pipe &outputP, CNF &cnfOp, Record &literal) {
    w2* workerArg = new w2{inPipe, outputP, cnfOp, literal};
    pthread_create(&worker, NULL, wM2, (void*) workerArg);
}

void SelectPipe::WaitUntilDone () {
	pthread_join (worker, NULL);
}

void SelectPipe::Use_n_Pages (int runlen) {
    //cout<<runlen<<endl;
}

typedef struct {
    Pipe &inPipe;
    Pipe &outputP;
    Function &computeMe;
} w3;
void* wM3(void*arg){
    int intSum = 0, intResult = 0;
    double doubleSum = 0.0, doubleResult = 0.0;
    ComparisonEngine comp;
    w3* workerArg = (w3*) arg;
    Record rec;
    Type t;
    while(workerArg->inPipe.Remove(&rec)){
        t = workerArg->computeMe.Apply(rec, intResult, doubleResult);
        if(t==Int){
            intSum += intResult;
        }
        else{
            doubleSum += doubleResult;
        }
    }

    Attribute DA = {"SUM", t};
    Schema out_sch ("out_sch", 1, &DA);
    Record res;
    char charsRes[100];
    if(t==Int){
        sprintf(charsRes, "%d|", intSum);
    }
    else{
        sprintf(charsRes, "%lf|", doubleSum);
    }
    res.ComposeRecord(&out_sch, charsRes);
    workerArg->outputP.Insert(&res);
    workerArg->outputP.ShutDown();
    return NULL;
}
void Sum::Run (Pipe &inPipe, Pipe &outputP, Function &computeMe){
    w3* workerArg = new w3{inPipe, outputP, computeMe};
    pthread_create(&worker, NULL, wM3, (void*) workerArg);
}
void Sum::WaitUntilDone (){
    pthread_join(worker, NULL);
}
void Sum::Use_n_Pages (int n){

}

typedef struct {
    Pipe &inPipe;
    Pipe &outputP;
    OrderMaker &groupAtts;
    Function &computeMe;
    int use_n_pages;
} w4;
void* wM4(void*arg){
    w4* workerArg = (w4*) arg;
    Pipe sorted(100);
    BigQ bigQ(workerArg->inPipe, sorted, workerArg->groupAtts, workerArg->use_n_pages);
    int intRes = 0, intSum = 0;
    double doubleRes = 0.0, doubleSum = 0.0;
    ComparisonEngine cmp;
    Record prev;
    Record cur;
    Type t;
    Attribute DA = {"SUM", t};
    Schema out_sch ("out_sch", 1, &DA);
    bool firstTime = true;
    while(sorted.Remove(&cur)){
        if(!firstTime && cmp.Compare(&cur, &prev, &workerArg->groupAtts)!=0){
            cout<<"==="<<endl;
            Record res;
            char charsRes[100];
            if(t==Int){
                sprintf(charsRes, "%d|", intSum);
            }
            else {
                sprintf(charsRes, "%lf|", doubleSum);
            }
            res.ComposeRecord(&out_sch, charsRes);
            workerArg->outputP.Insert(&res);

            intSum = 0;
            doubleSum = 0.0;
        }
        firstTime = false;
        t = workerArg->computeMe.Apply(cur, intRes, doubleRes);
        if(t==Int){
            intSum += intRes;
        }
        else {
            doubleSum += doubleRes;
        }
        prev.Copy(&cur);
    }
    Record res;
    char charsRes[100];
    if(t==Int){
        sprintf(charsRes, "%d|", intSum);
    }
    else {
        sprintf(charsRes, "%lf|", doubleSum);
    }
    res.ComposeRecord(&out_sch, charsRes);
    workerArg->outputP.Insert(&res);
    workerArg->outputP.ShutDown();
    return NULL;
}
void GroupBy::Run (Pipe &inPipe, Pipe &outputP, OrderMaker &groupAtts, Function &computeMe){
    w4* workerArg = new w4{inPipe, outputP, groupAtts, computeMe, use_n_pages};
    pthread_create(&worker, NULL, wM4, (void*) workerArg);
}
void GroupBy::WaitUntilDone (){
    pthread_join(worker, NULL);
}
void GroupBy::Use_n_Pages (int n){
    use_n_pages = n;
}

void Project::Run (Pipe &inPipe, Pipe &outputP, int *keepMe, int numAttsInput, int numAttsOutput) {
    ProjStruct* projectArg = new ProjStruct;
    projectArg->inPipe = &inPipe;
    projectArg->outputP = &outputP;
    projectArg->keepMe = keepMe;
    projectArg->numAttsInput = numAttsInput;
    projectArg->numAttsOutput = numAttsOutput;
    pthread_create(&workerThread, NULL, ProjectWorker, (void*) projectArg);
}

void* ProjectWorker (void* arg) {
    ProjStruct* projectArg = (ProjStruct*) arg;
    Record record;
    while (projectArg->inPipe->Remove(&record) == 1) {
        Record* tempRecord = new Record;
        tempRecord->Consume(&record);
        tempRecord->Project(projectArg->keepMe, projectArg->numAttsOutput, projectArg->numAttsInput);
        projectArg->outputP->Insert(tempRecord);     
    }
    projectArg->outputP->ShutDown();
    return NULL;
}

void Project::WaitUntilDone() {
    pthread_join(workerThread, NULL);
}

void Project::Use_n_Pages(int n) {

}

void DuplicateRemoval::Run (Pipe &inPipe, Pipe &outputP, Schema &mySchema) { 
    DuplicateRemovalArg* duplicateRemovalArg = new DuplicateRemovalArg;
    duplicateRemovalArg->inPipe = &inPipe;
    duplicateRemovalArg->outputP = &outputP;
    OrderMaker* order = new OrderMaker(&mySchema);
    duplicateRemovalArg->order = order;
    if (this->runLen <= 0)
        duplicateRemovalArg->runLen = 8;
    else
        duplicateRemovalArg->runLen = this->runLen;
    pthread_create(&workerThread, NULL, DuplicateRemovalWorker, (void*) duplicateRemovalArg);
}

void* DuplicateRemovalWorker (void* arg) {
    DuplicateRemovalArg* duplicateRemovalArg = (DuplicateRemovalArg*) arg;
    ComparisonEngine comparisonEngine;
    Record cur, last;
    Pipe* sortedPipe = new Pipe(1000);
    BigQ* bq = new BigQ(*(duplicateRemovalArg->inPipe), *sortedPipe, *(duplicateRemovalArg->order), duplicateRemovalArg->runLen);
    sortedPipe->Remove(&last);
    Schema schema ("catalog", "partsupp");
    while (sortedPipe->Remove(&cur) == 1) {
        if (comparisonEngine.Compare(&last, &cur, duplicateRemovalArg->order) != 0) {
            Record* tempRecord = new Record;
            tempRecord->Consume(&last);
            duplicateRemovalArg->outputP->Insert(tempRecord);
            last.Consume(&cur);
        }
    }
    duplicateRemovalArg->outputP->Insert(&last);
    duplicateRemovalArg->outputP->ShutDown();
    return NULL;
}

void DuplicateRemoval::WaitUntilDone () { 
    pthread_join(workerThread, NULL);
}

void DuplicateRemoval::Use_n_Pages (int n) {
    this->runLen = n;
}

void WriteOut::Run (Pipe &inPipe, FILE *outFile, Schema &mySchema) {
    WriteOutArg* writeOutArg = new WriteOutArg;
    writeOutArg->inPipe = &inPipe;
    writeOutArg->outFile = outFile;
    writeOutArg->schema = &mySchema;
    pthread_create(&workerThread, NULL, WriteOutWorker, (void*) writeOutArg);
}

void* WriteOutWorker (void* arg) {
    WriteOutArg* writeOutArg = (WriteOutArg*) arg;
    Record cur;
    while (writeOutArg->inPipe->Remove(&cur) == 1) {
        int numOfAtts = writeOutArg->schema->GetNumAtts();
        Attribute *attribute = writeOutArg->schema->GetAtts();
        for (int i = 0; i < numOfAtts; i++) {
            fprintf(writeOutArg->outFile, "%s:", attribute[i].name);
            int pointer = ((int *) cur.bits)[i + 1];
            fprintf(writeOutArg->outFile, "[");
            if (attribute[i].myType == Int) {
                int *writeOutInt = (int*) &(cur.bits[pointer]);
                fprintf(writeOutArg->outFile, "%d", *writeOutInt);
            }
            else if (attribute[i].myType == Double) {
                double *writeOutDouble = (double*) &(cur.bits[pointer]);
                fprintf(writeOutArg->outFile, "%f", *writeOutDouble);
            }
            else if (attribute[i].myType == String) {
                char* writeOutString = (char*) &(cur.bits[pointer]);
                fprintf(writeOutArg->outFile, "%s", writeOutString);
            }
            fprintf(writeOutArg->outFile, "]");
            if (i != numOfAtts - 1)
                fprintf(writeOutArg->outFile, ", ");
        }
        fprintf(writeOutArg->outFile, "\n");
    }
    return NULL;
}

void WriteOut::WaitUntilDone () {
    pthread_join(workerThread, NULL);
}

void WriteOut::Use_n_Pages (int n) { 

}

void Join::Run (Pipe &inPipeL, Pipe &inPipeR, Pipe &outputP, CNF &cnfOp, Record &literal) {
    JoinArg* joinArg = new JoinArg;
    joinArg->inPipeL = &inPipeL;
    joinArg->inPipeR = &inPipeR;
    joinArg->outputP = &outputP;
    joinArg->cnfOp = &cnfOp;
    joinArg->literal = &literal;
    if (this->runLen <= 0)
        joinArg->runLen = 8;
    else
        joinArg->runLen = this->runLen;
    pthread_create(&workerThread, NULL, JoinWorker, (void*) joinArg);
}

void* JoinWorker(void* arg) {
    JoinArg* joinArg = (JoinArg*) arg;
    OrderMaker leftOrder, rightOrder;
    joinArg->cnfOp->GetSortOrders(leftOrder, rightOrder);
    if (leftOrder.numAtts > 0 && rightOrder.numAtts > 0) {
        JoinWorker_Merge(joinArg, &leftOrder, & rightOrder);
    }
    else {
        cout << "BlockNestJoin" << endl;
        JoinWorker_BlockNested(joinArg);
    }
    joinArg->outputP->ShutDown();
    return NULL;
}

void JoinWorker_AddMergedRecord(Record* leftRecord, Record* rightRecord, Pipe* pipe) {
    int numOfAttsLeft = ((((int*) leftRecord->bits)[1]) / sizeof(int)) - 1;
    int numOfAttsRight = ((((int*) rightRecord->bits)[1]) / sizeof(int)) - 1;
    int* attsToKeep = new int[numOfAttsLeft + numOfAttsRight];
    for (int i = 0; i < numOfAttsLeft; i++)
        attsToKeep[i] = i;
    for (int i = numOfAttsLeft; i < numOfAttsLeft + numOfAttsRight; i++)
        attsToKeep[i] = i - numOfAttsLeft;
    Record mergedRecord;
    mergedRecord.MergeRecords(leftRecord, rightRecord, numOfAttsLeft, numOfAttsRight, attsToKeep, numOfAttsLeft + numOfAttsRight, numOfAttsLeft);
    pipe->Insert(&mergedRecord);
}

void JoinWorker_Merge(JoinArg* joinArg, OrderMaker* leftOrder, OrderMaker* rightOrder) {
    Pipe* sortedLeftPipe = new Pipe(1000);
    Pipe* sortedRightPipe = new Pipe(1000);
    BigQ* tempL = new BigQ(*(joinArg->inPipeL), *sortedLeftPipe, *leftOrder, joinArg->runLen);
    BigQ* tempR = new BigQ(*(joinArg->inPipeR), *sortedRightPipe, *rightOrder, joinArg->runLen);
    Record leftRecord;
    Record rightRecord;
    bool isFinish = false;
    if (sortedLeftPipe->Remove(&leftRecord) == 0)
        isFinish = true;
    if (sortedRightPipe->Remove(&rightRecord) == 0)
        isFinish = true;
    ComparisonEngine comparisonEngine;
    while (!isFinish) {
        int compareRes = comparisonEngine.Compare(&leftRecord, leftOrder, &rightRecord, rightOrder);
        if (compareRes == 0) {
            vector<Record*> vl;
            vector<Record*> vr;
            while (true) {
                Record* oldLeftRecord = new Record;
                oldLeftRecord->Consume(&leftRecord);
                vl.push_back(oldLeftRecord);
                if (sortedLeftPipe->Remove(&leftRecord) == 0) {
                    isFinish = true;
                    break;
                }
                if (comparisonEngine.Compare(&leftRecord, oldLeftRecord, leftOrder) != 0) {
                    break;
                }
            }
            while (true) {
                Record* oldRightRecord = new Record;
                oldRightRecord->Consume(&rightRecord);
                vr.push_back(oldRightRecord);
                if (sortedRightPipe->Remove(&rightRecord) == 0) {
                    isFinish = true;
                    break;
                }
                if (comparisonEngine.Compare(&rightRecord, oldRightRecord, rightOrder) != 0) {
                    break;
                }
            }
            for (int i = 0; i < vl.size(); i++) {
                for (int j = 0; j < vr.size(); j++) {
                    JoinWorker_AddMergedRecord(vl[i], vr[j], joinArg->outputP);
                }
            }
            vl.clear();
            vr.clear();
        }
        else if (compareRes > 0) {
            if (sortedRightPipe->Remove(&rightRecord) == 0)
                isFinish = true;
        }
        else {
            if (sortedLeftPipe->Remove(&leftRecord) == 0)
                isFinish = true;
        }
    }
    while (sortedLeftPipe->Remove(&leftRecord) == 1);
    while (sortedRightPipe->Remove(&rightRecord) == 1);
}

void JoinWorker_BlockNested(JoinArg* joinArg) {
    DBFile tempFile;
    char* fileName = new char[100];
    sprintf(fileName, "BlockNestedTemp%d.bin", pthread_self());
    tempFile.Create(fileName, heap, NULL);
    tempFile.Open(fileName);
    Record record;
    while (joinArg->inPipeL->Remove(&record) == 1)
        tempFile.Add(record);
    
    Record record1, record2;
    ComparisonEngine comparisonEngine;
    while (joinArg->inPipeR->Remove(&record1) == 1) {
        tempFile.MoveFirst();
        while (tempFile.GetNext(record) == 1) {
            if (comparisonEngine.Compare(&record1, &record2, joinArg->literal, joinArg->cnfOp)) {
                JoinWorker_AddMergedRecord(&record1, &record2, joinArg->outputP);
            }
        }
    }
}

void Join::WaitUntilDone () { 
    pthread_join(workerThread, NULL);
}
    
void Join::Use_n_Pages (int n) { 
    this->runLen = n;
}
