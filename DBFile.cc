#include <iostream>
#include <ostream>
#include <string>
#include <cstring>
#include <fstream>

#include "DBFile.h"
#include "Heap.h"
#include "Sorted.h"

using namespace std;

DBFile::DBFile() {
    file = nullptr;
}

DBFile::~DBFile() {
    delete file;
}

int DBFile::Create(const char *filePath, typeOfFile type, void *startUp) {
    if (filePath == nullptr || type < heap || type > tree) return 0;

    if (type == heap) {
        file = new Heap();
    }
    else if (type == sorted) {
        file = new Sorted();
    }
    else {
        cout << "Invalid file type!" << endl;
        exit(1);
    }

    return file->Create(filePath, type, startUp);
}

int DBFile::Open(const char *filePath) {
    if (filePath == nullptr) return 0;

    // Get file-type info from meta file.
    std::ifstream metaFile;
    string metaFilePath(filePath);
     //cout << metaFilePath << " is being opened" << '\n';
    metaFilePath.append(".metadata");

    metaFile.open(metaFilePath.c_str());

    if (metaFile.is_open()) {
        string readLine;
        getline(metaFile, readLine);

        if (strcmp(readLine.c_str(), FILE_TYPE_HEAP) == 0) {
            file = new Heap();
        }
        else if (strcmp(readLine.c_str(), FILE_TYPE_SORTED) == 0) {
            getline(metaFile, readLine);
            int runLen = stoi(readLine);

            getline(metaFile, readLine);
            auto *orderMaker = new OrderMaker();
            orderMaker->FromString(readLine);

            file = new Sorted(orderMaker, runLen);
        }
        else {
            cout << "Invalid file type value \"" << readLine << "\" stored in metadata file." << endl;
            exit(1);
        }

        metaFile.close();
    }
    else {
        cout << "ERROR - DBFILE : Unable to create metadata file " << metaFilePath << endl;
        return 0;
    }

    return file->Open(filePath);
}

int DBFile::Close() {
    return file->Close();
}

void DBFile::Load(Schema &schema, const char *loadPath) {
    file->Load(schema, loadPath);
}

void DBFile::MoveFirst() {
    file->MoveFirst();
}

void DBFile::Add(Record &addMe) {
    file->Add(addMe);
}

int DBFile::GetNext(Record &fetchMe) {
    return file->GetNext(fetchMe);
}

int DBFile::GetNext(Record &fetchMe, CNF &cnf, Record &literal) {
    return file->GetNext(fetchMe, cnf, literal);
}