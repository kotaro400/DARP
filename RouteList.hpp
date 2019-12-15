#pragma once

#include"InputData.hpp"
#include <vector>
//ルートの数は車両の数

class RouteList {
private:
    int VehicleNum;
    vector<vector<int> > Routelist;

public:
	RouteList(int VehicleNum);
    void makeInitialRoute(int RequestSize);
    int getRouteSize(int number);
};