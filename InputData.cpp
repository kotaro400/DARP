#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "InputData.hpp"
#include "myfunction.hpp"

using namespace std;

void InputData::setInputData(string inputfile){
    int i;
    vector<int> temp_int_v;
    vector<double> temp_double_v;
    vector<string> temp_string_v;

    cout << "file name:" << inputfile << endl;

    ifstream file(inputfile);
    if (!file) {
		cerr << "ERROR: could not open inputfile." << endl;
		exit(1);
	}
    string line;
    // 車両数、利用者数、ルートの最大時間、容量を取得
    getline(file,line);
    temp_int_v = splitint(line, ',');
    this->VehicleNum = temp_int_v[0];
    this->UserSize = temp_int_v[1];
    this->MaximumRouteDuration = temp_int_v[2];
    this->VehicleCapacity = temp_int_v[3];
    temp_int_v.clear();

    this->LocationList.reserve(2 * this->UserSize);
	this->LocationList.resize(2 * this->UserSize);
    this->RequestList.reserve(2 * this->UserSize);
    this->RequestList.resize(2 * this->UserSize);


    
    // firstdepot
    getline(file,line);
    cout << "first depot" << endl;

    // requests (pickup)
    cout << "pickup" << endl;
    for(int i=0;i < this->UserSize; i++){
        getline(file,line);
        temp_string_v = splitstring(line,',');
        cout << line << endl;
        temp_string_v.clear();
    }
    // requests (dropoff)
    cout << "dropoff" << endl;
    for(int i=0; i < this->UserSize;i++){
        getline(file,line);
        temp_string_v = splitstring(line,',');
        cout << line << endl;
        temp_string_v.clear();
    }

    // lastdepot
    getline(file,line);

    cout << "last depot" << endl;
    cout << line << endl;

}



int InputData::getVehicleNum() {
    return VehicleNum;
}
int InputData::getUserSize() {
    return UserSize;
}
int InputData::getMaximumRouteDuration() {
    return MaximumRouteDuration;
}
int InputData::getVehicleCapacity() {
    return VehicleCapacity;
}