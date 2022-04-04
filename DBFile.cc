#include <iostream>
#include <ostream>
#include <string>
#include <cstring>
#include <fstream>

#include "DBFile.h"
#include "Heap.h"
#include "Sorted.h"

using namespace std;

DBFile::DBFile()
{
    file = nullptr;
}

DBFile::~DBFile()
{
    delete file;
}

int DBFile::Create(const char *filePath, typeOfFile type, void *startUp)
{
    // cout << "DBFILE: Create" << endl;
    if (filePath == nullptr || type < heap || type > tree)
        return 0;

    if (type == heap)
    {
        file = new Heap();
    }
    else if (type == sorted)
    {
        file = new Sorted();
    }
    else
    {
        cout << "File type! not valid" << endl;
        exit(1);
    }

    return file->Create(filePath, type, startUp);
}

int DBFile::Open(const char *filePath)
{
    if (filePath == nullptr)
        return 0;

    std::ifstream metadataFile;
    string metadataFilePath(filePath);
    metadataFilePath.append(".metadata");

    metadataFile.open(metadataFilePath.c_str());

    if (metadataFile.is_open())
    {
        string readLine;
        getline(metadataFile, readLine);

        if (strcmp(readLine.c_str(), FILE_TYPE_HEAP) == 0)
        {
            file = new Heap();
        }
        else if (strcmp(readLine.c_str(), FILE_TYPE_SORTED) == 0)
        {
            getline(metadataFile, readLine);
            int runLen = stoi(readLine);

            getline(metadataFile, readLine);
            auto *orderMaker = new OrderMaker();
            orderMaker->FromString(readLine);

            file = new Sorted(orderMaker, runLen);
        }
        else
        {
            cout << "File type value invalid \"" << readLine << "\" in metadata file." << endl;
            exit(1);
        }

        metadataFile.close();
    }
    else
    {
        // cout << "DBFILE : Not able to create the metadata file " << metadataFilePath << endl;
        return 0;
    }

    return file->Open(filePath);
}

int DBFile::Close()
{
    return file->Close();
}

void DBFile::Load(Schema &schema, const char *loadPath)
{
    file->Load(schema, loadPath);
}

void DBFile::MoveFirst()
{
    file->MoveFirst();
}

void DBFile::Add(Record &addMe)
{
    file->Add(addMe);
}

int DBFile::GetNext(Record &fetchMe)
{
    return file->GetNext(fetchMe);
}

int DBFile::GetNext(Record &fetchMe, CNF &cnf, Record &literal)
{
    return file->GetNext(fetchMe, cnf, literal);
}