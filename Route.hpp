#pragma once

#include"InputData.hpp"
#include <vector>
//ルートの数は車両の数

class Route {
private:
    vector<int> order;
    int RouteNumber;
	int number_of_customers; //ルートで実行されるユーザの人数 == orderの長さの半分

public:
	Route();

};