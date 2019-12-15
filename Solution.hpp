#pragma once

#include <vector>
#include "RouteList.hpp"

class Solution {
private:
    RouteList* routelist;
    vector<vector<double> > timelist;
    // routelistから、LPを解いてtimelistを作成っていう流れかなあ
    double RouteDistance;
public:
    Solution(RouteList* routelist);
    int test();
    double CalcRouteDistance(RouteList* routelist);
};