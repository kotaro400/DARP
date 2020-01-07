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
    void InnerRouteChange_node(int customerSize); // 他のルートに挿入 インデックスはランダム
    void OuterRouteChange_random(int customerSize);
    void InnerRouteChange_specified(int customerSize,int worst); //ペナルティの大きいノードを単体で違う場所に挿入
    void InnerOrderChange_requestset(int RouteIndex);
    void OuterRouteChange_specified(int customerSize,int worstRouteIndex);
};