#include <iostream>
#include "InputData.hpp"
#include "Route.hpp"
#include "RouteList.hpp"
#include "Location.hpp"
#include "Cost.hpp"
#include "myfunction.hpp"
#include <gurobi_c++.h>
using namespace std;

int main(int argc, char *argv[]){
    int i,j;
    try{
        GRBEnv env = GRBEnv();
    } catch (GRBException e) {
        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    } catch (...) {
        cout << "Error during optimization" << endl;
    }
    
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
    Cost cost(inputdata.getRequestSize());
    for (i=0;i<=inputdata.getRequestSize();i++){
        for (j=0;j<=inputdata.getRequestSize();j++){
            // コストを計算
            loc1 = inputdata.getLocationPointer(i);
            loc2 = inputdata.getLocationPointer(j);
            cost.setCost(i,loc1->getLat(),loc1->getLng(),j,loc2->getLat(),loc2->getLng());
        }
    }
    // 乗車時間のペナルティ関数をここでいれる
    for(int i=1;i<=inputdata.getRequestSize()/2;i++){
        // cout << "i:" << i << endl;
        // cout << cost.getCost(i,i+inputdata.getRequestSize()/2) << endl;
        vector<double> vec{ 0.0, 0.0, 0.0,cost.getCost(i,i+inputdata.getRequestSize()/2),5,cost.getCost(i,i+inputdata.getRequestSize()/2)*(-5)};
        inputdata.setRideTimePenalty(i-1,vec);
        vec.clear();
    }
    cout << inputdata.getRideInfo(0) << endl;



    // vector<Route> RouteList;
    // RouteList.reserve((inputdata.getVehicleNum()));
    // RouteList.resize((inputdata.getVehicleNum()));
    // for(i=0;i<inputdata.getVehicleNum();i++) {
    //     RouteList[i].setRouteNumber(i);
    // }

    // RouteListクラス:複数のルートをまとめて保持するクラス 
    // Routeクラスよりも初期解生成とか近傍探索がしやすそう
    RouteList RouteList(inputdata.getVehicleNum());
    RouteList.makeInitialRoute(inputdata.getRequestSize());



  
}
