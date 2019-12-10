#pragma once

#include"InputData.hpp"
#include <vector>
//ルートの数は車両の数

class Route {
private:
    vector<int> order;
    int RouteNumber;

public:
	Route();
    void setRouteNumber(int num);
    Route* getRouteByNum(int num);  

};