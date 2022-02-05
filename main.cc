#include <iostream>
#include "Record.h"
#include <stdlib.h>
using namespace std;

extern "C" {
	int yyparse(void);   // defined in y.tab.c
}

extern struct AndList *final;

int main () {

	// try to parse the CNF
	cout << "Enter in your CNF: ";
  	if (yyparse() != 0) {
		cout << "Can't parse your CNF.\n";
		exit (1);
	}

	// suck up the schema from the file
	Schema lineitem ("catalog", "lineitem");

	// grow the CNF expression from the parse tree 
	CNF myComparison;
	Record literal;
	myComparison.GrowFromParseTree (final, &lineitem, literal);
	
	// print out the comparison to the screen
	myComparison.Print ();

	// now open up the text file and start procesing it
	FILE *tableFile = fopen ("/cise/homes/karanasthana/git/dbi-project-impl/data-files/lineitem.tbl", "r");
	FILE *tempFileToWrite = fopen ("/cise/homes/karanasthana/git/dbi-project-impl/data-files/temp_page_test.tbl", "w");
	
	Record temp;
	Record temp2;

	Schema mySchema ("catalog", "lineitem");

	Page p2;

	//char *bits = literal.GetBits ();
	//cout << " numbytes in rec " << ((int *) bits)[0] << endl;
	//literal.Print (&supplier);

        // read in all of the records from the text file and see if they match
	// the CNF expression that was typed in
	int counter = 0;
	int resultAppend = 0;
	ComparisonEngine comp;
        while (temp.SuckNextRecord (&mySchema, tableFile) == 1) {
		 	counter++;
		 	if (counter % 10000 == 0) {
		 		cerr << counter << "\n";
		 	}
			temp2 = temp;
			resultAppend = p2.Append(&temp);
			if (resultAppend == 0) {
				cout << "Error!!! out of page memory" <<endl;
				break;
			}
			string ss = temp2.PrintString (&mySchema);
			cout << ss;
			int n2 = ss.length();
	
			char char_array[n2 + 1];
			// copying the contents of the
			// string to char array
			strcpy(char_array, ss.c_str());
			fprintf (tempFileToWrite, char_array);
			cout << "Wrote to the file";
	//	 	if (comp.Compare (&temp, &literal, &myComparison))
		 	//			temp.Print (&mySchema);

		}
		fclose (tempFileToWrite);

		int appendResult = 0;
		counter = 0;
	//	while (temp2.SuckNextRecord (&mySchema, tableFile) == 1) {
	//		counter++;
	//		if (counter % 10000 == 0) {
	//			cerr << counter << "\n";
	//		}
	//		appendResult = p2.Append(&temp2);
	//		cout << "Just after append result with result as : %d", appendResult;
	//		if (appendResult == 0) {
	//			cout << "Page Size complete!";
	//			break;
	//		}
	//		//tempFileToWrite.
	//		cout << "Appending complete";
	//		temp2.Print (&mySchema);
	//		fprintf (tempFileToWrite, "%s", counter);
	//		//cout<<counter<<endl;
	//	}

}