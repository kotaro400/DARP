#include <iostream>
#include "InputData.hpp"
#include "Route.hpp"
using namespace std;

int main(int argc, char *argv[]){
    InputData inputdata;
    string inputfile = argv[1];
    inputdata.setInputData(inputfile = argv[1]);
    cout << "----" << endl;
    cout << inputdata.getVehicleNum() << endl;
    cout << inputdata.getRequestSize() << endl;
    cout << inputdata.getMaximumRouteDuration() << endl;
    cout << inputdata.getVehicleCapacity() << endl;

    vector<Route> RouteList(inputdata.getVehicleNum());
  
}
