#include "Schema.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>

int Schema :: Find (const char *attName) {
	cout << attName << " AND THE NUMATTS ARE - " << numAtts << '\n';
	for (int i = 0; i < numAtts; i++) {
		cout << myAtts[i].name << '\n';
		if (!strcmp (attName, myAtts[i].name)) {
			cout << '\n' << '\n';
			return i;
		}
	}

	cout << '\n' << '\n';
	// if we made it here, the attribute was not found
	return -1;
}

Type Schema :: FindType (const char *attName) {

	for (int i = 0; i < numAtts; i++) {
		if (!strcmp (attName, myAtts[i].name)) {
			return myAtts[i].myType;
		}
	}

	// if we made it here, the attribute was not found
	return Int;
}

int Schema :: GetNumAtts () {
	return numAtts;
}

Attribute *Schema :: GetAtts () {
	return myAtts;
}

Schema :: Schema (const char *fName, int numAttrs, Attribute *attrs) {
    // //this->fileName = fileName;
	// myAtts = new Attribute[numAtts];
    // for (int i = 0; i < numAttrs; i++) {
    //     //Attribute attr;
    //     myAtts[i].name = attrs[i].name;
    //     myAtts[i].myType = attrs[i].myType;
    //     //this->attrs.push_back(attr);
    // }
	// cout<<"Schema.cc line 51" << fName << '\n';
	this->fileName = fName;
	cout<<"Schema.cc line 53" << this->fileName << " and numAtts is - " << numAtts << " and numAttrs is - " << numAttrs << '\n';
	myAtts = new Attribute[numAttrs];
    for (int i = 0; i < numAttrs; i++) {
		// cout<<"Schema.cc line 55 for i = " << i << '\n';
        Attribute attr;
		cout<<"Schema.cc line 57 for i = " << i << " name->" << attrs[i].name << " type->" << attrs[i].myType << '\n';
        attr.name = attrs[i].name;
		// cout<<"Schema.cc line 59 for i = " << i << '\n';
        attr.myType = attrs[i].myType;
		// cout<<"Schema.cc line 61 for i = " << i << '\n';
        myAtts[i] = attr;
		// cout<<"Schema.cc line 63 for i = " << i << '\n';
    }
}

Schema :: Schema (const char *fName, const char *relName) {

	FILE *foo = fopen (fName, "r");
	
	// this is enough space to hold any tokens
	char space[200];

	fscanf (foo, "%s", space);
	int totscans = 1;

	// see if the file starts with the correct keyword
	if (strcmp (space, "BEGIN")) {
		cout << "Unfortunately, this does not seem to be a schema file.\n";
		exit (1);
	}	
		
	while (1) {

		// check to see if this is the one we want
		fscanf (foo, "%s", space);
		totscans++;
		if (strcmp (space, relName)) {

			// it is not, so suck up everything to past the BEGIN
			while (1) {

				// suck up another token
				if (fscanf (foo, "%s", space) == EOF) {
					cerr << "Could not find the schema for the specified relation.\n";
					exit (1);
				}

				totscans++;
				if (!strcmp (space, "BEGIN")) {
					break;
				}
			}

		// otherwise, got the correct file!!
		} else {
			break;
		}
	}

	// suck in the file name
	fscanf (foo, "%s", space);
	totscans++;
	fileName = strdup (space);

	// count the number of attributes specified
	numAtts = 0;
	while (1) {
		fscanf (foo, "%s", space);
		if (!strcmp (space, "END")) {
			break;		
		} else {
			fscanf (foo, "%s", space);
			numAtts++;
		}
	}

	// now actually load up the schema
	fclose (foo);
	foo = fopen (fName, "r");

	// go past any un-needed info
	for (int i = 0; i < totscans; i++) {
		fscanf (foo, "%s", space);
	}

	// and load up the schema
	myAtts = new Attribute[numAtts];
	for (int i = 0; i < numAtts; i++ ) {

		// read in the attribute name
		fscanf (foo, "%s", space);	
		myAtts[i].name = strdup (space);

		// read in the attribute type
		fscanf (foo, "%s", space);
		if (!strcmp (space, "Int")) {
			myAtts[i].myType = Int;
		} else if (!strcmp (space, "Double")) {
			myAtts[i].myType = Double;
		} else if (!strcmp (space, "String")) {
			myAtts[i].myType = String;
		} else {
			cout << "Bad attribute type for " << myAtts[i].name << "\n";
			exit (1);
		}
	}

	fclose (foo);
}

Schema :: ~Schema () {
	delete [] myAtts;
	myAtts = 0;
}

