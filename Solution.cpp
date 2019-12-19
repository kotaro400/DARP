#include "Solution.hpp"
using namespace std;
#include <iostream>

Solution::Solution(int CustomerSize,int VehicleNum){
    cout << "solutionのコンストラクタ" << endl;
}
double Solution::CalcRouteDistance(RouteList* routelist){
    double total=0;
    for(int i=0;i<this->routelist->getVehicleNum();i++){
        for (int j=0;j<this->routelist->getRouteSize(i);j++){
            cout << this->routelist->getRoute(i,j) << endl;
        }
        cout << "-----" << endl;
    }
    return 0.0;
}