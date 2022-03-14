#include <iostream>
#include <ostream>
#include <string>
#include <fstream>

#include "Heap.h"

using namespace std;

void Heap::WriteToFile() {
    file->AddPage(write, wIndex++);
    write->EmptyItOut();
}

Heap::Heap() {
    file = new File();

    read = new Page();
    write = new Page();

    rIndex = wIndex = 0;

    comparisonEngine = new ComparisonEngine();
}

Heap::~Heap() {
    delete file;
    delete read;
    delete write;
    delete comparisonEngine;
}

int Heap::Create(const char *filePath, typeOfFile type, void *startUp) {
    // Reset read and write pointers.
    rIndex = wIndex = 0;

    this->fileLocation = string(filePath);

    // Create bin file.
    file->Open(0, filePath);

    return 1;
}

int Heap::Open(const char *filePath) {
    // Opens bin file.
    file->Open(1, filePath);
    return 1;
}

int Heap::Close() {
    // Write off the last page.
    if (write->GetNumberOfRecs() > 0) WriteToFile();

    // Create meta file, and store the file-type into it.
    string metaFilePath(fileLocation);
    metaFilePath.append(".metadata");
    ofstream metaFile;
    metaFile.open(metaFilePath.c_str());
    if (!metaFile.is_open()) {
        cout << "ERROR - HEAP : Unable to create meta data file " << metaFilePath << endl;
        exit(1);
    }
    metaFile << FILE_TYPE_HEAP;
    metaFile.close();

    return file->Close();
}

void Heap::CloseFile() {
    // Write off the last page.
    if (write->GetNumberOfRecs() > 0) WriteToFile();

    file->Close();
}

void Heap::Load(Schema &schema, const char *loadPath) {
    int count = 0;

    FILE *tblFile = fopen(loadPath, "r");
    if (tblFile != nullptr) {
        // Read all the records from the tbl file and add it to our page object one by one.
        Record next;
        while (next.SuckNextRecord(&schema, tblFile)) {
            Add(next);
            count++;
        }
        fclose(tblFile);
    }
    else {
        cout << "Unable to open file " << tblFile << endl;
        exit(1);
    }

    cout << "Loaded " << count << " records from " << loadPath << endl;
}

void Heap::MoveFirst() {
    rIndex = 0;
    file->GetPage(read, rIndex++);
}

void Heap::Add(Record &addMe) {
    /*
     * Check if there's enough space left on this page.
     * If not, write this page to the file, empty the page and add this record to it, Else just add the record.
     */
    if (!write->Append(&addMe)) {
        WriteToFile();
        write->Append(&addMe);
    }
}

// Gets the next record from the file.
int Heap::GetNext(Record &fetchMe) {
    /*
     * Check if there are any records left to be read on the current read, and just update the
     * fetchMe with the value of next record from this read.
     * If not, update read to point to the next page in the file, and then update the value
     * of fetchMe for this new read.
     *
     * Return 0 if reached the end of the file, else return 1.
     */
    if (!read->GetFirst(&fetchMe)) {
        if (rIndex < file->GetLength() - 1) {
            file->GetPage(read, rIndex++);
            read->GetFirst(&fetchMe);
        }
        else
            return 0;
    }
    return 1;
}

int Heap::GetNext(Record &fetchMe, CNF &cnf, Record &literal) {
    /*
     * Infinitely look for the next record satisfying the given CNF until we find one.
     * In each iteration of the while loop there are 3 possibilities.
     * 1. Reached to the end of the file - return 0.
     * 2. Found the record matching CNF - return 1.
     * 3. Record does not satisfy the CNF - move to next record.
     */
    while (true) {
        int hasRecord = GetNext(fetchMe);
        if (!hasRecord) return 0;

        if (comparisonEngine->Compare(&fetchMe, &literal, &cnf)) {
            return 1;
        }
    }
}