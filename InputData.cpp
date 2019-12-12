#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "InputData.hpp"
#include "myfunction.hpp"
#include "Location.hpp"
#include "Request.hpp"

using namespace std;

void InputData::setInputData(string inputfile){
    int i,nodetype;
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
    this->PickupList.reserve(this->RequestSize/2+1);
    this->PickupList.resize(this->RequestSize/2+1);
    this->DropoffList.reserve(this->RequestSize/2+1);
    this->DropoffList.resize(this->RequestSize/2+1);
    
    // depot
    getline(file,line);
    temp_string_v = splitstring(line,',');
    this->locationList[0].setLocation(stoi(temp_string_v[0]),stod(temp_string_v[1]),stod(temp_string_v[2]));
    temp_string_v.clear();
    getline(file,line); //デポのペナルティ関数だから無視

    
    for(int i=1;i <= this->RequestSize; i++){
        getline(file,line);
        temp_string_v = splitstring(line,',');
        this->locationList[i].setLocation(stoi(temp_string_v[0]),stod(temp_string_v[1]),stod(temp_string_v[2]));
        if (i<= RequestSize/2){
            this->PickupList[i].setPickup(stod(temp_string_v[0]),stod(temp_string_v[3]),stod(temp_string_v[4]),stoi(temp_string_v[5]),stoi(temp_string_v[6]));
            getline(file,line);
            temp_double_v = splitdouble(line,',');
            this->PickupList[i].setPickPenalty(temp_double_v);
        }else{
            this->DropoffList[i - this->RequestSize/2].setDropoff(stod(temp_string_v[0]),stod(temp_string_v[3]),stod(temp_string_v[4]),stoi(temp_string_v[5]),stoi(temp_string_v[6]));
            getline(file,line);
            temp_double_v = splitdouble(line,',');
            this->DropoffList[i - this->RequestSize/2].setDropPenalty(temp_double_v);
        }
        temp_string_v.clear();
        temp_double_v.clear();
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

Location* InputData::getLocationPointer(int index){
    return &this->locationList[index];
}

