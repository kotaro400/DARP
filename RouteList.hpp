#pragma once

#include"InputData.hpp"
#include <vector>
//ルートの数は車両の数

class RouteList {
private:
    int VehicleNum;
    vector<vector<int> > Routelist;

public:
    int getVehicleNum();
	RouteList(int VehicleNum);
    ~RouteList();
    void makeInitialRoute(int RequestSize);
    int getRouteSize(int number);
    vector<int>* getRoutePointerByIndex(int index);
    int getRouteListSize();
    int getRoute(int RouteNumber,int RouteOrder);
    void InnerRouteChange_requestSet();
};