#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "InputData.hpp"
#include "myfunction.hpp"
#include "Location.hpp"

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
    this->RequestSize = temp_int_v[1];
    this->MaximumRouteDuration = temp_int_v[2];
    this->VehicleCapacity = temp_int_v[3];
    temp_int_v.clear();

    this->locationList.reserve(this->RequestSize);
	this->locationList.resize(this->RequestSize);
    this->RequestList.reserve(this->RequestSize);
    this->RequestList.resize(this->RequestSize);

    
    // firstdepot
    getline(file,line);
    temp_string_v = splitstring(line,',');
    this->locationList[0].setLocation(stoi(temp_string_v[0]),stod(temp_string_v[1]),stod(temp_string_v[2]));
    temp_string_v.clear();
    

    
    for(int i=1;i <= this->RequestSize; i++){
        getline(file,line);
        temp_string_v = splitstring(line,',');
        this->locationList[i].setLocation(stoi(temp_string_v[0]),stod(temp_string_v[1]),stod(temp_string_v[2]));
        temp_string_v.clear();
    }
    for(int i=1;i<=this->RequestSize;i++){
        cout << this->locationList[i].getLat() << endl;
    }

}



int InputData::getVehicleNum() {
    return VehicleNum;
}
int InputData::getRequestSize() {
    return RequestSize;
}
int InputData::getMaximumRouteDuration() {
    return MaximumRouteDuration;
}
int InputData::getVehicleCapacity() {
    return VehicleCapacity;
}