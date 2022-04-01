#include <iostream>
#include <ostream>
#include <string>
#include <fstream>

#include "Heap.h"

using namespace std;

void Heap::WriteToFile()
{
    file->AddPage(write, wIndex++);
    write->EmptyItOut();
}

Heap::Heap()
{
    file = new File();

    read = new Page();
    write = new Page();

    rIndex = wIndex = 0;

    comp = new ComparisonEngine();
}

Heap::~Heap()
{
    delete file;
    delete read;
    delete write;
    delete comp;
}

int Heap::Create(const char *fp, typeOfFile type, void *startUp)
{
    rIndex = wIndex = 0;

    this->filePath = string(fp);

    file->Open(0, fp);

    return 1;
}

int Heap::Open(const char *fp)
{
    file->Open(1, fp);
    return 1;
}

int Heap::Close()
{
    if (write->GetNumberOfRecs() > 0)
        WriteToFile();

    string metadataFilePath(filePath);
    metadataFilePath.append(".metadata");
    ofstream metadataFile;
    metadataFile.open(metadataFilePath.c_str());
    if (!metadataFile.is_open())
    {
        cout << "HEAP : Not able to create meta data file " << metadataFilePath << endl;
        exit(1);
    }
    metadataFile << FILE_TYPE_HEAP;
    metadataFile.close();

    return file->Close();
}

void Heap::CloseFile()
{
    if (write->GetNumberOfRecs() > 0)
        WriteToFile();

    file->Close();
}

void Heap::Load(Schema &schema, const char *loadPath)
{
    int count = 0;

    FILE *tblFile = fopen(loadPath, "r");
    if (tblFile != nullptr)
    {
        Record next;
        while (next.SuckNextRecord(&schema, tblFile))
        {
            Add(next);
            count++;
        }
        fclose(tblFile);
    }
    else
    {
        cout << "Not able to open file " << tblFile << endl;
        exit(1);
    }

    cout << "Loaded " << count << " records from " << loadPath << endl;
}

void Heap::MoveFirst()
{
    rIndex = 0;
    file->GetPage(read, rIndex++);
}

void Heap::Add(Record &addMe)
{

    if (!write->Append(&addMe))
    {
        WriteToFile();
        write->Append(&addMe);
    }
}

int Heap::GetNext(Record &fetchMe)
{

    if (!read->GetFirst(&fetchMe))
    {
        if (rIndex < file->GetLength() - 1)
        {
            file->GetPage(read, rIndex++);
            read->GetFirst(&fetchMe);
        }
        else
            return 0;
    }
    return 1;
}

int Heap::GetNext(Record &fetchMe, CNF &cnf, Record &literal)
{

    while (true)
    {
        int hasRecord = GetNext(fetchMe);
        if (!hasRecord)
            return 0;

        if (comp->Compare(&fetchMe, &literal, &cnf))
        {
            return 1;
        }
    }
}