#include <iostream>
#include "InputData.hpp"
using namespace std;

int main(int argc, char *argv[]){
    InputData inputdata;
    string inputfile = argv[1];
    inputdata.setInputData(inputfile = argv[1]);
    cout << inputdata.getVehicleNum() << endl;
    cout << inputdata.getUserSize() << endl;
    cout << inputdata.getMaximumRouteDuration() << endl;
    cout << inputdata.getVehicleCapacity() << endl;
  
}
