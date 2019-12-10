#pragma once

#include"InputData.hpp"
#include <vector>
//ルートの数は車両の数

class RouteList {
private:
    int VehicleNum;
    vector<vector<int> > Route;

public:
	RouteList(int VehicleNum);
    void makeInitialRoute(int RequestSize);
};