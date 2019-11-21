#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "InputData.h"

using namespace std;
void InputData::setInputData(string inputfile){
    string str;
    cout << inputfile << endl;
    ifstream file(inputfile);
    if (!file) {
		cerr << "ERROR: could not open inputfile." << endl;
		exit(1);
	}
    while(getline(file,str)){
        cout << str << endl;
    }
}