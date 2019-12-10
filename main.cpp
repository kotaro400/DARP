#include <iostream>
#include "InputData.hpp"
#include "Route.hpp"
#include "RouteList.hpp"
#include "Location.hpp"
#include "Cost.hpp"
#include "myfunction.hpp"
using namespace std;

int main(int argc, char *argv[]){
    int i,j;
    
    // 入力を受け取り
    InputData inputdata;
    string inputfile = argv[1];
    inputdata.setInputData(inputfile = argv[1]);
    cout << "車両数:" << inputdata.getVehicleNum() << endl;
    cout << "リクエスト数:" << inputdata.getRequestSize() << endl;
    cout << "ルートの最大長さ:" << inputdata.getMaximumRouteDuration() << endl;
    cout << "車両の容量:" << inputdata.getVehicleCapacity() << endl;

    // 2店間の距離と時間を計算
    Location* loc1;
    Location* loc2;
    for (i=0;i<=inputdata.getRequestSize();i++){
        for (j=0;j<=inputdata.getRequestSize();j++){
            // コストを計算
            loc1 = inputdata.getLocationPointer(i);
            loc2 = inputdata.getLocationPointer(j);
        }
    }
    Location* test = inputdata.getLocationPointer(2);
    cout << test->getLat() << endl;
    Cost cost(53);



    // vector<Route> RouteList;
    // RouteList.reserve((inputdata.getVehicleNum()));
    // RouteList.resize((inputdata.getVehicleNum()));
    // for(i=0;i<inputdata.getVehicleNum();i++) {
    //     RouteList[i].setRouteNumber(i);
    // }
    RouteList RouteList(inputdata.getVehicleNum());
    RouteList.makeInitialRoute(inputdata.getRequestSize());



  
}
