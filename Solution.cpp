#include "Solution.hpp"
using namespace std;
#include <iostream>

Solution::Solution(RouteList* routelist){
    cout << "solutionのコンストラクタ" << endl;
    this->routelist = routelist;
}

int Solution::test() {
    cout << "solution::test" << endl;
    cout << this->routelist->getRouteSize(2) << endl;
    return 1;
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