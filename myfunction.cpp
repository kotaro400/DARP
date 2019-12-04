#include<iostream>
#include<vector>
#include<string>
#include <sstream>
using namespace std;

#include "myfunction.hpp"

vector<int> splitint(string input, char delimiter){
    int first = 0;
    int last = input.find_first_of(delimiter);
    vector<int> result;
    while(first < input.size()){
        string subStr(input,first,last-first);
        result.push_back(stoi(subStr));
        first = last+1;
        last = input.find_first_of(delimiter,first);

        if (last==string::npos){
            last = input.size();
        }
        
    }
    return result;
}
vector<string> splitstring(string input, char delimiter){
    int first = 0;
    int last = input.find_first_of(delimiter);
    vector<string> result;
    while(first < input.size()){
        string subStr(input,first,last-first);
        result.push_back(subStr);
        first = last+1;
        last = input.find_first_of(delimiter,first);

        if (last==string::npos){
            last = input.size();
        }
        
    }
    return result;
}

vector<double> splitdouble(string input, char delimiter){
    istringstream stream(input);
    double temp;
	string field;
	vector<double> result;

	while (getline(stream, field, delimiter))
	{
		try {
			temp = stod(field);
		}
		catch (const std::invalid_argument& e) {
			// std::cout << e.what(); // "stod: no conversion"
			std::cout << "Error: cannot convert to double.";
		}
		result.push_back(temp);
	}
    return result;
}